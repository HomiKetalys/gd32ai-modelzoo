#ifndef _AI_MODEL_
#define _AI_MODEL_

#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "math.h"

//Put the buffer of the model into external memory.Ensure that your external memory is initialized before the model runs
//#define TO_EXT

//enable sparse patch
//#define ENABLE_SPARSE_PATCH


MODEL_CONF_CODE

CONF_THR_CODE

NMS_THR_CODE

RCU_CODE

INFER_FRAME_CODE

MODEL_INFO_CODE


#if (!defined(IC_MODEL))&&(!defined(OD_MODEL))
    #error "unkown type of model"
#endif

#if defined(TinyEngine)
    #if defined (__GNUC__)||(( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))

    #else
        #error "TinyEngine unsupport AC5"
    #endif
#endif

//network1 import
#if defined(X_CUBE_AI)
    #include "ai_platform.h"
    #include "network_1.h"
    #include "network_1_data.h"
#elif defined(TinyEngine)
    #include "gen_network_1_Model.h"
    void network_1_invoke(float* labels);
    signed char* get_network_1_Input();
    signed char* get_network_1_Output();
#endif


//Location of spatial separation
SEPARATION_CODE

#if SEPARATION>0
    #if defined(X_CUBE_AI)
        //network2 import
        #include "network_2.h"
        #include "network_2_data.h"
    #elif defined(TinyEngine)
        #include "gen_network_2_Model.h"
        void network_2_invoke(float* labels);
        signed char* get_network_2_Input();
        signed char* get_network_2_Output();
    #endif
    SEPARATION_SCALE_CODE
    #define INPUT_HEIGHT SEPARATION_SCALE*INPUT_1_H
    #define INPUT_WIDTH SEPARATION_SCALE*INPUT_1_W
    FIX_FACTOR0_CODE
    FIX_FACTOR1_CODE
    #if defined(X_CUBE_AI)
        #define ACTIVATION_SIZE (AI_NETWORK_1_DATA_ACTIVATIONS_SIZE>AI_NETWORK_2_DATA_ACTIVATIONS_SIZE?AI_NETWORK_1_DATA_ACTIVATIONS_SIZE:AI_NETWORK_2_DATA_ACTIVATIONS_SIZE)
    #elif defined(TinyEngine)
        #define ACTIVATION_SIZE (NETWORK_1_BUFFER_SIZE>NETWORK_2_BUFFER_SIZE?NETWORK_1_BUFFER_SIZE:NETWORK_2_BUFFER_SIZE)
    #endif
    #if defined(OD_MODEL)
        #define OUT_HEIGHT OUTPUT_2_H
    #endif
    #define OUT_CHANNEL OUTPUT_2_C

    typedef signed char ai1_out_type;
#else
    #define SEPARATION_SCALE 1
    #define INPUT_HEIGHT SEPARATION_SCALE*INPUT_1_H
    #define INPUT_WIDTH SEPARATION_SCALE*INPUT_1_W
    #if defined(X_CUBE_AI)
        #define ACTIVATION_SIZE AI_NETWORK_1_DATA_ACTIVATIONS_SIZE
    #elif defined(TinyEngine)
        #define ACTIVATION_SIZE NETWORK_1_BUFFER_SIZE
    #endif
    #if defined(OD_MODEL)
        #define OUT_HEIGHT OUTPUT_1_H
    #endif
    #define OUT_CHANNEL OUTPUT_1_C

    typedef float ai1_out_type;
#endif

typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

#define MAX_OBJ_NUM OUT_HEIGHT

#if defined(ENABLE_SPARSE_PATCH)
    #define MAX_DIFF_VAL 60
    #define MAX_DIFF_COUNT 40
#endif

IMG_NORM_CODE

#if !defined(AI_ALIGNED)
    #if defined(__GNUC__)||defined(__CC_ARM)||(( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))
        #define AI_ALIGNED(x)         __attribute__((aligned(x)))
    #else
        #error "unsupport complier"
    #endif
#endif


#if defined(OD_MODEL)
    typedef struct
    {
        float x_min;
        float y_min;
        float x_max;
        float y_max;
    }BBox;

    typedef struct
    {
        float confi;
        BBox bbox;
        u32 cls_index;
        const char *name;
    }ObjectResult;

    void handle_preds(float *preds,float conf);
    void nms(ObjectResult objects[],ObjectResult results[],u32* object_num,float nms_conf);
    extern ObjectResult results[];
    extern u32 object_num;
#elif defined(IC_MODEL)
    extern u32 result;
    extern float conf;
#endif

void AI_Run();
void AI_Init();

extern const char *activities[];

#if defined(OD_MODEL)
    #define ai_get_obj_name(idx) results[idx].name
    #define ai_get_obj_xyxy(idx,x0_addr,y0_addr,x1_addr,y1_addr) \
        *(x0_addr)=results[idx].bbox.x_min; \
        *(x1_addr)=results[idx].bbox.x_max; \
        *(y0_addr)=results[idx].bbox.y_min; \
        *(y1_addr)=results[idx].bbox.y_max

    #define ai_get_obj_conf(idx) results[idx].confi
    #define ai_get_obj_num() object_num
#elif defined(IC_MODEL)
    #define ai_get_img_cls() activities[result]
    #define ai_get_img_conf() conf
#endif

#define RGB565ToRGB888C(rgb565, r_addr, g_addr, b_addr) \
    *(r_addr) = ((0xF800 & (rgb565)) >> 8) ; \
    *(g_addr) = ((0x07E0 & (rgb565)) >> 3 ) ; \
    *(b_addr) = ((0x001F & (rgb565)) << 3 )

#define clip(x,low,high,r_addr) \
    *(r_addr)=(x)>(low)?(x):(low); \
    *(r_addr)=(*(r_addr))<(high)?(*(r_addr)):(high)

#define abs_diff(a,b) (a)>=(b)?(a)-(b):(b)-(a);

#if defined(IMG_NORM_BIAS_ONLY)
    #define pixel_norm(r_addr,g_addr,b_addr) \
        *(r_addr)+=bias; \
        *(g_addr)+=bias; \
        *(b_addr)+=bias
#elif defined(IMG_NORM)
    #define pixel_norm(r_addr,g_addr,b_addr) \
        clip(((float)*(r_addr))*weight_r+bias_r,-128.f,127.f,r_addr); \
        clip(((float)*(g_addr))*weight_g+bias_g,-128.f,127.f,g_addr); \
        clip(((float)*(b_addr))*weight_b+bias_b,-128.f,127.f,b_addr)
#endif



#if defined(NEED_RCU)
//    example for gd32h759i-eval
//    #include "gd32h7xx.h"
//    #include "gd32h7xx_rcu.h"
//    #define rcu_enable() rcu_periph_clock_enable(RCU_CRC)

//    example for gd32f470i BluePill
//    #include "gd32f470x_conf.h"
//    #include "RCU.h"
//    #define rcu_enable() rcu_periph_clock_enable(RCU_CRC)

    #if !defined(rcu_enable)
        #error "you need to define rcu_enable() when the version of x-cube-ai is lower than 9.0.0"
    #endif
#endif


/*
Define your image reading method in the form of macros or functions here
The method must be able to be called in the following form:
unsigned short int r,g,b;
unsigned int i=0,j=0;
img_pixel_read(i,j,&r,g,&b);
*/
//Using macro definition
//example for gd32h759i-eval
//#include "dci_ov2640.h"
//#define img_pixel_read(i,j,r_addr,g_addr,b_addr) \
//    u16 rgb565=*(((u16 *)0xC0000000)+(i)*IMAGE_WIDTH+IMAGE_WIDTH-(j)); \
//    RGB565ToRGB888C(rgb565, r_addr, g_addr, b_addr)

//example for gd32f470i BluePill
//#include "Camera.h"
//extern u32 s_iFrameAddr;
//#define img_pixel_read(i,j,r_addr,g_addr,b_addr) \
//    u16 rgb565=*(((u16 *)s_iFrameAddr)+(IMAGE_HEIGHT-i)*IMAGE_WIDTH+IMAGE_WIDTH-j); \
//    RGB565ToRGB888C(rgb565, r_addr, g_addr, b_addr)

//Using function
//void your_img_read_function_name(unsigned int,unsigned int,unsigned short int,unsigned short int,unsigned short int);
//#define img_pixel_read your_img_read_function_name

#if !defined(img_pixel_read)
    #error "Implement your image reading method above"
#endif

#endif
