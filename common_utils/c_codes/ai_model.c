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
u8 *ai1InData;
ai1_out_type *ai1OutData;
#if SEPARATION>0
signed char *ai2InData;
float *ai2OutData;
#endif
ai_u8 activations [ACTIVATION_SIZE] __attribute__((at((u32)0xD0E00000)));
#else
u8 ai1InData[AI_NETWORK_1_IN_1_SIZE];
ai1_out_type ai1OutData[AI_NETWORK_1_OUT_1_SIZE] ;
#if SEPARATION>0
signed char ai2InData[AI_NETWORK_2_IN_1_SIZE];
float ai2OutData[AI_NETWORK_2_OUT_1_SIZE];
#endif
ai_u8 activations[ACTIVATION_SIZE];
#endif
#if SEPARATION>0
#define aiOutData ai2OutData
#else
#define aiOutData ai1OutData
#endif
#define aiInData ai1InData

//const char* activities[5] = {"CLOSED_EYE","OPENED_EYE","CLOSED_MOUTH","OPENDED_MOUTH","HAND"};
//const char* activities[80] = {"person","bicycle","car","motorbike","aeroplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign",
//	"parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase",
//	"frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup",
//	"fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","sofa","pottedplant",
//	"bed","diningtable","toilet","tvmonitor","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book",
//	"clock","vase","scissors","teddy bear","hair drier","toothbrush"
//};


ai_buffer * ai_input1;
ai_buffer * ai_output1;
#if SEPARATION>0
ai_buffer * ai_input2;
ai_buffer * ai_output2;
#endif

static u32 img_width;
static u32 img_height;
static u32 img_type;

static void AI1_Run(u8 *pIn,ai1_out_type *pOut)
{
  ai_i32 batch;
  ai_error err;

  /* Update IO handlers with the data payload */
  ai_input1[0].data = AI_HANDLE_PTR(pIn);
  ai_output1[0].data = AI_HANDLE_PTR(pOut);

  batch = ai_network_1_run(network1, ai_input1, ai_output1);
  if (batch != 1) {
    err = ai_network_1_get_error(network1);
    printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
//    Error_Handler();
  }
}

#if SEPARATION>0
static void AI2_Run(signed char *pIn, float *pOut)
{
  ai_i32 batch;
  ai_error err;

  /* Update IO handlers with the data payload */
  ai_input2[0].data = AI_HANDLE_PTR(pIn);
  ai_output2[0].data = AI_HANDLE_PTR(pOut);

  batch = ai_network_2_run(network2, ai_input2, ai_output2);
  if (batch != 1) {
    err = ai_network_2_get_error(network2);
    printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
//    Error_Handler();
  }
}
#endif


void AI_Init(u32 img_width_,u32 img_height_,u32 img_type_)
{

#ifdef TO_EXT
	ai1InData=MyMalloc(2,AI_NETWORK_1_IN_1_SIZE*sizeof(u8));
	ai1OutData=MyMalloc(2,AI_NETWORK_1_OUT_1_SIZE*sizeof(u8));
#if SEPARATION>0
	ai2InData=MyMalloc(2,AI_NETWORK_2_IN_1_SIZE*sizeof(u8));
	ai2OutData=MyMalloc(2,AI_NETWORK_2_OUT_1_SIZE*sizeof(float));
#endif
#endif

    ai_error err;
    rcu_periph_clock_enable(RCU_CRC);
	img_width=img_width_;
	img_height=img_height_;
	img_type=img_type_;



  /* Create a local array with the addresses of the activations buffers */
//  const ai_handle act_addr[] = { activations0,activations1 };
    const ai_handle act_addr[] = { activations };

  /* Create an instance of the model */
    err = ai_network_1_create_and_init(&network1, act_addr, NULL);
#if SEPARATION>0
    err = ai_network_2_create_and_init(&network2, act_addr, NULL);
#endif
    if (err.type != AI_ERROR_NONE) {
        printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
    //    Error_Handler();
    }
    ai_input1 = ai_network_1_inputs_get(network1, NULL);
    ai_output1 = ai_network_1_outputs_get(network1, NULL);
#if SEPARATION>0
	ai_input2 = ai_network_2_inputs_get(network2, NULL);
    ai_output2 = ai_network_2_outputs_get(network2, NULL);
#endif
}


inline void RGB565ToRGB888C(u16 *rgb565, u8* r, u8* g, u8* b)
{
    *r = ((0xF800 & *rgb565) >> 8) ;
    *g = ((0x07E0 & *rgb565) >> 3 ) ;
    *b = ((0x001F & *rgb565) << 3 ) ;
}


void run_ai1_part(u8 *img,u32 part_index)
{
	u32 offh=(part_index/SEPARATION_SCALE);
	u32 offw=(part_index%SEPARATION_SCALE);
	u32 step=0;
	if(img_type==0)
		step=2;
	u32 i,j;
	u8 r,g,b;

	for(i=0;i<AI_NETWORK_1_IN_1_HEIGHT;i++)
	{
		for(j=0;j<AI_NETWORK_1_IN_1_WIDTH;j++)
		{
			RGB565ToRGB888C((u16*) (img+(img_height*img_width-1-(i+AI_NETWORK_1_IN_1_HEIGHT*offh)*img_width-(j+AI_NETWORK_1_IN_1_WIDTH*offw))*step),&r,&g,&b);
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
	for(int i=0;i<SEPARATION_SCALE*SEPARATION_SCALE;i++)
	{
		run_ai1_part(img,i);
	}
}
#if SEPARATION>0
void int_fix(signed char *data,u32 n)
{
	for(int i=0;i<n;i++)
	{
		*(data+i)=(*(data+i)+128)*FIX_FACTOR-128;
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
