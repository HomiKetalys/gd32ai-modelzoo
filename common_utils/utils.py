import os
import shutil
import sys
from io import StringIO
from typing import Union, Callable

import cv2
import matplotlib.pyplot as plt
import numpy as np
import onnxruntime as ort
import tensorflow as tf
from PIL import Image
from timm.data import ToNumpy
from torchvision import transforms


class multipleSave(object):
    def __getattr__(self, attr, *arguments):
        return self._wrap(attr, *arguments)

    def __init__(self, stdout, myfiles):
        self._stdout = stdout
        self._myfiles = myfiles

    def _wrap(self, attr, *arguments):
        def g(*a, **kw):
            res = getattr(self._stdout, attr, *arguments)(*a, **kw)
            for _p in self._myfiles:
                with open(_p, "a", encoding="utf-8") as f:
                    getattr(f, attr, *arguments)(*a, **kw)
            return res

        return g


class LogSaver():
    def __init__(self, config_path, save_folder_path):
        config_name = os.path.split(config_path)[-1]
        folder_name = ".".join(config_name.split(".")[:-1])
        name_index = 0
        folder_path = ""
        if not os.path.exists(save_folder_path):
            os.makedirs(save_folder_path)
        while name_index < 10000:
            folder_name_ = folder_name + f"_{name_index:04d}"
            folder_path = os.path.join(save_folder_path, folder_name_)
            if os.path.exists(folder_path):
                name_index += 1
            else:
                os.mkdir(folder_path)
                break
        assert name_index < 10000, "too many results existed"
        self.result_path = folder_path
        os.makedirs(self.result_path, exist_ok=True)
        config_backup_path = os.path.join(self.result_path, config_name)
        shutil.copy(config_path, config_backup_path)
        self.log_path = os.path.join(self.result_path, "log.txt")

    def get_result_path(self):
        return self.result_path

    def collect_prints(self, func, *args, **kwargs):
        old_stdout = sys.stdout
        try:
            sys.stdout = multipleSave(sys.stdout, [self.log_path])
            func(*args, **kwargs)
            sys.stdout = old_stdout
        finally:
            sys.stdout = old_stdout


class tfOrtModelRuner():
    def __init__(self, model_path: str):
        if model_path.endswith(".tflite"):
            self.model_interpreter = tf.lite.Interpreter(model_path=model_path)
            self.model_interpreter.allocate_tensors()
            self.model_input_details = self.model_interpreter.get_input_details()[0]
            self.model_output_details = self.model_interpreter.get_output_details()
            self.model_type = 1
        else:
            self.ort_sess = ort.InferenceSession(model_path)
            self.model_type = 0

    def __call__(self, x):
        if self.model_type == 0:
            return self.ort_sess.run(None, {'input.1': x})[0]
        else:
            self.model_interpreter.set_tensor(self.model_input_details['index'], x)
            self.model_interpreter.invoke()
            out_list = []
            for output_details in self.model_output_details:
                out_list.append(self.model_interpreter.get_tensor(output_details['index']))
            if len(out_list) == 1:
                return out_list[0]
            else:
                return out_list


def l2(x, y):
    return np.sqrt(np.sum((x - y) ** 2, 1))


def silhouette_coef(x, indexs, cluster_num, center, criterion):
    dis_all = 0
    for i in range(0, cluster_num):
        x_in = x[indexs == i, :]
        dis_out = np.zeros((x_in.shape[0]))
        for j in range(0, x_in.shape[0]):
            sample = x_in[j:j + 1, :]
            idx = np.ones((cluster_num,), dtype="int") == 1
            idx[i] = False
            center_out = center[idx, :]
            dis = criterion(center_out, sample)
            idx = np.argmin(dis)
            if idx >= i:
                idx += 1
            x_out = x[indexs == idx, :]
            dis = criterion(x_out, sample)
            dis_out[j] = np.mean(dis)
        x_in_r0 = np.repeat(x_in[:, :, None], repeats=x_in.shape[0], axis=2)
        x_in_r1 = x_in_r0.transpose((2, 1, 0))
        dis_in = criterion(x_in_r0, x_in_r1)
        dis_in = np.mean(dis_in, axis=1)
        dis = (dis_out - dis_in) / np.maximum(dis_in, dis_out)
        dis_all += np.sum(dis)
    dis_all = dis_all / x.shape[0]
    return dis_all


class kmeans():
    def __init__(self, cluster_num: Union[str, int], criterion: Callable = l2):
        self.cluster_num = cluster_num
        self.criterion = criterion
        self.max_iter_num = 100

    def fit_(self, x, cluster_num):
        final_center = None
        final_dis = -1
        for i in range(0, 20):
            center_index = np.random.randint(0, x.shape[0], (cluster_num,))
            center = np.copy(x[center_index, :])
            total_dis = 0
            for iter_num in range(0, self.max_iter_num):
                center_r = np.repeat(center[:, :, None], repeats=x.shape[0], axis=2)
                x_r = np.repeat(x[:, :, None], repeats=cluster_num, axis=2)
                center_r = np.transpose(center_r, (2, 1, 0))
                dis = self.criterion(x_r, center_r)
                indexs = np.argmin(dis, axis=1)
                for i in range(0, cluster_num):
                    center[i, :] = np.mean(x[indexs == i, :], axis=0)
                if iter_num + 1 == self.max_iter_num:
                    # total_dis = np.sum(np.reshape(dis,(-1,1))[np.arange(0, x.shape[0]) * cluster_num + indexs])
                    total_dis = silhouette_coef(x, indexs, cluster_num, center, self.criterion)
            if total_dis > final_dis:
                final_dis = total_dis
                final_center = center
        return final_center, final_dis

    def fit(self, x):
        x = np.array(x)
        if isinstance(self.cluster_num, int):
            self.center, _ = self.fit_(x, self.cluster_num)
        elif self.cluster_num == "auto":
            dis_list = []
            center_list = []
            for i in range(1, 20):
                center, dis = self.fit_(x, i + 1)
                dis_list.append(dis)
                center_list.append(center)
            dis_list = np.array(dis_list)
            print(dis_list)
            index = np.argmax(dis_list)
            self.center = center_list[index]
            self.cluster_num = index + 2

    def predict(self, x):
        center = np.repeat(self.center[:, :, None], repeats=x.shape[0], axis=2)
        x = np.repeat(x[:, :, None], repeats=self.cluster_num, axis=2)
        center = np.transpose(center, (2, 1, 0))
        dis = self.criterion(x, center)
        return np.argmin(dis, axis=1)


class GMM():
    def __init__(self, cluster_num: Union[str, int] = "auto", criterion: Callable = l2):
        self.cluster_num=cluster_num
        self.criterion=criterion
        self.max_iter_num=100


    def fit_(self, x, cluster_num):
        x_mean=np.mean(x,axis=0,keepdims=True)
        x_var=np.var(x,axis=0,keepdims=True)
        x=(x-x_mean)/np.sqrt(x_var+1e-8)
        self.x_mean=x_mean
        self.x_var=x_var
        mean=np.random.randn(cluster_num,x.shape[1])
        covar_matrix=np.diag(np.ones(x.shape[1]))[None,:,:].repeat(cluster_num,0)
        pz=np.random.random((cluster_num,))
        pz=pz/np.sum(pz)
        for n in range(0,self.max_iter_num):
            p_xj_1zi_list=[]
            for i in range(0,cluster_num):
                cvmati = np.linalg.inv(covar_matrix[i])
                cvmatd = np.linalg.det(covar_matrix[i])
                p_xj_1zi = np.exp(-0.5 * np.sum((x - mean[i,:]) * ((x - mean[i,:]) @cvmati ), axis=1,keepdims=True)) / (
                        np.sqrt(2 * np.pi * cvmatd) ** x.shape[1])
                p_xj_1zi_list.append(p_xj_1zi)
            p_xj_1zi=np.concatenate(p_xj_1zi_list,axis=1)
            Qjzi = p_xj_1zi / (p_xj_1zi @ pz[:,None])
            pz = np.mean(Qjzi, axis=0)
            for i in range(0, cluster_num):
                mean[i,:]=(Qjzi[:,i]@x)/np.sum(Qjzi[:,i])
                xt=np.sqrt(Qjzi[:,i:i+1])*(x-mean[i,:])
                covar_matrix[i,:,:]=(np.transpose(xt)@xt)/np.sum(Qjzi[:,i])
        return mean,covar_matrix,pz

    def fit(self, x):
        x = np.array(x)
        if isinstance(self.cluster_num, int):
            self.mean,self.covar_matrix,self.pz = self.fit_(x, self.cluster_num)
        elif self.cluster_num == "auto":
            return

    def predict(self, x):
        x = (x - self.x_mean) / np.sqrt(self.x_var + 1e-8)
        p_xj_1zi_list = []
        for i in range(0, self.cluster_num):
            cvmati = np.linalg.inv(self.covar_matrix[i])
            cvmatd = np.linalg.det(self.covar_matrix[i])
            p_xj_1zi = np.exp(-0.5 * np.sum((x - self.mean[i, :]) * ((x - self.mean[i, :]) @ cvmati), axis=1, keepdims=True)) / (
                    np.sqrt(2 * np.pi * cvmatd) ** x.shape[1])
            p_xj_1zi_list.append(p_xj_1zi)
        p_xj_1zi = np.concatenate(p_xj_1zi_list, axis=1)
        Qjzi = p_xj_1zi / (p_xj_1zi @ self.pz[:, None])
        return np.argmax(Qjzi, axis=1)


def cos(x, y):
    return -np.sum(x * y, axis=1) / (np.sqrt(np.sum(x ** 2, axis=1)) * np.sqrt(np.sum(y ** 2, axis=1)))


def radius(x, y):
    return np.abs(np.sqrt(np.sum(x ** 2, axis=1)) - np.sqrt(np.sum(y ** 2, axis=1)))


def kmeans_main():
    N = "auto"
    x1 = np.array([0, 0]) + np.random.randn(180,2)
    x2 = np.array([2,2]) + np.random.randn(180,2)
    x3 = np.array([3, 3]) + np.random.randn(180,2)
    x = np.concatenate((x1, x2, x3), axis=0)

    model = kmeans(N, criterion=l2)
    model.fit(x)
    print(f"num:{model.cluster_num}")
    pred = model.predict(x)
    color = np.random.randint(0, 256, (model.cluster_num, 3))
    color_map = color[pred, :]
    plt.scatter(x[:, 0], x[:, 1], c=color_map / 255)
    plt.show()
    input()

def gmm_main():
    N = "auto"
    x1 = np.array([0, 0]) + np.random.randn(180,2)
    x2 = np.array([2, 2]) + np.random.randn(180,2)
    x3 = np.array([4, 4]) + np.random.randn(180,2)
    x = np.concatenate((x1, x2, x3), axis=0)

    model = GMM(3, criterion=l2)
    model.fit(x)
    print(f"num:{model.cluster_num}")
    pred = model.predict(x)
    color = np.random.randint(0, 256, (model.cluster_num, 3))
    color_map = color[pred, :]
    plt.scatter(x[:, 0], x[:, 1], c=color_map / 255)
    plt.show()
    input()

def img2rgb565code(img_path,img_size=(192,192)):
    img = Image.open(img_path)
    tfs = transforms.Compose(
        [
            transforms.Resize(int(img_size[0]/0.875)),
            transforms.CenterCrop(img_size),
            ToNumpy(),
        ]
    )
    img = tfs(img)
    img=np.transpose(img,(1,2,0)).astype("int")
    img_rgb565=(img[:,:,0]//2**3)*2**11+(img[:,:,1]//2**2)*2**5+img[:,:,2]//2**3
    img_rgb565=img_rgb565.astype("uint16")
    img_rgb565=np.rot90(img_rgb565,2)
    img_rgb565_=img_rgb565.tobytes()
    code=f"const u8 example_img[{img_size[0]}*{img_size[1]}*2]="+"{"
    for i in range(0,img_size[0]*img_size[1]*2):
        a="0x"+img_rgb565_[i:i+1].hex()
        if i!=img_size[0]*img_size[1]*2-1:
            a+=","
        else:
            a+="};"
        code+=a
    return code





if __name__ == "__main__":
    print(img2rgb565code("../image_classification/ml_fastvit/test_imgs/840315.jpg",(192,192)))