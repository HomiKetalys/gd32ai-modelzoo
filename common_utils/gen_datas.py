import argparse
import copy
import json
import random

import numpy as np
import torch
from torchvision import transforms
import cv2
import os
from common_utils.facelandmarks.faceland import FaceLanndInference
from hdface.hdface import hdface_detector


def xyxy2xywh(xyxy):
    xywh = [
        (xyxy[0] + xyxy[2]) // 2,
        (xyxy[1] + xyxy[3]) // 2,
        xyxy[2] - xyxy[0],
        xyxy[3] - xyxy[1],
    ]
    return xywh


def xywh2xyxy(xywh):
    xyxy = [
        (2 * xywh[0] - xywh[2]) // 2,
        (2 * xywh[1] - xywh[3]) // 2,
        (2 * xywh[0] + xywh[2]) // 2,
        (2 * xywh[1] + xywh[3]) // 2,
    ]
    return xyxy


def xyxy0_in_xyxy1(xyxy0: list, xyxy1: list):
    new_xyxy0 = [
        max(xyxy1[0], xyxy0[0]),
        max(xyxy1[1], xyxy0[1]),
        min(xyxy1[2], xyxy0[2]),
        min(xyxy1[3], xyxy0[3])]
    if new_xyxy0[0] >= new_xyxy0[2] or new_xyxy0[1] >= new_xyxy0[3]:
        return None
    else:
        return new_xyxy0




def gen_datas(wflw_path, lapa_path, hagrid_path, save_path):
    object_nums=[0]*5
    r12_thr=0.15
    r3_thr=0.35
    with open(save_path + '/img_paths.txt', 'w') as f0:
        face_datas_path = save_path + '/images'
        os.makedirs(face_datas_path, exist_ok=True)
        if wflw_path is not None:
            images_path = wflw_path + '/images'
            labels_path = wflw_path + '/list_98pt_rect_attr_train_test/list_98pt_rect_attr_train.txt'

            key_point_num = 98
            with open(labels_path, 'r') as f:
                lines = f.readlines()
            eye1_index = list(range(60, 68))
            eye2_index = list(range(68, 76))
            mouth_index = list(range(88, 96))
            num = 0
            for line in lines:
                element = line.strip().split(' ')
                key_points = [float(x) for x in element[:key_point_num * 2]]
                face_xyxy = [float(x) for x in element[key_point_num * 2:key_point_num * 2 + 4]]
                image_path = images_path + '/' + element[-1]
                img = cv2.imread(image_path)
                x0, y0, x1, y1 = face_xyxy
                h, w = img.shape[:2]
                rl = np.random.randint(0, 200, (4,))
                # rl = [200, 200, 200, 200]
                if (y1 - y0) * (x1 - x0) < 48 * 48:
                    continue
                y0_, x0_, y1_, x1_ = (
                    int(max(y0 - (y1 - y0) * rl[0] / 100, 0)),
                    int(max(x0 - (x1 - x0) * rl[1] / 100, 0)),
                    int(min(y1 + (y1 - y0) * rl[2] / 100, h)),
                    int(min(x1 + (x1 - x0) * rl[3] / 100, w)))
                img = img[y0_:y1_, x0_:x1_, :]
                for i in range(key_point_num):
                    key_points[2 * i], key_points[2 * i + 1] = key_points[2 * i] - x0_, key_points[2 * i + 1] - y0_
                h, w = img.shape[:2]
                key_points = np.array(key_points)
                key_points = np.reshape(key_points, (-1, 2))
                eye1 = key_points[eye1_index, :]
                eye2 = key_points[eye2_index, :]
                mouth = key_points[mouth_index, :]
                r1 = np.sqrt(np.sum((eye1[2, :] - eye1[6, :]) ** 2)) / np.sqrt(
                    np.sum((eye1[0, :] - eye1[4, :]) ** 2) + 1e-8)
                r2 = np.sqrt(np.sum((eye2[2, :] - eye2[6, :]) ** 2)) / np.sqrt(
                    np.sum((eye2[0, :] - eye2[4, :]) ** 2) + 1e-8)
                r3 = np.sqrt(np.sum((mouth[2, :] - mouth[6, :]) ** 2)) / np.sqrt(
                    np.sum((mouth[0, :] - mouth[4, :]) ** 2) + 1e-8)
                eye1_xyxy = np.concatenate((np.min(eye1, axis=0), np.max(eye1, axis=0)), axis=0)
                eye2_xyxy = np.concatenate((np.min(eye2, axis=0), np.max(eye2, axis=0)), axis=0)
                mouth_xyxy = np.concatenate((np.min(mouth, axis=0), np.max(mouth, axis=0)), axis=0)
                eye1_xywh = np.concatenate(((eye1_xyxy[:2] + eye1_xyxy[2:]) / 2, eye1_xyxy[2:] - eye1_xyxy[:2]), axis=0)
                eye2_xywh = np.concatenate(((eye2_xyxy[:2] + eye2_xyxy[2:]) / 2, eye2_xyxy[2:] - eye2_xyxy[:2]), axis=0)
                mouth_xywh = np.concatenate(((mouth_xyxy[:2] + mouth_xyxy[2:]) / 2, mouth_xyxy[2:] - mouth_xyxy[:2]),
                                            axis=0)

                cv2.imwrite(face_datas_path + f'/wflw_{num:04d}_' + element[-1].split('/')[-1], img)
                f0.write(
                    os.path.relpath(face_datas_path + f'/wflw_{num:04d}_' + element[-1].split('/')[-1] + '\n',
                                    save_path))
                with open(face_datas_path + f'/wflw_{num:04d}_' + ".".join(
                        element[-1].split('/')[-1].split('.')[:-1]) + '.txt', 'w') as f1:
                    if r1 > r12_thr:
                        eye1_state = 1
                        object_nums[1] += 1
                    else:
                        eye1_state = 0
                        object_nums[0] += 1
                    if r2 > r12_thr:
                        eye2_state = 1
                        object_nums[1] += 1
                    else:
                        eye2_state = 0
                        object_nums[0] += 1
                    if r3 > r3_thr:
                        mouth_state = 3
                        object_nums[3] += 1
                    else:
                        mouth_state = 2
                        object_nums[2] += 1
                    f1.write(
                        f'{eye1_state} {eye1_xywh[0] / w} {eye1_xywh[1] / h} {eye1_xywh[2] / w} {eye1_xywh[3] / h}\n')
                    f1.write(
                        f'{eye2_state} {eye2_xywh[0] / w} {eye2_xywh[1] / h} {eye2_xywh[2] / w} {eye2_xywh[3] / h}\n')
                    f1.write(
                        f'{mouth_state} {mouth_xywh[0] / w} {mouth_xywh[1] / h} {mouth_xywh[2] / w} {mouth_xywh[3] / h}')
                num += 1
        if lapa_path is not None:
            train_path = lapa_path + '/train'
            labels_path = train_path + '/landmarks'
            imgs_path = train_path + '/images'
            labels_list = os.listdir(labels_path)
            eye1_index = list(range(66, 74))
            eye2_index = list(range(75, 83))
            mouth_index = list(range(96, 104))
            for label_file_name in labels_list:
                with open(labels_path + '/' + label_file_name, 'r') as f:
                    lines = f.readlines()
                lines = [x.strip() for x in lines[1:]]
                key_points = [float(x) for x in str.join(' ', lines).split(' ')]

                img_path = imgs_path + '/' + label_file_name.replace('.txt', '.jpg')
                img = cv2.imread(img_path)
                key_point_num = len(lines)
                h, w = img.shape[:2]
                for i in range(key_point_num):
                    key_points[2 * i], key_points[2 * i + 1] = min(key_points[2 * i], w), min(key_points[2 * i + 1], h)
                for i in range(key_point_num):
                    key_points[2 * i], key_points[2 * i + 1] = max(key_points[2 * i], 0), max(key_points[2 * i + 1], 0)
                key_points_ = np.array(key_points)
                key_points_ = np.reshape(key_points_, (-1, 2))

                face_xyxy = np.concatenate((np.min(key_points_, axis=0), np.max(key_points_, axis=0)), axis=0)
                # face_xyxy[1] = max(face_xyxy[1] - (face_xyxy[3] - face_xyxy[1]) / 3, 0)

                x0, y0, x1, y1 = face_xyxy
                h, w = img.shape[:2]
                rl = np.random.randint(0, 200, (4,))
                if (y1 - y0) * (x1 - x0) < 48 * 48:
                    continue
                y0_, x0_, y1_, x1_ = (
                    int(max(y0 - (y1 - y0) * rl[0] / 100, 0)),
                    int(max(x0 - (x1 - x0) * rl[1] / 100, 0)),
                    int(min(y1 + (y1 - y0) * rl[2] / 100, h)),
                    int(min(x1 + (x1 - x0) * rl[3] / 100, w)))
                img = img[y0_:y1_, x0_:x1_, :]
                for i in range(key_point_num):
                    key_points[2 * i], key_points[2 * i + 1] = key_points[2 * i] - x0_, key_points[2 * i + 1] - y0_
                h, w = img.shape[:2]
                key_points = np.array(key_points)
                key_points = np.reshape(key_points, (-1, 2))

                eye1 = key_points[eye1_index, :]
                eye2 = key_points[eye2_index, :]
                mouth = key_points[mouth_index, :]
                r1 = np.sqrt(np.sum((eye1[2, :] - eye1[6, :]) ** 2)) / np.sqrt(
                    np.sum((eye1[0, :] - eye1[4, :]) ** 2) + 1e-8)
                r2 = np.sqrt(np.sum((eye2[2, :] - eye2[6, :]) ** 2)) / np.sqrt(
                    np.sum((eye2[0, :] - eye2[4, :]) ** 2) + 1e-8)
                r3 = np.sqrt(np.sum((mouth[2, :] - mouth[6, :]) ** 2)) / np.sqrt(
                    np.sum((mouth[0, :] - mouth[4, :]) ** 2) + 1e-8)
                eye1_xyxy = np.concatenate((np.min(eye1, axis=0), np.max(eye1, axis=0)), axis=0)
                eye2_xyxy = np.concatenate((np.min(eye2, axis=0), np.max(eye2, axis=0)), axis=0)
                mouth_xyxy = np.concatenate((np.min(mouth, axis=0), np.max(mouth, axis=0)), axis=0)
                eye1_xywh = np.concatenate(((eye1_xyxy[:2] + eye1_xyxy[2:]) / 2, eye1_xyxy[2:] - eye1_xyxy[:2]), axis=0)
                eye2_xywh = np.concatenate(((eye2_xyxy[:2] + eye2_xyxy[2:]) / 2, eye2_xyxy[2:] - eye2_xyxy[:2]), axis=0)
                mouth_xywh = np.concatenate(((mouth_xyxy[:2] + mouth_xyxy[2:]) / 2, mouth_xyxy[2:] - mouth_xyxy[:2]),
                                            axis=0)

                cv2.imwrite(face_datas_path + '/lapa_' + label_file_name.replace('.txt', '.jpg'), img)
                f0.write(os.path.relpath(face_datas_path + '/lapa_' + label_file_name.replace('.txt', '.jpg') + "\n",
                                         save_path))
                with open(face_datas_path + '/lapa_' + label_file_name, 'w') as f1:
                    if r1 > r12_thr:
                        eye1_state = 1
                        object_nums[1] += 1
                    else:
                        eye1_state = 0
                        object_nums[0] += 1
                    if r2 > r12_thr:
                        eye2_state = 1
                        object_nums[1] += 1
                    else:
                        eye2_state = 0
                        object_nums[0] += 1
                    if r3 > r3_thr:
                        mouth_state = 3
                        object_nums[3] += 1
                    else:
                        mouth_state = 2
                        object_nums[2] += 1
                    f1.write(
                        f'{eye1_state} {eye1_xywh[0] / w} {eye1_xywh[1] / h} {eye1_xywh[2] / w} {eye1_xywh[3] / h}\n')
                    f1.write(
                        f'{eye2_state} {eye2_xywh[0] / w} {eye2_xywh[1] / h} {eye2_xywh[2] / w} {eye2_xywh[3] / h}\n')
                    f1.write(
                        f'{mouth_state} {mouth_xywh[0] / w} {mouth_xywh[1] / h} {mouth_xywh[2] / w} {mouth_xywh[3] / h}')

        if hagrid_path is not None:
            det = hdface_detector(use_cuda=True)
            checkpoint = torch.load('facelandmarks.pth')
            plfd_backbone = FaceLanndInference().cuda()
            plfd_backbone.load_state_dict(checkpoint)
            plfd_backbone.eval()
            plfd_backbone = plfd_backbone.cuda()
            transform = transforms.Compose([transforms.ToTensor()])

            def get_faceld(image_path):
                image = cv2.imread(image_path)
                height, width = image.shape[:2]
                img_det = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                result = det.detect_face(img_det)
                face_list = []
                for i in range(len(result)):
                    box = result[i]['box']
                    cls = result[i]['cls']
                    pts = result[i]['pts']
                    if cls < 0.5:
                        continue
                    x1, y1, x2, y2 = box
                    # cv2.rectangle(image, (x1, y1), (x2, y2), (255, 255, 0))
                    w = x2 - x1 + 1
                    h = y2 - y1 + 1

                    size_w = int(max([w, h]) * 0.8)
                    size_h = int(max([w, h]) * 0.8)
                    cx = x1 + w // 2
                    cy = y1 + h // 2
                    x1 = cx - size_w // 2
                    x2 = x1 + size_w
                    y1 = cy - int(size_h * 0.4)
                    y2 = y1 + size_h

                    left = 0
                    top = 0
                    bottom = 0
                    right = 0
                    if x1 < 0:
                        left = -x1
                    if y1 < 0:
                        top = -y1
                    if x2 >= width:
                        right = x2 - width
                    if y2 >= height:
                        bottom = y2 - height

                    x1 = max(0, int(x1))
                    y1 = max(0, int(y1))

                    x2 = min(width, int(x2))
                    y2 = min(height, int(y2))
                    cropped = image[y1:y2, x1:x2]
                    #                print(top, bottom, left, right)
                    cropped = cv2.copyMakeBorder(cropped, top, bottom, left, right, cv2.BORDER_CONSTANT, 0)

                    # cropped = cv2.resize(cropped, (112, 112))

                    input = cv2.resize(cropped, (112, 112))
                    input = cv2.cvtColor(input, cv2.COLOR_BGR2RGB)
                    input = transform(input).unsqueeze(0).cuda()
                    landmarks = plfd_backbone(input)
                    pre_landmark = landmarks[0]
                    pre_landmark = pre_landmark.cpu().detach().numpy().reshape(-1, 2) * [size_w, size_h]

                    # cv2.rectangle(image, (x1, y1), (x2, y2), (0, 255, 0))
                    label = []
                    for (x, y) in pre_landmark.astype(np.int32):
                        label.append([x1 - left + x, y1 - bottom + y])
                        # cv2.circle(image, (x1 - left + x, y1 - bottom + y), 2, (255, 0, 255), 2)
                    face_list.append([[(x1 + x2) / 2, (y1 + y2) / 2, x2 - x1, y2 - y1], np.array(label)])
                return face_list



            num = 0



            dataset_path = hagrid_path
            images_path = dataset_path + '/images'
            ann_path = dataset_path + '/ann'
            images_class_list = os.listdir(images_path)
            for c_name in images_class_list:
                # c_name = "stop_inverted"
                json_path = ann_path + '/' + c_name + '.json'
                with open(json_path, 'r') as f:
                    ann_data = json.load(f)
                for pic_name in ann_data.keys():
                    # pic_name="a1d44c7b-a931-4d74-a76a-78819e6714d4"
                    pic_ann_data = ann_data[pic_name]
                    pic_name = pic_name + '.jpg'
                    # pic_name = "a1d44c7b-a931-4d74-a76a-78819e6714d4.jpg"

                    images_c_path = images_path + '/' + c_name
                    pic_path = images_c_path + '/' + pic_name
                    face_list = get_faceld(pic_path)
                    image = cv2.imread(pic_path)
                    h, w = image.shape[:2]

                    def get_target_xyxy(target):
                        tc, data = target
                        if tc == 0:
                            return xywh2xyxy(data[0])
                        elif tc == 1:
                            xywh=np.array([int((data[0]+data[2]/2)*w),int((data[1]+data[3]/2)*h),int(data[2]*w),int(data[3]*h)])
                            xyxy = [
                                max(0, int(xywh[0] - xywh[2] // 2)),
                                max(0, int(xywh[1] - xywh[3] // 2)),
                                min(w, int(xywh[0] + xywh[2] // 2)),
                                min(h, int(xywh[1] + xywh[3] // 2))]
                            return xyxy

                    def get_targets(t1, crop_xyxy, keep_targets, object_nums):
                        t1c, t1_data = t1
                        eye1_index = list(range(60, 68))
                        eye2_index = list(range(68, 76))
                        mouth_index = list(range(88, 96))
                        if t1c == 0:
                            face_lm = np.array(t1_data[1])
                            key_points = face_lm
                            eye1 = key_points[eye1_index, :]
                            eye2 = key_points[eye2_index, :]
                            mouth = key_points[mouth_index, :]
                            r1 = np.sqrt(np.sum((eye1[2, :] - eye1[6, :]) ** 2)) / np.sqrt(
                                np.sum((eye1[0, :] - eye1[4, :]) ** 2) + 1e-8)
                            r2 = np.sqrt(np.sum((eye2[2, :] - eye2[6, :]) ** 2)) / np.sqrt(
                                np.sum((eye2[0, :] - eye2[4, :]) ** 2) + 1e-8)
                            r3 = np.sqrt(np.sum((mouth[2, :] - mouth[6, :]) ** 2)) / np.sqrt(
                                np.sum((mouth[0, :] - mouth[4, :]) ** 2) + 1e-8)
                            eye1_xyxy = list(np.concatenate((np.min(eye1, axis=0), np.max(eye1, axis=0)),
                                                            axis=0))
                            eye2_xyxy = list(np.concatenate((np.min(eye2, axis=0), np.max(eye2, axis=0)),
                                                            axis=0))
                            mouth_xyxy = list(np.concatenate((np.min(mouth, axis=0), np.max(mouth, axis=0)),
                                                             axis=0))
                            eye1_new_xyxy = xyxy0_in_xyxy1(eye1_xyxy, crop_xyxy)
                            eye2_new_xyxy = xyxy0_in_xyxy1(eye2_xyxy, crop_xyxy)
                            mouth_new_xyxy = xyxy0_in_xyxy1(mouth_xyxy, crop_xyxy)
                            if eye1_new_xyxy is not None:
                                if r1 > r12_thr:
                                    object_nums[1] += 1
                                    keep_targets.append([1, eye1_new_xyxy])
                                else:
                                    object_nums[0] += 1
                                    keep_targets.append([0, eye1_new_xyxy])
                            if eye2_new_xyxy is not None:
                                if r2 > r12_thr:
                                    object_nums[1] += 1
                                    keep_targets.append([1, eye2_new_xyxy])
                                else:
                                    object_nums[0] += 1
                                    keep_targets.append([0, eye2_new_xyxy])
                            if mouth_new_xyxy is not None:
                                if r3 > r3_thr:
                                    object_nums[3] += 1
                                    keep_targets.append([3, mouth_new_xyxy])
                                else:
                                    object_nums[2] += 1
                                    keep_targets.append([2, mouth_new_xyxy])
                        else:
                            t1_xyxy = get_target_xyxy(t1)
                            t1_new_xyxy = xyxy0_in_xyxy1(t1_xyxy, crop_xyxy)
                            keep_targets.append([4, t1_new_xyxy])

                    targets = []
                    targets_copy=[]
                    for face_data in face_list:
                        fa_xyxy=get_target_xyxy([0,face_data])
                        if (fa_xyxy[2]-fa_xyxy[0])*(fa_xyxy[3]-fa_xyxy[1])<48*48:
                            continue
                        targets.append([0, face_data])
                        targets_copy.append([0, face_data])
                    for hand_data in pic_ann_data['bboxes']:
                        targets.append([1, hand_data])
                        targets_copy.append([1, hand_data])
                    for t_num,t0 in enumerate(targets):
                        h, w = image.shape[:2]
                        t0_xyxy = get_target_xyxy(t0)
                        rl = np.random.randint(0, 200, (4,))
                        crop_xyxy = [
                            max(0, int(t0_xyxy[0] - (t0_xyxy[2] - t0_xyxy[0]) * rl[0] / 100)),
                            max(0, int(t0_xyxy[1] - (t0_xyxy[3] - t0_xyxy[1]) * rl[1] / 100)),
                            min(w, int(t0_xyxy[2] + (t0_xyxy[2] - t0_xyxy[0]) * rl[2] / 100)),
                            min(h, int(t0_xyxy[3] + (t0_xyxy[3] - t0_xyxy[1]) * rl[3] / 100))
                        ]
                        keep_targets = []

                        for tti,t1 in enumerate(targets_copy):
                            if t_num==tti:
                                continue
                            t1_xyxy = get_target_xyxy(t1)
                            t1_new_xyxy = xyxy0_in_xyxy1(t1_xyxy, crop_xyxy)
                            if t1_new_xyxy is None:
                                continue
                            else:
                                get_targets(t1,crop_xyxy,keep_targets,object_nums)
                        get_targets(t0,crop_xyxy,keep_targets,object_nums)
                        image_=image[crop_xyxy[1]:crop_xyxy[3],crop_xyxy[0]:crop_xyxy[2],:]
                        h,w=image_.shape[:2]
                        with open(face_datas_path + '/hagrid_' + str(num) + '_'+str(t_num)+'_' + c_name+'_'+ pic_name.replace('.jpg', '.txt'),
                                  'w') as f1:
                            for ti,tg in enumerate(keep_targets):
                                lb=tg[0]
                                tg_xyxy=tg[1]
                                tg_xyxy=[
                                    tg_xyxy[0]-crop_xyxy[0],
                                    tg_xyxy[1]-crop_xyxy[1],
                                    tg_xyxy[2]-crop_xyxy[0],
                                    tg_xyxy[3]-crop_xyxy[1]
                                ]
                                tg_xywh=xyxy2xywh(tg_xyxy)
                                f1.write(f'{lb} {tg_xywh[0] / w} {tg_xywh[1] / h} {tg_xywh[2] / w} {tg_xywh[3] / h}')
                                if ti!=len(keep_targets)-1:
                                    f1.write("\n")

                            cv2.imwrite(face_datas_path + '/hagrid_' + str(num) + '_'+str(t_num)+'_' + c_name+'_'+pic_name, image_)
                            f0.write(os.path.relpath(face_datas_path + '/hagrid_' + str(num) + '_'+str(t_num)+'_'+ c_name+'_' + pic_name + '\n',save_path))
                    num += 1
    print(f"{object_nums}")

def gen_train_val_datas(train_path,rate=0.8):
    with open(train_path,"r") as f:
        lines_=f.readlines()
    folder=os.path.split(train_path)[0]
    lines=[]
    for line in lines_:
        if line[0]==".":
            line=os.path.relpath(line,folder)
        lines.append(line)
    random.shuffle(lines)
    num=len(lines)
    train_num=int(num*rate)
    train_lines=lines[:train_num]
    val_lines=lines[train_num:]
    with open(folder+'/train0.txt','w') as f:
        f.writelines(train_lines)
    with open(folder+'/val0.txt','w') as f:
        f.writelines(val_lines)

def category_sum(train_path):
    object_nums = [0] * 5
    with open(train_path,"r") as f:
        lines=f.readlines()
    for line in lines:
        line=line.strip()
        img_path=os.path.abspath(os.path.join(os.path.split(train_path)[0],line))
        label_path=os.path.splitext(img_path)[0]+".txt"
        with open(label_path,"r") as f:
            labels=f.readlines()
        for label in labels:
            label=int(label.split(" ")[0])
            object_nums[label]+=1
    print(f"{object_nums}")



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--wflw_path', type=str,required=True)
    parser.add_argument('--lapa_path', type=str, required=True)
    parser.add_argument('--hagrid_path', type=str, required=True)
    parser.add_argument('--save_path', type=str, required=True)
    args=parser.parse_args()
    gen_datas(wflw_path=args.wflw_path, lapa_path=args.lapa_path, hagrid_path=args.hagrid_path,save_path=args.save_path)
    # gen_datas(wflw_path=None, lapa_path=None, hagrid_path="../datasets/HaGrid",
    #           save_path="../datasets/abnormal_drive_0")
    # gen_train_val_datas("../datasets/abnormal_drive_0/img_paths.txt")
    # category_sum("../datasets/abnormal_drive_0/train0.txt")