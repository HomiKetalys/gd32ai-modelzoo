import argparse
from common_utils.utils import LogSaver
from modified_files import pytorch2onnx_main
from modified_files import evaluation
import os
from modified_files import onnx_converter
from modified_files import load_datafile


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
        if name.endswith(".data"):
            cfg_path = os.path.join(export_path, name)
    assert cfg_path is not None
    cfg = load_datafile(cfg_path)

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
            int8_mean=0,
            int8_std=255,
            image_root=val_path,
            separation=0,
            separation_scale=2
        )

# def consistency_verify(model_path,
paths=[
    'modified_files/Yolo_FastestV2_main/modelzoo/coco2017-0.241078ap-model.pth',
    'modelzoo/coco_sp_0000/coco-20-epoch-0.199881ap-model.pth',
    "modelzoo/coco_sp_0005/coco-220-epoch-0.213236ap-model.pth",
]

def export(opt,save_path):
    onnx_path=os.path.join(save_path,"onnx")
    tflite_path=os.path.join(save_path,"tflite")
    os.makedirs(onnx_path)
    os.makedirs(tflite_path)
    pytorch2onnx_main(opt.data, opt.model_path, onnx_path)
    if opt.convert_type==0:
        evaluation( opt.data, onnx_path, 1)
    elif opt.convert_type==1:
        onnx2tflite_main( save_path,opt.tflite_val_path)
        evaluation( opt.data, tflite_path, 1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='modelzoo/coco_sp_0005/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default=paths[2],
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='model type,0 for onnx,1 for tflite')
    parser.add_argument('--onnx_output', type=str, default='results/export/onnx',
                        help='The path where the onnx model is saved')
    parser.add_argument('--tflite_output', type=str, default='results/export/tflite',
                        help='The path where the tflite model is saved')
    parser.add_argument('--tflite_val_path', type=str, default='../../../datasets/coco2017/images/val2017',
                        help='The path where the image which quantity need is saved')
    opt = parser.parse_args()
    lger = LogSaver(opt.data, "results/export")
    lger.collect_prints(export,opt,lger.result_path)


