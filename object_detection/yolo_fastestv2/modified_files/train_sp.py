import os
import math
import argparse
import shutil

from tqdm import tqdm

import torch
from torch import optim
from torch.utils.data import dataset
from torchsummary import summary

from .utils import utils_sp,loss_sp,datasets_sp
from .model import detector_sp

def train(opt,log_and_save):
    # modified
    # 指定训练配置文件
    # parser = argparse.ArgumentParser()
    # parser.add_argument('--data', type=str, default='configs/ab_drive.data',
    #                     help='Specify training profile *.data')
    # opt = parser.parse_args()
    # cfg = utils.utils.load_datafile(opt.data)
    cfg = utils_sp.load_datafile(opt.data)

    print("训练配置:")
    print(cfg)

    # modified
    # 数据集加载
    # train_dataset = utils.datasets.TensorDataset(cfg["train"], cfg["width"], cfg["height"], imgaug=True)
    # val_dataset = utils.datasets.TensorDataset(cfg["val"], cfg["width"], cfg["height"], imgaug=False)
    train_dataset = datasets_sp.TensorDatasetSp(cfg["train"], cfg["width"], cfg["height"],cfg["label_flag"], imgaug=True)
    val_dataset = datasets_sp.TensorDatasetSp(cfg["val"], cfg["width"], cfg["height"],cfg["label_flag"], imgaug=False)

    batch_size = int(cfg["batch_size"] / cfg["subdivisions"])
    nw = min([os.cpu_count(), batch_size if batch_size > 1 else 0, 8])
    # 训练集
    train_dataloader = torch.utils.data.DataLoader(train_dataset,
                                                   batch_size=batch_size,
                                                   shuffle=True,
                                                   # modified
                                                   # collate_fn=utils.datasets.collate_fn,
                                                   collate_fn=datasets_sp.collate_fn,
                                                   num_workers=nw,
                                                   pin_memory=True,
                                                   drop_last=True,
                                                   persistent_workers=True
                                                   )
    # 验证集
    val_dataloader = torch.utils.data.DataLoader(val_dataset,
                                                 batch_size=batch_size,
                                                 shuffle=False,
                                                 # modified
                                                 # collate_fn=utils.datasets.collate_fn,
                                                 collate_fn=datasets_sp.collate_fn,
                                                 num_workers=nw,
                                                 pin_memory=True,
                                                 drop_last=False,
                                                 persistent_workers=True
                                                 )

    # 指定后端设备CUDA&CPU
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    # modified
    # 判断是否加载预训练模型
    # load_param = False
    # premodel_path = cfg["pre_weights"]
    # if premodel_path != None and os.path.exists(premodel_path):
    #     load_param = True
    load_param = False
    premodel_path = cfg["pre_weights"]
    if premodel_path == "None":
        load_param = 2
    elif premodel_path == 'pretrain':
        load_param = 0
    elif os.path.exists(premodel_path):
        load_param = 1

    # modified
    # 初始化模型结构
    # model = model.detector.Detector(cfg["classes"], cfg["anchor_num"], load_param).to(device)
    model = detector_sp.DetectorSp(cfg["classes"], cfg["anchor_num"], load_param, separation=cfg["separation"],
                       separation_scale=cfg["separation_scale"]).to(device)
    summary(model, input_size=(3, cfg["height"], cfg["width"]))

    # modified
    # 加载预训练模型参数
    # if load_param ==True:
    #     model.load_state_dict(torch.load(premodel_path, map_location=device), strict=False)
    #     print("Load finefune model param: %s" % premodel_path)
    # else:
    #     print("Initialize weights: model/backbone/backbone.pth")
    if load_param == 1:
        model.load_state_dict(torch.load(premodel_path, map_location=device), strict=False)
        print("Load finefune model param: %s" % premodel_path)
    elif load_param == 0:
        print("Initialize weights: model/backbone/backbone.pth")

    # 构建SGD优化器
    optimizer = optim.SGD(params=model.parameters(),
                          lr=cfg["learning_rate"],
                          momentum=0.949,
                          weight_decay=0.0005,
                          )

    # 学习率衰减策略
    scheduler = optim.lr_scheduler.MultiStepLR(optimizer,
                                               milestones=cfg["steps"],
                                               gamma=0.1)

    print('Starting training for %g epochs...' % cfg["epochs"])

    batch_num = 0
    for epoch in range(cfg["epochs"]):
        model.train()
        pbar = tqdm(train_dataloader)

        for imgs, targets in pbar:
            # 数据预处理
            imgs = imgs.to(device).float() / 255.0
            targets = targets.to(device)

            # 模型推理
            preds = model(imgs)
            # loss计算
            # modified
            # iou_loss, obj_loss, cls_loss, total_loss = utils.loss.compute_loss(preds, targets, cfg, device)
            iou_loss, obj_loss, cls_loss, total_loss = loss_sp.compute_loss(preds, targets, cfg, device)

            # 反向传播求解梯度
            total_loss.backward()

            # 学习率预热
            for g in optimizer.param_groups:
                warmup_num = 5 * len(train_dataloader)
                if batch_num <= warmup_num:
                    scale = math.pow(batch_num / warmup_num, 4)
                    g['lr'] = cfg["learning_rate"] * scale

                lr = g["lr"]

            # 更新模型参数
            if batch_num % cfg["subdivisions"] == 0:
                optimizer.step()
                optimizer.zero_grad()

            # 打印相关信息
            info = "Epoch:%d LR:%f CIou:%f Obj:%f Cls:%f Total:%f" % (
                epoch, lr, iou_loss, obj_loss, cls_loss, total_loss)
            pbar.set_description(info)

            batch_num += 1

        # 模型保存
        if epoch % 10 == 0 and epoch>0:
            model.eval()
            # 模型评估
            print("computer mAP...")
            # modified
            # _, _, AP, _ = utils.utils.evaluation(val_dataloader, cfg, model, device)
            _, _, AP, _ = utils_sp.evaluation(val_dataloader, cfg, model, device,cfg["conf_thr"],nms_thresh=cfg["nms_thr"],iou_thres=cfg["iou_thr"])
            print("computer PR...")
            # modified
            # precision, recall, _, f1 = utils.utils.evaluation(val_dataloader, cfg, model, device, 0.3)
            precision, recall, _, f1 = utils_sp.evaluation(val_dataloader, cfg, model, device, 0.3,nms_thresh=cfg["nms_thr"],iou_thres=cfg["iou_thr"])
            # modified
            # print("Precision:%f Recall:%f AP:%f F1:%f" % (precision, recall, AP, f1))
            log_and_save(precision,recall,AP,f1,epoch,model,cfg)



        # 学习率调整
        scheduler.step()