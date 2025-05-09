#include "ai_model.h"

#if defined(X_CUBE_AI)
    ai_handle network1;
    #if SEPARATION>0
        ai_handle network2;
    #endif
#endif

#if defined(TO_EXT)
    signed char *ai1InData;
    ai1_out_type *ai1OutData;
    #if defined(X_CUBE_AI)
        ai_u8 *activations=EXT_RAM_ADDR;
    #endif
    #if SEPARATION>0
        #if defined(X_CUBE_AI)
            signed char *ai2InData=EXT_RAM_ADDR+ACTIVATION_SIZE;
        #elif defined(TinyEngine)
            signed char *ai2InData=EXT_RAM_ADDR+ACTIVATION_SIZE;
        #endif
        float *ai2OutData;
    #endif
#else
    signed char *ai1InData;
    ai1_out_type *ai1OutData;
    #if defined(X_CUBE_AI)
        AI_ALIGNED(32)
        ai_u8 activations[ACTIVATION_SIZE];
    #elif defined(TinyEngine)||defined(MTE)
        AI_ALIGNED(32)
        signed char buffer[ACTIVATION_SIZE];
    #endif
    #if SEPARATION>0
        AI_ALIGNED(32)
        signed char ai2InData[INPUT_2_H*INPUT_2_W*INPUT_2_C];
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

#if defined(X_CUBE_AI)
    ai_buffer * ai_input1;
    ai_buffer * ai_output1;
    #if SEPARATION>0
        ai_buffer * ai_input2;
        ai_buffer * ai_output2;
    #endif
#endif

ACTIVITIES_CODE

#if defined(OD_MODEL)
    ObjectResult objects[MAX_OBJ_NUM];
    ObjectResult results[MAX_OBJ_NUM];
    u32 object_num=0;
#elif defined(IC_MODEL)
    u32 result=0;
    float conf;
#endif

#if defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ) && defined(TinyEngine)
void arm_nn_mat_mult_kernel_s8_s16_4col(){}
void mat_mult_kernel_s8_s16_reordered(){}
void get_kernel_buffer(){}
void arm_nn_mat_mult_kernel_s8_s16_reordered(){}
void arm_s8_to_s16_unordered_with_offset(){}
void __aeabi_d2h(){}
#endif

void AI_Error_Handler()
{
    while(1);
}

u8 init=0;

void AI_Init()
{
    if(init)
        return;
    #if defined(X_CUBE_AI)
        ai_error err1,err2;
        #if defined(NEED_RCU)
            rcu_enable();
        #endif
        /* Create a local array with the addresses of the activations buffers */
//        const ai_handle act_addr[] = { activations0,activations1 };
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
    #elif defined(TinyEngine)
        ai1InData=get_network_1_Input();
        ai1OutData=get_network_1_Output();
        #if SEPARATION>0
            ai2OutData = get_network_2_Output();
        #endif
    #elif defined(MTE)
        set_mte_mem_addr(&buffer[0]);
        ai1InData=get_network_1_input_addr();
        ai1OutData=get_network_1_output_addr();
        #if SEPARATION>0
            ai2OutData = get_network_2_output_addr();
        #endif
    #endif
    init=1;
}

void AI1_Run(signed char *pIn,ai1_out_type *pOut)
{
    #if defined(X_CUBE_AI)
        ai_i32 batch;

        /* Update IO handlers with the data payload */
        ai_input1[0].data = AI_HANDLE_PTR(pIn);
        ai_output1[0].data = AI_HANDLE_PTR(pOut);

        batch = ai_network_1_run(network1, ai_input1, ai_output1);
        if (batch != 1) {
            AI_Error_Handler();
        }
    #elif defined(TinyEngine)
        network_1_invoke(0);
    #elif defined(MTE)
        network_1();
    #endif
}

#if SEPARATION>0
    void AI2_Run(signed char *pIn, float *pOut)
    {
        #if defined(X_CUBE_AI)
            ai_i32 batch;

            /* Update IO handlers with the data payload */
            ai_input2[0].data = AI_HANDLE_PTR(pIn);
            ai_output2[0].data = AI_HANDLE_PTR(pOut);

            batch = ai_network_2_run(network2, ai_input2, ai_output2);
            if (batch != 1) {
                AI_Error_Handler();
            }
        #elif defined(TinyEngine)
            signed char *inp=get_network_2_Input();
            memcpy(inp,pIn,INPUT_2_H*INPUT_2_W*INPUT_2_C);
            network_2_invoke(0);
        #elif defined(MTE)
            signed char *inp=get_network_2_input_addr();
            memcpy(inp,pIn,INPUT_2_H*INPUT_2_W*INPUT_2_C);
            network_2();
        #endif
    }

    void int_fix(signed char *data)
    {
        for(int i=0;i<INPUT_2_H*INPUT_2_W*INPUT_2_C;i++)
        {
            clip((*(data+i))*FIX_FACTOR0+FIX_FACTOR1,-128.f,127.f,data+i);
        }
    }
#endif



#if defined(ENABLE_SPARSE_PATCH)
    uint8_t point_in_results(u32 i,u32 j)
    {
        int k;
        for(k=0;k<object_num;k++)
        {
            if(j>=(u32)results[k].bbox.x_min&&j<(u32)results[k].bbox.x_max&&i>=(u32)results[k].bbox.y_min&&i<(u32)results[k].bbox.y_max)
            {
                return 1;
            }
        }
        return 0;
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
    for(patch_i=0;patch_i<INPUT_1_H;patch_i++)
    {
        #if defined(ENABLE_SPARSE_PATCH)
            idxb=(patch_i+INPUT_1_H*patch_idx_h)*INPUT_WIDTH*3+3*INPUT_1_W*patch_idx_w;
        #endif
        for(patch_j=0;patch_j<INPUT_1_W;patch_j++)
        {
            i=patch_i+INPUT_1_H*patch_idx_h;
            j=patch_j+INPUT_1_W*patch_idx_w;
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
            #if defined(RGB_MODE)
                aiInData[idx++]=r;
                aiInData[idx++]=g;
                aiInData[idx++]=b;
            #elif defined(BGR_MODE)
                aiInData[idx++]=b;
                aiInData[idx++]=g;
                aiInData[idx++]=r;
            #else
                #error "unkown image channel mode"
            #endif
        }
    }
    #if SEPARATION>0
        #if defined(ENABLE_SPARSE_PATCH)
            if(diff_count>MAX_DIFF_COUNT)
            {
        #endif
                AI1_Run(ai1InData,ai1OutData);
                for(patch_i=0;patch_i<OUTPUT_1_H;patch_i++)
                {
                    idx=(patch_i+OUTPUT_1_H*patch_idx_h)*INPUT_2_C*INPUT_2_W+OUTPUT_1_W*patch_idx_w*OUTPUT_1_C;
                    memcpy(ai2InData+idx,ai1OutData+patch_i*INPUT_2_C*OUTPUT_1_W,INPUT_2_C*OUTPUT_1_W);
                }
        #if defined(ENABLE_SPARSE_PATCH)
            }
            else
            {
                for(patch_i=0;patch_i<OUTPUT_1_H;patch_i++)
                {
                    idx=(patch_i+OUTPUT_1_H*patch_idx_h)*INPUT_2_C*INPUT_2_W+OUTPUT_1_W*patch_idx_w*OUTPUT_1_C;
                    memset(ai2InData+idx,128,INPUT_2_C*OUTPUT_1_W);
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
    AI_Init();
    run_ai1();
    #if SEPARATION>0
        int_fix(ai2InData);
        AI2_Run(ai2InData,ai2OutData);
    #endif
    #if defined(OD_MODEL)
        handle_preds(aiOutData,CONF_THR);
        nms(objects,results,&object_num,NMS_THR);
    #elif defined(IC_MODEL)
        u32 i,index=0;;
        float max_conf=aiOutData[0];
        for(i=1;i<OUT_CHANNEL;i++)
        {
            if(aiOutData[i]>max_conf)
            {
                max_conf=aiOutData[i];
                index=i;
            }
        }
        result=index;
        conf=max_conf;
    #endif
    #if defined(ENABLE_SPARSE_PATCH)
        update_background();
    #endif
}