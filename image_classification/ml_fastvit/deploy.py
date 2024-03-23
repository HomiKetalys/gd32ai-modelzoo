import argparse
import json
import shutil
import subprocess
import sys

import numpy as np
import yaml

from common_utils.utils import LogSaver
import os

from submodules.ml_fastvit.validate import ClassifierTfOrt
from pytorch2tflite import export




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
        lib_type = "ARMCortexM4"
    else:
        raise ValueError("Unsupported series")
    if opt.compiler==0:
        prefix="MDK"
        lib_name = "NetworkRuntime801_CM4_Keil.lib"
    elif opt.compiler==1:
        prefix="GCC"
        lib_name = "NetworkRuntime801_CM4_GCC.a"
    else:
        raise ValueError("unsupported compiler")
    lib_src_path = os.path.join(stlib_src_path, "Lib", prefix, lib_type)
    assert os.path.exists(lib_src_path), "stlib do not exist"
    lib_src_path = os.path.join(lib_src_path, lib_name)
    if opt.compiler==1:
        prefix="lib"
    else:
        prefix=""
    lib_dst_path = os.path.join(stlib_path, "Lib", prefix+lib_name)
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



def code_replace(line,cfg,tfmodel):
    sp = cfg["separation"]
    spc = cfg["separation_scale"]
    if sp>0:
        fix_factor0 = tfmodel.fix0
        fix_factor1=tfmodel.fix1

    w,b=tfmodel.weight,tfmodel.bias
    mean=[123.675, 116.28, 103.53]
    std=[58.395, 57.12, 57.375]

    if "SEPARATION_CODE" in line:
        line = f"#define SEPARATION {sp}\n"
    elif "SEPARATION_SCALE_CODE" in line:
        line = f"#define SEPARATION_SCALE {spc}\n"
    elif "CONF_THR_CODE" in line:
        line = f""
    elif "NMS_THR_CODE" in line:
        line = f""
    elif "FIX_FACTOR0_CODE" in line:
        if sp > 0:
            line = f"#define FIX_FACTOR0 {fix_factor0}f\n"
        else:
            line = ""
    elif "FIX_FACTOR1_CODE" in line:
        if sp > 0:
            line = f"#define FIX_FACTOR1 {fix_factor1}f\n"
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

    return line

def gen_ai_model_codes(opt, ai_model_path, tflite_path):

    with open(opt.config,'r') as f:
        cfg = yaml.safe_load(f)
    tfmodel = ClassifierTfOrt(cfg,tflite_path)

    with open("../../common_utils/c_codes/image_classification/ai_model.h", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            line=code_replace(line,cfg,tfmodel)
            if len(line)>0:
                f.write(line)

    with open("../../common_utils/c_codes/image_classification/ai_model.c", "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = code_replace(line,cfg,tfmodel)
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



def deploy_main(opt, save_path, gd32_path):
    print(opt.__dict__)
    export(opt, save_path)
    tflite_path = os.path.join(save_path, "tflite")
    deploy(opt, save_path, tflite_path, gd32_path)


paths = [

]
val_paths=[

]



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='modelzoo/20240318-110647-fastvit_t4-224/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="modelzoo/20240318-110647-fastvit_t4-224/model_best.pth.tar",
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default='../../../datasets/food-101/validation',
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--gd32_project_path', type=str, default="",
                        help='The path of gd32 project')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default="D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
                        help='The path of stm32cubeai')
    parser.add_argument('--series', type=str, default="h7",
                        help='The series of gd32,f4 or h7')
    parser.add_argument('--eval', type=bool, default=True,
                        help='eval exported model')
    parser.add_argument('--compiler', type=int, default=1,
                        help='compiler type,0 for armcc,1 fro gcc')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=(192,192),
                        help='Specify the image size of the input for the exported model.the img size in config is default')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/deploy")
    lger.collect_prints(deploy_main, opt, lger.result_path, lger.result_path)
