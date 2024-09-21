import argparse

from common_utils.utils import LogSaver, common_deploy, gen_common_od_codes
from export import export
from submodules.FastestDet.utils.tool import *
from submodules.FastestDet.utils.datasets import *
from submodules.yolov10.myolov10t import YOLOv10OrtTf
from functools import partial


def code_replace(opt, line, cfg, tfmodel):
    sp = cfg.separation
    spc = cfg.separation_scale
    conf_thr = opt.conf_thr
    nms_thr = opt.nms_thr
    w, b = tfmodel.weight, tfmodel.bias
    reg_max = cfg.reg_max
    reg_scale = cfg.reg_scale

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
    elif "MODEL_CONF_CODE" in line:
        line = f"#define yolov10\n"
        line += f"#define OD_MODEL\n"
        line += f"#define REG_MAX {reg_max}\n"
        line += f"#define REG_SCALE {reg_scale}\n"
        line += f"#define RGB_MODE\n"
        if cfg.use_taa:
            line += f"#define USE_TAA\n"
    elif "ACTIVITIES_CODE" in line:
        names_path = cfg.names
        with open(names_path, "r", encoding="utf-8") as f:
            names_ = f.readlines()
        names = []
        for name in names_:
            names.append('"' + name.strip() + '"')
        names = ",".join(names)
        line = "const char *activities[]={" + names + "};\n"
    return line


def deploy(opt, save_path, tflite_path, gen_codes_path):
    cfg = LoadYaml(opt.config)
    tfmodel = YOLOv10OrtTf(cfg, tflite_path)
    gen_ai_model_codes = partial(gen_common_od_codes, code_replace=code_replace, cfg=cfg, tfmodel=tfmodel)
    common_deploy(opt, save_path, tflite_path, gen_codes_path, gen_ai_model_codes)


def deploy_main(opt, save_path, c_project_path):
    print(opt.__dict__)
    if opt.stm32cubeai_path is None:
        os.environ['DO_NOT_USE_PACK_FOR_RESIZE2'] = "True"
    export(opt, save_path)
    tflite_path = os.path.join(save_path, "tflite")
    if c_project_path is None:
        c_project_path = save_path
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
    '../../../datasets/coco2017/images/val2017',
    '../../../datasets/coco2017/val2017_person.txt']

x_cube_ai_v = [
    "D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
    "F:/EDGEDL/en.x-cube-ai-windows-v9-0-0/stedgeai-windows-9.0.0",
]

c_project_path = [
    r"F:\MyWork\KeilWork\gd32h7_rtt_gcc\project.uvprojx",
    r"F:\MyWork\KeilWork\gd32ai_modelzoo_example_keil_project\GD32H759I_EVAL_DET_GCC\MDK-ARM\GD32H759I_EVAL.uvprojx",
]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='../../modelzoo/object_detection/yolov10/coco_80/coco_80.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--weight', type=str, default='../../modelzoo/object_detection/yolov10/coco_80/weights/best.pth',
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=None,
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--c_project_path', type=str,
                        default=c_project_path[1],
                        help='The path of c project,None= results/deploy/xxxx_00xx')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default=None,
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
    parser.add_argument('--deploy_path', type=str, default="results/deploy",
                        help='')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, opt.deploy_path)
    lger.collect_prints(deploy_main, opt, lger.result_path, opt.c_project_path)
