from os.path import join
import argparse
import numpy as np
import os
import random
from submodules.Yolo_FastestV2.genanchors import genanchors

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--traintxt', default='../../../datasets/abnormal_drive_0/train0.txt',
                        help='path to traintxt\n')
    parser.add_argument('--output_dir', default='./', type=str,
                        help='Output anchor directory\n')
    parser.add_argument('--label_flag', default='', type=str,
                        help='label flag\n')
    parser.add_argument('--num_clusters', default=6, type=int,
                        help='number of clusters\n')

    parser.add_argument('--input_width', default=256, type=int,
                        help='model input width\n')

    parser.add_argument('--input_height', default=256, type=int,
                        help='model input height\n')

    args = parser.parse_args()
    genanchors(args)
