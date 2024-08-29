import os
import argparse
import torch
from submodules.Yolo_FastestV2.train import train
from common_utils.utils import LogSaver


class TrainLogSaver(LogSaver):
    def __init__(self, config_path, save_folder_path):
        super(TrainLogSaver, self).__init__(config_path, save_folder_path)
        self.max_map = 0

    def log_and_save(self, precision, recall, AP, f1, epoch, model, cfg):
        info0 = f"epoch:{epoch:04d}"
        info1 = (f"Precision:{list(precision)}\n"
                 f"Recall:{list(recall)}\n"
                 f"AP:{list(AP)}\n"
                 f"F1:{list(f1)}")
        info2 = (f"Precision_mean:{precision.mean()}\n"
                 f"Recall_mean:{recall.mean()}\n"
                 f"AP_mean:{AP.mean()}\n"
                 f"F1_mean:{f1.mean()}")
        print(info0)
        print(info1)
        print(info2)

        torch.save(model.state_dict(), os.path.join(self.result_path, "%s-%d-epoch-%fap-model.pth" %
                                                    (cfg["model_name"], epoch, AP.mean())))
        if AP.mean() > self.max_map:
            self.max_map = AP.mean()
            torch.save(model.state_dict(), os.path.join(self.result_path, "best.pth"))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=str, default='configs/coco_person.data',
                        help='Specify training profile *.data')
    opt = parser.parse_args()
    lger = TrainLogSaver(opt.config, "results/train")
    lger.collect_prints(train, opt, lger.log_and_save)


if __name__ == '__main__':
    main()
