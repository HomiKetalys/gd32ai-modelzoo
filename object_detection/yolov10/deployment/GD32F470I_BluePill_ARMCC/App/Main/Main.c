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


extern u32 s_iFrameAddr;

void ConfigDCI(void);
void ConfigTimer(unsigned int, unsigned short);


void  InitHardware(void)
{  

    SystemInit();        
    InitSDRAM();
    InitRCU();           
    InitNVIC();          
    InitUART0(115200);   
    InitUART2(115200);
    InitTimer();         
    InitSysTick();       
    
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

u32 total_time=0;
u32 time_count=0;
void detect_once(void)
{
    u32 i,x, y,t0,t1,t2;
    char str[64];
    u16 *frame=(u16 *) frame_addr;

    t0=get_time_stamp();
    AI_Run();
    t1=get_time_stamp();
    total_time+=t1-t0;
    time_count++;

    t1=get_time_stamp();
    for(y = 24; y <= LCD_Y-1; y++)
    {
        for(x = 0; x <= LCD_X-1; x++)
        {
            if((x>=SHOW_X0)&&(x<=SHOW_X1)&&(y>=SHOW_Y0)&&(y<=SHOW_Y1))
                LCDDrawPoint(x, y, frame[(IMAGE_HEIGHT-y+SHOW_Y0)*IMAGE_WIDTH+((IMAGE_WIDTH-x+SHOW_X0))]);
            else
                LCDDrawPoint(x, y, 0xffff);
        }
    }
#ifndef TEST_TIME
    float x0,y0,x1,y1;
    for(i=0;i<ai_get_obj_num();i++)
    {
        ai_get_obj_xyxy(i,&x0,&y0,&x1,&y1);
        LCDShowString(SHOW_X0+(u32)x0,SHOW_Y0+(u32)y0-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *)ai_get_obj_name(i));
        LCDDrawRectangle(SHOW_X0+(u32)x0,SHOW_Y0+(u32)y0,SHOW_X0+(u32)x1,SHOW_Y0+(u32)y1,0xfff0);
    }
#endif
    t2=get_time_stamp();
    t1=t2-t1;

    sprintf(str,"infer_time:%d.%03dms",total_time/time_count/1000,total_time/time_count%1000);
    LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
    sprintf(str,"display_time:%d.%dms",t1/1000,t1%1000);
    LCDShowString(0,24*2,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
}

int main(void)
{  
    InitHardware(); 

    InitLCD();             
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
  timer_parameter_struct timer_initpara;                


  rcu_periph_clock_enable(RCU_TIMER1);                  

  timer_deinit(TIMER1);                                 
  timer_struct_para_init(&timer_initpara);              
 

  timer_initpara.prescaler         = psc;               
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  
  timer_initpara.period            = arr;               
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  
  timer_init(TIMER1, &timer_initpara);                  

  timer_enable(TIMER1);
}
