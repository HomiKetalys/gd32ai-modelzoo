import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

import tensorflow as tf
from tensorflow import keras
from onnx import numpy_helper
from ..utils.dataloader_sp import RandomLoader,ImageLoader





def tflite_builder(keras_model, weight_quant: bool = False, int8_model: bool = False, image_root: str = None,
                   int8_mean=None, int8_std=None,separation=0,separation_scale=2,onnx_path=None,):
    if int8_std is None:
        int8_std = [58.395, 57.12, 57.375]
    if int8_mean is None:
        int8_mean = [123.675, 116.28, 103.53]

    converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
    if weight_quant or int8_model:
        converter.experimental_new_converter = True
        converter.optimizations = [tf.lite.Optimize.DEFAULT]

    if int8_model:
        assert len(keras_model.inputs) == 1, f"help want, only support single input model."
        shape = list(keras_model.inputs[0].shape)
        dataset = RandomLoader(shape) if image_root is None else ImageLoader(image_root, shape, int8_mean, int8_std,
                                                                             separation,separation_scale,onnx_path=onnx_path)
        converter.representative_dataset = lambda: dataset
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8, tf.lite.OpsSet.SELECT_TF_OPS]
        converter.target_spec.supported_types = []
        if separation>0:
            converter.inference_input_type = tf.uint8
            converter.inference_output_type = tf.int8
        else:
            if separation==0:
                converter.inference_input_type = tf.uint8
            else:
                converter.inference_input_type = tf.int8
            converter.inference_output_type = tf.float32
        converter.experimental_new_converter = True

    tflite_model = converter.convert()
    return tflite_model