#ifndef _AI_MODEL_
#define _AI_MODEL_

#include "ai_platform.h"

#include "stdio.h"
#include "string.h"

//
//#define TEST_TIME_ONLY

//Put the buffer of the model into external memory.Ensure that your external memory is initialized before the model runs
//#define TO_EXT

MODEL_CONF_CODE

RCU_CODE

//network1 import
#include "network_1.h"
#include "network_1_data.h"
//Location of spatial separation
SEPARATION_CODE
#if SEPARATION>0
//network2 import
#include "network_2.h"
#include "network_2_data.h"
SEPARATION_SCALE_CODE
#define INPUT_HEIGHT SEPARATION_SCALE*AI_NETWORK_1_IN_1_HEIGHT
#define INPUT_WIDTH SEPARATION_SCALE*AI_NETWORK_1_IN_1_WIDTH
#define CLASS_NUM AI_NETWORK_2_OUT_1_CHANNEL
FIX_FACTOR0_CODE
FIX_FACTOR1_CODE
#define ACTIVATION_SIZE (AI_NETWORK_1_DATA_ACTIVATIONS_SIZE>AI_NETWORK_2_DATA_ACTIVATIONS_SIZE?AI_NETWORK_1_DATA_ACTIVATIONS_SIZE:AI_NETWORK_2_DATA_ACTIVATIONS_SIZE)
#define AI_NETWORK_OUT_1_HEIGHT AI_NETWORK_2_OUT_1_HEIGHT
#define AI_NETWORK_OUT_1_CHANNEL AI_NETWORK_2_OUT_1_CHANNEL

typedef signed char ai1_out_type;

#else
#define SEPARATION_SCALE 1
#define INPUT_HEIGHT SEPARATION_SCALE*AI_NETWORK_1_IN_1_HEIGHT
#define INPUT_WIDTH SEPARATION_SCALE*AI_NETWORK_1_IN_1_WIDTH
#define CLASS_NUM AI_NETWORK_1_OUT_1_CHANNEL
#define ACTIVATION_SIZE AI_NETWORK_1_DATA_ACTIVATIONS_SIZE
#define AI_NETWORK_OUT_1_HEIGHT AI_NETWORK_1_OUT_1_HEIGHT
#define AI_NETWORK_OUT_1_CHANNEL AI_NETWORK_1_OUT_1_CHANNEL

typedef float ai1_out_type;

#endif

typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

IMG_NORM_CODE



void AI_Run();
void AI_Init();
extern u32 result;
extern float conf;
extern const char *activities[];

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

#define ai_get_img_cls() activities[result]
#define ai_get_img_conf() conf

#if defined(NEED_RCU)
//example for gd32h759i-eval
//#include "gd32h7xx.h"
//#include "gd32h7xx_rcu.h"
//#define rcu_enable() rcu_periph_clock_enable(RCU_CRC)

//example for gd32f470i BluePill
//#include "gd32f470x_conf.h"
//#include "RCU.h"
//#define rcu_enable() rcu_periph_clock_enable(RCU_CRC)

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
