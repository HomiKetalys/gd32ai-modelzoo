import os
import cv2
import numpy as np
import torch
from ..Yolo_FastestV2_main import TensorDataset,img_aug,collate_fn


class TensorDatasetSp(TensorDataset):
    def __init__(self, path, img_size_width=352, img_size_height=352, label_flag="",imgaug=False):
        self.modified_init(path, img_size_width, img_size_height,imgaug)
        # check label files
        with open(self.path, 'r') as f:
            for line in f.readlines():
                data_path = line.strip()
                if not os.path.isabs(data_path):
                    data_path = os.path.join(os.path.split(self.path)[0], data_path)
                if os.path.exists(data_path):
                    img_type = data_path.split(".")[-1]
                    if img_type not in self.img_formats:
                        raise Exception("img type error:%s" % img_type)
                    else:
                        label_path = os.path.splitext(data_path)[0] + ".txt"
                        if os.path.exists(label_path):
                            self.data_list.append((data_path, label_path))
                        else:
                            if len(label_flag)>0:
                                label_path=label_path.replace("images",label_flag)
                            if os.path.exists(label_path):
                                self.data_list.append((data_path, label_path))
                            else:
                                raise Exception("%s is not exist" % label_path)
                else:
                    raise Exception("%s is not exist" % data_path)

    def __getitem__(self, index):
        img_path, label_path = self.data_list[index]
        return self.modified_getitem(img_path, label_path)

    # modify from __getitem__ of TensorDataset
    def modified_getitem(self, img_path, label_path):
        # 归一化操作
        img = cv2.imread(img_path)
        # img=cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
        img = cv2.resize(img, (self.img_size_width, self.img_size_height), interpolation=cv2.INTER_LINEAR)
        # 数据增强
        if self.imgaug == True:
            img = img_aug(img)
        img = img.transpose(2, 0, 1)

        # 加载label文件
        if os.path.exists(label_path):
            label = []
            with open(label_path, 'r') as f:
                for line in f.readlines():
                    l = line.strip().split(" ")
                    label.append([0, l[0], l[1], l[2], l[3], l[4]])
            label = np.array(label, dtype=np.float32)

            if label.shape[0]:
                assert label.shape[1] == 6, '> 5 label columns: %s' % label_path
                # assert (label >= 0).all(), 'negative labels: %s'%label_path
                # assert (label[:, 1:] <= 1).all(), 'non-normalized or out of bounds coordinate labels: %s'%label_path
        else:
            raise Exception("%s is not exist" % label_path)

        return torch.from_numpy(img), torch.from_numpy(label)

    # modified from __init__ of TensorDataset
    def modified_init(self, path, img_size_width, img_size_height, imgaug):
        assert os.path.exists(path), "%s文件路径错误或不存在" % path
        self.path = path
        self.data_list = []
        self.img_size_width = img_size_width
        self.img_size_height = img_size_height
        self.img_formats = ['bmp', 'jpg', 'jpeg', 'png']
        self.imgaug = imgaug
