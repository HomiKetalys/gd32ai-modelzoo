import argparse

from common_utils.utils import LogSaver
from submodules.ml_fastvit.validate import validate

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='results/train/20240318-110647-fastvit_t4-224/food-101.yaml',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default="results/export/args_0010/tflite",
                        help='The path of the model')
    # parser.add_argument('--model_path', type=str, default='./modelzoo',
    #                     help='The path of the model')
    parser.add_argument('--img_size', type=int,nargs='+' ,default=(192,192),
                        help='Specify the image size of the input for the exported model.the img size in config is default')
    opt = parser.parse_args()
    lger=LogSaver(opt.data,"results/eval")
    lger.collect_prints(validate,opt.data,opt.model_path,1,opt.img_size)