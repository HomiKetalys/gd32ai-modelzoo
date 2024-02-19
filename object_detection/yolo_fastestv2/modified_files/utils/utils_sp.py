import os

import torch

from ..Yolo_FastestV2_main import xywh2xyxy, bbox_iou, compute_ap, get_batch_statistics, \
    non_max_suppression, make_grid
from tqdm import tqdm
import numpy as np
import torch.nn.functional as F


# 加载data
def load_datafile(data_path):
    # 需要配置的超参数
    cfg = {"model_name": None,

           "epochs": None,
           "steps": None,
           "batch_size": None,
           "subdivisions": None,
           "learning_rate": None,

           "pre_weights": None,
           "classes": None,
           "width": None,
           "height": None,
           "anchor_num": None,
           # modified
           #
           "separation": 0,
           "separation_scale": 2,
           "anchors": None,
           "conf_thr": 0.01,
           "nms_thr": 0.4,
           "iou_thr": 0.5,
           # modified
           #
           "label_flag": "",
           "val": None,
           "train": None,
           "names": None
           }

    assert os.path.exists(data_path), "请指定正确配置.data文件路径"

    # 指定配置项的类型
    list_type_key = ["anchors", "steps"]
    str_type_key = ["model_name", "val", "train", "names", "pre_weights", "label_flag"]
    int_type_key = ["epochs", "batch_size", "classes", "width",
                    "height", "anchor_num", "subdivisions", "separation", "separation_scale"]
    float_type_key = ["learning_rate", "iou_thr", "nms_thr", "conf_thr"]

    # 加载配置文件
    with open(data_path, 'r') as f:
        for line in f.readlines():
            if line == '\n' or line[0] == "[":
                continue
            else:
                data = line.strip().split("=")
                # 配置项类型转换
                if data[0] in cfg:
                    if data[0] in int_type_key:
                        cfg[data[0]] = int(data[1])
                    elif data[0] in str_type_key:
                        cfg[data[0]] = data[1]
                    elif data[0] in float_type_key:
                        cfg[data[0]] = float(data[1])
                    elif data[0] in list_type_key:
                        cfg[data[0]] = [float(x) for x in data[1].split(",")]
                    else:
                        print("配置文件有错误的配置项")
                else:
                    print("%s配置文件里有无效配置项:%s" % (data_path, data))
    return cfg


# 特征图后处理
def handel_preds(preds, cfg, device, one_norm=True):
    # 加载anchor配置
    anchors = np.array(cfg["anchors"])
    anchors = torch.from_numpy(anchors.reshape(len(preds) // 3, cfg["anchor_num"], 2)).to(device)

    output_bboxes = []
    layer_index = [0, 0, 0, 1, 1, 1]

    for i in range(len(preds) // 3):
        bacth_bboxes = []
        reg_preds = preds[i * 3]
        obj_preds = preds[(i * 3) + 1]
        cls_preds = preds[(i * 3) + 2]

        for r, o, c in zip(reg_preds, obj_preds, cls_preds):
            r = r.permute(1, 2, 0)
            r = r.reshape(r.shape[0], r.shape[1], cfg["anchor_num"], -1)

            o = o.permute(1, 2, 0)
            o = o.reshape(o.shape[0], o.shape[1], cfg["anchor_num"], -1)

            c = c.permute(1, 2, 0)
            c = c.reshape(c.shape[0], c.shape[1], 1, c.shape[2])
            c = c.repeat(1, 1, 3, 1)

            anchor_boxes = torch.zeros(r.shape[0], r.shape[1], r.shape[2], r.shape[3] + c.shape[3] + 1)

            # 计算anchor box的cx, cy
            grid = make_grid(r.shape[0], r.shape[1], cfg, device)
            stride = cfg["height"] / r.shape[0]
            if one_norm:
                anchor_boxes[:, :, :, :2] = ((r[:, :, :, :2].sigmoid() * 2. - 0.5) + grid) * stride

                # 计算anchor box的w, h
                anchors_cfg = anchors[i]
                anchor_boxes[:, :, :, 2:4] = (r[:, :, :, 2:4].sigmoid() * 2) ** 2 * anchors_cfg  # wh

                # 计算obj分数
                anchor_boxes[:, :, :, 4] = o[:, :, :, 0].sigmoid()

                # 计算cls分数
                anchor_boxes[:, :, :, 5:] = F.softmax(c[:, :, :, :], dim=3)
            else:
                anchor_boxes[:, :, :, :2] = ((r[:, :, :, :2] * 2. - 0.5) + grid) * stride

                # 计算anchor box的w, h
                anchors_cfg = anchors[i]
                anchor_boxes[:, :, :, 2:4] = (r[:, :, :, 2:4] * 2) ** 2 * anchors_cfg  # wh

                # 计算obj分数
                anchor_boxes[:, :, :, 4] = o[:, :, :, 0]

                # 计算cls分数
                anchor_boxes[:, :, :, 5:] = c

            # torch tensor 转为 numpy array
            anchor_boxes = anchor_boxes.cpu().detach().numpy()
            bacth_bboxes.append(anchor_boxes)

            # n, anchor num, h, w, box => n, (anchor num*h*w), box
        bacth_bboxes = torch.from_numpy(np.array(bacth_bboxes))
        bacth_bboxes = bacth_bboxes.view(bacth_bboxes.shape[0], -1, bacth_bboxes.shape[-1])

        output_bboxes.append(bacth_bboxes)

        # merge
    output = torch.cat(output_bboxes, 1)

    return output

def ap_per_class(tp, conf, pred_cls, target_cls):
    """ Compute the average precision, given the recall and precision curves.
    Source: https://github.com/rafaelpadilla/Object-Detection-Metrics.
    # Arguments
        tp:    True positives (list).
        conf:  Objectness value from 0-1 (list).
        pred_cls: Predicted object classes (list).
        target_cls: True object classes (list).
    # Returns
        The average precision as computed in py-faster-rcnn.
    """

    # Sort by objectness
    i = np.argsort(-conf)
    tp, conf, pred_cls = tp[i], conf[i], pred_cls[i]

    # Find unique classes
    unique_classes = np.unique(target_cls)

    # Create Precision-Recall curve and compute AP for each class
    ap, p, r = [], [], []
    for c in unique_classes:
        i = pred_cls == c
        n_gt = (target_cls == c).sum()  # Number of ground truth objects
        n_p = i.sum()  # Number of predicted objects

        if n_p == 0 and n_gt == 0:
            continue
        elif n_p == 0 or n_gt == 0:
            ap.append(0)
            r.append(0)
            p.append(0)
        else:
            # Accumulate FPs and TPs
            fpc = (1 - tp[i]).cumsum()
            tpc = (tp[i]).cumsum()

            # Recall
            recall_curve = tpc / (n_gt + 1e-16)
            r.append(recall_curve[-1])

            # Precision
            precision_curve = tpc / (tpc + fpc)
            p.append(precision_curve[-1])

            # AP from recall-precision curve
            ap.append(compute_ap(recall_curve, precision_curve))

    # Compute F1 score (harmonic mean of precision and recall)
    p, r, ap = np.array(p), np.array(r), np.array(ap)
    f1 = 2 * p * r / (p + r + 1e-16)

    return p,r,ap,f1


# 模型评估
def evaluation(val_dataloader, cfg, model, device, conf_thres=0.01, nms_thresh=0.4, iou_thres=0.5, one_norm=True):
    labels = []
    sample_metrics = []  # List of tuples (TP, confs, pred)
    pbar = tqdm(val_dataloader)

    for imgs, targets in pbar:
        imgs = imgs.to(device).float() / 255.0
        targets = targets.to(device)

        # Extract labels
        labels += targets[:, 1].tolist()
        # Rescale target
        targets[:, 2:] = xywh2xyxy(targets[:, 2:])
        targets[:, 2:] *= torch.tensor([cfg["width"], cfg["height"], cfg["width"], cfg["height"]]).to(device)

        # 对预测的anchorbox进行nms处理
        with torch.no_grad():
            # imgs=imgs[:,::-1,:,:]
            preds = model(imgs)

            # 特征图后处理:生成anchorbox
            output = handel_preds(preds, cfg, device,one_norm)
            output_boxes = non_max_suppression(output, conf_thres=conf_thres, iou_thres=nms_thresh)

        #     output = decode_predictions(preds, normalize=True, org_img_width=cfg["width"],
        #                                 org_img_height=cfg["height"])
        #     output_boxes = do_nms(output, nms_thresh=cfg["nms_thr"], confidence_thresh=cfg["conf_thr"])
        #     output_boxes=np.concatenate((output_boxes[1][:,1:],output_boxes[1][:,:1]),axis=1)
        # output_boxes=torch.tensor(output_boxes,device=device)[None,:,:]
        sample_metrics += get_batch_statistics(output_boxes, targets, iou_thres, device)
        pbar.set_description("Evaluation model:")

    if len(sample_metrics) == 0:  # No detections over whole validation set.
        print("---- No detections over whole validation set ----")
        return None

    # Concatenate sample statistics
    true_positives, pred_scores, pred_labels = [np.concatenate(x, 0) for x in list(zip(*sample_metrics))]
    metrics_output = ap_per_class(true_positives, pred_scores, pred_labels, labels)

    return metrics_output


def decode_predictions(predictions, normalize=True, org_img_height=None, org_img_width=None):
    '''
    Retrieve object bounding box coordinates from predicted offsets and anchor box coordinates
    Arguments:
        predictions: output of SSD-based human detection model, a tensor of [None, #boxes, 1+n_classes+4+4]
        normalize: coordinates are normalized or not, bool
        org_img_height: original image height, used if normalize=True, integer
        org_img_width: original image width, used if normalize=True, integer
    Returns:
        predictions_decoded_raw: object bounding boxes and categories, a tensor of [None, #boxes, 1+n_classes+4]
    '''
    # predictions=np.concatenate(predictions,axis=2)
    predictions_decoded_raw = np.copy(predictions[:, :, :-4])
    # offsets are normalized with height and width of anchor boxes
    predictions_decoded_raw[:, :, [-4, -2]] *= np.expand_dims(predictions[:, :, -2] - predictions[:, :, -4], axis=-1)
    predictions_decoded_raw[:, :, [-3, -1]] *= np.expand_dims(predictions[:, :, -1] - predictions[:, :, -3], axis=-1)
    predictions_decoded_raw[:, :, -4:] += predictions[:, :, -4:]

    if normalize:
        predictions_decoded_raw[:, :, [-4, -2]] *= org_img_width
        predictions_decoded_raw[:, :, [-3, -1]] *= org_img_height

    return predictions_decoded_raw


def do_nms(preds_decoded, nms_thresh=0.45, confidence_thresh=0.5):
    '''
    Non-maximum suppression, removing overlapped bounding boxes based on IoU metric and keeping bounding boxes with the highest score
    Arguments:
        preds_decoded: return of decode_predictions function, a tensor of [None, #boxes, 1+n_classes+4]
        nms_thresh: IoU threshold to remove overlapped bounding boxes, a float between 0 and 1
        confidence_thresh: minimum score to keep bounding boxes, a float between 0 and 1
    Returns:
        final_preds: detection results after non-maximum suppression
    '''
    n_boxes = int(preds_decoded.shape[1])
    batch_size = int(preds_decoded.shape[0])
    n_classes_bg = int(preds_decoded.shape[2]) - 4

    final_preds = dict()

    def interval_overlap(interval_a, interval_b):
        '''
        Find overlap between two intervals
        Arguments:
            interval_a: [x1, x2]
            interval_b: [x3, x4]
        Returns:
            overlapped distance
        '''
        x1, x2 = interval_a
        x3, x4 = interval_b
        if x3 < x1:
            if x4 < x1:
                return 0
            else:
                return min(x2, x4) - x1
        else:
            if x2 < x3:
                return 0
            else:
                return min(x2, x4) - x3

    def bbox_iou(box1, box2):
        '''
        Find IoU between two boxes
        Arguments:
            box1 = [xmin, ymin, xmax, ymax]
            box2 = [xmin, ymin, xmax, ymax]
        Returns:
            iou similarity
        '''
        intersect_w = interval_overlap([box1[0], box1[2]], [box2[0], box2[2]])
        intersect_h = interval_overlap([box1[1], box1[3]], [box2[1], box2[3]])
        intersect = intersect_w * intersect_h
        w1, h1 = box1[2] - box1[0], box1[3] - box1[1]
        w2, h2 = box2[2] - box2[0], box2[3] - box2[1]
        union = w1 * h1 + w2 * h2 - intersect
        return float(intersect) / union

    for b_item in preds_decoded:  # loop for each batch item
        for c in range(1, n_classes_bg):  # loop for each object category
            single_class = b_item[:,
                           [c, -4, -3, -2, -1]]  # retrieve predictions [score, xmin, ymin, xmax, ymax] for category c
            threshold_met = single_class[
                single_class[:, 0] > confidence_thresh]  # filter predictions with minimum confidence score threshold
            if threshold_met.shape[0] > 0:
                # sort confidence score in descending order
                sorted_indices = np.argsort([-elm[0] for elm in threshold_met])
                for i in range(len(sorted_indices)):  # loop for bounding boxes in order of highest score
                    index_i = sorted_indices[i]
                    if threshold_met[index_i, 0] == 0:  # verify if this box was processed
                        continue
                    box_i = threshold_met[index_i, -4:]  # get [xmin, ymin, xmax, ymax] of box_i
                    for j in range(i + 1, len(sorted_indices)):
                        index_j = sorted_indices[j]
                        if threshold_met[index_j, 0] == 0:  # verify if this box was processed
                            continue
                        box_j = threshold_met[index_j, -4:]  # get [xmin, ymin, xmax, ymax] of box_j
                        if bbox_iou(box_i,
                                    box_j) >= nms_thresh:  # check if two boxes are overlapped based on IoU metric
                            threshold_met[index_j, 0] = 0  # if Yes, remove bounding box with smaller confidence score
                threshold_met = threshold_met[threshold_met[:, 0] > 0]
            final_preds[c] = threshold_met  # detection results after non-maximum suppression of object category c

    return final_preds
