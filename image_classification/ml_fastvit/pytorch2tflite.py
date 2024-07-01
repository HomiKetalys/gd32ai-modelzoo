#
# For licensing see accompanying LICENSE file.
# Copyright (C) 2023 Apple Inc. All Rights Reserved.
#
import argparse
import os

# import coremltools
import torch
import yaml
from torch import nn

from image_classification.ml_fastvit.submodules.ml_fastvit.models.modules.mobileone import reparameterize_model
from submodules.ml_fastvit import models
from common_utils.onnx2tflite.converter import onnx_converter
from common_utils.utils import LogSaver
from evaluation import validate as evaluate


def parse_args(parser: argparse.ArgumentParser) -> argparse.ArgumentParser:
    parser.add_argument(
        "--variant", type=str, required=True, help="Provide fastvit model variant name."
    )
    parser.add_argument(
        "--output-dir",
        type=str,
        default=".",
        help="Provide location to save exported models.",
    )
    parser.add_argument(
        "--checkpoint",
        type=str,
        default=None,
        help="Provide location of trained checkpoint.",
    )
    return parser

def onnx2tflite_main(export_path, val_path):
    assert os.path.exists(export_path)
    tflite_save_path = os.path.join(export_path,"tflite")
    onnx_save_path=os.path.join(export_path,"onnx")
    cfg_path = None
    model_front_path = None
    model_post_path = None
    model_path = None
    name_list = os.listdir(export_path)
    for name in name_list:
        if name.endswith(".yaml"):
            cfg_path = os.path.join(export_path, name)
    assert cfg_path is not None
    with open(cfg_path,'r') as f:
        cfg=yaml.safe_load(f)

    name_list = os.listdir(onnx_save_path)
    for name in name_list:
        if name.endswith(".onnx"):
            if "front" in name:
                model_front_path = os.path.join(onnx_save_path, name)
            elif "post" in name:
                model_post_path = os.path.join(onnx_save_path, name)
            else:
                model_path = os.path.join(onnx_save_path, name)
    if model_path is None:
        onnx_converter(
            onnx_model_path=model_front_path,
            need_simplify=True,
            output_path=tflite_save_path,
            target_formats=['tflite'],  # or ['keras'], ['keras', 'tflite']
            weight_quant=True,
            int8_model=True,
            int8_mean=None,
            int8_std=None,
            image_root=val_path,
            separation=cfg["separation"],
            separation_scale=cfg["separation_scale"]
        )

        onnx_converter(
            onnx_model_path=model_post_path,
            need_simplify=True,
            output_path=tflite_save_path,
            target_formats=['tflite'],  # or ['keras'], ['keras', 'tflite']
            weight_quant=True,
            int8_model=True,
            int8_mean=None,
            int8_std=None,
            image_root=val_path,
            separation=-cfg["separation"],
            separation_scale=cfg["separation_scale"]
        )
    else:
        onnx_converter(
            onnx_model_path=model_path,
            need_simplify=True,
            output_path=tflite_save_path,
            target_formats=['tflite'],  # or ['keras'], ['keras', 'tflite']
            weight_quant=True,
            int8_model=True,
            int8_mean=None,
            int8_std=None,
            image_root=val_path,
            separation=0,
            separation_scale=2
        )

activations={
    "ReLU":nn.ReLU,
    "ReLU6":nn.ReLU6,
}

def pytorch2onnx_main(config_path, weight_path, onnx_save_path,img_size=None):
    with open(config_path,'r') as f:
        cfg=yaml.safe_load(f)
    separation=cfg["separation"]
    separation_scale=cfg["separation_scale"]
    model = getattr(models, cfg["model"])(separation=separation,separation_scale=separation_scale,act_layer=activations[cfg["global_act"]],num_classes=cfg["num_classes"])
    if weight_path is not None:
        chkpt = torch.load(weight_path)
        model.load_state_dict(chkpt["state_dict"])
    model.eval()
    model = reparameterize_model(model)
    if img_size is None:
        img_size=cfg["input_size"][1:]
    if separation:
        model.export=1
        test_data = torch.rand(1, 3, img_size[0] // separation_scale, img_size[1] // separation_scale)
        torch.onnx.export(model,  # model being run
                          test_data,  # model input (or a tuple for multiple inputs)
                          os.path.join(onnx_save_path, 'model_front.onnx'),
                          # where to save the model (can be a file or file-like object)
                          export_params=True,  # store the trained parameter weights inside the model file
                          opset_version=11,  # the ONNX version to export the model to
                          do_constant_folding=True)  # whether to execute constant folding for optimization
        test_data = torch.rand(1, model.stage_out_channels[separation - 1], img_size[0] // 2 ** separation,
                               img_size[1] // 2 ** separation)
        model.export = 2
        torch.onnx.export(model,  # model being run
                          test_data,  # model input (or a tuple for multiple inputs)
                          os.path.join(onnx_save_path, 'model_post.onnx'),
                          # where to save the model (can be a file or file-like object)
                          export_params=True,  # store the trained parameter weights inside the model file
                          opset_version=11,  # the ONNX version to export the model to
                          do_constant_folding=True)  # whether to execute constant folding for optimization
    else:
        test_data = torch.rand(1, 3,  img_size[0] ,  img_size[1])
        torch.onnx.export(model,  # model being run
                          test_data,  # model input (or a tuple for multiple inputs)
                          os.path.join(onnx_save_path, 'model.onnx'),
                          # where to save the model (can be a file or file-like object)
                          export_params=True,  # store the trained parameter weights inside the model file
                          opset_version=11,  # the ONNX version to export the model to
                          do_constant_folding=True)  # whether to execute constant folding for optimization


def export(opt,save_path):
    print(f"pytorch_model_path:{opt.model_path}")
    onnx_path=os.path.join(save_path,"onnx")
    tflite_path=os.path.join(save_path,"tflite")
    os.makedirs(onnx_path)
    os.makedirs(tflite_path)
    pytorch2onnx_main(opt.config, opt.model_path, onnx_path,opt.img_size)
    if opt.convert_type==0:
        if opt.eval:
            evaluate( opt.config, onnx_path, 1,opt.img_size)
    elif opt.convert_type==1:
        onnx2tflite_main( save_path,opt.tflite_val_path)
        if opt.eval:
            evaluate( opt.config, tflite_path, 1,opt.img_size)




if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default="results/train/20240327-181513-mobilenet_v3_small_0_75-224/food-101.yaml",
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="results/train/20240327-181513-mobilenet_v3_small_0_75-224/model_best.pth.tar",
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='model type,0 for onnx,1 for tflite')
    parser.add_argument('--tflite_val_path', type=str, default="../../../datasets/food-101/images",
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--eval', type=bool, default=True,
                        help='eval exported model')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=(192,192),
                        help='Specify the image size of the input for the exported model.the img size in config is default')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/export")
    lger.collect_prints(export,opt,lger.result_path)
