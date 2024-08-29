import argparse
import sys
from common_utils.utils import LogSaver
from common_utils.onnx2tflite.converter import onnx_converter
from submodules.Yolo_FastestV2.pytorch2onnx import pytorch2onnx_main
from submodules.Yolo_FastestV2.evaluation import evaluation
import os
from submodules.Yolo_FastestV2.utils.utils import load_datafile


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
            int8_mean=0,
            int8_std=255,
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
            int8_mean=0,
            int8_std=255,
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
    'modelzoo/coco_sp_0001/coco-180-epoch-0.117769ap-model.pth',
    "modelzoo/coco_sp_0002/coco-160-epoch-0.174393ap-model.pth",
    "modelzoo/coco_sp_0003/coco-210-epoch-0.157492ap-model.pth",
    "modelzoo/coco_sp_0004/coco-220-epoch-0.213236ap-model.pth",
]

def export(opt,save_path):
    print(f"pytorch_model_path:{opt.weight}")
    onnx_path=os.path.join(save_path,"onnx")
    tflite_path=os.path.join(save_path,"tflite")
    os.makedirs(onnx_path)
    os.makedirs(tflite_path)
    pytorch2onnx_main(opt.config, opt.weight, onnx_path)
    if opt.convert_type==0:
        if opt.eval:
            evaluation( opt.config, onnx_path, 1)
    elif opt.convert_type==1:
        if opt.tflite_val_path is None:
            label=os.path.split(opt.config)[1].split(".")[0]
            assert label in val_paths.keys()
            tflite_val_path=val_paths[label]
        else:
            tflite_val_path=opt.tflite_val_path
        if not os.path.isabs(tflite_val_path):
            tflite_val_path=os.path.abspath(os.path.join(os.path.dirname(__file__),tflite_val_path))
        onnx2tflite_main( save_path,tflite_val_path)
        if opt.eval:
            evaluation( opt.config, tflite_path, 1)


val_paths = {
    'ab_drive':'../../../datasets/abnormal_drive_0/images',
    'coco_80':'../../../datasets/coco2017/images/val2017',
    'coco_person':'../../../datasets/coco2017/val2017_person.txt',
    'hand':'../../../datasets/hand_det/images',
    'coco_sp':'../../../datasets/coco2017/images/val2017',
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='modelzoo/coco_sp_0004/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--weight', type=str, default="modelzoo/coco_sp_0004/best.pth",
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='model type,0 for onnx,1 for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=None,
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--eval', type=bool, default=False,
                        help='eval exported model')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/export")
    lger.collect_prints(export,opt,lger.result_path)


