# Training and Deployment of Object Detection Models

<div align="center">

[中文](README.md) | [English](README_en.md)

</div>

1. [yolofastestv2](yolo_fastestv2) 
2. [yolov10](yolov10) 

# Metrics
The metrics ultimately exported as quantization model. When nothing is precised in the model name, training is done using transfer learning technique from a pre-trained model. Else, "tfs" stands for "training from scratch"."sp" represents the use of spatially separated model deployment.Meanwhile, the table presents the metrics of the model in [stm32ai-modelzoo](https://github.com/STMicroelectronics/stm32ai-modelzoo/blob/main/object_detection/pretrained_models/README.md) .The metric of the model with an asterisk in its name comes from stm32ai-modelzoo.
## On COCO2017 Person Dataset

| model                           | Implementation | Dataset     | Input Resolution | mAP*   | MACCs (M) | Activation RAM (KiB) | Weights Flash (KiB) | Inference framework |
|---------------------------------|----------------|-------------|------------------|--------|-----------|----------------------|---------------------|---------------------|
| Yolo-FastestV2 sp               | Pytorch        | COCO        | 192x192x3        | 41.52% | 32.47     | 116.18               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp tfs           | Pytorch        | COCO        | 192x192x3        | 39.75% | 32.47     | 116.18               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Pytorch        | COCO        | 192x192x3        | 41.52% | -         | 91.76                | -                   | TinyEngine          |
| Yolo-FastestV2 sp tfs           | Pytorch        | COCO        | 192x192x3        | 39.75% | -         | 91.76                | -                   | TinyEngine          |
| Yolo-FastestV2 sp               | Pytorch        | COCO        | 256x256x3        | 49.00% | 57.71     | 214.99               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp tfs           | Pytorch        | COCO        | 256x256x3        | 46.87% | 57.71     | 214.99               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Pytorch        | COCO        | 256x256x3        | 49.00% | -         | 157.36               | -                   | TinyEngine          |
| Yolo-FastestV2 sp tfs           | Pytorch        | COCO        | 256x256x3        | 46.87% | -         | 157.36               | -                   | TinyEngine          |
| YoloV10 sp                      | Pytorch        | COCO Person | 256x256x3        | 52.45% | 57.24     | 148.86               | 372.51              | X-CUBE-AI 8.0.1     |
| YoloV10 sp tfs                  | Pytorch        | COCO Person | 256x256x3        | 50.00% | 57.24     | 148.86               | 372.51              | X-CUBE-AI 8.0.1     |
| YoloV10 sp                      | Pytorch        | COCO Person | 256x256x3        | 52.45% | -         | 144.82               | -                   | TinyEngine          |
| YoloV10 sp tfs                  | Pytorch        | COCO Person | 256x256x3        | 50.00% | -         | 144.82               | -                   | TinyEngine          |
| *SSD MobileNet v1 0.25          | TensorFlow     | COCO Person | 192x192x3        | 33.70% | 40.48     | 266.3                | 438.28              | X-CUBE-AI 8.1.0     |
| *SSD MobileNet v1 0.25          | TensorFlow     | COCO Person | 256x256x3        | 46.26% | 72.55     | 456.1                | 595.66              | X-CUBE-AI 8.1.0     |
| *ST Yolo LC v1 tfs              | TensorFlow     | COCO Person | 192x192x3        | 31.61% | 61.9      | 166.29               | 276.73              | X-CUBE-AI 9.1.0     |
| *ST Yolo LC v1 tfs              | TensorFlow     | COCO Person | 256x256x3        | 40.58% | 110.05    | 278.29               | 276.73              | X-CUBE-AI 9.1.0     |
| *SSD MobileNet v2 0.35 FPN-lite | TensorFlow     | COCO Person | 224x224x3        | 48.67% | 167.15    | 956.82               | 1007.78             | X-CUBE-AI 9.1.0     |

* EVAL_IOU = 0.4, NMS_THRESH = 0.5, SCORE_THRESH = 0.001
## On COCO2017 Val Dataset


| model                 | Implementation | Input Resolution | mAP*   | MACCs (M) | Activation RAM (KiB) | Weights Flash (KiB) | Inference framework |
|-----------------------|----------------|------------------|--------|-----------|----------------------|---------------------|---------------------|
| Yolo-FastestV2 sp     | Pytorch        | 192x192x3        | 17.33% | 32.47     | 116.18               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp tfs | Pytorch        | 192x192x3        | 11.82% | 32.47     | 116.18               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp     | Pytorch        | 256x256x3        | 21.11% | 57.71     | 214.99               | 385.79              | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp tfs | Pytorch        | 256x256x3        | 16.18% | 57.71     | 214.99               | 385.79              | X-CUBE-AI 8.0.1     |
| YoloV10t sp tfs       | Pytorch        | 256x256x3        | 19.21% | 57.24     | 148.86               | 372.51              | X-CUBE-AI 8.0.1     |


* EVAL_IOU = 0.4, NMS_THRESH = 0.5, SCORE_THRESH = 0.001
## On Fusion of Abnormal Behavior Driving Dataset(FABD)


| model                 | Implementation  | Input Resolution  | mAP*    | MACCs (M)  | Activation RAM (KiB)   | Weights Flash (KiB)  | Inference framework  |
|-----------------------|-----------------|-------------------|---------|------------|------------------------|----------------------|----------------------|
| Yolo-FastestV2 sp tfs | Pytorch         | 256x256x3         | 54.98%  | 57.71      | 214.99                 | 385.79               | X-CUBE-AI 8.0.1      |

* EVAL_IOU = 0.5, NMS_THRESH = 0.4, SCORE_THRESH = 0.01
## Time
The inference time is measured by programs compiled using GCC, while programs compiled using ARMCC measure approximately 30% more time.

| Model                           | Format | Resolution | Board      | Execution Engine | Frequency | Inference time (ms) | Inference framework |
|---------------------------------|--------|------------|------------|------------------|-----------|---------------------|---------------------|
| Yolo-FastestV2 sp               | Int8   | 192x192x3  | GD32F470I  | 1 CPU            | 240 MHz   | 500.86 ms           | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Int8   | 192x192x3  | GD32F470I  | 1 CPU            | 240 MHz   | 448.24 ms           | X-CUBE-AI 9.0.0     |
| Yolo-FastestV2 sp               | Int8   | 192x192x3  | GD32F470I  | 1 CPU            | 240 MHz   | 430.52 ms           | TinyEngine          |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32F470I  | 1 CPU            | 240 MHz   | 863.13 ms           | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32F470I  | 1 CPU            | 240 MHz   | 784.14 ms           | X-CUBE-AI 9.0.0     |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32F470I  | 1 CPU            | 240 MHz   | 749.00 ms           | TinyEngine          |
| Yolo-FastestV2 sp               | Int8   | 192x192x3  | GD32H759I  | 1 CPU            | 600 MHz   | 143.66 ms           | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Int8   | 192x192x3  | GD32H759I  | 1 CPU            | 600 MHz   | 132.73 ms           | TinyEngine          |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 235.58 ms           | X-CUBE-AI 8.0.1     |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 221.51 ms           | X-CUBE-AI 9.0.0     |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 234.13 ms           | X-CUBE-AI 9.1.0     |
| Yolo-FastestV2 sp               | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 219.36 ms           | TinyEngine          |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32F470I  | 1 CPU            | 240 MHz   | 776.14 ms           | X-CUBE-AI 9.0.0     |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32F470I  | 1 CPU            | 240 MHz   | 707.87 ms           | TinyEngine          |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 219.70 ms           | X-CUBE-AI 8.0.1     |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 192.57 ms           | X-CUBE-AI 9.0.0     |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 208.77 ms           | X-CUBE-AI 9.1.0     |
| YoloV10t sp                     | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 190.61 ms           | TinyEngine          |
| *SSD Mobilenet v1 0.25          | Int8   | 192x192x3  | STM32H747I | 1 CPU            | 400 MHz   | 149.22 ms           | X-CUBE-AI 9.1.0     |
| SSD Mobilenet v1 0.25           | Int8   | 192x192x3  | GD32H759I  | 1 CPU            | 600 MHz   | 121.10 ms           | X-CUBE-AI 9.1.0     |
| *SSD Mobilenet v1 0.25          | Int8   | 256x256x3  | STM32H747I | 1 CPU            | 400 MHz   | 266.4 ms            | X-CUBE-AI 9.1.0     |
| SSD Mobilenet v1 0.25           | Int8   | 256x256x3  | GD32H759I  | 1 CPU            | 600 MHz   | 208.05 ms           | X-CUBE-AI 9.1.0     |
| *ST Yolo LC v1                  | Int8   | 192x192x3  | STM32H747I | 1 CPU            | 400 MHz   | 179.35 ms           | X-CUBE-AI 9.1.0     |
| *ST Yolo LC v1                  | Int8   | 256x256x3  | STM32H747I | 1 CPU            | 400 MHz   | 321.23 ms           | X-CUBE-AI 9.1.0     |
| *SSD MobileNet v2 0.35 FPN-lite | Int8   | 224x224x3  | STM32H747I | 1 CPU            | 400 MHz   | 675.63 ms           | X-CUBE-AI 9.1.0     |