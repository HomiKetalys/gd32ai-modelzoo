import os
import json
from tqdm import tqdm
import argparse




def convert(size, box):
    dw = 1. / (size[0])
    dh = 1. / (size[1])
    x = box[0] + box[2] / 2.0
    y = box[1] + box[3] / 2.0
    w = box[2]
    h = box[3]

    x = x * dw
    w = w * dw
    y = y * dh
    h = h * dh
    return (x, y, w, h)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--images_path', default='../../datasets/coco2017/images/val2017', type=str)
    parser.add_argument('--json_file', default='../../datasets/coco2017/annotations/instances_val2017.json', type=str)
    parser.add_argument('--ana_txt_save_path', default='../../datasets/coco2017/coco_person/val2017', type=str)
    parser.add_argument('--out_txt_path', default='../../datasets/coco2017/val2017_person.txt', type=str)
    parser.add_argument('--export_categorys', default='all', type=str)
    arg = parser.parse_args()

    images_path=arg.images_path
    json_file = arg.json_file
    ana_txt_save_path = arg.ana_txt_save_path
    out_txt_path=arg.out_txt_path
    # exports_categorys="all"
    if arg.export_categorys=="all":
        exports_categorys="all"
    else:
        exports_categorys = [int(x) for x in arg.export_categorys.split(" ")]

    data = json.load(open(json_file, 'r'))
    if not os.path.exists(ana_txt_save_path):
        os.makedirs(ana_txt_save_path)

    id_map = {}  # coco数据集的id不连续！重新映射一下再输出！
    for i, category in enumerate(data['categories']):
        id_map[category['id']] = i

    # 通过事先建表来降低时间复杂度
    max_id = 0
    for img in data['images']:
        max_id = max(max_id, img['id'])
    # 注意这里不能写作 [[]]*(max_id+1)，否则列表内的空列表共享地址
    img_ann_dict = [[] for i in range(max_id + 1)]
    for i, ann in enumerate(data['annotations']):
        img_ann_dict[ann['image_id']].append(i)

    f0=open(out_txt_path,"w")
    for img in tqdm(data['images']):
        filename = img["file_name"]
        img_width = img["width"]
        img_height = img["height"]
        img_id = img["id"]
        head, tail = os.path.splitext(filename)
        ana_txt_name = head + ".txt"  # 对应的txt名字，与jpg一致
        # if len(img_ann_dict[img_id])>0:
        if exports_categorys=="all":
            f_txt = open(os.path.join(ana_txt_save_path, ana_txt_name), 'w')
            '''for ann in data['annotations']:
                if ann['image_id'] == img_id:
                    box = convert((img_width, img_height), ann["bbox"])
                    f_txt.write("%s %s %s %s %s\n" % (id_map[ann["category_id"]], box[0], box[1], box[2], box[3]))'''
            # 这里可以直接查表而无需重复遍历
            for ann_id in img_ann_dict[img_id]:
                ann = data['annotations'][ann_id]
                box = convert((img_width, img_height), ann["bbox"])
                f_txt.write("%s %s %s %s %s\n" % (id_map[ann["category_id"]], box[0], box[1], box[2], box[3]))
            f_txt.close()
            f0.write(os.path.relpath(os.path.join(images_path,filename),os.path.split(out_txt_path)[0])+'\n')
        else:
            '''for ann in data['annotations']:
                if ann['image_id'] == img_id:
                    box = convert((img_width, img_height), ann["bbox"])
                    f_txt.write("%s %s %s %s %s\n" % (id_map[ann["category_id"]], box[0], box[1], box[2], box[3]))'''
            # 这里可以直接查表而无需重复遍历
            have=0
            for ann_id in img_ann_dict[img_id]:
                ann = data['annotations'][ann_id]
                box = convert((img_width, img_height), ann["bbox"])
                if id_map[ann["category_id"]] in exports_categorys:
                    have=1
                    with open(os.path.join(ana_txt_save_path, ana_txt_name), 'a') as f_txt:
                        f_txt.write("%s %s %s %s %s\n" % (id_map[ann["category_id"]], box[0], box[1], box[2], box[3]))
            if have:
                f0.write(os.path.relpath(os.path.join(images_path,filename),os.path.split(out_txt_path)[0])+'\n')

