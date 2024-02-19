import os
import torch
import argparse
from tqdm import tqdm

from torchsummary import summary
from .utils import utils_sp, datasets_sp
from .model import detector_sp
from .model.detector_sp import DetectorOrtTf

eval_type_name = ["torch", "onnx or tflite"]


def evaluation(cfg_path, model_path, eval_type):
    # modified
    # cfg = utils.utils.load_datafile(opt.data)
    cfg = utils_sp.load_datafile(cfg_path)

    assert os.path.exists(model_path), "请指定正确的模型路径"

    # 打印消息
    print("评估配置:")
    print("model_name:%s" % cfg["model_name"])
    print("width:%d height:%d" % (cfg["width"], cfg["height"]))
    print("val:%s" % (cfg["val"]))
    print("model_path:%s" % (model_path))
    # modified
    print(f"eval_type:{eval_type_name[eval_type]}")

    # 加载数据
    # modified
    # val_dataset = utils.datasets.TensorDataset(cfg["val"], cfg["width"], cfg["height"], imgaug = False)
    val_dataset = datasets_sp.TensorDatasetSp(cfg["val"], cfg["width"], cfg["height"], cfg["label_flag"], imgaug=False)

    batch_size = int(cfg["batch_size"] / cfg["subdivisions"])
    nw = min([os.cpu_count(), batch_size if batch_size > 1 else 0, 8])

    val_dataloader = torch.utils.data.DataLoader(val_dataset,
                                                 batch_size=batch_size,
                                                 shuffle=False,
                                                 # modified
                                                 # collate_fn=utils.datasets_sp.collate_fn,
                                                 collate_fn=datasets_sp.collate_fn,
                                                 num_workers=nw,
                                                 pin_memory=True,
                                                 drop_last=False,
                                                 persistent_workers=True
                                                 )

    # 指定后端设备


    if eval_type == 0:
        device = torch.device("cuda")
        # 初始化模型
        # modified
        # model = model.detector_sp.Detector(cfg["classes"], cfg["anchor_num"], True).to(device)
        model = detector_sp.DetectorSp(cfg["classes"], cfg["anchor_num"], 2, separation=cfg["separation"],
                                       separation_scale=cfg["separation_scale"]).to(device)
        model.load_state_dict(torch.load(model_path, map_location=device))
        # sets the module in eval node
        model.eval()

        # 打印模型结构
        summary(model, input_size=(3, cfg["height"], cfg["width"]))
        one_norm = True
    elif eval_type == 1:
        device = torch.device("cpu")
        model = DetectorOrtTf(cfg, model_path)
        one_norm = False

    # 模型评估
    print("computer mAP...")
    # modified
    # _, _, AP, _ = utils.utils_sp.evaluation(val_dataloader, cfg, model, device)
    _, _, AP, _ = utils_sp.evaluation(val_dataloader, cfg, model, device, cfg["conf_thr"], nms_thresh=cfg["nms_thr"],
                                      iou_thres=cfg["iou_thr"], one_norm=one_norm)
    print("computer PR...")
    # modified
    # precision, recall, _, f1 = utils.utils.evaluation(val_dataloader, cfg, model, device, 0.3)
    precision, recall, _, f1 = utils_sp.evaluation(val_dataloader, cfg, model, device, 0.3, nms_thresh=cfg["nms_thr"],
                                                   iou_thres=cfg["iou_thr"], one_norm=one_norm)

    info = (f"Precision:{list(precision)}\n"
            f"Recall:{list(recall)}\n"
            f"AP:{list(AP)}\n"
            f"F1:{list(f1)}")
    print(info)
    print("Precision:%f Recall:%f AP:%f F1:%f" % (precision.mean(), recall.mean(), AP.mean(), f1.mean()))
