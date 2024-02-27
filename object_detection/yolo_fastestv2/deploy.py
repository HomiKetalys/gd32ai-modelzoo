import argparse
import shutil
import subprocess
import sys
from common_utils.utils import LogSaver
import os

from model import detector
from pytorch2tflite import export
from utils.utils import load_datafile




# D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1/Utilities/windows/stm32ai.exe
# generate
# --name network_1
# -m model_path
# --type tflite
# --compression none
# --verbosity 1
# --workspace
# --ouput
# --allocate-inputs
# --series stm32h7
# --allocate-outputs

def gen_net_codes(stm32ai_exe_path, model_path, name, work_space_path, output_path):
    cmd = (f"{stm32ai_exe_path} "
           f"generate "
           f"--name {name} "
           f"-m {model_path} "
           f"--type tflite "
           f"--compression none "
           f"--verbosity 1 "
           f"--workspace {work_space_path} "
           f"--output {output_path} "
           f"--allocate-inputs "
           f"--series stm32h7 "
           f"--allocate-outputs")
    subprocess.run(['powershell', '-Command', cmd], stdout=subprocess.PIPE)


def copy_stlib(opt, stm32ai_path, stlib_path):
    stlib_src_path = os.path.join(stm32ai_path, "Middlewares", "ST", "AI")
    if opt.series == "f4":
        lib_type = "ARMCortexM4"
    elif opt.series == "h7":
        lib_type = "STM32H7"
    else:
        raise ValueError("Unsupported series")
    lib_src_path = os.path.join(stlib_src_path, "Lib", "MDK", lib_type)
    names = os.listdir(lib_src_path)
    assert len(names) == 1, "Unsupported lib"
    name = names[0]
    lib_src_path = os.path.join(lib_src_path, name)
    lib_dst_path = os.path.join(stlib_path, "Lib", name)
    os.makedirs(os.path.join(stlib_path, "Lib"), exist_ok=True)
    shutil.copy(lib_src_path, lib_dst_path)
    inc_dst_path = os.path.join(stlib_path, "Inc")
    if os.path.exists(inc_dst_path):
        shutil.rmtree(inc_dst_path)
    inc_src_path = os.path.join(stlib_src_path, "Inc")
    shutil.copytree(inc_src_path, inc_dst_path)
    license_src_path = os.path.join(stlib_src_path, "LICENSE.txt")
    license_dst_path = os.path.join(stlib_path, "LICENSE.txt")
    shutil.copy(license_src_path, license_dst_path)


def gen_utils_codes(opt, utils_path):
    with open("../../common_utils/c_codes/utils.c", "r") as f:
        lines = f.readlines()
    cfg = load_datafile(opt.data)
    anchors = cfg["anchors"]
    with open(os.path.join(utils_path, "utils.c"), "w", encoding="utf-8") as f:
        for line in lines:
            if "ANCHOR_CODE" in line:
                anchors_txt = str(anchors)[1:-1]
                line = "float anchors[12]={" + anchors_txt + "};\n"
            f.write(line)


def gen_ai_model_codes(opt, ai_model_path, tflite_path):
    ai_model_c_path = os.path.join(ai_model_path, "ai_model.c")
    shutil.copy("../../common_utils/c_codes/ai_model.c", ai_model_c_path)
    with open("../../common_utils/c_codes/ai_model.h", "r") as f:
        lines = f.readlines()
    cfg = load_datafile(opt.data)
    sp = cfg["separation"]
    spc = cfg["separation_scale"]
    conf_thr = opt.conf_thr
    nms_thr = opt.nms_thr
    fix_factor = detector.DetectorOrtTf(cfg, tflite_path).fix
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            if "SEPARATION_CODE" in line:
                line = f"#define SEPARATION {sp}\n"
            elif "SEPARATION_SCALE_CODE" in line:
                line = f"#define SEPARATION_SCALE {spc}\n"
            elif "CONF_THR_CODE" in line:
                line = f"#define CONF_THR {conf_thr}\n"
            elif "NMS_THR_CODE" in line:
                line = f"#define NMS_THR {nms_thr}\n"
            elif "FIX_FACTOR_CODE" in line:
                line = f"#define FIX_FACTOR {fix_factor}\n"
            f.write(line)


def deploy(opt, save_path, tflite_path, gd32_path):
    model_front_path = None
    model_post_path = None
    model_path = None
    name_list = os.listdir(tflite_path)
    for name in name_list:
        if name.endswith(".tflite"):
            if "front" in name:
                model_front_path = os.path.join(tflite_path, name)
            elif "post" in name:
                model_post_path = os.path.join(tflite_path, name)
            else:
                model_path = os.path.join(tflite_path, name)

    stm32ai_exe_path = os.path.join(opt.stm32cubeai_path, "Utilities", "windows", "stm32ai.exe")
    temp_path = os.path.join(save_path, "temp")
    output_path = os.path.join(gd32_path, "Edge_AI")
    output_model_path = os.path.join(output_path, "model")
    stlib_path = os.path.join(output_path, "ST_Lib")
    utils_path = os.path.join(output_path, "utils")

    os.makedirs(temp_path, exist_ok=True)
    os.makedirs(output_path, exist_ok=True)
    os.makedirs(output_model_path, exist_ok=True)
    os.makedirs(stlib_path, exist_ok=True)
    os.makedirs(utils_path, exist_ok=True)
    copy_stlib(opt, opt.stm32cubeai_path, stlib_path)

    license_src_path = os.path.join(os.path.join(opt.stm32cubeai_path, "Middlewares", "ST", "AI"), "LICENSE.txt")
    license_dst_path = os.path.join(output_model_path, "LICENSE.txt")
    shutil.copy(license_src_path, license_dst_path)

    if model_path is None:
        gen_net_codes(stm32ai_exe_path, model_front_path, "network_1", temp_path, output_model_path)
        gen_net_codes(stm32ai_exe_path, model_post_path, "network_2", temp_path, output_model_path)
    else:
        gen_net_codes(stm32ai_exe_path, model_path, "network_1", temp_path, output_model_path)

    gen_utils_codes(opt, utils_path)
    gen_ai_model_codes(opt, output_path, tflite_path)


def deploy_main(opt, save_path, gd32_path):
    export(opt, save_path)
    tflite_path = os.path.join(save_path, "tflite")
    deploy(opt, save_path, tflite_path, gd32_path)


paths = [
    'modified_files/Yolo_FastestV2_main/modelzoo/coco2017-0.241078ap-model.pth',
    'modelzoo/coco_sp_0001/coco-180-epoch-0.117769ap-model.pth',
    "modelzoo/coco_sp_0002/coco-160-epoch-0.174393ap-model.pth",
    "modelzoo/coco_sp_0003/coco-210-epoch-0.157492ap-model.pth",
    "modelzoo/coco_sp_0004/coco-220-epoch-0.213236ap-model.pth",
]
val_paths=[
    '../../../datasets/abnormal_drive_0/images',
    '../../../datasets/coco2017/images/val2017']
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='modelzoo/coco_sp_0004/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="modelzoo/coco_sp_0004/best.pth",
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=val_paths[1],
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--gd32_project_path', type=str, default="",
                        help='The path of gd32 project')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default="D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
                        help='The path of stm32cubeai')
    parser.add_argument('--series', type=str, default="f4",
                        help='The series of gd32,f4 or h7')
    parser.add_argument('--conf_thr', type=float, default=0.5,
                        help='confidence threshold')
    parser.add_argument('--nms_thr', type=float, default=0.5,
                        help='nomaxsupression threshold')
    parser.add_argument('--eval', type=bool, default=False,
                        help='eval exported model')
    opt = parser.parse_args()
    lger = LogSaver(opt.data, "results/deploy")
    lger.collect_prints(deploy_main, opt, lger.result_path, lger.result_path)
