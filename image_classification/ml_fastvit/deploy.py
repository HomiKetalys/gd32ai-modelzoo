import argparse
import shutil
import subprocess
from functools import partial

import yaml
import os
from common_utils.utils import LogSaver,common_deploy,gen_common_ic_codes
from submodules.ml_fastvit.validate import ClassifierTfOrt
from export import export


def code_replace(opt,line,cfg,tfmodel):
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
    elif "MODEL_CONF_CODE" in line:
        line = f"#define RGB_MODE\n" \
               f"#define IC_MODEL\n" \
               f"#define fastvit\n"
    elif "ACTIVITIES_CODE" in line:
        names_path=os.path.join(cfg["data_dir"],"validation")
        names_=os.listdir(names_path)
        names=[]
        for name in names_:
            names.append('"'+name.strip()+'"')
        names=",".join(names)
        line="const char *activities[]={"+names+"};\n"
    return line

def deploy(opt, save_path, tflite_path, c_project_path):
    with open(opt.config,'r') as f:
        cfg = yaml.safe_load(f)
    tfmodel = ClassifierTfOrt(cfg,tflite_path)
    gen_ai_model_codes=partial(gen_common_ic_codes, code_replace=code_replace, cfg=cfg, tfmodel=tfmodel)
    common_deploy(opt,save_path,tflite_path,c_project_path,gen_ai_model_codes)



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

x_cube_ai_v=[
"D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
"F:/EDGEDL/en.x-cube-ai-windows-v9-0-0/stedgeai-windows-9.0.0",
"F:/EDGEDL/en.x-cube-ai-windows-v9-1-0/stedgeai-windows-9.1.0",
]

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='../../modelzoo/image_classification/ml_fastvit/food-101-sp-224/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--weight', type=str, default='../../modelzoo/image_classification/ml_fastvit/food-101-sp-224/model_best.pth.tar',
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='only 1,for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=None,
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--c_project_path', type=str, default=None,
                        help='The path of c project,None= results/deploy/xxxx_00xx')
    parser.add_argument('--stm32cubeai_path', type=str,
                        default=x_cube_ai_v[1],
                        help='The path of stm32cubeai')
    parser.add_argument('--series', type=str, default="h7",
                        help='The series of gd32,f4 or h7')
    parser.add_argument('--eval', type=bool, default=False,
                        help='eval exported model')
    parser.add_argument('--compiler', type=int, default=1,
                        help='compiler type,0 for armcc,1 fro gcc')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=None,
                        help='Specify the image size of the input for the exported model.the img size in config is default')
    parser.add_argument('--deploy_path', type=str, default="results/deploy",
                        help='')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, opt.deploy_path)
    lger.collect_prints(deploy_main, opt, lger.result_path, opt.c_project_path)
