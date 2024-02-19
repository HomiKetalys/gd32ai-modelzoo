from os.path import join
import argparse
import numpy as np
import os
import random
from modified_files import kmeans


# modified from main of genanchors
def main():
    if not os.path.exists(args.output_dir):
        os.mkdir(args.output_dir)

    f = open(args.traintxt)

    lines = [line.rstrip('\n') for line in f.readlines()]

    annotation_dims = []

    size = np.zeros((1, 1, 3))
    for line in lines:

        # line = line.replace('images','labels')
        # line = line.replace('img1','labels')
        # modified
        # line = line.replace('images', "labels")

        line = line.replace('.jpg', '.txt')
        line = line.replace('.png', '.txt')
        # modified
        #
        if not os.path.exists(line):
            if len(args.label_flag) > 0:
                line = line.replace('images', args.label_flag)
        if not os.path.isabs(line):
            line = os.path.join(os.path.split(args.traintxt)[0], line)
        print(line)
        f2 = open(line)
        for line in f2.readlines():
            line = line.rstrip('\n')
            w, h = line.split(' ')[3:]
            # print(w,h)
            annotation_dims.append(tuple(map(float, (w, h))))
    annotation_dims = np.array(annotation_dims)

    eps = 0.005
    width_in_cfg_file = args.input_width
    height_in_cfg_file = args.input_height

    if args.num_clusters == 0:
        for num_clusters in range(1, 11):  # we make 1 through 10 clusters
            anchor_file = join(args.output_dir, 'anchors%d.txt' % (num_clusters))

            indices = [random.randrange(annotation_dims.shape[0]) for i in range(num_clusters)]
            centroids = annotation_dims[indices]
            kmeans(annotation_dims, centroids, eps, anchor_file, width_in_cfg_file, height_in_cfg_file)
            print('centroids.shape', centroids.shape)
    else:
        anchor_file = join(args.output_dir, 'anchors%d.txt' % (args.num_clusters))
        indices = [random.randrange(annotation_dims.shape[0]) for i in range(args.num_clusters)]
        centroids = annotation_dims[indices]
        kmeans(annotation_dims, centroids, eps, anchor_file, width_in_cfg_file, height_in_cfg_file)
        print('centroids.shape', centroids.shape)


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
    main()
