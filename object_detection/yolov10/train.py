import copy
import math
import argparse
import os

import torch.utils.data
from tqdm import tqdm
from torch import optim

from submodules.FastestDet.utils.tool import *
from submodules.FastestDet.utils.datasets import *
from submodules.FastestDet.utils.evaluation import CocoDetectionEvaluator
from submodules.yolov10.myolov10t import YOLOv10t
# from submodules.yolov10.myolot import YOLOv10t
from submodules.FastestDet.module.loss import v10DetectorLoss
import thop
from common_utils.utils import LogSaver

from torch.cuda.amp import autocast
from torch.cuda.amp import GradScaler

# 指定后端设备CUDA&CPU
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")


def train(opt, save_path):
    assert os.path.exists(opt.config), "请指定正确的配置文件路径"

    # 解析yaml配置文件
    cfg = LoadYaml(opt.config)
    print(cfg)

    ckpt_path = os.path.join(save_path, "weights")
    os.makedirs(ckpt_path, exist_ok=True)

    # 初始化模型结构
    if opt.weight is not None:
        print("load weight from:%s" % opt.weight)
        model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max,cfg.use_taa).to(device)
        ckpt=torch.load(opt.weight)
        ckpt['detect.cv3.0.1.weight']=ckpt['detect.cv3.0.1.weight'][0:cfg.category_num,:,:,:]
        ckpt['detect.cv3.0.1.bias']=ckpt['detect.cv3.0.1.bias'][0:cfg.category_num]
        model.load_state_dict(ckpt,strict=False)
    else:
        model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max,cfg.use_taa).to(device)
    os.environ['YOLOV10_EXPORT'] = "None"
    # # 打印网络各层的张量维度
    params_num = sum(x.numel() for x in model.parameters())
    print(f"params:{params_num}")
    im = torch.empty((1, 3, cfg.input_height, cfg.input_width)).cuda()
    flops = thop.profile(copy.deepcopy(model), inputs=[im], verbose=True)[0] / 1e9 * 2
    print(f"flops:{flops}GFLOPs")
    # 构建优化器
    print("use SGD optimizer")
    optimizer = optim.SGD(params=model.parameters(),
                          lr=cfg.learn_rate,
                          momentum=0.949,
                          weight_decay=0.0005,
                          )
    # 学习率衰减策略
    scheduler = optim.lr_scheduler.MultiStepLR(optimizer,
                                               milestones=cfg.milestones,
                                               gamma=0.1)


    # 定义损失函数
    loss_function = v10DetectorLoss(device,nc=cfg.category_num, reg_max=cfg.reg_max, reg_scale=cfg.reg_scale,use_taa=cfg.use_taa)

    # 定义验证函数
    evaluation = CocoDetectionEvaluator(cfg.names, device,cfg)

    # 数据集加载
    val_dataset = TensorDataset(cfg.val_txt, cfg.input_width, cfg.input_height, False,label_flag=cfg.label_flag)
    train_dataset = TensorDataset(cfg.train_txt, cfg.input_width, cfg.input_height, True,label_flag=cfg.label_flag)
    # 验证集
    val_dataloader = torch.utils.data.DataLoader(val_dataset,
                                                 batch_size=cfg.batch_size,
                                                 shuffle=False,
                                                 collate_fn=collate_fn,
                                                 num_workers=8,
                                                 drop_last=False,
                                                 persistent_workers=True,
                                                 pin_memory=True,
                                                 )
    # 训练集
    train_dataloader = torch.utils.data.DataLoader(train_dataset,
                                                   batch_size=cfg.batch_size,
                                                   shuffle=True,
                                                   collate_fn=collate_fnt if cfg.use_taa else collate_fn,
                                                   num_workers=8,
                                                   drop_last=True,
                                                   persistent_workers=True,
                                                   pin_memory=True,
                                                   )

    # def lr_sch(step):
    #     if step<3*len(train_dataloader):
    #         return math.pow(step / (3*len(train_dataloader)), 4)
    #     elif step<(cfg.end_epoch+1)*len(train_dataloader):
    #         step=step-3*len(train_dataloader)
    #         return 10**(-4*step/((cfg.end_epoch-2)*len(train_dataloader)))
    #     else:
    #         return 0
    # # 学习率衰减策略
    # scheduler = torch.optim.lr_scheduler.LambdaLR(optimizer, lr_sch)

    # 迭代训练
    batch_num = 0
    print('Starting training for %g epochs...' % cfg.end_epoch)
    scaler = None
    if cfg.amp:
        scaler = GradScaler()
    # torch.set_float32_matmul_precision("high")
    model.eval()
    map50_max = 0
    evaluation.compute_map(val_dataloader, model, cfg)
    for epoch in range(cfg.end_epoch + 1):
        model.train()
        pbar = tqdm(train_dataloader)
        train_statics=[]
        for imgs, targets in pbar:
            # 数据预处理
            imgs = imgs.to(device).float() / 255.0
            targets = targets.to(device)
            if scaler is not None:
                with autocast():
                    # 模型推理
                    preds = model(imgs)
                    # loss计算
                iou, obj, cls, dfl, total = loss_function(preds, targets)
                # 反向传播求解梯度
                scaler.scale(total).backward()
                # 更新模型参数
                scaler.step(optimizer)
                # lr = scheduler.get_last_lr()[0]
                # scheduler.step()
                scaler.update()
            else:
                # 模型推理
                preds = model(imgs)
                # loss计算
                iou, obj, cls, dfl, total = loss_function(preds, targets)
                # 反向传播求解梯度
                total.backward()
                # 更新模型参数
                optimizer.step()
                # lr = scheduler.get_last_lr()[0]
                # scheduler.step()

            optimizer.zero_grad()

            lr = 0
            for g in optimizer.param_groups:
                warmup_num = 5 * len(train_dataloader)
                if batch_num <= warmup_num:
                    scale = math.pow(batch_num / warmup_num, 4)
                    g['lr'] = cfg.learn_rate * scale
                lr = g["lr"]

            # 打印相关训练信息
            info = "Epoch:%d LR:%f IOU:%f Obj:%f Cls:%f Dfl:%f Total:%f" % (
                epoch, lr, iou, obj, cls, dfl, total)
            train_statics.append([float(lr),float(iou),float(obj),float(cls),float(dfl),float(total)])
            pbar.set_description(info)
            batch_num += 1
        train_statics=np.array(train_statics).mean(axis=0)
        info = "\rEpoch:%d LR:%f IOU:%f Obj:%f Cls:%f Dfl:%f Total:%f" % (
            epoch, train_statics[0], train_statics[1], train_statics[2], train_statics[3], train_statics[4], train_statics[5])
        print(info)
        # 模型验证及保存
        if epoch % 10 == 0 and epoch > 0:
            # 模型评估
            model.eval()
            print("computer mAP...")
            # mAP05 = self.evaluation.compute_map(self.train_dataloader, self.model)
            # torch.save(self.model.state_dict(), "checkpoint/weight_AP05_%f_%d-epoch.pth"%(mAP05, epoch))
            mAP05 = evaluation.compute_map(val_dataloader, model, cfg)
            torch.save(model.state_dict(),
                       os.path.join(ckpt_path, "weight_AP05_%f_%d_epoch.pth" % (mAP05, epoch)))
            if mAP05 > map50_max:
                map50_max = mAP05
                torch.save(model.state_dict(),
                           os.path.join(ckpt_path, "best.pth"))

        scheduler.step()




if __name__ == "__main__":
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default="configs/coco_80.yaml", help='.yaml config')
    parser.add_argument('--weight', type=str, default=None, help='.weight config')

    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/train")
    lger.collect_prints(train, opt, lger.result_path)
