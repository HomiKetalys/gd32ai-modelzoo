import argparse
import os.path

import PIL
import cv2
import numpy as np
import torch

from common_utils.utils import LogSaver
from submodules.ml_fastvit.validate import cfg_analyzer
from PIL import Image
from PIL import ImageOps
from torchvision import transforms
from timm.data.transforms import ToNumpy

def test(cfg_path,model_path,img_path,result_path,eval_type,label_file=None,img_size=None):
    model,_, data_config = cfg_analyzer(cfg_path, model_path, eval_type,img_size)[0:3]
    c_name=os.listdir(img_path)
    cnum=0
    all=0
    for i,cn in enumerate(c_name):
        # igp=img_path
        igp=os.path.join(img_path,cn)
        if os.path.isfile(igp):
            img_path_list=[igp]
        else:
            img_path_list=[os.path.join(igp,name) for name in os.listdir(igp)]
        results=[]
        for p in img_path_list:
            img=Image.open(p)
            tfs=transforms.Compose(
                [
                    transforms.Resize((192,192)),
                    # transforms.CenterCrop((192,192)),
                    ToNumpy(),
                ]
            )
            img=tfs(img)
            #
            img=img/255
            img=(img-np.array((0.485, 0.456, 0.406))[:,None,None])/np.array((0.229, 0.224, 0.225))[:,None,None]
            x=img[None,:,:,:]
            x=torch.tensor(x)
            preds=model(x).cpu().numpy()
            idx=np.argmax(preds)
            results.append(idx)
            if idx==i:
                cnum+=1
            all+=1
        print(f"{i}:{cnum/all}")
        cnum = 0
        all = 0
    # print(cnum/all)
    a=np.array(results)
    print(np.sum(a==21)/len(a))
    print(results)

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='results/train/20240323-183131-fastvit_t4-224/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="results/deploy/food-101_0003/tflite",
                        help='The path of the .pth model to be transformed')
    parser.add_argument('--img', type=str, default='../../../datasets/food-101/validation',
                        help='The path of test image')
    parser.add_argument('--eval_type', type=int, default=1,
                        help='eval type')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=(192,192),
                        help='Specify the image size of the input for the model.the img size in config is default')
    opt = parser.parse_args()
    lger=LogSaver(opt.data,"results/test")
    lger.collect_prints(test,opt.data,opt.model_path,opt.img,lger.result_path,opt.eval_type,img_size=opt.img_size)



