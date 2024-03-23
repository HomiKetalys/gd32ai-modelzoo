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

def test(cfg_path,model_path,img_path,result_path,eval_type,label_file=None):
    model,_, data_config = cfg_analyzer(cfg_path, model_path, eval_type)[0:3]
    # c_name=os.listdir(img_path)
    # cnum=0
    # all=0
    # for i,cn in enumerate(c_name):
    igp=img_path
    if os.path.isfile(igp):
        img_path_list=[igp]
    else:
        img_path_list=[os.path.join(img_path,name) for name in os.listdir(igp)]
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
    #         if idx==i:
    #             cnum+=1
    #         all+=1
    # print(cnum/all)
    print(results)

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='results/train/20240318-110647-fastvit_t4-224/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="results/export/args_0011/tflite",
                        help='The path of the .pth model to be transformed')
    parser.add_argument('--img', type=str, default='test_imgs/123.png',
                        help='The path of test image')
    parser.add_argument('--eval_type', type=int, default=1,
                        help='eval type')
    opt = parser.parse_args()
    lger=LogSaver(opt.data,"results/test")
    lger.collect_prints(test,opt.data,opt.model_path,opt.img,lger.result_path,opt.eval_type)



