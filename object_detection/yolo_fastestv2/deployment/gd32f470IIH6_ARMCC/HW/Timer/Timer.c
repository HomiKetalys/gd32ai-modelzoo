/*********************************************************************************************************
* ģ�����ƣ�Timer.c
* ժ    Ҫ��Timerģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2022 Leyutek. All rights reserved.)
* ������ڣ�2022��01��01��  
* ��    �ݣ�
* ע    �⣺
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
#include "Timer.h"
#include "gd32f470x_conf.h"
#include "Beep.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static unsigned char  s_i1msFlag = FALSE;     //��1ms��־λ��ֵ����ΪFALSE
static unsigned char  s_i2msFlag  = FALSE;    //��2ms��־λ��ֵ����ΪFALSE
static unsigned char  s_i1secFlag = FALSE;    //��1s��־λ��ֵ����ΪFALSE
u32 s_iCnt1000  = 0; 
u8 time_start=0;

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ConfigTimer1(unsigned short arr, unsigned short psc); //����TIMER1
static void ConfigTimer4(unsigned short arr, unsigned short psc); //����TIMER4
static u64 s_iSysTime  = 0;        //ϵͳ����ʱ�䣨ms��

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigTimer1
* �������ܣ�����TIMER1 
* ���������arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void ConfigTimer1(unsigned short arr, unsigned short psc)
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

  timer_interrupt_enable(TIMER1, TIMER_INT_UP);         //ʹ�ܶ�ʱ���ĸ����ж�
  nvic_irq_enable(TIMER1_IRQn, 1, 0);                   //����NVIC �������ȼ�

  timer_enable(TIMER1);  //ʹ�ܶ�ʱ��
}

/*********************************************************************************************************
* �������ƣ�ConfigTimer4
* �������ܣ�����TIMER4 
* ���������arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
static  void ConfigTimer4(unsigned short arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara���ڴ�Ŷ�ʱ���Ĳ���

  //ʹ��RCU���ʱ��
  rcu_periph_clock_enable(RCU_TIMER4);                  //ʹ��TIMER4��ʱ��

  timer_deinit(TIMER4);                                 //����TIMER4�����ָ�Ĭ��ֵ
  timer_struct_para_init(&timer_initpara);              //��ʼ��timer_initpara
 
  //����TIMER4
  timer_initpara.prescaler         = psc;               //����Ԥ��Ƶ��ֵ
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //�������ϼ���ģʽ
  timer_initpara.period            = arr;               //�����Զ���װ��ֵ
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //����ʱ�ӷָ�
  timer_init(TIMER4, &timer_initpara);                  //���ݲ�����ʼ����ʱ��

  timer_interrupt_enable(TIMER4, TIMER_INT_UP);         //ʹ�ܶ�ʱ���ĸ����ж�
  nvic_irq_enable(TIMER4_IRQn, 1, 0);                   //����NVIC�������ȼ�

  timer_enable(TIMER4);  //ʹ�ܶ�ʱ��
}

/*********************************************************************************************************
* �������ƣ�TIMER1_IRQHandler
* �������ܣ�TIMER1�жϷ����� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺ÿ�������һ���жϷ�����
*********************************************************************************************************/
void TIMER1_IRQHandler(void)  
{
  static  unsigned short s_iCnt2 = 0;                                //����һ����̬����s_iCnt2��Ϊ2ms������

  if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)    //�ж϶�ʱ�������ж��Ƿ���
  {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);           //�����ʱ�������жϱ�־ 
  }

  s_i1msFlag = TRUE;    //��1ms��־λ��ֵ����ΪTRUE 

  s_iCnt2++;              //2ms�������ļ���ֵ��1
  
  if(s_iCnt2 >= 2)        //2ms�������ļ���ֵ���ڻ����2
  {
    s_iCnt2 = 0;          //����2ms�������ļ���ֵΪ0
    s_i2msFlag = TRUE;    //��2ms��־λ��ֵ����ΪTRUE 
  }
	
	//����������ʱ
  BeepCalcDown(1);

  //ϵͳ����ʱ���һ
  s_iSysTime++;
}

/*********************************************************************************************************
* �������ƣ�TIMER4_IRQHandler
* �������ܣ�TIMER4�жϷ����� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺ÿ�������һ���жϷ�����
*********************************************************************************************************/
void TIMER4_IRQHandler(void)  
{

  if (timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP) == SET)    //�ж϶�ʱ�������ж��Ƿ���
  {
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);           //�����ʱ�������жϱ�־ 
  }
  
  s_iCnt1000+=time_start;           //1000ms�������ļ���ֵ��1
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitTimer
* �������ܣ���ʼ��Timerģ�� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
void InitTimer(void)
{
  ConfigTimer1(999, 239);  //240MHz/(239+1)=1MHz����0������999Ϊ1ms
  ConfigTimer4(999, 239);  //240MHz/(239+1)=1MHz����0������999Ϊ1ms
}

/*********************************************************************************************************
* �������ƣ�Get1msFlag
* �������ܣ���ȡ1ms��־λ��ֵ
* ���������void
* ���������void
* �� �� ֵ��s_i1msFlag-1ms��־λ��ֵ
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
unsigned char  Get1msFlag(void)
{
  return(s_i1msFlag);     //����1ms��־λ��ֵ
}

/*********************************************************************************************************
* �������ƣ�Clr1msFlag
* �������ܣ����1ms��־λ  
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
void  Clr1msFlag(void)
{
  s_i1msFlag = FALSE;     //��1ms��־λ��ֵ����ΪFALSE 
}

/*********************************************************************************************************
* �������ƣ�Get2msFlag
* �������ܣ���ȡ2ms��־λ��ֵ
* ���������void
* ���������void
* �� �� ֵ��s_i2msFlag-2ms��־λ��ֵ
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
unsigned char  Get2msFlag(void)
{
  return(s_i2msFlag);     //����2ms��־λ��ֵ
}

/*********************************************************************************************************
* �������ƣ�Clr2msFlag
* �������ܣ����2ms��־λ  
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
void  Clr2msFlag(void)
{
  s_i2msFlag = FALSE;     //��2ms��־λ��ֵ����ΪFALSE 
}

/*********************************************************************************************************
* �������ƣ�Get1SecFlag
* �������ܣ���ȡ1s��־λ��ֵ  
* ���������void
* ���������void
* �� �� ֵ��s_i1secFlag-1s��־λ��ֵ
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
unsigned char  Get1SecFlag(void)
{
  return(s_i1secFlag);    //����1s��־λ��ֵ
}

/*********************************************************************************************************
* �������ƣ�Clr1SecFlag
* �������ܣ����1s��־λ  
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2022��01��01��
* ע    �⣺
*********************************************************************************************************/
void  Clr1SecFlag(void)
{
  s_i1secFlag = FALSE;    //��1s��־λ��ֵ����ΪFALSE
}

/*********************************************************************************************************
* �������ƣ�GetSysTime
* �������ܣ���ȡϵͳ����ʱ�䣨ms��
* ���������void
* ���������void
* �� �� ֵ��ϵͳ����ʱ�䣨ms��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u64 GetSysTime(void)
{
  return s_iSysTime;
}