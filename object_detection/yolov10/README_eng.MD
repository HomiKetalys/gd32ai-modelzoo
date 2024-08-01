

# Training and deployment of the Yolov10t model

This will provide training for the Yolov10t model and deployment.

# Preparation

1. Use git to clone the gd32ai modelzoo project and init submodules.
```
git clone https://github.com/HomiKetalys/gd32ai-modelzoo
cd gd32ai-modelzoo
git submodule update --init --recursive
```
2. Add the project root directory to PYTHONPATH and switch to yolov10 directory.
 - on powershell.
```
$env:PYTHONPATH=$(pwd)
cd object_detection/yolov10
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
label cx                  cy                  h           w
0     0.7117890625000001  0.05518711018711019 0.035546875 0.07432432432432433
13    0.21646875000000004 0.25730769230769235 0.1015625   0.10280665280665281
```
4. Place the image file used for training in train_label0.txt, and the image file used for validation in val_label0.txt. it looks like:
```
images\train2017\000000391895.jpg
images\train2017\000000522418.jpg
images\train2017\000000184613.jpg
```
If it is a relative path, then it is the relative path relative to the text file.
## Environmental preparation
Install the environment(anaconda recommended) according to [YoloV10](https://github.com/THU-MIG/yolov10) , [onnx2tflite](https://github.com/MPolaris/onnx2tflite) .
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
  3. If you require a dataset of specified categories in COCO, you need to add the parameter '--export_categorys'. For example, if you require a person dataset with each category separated by a space, execute the following command:
```
python ../../common_utils/coco2yolo.py --images_path "{datasets_root}/COCO2017/images/train2017" --json_file "{datasets_root}/COCO2017/annotations/instances_train2017.json" --ana_txt_save_path "{datasets_root}/COCO2017/coco_person/val2017" --out_txt_path "{datasets_root}/COCO2017/val2017_person.txt" --export_categorys "0"
```
# Train
## set configuration file
The configuration file is located in the configs folder in the current directory.Its content looks like the following:
```
DATASET:
  TRAIN: "../../../datasets/coco2017/train2017.txt"
  VAL: "../../../datasets/coco2017/val2017.txt"
  NAMES: "configs/coco_80.names"
MODEL:
  NC: 80
  INPUT_WIDTH: 256
  INPUT_HEIGHT: 256
  SEPARATION: 3
  SEPARATION_SCALE: 2
  REG_MAX: 1
  REG_SCALE: 0.5
TRAIN:
  LR: 0.001
  THRESH: 0.25
  WARMUP: true
  BATCH_SIZE: 96
  END_EPOCH: 300
  MILESTIONES:
    - 100
    - 200
    - 250
  AMP: False
  USE_TAA: False
VAL:
  CONF: 0.001
  NMS: 0.5
  IOU: 0.4
```
1. Configure various configuration parameters.The key is to set the dataset path and label name file settings.


## start train
Execute the following command to start training. Change the `--yaml` parameter to your own configuration file. If you need to continue training on a pre training, add the `--weight` parameter and specify the path to the .pth file.
```
python train.py --yaml configs/coco_80.yaml
```
The training results will be saved in results/train.Taking the configuration file coco_80.yaml as an example, the first training result will be saved to results/train/coco_80_0000.

# Export And Evaluation
 Next, export the training results of the model as onnx or tflite.Execute the following command to export.
```
python export.py --yaml results/train/coco_80_0000/coco_80.yaml --weight results/train/coco_80_0000/weights/best.pth --convert_type 1 --tflite_val_path "{datasets_root}/COCO2017/images/val2017 --eval 1"
```
`convert_type` controls the conversion type,0 for onnx,1 for tflite.If the exported file is of type tflite, it is quantized. Meanwhile, the exported model will be evaluated.
# Deploy
Next, the exported model will be converted into C language code or further deployed on a specific chip.Taking "deployment/GD32H759I2_EVAL_GCC/MDK-ARM/GD32H759I2_EVAL.uvprojx" as an example.
1. Choose one of the following two inference engines
- If you choose to use X-CUBE-AI, you will need to download [X-CUBE-AI](https://www.st.com/en/embedded-software/x-cube-ai.html#st-get-software) and unzip it。Select the version you need based on the table below.If the required version is less than 9.0.0, [STM32CUBEIDE](https://www.st.com.cn/content/st_com/zh/stm32cubeide.html#st-get-software)  needs to be pre installed,And install the corresponding version of X-CUBE-AI in STM32CUBEIDE, then copy the file according to the prompts in [issue](https://github.com/HomiKetalys/gd32ai-modelzoo/issues/2#issuecomment-2143376547) .


| X-CUBE-AI version | GCC | ARMCC |
|-------------------|-----|-------|
| 8.0.1             | √   | √     |
| 8.1.0             | √   | √     |
| 9.0.0             | √   | ×     |

- If you choose to use TinyEngine, there is no need for additional operation, as it comes with this project.If you use TinyEngine, you don't need to pass `--stm32cubeai_path`.

2. Install [keil5](https://www.keil.com/update/rvmdk.asp) 5.29。
3. Download [gcc-arm-none-eabi](https://developer.arm.com/downloads/-/gnu-rm) 10.3-2021.10 and unzip it,If using ARMCC, there is no need.
4. If you want to deploy on GD32 devices, download [GD32H7xx AddOn](https://www.gd32mcu.com/cn/download?kw=GD32H7)  and install it.
5. Execute the following command to generate model inference C language code, where each parameter is modified to the one you need.If the parameter `--c_project_path` is a folder path, an Edge_AI folder will be generated in that folder. If it is a. uvprojx file from keil5, it will be directly deployed to the corresponding keil5 project.

```
python deploy.py --yaml results/train/coco_80_0000/coco_80.yaml --weight results/train/coco_80_0000/best.pth --tflite_val_path "{datasets_root}/COCO2017/images/val2017" --c_project_path deployment/GD32H759I_EVAL_GCC/MDK-ARM/GD32H759I_EVAL.uvprojx --series h7 --stm32cubeai_path ”{X-CUBE-AI PATH}/stedgeai-windows-9.0.0“
```
6. Use keil5 to open "deployment/GD32H759I2_EVAL_GCC/MDK-ARM/GD32H759I2_EVAL.uvprojx" and configure the gcc path in keil5. If it is ARMCC, it is not necessary. If you specify `--c_project_path` as the folder path, you also need to add the corresponding. c file, include path, and. a (. lib) library file in the keil5 project where you need to use the model. The sample project has been added.
7. Implement your image reading method at the end of the ai_madel. h file. For the sample project, provide an example of the image reading method, and simply remove the corresponding annotation for the image reading method.
8. In your project, call AI_Run() to run the model, call get_obj_num() to get the number of detection targets, call get_obj_name(id) to get the category of the idth target, and call get_obj_xyxy(id,&x0,&y0,&x1,&y1) to get the box of the idth target. The sample project has been written with sample applications. Finally compile and flash.

# Images




