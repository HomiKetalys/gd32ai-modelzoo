import torch
import numpy as np
from torch import nn

from ..Yolo_FastestV2_main import loss_bbox_iou as bbox_iou
from ..Yolo_FastestV2_main import smooth_BCE

layer_index = [0, 0, 0, 1, 1, 1]

def build_target(preds, targets, cfg, device):
    tcls, tbox, indices, anch = [], [], [], []
    # anchor box数量, 当前batch的标签数量
    anchor_num, label_num = cfg["anchor_num"], targets.shape[0]

    # 加载anchor配置
    anchors = np.array(cfg["anchors"])
    anchors = torch.from_numpy(anchors.reshape(len(preds) // 3, anchor_num, 2)).to(device)

    # modified
    gain = torch.ones(7, device=device).long()

    at = torch.arange(anchor_num, device=device).float().view(anchor_num, 1).repeat(1, label_num)
    targets = torch.cat((targets.repeat(anchor_num, 1, 1), at[:, :, None]), 2)

    g = 0.5  # bias
    off = torch.tensor([[0, 0],
                        [1, 0], [0, 1], [-1, 0], [0, -1],  # j,k,l,m
                        # [1, 1], [1, -1], [-1, 1], [-1, -1],  # jk,jm,lk,lm
                        ], device=device).float() * g  # offsets

    for i, pred in enumerate(preds):
        if i % 3 == 0:
            # 输出特征图的维度
            _, _, h, w = pred.shape

            assert cfg["width"] / w == cfg["height"] / h, "特征图宽高下采样不一致"

            # 计算下采样倍数
            stride = cfg["width"] / w

            # 该尺度特征图对应的anchor配置
            anchors_cfg = anchors[layer_index[i]] / stride

            # 将label坐标映射到特征图上
            gain[2:6] = torch.tensor(pred.shape)[[3, 2, 3, 2]]

            gt = targets * gain

            if label_num:
                # anchor iou匹配
                r = gt[:, :, 4:6] / anchors_cfg[:, None]
                j = torch.max(r, 1. / r).max(2)[0] < 2

                t = gt[j]
                # 扩充维度并复制数据
                # Offsets
                gxy = t[:, 2:4]  # grid xy
                gxi = gain[[2, 3]] - gxy  # inverse
                j, k = ((gxy % 1. < g) & (gxy > 1.)).T
                l, m = ((gxi % 1. < g) & (gxi > 1.)).T
                j = torch.stack((torch.ones_like(j), j, k, l, m))
                t = t.repeat((5, 1, 1))[j]
                offsets = (torch.zeros_like(gxy)[None] + off[:, None])[j]
            else:
                t = targets[0]
                offsets = 0

            # Define
            b, c = t[:, :2].long().T  # image, class
            gxy = t[:, 2:4]  # grid xy
            gwh = t[:, 4:6]  # grid wh
            gij = (gxy - offsets).long()
            gi, gj = gij.T  # grid xy indices

            # Append
            a = t[:, 6].long()  # anchor indices
            indices.append((b, a, gj.clamp_(0, gain[3] - 1), gi.clamp_(0, gain[2] - 1)))  # image, anchor, grid indices
            tbox.append(torch.cat((gxy - gij, gwh), 1))  # box
            anch.append(anchors_cfg[a])  # anchors
            tcls.append(c)  # class

    return tcls, tbox, indices, anch


def compute_loss(preds, targets, cfg, device):
    balance = [1.0, 0.4]

    ft = torch.cuda.FloatTensor if preds[0].is_cuda else torch.Tensor
    lcls, lbox, lobj = ft([0]), ft([0]), ft([0])

    # 定义obj和cls的损失函数
    BCEcls = nn.CrossEntropyLoss()
    BCEobj = nn.BCEWithLogitsLoss(pos_weight=torch.tensor(1.0, device=device))

    cp, cn = smooth_BCE(eps=0.0)

    # 构建gt
    tcls, tbox, indices, anchors = build_target(preds, targets, cfg, device)

    for i, pred in enumerate(preds):
        # 计算reg分支loss
        if i % 3 == 0:
            pred = pred.reshape(pred.shape[0], cfg["anchor_num"], -1, pred.shape[2], pred.shape[3])
            pred = pred.permute(0, 1, 3, 4, 2)

            # 判断当前batch数据是否有gt
            if len(indices):
                b, a, gj, gi = indices[layer_index[i]]
                nb = b.shape[0]

                if nb:
                    ps = pred[b, a, gj, gi]

                    pxy = ps[:, :2].sigmoid() * 2. - 0.5
                    pwh = (ps[:, 2:4].sigmoid() * 2) ** 2 * anchors[layer_index[i]]
                    pbox = torch.cat((pxy, pwh), 1)  # predicted box
                    ciou = bbox_iou(pbox.t(), tbox[layer_index[i]], x1y1x2y2=False,
                                    CIoU=True)  # ciou(prediction, target)
                    lbox += (1.0 - ciou).mean()

        # 计算obj分支loss
        elif i % 3 == 1:
            pred = pred.reshape(pred.shape[0], cfg["anchor_num"], -1, pred.shape[2], pred.shape[3])
            pred = pred.permute(0, 1, 3, 4, 2)

            tobj = torch.zeros_like(pred[..., 0])  # target obj

            # 判断当前batch数据是否有gt
            if len(indices):
                b, a, gj, gi = indices[layer_index[i]]
                nb = b.shape[0]

                if nb:
                    ps = pred[b, a, gj, gi]
                    tobj[b, a, gj, gi] = 1.0

            lobj += BCEobj(pred[..., 0], tobj) * balance[layer_index[i]]  # obj loss

        # 计算cls分支loss
        elif i % 3 == 2:
            pred = pred.reshape(pred.shape[0], 1, -1, pred.shape[2], pred.shape[3])
            pred = pred.permute(0, 1, 3, 4, 2)

            if len(indices):
                b, a, gj, gi = indices[layer_index[i]]
                nb = b.shape[0]

                if nb:
                    ps = pred[b, 0, gj, gi]

                    if ps.size(1) > 1:
                        # t = torch.full_like(ps[:, :], cn)  # targets
                        # t[range(nb), tcls[layer_index[i]]] = cp
                        lcls += BCEcls(ps[:, :], tcls[layer_index[i]]) / cfg["classes"]  # BCE
        else:
            print("error")
            raise

    lbox *= 3.2
    lobj *= 64
    lcls *= 32
    loss = lbox + lobj + lcls

    return lbox, lobj, lcls, loss