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

		InitSDRAM();         //��ʼ��SDRAMģ��
}

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



//��ʼ�����ģ��
void detectInit()
{
	AI_Init(IMAGE_WIDTH,IMAGE_HEIGHT,0);
	LCDFillPixel(0,0,24*20-1,32*25-1,0xffff);
	LCDShowString(0,0,24*20-1,32,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");
}

//u16 zp[IMAGE_WIDTH*IMAGE_HEIGHT*2] __attribute__((at((u32)0xD0F00000 )));

//���һ��
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

	//�������ʾ����Ļ
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
