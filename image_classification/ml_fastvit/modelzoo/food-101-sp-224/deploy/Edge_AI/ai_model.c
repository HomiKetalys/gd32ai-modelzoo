#include "ai_model.h"

ai_handle network1;
#if SEPARATION>0
ai_handle network2;
#endif

#if !defined(TEST_TIME_ONLY)
u32 result=-1;
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

#if SEPARATION>0
#define aiOutData ai2OutData
#else
#define aiOutData ai1OutData
#endif
#define aiInData ai1InData

#define IMG_NORM
#define bias_r -127.50911549119088f
#define bias_g -123.10415938928682f
#define bias_b -110.70924631090935f
#define weight_r 0.9178016211133283f
#define weight_g 0.938288264441751f
#define weight_b 0.9341180943775654f


ai_buffer * ai_input1;
ai_buffer * ai_output1;
#if SEPARATION>0
ai_buffer * ai_input2;
ai_buffer * ai_output2;
#endif

const char *activities[]={"apple_pie","baby_back_ribs","baklava","beef_carpaccio","beef_tartare","beet_salad","beignets","bibimbap","bread_pudding","breakfast_burrito","bruschetta","caesar_salad","cannoli","caprese_salad","carrot_cake","ceviche","cheesecake","cheese_plate","chicken_curry","chicken_quesadilla","chicken_wings","chocolate_cake","chocolate_mousse","churros","clam_chowder","club_sandwich","crab_cakes","creme_brulee","croque_madame","cup_cakes","deviled_eggs","donuts","dumplings","edamame","eggs_benedict","escargots","falafel","filet_mignon","fish_and_chips","foie_gras","french_fries","french_onion_soup","french_toast","fried_calamari","fried_rice","frozen_yogurt","garlic_bread","gnocchi","greek_salad","grilled_cheese_sandwich","grilled_salmon","guacamole","gyoza","hamburger","hot_and_sour_soup","hot_dog","huevos_rancheros","hummus","ice_cream","lasagna","lobster_bisque","lobster_roll_sandwich","macaroni_and_cheese","macarons","miso_soup","mussels","nachos","omelette","onion_rings","oysters","pad_thai","paella","pancakes","panna_cotta","peking_duck","pho","pizza","pork_chop","poutine","prime_rib","pulled_pork_sandwich","ramen","ravioli","red_velvet_cake","risotto","samosa","sashimi","scallops","seaweed_salad","shrimp_and_grits","spaghetti_bolognese","spaghetti_carbonara","spring_rolls","steak","strawberry_shortcake","sushi","tacos","takoyaki","tiramisu","tuna_tartare","waffles"};

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



void run_ai1_patch(u32 patch_idx)
{
    u32 patch_idx_h=(patch_idx/SEPARATION_SCALE);
    u32 patch_idx_w=(patch_idx%SEPARATION_SCALE);
    u32 i,j,patch_i,patch_j,idx=0;
    u8 r,g,b;

    for(patch_i=0;patch_i<AI_NETWORK_1_IN_1_HEIGHT;patch_i++)
    {
        for(patch_j=0;patch_j<AI_NETWORK_1_IN_1_WIDTH;patch_j++)
        {
            i=patch_i+AI_NETWORK_1_IN_1_HEIGHT*patch_idx_h;
            j=patch_j+AI_NETWORK_1_IN_1_WIDTH*patch_idx_w;
            img_pixel_read(i,j,&r,&g,&b);
            pixel_norm(&r,&g,&b);
            aiInData[idx++]=r;
            aiInData[idx++]=g;
            aiInData[idx++]=b;
        }
    }
#if SEPARATION>0
    AI1_Run(ai1InData,ai1OutData);
    for(patch_i=0;patch_i<AI_NETWORK_1_OUT_1_HEIGHT;patch_i++)
    {
        idx=(patch_i+AI_NETWORK_1_OUT_1_HEIGHT*patch_idx_h)*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_2_IN_1_WIDTH+AI_NETWORK_1_OUT_1_WIDTH*patch_idx_w*AI_NETWORK_2_IN_1_CHANNEL;
        memcpy(ai2InData+idx,ai1OutData+patch_i*AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH,AI_NETWORK_2_IN_1_CHANNEL*AI_NETWORK_1_OUT_1_WIDTH);
    }
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
    u32 i,index=0;;
    float max_conf=aiOutData[0];
    for(i=1;i<CLASS_NUM;i++)
    {
        if(aiOutData[i]>max_conf)
        {
            max_conf=aiOutData[i];
            index=i;
        }
    }
    result=index;
#endif
}
