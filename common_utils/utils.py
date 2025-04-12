import argparse
import copy
import importlib
import os
import re
import shutil
import sys
import subprocess
import warnings
from functools import partial

import matplotlib.pyplot as plt
import numpy as np
import onnxruntime as ort
import psutil
import select
import tensorflow as tf
from typing import Union, Callable
# from PIL import Image
# from timm.data import ToNumpy
# from torchvision import transforms

import xml.etree.ElementTree as ET



class multipleSave(object):
    def __getattr__(self, attr, *arguments):
        return self._wrap(attr, *arguments)

    def __init__(self, stdout, myfiles):
        self._stdout = stdout
        self._myfiles = myfiles

    def _wrap(self, attr, *arguments):
        def g(*a, **kw):
            res = getattr(self._stdout, attr, *arguments)(*a, **kw)
            for _p in self._myfiles:
                with open(_p, "a", encoding="utf-8") as f:
                    getattr(f, attr, *arguments)(*a, **kw)
            return res

        return g


class LogSaver():
    def __init__(self, config_path, save_folder_path,auto_rep=False):
        config_name = os.path.split(config_path)[-1]
        folder_name = ".".join(config_name.split(".")[:-1])
        name_index = 0
        folder_path = ""
        if "results" in save_folder_path:
            auto_rep=True
        if auto_rep:
            if not os.path.exists(save_folder_path):
                os.makedirs(save_folder_path)
            while name_index < 10000:
                folder_name_ = folder_name + f"_{name_index:04d}"
                folder_path = os.path.join(save_folder_path, folder_name_)
                if os.path.exists(folder_path):
                    name_index += 1
                else:
                    os.mkdir(folder_path)
                    break
            assert name_index < 10000, "too many results existed"
        else:
            folder_path=save_folder_path
        self.result_path = folder_path
        os.makedirs(self.result_path, exist_ok=True)
        config_backup_path = os.path.join(self.result_path, config_name)
        shutil.copy(config_path, config_backup_path)
        self.log_path = os.path.join(self.result_path, "log.txt")

    def get_result_path(self):
        return self.result_path

    def collect_prints(self, func, *args, **kwargs):
        old_stdout = sys.stdout
        try:
            sys.stdout = multipleSave(sys.stdout, [self.log_path])
            func(*args, **kwargs)
            sys.stdout = old_stdout
        finally:
            sys.stdout = old_stdout


class tfOrtModelRuner():
    def __init__(self, model_path: str):
        if model_path.endswith(".tflite"):
            self.model_interpreter = tf.lite.Interpreter(model_path=model_path)
            self.model_interpreter.allocate_tensors()
            self.model_input_details = self.model_interpreter.get_input_details()[0]
            self.model_output_details = self.model_interpreter.get_output_details()
            self.model_type = 1
        else:
            self.ort_sess = ort.InferenceSession(model_path)
            self.model_type = 0

    def __call__(self, x):
        if self.model_type == 0:
            return self.ort_sess.run(None, {'input.1': x})[0]
        else:
            self.model_interpreter.set_tensor(self.model_input_details['index'], x)
            self.model_interpreter.invoke()
            out_list = []
            for output_details in self.model_output_details:
                out_list.append(self.model_interpreter.get_tensor(output_details['index']))
            if len(out_list) == 1:
                return out_list[0]
            else:
                return out_list


def extract_version_number(text):
    version_pattern = re.compile(r'(\d+(\.\d+){1,2})')
    match = version_pattern.search(text)
    if match:
        return match.group(1)
    return None


def copy_stcore(opt, stm32ai_path, core_path, version):
    stlib_src_path = os.path.join(stm32ai_path, "Middlewares", "ST", "AI")
    if opt.series == "f4":
        lib_type = "ARMCortexM4"
    elif opt.series == "h7":
        if version < "9.0.0":
            lib_type = "ARMCortexM4"
        else:
            lib_type = "ARMCortexM7"
    else:
        assert False, f"Unsupported target:{opt.series}"

    prefix = "MDK"
    lib_src_path = os.path.join(stlib_src_path, "Lib", prefix, lib_type)
    assert os.path.exists(lib_src_path), f"Stlib path:{lib_src_path}\n do not exist"
    lib_name = os.listdir(lib_src_path)[0]
    lib_src_path = os.path.join(lib_src_path, lib_name)
    lib_dst_path = os.path.join(core_path, "Lib", "NetworkRuntime.lib")
    os.makedirs(os.path.join(core_path, "Lib"), exist_ok=True)
    shutil.copy(lib_src_path, lib_dst_path)

    prefix = "GCC"
    lib_src_path = os.path.join(stlib_src_path, "Lib", prefix, lib_type)
    assert os.path.exists(lib_src_path), f"Stlib path:{lib_src_path}\n do not exist"
    lib_names = os.listdir(lib_src_path)
    lib_name = None
    for name in lib_names:
        if "PIC" not in name:
            lib_name = name
            break
    assert lib_name is not None, f"Library file not found in folder path :{lib_src_path}"
    lib_src_path = os.path.join(lib_src_path, lib_name)
    prefix = "lib"
    lib_dst_path = os.path.join(core_path, "Lib", prefix + "NetworkRuntime.a")
    os.makedirs(os.path.join(core_path, "Lib"), exist_ok=True)
    shutil.copy(lib_src_path, lib_dst_path)

    inc_dst_path = os.path.join(core_path, "Inc")
    if os.path.exists(inc_dst_path):
        shutil.rmtree(inc_dst_path)
    inc_src_path = os.path.join(stlib_src_path, "Inc")
    shutil.copytree(inc_src_path, inc_dst_path)
    license_src_path = os.path.join(stlib_src_path, "LICENSE.txt")
    license_dst_path = os.path.join(core_path, "LICENSE.txt")
    shutil.copy(license_src_path, license_dst_path)

def copy_tecore(core_path):
    d_inc_path=os.path.join(core_path,"Inc")
    d_src_path=os.path.join(core_path,"Src")
    d_lib_path=os.path.join(core_path,"Lib")
    os.makedirs(d_lib_path,exist_ok=True)
    s_inc_path=os.path.join(os.path.split(os.path.abspath(__file__))[0],"tinyengine/TinyEngine/include")
    s_src_path=os.path.join(os.path.split(os.path.abspath(__file__))[0],"tinyengine/TinyEngine/src/kernels")
    shutil.copytree(s_inc_path,d_inc_path)
    shutil.copytree(s_src_path,d_src_path)

def copy_mtecore(core_path):
    d_inc_path=os.path.join(core_path,"Inc")
    d_src_path=os.path.join(core_path,"Src")
    d_lib_path=os.path.join(core_path,"Lib")
    os.makedirs(d_lib_path,exist_ok=True)
    s_inc_path=os.path.join(os.path.split(os.path.abspath(__file__))[0],"MTE/mte_core/inc")
    s_src_path=os.path.join(os.path.split(os.path.abspath(__file__))[0],"MTE/mte_core/src")
    shutil.copytree(s_inc_path,d_inc_path)
    shutil.copytree(s_src_path,d_src_path)




class CCR():
    def __init__(self, tflite1_path, tflite2_path=None):
        tflite_model = tfOrtModelRuner(tflite1_path)
        self.input1_shape = tflite_model.model_input_details['shape'][1:]
        self.output1_shape = tflite_model.model_output_details[0]['shape'][1:]
        self.input2_shape = None
        self.ident=""
        self.custom_code_replace=None
        if tflite2_path is not None:
            tflite_model = tfOrtModelRuner(tflite2_path)
            self.input2_shape = tflite_model.model_input_details['shape'][1:]
            self.output2_shape = tflite_model.model_output_details[0]['shape'][1:]

    def common_code_replace(self, opt, line,**kwargs):
        self.get_ident(line)
        if self.custom_code_replace is not None:
            line=self.custom_code_replace(opt,line,**kwargs)
        if opt.engine is not None:
            version = extract_version_number(os.path.split(opt.engine)[1])
            if "RCU_CODE" in line:
                if version is not None and version < "9.0.0":
                    line = "#define NEED_RCU\n"
                else:
                    line = ""
        elif "RCU_CODE" in line:
            line = ""
        if "MODEL_INFO_CODE" in line:
            line = f"#define INPUT_1_H {self.input1_shape[0]}\n" \
                   f"#define INPUT_1_W {self.input1_shape[1]}\n" \
                   f"#define INPUT_1_C {self.input1_shape[2]}\n"

            if self.input2_shape is not None:
                line += f"#define OUTPUT_1_H {self.output1_shape[0]}\n" \
                        f"#define OUTPUT_1_W {self.output1_shape[1]}\n" \
                        f"#define OUTPUT_1_C {self.output1_shape[2]}\n" \
                        f"#define INPUT_2_H {self.input2_shape[0]}\n" \
                        f"#define INPUT_2_W {self.input2_shape[1]}\n" \
                        f"#define INPUT_2_C {self.input2_shape[2]}\n"
                if len(self.output2_shape)>1:
                    line+=f"#define OUTPUT_2_H {self.output2_shape[0]}\n" \
                          f"#define OUTPUT_2_C {self.output2_shape[1]}\n"
                else:
                    line+=f"#define OUTPUT_2_C {self.output2_shape[0]}\n"
            else:
                if len(self.output1_shape)>1:
                    line += f"#define OUTPUT_1_H {self.output1_shape[0]}\n" \
                            f"#define OUTPUT_1_C {self.output1_shape[1]}\n"
                else:
                    line += f"#define OUTPUT_1_C {self.output1_shape[0]}\n"
        elif "INFER_FRAME_CODE" in line:
            if opt.engine is not None:
                if opt.engine == "MTE":
                    line = "#define MTE\n"
                else:
                    line = "#define X_CUBE_AI\n"
            else:
                line = "#define TinyEngine\n"
        if "_CODE" in line:
            line = ""
        else:
            if len(self.ident)>0:
                lines=line.split("\n")
                line=""
                for l in lines:
                    if len(l)>0:
                        line+=self.ident+l+"\n"

        self.ident=""

        return line

    def get_ident(self,line):
        if "_CODE" in line:
            line_code=line.replace(" ","")
            self.ident=line[:line.find(line_code)]


def gen_common_ic_codes(opt, utils_path, ai_model_path, ccr, code_replace, **kwargs):
    ccr.custom_code_replace=code_replace
    c_codes_path = os.path.join(os.path.abspath(os.path.split(__file__)[0]), "c_codes")
    with open(os.path.join(c_codes_path, "ai_model.h"), "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            line = ccr.common_code_replace(opt, line,**kwargs)
            if len(line) > 0:
                f.write(line)

    with open(os.path.join(c_codes_path, "ai_model.c"), "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = ccr.common_code_replace(opt, line, **kwargs)
            if len(line) > 0:
                f.write(line)


def gen_common_od_codes(opt, utils_path, ai_model_path, ccr, code_replace, **kwargs):
    ccr.custom_code_replace = code_replace
    c_codes_path = os.path.join(os.path.abspath(os.path.split(__file__)[0]), "c_codes")
    with open(os.path.join(c_codes_path, "ai_model.h"), "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.h"), "w", encoding="utf-8") as f:
        for line in lines:
            line = ccr.common_code_replace(opt, line, **kwargs)
            if len(line) > 0:
                f.write(line)

    with open(os.path.join(c_codes_path, "ai_model.c"), "r") as f:
        lines = f.readlines()
    with open(os.path.join(ai_model_path, "ai_model.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = ccr.common_code_replace(opt, line, **kwargs)
            if len(line) > 0:
                f.write(line)

    with open(os.path.join(c_codes_path, "utils.c"), "r") as f:
        lines = f.readlines()
    with open(os.path.join(utils_path, "utils.c"), "w", encoding="utf-8") as f:
        for line in lines:
            line = ccr.common_code_replace(opt, line, **kwargs)
            if len(line) > 0:
                f.write(line)


def common_deploy(opt, save_path, tflite_path, gen_codes_path, gen_ai_model_codes):
    uvprojx_path = None
    if os.path.isfile(gen_codes_path):
        uvprojx_path = gen_codes_path
        gen_codes_path, uvprojx_name = os.path.split(gen_codes_path)
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

    temp_path = os.path.join(save_path, "temp")
    output_path = os.path.join(gen_codes_path, "Edge_AI")
    output_model_path = os.path.join(output_path, "model")
    if os.path.exists(output_path):
        shutil.rmtree(output_path)
    ai_core_path = os.path.join(output_path, "ai_core")
    utils_path = os.path.join(output_path, "utils")

    os.makedirs(temp_path, exist_ok=True)
    os.makedirs(output_path, exist_ok=True)
    os.makedirs(output_model_path, exist_ok=True)
    os.makedirs(ai_core_path, exist_ok=True)
    os.makedirs(utils_path, exist_ok=True)

    if opt.engine is not None:
        if opt.engine == "MTE":
            copy_mtecore(core_path=ai_core_path)
            gen_net_func = gen_net_codes_mte
        else:
            version = extract_version_number(os.path.split(opt.engine)[1])
            assert version is not None, f"Unkown X-CUBE-AI version:{version}"
            if version < "9.0.0":
                warnings.warn(
                    f"The version of X-CUBE-AI:{version}<9.0.0.the generated codes may be unable to run your device")
                stm32ai_exe_path = os.path.join(opt.engine, "Utilities", "windows", "stm32ai.exe")
            else:
                stm32ai_exe_path = os.path.join(opt.engine, "Utilities", "windows", "stedgeai.exe")

            copy_stcore(opt, opt.engine, ai_core_path, version)

            license_src_path = os.path.join(os.path.join(opt.engine, "Middlewares", "ST", "AI"), "LICENSE.txt")
            license_dst_path = os.path.join(output_model_path, "LICENSE.txt")
            shutil.copy(license_src_path, license_dst_path)

            gen_net_func = partial(gen_net_codes_xcubeai, stm32ai_exe_path=stm32ai_exe_path, version=version)
    else:
        copy_mtecore(ai_core_path)
        gen_net_func = gen_net_codes_mte

    if model_path is None:
        if opt.engine=="MTE":
            gen_net_func([model_front_path,model_post_path], ["network_1","network_2"], temp_path, output_model_path)
        else:
            gen_net_func(model_front_path, "network_1", temp_path, output_model_path)
            gen_net_func(model_post_path, "network_2", temp_path, output_model_path)
        ccr = CCR(model_front_path, model_post_path)
    else:
        gen_net_func(model_path, "network_1", temp_path, output_model_path)
        ccr = CCR(model_path)

    gen_ai_model_codes(opt, utils_path, output_path, ccr)

    if uvprojx_path is not None:
        if opt.series == "f4":
            cm = "m4"
        elif opt.series == "h7":
            # if version is not None:
            #     if version < "9.0.0":
            #         cm = "m4"
            #     else:
            #         cm = "m7"
            # else:
            cm="m7"
        else:
            assert False, f"Unsupported series {opt.series}"
        set_lib=opt.engine is not None
        deploy_to_keil5(uvprojx_path, cm,set_lib)


def gen_net_codes_xcubeai(model_path, name, work_space_path, output_path, stm32ai_exe_path, version):
    if version >= "9.0.0":
        param_name = "target"
    else:
        param_name = "series"
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
           f"--{param_name} stm32h7 "
           f"--allocate-outputs")
    print(f"Command:\n{cmd}\nwill be excuted to generate net codes")
    process=subprocess.Popen(cmd,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    out_str,_=process.communicate(input=b"n\r\n")

    # result = subprocess.run(cmd, stdout=subprocess.PIPE)
    # result = subprocess.run(['powershell', '-Command', cmd], stdout=subprocess.PIPE)
    # print(out_str.decode("utf-8","ignore"))
    if process.returncode != 0:
        print("Net codes generation failed")
        exit(-1)
    print("Net codes generation successful")


def gen_net_codes_te(tflite_path, name, work_space_path, output_path):
    from .tinyengine.code_generator.CodegenUtilTFlite import GenerateSourceFilesFromTFlite
    peakmem = GenerateSourceFilesFromTFlite(
        tflite_path,
        life_cycle_path=os.path.join(work_space_path, f"{name}_lifecycle.png"),
        model_name=name,
        codegen_root=output_path,
    )
    print(f"Peak memory: {peakmem} bytes")
    print("Net codes generation successful")

def gen_net_codes_mte(tflite_path, name, work_space_path, output_path):
    from .MTE.mte_cg.gen_c_model import gen_codes_from_models
    gen_codes_from_models(tflite_path,output_path,work_space_path,name)



def MDK4GCCAddEdgeAIGroup(parent, file_list, project_path):
    # don't add an empty group
    if len(file_list) == 0:
        return

    group = ET.SubElement(parent, 'Group')
    group_name = ET.SubElement(group, 'GroupName')
    group_name.text = "Edge_AI"

    group_option = ET.SubElement(group, 'GroupOption')
    common_property = ET.SubElement(group_option, "CommonProperty")
    common_property_dict = {
        "UseCPPCompiler": "0",
        "RVCTCodeConst": "0",
        "RVCTZI": "0",
        "RVCTOtherData": "0",
        "ModuleSelection": "0",
        "IncludeInBuild": "2",
        "AlwaysBuild": "2",
        "GenerateAssemblyFile": "2",
        "AssembleAssemblyFile": "2",
        "PublicsOnly": "2",
        "StopOnExitCode": "11",
        "CustomArgument": None,
        "IncludeLibraryModules": None,
        "ComprImg": "1",
    }
    for key_name in common_property_dict:
        key = ET.SubElement(common_property, key_name)
        key.text = common_property_dict[key_name]

    group_arm = ET.SubElement(group_option, "GroupArm")
    carm = ET.SubElement(group_arm, "Carm")
    carm_dict = {
        "arpcs": "2",
        "stkchk": "2",
        "reentr": "2",
        "interw": "2",
        "bigend": "2",
        "Strict": "0",
        "Optim": "3",
        "wLevel": "1",
        "uThumb": "2",
    }
    for key_name in carm_dict:
        key = ET.SubElement(carm, key_name)
        key.text = carm_dict[key_name]

    various_controls = ET.SubElement(carm, "VariousControls")
    ET.SubElement(various_controls, "MiscControls")
    ET.SubElement(various_controls, "Define")
    ET.SubElement(various_controls, "Undefine")
    ET.SubElement(various_controls, "IncludePath")

    aarm = ET.SubElement(group_arm, "Aarm")
    bbe = ET.SubElement(aarm, "bBE")
    bbe.text = "2"
    interw = ET.SubElement(aarm, "interw")
    interw.text = "2"
    various_controls = ET.SubElement(aarm, "VariousControls")
    ET.SubElement(various_controls, "MiscControls")
    ET.SubElement(various_controls, "Define")
    ET.SubElement(various_controls, "Undefine")
    ET.SubElement(various_controls, "IncludePath")

    files = ET.SubElement(group, "Files")
    for file_path in file_list:
        file_name = os.path.split(file_path)[1]
        file = ET.SubElement(files, "File")
        file_n = ET.SubElement(file, "FileName")
        file_n.text = file_name
        file_t = ET.SubElement(file, "FileType")
        file_t.text = "1"
        file_p = ET.SubElement(file, "FilePath")
        file_p.text = os.path.relpath(file_path, project_path)

    return group


def MDK4ARMCCAddEdgeAIGroup(parent, file_list, project_path):
    # don't add an empty group
    if len(file_list) == 0:
        return

    group = ET.SubElement(parent, 'Group')
    group_name = ET.SubElement(group, 'GroupName')
    group_name.text = "Edge_AI"

    group_option = ET.SubElement(group, 'GroupOption')
    common_property = ET.SubElement(group_option, "CommonProperty")
    common_property_dict = {
        "UseCPPCompiler": "0",
        "RVCTCodeConst": "0",
        "RVCTZI": "0",
        "RVCTOtherData": "0",
        "ModuleSelection": "0",
        "IncludeInBuild": "2",
        "AlwaysBuild": "2",
        "GenerateAssemblyFile": "2",
        "AssembleAssemblyFile": "2",
        "PublicsOnly": "2",
        "StopOnExitCode": "11",
        "CustomArgument": None,
        "IncludeLibraryModules": None,
        "ComprImg": "1",
    }
    for key_name in common_property_dict:
        key = ET.SubElement(common_property, key_name)
        key.text = common_property_dict[key_name]

    group_arm = ET.SubElement(group_option, "GroupArmAds")
    cads = ET.SubElement(group_arm, "Cads")
    cads_dict = {
        "interw": "2",
        "Optim": "3",
        "oTime": "1",
        "SplitLS": "2",
        "OneElfS": "2",
        "Strict": "2",
        "EnumInt": "2",
        "PlainCh": "2",
        "Ropi": "2",
        "Rwpi": "2",
        "wLevel": "1",
        "uThumb": "2",
        "uSurpInc": "2",
        "uC99": "2",
        "uGnu": "2",
        "useXO": "2",
        "v6Lang": "0",
        "v6LangP": "0",
        "vShortEn": "2",
        "vShortWch": "2",
        "v6Lto": "2",
        "v6WtE": "2",
        "v6Rtti": "2",
    }
    for key_name in cads_dict:
        key = ET.SubElement(cads, key_name)
        key.text = cads_dict[key_name]

    various_controls = ET.SubElement(cads, "VariousControls")
    ET.SubElement(various_controls, "MiscControls")
    ET.SubElement(various_controls, "Define")
    ET.SubElement(various_controls, "Undefine")
    ET.SubElement(various_controls, "IncludePath")

    aads = ET.SubElement(group_arm, "Aads")
    aads_dict = {
        "interw": "2",
        "Ropi": "2",
        "Rwpi": "2",
        "thumb": "2",
        "SplitLS": "2",
        "SwStkChk": "2",
        "NoWarn": "2",
        "uSurpInc": "2",
        "useXO": "2",
        "uClangAs": "2",
    }
    for key_name in aads_dict:
        key = ET.SubElement(aads, key_name)
        key.text = aads_dict[key_name]
    various_controls = ET.SubElement(aads, "VariousControls")
    ET.SubElement(various_controls, "MiscControls")
    ET.SubElement(various_controls, "Define")
    ET.SubElement(various_controls, "Undefine")
    ET.SubElement(various_controls, "IncludePath")

    files = ET.SubElement(group, "Files")
    for file_path in file_list:
        file_name = os.path.split(file_path)[1]
        file = ET.SubElement(files, "File")
        file_n = ET.SubElement(file, "FileName")
        file_n.text = file_name
        file_t = ET.SubElement(file, "FileType")
        if file_name.endswith(".c"):
            file_t.text = "1"
        elif file_name.endswith(".lib"):
            file_t.text = "4"
        file_p = ET.SubElement(file, "FilePath")
        file_p.text = os.path.relpath(file_path, project_path)

    return group


fpu_dict = {
    "m7": "fpv5-d16",
    "m4": "fpv4-sp-d16",
}


def deploy_to_keil5_gcc(root, uvprojx_path, cm="m7",set_lib=False):
    include_paths = root.find("Targets/Target/TargetOption/TargetArm/Carm/VariousControls/IncludePath")
    paths = include_paths.text
    if paths is None:
        paths = ""
    path_set = set(paths.split(";"))
    path_set = path_set.union([
        ".\\Edge_AI\\model",
        ".\\Edge_AI",
        ".\\Edge_AI\\ai_core\\Inc"])
    include_paths.text = ";".join(list(path_set))


    lib_names = root.find("Targets/Target/TargetOption/TargetArm/LDarm/IncludeLibs")
    paths = lib_names.text
    if paths is None:
        path_set = set()
    else:
        path_set = set(paths.split(";"))
    if set_lib:
        path_set = path_set.union(["NetworkRuntime"])
    else:
        if "NetworkRuntime" in path_set:
            path_set.remove("NetworkRuntime")
    lib_names.text = ";".join(list(path_set))
    lib_paths = root.find("Targets/Target/TargetOption/TargetArm/LDarm/IncludeDir")
    paths = lib_paths.text
    if paths is None:
        path_set = set()
    else:
        path_set = set(paths.split(";"))
    if set_lib:
        path_set = path_set.union([".\\Edge_AI\\ai_core\\Lib"])
    else:
        if ".\\Edge_AI\\ai_core\\Lib" in path_set:
            path_set.remove(".\\Edge_AI\\ai_core\\Lib")
    lib_paths.text = ";".join(list(path_set))

    mi_dict = {
        "-mcpu": "cortex-" + cm,
        "-mfpu": fpu_dict[cm],
        "-mfloat-abi": "hard",
    }

    misc = root.find("Targets/Target/TargetOption/TargetArm/LDarm/Misc")
    misc_list = misc.text
    if misc_list is None:
        misc_list = []
    else:
        misc_list = misc_list.split(" ")
    mi_dict_ = copy.deepcopy(mi_dict)
    for i, mi in enumerate(misc_list):
        keys = list(mi_dict_.keys())
        for key in keys:
            if key in mi:
                misc_list[i] = key + "=" + mi_dict_[key]
                mi_dict_.pop(key)
    keys = mi_dict_.keys()
    for key in keys:
        misc_list.append(key + "=" + mi_dict_[key])
    misc.text = " ".join(misc_list)

    misc = root.find("Targets/Target/TargetOption/TargetArm/Carm/VariousControls/MiscControls")
    misc_list = misc.text
    if misc_list is None:
        misc_list = []
    else:
        misc_list = misc_list.split(" ")
    mi_dict_ = copy.deepcopy(mi_dict)
    for i, mi in enumerate(misc_list):
        keys = list(mi_dict_.keys())
        for key in keys:
            if key in mi:
                misc_list[i] = key + "=" + mi_dict_[key]
                mi_dict_.pop(key)
    keys = mi_dict_.keys()
    for key in keys:
        misc_list.append(key + "=" + mi_dict_[key])
    misc.text = " ".join(misc_list)

    groups = root.find('Targets/Target/Groups')
    if groups is None:
        groups = ET.SubElement(root.find('Targets/Target'), 'Groups')
    else:
        group_list = groups.findall("Group")
        for grp in group_list:
            grp_n = grp.find("GroupName")
            if grp_n.text == "Edge_AI":
                groups.remove(grp)

    uvprojx_project_path = os.path.split(uvprojx_path)[0]
    Edge_AI_path = os.path.join(uvprojx_project_path, "Edge_AI")

    def get_c_file_path(root_path):
        names = os.listdir(root_path)
        path_list = []
        for name in names:
            path = os.path.join(root_path, name)
            if os.path.isfile(path):
                if name.endswith(".c"):
                    path_list.append(path)
            else:
                path_list += get_c_file_path(path)
        return path_list

    c_file_list = get_c_file_path(Edge_AI_path)
    MDK4GCCAddEdgeAIGroup(groups, c_file_list, uvprojx_project_path)


def deploy_to_keil5_armcc(root, uvprojx_path, cm="m7"):
    include_paths = root.find("Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/IncludePath")
    paths = include_paths.text
    if paths is None:
        paths = ""
    path_set = set(paths.split(";"))
    path_set = path_set.union([
        ".\\Edge_AI\\model",
        ".\\Edge_AI",
        ".\\Edge_AI\\ai_core\\Inc"])
    include_paths.text = ";".join(list(path_set))

    groups = root.find('Targets/Target/Groups')
    if groups is None:
        groups = ET.SubElement(root.find('Targets/Target'), 'Groups')
    else:
        group_list = groups.findall("Group")
        for grp in group_list:
            grp_n = grp.find("GroupName")
            if grp_n.text == "Edge_AI":
                groups.remove(grp)

    uvprojx_project_path = os.path.split(uvprojx_path)[0]
    Edge_AI_path = os.path.join(uvprojx_project_path, "Edge_AI")

    def get_clib_file_path(root_path):
        names = os.listdir(root_path)
        path_list = []
        for name in names:
            path = os.path.join(root_path, name)
            if os.path.isfile(path):
                if name.endswith(".c") or name.endswith(".lib"):
                    path_list.append(path)
            else:
                path_list += get_clib_file_path(path)
        return path_list

    c_file_list = get_clib_file_path(Edge_AI_path)
    MDK4ARMCCAddEdgeAIGroup(groups, c_file_list, uvprojx_project_path)


def deploy_to_keil5(uvprojx_path, cm="m7",set_lib=False):
    tree = ET.parse(uvprojx_path)
    root = tree.getroot()

    # out = open(os.path.join(os.path.split(uvprojx_path)[0],"text.uvprojx"), 'w')
    # out.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
    complier_type = root.find("Targets/Target/ToolsetName")
    if complier_type.text == "ARM-GNU":
        deploy_to_keil5_gcc(root, uvprojx_path, cm,set_lib)
    elif complier_type.text == "ARM-ADS":
        deploy_to_keil5_armcc(root, uvprojx_path, cm)
    else:
        assert False, "Unsupported uxprojx complier"


    # out.write(ET.tostring(root, encoding='utf-8').decode())
    # out.close()
    tree.write(uvprojx_path, encoding="utf-8", xml_declaration=True, short_empty_elements=False)


def l2(x, y):
    return np.sqrt(np.sum((x - y) ** 2, 1))


def silhouette_coef(x, indexs, cluster_num, center, criterion):
    dis_all = 0
    for i in range(0, cluster_num):
        x_in = x[indexs == i, :]
        dis_out = np.zeros((x_in.shape[0]))
        for j in range(0, x_in.shape[0]):
            sample = x_in[j:j + 1, :]
            idx = np.ones((cluster_num,), dtype="int") == 1
            idx[i] = False
            center_out = center[idx, :]
            dis = criterion(center_out, sample)
            idx = np.argmin(dis)
            if idx >= i:
                idx += 1
            x_out = x[indexs == idx, :]
            dis = criterion(x_out, sample)
            dis_out[j] = np.mean(dis)
        x_in_r0 = np.repeat(x_in[:, :, None], repeats=x_in.shape[0], axis=2)
        x_in_r1 = x_in_r0.transpose((2, 1, 0))
        dis_in = criterion(x_in_r0, x_in_r1)
        dis_in = np.mean(dis_in, axis=1)
        dis = (dis_out - dis_in) / np.maximum(dis_in, dis_out)
        dis_all += np.sum(dis)
    dis_all = dis_all / x.shape[0]
    return dis_all


class kmeans():
    def __init__(self, cluster_num: Union[str, int], criterion: Callable = l2):
        self.cluster_num = cluster_num
        self.criterion = criterion
        self.max_iter_num = 100

    def fit_(self, x, cluster_num):
        final_center = None
        final_dis = -1
        for i in range(0, 20):
            center_index = np.random.randint(0, x.shape[0], (cluster_num,))
            center = np.copy(x[center_index, :])
            total_dis = 0
            for iter_num in range(0, self.max_iter_num):
                center_r = np.repeat(center[:, :, None], repeats=x.shape[0], axis=2)
                x_r = np.repeat(x[:, :, None], repeats=cluster_num, axis=2)
                center_r = np.transpose(center_r, (2, 1, 0))
                dis = self.criterion(x_r, center_r)
                indexs = np.argmin(dis, axis=1)
                for i in range(0, cluster_num):
                    center[i, :] = np.mean(x[indexs == i, :], axis=0)
                if iter_num + 1 == self.max_iter_num:
                    # total_dis = np.sum(np.reshape(dis,(-1,1))[np.arange(0, x.shape[0]) * cluster_num + indexs])
                    total_dis = silhouette_coef(x, indexs, cluster_num, center, self.criterion)
            if total_dis > final_dis:
                final_dis = total_dis
                final_center = center
        return final_center, final_dis

    def fit(self, x):
        x = np.array(x)
        if isinstance(self.cluster_num, int):
            self.center, _ = self.fit_(x, self.cluster_num)
        elif self.cluster_num == "auto":
            dis_list = []
            center_list = []
            for i in range(1, 20):
                center, dis = self.fit_(x, i + 1)
                dis_list.append(dis)
                center_list.append(center)
            dis_list = np.array(dis_list)
            print(dis_list)
            index = np.argmax(dis_list)
            self.center = center_list[index]
            self.cluster_num = index + 2

    def predict(self, x):
        center = np.repeat(self.center[:, :, None], repeats=x.shape[0], axis=2)
        x = np.repeat(x[:, :, None], repeats=self.cluster_num, axis=2)
        center = np.transpose(center, (2, 1, 0))
        dis = self.criterion(x, center)
        return np.argmin(dis, axis=1)


class GMM():
    def __init__(self, cluster_num: Union[str, int] = "auto", criterion: Callable = l2):
        self.cluster_num = cluster_num
        self.criterion = criterion
        self.max_iter_num = 100

    def fit_(self, x, cluster_num):
        x_mean = np.mean(x, axis=0, keepdims=True)
        x_var = np.var(x, axis=0, keepdims=True)
        x = (x - x_mean) / np.sqrt(x_var + 1e-8)
        self.x_mean = x_mean
        self.x_var = x_var
        mean = np.random.randn(cluster_num, x.shape[1])
        covar_matrix = np.diag(np.ones(x.shape[1]))[None, :, :].repeat(cluster_num, 0)
        pz = np.random.random((cluster_num,))
        pz = pz / np.sum(pz)
        for n in range(0, self.max_iter_num):
            p_xj_1zi_list = []
            for i in range(0, cluster_num):
                cvmati = np.linalg.inv(covar_matrix[i])
                cvmatd = np.linalg.det(covar_matrix[i])
                p_xj_1zi = np.exp(
                    -0.5 * np.sum((x - mean[i, :]) * ((x - mean[i, :]) @ cvmati), axis=1, keepdims=True)) / (
                                   np.sqrt(2 * np.pi * cvmatd) ** x.shape[1])
                p_xj_1zi_list.append(p_xj_1zi)
            p_xj_1zi = np.concatenate(p_xj_1zi_list, axis=1)
            Qjzi = p_xj_1zi / (p_xj_1zi @ pz[:, None])
            pz = np.mean(Qjzi, axis=0)
            for i in range(0, cluster_num):
                mean[i, :] = (Qjzi[:, i] @ x) / np.sum(Qjzi[:, i])
                xt = np.sqrt(Qjzi[:, i:i + 1]) * (x - mean[i, :])
                covar_matrix[i, :, :] = (np.transpose(xt) @ xt) / np.sum(Qjzi[:, i])
        return mean, covar_matrix, pz

    def fit(self, x):
        x = np.array(x)
        if isinstance(self.cluster_num, int):
            self.mean, self.covar_matrix, self.pz = self.fit_(x, self.cluster_num)
        elif self.cluster_num == "auto":
            return

    def predict(self, x):
        x = (x - self.x_mean) / np.sqrt(self.x_var + 1e-8)
        p_xj_1zi_list = []
        for i in range(0, self.cluster_num):
            cvmati = np.linalg.inv(self.covar_matrix[i])
            cvmatd = np.linalg.det(self.covar_matrix[i])
            p_xj_1zi = np.exp(
                -0.5 * np.sum((x - self.mean[i, :]) * ((x - self.mean[i, :]) @ cvmati), axis=1, keepdims=True)) / (
                               np.sqrt(2 * np.pi * cvmatd) ** x.shape[1])
            p_xj_1zi_list.append(p_xj_1zi)
        p_xj_1zi = np.concatenate(p_xj_1zi_list, axis=1)
        Qjzi = p_xj_1zi / (p_xj_1zi @ self.pz[:, None])
        return np.argmax(Qjzi, axis=1)


def cos(x, y):
    return -np.sum(x * y, axis=1) / (np.sqrt(np.sum(x ** 2, axis=1)) * np.sqrt(np.sum(y ** 2, axis=1)))


def radius(x, y):
    return np.abs(np.sqrt(np.sum(x ** 2, axis=1)) - np.sqrt(np.sum(y ** 2, axis=1)))


def kmeans_main():
    N = "auto"
    x1 = np.array([0, 0]) + np.random.randn(180, 2)
    x2 = np.array([2, 2]) + np.random.randn(180, 2)
    x3 = np.array([3, 3]) + np.random.randn(180, 2)
    x = np.concatenate((x1, x2, x3), axis=0)

    model = kmeans(N, criterion=l2)
    model.fit(x)
    print(f"num:{model.cluster_num}")
    pred = model.predict(x)
    color = np.random.randint(0, 256, (model.cluster_num, 3))
    color_map = color[pred, :]
    plt.scatter(x[:, 0], x[:, 1], c=color_map / 255)
    plt.show()
    input()


def gmm_main():
    N = "auto"
    x1 = np.array([0, 0]) + np.random.randn(180, 2)
    x2 = np.array([2, 2]) + np.random.randn(180, 2)
    x3 = np.array([4, 4]) + np.random.randn(180, 2)
    x = np.concatenate((x1, x2, x3), axis=0)

    model = GMM(3, criterion=l2)
    model.fit(x)
    print(f"num:{model.cluster_num}")
    pred = model.predict(x)
    color = np.random.randint(0, 256, (model.cluster_num, 3))
    color_map = color[pred, :]
    plt.scatter(x[:, 0], x[:, 1], c=color_map / 255)
    plt.show()
    input()


# def img2rgb565code(img_path,img_size=(192,192)):
#     img = Image.open(img_path)
#     tfs = transforms.Compose(
#         [
#             transforms.Resize(int(img_size[0]/0.875)),
#             transforms.CenterCrop(img_size),
#             ToNumpy(),
#         ]
#     )
#     img = tfs(img)
#     img=np.transpose(img,(1,2,0)).astype("int")
#     img_rgb565=(img[:,:,0]//2**3)*2**11+(img[:,:,1]//2**2)*2**5+img[:,:,2]//2**3
#     img_rgb565=img_rgb565.astype("uint16")
#     img_rgb565=np.rot90(img_rgb565,2)
#     img_rgb565_=img_rgb565.tobytes()
#     code=f"const u8 example_img[{img_size[0]}*{img_size[1]}*2]="+"{"
#     for i in range(0,img_size[0]*img_size[1]*2):
#         a="0x"+img_rgb565_[i:i+1].hex()
#         if i!=img_size[0]*img_size[1]*2-1:
#             a+=","
#         else:
#             a+="};"
#         code+=a
#     return code


def gen_code_test(tflite_path, name, work_space_path, output_path):
    from tinyengine.code_generator.CodegenUtilTFlite import GenerateSourceFilesFromTFlite
    peakmem = GenerateSourceFilesFromTFlite(
        tflite_path,
        life_cycle_path=os.path.join(work_space_path, "lifecycle.png"),
        model_name=name,
        codegen_root=output_path,
    )
    print(f"Peak memory: {peakmem} bytes")

x_cube_ai_v=[
"D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
"F:/EDGEDL/en.x-cube-ai-windows-v9-0-0/stedgeai-windows-9.0.0",
"F:/EDGEDL/en.x-cube-ai-windows-v9-1-0/stedgeai-windows-9.1.0",
]

def deploy_tflite(tflite_path):
    stm32ai_exe_path = os.path.join(x_cube_ai_v[2], "Utilities", "windows", "stedgeai.exe")
    gen_net_func = partial(gen_net_codes_xcubeai, stm32ai_exe_path=stm32ai_exe_path, version="9.1.0")

    gen_net_func(tflite_path, "network_1", "temp/x_cube_ai/temp", "temp/x_cube_ai/model")

def auto_deploy():
    proot=os.path.split(os.path.split(os.path.abspath(__file__))[0])[0]
    root=os.path.join(proot,"modelzoo")
    names=["object_detection","image_classification"]
    config_types=[".data",".yaml"]

    def find_config(path):
        file_names=os.listdir(path)
        for file_name in file_names:
            for config_type in config_types:
                if file_name.endswith(config_type):
                    file_path=os.path.join(path,file_name)
                    if os.path.isfile(file_path):
                        return file_path
        return None

    def find_weight(path):
        file_names = os.listdir(path)

        for file_name in file_names:
            file_path=os.path.join(path,file_name)
            if os.path.isfile(file_path):
                if "best" in file_name:
                    return file_path
            else:
                weight_path = find_weight(file_path)
                if weight_path is not None:
                    return weight_path
        return None

    class tempOpt():
        def __init__(self):
            self.config=None
            self.weight=None
            self.convert_type=1
            self.c_project_path=None
            self.tflite_val_path=None
            self.engine=None
            self.series=None
            self.eval=False
            self.compiler=1
            self.img_size=None
            self.conf_thr=0.3
            self.nms_thr=0.5

    infer_frames_path={
        'TinyEngine':None,
        'X-CUBE-AI':r"D:/STM32CubeIDE_1.12.1/STM32CubeIDE/STM32Cube/Repo/Packs/STMicroelectronics/X-CUBE-AI/8.0.1",
    }
    infer_frames=['TinyEngine','X-CUBE-AI']
    series=['f4','h7']
    unsupport_dict={
        'ml_fastvit':['f4','TinyEngine']
    }

    sys_path=copy.deepcopy(sys.path)
    for name in names:
        path=os.path.join(root,name)
        model_names=os.listdir(path)
        for model_name in model_names:
            modelzoo_path=os.path.join(path,model_name)
            if os.path.isfile(modelzoo_path):
                continue
            rel_path=os.path.relpath(modelzoo_path,root)
            model_path=os.path.abspath(os.path.join(proot,rel_path))
            # exec(f"from {name}.{model_name}.deploy import deploy_main")
            os.chdir(model_path)
            sys.path=copy.deepcopy(sys_path)
            sys.path.append(model_path)
            # del deploy_main
            # deploy_main = importlib.import_module(f"deploy")

            # spec = importlib.util.spec_from_file_location("deploy", os.path.join(model_path,"deploy.py"))
            # deploy_module = importlib.util.module_from_spec(spec)
            # spec.loader.exec_module(deploy_module)
            # deploy_main=getattr(deploy_module,"deploy_main")

            model_ins_names=os.listdir(modelzoo_path)
            for model_ins_name in model_ins_names:
                model_ins_path=os.path.join(modelzoo_path,model_ins_name)
                config_path=find_config(model_ins_path)
                assert config_path is not None
                weight_path=find_weight(model_ins_path)
                assert weight_path is not None
                opt=tempOpt()
                opt.config=config_path
                opt.weight=weight_path

                for infra in infer_frames:
                    if model_name in unsupport_dict.keys():
                        if infra in unsupport_dict[model_name]:
                            continue
                    opt.engine=infer_frames_path[infra]
                    for seri in series:
                        if model_name in unsupport_dict.keys():
                            if seri in unsupport_dict[model_name]:
                                continue
                        opt.series=seri
                        deploy_path=os.path.join(model_ins_path,infra,seri)
                        if os.path.exists(deploy_path):
                            continue
                        os.makedirs(deploy_path)
                        cmd=f'conda activate gd32ai-modelzoo && set PYTHONPATH={proot} && python "{os.path.join(model_path,"deploy.py")}" ' \
                            f'--config "{opt.config}" ' \
                            f'--weight "{opt.weight}" ' \
                            f'--c_project_path "{deploy_path}" ' \
                            f'--deploy_path "{deploy_path}" '
                        if opt.engine is not None:
                            cmd+=f'--engine "{opt.engine}" '
                        cmd+=f'--series "{opt.series}" ' \
                             f'--eval True'
                        assert os.system(cmd)==0







if __name__ == "__main__":
    # gen_code_test("temp/model.tflite", "network_1", "temp/temp", "temp/codegen")
    # deploy_tflite("temp/mobilenet_v2_0.35_128_tfs_int8.tflite")
    auto_deploy()
