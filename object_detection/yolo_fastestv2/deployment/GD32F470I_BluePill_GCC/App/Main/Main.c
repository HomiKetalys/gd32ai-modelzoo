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
#include "Touch.h"
#include "ff.h"
#include "FontLib.h"
#include "Camera.h"
#include "Beep.h"
#include "UART2.h"
#include "string.h"
#include "ai_model.h"

//外部变量声明
extern u32 s_iFrameAddr;

void ConfigDCI(void);
void ConfigTimer(unsigned int, unsigned short);


/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
void InitHardware(void);       //初始化硬件相关的模块


/*********************************************************************************************************
* 函数名称：InitHardware
* 函数功能：所有的硬件相关的模块初始化函数都放在此函数中
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void  InitHardware(void)
{  

    SystemInit();        //系统初始化
    InitSDRAM();
    InitRCU();           //初始化RCC模块
    InitNVIC();          //初始化NVIC模块
    InitUART0(115200);   //初始化UART模块
    InitUART2(115200);
    InitTimer();         //初始化Timer模块
    InitSysTick();       //初始化SysTick模块
    
}

#define LCD_X 480
#define LCD_Y 800
#define SHOW_CX (LCD_X/2-1)
#define SHOW_CY (LCD_Y-LCD_X/2-1)
#define SHOW_X0 (SHOW_CX-(INPUT_WIDTH-1)/2)
#define SHOW_Y0 (SHOW_CY-(INPUT_HEIGHT-1)/2)
#define SHOW_X1 (SHOW_CX+(INPUT_WIDTH-1)/2)
#define SHOW_Y1 (SHOW_CY+(INPUT_HEIGHT-1)/2)
#define frame_addr s_iFrameAddr
#define get_time_stamp() timer_counter_read(TIMER1)

extern ObjectResult results[];
extern u32 object_num;
extern const char *activities[];


//检测一次
void detect_once(void)
{
    u32 i,x, y,t0,t1,t2;
    char str[64];
    u16 *frame=(u16 *) frame_addr;

    t0=get_time_stamp();
    AI_Run();
    t1=get_time_stamp();
    t0=t1-t0;


    //将结果显示在屏幕
    t1=get_time_stamp();
    for(y = 24; y <= LCD_Y-1; y++)
    {
        for(x = 0; x <= LCD_X-1; x++)
        {
            if((x>=SHOW_X0)&&(x<=SHOW_X1)&&(y>=SHOW_Y0)&&(y<=SHOW_Y1))
                LCDDrawPoint(x, y, frame[(y-SHOW_Y0)*IMAGE_WIDTH+((x-SHOW_X0))]);
            else
                LCDDrawPoint(x, y, 0xffff);
        }
    }
#ifndef TEST_TIME
    for(i=0;i<object_num;i++)
    {
        LCDShowString(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *)activities[results[i].cls_index]);
        LCDDrawRectangle(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min,SHOW_X0+(u32)results[i].bbox.x_max,SHOW_Y0+(u32)results[i].bbox.y_max,0xfff0);
    }
#endif
    t2=get_time_stamp();
    t1=t2-t1;

    sprintf(str,"infer_time:%d.%dms",t0/1000,t0%1000);
    LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
    sprintf(str,"display_time:%d.%dms",t1/1000,t1%1000);
    LCDShowString(0,24*2,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
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

    InitLCD();             //初始化LCD模块
    InitCamera();
    LCDDisplayDir(LCD_SCREEN_VERTICAL);
    ConfigTimer(0xffffffff,239);
    
    AI_Init();
    LCDClear(0xffff);
    LCDShowString(0,0,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");

    while(1)
    {
        detect_once();
    }
}

void ConfigTimer(unsigned int arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara用于存放定时器的参数

  //使能RCU相关时钟
  rcu_periph_clock_enable(RCU_TIMER1);                  //使能TIMER1的时钟

  timer_deinit(TIMER1);                                 //设置TIMER1参数恢复默认值
  timer_struct_para_init(&timer_initpara);              //初始化timer_initpara
 
  //配置TIMER1
  timer_initpara.prescaler         = psc;               //设置预分频器值
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //设置向上计数模式
  timer_initpara.period            = arr;               //设置自动重装载值
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //设置时钟分割
  timer_init(TIMER1, &timer_initpara);                  //根据参数初始化定时器

  timer_enable(TIMER1);  //使能定时器
}
