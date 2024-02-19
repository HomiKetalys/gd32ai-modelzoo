import os
import random

import cv2
import logging
import numpy as np
import onnxruntime as ort
import tensorflow as tf
from ..onnx2tflite.utils.dataloader import RandomLoader

LOG = logging.getLogger("Quantization DataLoder :")

class ImageLoader(object):
    '''
        generate data for quantization from image datas.
        img_quan_data = (img - mean)/std, it's important for accuracy of model.
    '''
    VALID_FORMAT = ['.jpg', '.png', '.jpeg']

    def __init__(self, img_root, target_size, mean=None, std=None, separation=0, separation_scale=2,
                 onnx_path=None) -> None:
        if std is None:
            std = [58.395, 57.12, 57.375]
        if mean is None:
            mean = [123.675, 116.28, 103.53]
        assert os.path.exists(img_root), F"{img_root} is not exists, please check!"
        self.fns = os.listdir(img_root)
        self.fns = list(filter(lambda fn: os.path.splitext(fn)[-1].lower() in self.VALID_FORMAT, self.fns))
        self.nums = len(self.fns)
        assert self.nums > 0, f"No images detected in {img_root}."
        if self.nums > 100:
            LOG.warning(f"{self.nums} images detected, the number of recommended images is less than 100.")
            random.shuffle(self.fns)
            self.fns=self.fns[:100]
            self.nums=100
        else:
            LOG.info(f"{self.nums} images detected.")
        self.fns = [os.path.join(img_root, fn) for fn in self.fns]

        self.batch, self.size = target_size[0], target_size[1:-1]
        if isinstance(mean, list):
            mean = np.array(mean, dtype=np.float32)
        if isinstance(std, list):
            std = np.array(std, dtype=np.float32)
        self.mean, self.std = mean, std

        self.separation = separation
        self.separation_scale = separation_scale
        self.onnx_path = onnx_path

    def __iter__(self):
        self.index = 0
        return self

    def __next__(self):
        if self.index >= self.nums:
            raise StopIteration()

        _input = cv2.imread(self.fns[self.index])
        h,w=self.size[0],self.size[1]


        if self.separation != 0:

            if self.separation > 0:
                _input = cv2.resize(_input, (self.separation_scale * w, self.separation_scale * h))
                _input = _input.astype(np.float32)
                _input = _input / 255
                i = random.randint(0, self.separation_scale-1)
                j = random.randint(0, self.separation_scale-1)
                _input = _input[
                         i * h :(i + 1) * h ,
                         j * w :(j + 1) * w ,
                         :
                         ]
            else:
                separation=-self.separation
                _input = cv2.resize(_input, (2**separation * w, 2**separation * h))
                _input = _input.astype(np.float32)
                _input = _input / 255
                ort_sess1 = ort.InferenceSession(self.onnx_path.replace("post","front"))
                _input = _input[None, :, :, :]
                _input = np.transpose(_input, (0, 3, 1, 2))
                out_list=[]
                for r in range(0,self.separation_scale):
                    for c in range(0,self.separation_scale):
                        im=_input[:,:,
                           r*2**separation *h//self.separation_scale:(r+1)*2**separation *h//self.separation_scale,
                           c*2**separation *w//self.separation_scale:(c+1)*2**separation *w//self.separation_scale,
                           ]
                        outputs = ort_sess1.run(None, {'input.1': im})
                        outputs = outputs[0][0]
                        outputs = np.transpose(outputs, (1, 2, 0))
                        out_list.append(outputs)
                h,w,c=out_list[0].shape[:3]
                out = np.zeros((h *self.separation_scale, w *self.separation_scale, c), dtype=np.float32)
                id=0
                for r in range(0,self.separation_scale):
                    for c in range(0,self.separation_scale):
                        out[r*h:(r+1)*h,c*w:(c+1)*w,:]=out_list[id]
                        id+=1
                _input=out
        else:
            _input = cv2.resize(_input, (w, h))
            _input = _input.astype(np.float32)
            if self.mean is not None:
                _input-=self.mean
            if self.std is not None:
                _input/=self.std

        _input = np.expand_dims(_input, axis=0)
        if self.batch > 1:
            _input = np.repeat(_input, self.batch, axis=0).astype(np.float32)

        self.index += 1
        return [_input]