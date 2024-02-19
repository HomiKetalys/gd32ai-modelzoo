/*********************************************************************************************************
* ģ�����ƣ�RTC.c
* ժ    Ҫ��RTC����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
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
#include "RTC.h"
#include "gd32f470x_conf.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//�����ַ���
static const char* s_arrWeek[] = 
{
  "����һ",
  "���ڶ�",
  "������",
  "������",
  "������",
  "������",
  "������",
};

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void RCUConfig(void);   //����RTCʱ��Դ
static u32  DecToBcd(u32 dec); //10����תBCD��
static u32  BcdToDec(u32 bcd); //BCD��ת10����

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�RCUConfig
* �������ܣ�����RTCʱ��Դ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ô˺���֮ǰ������ʹ�ܶԱ�����Ĵ�����д����
*********************************************************************************************************/
static void RCUConfig(void)
{
  //�޸�RTCʱ��ԴΪ�ⲿ32k����
  if(((RCU_BDCTL >> 8) & 0x03) != 0x01)
  {
    rcu_osci_on(RCU_LXTAL);                 //�����ⲿ32k����
    rcu_osci_stab_wait(RCU_LXTAL);          //�ȴ��ⲿ�������ȶ�
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL); //RTCʱ��Դѡ���ⲿ����
  }
}

/*********************************************************************************************************
* �������ƣ�DecToBcd
* �������ܣ�10����תBCD��
* ���������dec��10������
* ���������void
* �� �� ֵ��BCD��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u32 DecToBcd(u32 dec)
{
  u32 hBit, lBit, bcd;
  dec = dec % 100;
  hBit = (dec / 10) & 0x0F;
  lBit = (dec % 10) & 0x0F;
  bcd = (hBit << 4) | lBit;
  return bcd;
}

/*********************************************************************************************************
* �������ƣ�BcdToDec
* �������ܣ�BCD��ת10����
* ���������bcd��BCD��
* ���������void
* �� �� ֵ��10����
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u32 BcdToDec(u32 bcd)
{
  u32 hBit, lBit, dec;
  hBit = (bcd >> 4) & 0x0F;
  lBit = (bcd >> 0) & 0x0F;
  dec = hBit * 10 + lBit;
  return dec;
}

/*********************************************************************************************************
* �������ƣ�RTC_Alarm_IRQHandler
* �������ܣ������жϷ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
  //����0
  if(RESET != rtc_flag_get(RTC_FLAG_ALRM0))
  {
    rtc_flag_clear(RTC_FLAG_ALRM0);
    exti_flag_clear(EXTI_17);
    printf("RTC_Alarm_IRQHandler: Alarm0\r\n");
  }
  
  //����1
  if(RESET != rtc_flag_get(RTC_FLAG_ALRM1))
  {
    rtc_flag_clear(RTC_FLAG_ALRM1);
    exti_flag_clear(EXTI_17);
    printf("RTC_Alarm_IRQHandler: Alarm1\r\n");
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitRTC
* �������ܣ���ʼ��RTCģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitRTC(void)
{

}

/*********************************************************************************************************
* �������ƣ�RTCSet
* �������ܣ�����RCT��ǰֵ
* �����������ǰʱ��ֵ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCSet(u32 year, u32 month, u32 date, u32 week, u32 hour, u32 min, u32 sec)
{
  //RTC��ʼ���ṹ��
  rtc_parameter_struct rtc_initpara;

  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��

  //����RTC����
  rtc_initpara.year           = DecToBcd(year);
  rtc_initpara.month          = month;
  rtc_initpara.date           = DecToBcd(date);
  rtc_initpara.day_of_week    = week;
  rtc_initpara.hour           = DecToBcd(hour);
  rtc_initpara.minute         = DecToBcd(min);
  rtc_initpara.second         = DecToBcd(sec);
  rtc_initpara.factor_asyn    = 127;
  rtc_initpara.factor_syn     = 255;
  rtc_initpara.am_pm          = RTC_AM;
  rtc_initpara.display_format = RTC_24HOUR;
  if(SUCCESS != rtc_init(&rtc_initpara))
  {
    printf("SetRTC: ����RTCʧ��\r\n");
    while(1){}
  }

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCSetDate
* �������ܣ�����RTC����
* �����������ǰ����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCSetDate(u32 year, u32 month, u32 date, u32 week)
{
  //RTC��ʼ���ṹ��
  rtc_parameter_struct rtc_initpara;

  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��

  //����RTC����
  rtc_current_time_get(&rtc_initpara);
  rtc_initpara.year           = DecToBcd(year);
  rtc_initpara.month          = month;
  rtc_initpara.date           = DecToBcd(date);
  rtc_initpara.day_of_week    = week;
  rtc_initpara.factor_asyn    = 127;
  rtc_initpara.factor_syn     = 255;
  rtc_initpara.am_pm          = RTC_AM;
  rtc_initpara.display_format = RTC_24HOUR;
  if(SUCCESS != rtc_init(&rtc_initpara))
  {
    printf("RTCSetDate: ����RTCʧ��\r\n");
    while(1){}
  }

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCSetTime
* �������ܣ�����RTCʱ��
* �����������ǰʱ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCSetTime(u32 hour, u32 min, u32 sec)
{
  //RTC��ʼ���ṹ��
  rtc_parameter_struct rtc_initpara;

  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��

  //����RTC����
  rtc_current_time_get(&rtc_initpara);
  rtc_initpara.hour           = DecToBcd(hour);
  rtc_initpara.minute         = DecToBcd(min);
  rtc_initpara.second         = DecToBcd(sec);
  rtc_initpara.factor_asyn    = 127;
  rtc_initpara.factor_syn     = 255;
  rtc_initpara.am_pm          = RTC_AM;
  rtc_initpara.display_format = RTC_24HOUR;
  if(SUCCESS != rtc_init(&rtc_initpara))
  {
    printf("RTCSetTime: ����RTCʧ��\r\n");
    while(1){}
  }

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCGetDate
* �������ܣ���ȡ��ǰ����
* ������������ص�ǰ����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCGetDate(u32* year, u32* month, u32* date, u32* week)
{
  //RTC�����ṹ��
  rtc_parameter_struct rtc_initpara;

  //��ȡ��ǰʱ��
  rtc_current_time_get(&rtc_initpara);

  //��������
  *year = 2000 + BcdToDec(rtc_initpara.year);
  *month = rtc_initpara.month;
  *date = BcdToDec(rtc_initpara.date);
  *week = rtc_initpara.day_of_week;
}

/*********************************************************************************************************
* �������ƣ�RTCGetTime
* �������ܣ���ȡ��ǰ����
* ������������ص�ǰʱ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCGetTime(u32* hour, u32* min, u32* sec)
{
  //RTC�����ṹ��
  rtc_parameter_struct rtc_initpara;

  //��ȡ��ǰʱ��
  rtc_current_time_get(&rtc_initpara);

  //����ʱ��
  *hour = BcdToDec(rtc_initpara.hour);
  *min = BcdToDec(rtc_initpara.minute);
  *sec = BcdToDec(rtc_initpara.second);
}

/*********************************************************************************************************
* �������ƣ�RTCPrintTime
* �������ܣ���ӡ��ǰʱ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCPrintTime(void)
{
  u32 year, month, date, week, hour, min, sec;

  //��ȡʱ������
  RTCGetDate(&year, &month, &date, &week);
  RTCGetTime(&hour, &min, &sec);

  //��ӡ���
  printf("��ǰʱ�䣺%d-%02d-%02d %s %02d:%02d:%02d\r\n", year, month, date, s_arrWeek[week - 1], hour, min, sec);
}

/*********************************************************************************************************
* �������ƣ�RTCSetAlarm0
* �������ܣ���������0
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCSetAlarm0(u32 hour, u32 min, u32 sec)
{
  //���Ӳ����ṹ��
  rtc_alarm_struct rtc_alarm; 

  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM0))
  {
    printf("RTCSetAlarm0: ��������0ʧ��\r\n");
    while(1){}
  }

  //�������Ӳ���
  rtc_alarm.alarm_mask      = RTC_ALARM_DATE_MASK; 
  rtc_alarm.weekday_or_date = RTC_MONDAY;
  rtc_alarm.alarm_day       = DecToBcd(1);
  rtc_alarm.alarm_hour      = DecToBcd(hour);
  rtc_alarm.alarm_minute    = DecToBcd(min);
  rtc_alarm.alarm_second    = DecToBcd(sec);
  rtc_alarm.am_pm           = RTC_AM;
  rtc_alarm_config(RTC_ALARM0, &rtc_alarm);
  rtc_alarm_enable(RTC_ALARM0);

  //�����ж�����
  rcu_all_reset_flag_clear();                           //�����־λ
  exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING); //�����ж���
  nvic_irq_enable(RTC_Alarm_IRQn, 3, 3);                //�����ж����ȼ�
  rtc_interrupt_enable(RTC_INT_ALARM0);                 //ʹ�������ж�

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCSetAlarm1
* �������ܣ���������1
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCSetAlarm1(u32 hour, u32 min, u32 sec)
{
  //���Ӳ����ṹ��
  rtc_alarm_struct rtc_alarm; 

  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM1))
  {
    printf("RTCSetAlarm1: ��������1ʧ��\r\n");
    while(1){}
  }

  //�������Ӳ���
  rtc_alarm.alarm_mask      = RTC_ALARM_DATE_MASK; 
  rtc_alarm.weekday_or_date = RTC_MONDAY;
  rtc_alarm.alarm_day       = DecToBcd(1);
  rtc_alarm.alarm_hour      = DecToBcd(hour);
  rtc_alarm.alarm_minute    = DecToBcd(min);
  rtc_alarm.alarm_second    = DecToBcd(sec);
  rtc_alarm.am_pm           = RTC_AM;
  rtc_alarm_config(RTC_ALARM1, &rtc_alarm);
  rtc_alarm_enable(RTC_ALARM1);

  //�����ж�����
  rcu_all_reset_flag_clear();                           //�����־λ
  exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING); //�����ж���
  nvic_irq_enable(RTC_Alarm_IRQn, 3, 3);                //�����ж����ȼ�
  rtc_interrupt_enable(RTC_INT_ALARM1);                 //ʹ�������ж�

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCDisableAlarm0
* �������ܣ���������0
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCDisableAlarm0(void)
{
  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM0))
  {
    printf("RTCDisableAlarm0: ��������0ʧ��\r\n");
    while(1){}
  }

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}

/*********************************************************************************************************
* �������ƣ�RTCDisableAlarm1
* �������ܣ���������1
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RTCDisableAlarm1(void)
{
  //д��ǰ׼��
  rcu_periph_clock_enable(RCU_PMU); //ʹ��PMUʱ��
  pmu_backup_write_enable();        //����Ա�����Ĵ�����д����
  RCUConfig();                      //����RTCʹ���ⲿ����
  rcu_periph_clock_enable(RCU_RTC); //ʹ��RTCʱ��
  rtc_register_sync_wait();         //�ȴ�ͬ��
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM1))
  {
    printf("RTCDisableAlarm1: ��������1ʧ��\r\n");
    while(1){}
  }

  //�˳�����
  rtc_register_sync_wait();   //�ȴ�ͬ��
  pmu_backup_write_disable(); //��ֹ�Ա�����Ĵ�����д����
}
