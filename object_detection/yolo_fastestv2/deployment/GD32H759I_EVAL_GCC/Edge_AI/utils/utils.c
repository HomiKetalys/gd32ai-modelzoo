#include "ai_model.h"
#if !defined(TEST_TIME_ONLY)
float anchors[12]={9.192727272727273, 14.101818181818182, 27.54909090909091, 37.44, 40.516363636363636, 100.58909090909091, 92.29818181818182, 56.89454545454546, 95.68727272727273, 156.03636363636363, 203.57818181818183, 188.26909090909092};
extern u32 object_num;
extern ObjectResult objects[];
extern ObjectResult results[];

#if defined(yolofastestv2)
void handle_preds(float *preds,float conf_thr)
{
    u32 i,j,stage,pos_x,pos_y;
    object_num=0;
    for(i=0;i<AI_NETWORK_OUT_1_HEIGHT;i++)
    {
        if(i<(INPUT_WIDTH*INPUT_HEIGHT)/(16*16))
        {
            pos_x=i%(INPUT_WIDTH/16);
            pos_y=i/(INPUT_WIDTH/16);
            stage=0;

        }
        else
        {
            pos_x=(i-(INPUT_WIDTH*INPUT_HEIGHT)/(16*16))%(INPUT_WIDTH/32);
            pos_y=(i-(INPUT_WIDTH*INPUT_HEIGHT)/(16*16))/(INPUT_WIDTH/32);
            stage=1;
        }
        u32 cls_index=0;
        float cls=0;
        if(AI_NETWORK_OUT_1_CHANNEL==15||AI_NETWORK_OUT_1_CHANNEL==16)
            cls=1;
        else
        {
            for(j=15;j<AI_NETWORK_OUT_1_CHANNEL;j++)
            {
                if(*(preds+AI_NETWORK_OUT_1_CHANNEL*i+j)>cls)
                {
                    cls=*(preds+AI_NETWORK_OUT_1_CHANNEL*i+j);
                    cls_index=j-15;
                }
            }
        }
        for(j=0;j<3;j++)
        {
            float *reg=preds+AI_NETWORK_OUT_1_CHANNEL*i+j*4;
            float obj=*(preds+AI_NETWORK_OUT_1_CHANNEL*i+12+j);
            if(obj*cls>conf_thr)
            {
                *(reg)=(*(reg)*2-0.5f+pos_x)*(stage+1)*16;
                *(reg+1)=(*(reg+1)*2-0.5f+pos_y)*(stage+1)*16;
                *(reg+2)=((*(reg+2)*2)*(*(reg+2)*2))*anchors[stage*6+j*2];
                *(reg+3)=((*(reg+3)*2)*(*(reg+3)*2))*anchors[stage*6+j*2+1];
                objects[object_num].confi=obj*cls;
                objects[object_num].bbox.x_min=*(reg)-*(reg+2)/2;
                objects[object_num].bbox.y_min=*(reg+1)-*(reg+3)/2;
                objects[object_num].bbox.x_max=*(reg)+*(reg+2)/2;
                objects[object_num].bbox.y_max=*(reg+1)+*(reg+3)/2;
                objects[object_num].cls_index=cls_index;
                object_num++;
            }
        }
    }
}
#elif defined(yolov10)
void handle_preds(float *preds,float conf_thr)
{
    u32 i,j,k;
    object_num=0;
    for(i=0;i<AI_NETWORK_OUT_1_HEIGHT;i++)
    {

        float xyxy_offset[4];
        for(j=0;j<4;j++)
        {
            xyxy_offset[j]=0;
            for(k=0;k<16;k++)
            {
                xyxy_offset[j]+=k*preds[i*AI_NETWORK_OUT_1_CHANNEL+j*16+k];
            }
        }
        u32 cls_index=0;
        float cls=0;
        for(j=64;j<AI_NETWORK_OUT_1_CHANNEL;j++)
        {
            if(preds[i*AI_NETWORK_OUT_1_CHANNEL+j]>cls)
            {
                cls=preds[i*AI_NETWORK_OUT_1_CHANNEL+j];
                cls_index=j-64;
            }
        }
        if(cls>conf_thr)
        {
            objects[object_num].bbox.x_min=(0.5+i%12-xyxy_offset[0])*16;
            objects[object_num].bbox.y_min=(0.5+i/12-xyxy_offset[1])*16;
            objects[object_num].bbox.x_max=(0.5+i%12+xyxy_offset[2])*16;
            objects[object_num].bbox.y_max=(0.5+i/12+xyxy_offset[3])*16;
            objects[object_num].confi=cls;
            objects[object_num].cls_index=cls_index;
            object_num++;
        }
    }
}
#else
#error "The expected model name is undefined"
#endif

int compare(const ObjectResult a, const ObjectResult b)
{
    return a.confi < b.confi ? 1 : -1;
}

float iou(ObjectResult a, ObjectResult b)
{
    float x1 = a.bbox.x_min > b.bbox.x_min ? a.bbox.x_min : b.bbox.x_min;  // std::max
    float y1 = a.bbox.y_min > b.bbox.y_min ? a.bbox.y_min : b.bbox.y_min;  // std::max
    float x2 = a.bbox.x_max > b.bbox.x_max ? b.bbox.x_max : a.bbox.x_max;  // std::min
    float y2 = a.bbox.y_max > b.bbox.y_max ? b.bbox.y_max : a.bbox.y_max;  // std::min

    if (x2 <= x1 || y2 <= y1) return 0;

    float a_width  = a.bbox.x_max - a.bbox.x_min;
    float a_height = a.bbox.y_max - a.bbox.y_min;
    float b_width =  b.bbox.x_max - b.bbox.x_min;
    float b_heihgt = b.bbox.y_max - b.bbox.y_min;

    float inter_area = (x2 - x1) * (y2 - y1);
    float outer_area = ((a_width * a_height) + b_width * b_heihgt - inter_area);
    if(outer_area==0)
    {
        return 0;
    }
    float iou = inter_area / outer_area;

    return iou;
}


void nms(ObjectResult object[], ObjectResult result[], u32* total, float nmsThreshold)
{
    qsort(object, *total, sizeof(ObjectResult), compare);

    for (uint16_t i = 0; i < *total; ++i)
    {
        uint16_t index = 1;
        result[i] = object[i];
        for (uint16_t j = i + 1; j < *total; ++j)
        {
            if (iou(result[i], object[j]) < nmsThreshold)
            {
                object[index] = object[j];
                index += 1;
            }
        }

        *total = index;

        if (index == 1)
        {
            *total = i + 1;
        }
    }
}
#endif