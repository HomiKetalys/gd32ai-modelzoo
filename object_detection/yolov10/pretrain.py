import copy
import multiprocessing

import numpy as np
import thop
import torchvision

import cv2
import os
import torch
from torch.cuda.amp import autocast
from torch.cuda.amp import GradScaler

from ultralytics.nn.modules.conv import Conv
from ultralytics.nn.modules.block import C2f, Bottleneck, C2ft
from torch import nn
from torch.utils.data import Dataset, DataLoader
from tqdm import tqdm


class preYOLOv10t(nn.Module):
    def __init__(self, nc=80):
        super(preYOLOv10t, self).__init__()
        self.channels = [24, 32, 48, 96, 192]

        self.stage_list = nn.ModuleList([
            nn.Sequential(
                Conv(3, self.channels[0], 3, 2),
            ),
            nn.Sequential(
                SCDown(self.channels[0], self.channels[1], 3, 2),
                C2ft(self.channels[1], self.channels[1], 1, True),
            ),
            nn.Sequential(
                SCDown(self.channels[1], self.channels[2], 3, 2),
                C2ft(self.channels[2], self.channels[2], 1, True),
            ),
            nn.Sequential(
                SCDown(self.channels[2], self.channels[3], 3, 2),
                C2ft(self.channels[3], self.channels[3], 2, True),
            ),
            nn.Sequential(
                SCDown(self.channels[3], self.channels[4], 3, 2),
                C2ft(self.channels[4], self.channels[4], 1, True),
            ),
        ]
        )

        # ckpt = torch.load("results/epoch_0029_acc_0.4164.pth")
        # self.load_state_dict(ckpt,strict=False)
        self.stage_list[0].insert(0, nn.Identity())
        # torch.save(self.state_dict(),"backbone.pth")
        self.final_conv = nn.Sequential(
            nn.Conv2d(self.channels[4], 1024, 1, 1, bias=False),
            nn.BatchNorm2d(1024),
            nn.ReLU(),
        )

        self.linear = nn.Linear(1024, nc)

    def forward(self, x):
        for m in self.stage_list:
            x = m(x)
        x = self.final_conv(x)
        x = torch.mean(x, dim=(2, 3))
        x = self.linear(x)
        return x


class imagenet(Dataset):
    def __init__(self, path):
        path = os.path.abspath(path)
        self.root_path = os.path.split(path)[0]
        cache_path = os.path.join(self.root_path, "cache")
        self.data = []
        folders_list = os.listdir(path)
        for i, folder_name in enumerate(folders_list):
            cls_folder = os.path.join(path, folder_name)
            cls_imgs_list = os.listdir(cls_folder)
            for name in cls_imgs_list:
                img_path = os.path.join(cls_folder, name)
                img_cache_path = os.path.join(cache_path, name)
                self.data.append([img_cache_path, i])


    def __getitem__(self, item):
        path, label = self.data[item]
        img = cv2.imread(path)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        img = img.transpose(2, 0, 1)
        img = torch.from_numpy(img)
        return img, label

    def __len__(self):
        return len(self.data)

    def create_cache_img(self, data):
        cache_path = os.path.join(self.root_path, "cache")
        for img_path, i in data:
            name = os.path.split(img_path)[1]
            img_cache_path = os.path.join(cache_path, name)
            if not os.path.exists(img_cache_path):
                img = cv2.imread(img_path)
                img = cv2.resize(img, (256, 256))
                img = img[16:240, 16:240, :]
                cv2.imwrite(img_cache_path, img)

    def create_cache(self):
        cache_path = os.path.join(self.root_path, "cache")
        os.makedirs(cache_path, exist_ok=True)
        data = []
        for img_path, i in self.data:
            name = os.path.split(img_path)[1]
            img_cache_path = os.path.join(cache_path, name)
            data.append([img_cache_path, i])

        l = len(self.data) // 8
        if False:
            process = multiprocessing.Pool()
            for i in range(0, 8):
                process.apply_async(self.create_cache_img, args=(self.data[i * l:min((i + 1) * l, len(self.data))],))
            process.close()
            process.join()
        else:
            self.create_cache_img(self.data)
        self.data = data

def train():
    model = preYOLOv10t(1000)
    im = torch.empty((1, 3, 256, 256))
    flops = thop.profile(copy.deepcopy(model), inputs=[im], verbose=True)[0] / 1e9 * 2
    print(f"flops:{flops}GFLOPs")
    # exit()
    print(f"params:{sum(x.numel() for x in model.parameters())}")

    tds = imagenet("../../../datasets/imagenet2012/train")
    vds = imagenet("../../../datasets/imagenet2012/validation")
    # tds.create_cache()
    # vds.create_cache()
    tdl = DataLoader(tds, 512, True, num_workers=8, pin_memory=True, drop_last=True, persistent_workers=True)
    vdl = DataLoader(vds, 512, False, num_workers=8, pin_memory=True, persistent_workers=True)

    opt = torch.optim.SGD(model.parameters(), lr=0.5,weight_decay=4e-5)
    lrs = torch.optim.lr_scheduler.LambdaLR(opt, lambda step: (1.0 - step / 150000) if step <= 150000 else 0)
    lossfun = nn.CrossEntropyLoss()
    model.cuda()
    model.eval()
    scaler = GradScaler()
    avg = torch.tensor((0.485, 0.456, 0.406), dtype=torch.float32)[None, :, None, None].cuda()
    std = torch.tensor((0.229, 0.224, 0.225), dtype=torch.float32)[None, :, None, None].cuda()
    step=0
    for epoch in range(0, 120):
        model.train()
        tdlb = tqdm(tdl)
        losses = []
        tps = []
        nums = []
        for x, y in tdlb:

            x = (x.cuda() / 255.0 -avg)/std
            y = y.cuda()
            with autocast():
                pred = model(x)
            loss = lossfun(pred, y)
            scaler.scale(loss).backward()
            # loss.backward()

            if (step+1)%2==0:
                lrs.step()

                scaler.step(opt)
                scaler.update()

                # opt.step()

                opt.zero_grad()

            pred = torch.argmax(pred, dim=1)
            tp = torch.sum(pred == y)
            num = x.shape[0]

            losses.append(float(loss))
            tps.append(int(tp))
            nums.append(int(num))
            step+=1
            tdlb.set_postfix_str(
                f"state:train,batch_loss:{loss:.4f},batch_acc:{tp / num:.4f},lr:{opt.param_groups[0]['lr']:.6f}")

        print(f"\rstate:train epoch:{epoch:04d} loss:{sum(losses) / len(losses):.4f} acc:{sum(tps) / sum(nums):.4f}")

        model.eval()
        vdlb = tqdm(vdl)
        losses = []
        tps = []
        nums = []
        for x, y in vdlb:
            x = (x.cuda() / 255.0 -avg)/std
            y = y.cuda()
            with torch.no_grad():
                pred = model(x)
            loss = lossfun(pred, y)
            pred = torch.argmax(pred, dim=1)
            tp = torch.sum(pred == y)
            num = x.shape[0]

            losses.append(float(loss))
            tps.append(int(tp))
            nums.append(int(num))
            vdlb.set_postfix_str(f"state:val,batch_loss:{loss:.4f},batch_acc:{tp / num:.4f}")

        print(f"\rstate:val epoch:{epoch:04d} loss:{sum(losses) / len(losses):.4f} acc:{sum(tps) / sum(nums):.4f}")
        torch.save(model.state_dict(), f"results/epoch_{epoch:04d}_acc_{sum(tps) / sum(nums):.4f}.pth")

if __name__ == "__main__":
    train()

