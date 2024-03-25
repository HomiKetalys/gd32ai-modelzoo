

# Training and deployment of the Yolo-FastestV2 model

This will provide training for the Yolo-FastestV2 model and deployment on GD32 devices

# Preparation

 1. Use git to clone the gd32ai modelzoo project and init submodules.
```
git clone https://github.com/HomiKetalys/gd32ai-modelzoo
git submodule update --init --recursive
```
 2. Add the project root directory to PYTHONPATH and switch to yolo-fastestv2 directory.
 - on powershell.
```
$env:PYTHONPATH=$(pwd)
cd gd32ai-modelzoo/object_detection/yolo_fastestv2
```

 3. Prepare your dataset.The directory structure of the dataset should be as follows:
```
  dataset
  ├── images
  │   ├── title1
  │   │   ├──img1.jpg
  │   │   └──img2.jpg
  │   └── title2
  │       └──img3.jpg
  │       └──img4.jpg
  ├── label0
  │   ├── title1
  │   │   ├──img1.txt
  │   │   └──img2.txt
  │   └── title2
  │       └──img3.txt
  │       └──img4.txt
  └── label1
```
The directory path where image files and corresponding annotation files are located should have the same structure, except for the differences between images and label0 or label1.
The annotation format in the annotation file txt is darknet yolo. it looks like:
```
0 0.7117890625000001 0.05518711018711019 0.035546875 0.07432432432432433
13 0.21646875000000004 0.25730769230769235 0.1015625 0.10280665280665281
```
 4. Place the image file used for training in train_label0.txt, and the image file used for validation in val_label0.txt. it looks like:
```
images\train2017\000000391895.jpg
images\train2017\000000522418.jpg
images\train2017\000000184613.jpg
```
  If it is a relative path, then it is the relative path relative to the text file.
 ## Environmental preparation
 Install the environment(anaconda recommended) according to [Yolo-FastestV2](https://github.com/dog-qiuqiu/Yolo-FastestV2), [onnx2tflite](https://github.com/MPolaris/onnx2tflite), and [facelandmarks](https://github.com/midasklr/facelandmarks) (If you are using the FABD dataset)
  ## Supported Datasets
  
 - COCO2017:If you want to train on the COCO2017 dataset, you can prepare the dataset by following the steps below.
 1. Download the COCO2017 dataset from [here](https://cocodataset.org/#download) .The directory structure should be as follows：
```
  COCO2017
  ├── images
  │   ├── train2017
  │   │   ├──000000000009.jpg
  ...
  │   │   └──000000581929.jpg
  │   └── val2017
  │       ├──000000000139.jpg
  ...
  │       └──000000581781.jpg
  └── annotations
      ├── instances_train2017.json
      └── instances_val2017.json
```
  2. Execute the following command to convert the COCO2017 dataset to the required format.
  
 Convert train set
```
python ../../common_utils/coco2yolo.py --images_path "{datasets_root}/COCO2017/images/train2017" --json_file "{datasets_root}/COCO2017/annotations/instances_train2017.json" --ana_txt_save_path "{datasets_root}/COCO2017/coco_80/train2017" --out_txt_path "{datasets_root}/COCO2017/train2017.txt"
```
Convert val set

```
python ../../common_utils/coco2yolo.py --images_path "{datasets_root}/COCO2017/images/val2017" --json_file "{datasets_root}/COCO2017/annotations/instances_val2017.json" --ana_txt_save_path "{datasets_root}/COCO2017/coco_80/val2017" --out_txt_path "{datasets_root}/COCO2017/val2017.txt"
```
- Fusion of Abnormal Behavior Driving Dataset(FABD):
1. Download the HaGrid Test dataset from [here](https://github.com/hukenovs/hagrid/tree/Hagrid_v1) .The directory structure should be as follows：
```
  HaGrid
  ├── images
  │   ├── call
  │   │   ├──000bc28d-49ee-4c08-a972-368e6fc7eeac.jpg
  ...
  │   │   └──fff9d99d-787b-47f7-a6c0-a1b6cad02063.jpg
  │   └── two_up_inverted
  │       ├──000b4a0d-fd66-4903-8645-5ad4396bd2b9.jpg
  ...
  │       └──ffd643cb-70d6-4d5a-bdd3-ada0aeb6d626.jpg
  └── ann
      ├── call.json
  ...
      └── two_up_inverted.json
```
 2. Download the WFLW dataset from [here](https://wywu.github.io/projects/LAB/WFLW.html) .The directory structure should be as follows：
```
  WFLW
  ├── images
  │   ├── 0--Parade
  │   │   ├──0_Parade_marchingband_1_116.jpg
  ...
  │   │   └──0_Parade_Parade_0_1040.jpg
  │   └── 61--Street_Battle
  │       ├──61_Street_Battle_streetfight_61_10.jpg
  ...
  │       └──61_Street_Battle_streetfight_61_1008.jpg
  └── list_98pt_rect_attr_train_test
```
  3. Download the Lapa dataset from [here](https://github.com/jd-opensource/lapa-dataset) .The directory structure should be as follows：
```
  Lapa
  ├── train
  │   ├── images
  │   ├── landmarks
  │   └── labels
  ├── val
  └── test
```
  4. Execute the following command to generate a fused FABD dataset:
```
  python ../../common_utils/gen_datas.py --wflw_path "{datasets_root}/WFLW" --lapa_path "{datasets_root}/Lapa" --hagrid_path=""{datasets_root}/HaGrid" --save_path "{datasets_root}/FABD"
```
 # Train
 ## set configuration file
 The configuration file is located in the configs folder in the current directory.Its content looks like the following:
```
[name]  
model_name=coco  
  
[train-configure]  
epochs=300  
steps=150,250  
batch_size=128  
subdivisions=1  
learning_rate=0.001  
  
[model-configure]  
pre_weights=None  
classes=80  
width=256  
height=256  
anchor_num=3  
separation=4  
separation_scale=2  
conf_thr=0.001  
nms_thr=0.5  
iou_thr=0.4  
anchors=9.192727272727273, 14.101818181818182, 27.54909090909091, 37.44, 40.516363636363636, 100.58909090909091, 92.29818181818182, 56.89454545454546, 95.68727272727273, 156.03636363636363, 203.57818181818183, 188.26909090909092  
  
[data-configure]  
label_flag=coco_80  
train=../../../datasets/coco2017/train2017.txt  
val=../../../datasets/coco2017/val2017.txt  
names=./configs/coco.names
```
  1. Due to the fact that the object detection method of Yolo FastV2 belongs to anchor based, it is necessary to cluster the distribution of target boxes first.Execute the following command to collect information on the target box for the specified dataset, such as COCO2017.
```
  python genanchors.py --traintxt "{datasets_root}/COCO2017/train2017.txt" --output_dir "./" --label_flag "coco_80" --num_clusters 6 --input_width 256 --input_height 256
```
  This command will generate the file anchor6.txt in the specified directory "./".Paste the first line of content from anchor6.txt into anchors in the configuration file.

   2. Fill in the remaining content in the configuration file.
   ## start train
   Execute the following command to start training.
```
  python train.py --data configs/coco_sp.data
```
  The training results will be saved in results/train.Taking the configuration file coco_sp.data as an example, the first training result will be saved to results/train/coco_sp_0000.
 # Export And Evaluation
 Next, export the training results of the model as onnx or tflite.Execute the following command to export.
```
  python pytorch2tflite.py --data results/train/coco_sp_0000/coco_sp.data --model_path results/train/coco_sp_0000/best.pth --convert_type 1 --tflite_val_path "{datasets_root}/COCO2017/images/val2017"
```
  `convert_type` controls the conversion type,0 for onnx,1 for tflite.If the exported file is of type tflite, it is quantized.
  Meanwhile, the exported model will be evaluated.
  # Deploy
  Next, deploy the exported model to GD32 device
  1. Install STM32CUBE-AI
  The deployment tool used in this project is STM32CUBE-AI. It is based on STM32CUBEIDE. STM32CUBEIDE can be downloaded from [here](https://www.st.com/content/st_com/zh/stm32cubeide.html) .Then install STM32CUBE-AI 8.0.1 in STM32CUBEIDE.
  2. Install keil5 5.29,you can download it from [here](https://www.keil.com/update/rvmdk.asp)
  3. Download gcc-arm-none-eabi 10.3-2021.10 from [here](https://developer.arm.com/downloads/-/gnu-rm) 
  4. Download GD32H7xx AddOn from [here](https://www.gd32mcu.com/cn/download?kw=GD32H7) and install pack.
  5. Execute the following command to generate codes.
```
  python deploy.py --data results/train/coco_sp_0000/coco_sp.data --model_path results/train/coco_sp_0000/best.pth --convert_type 1 --tflite_val_path "{datasets_root}/COCO2017/images/val2017" --gd32_project_path deployment/gd32h759IMK6_GCC --series h7 --compiler 1
```
5.  Open "deployment/gd32h759IMK6_GCC/MDK-ARM/GD32H759I_EVAL.uvprojx" with keil5 and configure gcc path in Keil5
6.  Compile and download code to flash memory。
# Metrics
The metrics ultimately exported as quantization model
When nothing is precised in the model name, training is done using transfer learning technique from a pre-trained model. 
Else, "tfs" stands for "training from scratch"."sp" represents the use of spatially separated model deployment
Meanwhile, the table presents the metrics of the model in [stm32ai-modelzoo](https://github.com/STMicroelectronics/stm32ai-modelzoo/blob/main/object_detection/pretrained_models/README.md) .
The metric of the model with an asterisk in its name comes from stm32ai model zoo
## On COCO2017 Person Dataset

| model                 | Implementation | Dataset                     | Input Resolution | mAP*   | MACCs (M) | Activation RAM (KiB) | Weights Flash (KiB) | STM32Cube.AI version |
|-----------------------|----------------|-----------------------------|------------------|--------|-----------|----------------------|---------------------|----------------------|
| Yolo-FastestV2 sp     | Pytorch        | COCO2017 val person dataset | 192x192x3        | 41.54% | 32.47     | 116.18               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp tfs | Pytorch        | COCO2017 val person dataset | 192x192x3        | 40.06% | 32.47     | 116.18               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp     | Pytorch        | COCO2017 val person dataset | 256x256x3        | 48.99% | 57.71     | 214.99               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp tfs | Pytorch        | COCO2017 val person dataset | 256x256x3        | 46.88% | 57.71     | 214.99               | 385.79              | 8.0.1                |
| SSD MobileNet v1 0.25 | TensorFlow     | COCO2017 val person dataset | 192x192x3        | 33.52% | 40.54     | 249.54               | 438.28              | 8.1.0                |
| SSD MobileNet v1 0.25 | TensorFlow     | COCO2017 val person dataset | 256x256x3        | 47.50% | 72.40     | 439.34               | 470.13              | 8.1.0                |
| ST Yolo LC v1 tfs     | TensorFlow     | COCO2017 val person dataset | 192x192x3        | 39.92% | 61.9      | 157.44               | 276.73              | 7.3.0                |
| ST Yolo LC v1 tfs     | TensorFlow     | COCO2017 val person dataset | 256x256x3        | 45.09% | 110.05    | 271.94               | 276.73              | 7.3.0                |

\* EVAL_IOU = 0.4, NMS_THRESH = 0.5, SCORE_THRESH = 0.001
## On COCO2017 Val Dataset
When nothing is precised in the model name, training is done using transfer learning technique from a pre-trained model. Else, "tfs" stands for "training from scratch".

| model                 | Implementation | Dataset              | Input Resolution | mAP*   | MACCs (M) | Activation RAM (KiB) | Weights Flash (KiB) | STM32Cube.AI version |
|-----------------------|----------------|----------------------|------------------|--------|-----------|----------------------|---------------------|----------------------|
| Yolo-FastestV2 sp     | Pytorch        | COCO2017 val dataset | 192x192x3        | 17.45% | 32.47     | 116.18               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp tfs | Pytorch        | COCO2017 val dataset | 192x192x3        | 11.78% | 32.47     | 116.18               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp     | Pytorch        | COCO2017 val dataset | 256x256x3        | 21.23% | 57.71     | 214.99               | 385.79              | 8.0.1                |
| Yolo-FastestV2 sp tfs | Pytorch        | COCO2017 val dataset | 256x256x3        | 16.06% | 57.71     | 214.99               | 385.79              | 8.0.1                |

\* EVAL_IOU = 0.4, NMS_THRESH = 0.5, SCORE_THRESH = 0.001
## On Fusion of Abnormal Behavior Driving Dataset(FABD)
When nothing is precised in the model name, training is done using transfer learning technique from a pre-trained model. Else, "tfs" stands for "training from scratch".

| model                 | Implementation | Dataset | Input Resolution | mAP*   | MACCs (M) | Activation RAM (KiB) | Weights Flash (KiB) | STM32Cube.AI version |
|-----------------------|----------------|---------|------------------|--------|-----------|----------------------|---------------------|----------------------|
| Yolo-FastestV2 sp tfs | Pytorch        | FABD    | 256x256x3        | 54.62% | 57.71     | 214.99               | 385.79              | 8.0.1                |

\* EVAL_IOU = 0.5, NMS_THRESH = 0.4, SCORE_THRESH = 0.01
# Results On GD32 Device
## Time
| Model                 | Format | Resolution | Board        | Execution Engine | Frequency | Inference time (ms) | STM32Cube.AI version |
|-----------------------|--------|------------|--------------|------------------|-----------|---------------------|----------------------|
| Yolo-FastestV2 sp     | Int8   | 192x192x3  | GD32F470IIH6 | 1 CPU            | 240 MHz   | 748 ms              | 8.0.1                |
| Yolo-FastestV2 sp     | Int8   | 256x256x3  | GD32F470IIH6 | 1 CPU            | 240 MHz   | 1304 ms             | 8.0.1                |
| SSD Mobilenet v1 0.25 | Int8   | 192x192x3  | GD32F470IIH6 | 1 CPU            | 240 MHz   | 808 ms              | 8.0.1                |
| SSD Mobilenet v1 0.25 | Int8   | 256x256x3  | GD32F470IIH6 | 1 CPU            | 240 MHz   | 1456 ms             | 8.0.1                |
| Yolo-FastestV2 sp     | Int8   | 192x192x3  | GD32H759IMK6 | 1 CPU            | 600 MHz   | 145 ms              | 8.0.1                |
| Yolo-FastestV2 sp     | Int8   | 256x256x3  | GD32H759IMK6 | 1 CPU            | 600 MHz   | 238 ms              | 8.0.1                |
| SSD Mobilenet v1 0.25 | Int8   | 192x192x3  | STM32H747I   | 1 CPU            | 400 MHz   | 241 ms              | 8.1.0                |
| SSD Mobilenet v1 0.25 | Int8   | 256x256x3  | STM32H747I   | 1 CPU            | 400 MHz   | 295 ms              | 8.1.0                |
## Images
![coco2017 on gd32](../../assets/img1.png)
![coco2017 on gd32](../../assets/img2.png)



