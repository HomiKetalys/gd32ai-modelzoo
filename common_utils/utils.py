import os
import shutil
import sys
from io import StringIO
import onnxruntime as ort
import tensorflow as tf

class multipleSave(object):
    def __getattr__(self, attr, *arguments):
        return self._wrap(attr, *arguments)

    def __init__(self, stdout,myfiles):
        self._stdout=stdout
        self._myfiles = myfiles

    def _wrap(self, attr, *arguments):
        def g(*a, **kw):
            res = getattr(self._stdout, attr, *arguments)(*a, **kw)
            for _p in self._myfiles:
                with open(_p,"a",encoding="utf-8") as f:
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
        os.makedirs(self.result_path,exist_ok=True)
        config_backup_path = os.path.join(self.result_path, config_name)
        shutil.copy(config_path, config_backup_path)
        self.log_path = os.path.join(self.result_path, "log.txt")

    def get_result_path(self):
        return self.result_path

    def collect_prints(self, func, *args, **kwargs):
        old_stdout = sys.stdout
        try:
            sys.stdout = multipleSave(sys.stdout,[self.log_path])
            func(*args, **kwargs)
            sys.stdout = old_stdout
        finally:
            sys.stdout = old_stdout

class modelRuner():
    def __init__(self,model_path:str):
        if model_path.endswith(".tflite"):
            self.model_interpreter = tf.lite.Interpreter(model_path=model_path)
            self.model_interpreter.allocate_tensors()
            self.model_input_details = self.model_interpreter.get_input_details()[0]
            self.model_output_details = self.model_interpreter.get_output_details()
            self.model_type=1
        else:
            self.ort_sess=ort.InferenceSession(model_path)
            self.model_type = 0

    def __call__(self,x):
        if self.model_type==0:
            return self.ort_sess.run(None, {'input.1': x})[0]
        else:
            self.model_interpreter.set_tensor(self.model_input_details['index'], x)
            self.model_interpreter.invoke()
            out_list=[]
            for output_details in self.model_output_details:
                out_list.append(self.model_interpreter.get_tensor(output_details['index']))
            if len(out_list)==1:
                return out_list[0]
            else:
                return out_list
