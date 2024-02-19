import os

from .onnx2tflite.utils import load_onnx_modelproto, keras_builder, get_elements_error
from .utils.builder_sp import tflite_builder
import logging

LOG = logging.getLogger("converter running:")
def onnx_converter(onnx_model_path: str, output_path: str = None,
                   input_node_names: list = None, output_node_names: list = None,
                   need_simplify: bool = True, target_formats=None,
                   native_groupconv: bool = False,
                   weight_quant: bool = False, int8_model: bool = False, image_root: str = None,
                   int8_mean=None, int8_std=None, separation=0, separation_scale=2, ) -> float:
    if target_formats is None:
        target_formats = ['keras', 'tflite']
    if int8_mean is None:
        int8_mean = [123.675, 116.28, 103.53]
    if int8_std is None:
        int8_std = [58.395, 57.12, 57.375]
    if not isinstance(target_formats, list) and 'keras' not in target_formats and 'tflite' not in target_formats:
        raise KeyError("'keras' or 'tflite' should in list")

    model_proto = load_onnx_modelproto(onnx_model_path, input_node_names, output_node_names, need_simplify)

    keras_model = keras_builder(model_proto, native_groupconv)

    if 'tflite' in target_formats:
        tflite_model = tflite_builder(keras_model, weight_quant, int8_model, image_root, int8_mean, int8_std,
                                      separation=separation, separation_scale=separation_scale,
                                      onnx_path=onnx_model_path)

    onnx_path, model_name = os.path.split(onnx_model_path)
    if output_path is None:
        output_path = onnx_path
    output_path = os.path.join(output_path, model_name.split('.')[0])

    keras_model_path = None
    if 'keras' in target_formats:
        keras_model_path = output_path + ".h5"
        keras_model.save(keras_model_path)
        LOG.info(f"keras model saved in {keras_model_path}")

    tflite_model_path = None
    if 'tflite' in target_formats:
        tflite_model_path = output_path + ".tflite"
        with open(tflite_model_path, "wb") as fp:
            fp.write(tflite_model)

    convert_result = {"keras": keras_model_path, "tflite": tflite_model_path, "keras_error": 0, "tflite_error": 0}
    # ignore quantization model
    if int8_model:
        return convert_result

    error_dict = {}
    try:
        error_dict = get_elements_error(model_proto, keras_model_path, tflite_model_path)
        keras_error, tflite_error = error_dict.get("keras", None), error_dict.get("tflite", None)
        if keras_error:
            if keras_error > 1e-2:
                LOG.error(
                    "h5 model elements' max error has reached {:^.4E}, but convert is done, please check {} carefully!".format(
                        keras_error, keras_model_path))
            elif keras_error > 1e-4:
                LOG.warning("h5 model elements' max error is {:^.4E}, pass, h5 saved in {}".format(keras_error,
                                                                                                   keras_model_path))
            else:
                LOG.info("h5 model elements' max error is {:^.4E}, pass, h5 saved in {}".format(keras_error,
                                                                                                keras_model_path))
        if tflite_error:
            if tflite_error > 1e-2:
                LOG.error(
                    "tflite model elements' max error has reached {:^.4E}, but convert is done, please check {} carefully!".format(
                        tflite_error, tflite_model_path))
            elif tflite_error > 1e-4:
                LOG.warning("tflite model elements' max error is {:^.4E}, pass, tflite saved in {}".format(tflite_error,
                                                                                                           tflite_model_path))
            else:
                LOG.info("tflite model elements' max error is {:^.4E}, pass, tflite saved in {}".format(tflite_error,
                                                                                                        tflite_model_path))
    except:
        LOG.warning("convert is successed, but model running is failed, please check carefully!")

    convert_result["keras_error"] = error_dict.get("keras", None)
    convert_result["tflite_error"] = error_dict.get("tflite", None)
    return convert_result