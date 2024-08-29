import argparse
from common_utils.utils import LogSaver
from submodules.Yolo_FastestV2.test import test

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='results/deploy/coco_sp_0001/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--weight', type=str, default="results/deploy/coco_sp_0001/tflite",
                        help='The path of the .pth model to be transformed')
    parser.add_argument('--img', type=str, default='test_imgs/000139.jpg',
                        help='The path of test image')
    parser.add_argument('--eval_type', type=int, default=1,
                        help='eval type')
    opt = parser.parse_args()
    lger=LogSaver(opt.config,"results/test")
    lger.collect_prints(test,opt.config,opt.weight,opt.img,lger.result_path,opt.eval_type)