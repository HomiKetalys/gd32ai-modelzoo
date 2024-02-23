import argparse
from common_utils.utils import LogSaver
from submodules.Yolo_FastestV2.test import test

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='modelzoo/coco_sp_0004/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="modelzoo/coco_sp_0004/coco-220-epoch-0.213236ap-model.pth",
                        help='The path of the .pth model to be transformed')
    parser.add_argument('--img', type=str, default='test_imgs/000139.jpg',
                        help='The path of test image')
    parser.add_argument('--eval_type', type=int, default=0,
                        help='eval type')
    opt = parser.parse_args()
    lger=LogSaver(opt.data,"results/test")
    lger.collect_prints(test,opt.data,opt.model_path,opt.img,lger.result_path,opt.eval_type)