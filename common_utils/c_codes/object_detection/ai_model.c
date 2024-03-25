#include "ai_model.h"

ai_handle network1;
#if SEPARATION>0
ai_handle network2;
#endif

#ifndef TEST_TIME
ObjectResult objects[AI_NETWORK_OUT_1_HEIGHT*3];
u32 object_num;
ObjectResult results[AI_NETWORK_OUT_1_HEIGHT*3];
#endif


#ifdef TO_EXT
signed char *ai1InData;
ai1_out_type *ai1OutData;
#if SEPARATION>0
signed char *ai2InData=EXT_RAM_ADDR+ACTIVATION_SIZE;
float *ai2OutData;
#endif
ai_u8 *activations=EXT_RAM_ADDR;
#else
signed char *ai1InData;
ai1_out_type *ai1OutData;
#if SEPARATION>0
signed char ai2InData[AI_NETWORK_2_IN_1_SIZE];
float *ai2OutData;
#endif
ai_u8 activations[ACTIVATION_SIZE];
#endif

#if SEPARATION>0
#define aiOutData ai2OutData
#else
#define aiOutData ai1OutData
#endif
#define aiInData ai1InData

IMG_NORM_CODE


ai_buffer * ai_input1;
ai_buffer * ai_output1;
#if SEPARATION>0
ai_buffer * ai_input2;
ai_buffer * ai_output2;
#endif

static u32 img_width;
static u32 img_height;
static u32 img_type;

void Error_Handler()
{
    while(1);
}

void AI1_Run(signed char *pIn,ai1_out_type *pOut)
{
    ai_i32 batch;
    ai_error err;

    /* Update IO handlers with the data payload */
    ai_input1[0].data = AI_HANDLE_PTR(pIn);
    ai_output1[0].data = AI_HANDLE_PTR(pOut);

    batch = ai_network_1_run(network1, ai_input1, ai_output1);
    if (batch != 1) {
        err = ai_network_1_get_error(network1);
        printf("AI ai_network1_run error - type=%d code=%d\r\n", err.type, err.code);
        Error_Handler();
    }
}

#if SEPARATION>0
void AI2_Run(signed char *pIn, float *pOut)
{
    ai_i32 batch;
    ai_error err;

    /* Update IO handlers with the data payload */
    ai_input2[0].data = AI_HANDLE_PTR(pIn);
    ai_output2[0].data = AI_HANDLE_PTR(pOut);

    batch = ai_network_2_run(network2, ai_input2, ai_output2);
    if (batch != 1) {
        err = ai_network_2_get_error(network2);
        printf("AI ai_network2_run error - type=%d code=%d\r\n", err.type, err.code);
        Error_Handler();
    }
}
#endif


void AI_Init(u32 img_width_,u32 img_height_,u32 img_type_)
{
    ai_error err1,err2;
    rcu_periph_clock_enable(RCU_CRC);
    img_width=img_width_;
    img_height=img_height_;
    img_type=img_type_;

    /* Create a local array with the addresses of the activations buffers */
//    const ai_handle act_addr[] = { activations0,activations1 };
    const ai_handle act_addr[] = { activations };

    /* Create an instance of the model */
    err1 = ai_network_1_create_and_init(&network1, act_addr, NULL);
    if (err1.type != AI_ERROR_NONE) {
        printf("ai_network_create error - type=%d code=%d\r\n", err1.type, err1.code);
        Error_Handler();
    }
#if SEPARATION>0
    err2 = ai_network_2_create_and_init(&network2, act_addr, NULL);
    if (err2.type != AI_ERROR_NONE) {
        printf("ai_network_create error - type=%d code=%d\r\n", err2.type, err2.code);
        Error_Handler();
    }
#endif
    ai_input1 = ai_network_1_inputs_get(network1, NULL);
    ai_output1 = ai_network_1_outputs_get(network1, NULL);
    ai1InData=ai_input1[0].data;
    ai1OutData=ai_output1[0].data;
#if SEPARATION>0
    ai_input2 = ai_network_2_inputs_get(network2, NULL);
    ai_output2 = ai_network_2_outputs_get(network2, NULL);
    ai2OutData=ai_output2[0].data;
#endif
}


__inline void RGB565ToRGB888C(u16 *rgb565, u8* r, u8* g, u8* b)
{
    *r = ((0xF800 & *rgb565) >> 8) ;
    *g = ((0x07E0 & *rgb565) >> 3 ) ;
    *b = ((0x001F & *rgb565) << 3 ) ;
}

__inline float clip(float x,float low,float high)
{
    x=x>low?x:low;
    return x<high?x:high;
}

__inline void pixel_trans(u16 *rgb565, u8* r, u8* g, u8* b)
{
    RGB565ToRGB888C(rgb565,r,g,b);
#ifdef IMG_NORM_BIAS_ONLY
    *r=*r+bias;
    *g=*g+bias;
    *b=*b+bias;
#elif defined(IMG_NORM)
    *r=clip((((float)*r)*weight_r+bias_r,-128.f,127.f);
    *g=clip((((float)*g)*weight_g+bias_g,-128.f,127.f);
    *b=clip((((float)*b)*weight_b+bias_b,-128.f,127.f);
#endif

}

void run_ai1_patch(u8 *img,u32 patch_index)
{
    u32 offh=(patch_index/SEPARATION_SCALE);
    u32 offw=(patch_index%SEPARATION_SCALE);
    u32 step=0;
    if(img_type==0)
        step=2;
    u32 i,j,index;
    u8 r,g,b;

    for(i=0;i<AI_NETWORK_1_IN_1_HEIGHT;i++)
    {
        for(j=0;j<AI_NETWORK_1_IN_1_WIDTH;j++)
        {
            index=(img_height*img_width-1-(i+AI_NETWORK_1_IN_1_HEIGHT*offh)*img_width-(j+AI_NETWORK_1_IN_1_WIDTH*offw))*step;
            pixel_trans((u16*) (img+index),&r,&g,&b);
            aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3]=b;
            aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3+1]=g;
            aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3+2]=r;
        }
    }

    AI1_Run(ai1InData,ai1OutData);
#if SEPARATION>0
    u32 k;

    for(i=0;i<AI_NETWORK_2_IN_1_HEIGHT/SEPARATION_SCALE;i++)
    {
        for(j=0;j<AI_NETWORK_2_IN_1_WIDTH/SEPARATION_SCALE;j++)
        {
            for(k=0;k<AI_NETWORK_2_IN_1_CHANNEL;k++)
            {
                u32 index1=(i+AI_NETWORK_1_OUT_1_HEIGHT*offh)*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_2_IN_1_WIDTH+(j+AI_NETWORK_1_OUT_1_WIDTH*offw)*AI_NETWORK_2_IN_1_CHANNEL+k;
                u32 index2=i*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH+j*AI_NETWORK_2_IN_1_CHANNEL+k;
                ai2InData[index1]=ai1OutData[index2];
            }
        }
    }
#endif
}

void run_ai1(u8 *img)
{
    for(u32 i=0;i<SEPARATION_SCALE*SEPARATION_SCALE;i++)
    {
        run_ai1_patch(img,i);
    }
}
#if SEPARATION>0
void int_fix(signed char *data,u32 n)
{
    for(int i=0;i<n;i++)
    {
        *(data+i)=clip((*(data+i))*FIX_FACTOR0+FIX_FACTOR1,-128.f,127.f);
    }
}
#endif

void AI_Run(u8 *img)
{
    run_ai1(img);
#if SEPARATION>0
    int_fix(ai2InData,AI_NETWORK_2_IN_1_SIZE);
    AI2_Run(ai2InData,ai2OutData);
#endif
#ifndef TEST_TIME
    handle_preds(aiOutData,CONF_THR);
    nms(objects,results,(uint16_t *) &object_num,NMS_THR);
#endif
}
