import copy
import argparse

import onnx
import torch.utils.data

from common_utils.onnx2tflite.converter import onnx_converter
from submodules.FastestDet.utils.tool import *
from submodules.FastestDet.utils.datasets import *
from submodules.FastestDet.utils.evaluation import CocoDetectionEvaluator
from submodules.yolov10.myolov10t import YOLOv10t
# from submodules.yolov10.myolot import YOLOv10t
import thop
from common_utils.utils import LogSaver
from val import val
# 指定后端设备CUDA&CPU
device = torch.device("cpu")
from onnxsim import simplify

def export_onnx(opt, save_path):
    assert os.path.exists(opt.config), "请指定正确的配置文件路径"

    # 解析yaml配置文件
    cfg = LoadYaml(opt.config)
    print(cfg)
    # 初始化模型结构
    if opt.weight is not None:
        print("load weight from:%s" % opt.weight)
        model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max,cfg.use_taa).to(device)
        model.load_state_dict(torch.load(opt.weight))
    else:
        model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max,cfg.use_taa).to(device)
    for m in model.modules():
        if getattr(m,"fuse",None) is not None:
            m.fuse()
        if getattr(m,"export",None) is not None:
            m.export=True
    os.environ['YOLOV10_EXPORT'] = "None"
    # # 打印网络各层的张量维度
    params_num = sum(x.numel() for x in model.parameters())
    print(f"params:{params_num}")
    im = torch.empty((1, 3, cfg.input_height, cfg.input_width)).to(device)
    flops = thop.profile(copy.deepcopy(model), inputs=[im], verbose=True)[0] / 1e9 * 2
    print(f"flops:{flops}GFLOPs")

    model.eval()
    model.export = True
    separation = cfg.separation
    separation_scale = cfg.separation_scale
    if separation:
        os.environ['YOLOV10_EXPORT']="FRONT"
        onnx_front_model_path=os.path.join(save_path, 'model_front.onnx')
        onnx_post_model_path = os.path.join(save_path, 'model_post.onnx')
        test_data = torch.rand(1, 3, cfg.input_height // separation_scale, cfg.input_width // separation_scale)
        torch.onnx.export(model,
                          test_data,
                          onnx_front_model_path,
                          export_params=True,
                          opset_version=11,
                          do_constant_folding=True)
        onnx_model=onnx.load_model(onnx_front_model_path)
        sim_model,check=simplify(onnx_model)
        if check:
            onnx.save(sim_model,onnx_front_model_path)
        else:
            print("onnx simplify failed")
        os.environ['YOLOV10_EXPORT']="POST"
        test_data = torch.rand(1, model.channels[separation - 1], cfg.input_height // 2 ** separation,
                               cfg.input_width  // 2 ** separation)
        torch.onnx.export(model,
                          test_data,
                          os.path.join(save_path, 'model_post.onnx'),
                          export_params=True,
                          # opset_version=12,
                          do_constant_folding=True)
        onnx_model=onnx.load_model(onnx_post_model_path)
        sim_model,check=simplify(onnx_model)
        if check:
            onnx.save(sim_model,onnx_post_model_path)
        else:
            print("onnx simplify failed")
    else:
        test_data = torch.rand(1, 3, cfg.input_height, cfg.input_width )
        onnx_model_path=os.path.join(save_path, 'model.onnx')
        torch.onnx.export(model,
                          test_data,
                          onnx_model_path,
                          export_params=True,
                          # opset_version=12,
                          do_constant_folding=True)
        onnx_model=onnx.load_model(onnx_model_path)
        sim_model,check=simplify(onnx_model)
        if check:
            onnx.save(sim_model,onnx_model_path)
        else:
            print("onnx simplify failed")

def export_tflite(opt, export_path,val_path):
    assert os.path.exists(opt.config), "请指定正确的配置文件路径"

    # 解析yaml配置文件
    cfg = LoadYaml(opt.config)
    print(cfg)
    assert os.path.exists(export_path)
    tflite_save_path = os.path.join(export_path,"tflite")
    onnx_save_path=os.path.join(export_path,"onnx")
    model_front_path = None
    model_post_path = None
    model_path = None
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
            separation=cfg.separation,
            separation_scale=cfg.separation_scale
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
            separation=-cfg.separation,
            separation_scale=cfg.separation_scale
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
            separation_scale=2,
        )

def export(opt,save_path):
    print(f"pytorch_model_path:{opt.weight}")
    onnx_path=os.path.join(save_path,"onnx")
    tflite_path=os.path.join(save_path,"tflite")
    os.makedirs(onnx_path)
    os.makedirs(tflite_path)
    export_onnx(opt, onnx_path)
    if opt.convert_type==0:
        if opt.eval:
            opt.weight=onnx_path
            val( opt, onnx_path, 1)
    elif opt.convert_type==1:
        if opt.tflite_val_path is None:
            label=os.path.split(opt.config)[1].split(".")[0]
            assert label in val_paths.keys()
            tflite_val_path=val_paths[label]
        else:
            tflite_val_path=opt.tflite_val_path
        if not os.path.isabs(tflite_val_path):
            tflite_val_path=os.path.abspath(os.path.join(os.path.dirname(__file__),tflite_val_path))
        export_tflite( opt,save_path,tflite_val_path)
        if opt.eval:
            opt.weight=tflite_path
            val( opt, tflite_path, 1)

val_paths = {
    'coco_80':'../../../datasets/coco2017/images/val2017',
    'coco_person':'../../../datasets/coco2017/val2017_person.txt',
    'hand':'../../../datasets/hand_det/images',
}

if __name__ == "__main__":
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='configs/coco_80.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--weight', type=str, default='../../modelzoo/object_detection/yolov10/coco_80/weights/best.pth',
                        help='The path of the model')
    parser.add_argument('--convert_type', type=int, default=1,
                        help='model type,0 for onnx,1 for tflite')
    parser.add_argument('--tflite_val_path', type=str, default=val_paths['coco_80'],
                        help='The path where the image which quantity need is saved')
    parser.add_argument('--eval', type=bool, default=True,
                        help='eval exported model')
    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/export")
    lger.collect_prints(export, opt, lger.result_path)