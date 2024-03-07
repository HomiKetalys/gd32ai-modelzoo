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
#include "UART2.h"
#include "string.h"
#include "ai_model.h"

//�ⲿ��������
extern u32 s_iFrameAddr;
extern u32 s_iCnt1000;
extern u32 s_iGetFrameFlag;
extern u8 time_start;

void ConfigDCI(void);



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






#define LCD_X 480
#define LCD_Y 800
#define SHOW_CX (LCD_X/2-1)
#define SHOW_CY (LCD_Y-LCD_X/2-1)
#define SHOW_X0 (SHOW_CX-(INPUT_WIDTH-1)/2)
#define SHOW_Y0 (SHOW_CY-(INPUT_HEIGHT-1)/2)
#define SHOW_X1 (SHOW_CX+(INPUT_WIDTH-1)/2)
#define SHOW_Y1 (SHOW_CY+(INPUT_HEIGHT-1)/2)
#define time_start() time_start=1
#define get_time_stamp() s_iCnt1000
#define get_frame_flag() s_iGetFrameFlag
#define clear_frame_flag() s_iGetFrameFlag=0
#define frame_addr s_iFrameAddr

extern ObjectResult results[];
extern u32 object_num;

const char* activities[80] = {"person","bicycle","car","motorbike","aeroplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign",
	"parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase",
	"frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup",
	"fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","sofa","pottedplant",
	"bed","diningtable","toilet","tvmonitor","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book",
	"clock","vase","scissors","teddy bear","hair drier","toothbrush"
};

//���һ��
void detect_once(void)
{
	while(!((1 == get_frame_flag())));
	u32 i,x, y,t0,t1,t2;
	char str[128];
	u16 *frame=(u16 *) frame_addr;
	
	time_start();
	
	t0=get_time_stamp();
	AI_Run((u8 *)frame);
	t1=get_time_stamp();
	t0=t1-t0;


	//�������ʾ����Ļ
	t1=get_time_stamp();
	for(y = 24; y <= LCD_Y-1; y++)
	{
		for(x = 0; x <= LCD_X-1; x++)
		{
			if((x>=SHOW_X0)&&(x<=SHOW_X1)&&(y>=SHOW_Y0)&&(y<=SHOW_Y1))
				LCDDrawPoint(x, y, frame[IMAGE_HEIGHT*IMAGE_WIDTH-1-(y-SHOW_Y0)*IMAGE_WIDTH-(x-SHOW_X0)]);
			else
				LCDDrawPoint(x, y, 0xffff);
		}
	}
#ifndef TEST_TIME
	
	for(i=0;i<object_num;i++)
	{
		LCDShowString(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *) activities[results[i].cls_index]);
		LCDDrawRectangle(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min,SHOW_X0+(u32)results[i].bbox.x_max,SHOW_Y0+(u32)results[i].bbox.y_max,0xfff0);
	}
#endif
	t2=get_time_stamp();
	t1=t2-t1;
	
	u32 sys=rcu_clock_freq_get(CK_SYS)/1000000;
	u32 ahb=rcu_clock_freq_get(CK_AHB)/1000000;
	u32 apb1=rcu_clock_freq_get(CK_APB1)/1000000;
	u32 apb2=rcu_clock_freq_get(CK_APB2)/1000000;

 
	sprintf(str,"infer_time:%dms",t0);
	LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"display_time:%dms",t1);
	LCDShowString(0,24*2,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"SYS:%dMHz",sys);
	LCDShowString(0,24*3,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"AHB:%dMHz",ahb);
	LCDShowString(0,24*4,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB1:%dMHz",apb1);
	LCDShowString(0,24*5,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB2:%dMHz",apb2);
	LCDShowString(0,24*6,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	
	//������һ֡����
	clear_frame_flag();
	ConfigDCI();
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
	
	AI_Init(IMAGE_WIDTH,IMAGE_HEIGHT,0);
	LCDClear(0xffff);
	LCDShowString(0,0,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");

  while(1)
  {
		Proc1msTask();
		Proc2msTask();
//		check_msg();
		detect_once();
  }
}
