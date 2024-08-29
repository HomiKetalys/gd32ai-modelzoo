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
from submodules.yolov10.myolov10t import YOLOv10t,YOLOv10OrtTf
from submodules.FastestDet.module.loss import v10DetectorLoss
import thop
from common_utils.utils import LogSaver

from torch.cuda.amp import autocast
from torch.cuda.amp import GradScaler

# 指定后端设备CUDA&CPU
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")


def val(opt, save_path,val_type=0):
    assert os.path.exists(opt.config), "请指定正确的配置文件路径"

    # 解析yaml配置文件
    cfg = LoadYaml(opt.config)
    print(cfg)

    ckpt_path = os.path.join(save_path, "weights")
    os.makedirs(ckpt_path, exist_ok=True)

    weight_path:str=opt.weight
    # 初始化模型结构
    if weight_path is not None:
        print("load weight from:%s" % weight_path)
        if os.path.isfile(weight_path):
            if weight_path.endswith(".pth"):
                model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max).to(device)
                model.load_state_dict(torch.load(opt.weight))

                for m in model.modules():
                    if getattr(m, "fuse", None) is not None:
                        m.fuse()
                os.environ['YOLOV10_EXPORT'] = "None"
                # # 打印网络各层的张量维度
                params_num = sum(x.numel() for x in model.parameters())
                print(f"params:{params_num}")
                im = torch.empty((1, 3, cfg.input_height, cfg.input_width)).cuda()
                flops = thop.profile(copy.deepcopy(model), inputs=[im], verbose=True)[0] / 1e9 * 2
                print(f"flops:{flops}GFLOPs")
                model.eval()
                val_type=1
            else:
                model = YOLOv10OrtTf(cfg,weight_path)
                val_type=0
        else:
            model = YOLOv10OrtTf(cfg, weight_path)
            val_type = 0
    else:
        model = YOLOv10t(cfg.category_num, cfg.separation, cfg.separation_scale, cfg.reg_max).to(device)
        for m in model.modules():
            if getattr(m, "fuse", None) is not None:
                m.fuse()
        os.environ['YOLOV10_EXPORT'] = "None"
        # # 打印网络各层的张量维度
        params_num = sum(x.numel() for x in model.parameters())
        print(f"params:{params_num}")
        im = torch.empty((1, 3, cfg.input_height, cfg.input_width)).cuda()
        flops = thop.profile(copy.deepcopy(model), inputs=[im], verbose=True)[0] / 1e9 * 2
        print(f"flops:{flops}GFLOPs")
        model.eval()
        val_type = 1

    # 定义验证函数
    evaluation = CocoDetectionEvaluator(cfg.names, device, cfg,val_type)



    # 数据集加载
    val_dataset = TensorDataset(cfg.val_txt, cfg.input_width, cfg.input_height, False,label_flag=cfg.label_flag)
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

    print("computer mAP...")
    # mAP05 = self.evaluation.compute_map(self.train_dataloader, self.model)
    # torch.save(self.model.state_dict(), "checkpoint/weight_AP05_%f_%d-epoch.pth"%(mAP05, epoch))
    mAP05 = evaluation.compute_map(val_dataloader, model, cfg)



if __name__ == "__main__":
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default="modelzoo/coco_80_256/coco_80.yaml", help='.yaml config')
    parser.add_argument('--weight', type=str, default="modelzoo/coco_80_256/weights/best.pth", help='.weight config')

    opt = parser.parse_args()
    lger = LogSaver(opt.config, "results/val")
    lger.collect_prints(val, opt, lger.result_path)