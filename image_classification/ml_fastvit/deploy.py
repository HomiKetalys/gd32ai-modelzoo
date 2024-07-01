import argparse
import shutil
import subprocess
import yaml
import os
from common_utils.utils import LogSaver,copy_stlib,gen_net_codes
from submodules.ml_fastvit.validate import ClassifierTfOrt
from pytorch2tflite import export


def code_replace(line,cfg,tfmodel,opt):
    sp = cfg["separation"]
    spc = cfg["separation_scale"]

    w,b=tfmodel.weight,tfmodel.bias
    mean=[123.675, 116.28, 103.53]
    std=[58.395, 57.12, 57.375]

    if "SEPARATION_CODE" in line:
        line = f"#define SEPARATION {sp}\n"
    elif "SEPARATION_SCALE_CODE" in line:
        line = f"#define SEPARATION_SCALE {spc}\n"
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
        line = f"#define IMG_NORM\n" \
               f"#define bias_r {-mean[0]/std[0]/w+b}f\n" \
               f"#define bias_g {-mean[1]/std[1]/w+b}f\n" \
               f"#define bias_b {-mean[2]/std[2]/w+b}f\n" \
               f"#define weight_r {1/std[0]/w}f\n" \
               f"#define weight_g {1/std[1]/w}f\n" \
               f"#define weight_b {1/std[2]/w}f\n"
    elif "ACTIVITIES_CODE" in line:
        names_path=os.path.join(cfg["data_dir"],"validation")
        names_=os.listdir(names_path)
        names=[]
        for name in names_:
            names.append('"'+name.strip()+'"')
        names=",".join(names)
        line="const char *activities[]={"+names+"};\n"
    return line

def gen_ai_model_codes(opt, ai_model_path, tflite_path):

    with open(opt.config,'r') as f:
        cfg = yaml.safe_load(f)
    tfmodel = ClassifierTfOrt(cfg,tflite_path)

    with open("../../common_utils/c_codes/image_classification/ai_model.h", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            line=code_replace(line,cfg,tfmodel,opt)
            if len(line)>0:
                f.write(line)

    with open("../../common_utils/c_codes/image_classification/ai_model.c", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = code_replace(line,cfg,tfmodel,opt)
            if len(line)>0:
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

    gen_ai_model_codes(opt, output_path, tflite_path)



def deploy_main(opt, save_path, c_project_path):
    print(opt.__dict__)
    export(opt, save_path)
    tflite_path = os.path.join(save_path, "tflite")
    if c_project_path is None:
        c_project_path=save_path
    deploy(opt, save_path, tflite_path, c_project_path)


paths = [
    "results/train/20240323-183131-fastvit_t4-224/",
    "modelzoo/20240318-110647-fastvit_t4-224/"
]
val_paths=[

]



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='modelzoo/food-101-sp-128/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default='modelzoo/food-101-sp-128/model_best.pth.tar',
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default='../../../datasets/food-101/validation',
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--c_project_path', type=str, default=None,
                        help='The path of c project,None= results/deploy/xxxx_00xx')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default="D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
                        help='The path of stm32cubeai')
    parser.add_argument('--series', type=str, default="h7",
                        help='The series of gd32,f4 or h7')
    parser.add_argument('--eval', type=bool, default=False,
                        help='eval exported model')
    parser.add_argument('--compiler', type=int, default=1,
                        help='compiler type,0 for armcc,1 fro gcc')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=None,
                        help='Specify the image size of the input for the exported model.the img size in config is default')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/deploy")
    lger.collect_prints(deploy_main, opt, lger.result_path, opt.c_project_path)
