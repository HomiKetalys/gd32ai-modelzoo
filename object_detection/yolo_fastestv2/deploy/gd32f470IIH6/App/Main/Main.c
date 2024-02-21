/*********************************************************************************************************
* ģ�����ƣ�Main.c
* ժ    Ҫ�����ļ���������Ӳ����ʼ��������main����
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2020��01��01��
* ��    �ݣ�
* ע    �⣺ע�⹴ѡOptions for Target 'Target1'->Code Generation->Use MicroLIB������printf�޷�ʹ��
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "Main.h"
#include "gd32f470x_conf.h"
#include "stdio.h"
#include "SysTick.h"
#include "RCU.h"
#include "NVIC.h"
#include "Timer.h"
#include "UART0.h"
#include "SDRAM.h"
#include "LED.h"
#include "TLILCD.h"
#include "JPEG.h"
#include "BMP.h"
#include "Touch.h"
#include "Malloc.h"
#include "ff.h"
#include "FontLib.h"
#include "Camera.h"
#include "Beep.h"



#include "ai_platform.h"
#include "UART2.h"
#include "string.h"

//�ⲿ��������
extern u32 s_iFrameAddr;
extern u32 s_iMode;
extern u32 s_iGetFrameFlag;
extern u32 s_iX0, s_iY0;
extern u32 s_iCnt1000;
extern u8 time_start;

//network1����
#include "network_1.h"
#include "network_1_data.h"
ai_handle network1;

//����ʽģ�Ͳ�������
#define OUT_POS_PREFIX 12
#define SEPERATION 0
//������
#if SEPERATION>0

//network2����
#include "network_2.h"
#include "network_2_data.h"
ai_handle network2;

#define SEPERATION_SCALE 2
#define INPUT_HEIGHT SEPERATION_SCALE*AI_NETWORK_1_IN_1_HEIGHT
#define INPUT_WIDTH SEPERATION_SCALE*AI_NETWORK_1_IN_1_WIDTH
#define CLASS_NUM AI_NETWORK_2_OUT_1_CHANNEL-3-OUT_POS_PREFIX
#define FIX_FACTOR 0.21075189090661883
#define ACTIVATION_SIZE AI_NETWORK_1_DATA_ACTIVATIONS_SIZE>AI_NETWORK_2_DATA_ACTIVATIONS_SIZE?AI_NETWORK_1_DATA_ACTIVATIONS_SIZE:AI_NETWORK_2_DATA_ACTIVATIONS_SIZE
#define AI_NETWORK_OUT_1_HEIGHT AI_NETWORK_2_OUT_1_HEIGHT
#define AI_NETWORK_OUT_1_CHANNEL AI_NETWORK_2_OUT_1_CHANNEL

typedef signed char ai1_out_type;
//������
#else
#define SEPERATION_SCALE 1
#define INPUT_HEIGHT SEPERATION_SCALE*AI_NETWORK_1_IN_1_HEIGHT
#define INPUT_WIDTH SEPERATION_SCALE*AI_NETWORK_1_IN_1_WIDTH
#define CLASS_NUM AI_NETWORK_1_OUT_1_CHANNEL-3-OUT_POS_PREFIX
#define ACTIVATION_SIZE AI_NETWORK_1_DATA_ACTIVATIONS_SIZE
#define AI_NETWORK_OUT_1_HEIGHT AI_NETWORK_1_OUT_1_HEIGHT
#define AI_NETWORK_OUT_1_CHANNEL AI_NETWORK_1_OUT_1_CHANNEL

typedef float ai1_out_type;

#endif

//����ģ�͵������������ͼ���ֵ���ڴ���������
//#define TO_EXT



#ifdef TO_EXT
u8 *ai1InData;
ai1_out_type *ai1OutData;
#if SEPERATION>0
signed char *ai2InData;
float *ai2OutData;
#endif
ai_u8 activations [ACTIVATION_SIZE] __attribute__((at((u32)0xD0E00000)));
#else
u8 ai1InData[AI_NETWORK_1_IN_1_SIZE];
ai1_out_type ai1OutData[AI_NETWORK_1_OUT_1_SIZE] ;
#if SEPERATION>0
signed char ai2InData[AI_NETWORK_2_IN_1_SIZE];
float ai2OutData[AI_NETWORK_2_OUT_1_SIZE];
#endif
ai_u8 activations [ACTIVATION_SIZE];
#endif
#if SEPERATION>0
#define aiOutData ai2OutData
#else
#define aiOutData ai1OutData
#endif
#define aiInData ai1InData

//const char* activities[5] = {"CLOSED_EYE","OPENED_EYE","CLOSED_MOUTH","OPENDED_MOUTH","SMOKING"};
const char* activities[80] = {"person","bicycle","car","motorbike","aeroplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign",
	"parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase",
	"frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup",
	"fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","sofa","pottedplant",
	"bed","diningtable","toilet","tvmonitor","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book",
	"clock","vase","scissors","teddy bear","hair drier","toothbrush"
};

ai_buffer * ai_input1;
ai_buffer * ai_output1;
#if SEPERATION>0
ai_buffer * ai_input2;
ai_buffer * ai_output2;
#endif

void ConfigDCI(void);
u32  WriteUART2(u8 *, u32 );


char uart_str[512];
u32 len=0;
u32 CE,OM,HD;
float state[CLASS_NUM];

//������ȡ�������к���
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
//���ͷ���к���
#if SEPERATION>0
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

//ģ�ͳ�ʼ������
static void AI_Init(void)
{
  ai_error err;
	rcu_periph_clock_enable(RCU_CRC);

  /* Create a local array with the addresses of the activations buffers */
//  const ai_handle act_addr[] = { activations0,activations1 };
  const ai_handle act_addr[] = { activations };
	
  /* Create an instance of the model */
  err = ai_network_1_create_and_init(&network1, act_addr, NULL);
#if SEPERATION>0
	err = ai_network_2_create_and_init(&network2, act_addr, NULL);
#endif
  if (err.type != AI_ERROR_NONE) {
    printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
//    Error_Handler();
  }
  ai_input1 = ai_network_1_inputs_get(network1, NULL);
  ai_output1 = ai_network_1_outputs_get(network1, NULL);
#if SEPERATION>0
	ai_input2 = ai_network_2_inputs_get(network2, NULL);
  ai_output2 = ai_network_2_outputs_get(network2, NULL);
#endif
}

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void InitSoftware(void);       //��ʼ�������ص�ģ��
static void InitHardware(void);       //��ʼ��Ӳ����ص�ģ��
static void Proc1msTask(void);        //1ms����
static void Proc2msTask(void);        //2ms��������
void detectInit(void);

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitSoftware
* �������ܣ����е������ص�ģ���ʼ�����������ڴ˺�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void  InitSoftware(void)
{
  InitLED();             //��ʼ��LEDģ��
  InitLCD();             //��ʼ��LCDģ��
  InitMemory(SRAMIN);    //��ʼ���ڲ��ڴ��
  InitMemory(SDRAMEX);   //��ʼ���ⲿ�ڴ��
//  InitFatFs();           //�����ļ�ϵͳ
  InitFontLib();         //��ʼ���ֿ����ģ��
//  InitJPEG();            //��ʼ������JPEGģ��
//  InitBMP();             //��ʼ��λͼ����ģ��
  InitTouch();           //��ʼ��������ģ��
  InitCamera();          //��ʼ������ͷģ��
	InitBeep();            //��ʼ��������ģ��
	detectInit();
}

/*********************************************************************************************************
* �������ƣ�InitHardware
* �������ܣ����е�Ӳ����ص�ģ���ʼ�����������ڴ˺�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void  InitHardware(void)
{  

  SystemInit();        //ϵͳ��ʼ��
  InitRCU();           //��ʼ��RCCģ��
  InitNVIC();          //��ʼ��NVICģ��
  InitUART0(115200);   //��ʼ��UARTģ��
	InitUART2(115200);
  InitTimer();         //��ʼ��Timerģ��
  InitSysTick();       //��ʼ��SysTickģ��
	
}
/*********************************************************************************************************
* �������ƣ�Proc1msTask
* �������ܣ�1ms�������� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static  void  Proc1msTask(void)
{
  static u8 s_iCnt = 0;
  if(Get1msFlag())
  {
    s_iCnt++;
    if(s_iCnt >= 20)
    {
      ScanTouch();  //������ɨ��
      s_iCnt = 0;
    }
    Clr1msFlag();
  }
}
/*********************************************************************************************************
* �������ƣ�Proc2msTask
* �������ܣ�2ms�������� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static  void  Proc2msTask(void)
{ 
//	static u8 s_iCnt = 0;
  if(Get2msFlag())  //�ж�2ms��־״̬
  { 
    //������˸����
    LEDFlicker(250);
		
//    s_iCnt++;
//    if(s_iCnt >= 20)
//    {
//      s_iCnt = 0;
//      CameraTask();
//    }
    Clr2msFlag();   //���2ms��־
  }
}
//��������RGB565תRGB888
inline void RGB565ToRGB888C(u16 rgb565, u8* r, u8* g, u8* b)
{
  *r = ((0xF800 & rgb565) >> 8) ;
  *g = ((0x07E0 & rgb565) >> 3 ) ;
  *b = ((0x001F & rgb565) << 3 ) ;
}
//����ʽģ�ʹ���ָ����ͼƬ,������浽���ͷ�����뻺����
void run_ai1_part(u16 *frame,u32 part_index)
{
	u32 offh=(part_index/SEPERATION_SCALE);
	u32 offw=(part_index%SEPERATION_SCALE);
	u32 i,j,k;
	u8 r,g,b;
	//��ɫת��
	for(i=0;i<AI_NETWORK_1_IN_1_HEIGHT;i++)
	{
		for(j=0;j<AI_NETWORK_1_IN_1_WIDTH;j++)
		{
			RGB565ToRGB888C(frame[IMAGE_HEIGHT*IMAGE_WIDTH-1-(i+AI_NETWORK_1_IN_1_HEIGHT*offh)*IMAGE_WIDTH-(j+AI_NETWORK_1_IN_1_WIDTH*offw)],&r,&g,&b);
			aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3]=b;
			aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3+1]=g;
			aiInData[i*AI_NETWORK_1_IN_1_WIDTH*3+j*3+2]=r;
		}
	}
	//������ȡ����
	AI1_Run(ai1InData,ai1OutData);
#if SEPERATION>0
	//ת�����������ͷ�����뻺����
	for(i=0;i<AI_NETWORK_2_IN_1_HEIGHT/SEPERATION_SCALE;i++)
	{
		for(j=0;j<AI_NETWORK_2_IN_1_WIDTH/SEPERATION_SCALE;j++)
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

void run_ai1(u16 *frame)
{
	for(int i=0;i<SEPERATION_SCALE*SEPERATION_SCALE;i++)
	{
		run_ai1_part(frame,i);
	}
}
#if SEPERATION>0
void int_fix(signed char *data,u32 n)
{
	for(int i=0;i<n;i++)
	{
		*(data+i)=(*(data+i)+128)*FIX_FACTOR-128;
	}
}
#endif

//��ʼ�����ģ��
void detectInit()
{
	AI_Init();
#ifdef TO_EXT
	ai1InData=MyMalloc(2,AI_NETWORK_1_IN_1_SIZE*sizeof(u8));
	ai1OutData=MyMalloc(2,AI_NETWORK_1_OUT_1_SIZE*sizeof(u8));
#if SEPERATION>0
	ai2InData=MyMalloc(2,AI_NETWORK_2_IN_1_SIZE*sizeof(u8));
	ai2OutData=MyMalloc(2,AI_NETWORK_2_OUT_1_SIZE*sizeof(float));
#endif
//	activations=MyMalloc(2,ACTIVATION_SIZE*sizeof(ai_u8));
#endif
	LCDFillPixel(0,s_iY0-96,32*15,s_iY0-96+24*22,0xffff);
}


//���һ��
void detect_once(void)
{
	while(!((0 != s_iMode)&&(1 == s_iGetFrameFlag)));
	u32 i,j,k;
	u32 x0, y0, x1, y1, x, y,t1,t2,t3,t4,t5;
	x0 = s_iX0-40+160-INPUT_WIDTH/2;
	y0 = s_iY0+160-INPUT_HEIGHT/2+48+48;
	x1 = x0 + INPUT_WIDTH - 1;
	y1 = y0 + INPUT_HEIGHT - 1;
	u16 *frame;
	frame=(u16 *) s_iFrameAddr;
	
	s_iCnt1000=0;
	time_start=1;
	LCDFillPixel(0,s_iY0-96,32*15,s_iY0-96+24,0xffff);
	LCDShowString(0,s_iY0-96,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"detected classes in img:");
	time_start=0;
	t1=s_iCnt1000;
	s_iCnt1000=0;
	
//		mySystemInit1();
	
	u32 sys=rcu_clock_freq_get(CK_SYS)/1000000;
	u32 ahb=rcu_clock_freq_get(CK_AHB)/1000000;
	u32 apb1=rcu_clock_freq_get(CK_APB1)/1000000;
	u32 apb2=rcu_clock_freq_get(CK_APB2)/1000000;
	//ͼ��Ԥ�����������ȡ����
	time_start=1;
	run_ai1(frame);
	time_start=0;
	t2=s_iCnt1000;
	s_iCnt1000=0;
	
	//��ⲿ��
	time_start=1;
#if SEPERATION>0
	int_fix(ai2InData,AI_NETWORK_2_IN_1_SIZE);
	AI2_Run(ai2InData,ai2OutData);
#endif
	time_start=0;
	t3=s_iCnt1000;
	s_iCnt1000=0;
	
//		mySystemInit2();
	//���������
	time_start=1;
	for(i=0;i<CLASS_NUM;i++)
	{
		state[i]=0;
	}
	for(i=0;i<AI_NETWORK_OUT_1_HEIGHT;i++)
	{
		for(j=OUT_POS_PREFIX;j<OUT_POS_PREFIX+3;j++)
		{
			if(aiOutData[i*AI_NETWORK_OUT_1_CHANNEL+j+OUT_POS_PREFIX]>0.5f)
			{
				for(k=OUT_POS_PREFIX+3;k<AI_NETWORK_OUT_1_CHANNEL;k++)
				{
					float conf=aiOutData[i*AI_NETWORK_OUT_1_CHANNEL+j]*aiOutData[i*AI_NETWORK_OUT_1_CHANNEL+k];
					if(conf>0.5f)
					{
						if(conf>state[k-3-OUT_POS_PREFIX])
						{
							state[k-3-OUT_POS_PREFIX]=conf;
						}
					}
				}
			}
		}
	}
	char str[128];
	bool obj=false;
	u8 obj_num=0;
	LCDFillPixel(0,s_iY0-96+24,32*15,s_iY0-96+24*3,0xffff);
	if(state[0]>state[1])
		state[1]=0;
	else
		state[0]=0;
	if(state[2]>state[3])
		state[3]=0;
	else
		state[2]=0;
	if(state[4]<0.7f)
		state[4]=0;
	for(i=0;i<CLASS_NUM;i++)
	{
		if(state[i]!=0)
		{

			printf("the class %d exist\r\n", i);
			sprintf(str,"%s",activities[i] );
			LCDShowString(160*obj_num,s_iY0-96+24,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0xf800,0xffff,str);
			obj_num+=1;
			obj=true;
		}
	}
	if(!obj)
	{
		LCDShowString(0,s_iY0-96+24,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"no class detected");
	}
	time_start=0;
	t4=s_iCnt1000;
	s_iCnt1000=0;
	//��ͼ����ʾ����Ļ
	time_start=1;
	i=0;
	for(y = y0; y <= y1; y++)
	{
		for(x = x0; x <= x1; x++)
		{
			LCDDrawPoint(x, y, frame[IMAGE_HEIGHT*IMAGE_WIDTH-1-(y-y0)*IMAGE_WIDTH-(x-x0)]);
			i++;
		}
	}
	time_start=0;
	t5=s_iCnt1000;
	s_iCnt1000=0;

	sprintf(str,"time/ms:%d;%d;%d;%d;%d;all:%d",t1,t2,t3,t4,t5,t1+t2+t3+t4+t5);
	LCDShowString(0,s_iY0-96+48,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"SYS:%dMHz",sys);
	LCDShowString(0,s_iY0-96+96+24,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"AHB:%dMHz",ahb);
	LCDShowString(160,s_iY0-96+96+24,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB1:%dMHz",apb1);
	LCDShowString(0,s_iY0-96+96+24*2,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB2:%dMHz",apb2);
	LCDShowString(160,s_iY0-96+96+24*2,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	
	//������һ֡����
	s_iGetFrameFlag = 0;
	ConfigDCI();
}
//������ռ�����
void collect(void)
{
	if(state[0]!=0)
		CE++;
	if(state[3]!=0)
		OM++;
	if(state[4]!=0)
		HD++;
}
//�����յ���Ϣ�Ƿ��м������
void check_msg(void)
{
	u32 num=ReadUART2((u8 *)uart_str+len,512);
	len+=num;
	char *pos;
	char str[128];
	uart_str[len+1]='\0';
	pos=strstr(uart_str,"/detect");
	u32 up_num=2;
	if(true)
	{
		len=0;
		CE=0;
		OM=0;
		HD=0;
		int i;
		for(i=0;i<up_num;i++)
		{
			detect_once();
			collect();
		}
		LCDFillPixel(0,s_iY0-96+24*3,32*15,s_iY0-96+24*5,0xffff);
		sprintf(str,"Detected State:CE:%d,OM:%d,HD:%d",CE,OM,HD);
		LCDShowString(0,s_iY0-96+72,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
		u32 init_pos=0;
		if(CE==up_num||OM==up_num)
		{
			WriteUART2("/msg 0/msg 0/msg 0/msg 0/msg 0/msg 0/msg 0",6*7);
			LCDShowString(0,s_iY0-96+96,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"Fatigue ");
			init_pos+=8;
		}
		if(HD==up_num)
		{
			WriteUART2("/msg 1/msg 1/msg 1/msg 1/msg 1/msg 1/msg 1",6*7);
			LCDShowString(12*init_pos,s_iY0-96+96,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"HAND");
		}
		if(CE!=up_num&&OM!=up_num&&HD!=up_num)
		{
				WriteUART2("/msg 2/msg 2/msg 2/msg 2/msg 2/msg 2/msg 2",6*7);
				LCDShowString(12*init_pos,s_iY0-96+96,32*15,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"NO ABNORMAL");
		}
//		WriteUART2("/data ",6);
//		WriteUART2((u8 *)s_iFrameAddr,192*192*2);
	}
	else
	{
		if(len>=512)
		{
			len=0;
		}
	}
}
	


/*********************************************************************************************************
* �������ƣ�main
* �������ܣ������� 
* ���������void
* ���������void
* �� �� ֵ��int
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
int main(void)
{  
  InitHardware(); //��ʼ��Ӳ����غ���
  InitSoftware(); //��ʼ�������غ���
	len=0;

  while(1)
  {
		Proc1msTask();
		Proc2msTask();
//		check_msg();
		detect_once();
  }
}
