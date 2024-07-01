import argparse
import shutil
import os
from common_utils.utils import LogSaver,copy_stlib,gen_net_codes
from submodules.Yolo_FastestV2.model import detector
from pytorch2tflite import export
from submodules.Yolo_FastestV2.utils.utils import load_datafile


def gen_utils_codes(opt, utils_path):
    with open("../../common_utils/c_codes/object_detection/utils.c", "r") as f:
        lines = f.readlines()
    cfg = load_datafile(opt.data)
    anchors = cfg["anchors"]
    with open(os.path.join(utils_path, "utils.c"), "w", encoding="utf-8") as f:
        for line in lines:
            if "ANCHOR_CODE" in line:
                anchors_txt = str(anchors)[1:-1]
                line = "float anchors[12]={" + anchors_txt + "};\n"
            f.write(line)


def code_replace(line, cfg, tfmodel):
    sp = cfg["separation"]
    spc = cfg["separation_scale"]
    conf_thr = opt.conf_thr
    nms_thr = opt.nms_thr
    w, b = tfmodel.weight, tfmodel.bias

    if "SEPARATION_CODE" in line:
        line = f"#define SEPARATION {sp}\n"
    elif "SEPARATION_SCALE_CODE" in line:
        line = f"#define SEPARATION_SCALE {spc}\n"
    elif "CONF_THR_CODE" in line:
        line = f"#define CONF_THR {conf_thr}f\n"
    elif "NMS_THR_CODE" in line:
        line = f"#define NMS_THR {nms_thr}f\n"
    elif "FIX_FACTOR0_CODE" in line:
        if sp > 0:
            line = f"#define FIX_FACTOR0 {tfmodel.fix0}f\n"
        else:
            line = ""
    elif "FIX_FACTOR1_CODE" in line:
        if sp > 0:
            line = f"#define FIX_FACTOR1 {tfmodel.fix1}f\n"
        else:
            line = ""
    elif "IMG_NORM_CODE" in line:
        line = f"#define IMG_NORM_BIAS_ONLY\n#define bias {int(b):d}\n"
    elif "MODEL_NAME_CODE" in line:
        line = f"#define yolofastestv2\n"
    elif "ACTIVITIES_CODE" in line:
        names_path=cfg["names"]
        with open(names_path,"r",encoding="utf-8") as f:
            names_=f.readlines()
        names=[]
        for name in names_:
            names.append('"'+name.strip()+'"')
        names=",".join(names)
        line="const char *activities[]={"+names+"};\n"
    return line


def gen_ai_model_codes(opt, ai_model_path, tflite_path):
    cfg = load_datafile(opt.data)
    tfmodel = detector.DetectorOrtTf(cfg, tflite_path)

    with open("../../common_utils/c_codes/object_detection/ai_model.h", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            line = code_replace(line, cfg, tfmodel)
            if len(line) > 0:
                f.write(line)

    with open("../../common_utils/c_codes/object_detection/ai_model.c", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = code_replace(line, cfg, tfmodel)
            if len(line) > 0:
                f.write(line)


def deploy(opt, save_path, tflite_path, gen_codes_path):
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
    output_path = os.path.join(gen_codes_path, "Edge_AI")
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


def deploy_main(opt, save_path, c_project_path):
    print(opt.__dict__)
    export(opt, save_path)
    tflite_path = os.path.join(save_path, "tflite")
    if c_project_path is None:
        c_project_path=save_path
    deploy(opt, save_path, tflite_path, c_project_path)


paths = [
    'modified_files/Yolo_FastestV2_main/modelzoo/coco2017-0.241078ap-model.pth',
    'modelzoo/coco_sp_0001/coco-180-epoch-0.117769ap-model.pth',
    "modelzoo/coco_sp_0002/coco-160-epoch-0.174393ap-model.pth",
    "modelzoo/coco_sp_0003/coco-210-epoch-0.157492ap-model.pth",
    "modelzoo/coco_sp_0004/coco-220-epoch-0.213236ap-model.pth",
]
val_paths = [
    '../../../datasets/abnormal_drive_0/images',
    '../../../datasets/coco2017/images/val2017']

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='modelzoo/ab_drive_0000/ab_drive.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="modelzoo/ab_drive_0000/best.pth",
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=val_paths[0],
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--c_project_path', type=str, default=None,
                        help='The path of c project,None= results/deploy/xxxx_00xx')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default="D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
                        help='The path of stm32cubeai')
    parser.add_argument('--series', type=str, default="h7",
                        help='The series of gd32,f4 or h7')
    parser.add_argument('--conf_thr', type=float, default=0.3,
                        help='confidence threshold')
    parser.add_argument('--nms_thr', type=float, default=0.5,
                        help='nomaxsupression threshold')
    parser.add_argument('--eval', type=bool, default=False,
                        help='eval exported model')
    parser.add_argument('--compiler', type=int, default=1,
                        help='compiler type,0 for armcc,1 fro gcc')
    opt = parser.parse_args()
    lger = LogSaver(opt.data, "results/deploy")
    lger.collect_prints(deploy_main, opt, lger.result_path, opt.c_project_path)
