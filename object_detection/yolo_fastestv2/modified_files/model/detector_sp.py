import numpy as np
import tensorflow as tf
import os
from ..Yolo_FastestV2_main import *
from .backbone.shufflenetv2_sp import *
from ..Yolo_FastestV2_main import Detector
from common_utils.utils import modelRuner


class DetectorOrtTf():
    def __init__(self, cfg, model_root):
        super(DetectorOrtTf, self).__init__()
        name_list = os.listdir(model_root)
        model_front_path = None
        model_post_path = None
        model_path = None
        self.separation = cfg["separation"]
        self.separation_scale = cfg["separation_scale"]
        for name in name_list:
            if name.endswith(".tflite") or name.endswith(".onnx"):
                if "front" in name:
                    model_front_path = os.path.join(model_root, name)
                elif "post" in name:
                    model_post_path = os.path.join(model_root, name)
                else:
                    model_path = os.path.join(model_root, name)
        if model_path is None:
            assert os.path.splitext(model_front_path)[-1] == os.path.splitext(model_post_path)[-1]
            self.model_type = os.path.splitext(model_front_path)[-1]
            self.model_front = modelRuner(model_front_path)
            self.model_post = modelRuner(model_post_path)
            if model_front_path.endswith(".tflite"):
                self.fix = self.model_front.model_output_details[0]["quantization"][0] / \
                           self.model_post.model_input_details["quantization"][0]
                print(f"\rfix_factor:{self.fix}")
            self.sp = 1
        else:
            self.model = modelRuner(model_path)
            self.sp = 0
            self.model_type = os.path.splitext(model_path)[-1]

    def __call__(self, inputs):
        pred_list0 = []
        pred_list1 = []
        for x in inputs:
            if self.model_type == ".tflite":
                x = x.permute(1, 2, 0).cpu().numpy() * 255
                x = x.astype("uint8")
                h, w, c = x.shape[:3]
            else:
                x = x.cpu().numpy()
                c,h,w = x.shape[:3]
            h0 = h
            w0 = w
            if self.sp == 1:
                y_list = []
                for r in range(0, self.separation_scale):
                    for c in range(0, self.separation_scale):
                        x_ = x[None, r * h // self.separation_scale:(r + 1) * h // self.separation_scale,
                             c * w // self.separation_scale:(c + 1) * w // self.separation_scale, :]
                        y = self.model_front(x_)[0]
                        y_list.append(y)
                h, w, c = y_list[0].shape[:3]
                y = np.zeros((h * self.separation_scale, w * self.separation_scale, c), dtype="int8")
                id = 0
                for r in range(0, self.separation_scale):
                    for c in range(0, self.separation_scale):
                        y[r * h:(r + 1) * h, c * w:(c + 1) * w, :] = y_list[id]
                        id += 1
                y = y[None, :, :, :].astype('float32')
                y = (y + 128) * self.fix - 128
                y = y.astype('int8')
                out = self.model_post(y)

            else:
                out = self.model(x[None, :, :, :])
                # out = np.concatenate(out,axis=2)
                # pred_list0.append(out)
            if self.model_type == ".tflite":
                c = out.shape[2]
                out0, out1 = out[:, :(h0 * w0 // 2 ** 8), :], out[:, (h0 * w0 // 2 ** 8):, :],
                out0 = np.reshape(out0, (1, h0 // 2 ** 4, w0 // 2 ** 4, c))
                out1 = np.reshape(out1, (1, h0 // 2 ** 5, w0 // 2 ** 5, c))
                out0 = torch.tensor(out0, device=inputs.device).permute(0, 3, 1, 2)
                out1 = torch.tensor(out1, device=inputs.device).permute(0, 3, 1, 2)
            else:
                c = out.shape[1]
                out0, out1 = out[:, :, :(h0 * w0 // 2 ** 8)], out[:, :, (h0 * w0 // 2 ** 8):],
                out0 = np.reshape(out0, (1, c, h0 // 2 ** 4, w0 // 2 ** 4))
                out1 = np.reshape(out1, (1, c, h0 // 2 ** 5, w0 // 2 ** 5))
                out0 = torch.tensor(out0, device=inputs.device)
                out1 = torch.tensor(out1, device=inputs.device)
            pred_list0.append(out0)
            pred_list1.append(out1)
        # out0=np.concatenate(pred_list0,axis=0)
        out0 = torch.cat(pred_list0, dim=0)
        # return out0
        out1 = torch.cat(pred_list1, dim=0)
        return out0[:, :12, :, ], out0[:, 12:15, :, ], out0[:, 15:, :, ], out1[:, :12, :, ], out1[:, 12:15, :, ], out1[
                                                                                                                  :,
                                                                                                                  15:,
                                                                                                                  :, ]


class DetectorSp(Detector):
    def __init__(self, classes, anchor_num, load_param, export_onnx=0, separation=False, separation_scale=2,
                 to_tflite=True, no_sigmoid=False):
        super(Detector, self).__init__()
        self.modified_init(classes, anchor_num, load_param, export_onnx, separation, separation_scale)
        self.to_tflite = to_tflite
        self.separation = separation
        self.no_sigmoid = no_sigmoid

    def forward(self, x):
        if self.export_onnx == 0:
            C2, C3 = self.backbone(x, self.export_onnx)

            cls_2, obj_2, reg_2, cls_3, obj_3, reg_3 = self.fpn(C2, C3)

            out_reg_2 = self.output_reg_layers(reg_2)
            out_obj_2 = self.output_obj_layers(obj_2)
            out_cls_2 = self.output_cls_layers(cls_2)

            out_reg_3 = self.output_reg_layers(reg_3)
            out_obj_3 = self.output_obj_layers(obj_3)
            out_cls_3 = self.output_cls_layers(cls_3)

            return out_reg_2, out_obj_2, out_cls_2, out_reg_3, out_obj_3, out_cls_3
        else:
            if self.separation == 0:
                C2, C3 = self.backbone(x, 0)

                cls_2, obj_2, reg_2, cls_3, obj_3, reg_3 = self.fpn(C2, C3)

                out_reg_2 = self.output_reg_layers(reg_2)
                out_obj_2 = self.output_obj_layers(obj_2)
                out_cls_2 = self.output_cls_layers(cls_2)

                out_reg_3 = self.output_reg_layers(reg_3)
                out_obj_3 = self.output_obj_layers(obj_3)
                out_cls_3 = self.output_cls_layers(cls_3)

                # out_reg_2 = out_reg_2.sigmoid()
                # out_obj_2 = out_obj_2.sigmoid()
                # out_cls_2 = F.softmax(out_cls_2, dim=1)
                #
                # out_reg_3 = out_reg_3.sigmoid()
                # out_obj_3 = out_obj_3.sigmoid()
                # out_cls_3 = F.softmax(out_cls_3, dim=1)
                #
                # print("export onnx ...")
                # return torch.cat((out_reg_2, out_obj_2, out_cls_2), 1).permute(0, 2, 3, 1), \
                #        torch.cat((out_reg_3, out_obj_3, out_cls_3), 1).permute(0, 2, 3, 1)

                if not self.no_sigmoid:
                    out_reg_2 = out_reg_2.sigmoid()
                    out_obj_2 = out_obj_2.sigmoid()
                    out_cls_2 = F.softmax(out_cls_2, dim=1)

                    out_reg_3 = out_reg_3.sigmoid()
                    out_obj_3 = out_obj_3.sigmoid()
                    out_cls_3 = F.softmax(out_cls_3, dim=1)

                out_2 = torch.cat((out_reg_2, out_obj_2, out_cls_2), dim=1)
                bs, ch, h, w = out_2.shape
                out_2 = torch.reshape(out_2, (bs, ch, h * w))

                out_3 = torch.cat((out_reg_3, out_obj_3, out_cls_3), dim=1)
                bs, ch, h, w = out_3.shape
                out_3 = torch.reshape(out_3, (bs, ch, h * w))
                out = torch.cat((out_2, out_3), dim=2)
                if not self.to_tflite:
                    out = out.permute(0, 2, 1)
                print("export onnx ...")
                return out

            elif self.export_onnx == 1:
                x = self.backbone(x, 1)
                return x
            else:
                C2, C3 = self.backbone(x, self.export_onnx)

                cls_2, obj_2, reg_2, cls_3, obj_3, reg_3 = self.fpn(C2, C3)

                out_reg_2 = self.output_reg_layers(reg_2)
                out_obj_2 = self.output_obj_layers(obj_2)
                out_cls_2 = self.output_cls_layers(cls_2)

                out_reg_3 = self.output_reg_layers(reg_3)
                out_obj_3 = self.output_obj_layers(obj_3)
                out_cls_3 = self.output_cls_layers(cls_3)

                if not self.no_sigmoid:
                    out_reg_2 = out_reg_2.sigmoid()
                    out_obj_2 = out_obj_2.sigmoid()
                    out_cls_2 = F.softmax(out_cls_2, dim=1)

                    out_reg_3 = out_reg_3.sigmoid()
                    out_obj_3 = out_obj_3.sigmoid()
                    out_cls_3 = F.softmax(out_cls_3, dim=1)

                out_2 = torch.cat((out_reg_2, out_obj_2, out_cls_2), dim=1)
                bs, ch, h, w = out_2.shape
                out_2 = torch.reshape(out_2, (bs, ch, h * w))

                out_3 = torch.cat((out_reg_3, out_obj_3, out_cls_3), dim=1)
                bs, ch, h, w = out_3.shape
                out_3 = torch.reshape(out_3, (bs, ch, h * w))
                out = torch.cat((out_2, out_3), dim=2)
                if not self.to_tflite:
                    out = out.permute(0, 2, 1)
                print("export onnx ...")
                return out

    # modify from __init__ of Detector
    def modified_init(self, classes, anchor_num, load_param, export_onnx=0, separation=False, separation_scale=2):
        out_depth = 72
        stage_out_channels = [-1, 24, 48, 96, 192]

        self.export_onnx = export_onnx
        # modified
        # self.backbone = ShuffleNetV2(stage_out_channels, load_param)
        self.backbone = ShuffleNetV2Sp(stage_out_channels, load_param, separation, separation_scale)
        self.fpn = LightFPN(stage_out_channels[-2] + stage_out_channels[-1], stage_out_channels[-1], out_depth)

        self.output_reg_layers = nn.Conv2d(out_depth, 4 * anchor_num, 1, 1, 0, bias=True)
        self.output_obj_layers = nn.Conv2d(out_depth, anchor_num, 1, 1, 0, bias=True)
        self.output_cls_layers = nn.Conv2d(out_depth, classes, 1, 1, 0, bias=True)


if __name__ == "__main__":
    model = Detector(80, 3, False)
    test_data = torch.rand(1, 3, 352, 352)
    torch.onnx.export(model,  # model being run
                      test_data,  # model input (or a tuple for multiple inputs)
                      "test.onnx",  # where to save the model (can be a file or file-like object)
                      export_params=True,  # store the trained parameter weights inside the model file
                      opset_version=11,  # the ONNX version to export the model to
                      do_constant_folding=True)  # whether to execute constant folding for optimization
