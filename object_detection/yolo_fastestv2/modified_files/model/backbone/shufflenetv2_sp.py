import torch
from torch import nn

from ...Yolo_FastestV2_main import ShuffleNetV2,ShuffleV2Block


class ShuffleV2Block_(ShuffleV2Block):
    def channel_shuffle(self, x):
        num_channels = x.shape[1]
        assert (num_channels % 4 == 0)
        x1, x2 = x[:, 0::2, :, :], x[:, 1::2, :, :]
        return x1, x2


class ShuffleNetV2Sp(ShuffleNetV2):
    def __init__(self, stage_out_channels, load_param, separation=0, separation_scale=2, img_channels=3):
        super(ShuffleNetV2,self).__init__()
        self.part_of_init(stage_out_channels,load_param)
        assert separation_scale % 2 == 0

        if load_param == 1:
            print("load nothing")

        self.separation = separation
        self.separation_scale = separation_scale
        self.stage_list = [self.first_conv, self.maxpool, self.stage2, self.stage3, self.stage4]
        self.stage_out_channels = [stage_out_channels[1], stage_out_channels[1], stage_out_channels[2],
                                   stage_out_channels[3], stage_out_channels[4]]
        assert separation < len(self.stage_list)

    def seperate(self, x):
        bs, ch, h, w = x.shape

        # loop crop method
        # x_list = []
        # for r in range(0, self.separation_scale):
        #     for c in range(0, self.separation_scale):
        #         x_list.append(x[:, :,
        #                       r * h // self.separation_scale:(r + 1) * h // self.separation_scale,
        #                       c * w // self.separation_scale:(c + 1) * w // self.separation_scale
        #                       ])
        # x = torch.cat(x_list, dim=0)

        # change dimension method
        x = x.view(bs, ch, self.separation_scale, h // self.separation_scale, self.separation_scale,
                   w // self.separation_scale)
        x = x.permute(0, 2, 4, 1, 3, 5)
        x = x.reshape(bs * self.separation_scale ** 2, ch, h // self.separation_scale, w // self.separation_scale)
        return x

    def montage(self,x):
        bs, ch, h, w = x.shape
        # loop crop method
        # x_list = torch.split(x, bs // self.separation_scale ** 2, dim=0)
        # xr_list = []
        # for c in range(0, self.separation_scale):
        #     xr_list.append(torch.cat(x_list[c * self.separation_scale:(c + 1) * self.separation_scale], dim=3))
        # x = torch.cat(xr_list, dim=2)

        # change dimension method
        x = x.view(bs // self.separation_scale ** 2, self.separation_scale, self.separation_scale, ch, h, w)
        x = x.permute(0, 3, 1, 4, 2, 5)
        x = x.reshape(bs // self.separation_scale ** 2, ch, self.separation_scale * h,
                      self.separation_scale * w)
        return x

    def forward(self, x, export=0):

        if export == 0 or (export > 0 and self.separation == 0):
            if self.separation:
                x=self.seperate(x)
            C2 = None
            for i, stg in enumerate(self.stage_list):
                if i == len(self.stage_list) - 1:
                    C2 = x
                x = stg(x)
                if self.separation == i + 1:
                    x=self.montage(x)
            C3 = x
            if C2 is not None:
                return C2, C3
            else:
                return C3
        elif export == 1:
            for i, stg in enumerate(self.stage_list):
                x = stg(x)
                if self.separation == i + 1:
                    return x
        elif export == 2:
            C2 = None
            for i, stg in enumerate(self.stage_list):
                if i == len(self.stage_list) - 1:
                    C2 = x
                if self.separation < i + 1:
                    x = stg(x)
            C3 = x
            if C2 is not None:
                return C2, C3
            else:
                return C3
        elif export == 3:
            assert self.separation > 0
            assert self.separation_scale > 1
            bs, ch, h, w = x.shape
            x_list = []
            for r in range(0, self.separation_scale):
                for c in range(0, self.separation_scale):
                    x_list.append(x[:, :,
                                  r * h // self.separation_scale:(r + 1) * h // self.separation_scale,
                                  c * w // self.separation_scale:(c + 1) * w // self.separation_scale
                                  ])
            C2 = None
            fuse = False
            for i, stg in enumerate(self.stage_list):
                if i == len(self.stage_list) - 1:
                    C2 = x
                if fuse:
                    x = stg(x)
                else:
                    for id, x in enumerate(x_list):
                        x = stg(x)
                        x_list[id] = x
                if self.separation == i + 1:
                    xr_list = []
                    for c in range(0, self.separation_scale):
                        xr_list.append(
                            torch.cat(x_list[c * self.separation_scale:(c + 1) * self.separation_scale], dim=3))
                    x = torch.cat(xr_list, dim=2)
                    fuse = True
            C3 = x
            if C2 is not None:
                return C2, C3
            else:
                return C3

    # modify from __init__ of ShuffleNetV2
    def part_of_init(self,stage_out_channels, load_param):
        self.stage_repeats = [4, 8, 4]
        self.stage_out_channels = stage_out_channels

        # building first layer
        input_channel = self.stage_out_channels[1]
        self.first_conv = nn.Sequential(
            nn.Conv2d(3, input_channel, 3, 2, 1, bias=False),
            nn.BatchNorm2d(input_channel),
            nn.ReLU(inplace=True),
        )

        self.maxpool = nn.MaxPool2d(kernel_size=3, stride=2, padding=1)

        stage_names = ["stage2", "stage3", "stage4"]
        for idxstage in range(len(self.stage_repeats)):
            numrepeat = self.stage_repeats[idxstage]
            output_channel = self.stage_out_channels[idxstage + 2]
            stageSeq = []
            for i in range(numrepeat):
                if i == 0:
                    # modified
                    # stageSeq.append(ShuffleV2Block(input_channel, output_channel,
                    #                                mid_channels=output_channel // 2, ksize=3, stride=2))
                    stageSeq.append(ShuffleV2Block_(input_channel, output_channel,
                                                   mid_channels=output_channel // 2, ksize=3, stride=2))
                else:
                    # modified
                    # stageSeq.append(ShuffleV2Block(input_channel // 2, output_channel,
                    #                                mid_channels=output_channel // 2, ksize=3, stride=1))
                    stageSeq.append(ShuffleV2Block_(input_channel//2, output_channel,
                                                   mid_channels=output_channel // 2, ksize=3, stride=1))
                input_channel = output_channel
            setattr(self, stage_names[idxstage], nn.Sequential(*stageSeq))

        if load_param == False:
            self._initialize_weights()
        else:
            print("load param...")

    #modify from _initialize_weights from ShuffleNetV2
    def _initialize_weights(self):
        print("initialize_weights...")
        device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.load_state_dict(torch.load("./modified_files/Yolo_FastestV2_main/model/backbone/backbone.pth", map_location=device), strict = True)


if __name__ == "__main__":
    model = ShuffleNetV2()
    print(model)
    test_data = torch.rand(1, 3, 320, 320)
    test_outputs = model(test_data)
    for out in test_outputs:
        print(out.size())
