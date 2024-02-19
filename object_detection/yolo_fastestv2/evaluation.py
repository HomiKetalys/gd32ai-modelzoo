import argparse
from modified_files import evaluation
from common_utils.utils import LogSaver

paths=[
    'modified_files/Yolo_FastestV2_main/modelzoo/coco2017-0.241078ap-model.pth',
    'modelzoo/coco_sp_0000/coco-20-epoch-0.199881ap-model.pth',
    'modelzoo/train/coco_sp_0002/coco-219-epoch-0.157780ap-model.pth'
]

if __name__ == '__main__':
    # 指定训练配置文件
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=str, default='modelzoo/coco_sp_0002/coco_sp.data',
                        help='Specify training profile *.data')
    parser.add_argument('--model_path', type=str, default=paths[2],
                        help='The path of the model')
    # parser.add_argument('--model_path', type=str, default='./modelzoo',
    #                     help='The path of the model')
    opt = parser.parse_args()
    lger=LogSaver(opt.data,"results/eval")
    lger.collect_prints(evaluation,opt.data,opt.model_path,0)



