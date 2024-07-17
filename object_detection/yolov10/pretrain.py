import cv2
import os
import torch
from ultralytics.nn.modules.conv import Conv
from ultralytics.nn.modules.block import SCDown,C2ft
from torch import nn
from torch.utils.data import Dataset,DataLoader
from tqdm import tqdm

class preYOLOv10t(nn.Module):
    def __init__(self, nc=80):
        super(preYOLOv10t, self).__init__()
        self.channels=[24,32,48,96,192]

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
                C2ft(self.channels[2], self.channels[2], 2, True),
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

        self.linear=nn.Linear(self.channels[4],nc)

    def forward(self,x):

        for m in self.stage_list:
            x=m(x)
        x=torch.mean(x,dim=(2,3))
        x=self.linear(x)
        return x

class imagenet(Dataset):
    def __init__(self,path):
        self.data = []
        folders_list=os.listdir(path)
        for i, folder_name in enumerate(folders_list):
            cls_folder = os.path.join(path , folder_name)
            cls_imgs_list = os.listdir(cls_folder)
            for name in cls_imgs_list:
                img_path = os.path.join(cls_folder , name)
                self.data.append([img_path, i])



    def __getitem__(self, item):
        path, label = self.data[item]
        img = cv2.imread(path)
        img = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
        img=cv2.resize(img,(352,352))
        img=img.transpose(2,0,1)
        img=torch.tensor(img,dtype=torch.float32)
        return img, label

    def __len__(self):
        return len(self.data)

if __name__=="__main__":

    tds=imagenet("../../../datasets/imagenet2012/train")
    vds=imagenet("../../../datasets/imagenet2012/validation")
    tdl=DataLoader(tds,96,True,num_workers=8,pin_memory=True,drop_last=True,persistent_workers=True)
    vdl=DataLoader(vds,96,False,num_workers=8,pin_memory=True,persistent_workers=True)
    model=preYOLOv10t(1000)
    opt=torch.optim.SGD(model.parameters(),lr=0.01,momentum=0.9,weight_decay=0.005)
    lrs=torch.optim.lr_scheduler.MultiStepLR(opt,[100,200,250],gamma=0.1)
    lossfun=nn.CrossEntropyLoss()
    model.cuda()
    model.eval()
    for epoch in range(0,300):
        model.train()
        tdlb=tqdm(tdl)
        for x,y in tdlb:
            x=x.cuda()
            y=y.cuda()
            pred=model(x)
            loss=lossfun(pred,y)
            loss.backward()
            opt.step()
            opt.zero_grad()

        model.eval()
        vdlb = tqdm(vdl)
        tp=0
        all=0
        for x,y in vdlb:
            x=x.cuda()
            y=y.cuda()
            with torch.no_grad():
                pred=model(x)
            loss=lossfun(x,y)
            x=torch.argmax(x,dim=1)
            tp+=torch.sum(x==y)
            all+=x.shape[0]
        print(f"acc:{tp/all}")
        lrs.step()
