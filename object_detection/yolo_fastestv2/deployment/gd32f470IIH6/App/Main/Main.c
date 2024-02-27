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
extern u32 s_iMode;
extern u32 s_iGetFrameFlag;
extern u32 s_iX0, s_iY0;
extern u32 s_iCnt1000;
extern u8 time_start;
extern ObjectResult results[];
extern u32 object_num;
//extern const char activities[];

void ConfigDCI(void);
u32  WriteUART2(u8 *, u32 );

#define SHOW_CX 240-1
#define SHOW_CY 560-1
#define SHOW_X0 (SHOW_CX-(INPUT_WIDTH-1)/2)
#define SHOW_Y0 (SHOW_CY-(INPUT_HEIGHT-1)/2)
#define SHOW_X1 (SHOW_CX+(INPUT_WIDTH-1)/2)
#define SHOW_Y1 (SHOW_CY+(INPUT_HEIGHT-1)/2)

const char* activities[80] = {"person","bicycle","car","motorbike","aeroplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign",
	"parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase",
	"frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup",
	"fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","sofa","pottedplant",
	"bed","diningtable","toilet","tvmonitor","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book",
	"clock","vase","scissors","teddy bear","hair drier","toothbrush"
};



char uart_str[512];
u32 len=0;
u32 CE,OM,HD;
float state[CLASS_NUM];

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
void detectInit(void);


#define RCU_MODIFY      {volatile uint32_t i; \
                         RCU_CFG0 |= RCU_AHB_CKSYS_DIV2; \
                         for(i=0;i<50000;i++); \
                         RCU_CFG0 |= RCU_AHB_CKSYS_DIV4; \
                         for(i=0;i<50000;i++);}

void MySystemInit (void)
{
    /* FPU settings */
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
    /* Reset the RCU clock configuration to the default reset state */
    /* Set IRC16MEN bit */
    RCU_CTL |= RCU_CTL_IRC16MEN;

    RCU_MODIFY
    
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    
    /* Reset HXTALEN, CKMEN and PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN);

    /* Reset HSEBYP bit */
    RCU_CTL &= ~(RCU_CTL_HXTALBPS);
    
    /* Reset CFG0 register */
    RCU_CFG0 = 0x00000000U;

    /* wait until IRC16M is selected as system clock */
    while(0 != (RCU_CFG0 & RCU_SCSS_IRC16M)){
    }

    /* Reset PLLCFGR register */
    RCU_PLL = 0x24003010U;

    /* Disable all interrupts */
    RCU_INT = 0x00000000U;
         
    /* Configure the System clock source, PLL Multiplier and Divider factors, 
        AHB/APBx prescalers and Flash settings */
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(1){
        }
    }
         
    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL |= PMU_CTL_LDOVS;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* Configure the main PLL, PSC = 25, PLL_N = 240, PLL_P = 2, PLL_Q = 5 */ 
    RCU_PLL = (25U | (240U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) | (5U << 24U));

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }
    
    /* Enable the high-drive to extend the clock frequency to 120 Mhz */
    PMU_CTL |= PMU_CTL_HDEN;
    while(0U == (PMU_CS & PMU_CS_HDRF)){
    }
    
    /* select the high-drive mode */
    PMU_CTL |= PMU_CTL_HDS;
    while(0U == (PMU_CS & PMU_CS_HDSRF)){
    } 
    
    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }

		InitSDRAM();         //初始化SDRAM模块
}

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
	detectInit();
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



//初始化检测模型
void detectInit()
{
	AI_Init(IMAGE_WIDTH,IMAGE_HEIGHT,0);
	LCDFillPixel(0,0,24*20-1,32*25-1,0xffff);
	LCDShowString(0,0,24*20-1,32,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");
}

//u16 zp[IMAGE_WIDTH*IMAGE_HEIGHT*2] __attribute__((at((u32)0xD0F00000 )));

//检测一次
void detect_once(void)
{
	while(!((0 != s_iMode)&&(1 == s_iGetFrameFlag)));
	u32 i,x, y,t1,t2,t3;
	char str[128];
	u16 *frame=(u16 *) s_iFrameAddr;
	
//		mySystemInit1();
	

	s_iCnt1000=0;

	time_start=1;
	AI_Run((u8 *)frame);
	time_start=0;
	t1=s_iCnt1000;

//		mySystemInit2();

	//将结果显示在屏幕
	LCDFillPixel(0,24,32*15-1,SHOW_Y0-1,0xffff);
	s_iCnt1000=0;
	time_start=1;
	for(y = SHOW_Y0; y <= SHOW_Y1; y++)
	{
		for(x = SHOW_X0; x <= SHOW_X1; x++)
		{
			LCDDrawPoint(x, y, frame[IMAGE_HEIGHT*IMAGE_WIDTH-1-(y-SHOW_Y0)*IMAGE_WIDTH-(x-SHOW_X0)]);
		}
	}
#ifndef TEST_TIME
	
	for(i=0;i<object_num;i++)
	{
		LCDShowString(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *) activities[results[i].cls_index]);
		LCDDrawRectangle(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min,SHOW_X0+(u32)results[i].bbox.x_max,SHOW_Y0+(u32)results[i].bbox.y_max,0xfff0);
	}
#endif
	time_start=0;
	t2=s_iCnt1000;
	
	u32 sys=rcu_clock_freq_get(CK_SYS)/1000000;
	u32 ahb=rcu_clock_freq_get(CK_AHB)/1000000;
	u32 apb1=rcu_clock_freq_get(CK_APB1)/1000000;
	u32 apb2=rcu_clock_freq_get(CK_APB2)/1000000;

 
	sprintf(str,"time/ms:%d;%d;all:%d",t1,t2,t1+t2);
	printf("time/ms:%d;%d;all:%d\r\n",t1,t2,t1+t2);
	LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"SYS:%dMHz",sys);
	LCDShowString(0,24*2,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"AHB:%dMHz",ahb);
	LCDShowString(240,24*2,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB1:%dMHz",apb1);
	LCDShowString(0,24*3,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	sprintf(str,"APB2:%dMHz",apb2);
	LCDShowString(240,24*3,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
	
	//开启下一帧传输
	s_iGetFrameFlag = 0;
	ConfigDCI();
}
//检测结果收集函数
void collect(void)
{
	if(state[0]!=0)
		CE++;
	if(state[3]!=0)
		OM++;
	if(state[4]!=0)
		HD++;
}
//检查接收的消息是否有检查命令
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
	len=0;

  while(1)
  {
		Proc1msTask();
		Proc2msTask();
//		check_msg();
		detect_once();
  }
}
