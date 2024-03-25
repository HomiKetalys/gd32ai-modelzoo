#ifndef _AI_MODEL_
#define _AI_MODEL_

#include "ai_platform.h"
#ifdef GD32F470
#include "gd32f470x_conf.h"
#include "RCU.h"
#elif defined(GD32H7XX)
#include "gd32h7xx.h"
#include "gd32h7xx_rcu.h"
#endif

#include "DataType.h"
#include "stdio.h"


//#define TEST_TIME
//Put the buffer of the model into external memory.Ensure that your external memory is initialized before the model runs
//#define TO_EXT
#ifdef TO_EXT
#ifdef GD32F470
#define EXT_RAM_ADDR 0xD0E00000
#elif defined(GD32H7XX)
#define EXT_RAM_ADDR 0xC0080000
#endif
#endif

//network1 import
#include "network_1.h"
#include "network_1_data.h"
//Location of spatial separation
SEPARATION_CODE
#if SEPARATION>0
//network2 import
#include "network_2.h"
#include "network_2_data.h"

#define SEPARATION_SCALE 2
#define INPUT_HEIGHT SEPARATION_SCALE*AI_NETWORK_1_IN_1_HEIGHT
#define INPUT_WIDTH SEPARATION_SCALE*AI_NETWORK_1_IN_1_WIDTH
#define CLASS_NUM AI_NETWORK_2_OUT_1_CHANNEL
FIX_FACTOR0_CODE
FIX_FACTOR1_CODE
#define ACTIVATION_SIZE AI_NETWORK_1_DATA_ACTIVATIONS_SIZE>AI_NETWORK_2_DATA_ACTIVATIONS_SIZE?AI_NETWORK_1_DATA_ACTIVATIONS_SIZE:AI_NETWORK_2_DATA_ACTIVATIONS_SIZE
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

USE_EXAMPLE_CODE

void AI_Run(u8 *);
void AI_Init(u32,u32,u32);

#endif
