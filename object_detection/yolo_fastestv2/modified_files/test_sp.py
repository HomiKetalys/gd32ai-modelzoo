import os
import cv2
import time
import argparse

import torch
from torchsummary import summary

from .model import detector_sp
from .utils import utils_sp
from .model.detector_sp import DetectorOrtTf

def test(cfg_path,model_path,img_path,result_path,eval_type):
    cfg = utils_sp.load_datafile(cfg_path)
    assert os.path.exists(model_path), "请指定正确的模型路径"
    assert os.path.exists(img_path), "请指定正确的测试图像路径"

    # 模型加载
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    if eval_type == 0:
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
        model = DetectorOrtTf(cfg, model_path)
        one_norm = False

    # 数据预处理
    ori_img = cv2.imread(img_path)
    res_img = cv2.resize(ori_img, (cfg["width"], cfg["height"]), interpolation=cv2.INTER_LINEAR)
    img = res_img.reshape(1, cfg["height"], cfg["width"], 3)
    img = torch.from_numpy(img.transpose(0, 3, 1, 2))
    img = img.to(device).float() / 255.0

    # 模型推理
    start = time.perf_counter()
    with torch.no_grad():
        preds = model(img)
    end = time.perf_counter()
    ftime = (end - start) * 1000.
    print("forward time:%fms" % ftime)

    # 特征图后处理
    output = utils_sp.handel_preds(preds, cfg, device,one_norm)
    output_boxes = utils_sp.non_max_suppression(output, conf_thres=0.3, iou_thres=0.4)

    # 加载label names
    LABEL_NAMES = []
    with open(cfg["names"], 'r') as f:
        for line in f.readlines():
            LABEL_NAMES.append(line.strip())

    h, w, _ = ori_img.shape
    scale_h, scale_w = h / cfg["height"], w / cfg["width"]

    # 绘制预测框
    for box in output_boxes[0]:
        box = box.tolist()

        obj_score = box[4]
        category = LABEL_NAMES[int(box[5])]

        x1, y1 = int(box[0] * scale_w), int(box[1] * scale_h)
        x2, y2 = int(box[2] * scale_w), int(box[3] * scale_h)

        cv2.rectangle(ori_img, (x1, y1), (x2, y2), (255, 255, 0), 2)
        cv2.putText(ori_img, '%.2f' % obj_score, (x1, y1 - 5), 0, 0.7, (0, 255, 0), 2)
        cv2.putText(ori_img, category, (x1, y1 - 25), 0, 0.7, (0, 255, 0), 2)

    cv2.imwrite(os.path.join(result_path,"test_result.png"), ori_img)

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='data/ab_drive.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default='modelzoo/abd-150-epoch-0.576318ap-model.pth',
                        help='The path of the .pth model to be transformed')
    parser.add_argument('--img', type=str, default='img/123.png',
                        help='The path of test image')
    opt = parser.parse_args()
    # lger=LogSaver(opt.data,"results/test")
    # lger.collect_prints(test,opt.data,opt.model_path,opt.img,lger.result_path)





