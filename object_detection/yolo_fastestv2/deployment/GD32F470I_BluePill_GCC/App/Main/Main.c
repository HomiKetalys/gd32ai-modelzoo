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
#include "Touch.h"
#include "ff.h"
#include "FontLib.h"
#include "Camera.h"
#include "Beep.h"
#include "UART2.h"
#include "string.h"
#include "ai_model.h"

//�ⲿ��������
extern u32 s_iFrameAddr;

void ConfigDCI(void);
void ConfigTimer(unsigned int, unsigned short);


/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
void InitHardware(void);       //��ʼ��Ӳ����ص�ģ��


/*********************************************************************************************************
* �������ƣ�InitHardware
* �������ܣ����е�Ӳ����ص�ģ���ʼ�����������ڴ˺�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void  InitHardware(void)
{  

    SystemInit();        //ϵͳ��ʼ��
    InitSDRAM();
    InitRCU();           //��ʼ��RCCģ��
    InitNVIC();          //��ʼ��NVICģ��
    InitUART0(115200);   //��ʼ��UARTģ��
    InitUART2(115200);
    InitTimer();         //��ʼ��Timerģ��
    InitSysTick();       //��ʼ��SysTickģ��
    
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


//���һ��
void detect_once(void)
{
    u32 i,x, y,t0,t1,t2;
    char str[64];
    u16 *frame=(u16 *) frame_addr;

    t0=get_time_stamp();
    AI_Run();
    t1=get_time_stamp();
    t0=t1-t0;


    //�������ʾ����Ļ
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

    InitLCD();             //��ʼ��LCDģ��
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
  timer_parameter_struct timer_initpara;                //timer_initpara���ڴ�Ŷ�ʱ���Ĳ���

  //ʹ��RCU���ʱ��
  rcu_periph_clock_enable(RCU_TIMER1);                  //ʹ��TIMER1��ʱ��

  timer_deinit(TIMER1);                                 //����TIMER1�����ָ�Ĭ��ֵ
  timer_struct_para_init(&timer_initpara);              //��ʼ��timer_initpara
 
  //����TIMER1
  timer_initpara.prescaler         = psc;               //����Ԥ��Ƶ��ֵ
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //�������ϼ���ģʽ
  timer_initpara.period            = arr;               //�����Զ���װ��ֵ
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //����ʱ�ӷָ�
  timer_init(TIMER1, &timer_initpara);                  //���ݲ�����ʼ����ʱ��

  timer_enable(TIMER1);  //ʹ�ܶ�ʱ��
}
