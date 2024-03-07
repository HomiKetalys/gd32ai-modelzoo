/*********************************************************************************************************
* 模块名称：Main.c
* 摘    要：主文件，包含软硬件初始化函数和main函数
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日
* 内    容：
* 注    意：注意勾选Options for Target 'Target1'->Code Generation->Use MicroLIB，否则printf无法使用
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
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

//外部变量声明
extern u32 s_iFrameAddr;
extern u32 s_iCnt1000;
extern u32 s_iGetFrameFlag;
extern u8 time_start;

void ConfigDCI(void);



/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void InitSoftware(void);       //初始化软件相关的模块
static void InitHardware(void);       //初始化硬件相关的模块
static void Proc1msTask(void);        //1ms任务
static void Proc2msTask(void);        //2ms处理任务


/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitSoftware
* 函数功能：所有的软件相关的模块初始化函数都放在此函数中
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  void  InitSoftware(void)
{
  InitLED();             //初始化LED模块
  InitLCD();             //初始化LCD模块
  InitMemory(SRAMIN);    //初始化内部内存池
  InitMemory(SDRAMEX);   //初始化外部内存池
//  InitFatFs();           //挂载文件系统
  InitFontLib();         //初始化字库管理模块
//  InitJPEG();            //初始化绘制JPEG模块
//  InitBMP();             //初始化位图绘制模块
  InitTouch();           //初始化触摸屏模块
  InitCamera();          //初始化摄像头模块
	InitBeep();            //初始化蜂鸣器模块
}

/*********************************************************************************************************
* 函数名称：InitHardware
* 函数功能：所有的硬件相关的模块初始化函数都放在此函数中
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static  void  InitHardware(void)
{  

  SystemInit();        //系统初始化
  InitRCU();           //初始化RCC模块
  InitNVIC();          //初始化NVIC模块
  InitUART0(115200);   //初始化UART模块
	InitUART2(115200);
  InitTimer();         //初始化Timer模块
  InitSysTick();       //初始化SysTick模块
	
}
/*********************************************************************************************************
* 函数名称：Proc1msTask
* 函数功能：1ms处理任务 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  void  Proc1msTask(void)
{
  static u8 s_iCnt = 0;
  if(Get1msFlag())
  {
    s_iCnt++;
    if(s_iCnt >= 20)
    {
      ScanTouch();  //触摸屏扫描
      s_iCnt = 0;
    }
    Clr1msFlag();
  }
}
/*********************************************************************************************************
* 函数名称：Proc2msTask
* 函数功能：2ms处理任务 
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static  void  Proc2msTask(void)
{ 
//	static u8 s_iCnt = 0;
  if(Get2msFlag())  //判断2ms标志状态
  { 
    //调用闪烁函数
    LEDFlicker(250);
		
//    s_iCnt++;
//    if(s_iCnt >= 20)
//    {
//      s_iCnt = 0;
//      CameraTask();
//    }
    Clr2msFlag();   //清除2ms标志
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

//检测一次
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


	//将结果显示在屏幕
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
	
	//开启下一帧传输
	clear_frame_flag();
	ConfigDCI();
}



/*********************************************************************************************************
* 函数名称：main
* 函数功能：主函数 
* 输入参数：void
* 输出参数：void
* 返 回 值：int
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
int main(void)
{  
  InitHardware(); //初始化硬件相关函数
  InitSoftware(); //初始化软件相关函数
	
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
