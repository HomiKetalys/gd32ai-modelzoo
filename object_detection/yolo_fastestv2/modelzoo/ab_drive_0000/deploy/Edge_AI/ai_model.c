#include "ai_model.h"

ai_handle network1;
#if SEPARATION>0
ai_handle network2;
#endif

#if !defined(TEST_TIME_ONLY)
ObjectResult objects[MAX_OBJ_NUM];
ObjectResult results[MAX_OBJ_NUM];
u32 object_num;
#endif


#if defined(TO_EXT)
signed char *ai1InData;
ai1_out_type *ai1OutData;
ai_u8 *activations=EXT_RAM_ADDR;
#if SEPARATION>0
signed char *ai2InData=EXT_RAM_ADDR+ACTIVATION_SIZE;
float *ai2OutData;
#endif
#else
signed char *ai1InData;
ai1_out_type *ai1OutData;
AI_ALIGNED(32)
ai_u8 activations[ACTIVATION_SIZE];
#if SEPARATION>0
AI_ALIGNED(32)
signed char ai2InData[AI_NETWORK_2_IN_1_SIZE];
float *ai2OutData;
#endif
#endif

#if defined(ENABLE_SPARSE_PATCH)
AI_ALIGNED(32)
u8 background[INPUT_HEIGHT*INPUT_WIDTH*3];
#endif

#if SEPARATION>0
#define aiOutData ai2OutData
#else
#define aiOutData ai1OutData
#endif
#define aiInData ai1InData


ai_buffer * ai_input1;
ai_buffer * ai_output1;
#if SEPARATION>0
ai_buffer * ai_input2;
ai_buffer * ai_output2;
#endif

const char *activities[]={"CE","OE","CM","OM","HD"};

void AI_Error_Handler()
{
    while(1);
}

void AI_Init()
{
    ai_error err1,err2;
    rcu_periph_clock_enable(RCU_CRC);

    /* Create a local array with the addresses of the activations buffers */
//    const ai_handle act_addr[] = { activations0,activations1 };
    const ai_handle act_addr[] = { activations };

    /* Create an instance of the model */
    err1 = ai_network_1_create_and_init(&network1, act_addr, NULL);
    if (err1.type != AI_ERROR_NONE) {
        AI_Error_Handler();
    }
#if SEPARATION>0
    err2 = ai_network_2_create_and_init(&network2, act_addr, NULL);
    if (err2.type != AI_ERROR_NONE) {
        AI_Error_Handler();
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

void AI1_Run(signed char *pIn,ai1_out_type *pOut)
{
    ai_i32 batch;

    /* Update IO handlers with the data payload */
    ai_input1[0].data = AI_HANDLE_PTR(pIn);
    ai_output1[0].data = AI_HANDLE_PTR(pOut);

    batch = ai_network_1_run(network1, ai_input1, ai_output1);
    if (batch != 1) {
        AI_Error_Handler();
    }
}

#if SEPARATION>0
void AI2_Run(signed char *pIn, float *pOut)
{
    ai_i32 batch;

    /* Update IO handlers with the data payload */
    ai_input2[0].data = AI_HANDLE_PTR(pIn);
    ai_output2[0].data = AI_HANDLE_PTR(pOut);

    batch = ai_network_2_run(network2, ai_input2, ai_output2);
    if (batch != 1) {
        AI_Error_Handler();
    }
}

void int_fix(signed char *data)
{
    for(int i=0;i<AI_NETWORK_2_IN_1_SIZE;i++)
    {
        clip((*(data+i))*FIX_FACTOR0+FIX_FACTOR1,-128.f,127.f,data+i);
    }
}

#endif



#if defined(ENABLE_SPARSE_PATCH)

bool point_in_results(u32 i,u32 j)
{
    int k;
    for(k=0;k<object_num;k++)
    {
        if(j>=(u32)results[k].bbox.x_min&&j<(u32)results[k].bbox.x_max&&i>=(u32)results[k].bbox.y_min&&i<(u32)results[k].bbox.y_max)
        {
            return true;
        }
    }
    return false;
}

void update_background()
{
    u32 i,j,idx=0;
    u8 r,g,b;
    for(i=0;i<INPUT_HEIGHT;i++)
    {
        for(j=0;j<INPUT_WIDTH;j++)
        {
            if(point_in_results(i,j))
            {
                idx+=3;
                continue;
            }
            img_pixel_read(i,j,&r,&g,&b);
            background[idx]=background[idx]/4*3+r/4;
            idx++;
            background[idx]=background[idx]/4*3+g/4;
            idx++;
            background[idx]=background[idx]/4*3+b/4;
            idx++;
        }
    }
}
#endif

void run_ai1_patch(u32 patch_idx)
{
    u32 patch_idx_h=(patch_idx/SEPARATION_SCALE);
    u32 patch_idx_w=(patch_idx%SEPARATION_SCALE);
    u32 i,j,patch_i,patch_j,idx=0;
    u8 r,g,b;
#if defined(ENABLE_SPARSE_PATCH)
    u32 idxb;
    u32 diff_count=0;
    u32 diff_val;
#endif
    for(patch_i=0;patch_i<AI_NETWORK_1_IN_1_HEIGHT;patch_i++)
    {
#if defined(ENABLE_SPARSE_PATCH)
        idxb=(patch_i+AI_NETWORK_1_IN_1_HEIGHT*patch_idx_h)*INPUT_WIDTH*3+3*AI_NETWORK_1_IN_1_WIDTH*patch_idx_w;
#endif
        for(patch_j=0;patch_j<AI_NETWORK_1_IN_1_WIDTH;patch_j++)
        {
            i=patch_i+AI_NETWORK_1_IN_1_HEIGHT*patch_idx_h;
            j=patch_j+AI_NETWORK_1_IN_1_WIDTH*patch_idx_w;
            img_pixel_read(i,j,&r,&g,&b);
#if defined(ENABLE_SPARSE_PATCH)
            diff_val=0;
            diff_val+=abs_diff(r,background[idxb]);
            idxb++;
            diff_val+=abs_diff(g,background[idxb]);
            idxb++;
            diff_val+=abs_diff(b,background[idxb]);
            idxb++;
            diff_count+=diff_val>MAX_DIFF_VAL;
#endif
            pixel_norm(&r,&g,&b);
            aiInData[idx++]=r;
            aiInData[idx++]=g;
            aiInData[idx++]=b;
        }
    }
#if SEPARATION>0

#if defined(ENABLE_SPARSE_PATCH)
    if(diff_count>MAX_DIFF_COUNT)
    {
#endif
        AI1_Run(ai1InData,ai1OutData);
        for(patch_i=0;patch_i<AI_NETWORK_1_OUT_1_HEIGHT;patch_i++)
        {
            idx=(patch_i+AI_NETWORK_1_OUT_1_HEIGHT*patch_idx_h)*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_2_IN_1_WIDTH+AI_NETWORK_1_OUT_1_WIDTH*patch_idx_w*AI_NETWORK_2_IN_1_CHANNEL;
            memcpy(ai2InData+idx,ai1OutData+patch_i*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH,AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH);
        }
#if defined(ENABLE_SPARSE_PATCH)
    }
    else
    {
        for(patch_i=0;patch_i<AI_NETWORK_1_OUT_1_HEIGHT;patch_i++)
        {
            idx=(patch_i+AI_NETWORK_1_OUT_1_HEIGHT*patch_idx_h)*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_2_IN_1_WIDTH+AI_NETWORK_1_OUT_1_WIDTH*patch_idx_w*AI_NETWORK_2_IN_1_CHANNEL;
            memset(ai2InData+idx,128,AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH);
        }
    }
#endif
#else
    AI1_Run(ai1InData,ai1OutData);
#endif
}

void run_ai1()
{
    for(u32 i=0;i<SEPARATION_SCALE*SEPARATION_SCALE;i++)
    {
        run_ai1_patch(i);
    }
}


void AI_Run()
{
    run_ai1();
#if SEPARATION>0
    int_fix(ai2InData);
    AI2_Run(ai2InData,ai2OutData);
#endif
#if !defined(TEST_TIME_ONLY)
    handle_preds(aiOutData,CONF_THR);
    nms(objects,results,&object_num,NMS_THR);
#if defined(ENABLE_SPARSE_PATCH)
    update_background();
#endif
#endif
}