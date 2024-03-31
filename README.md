# Welcome to GD32 Model Zoo

Here will provide some deep learning models that can be deployed on GD32 devices。These models have been optimized for GD32 and can run on GD32 devices with fewer memory requirements.This project has the following characteristics。

 - The peak memory usage of the model is adjustable
 - The provided model can be retrained
# Main features: Spatial separation model
![Spatial separation model](assets/model.png)


# Available models

 - [Object Detection](https://github.com/HomiKetalys/gd32ai-modelzoo/tree/main/object_detection):
   - [Yolo-FastestV2](https://github.com/HomiKetalys/gd32ai-modelzoo/tree/main/object_detection/yolo_fastestv2):
     - Now provide models trained on the following dataset
       - COCO2017 Dataset
       - Fusion of Abnormal Behavior Driving Dataset
     - Now available for deployment on the following devices
       - GD32F470IIH6
       - GD32H759IMK6
 - [Image Classification](https://github.com/HomiKetalys/gd32ai-modelzoo/tree/main/image_classification):
   - [FastVit](https://github.com/HomiKetalys/gd32ai-modelzoo/tree/main/image_classification/ml-fastvit):
     - Now provide models trained on the following dataset
       - Food-101 Dataset
     - Now available for deployment on the following devices
       - GD32H759IMK6
 
# reference
 - [onnx2tflite](https://github.com/MPolaris/onnx2tflite)
 - [Yolo-FastestV2](https://github.com/dog-qiuqiu/Yolo-FastestV2)
 - [facelandmarks](https://github.com/midasklr/facelandmarks)
 - [stm32ai-modelzoo](https://github.com/STMicroelectronics/stm32ai-modelzoo/tree/main)
 - [fastvit](https://github.com/apple/ml-fastvit)

# Disclaimers
**Please carefully read and understand all the rights and limitations stipulated in this Disclaimer**

GD32AI ModelZoo (hereinafter referred to as "the Software") is developed by HomiKetalys (contact information: homiketalys@gmail.com A research project on an ultra lightweight deep learning model conducted by the author of this software, with only academic testing conducted on GD32. The following are all rights and restrictions stipulated in this software:
1. This software is strictly prohibited from being used for any infringement, infringement of the interests of others, illegal or other improper behavior. The software and its authors shall not be liable for any direct or indirect liability arising therefrom.
2. The implementation of the functions of this software relies on various third-party software. For any infringement, infringement on the interests of others, illegal or other improper behavior caused by the failure to comply with the rights and restrictions provided by the third-party software, this software and its author shall not be liable for any direct or indirect liability arising from this.

