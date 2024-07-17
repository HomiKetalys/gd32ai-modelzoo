/*********************************************************************************************************
* 模块名称：RTC.c
* 摘    要：RTC驱动模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日
* 内    容：
* 注    意：
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
#include "RTC.h"
#include "gd32f470x_conf.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
//星期字符串
static const char* s_arrWeek[] = 
{
  "星期一",
  "星期二",
  "星期三",
  "星期四",
  "星期五",
  "星期六",
  "星期天",
};

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void RCUConfig(void);   //配置RTC时钟源
static u32  DecToBcd(u32 dec); //10进制转BCD码
static u32  BcdToDec(u32 bcd); //BCD码转10进制

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：RCUConfig
* 函数功能：配置RTC时钟源
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：调用此函数之前必须先使能对备份域寄存器的写访问
*********************************************************************************************************/
static void RCUConfig(void)
{
  //修改RTC时钟源为外部32k晶振
  if(((RCU_BDCTL >> 8) & 0x03) != 0x01)
  {
    rcu_osci_on(RCU_LXTAL);                 //开启外部32k晶振
    rcu_osci_stab_wait(RCU_LXTAL);          //等待外部晶振工作稳定
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL); //RTC时钟源选用外部晶振
  }
}

/*********************************************************************************************************
* 函数名称：DecToBcd
* 函数功能：10进制转BCD码
* 输入参数：dec：10进制数
* 输出参数：void
* 返 回 值：BCD码
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：BcdToDec
* 函数功能：BCD码转10进制
* 输入参数：bcd：BCD码
* 输出参数：void
* 返 回 值：10进制
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：RTC_Alarm_IRQHandler
* 函数功能：闹钟中断服务函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
  //闹钟0
  if(RESET != rtc_flag_get(RTC_FLAG_ALRM0))
  {
    rtc_flag_clear(RTC_FLAG_ALRM0);
    exti_flag_clear(EXTI_17);
    printf("RTC_Alarm_IRQHandler: Alarm0\r\n");
  }
  
  //闹钟1
  if(RESET != rtc_flag_get(RTC_FLAG_ALRM1))
  {
    rtc_flag_clear(RTC_FLAG_ALRM1);
    exti_flag_clear(EXTI_17);
    printf("RTC_Alarm_IRQHandler: Alarm1\r\n");
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitRTC
* 函数功能：初始化RTC模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitRTC(void)
{

}

/*********************************************************************************************************
* 函数名称：RTCSet
* 函数功能：设置RCT当前值
* 输入参数：当前时间值
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCSet(u32 year, u32 month, u32 date, u32 week, u32 hour, u32 min, u32 sec)
{
  //RTC初始化结构体
  rtc_parameter_struct rtc_initpara;

  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步

  //配置RTC参数
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
    printf("SetRTC: 配置RTC失败\r\n");
    while(1){}
  }

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCSetDate
* 函数功能：设置RTC日期
* 输入参数：当前日期
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCSetDate(u32 year, u32 month, u32 date, u32 week)
{
  //RTC初始化结构体
  rtc_parameter_struct rtc_initpara;

  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步

  //配置RTC参数
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
    printf("RTCSetDate: 配置RTC失败\r\n");
    while(1){}
  }

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCSetTime
* 函数功能：设置RTC时间
* 输入参数：当前时间
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCSetTime(u32 hour, u32 min, u32 sec)
{
  //RTC初始化结构体
  rtc_parameter_struct rtc_initpara;

  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步

  //配置RTC参数
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
    printf("RTCSetTime: 配置RTC失败\r\n");
    while(1){}
  }

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCGetDate
* 函数功能：获取当前日期
* 输入参数：返回当前日期
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCGetDate(u32* year, u32* month, u32* date, u32* week)
{
  //RTC参数结构体
  rtc_parameter_struct rtc_initpara;

  //获取当前时间
  rtc_current_time_get(&rtc_initpara);

  //返回日期
  *year = 2000 + BcdToDec(rtc_initpara.year);
  *month = rtc_initpara.month;
  *date = BcdToDec(rtc_initpara.date);
  *week = rtc_initpara.day_of_week;
}

/*********************************************************************************************************
* 函数名称：RTCGetTime
* 函数功能：获取当前日期
* 输入参数：返回当前时间
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCGetTime(u32* hour, u32* min, u32* sec)
{
  //RTC参数结构体
  rtc_parameter_struct rtc_initpara;

  //获取当前时间
  rtc_current_time_get(&rtc_initpara);

  //返回时间
  *hour = BcdToDec(rtc_initpara.hour);
  *min = BcdToDec(rtc_initpara.minute);
  *sec = BcdToDec(rtc_initpara.second);
}

/*********************************************************************************************************
* 函数名称：RTCPrintTime
* 函数功能：打印当前时间
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCPrintTime(void)
{
  u32 year, month, date, week, hour, min, sec;

  //获取时间日期
  RTCGetDate(&year, &month, &date, &week);
  RTCGetTime(&hour, &min, &sec);

  //打印输出
  printf("当前时间：%d-%02d-%02d %s %02d:%02d:%02d\r\n", year, month, date, s_arrWeek[week - 1], hour, min, sec);
}

/*********************************************************************************************************
* 函数名称：RTCSetAlarm0
* 函数功能：设置闹钟0
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCSetAlarm0(u32 hour, u32 min, u32 sec)
{
  //闹钟参数结构体
  rtc_alarm_struct rtc_alarm; 

  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM0))
  {
    printf("RTCSetAlarm0: 禁用闹钟0失败\r\n");
    while(1){}
  }

  //配置闹钟参数
  rtc_alarm.alarm_mask      = RTC_ALARM_DATE_MASK; 
  rtc_alarm.weekday_or_date = RTC_MONDAY;
  rtc_alarm.alarm_day       = DecToBcd(1);
  rtc_alarm.alarm_hour      = DecToBcd(hour);
  rtc_alarm.alarm_minute    = DecToBcd(min);
  rtc_alarm.alarm_second    = DecToBcd(sec);
  rtc_alarm.am_pm           = RTC_AM;
  rtc_alarm_config(RTC_ALARM0, &rtc_alarm);
  rtc_alarm_enable(RTC_ALARM0);

  //闹钟中断配置
  rcu_all_reset_flag_clear();                           //清除标志位
  exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING); //设置中断线
  nvic_irq_enable(RTC_Alarm_IRQn, 3, 3);                //设置中断优先级
  rtc_interrupt_enable(RTC_INT_ALARM0);                 //使能闹钟中断

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCSetAlarm1
* 函数功能：设置闹钟1
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCSetAlarm1(u32 hour, u32 min, u32 sec)
{
  //闹钟参数结构体
  rtc_alarm_struct rtc_alarm; 

  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM1))
  {
    printf("RTCSetAlarm1: 禁用闹钟1失败\r\n");
    while(1){}
  }

  //配置闹钟参数
  rtc_alarm.alarm_mask      = RTC_ALARM_DATE_MASK; 
  rtc_alarm.weekday_or_date = RTC_MONDAY;
  rtc_alarm.alarm_day       = DecToBcd(1);
  rtc_alarm.alarm_hour      = DecToBcd(hour);
  rtc_alarm.alarm_minute    = DecToBcd(min);
  rtc_alarm.alarm_second    = DecToBcd(sec);
  rtc_alarm.am_pm           = RTC_AM;
  rtc_alarm_config(RTC_ALARM1, &rtc_alarm);
  rtc_alarm_enable(RTC_ALARM1);

  //闹钟中断配置
  rcu_all_reset_flag_clear();                           //清除标志位
  exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING); //设置中断线
  nvic_irq_enable(RTC_Alarm_IRQn, 3, 3);                //设置中断优先级
  rtc_interrupt_enable(RTC_INT_ALARM1);                 //使能闹钟中断

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCDisableAlarm0
* 函数功能：禁用闹钟0
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCDisableAlarm0(void)
{
  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM0))
  {
    printf("RTCDisableAlarm0: 禁用闹钟0失败\r\n");
    while(1){}
  }

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}

/*********************************************************************************************************
* 函数名称：RTCDisableAlarm1
* 函数功能：禁用闹钟1
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RTCDisableAlarm1(void)
{
  //写入前准备
  rcu_periph_clock_enable(RCU_PMU); //使能PMU时钟
  pmu_backup_write_enable();        //允许对备份域寄存器的写访问
  RCUConfig();                      //配置RTC使用外部晶振
  rcu_periph_clock_enable(RCU_RTC); //使能RTC时钟
  rtc_register_sync_wait();         //等待同步
  if(SUCCESS != rtc_alarm_disable(RTC_ALARM1))
  {
    printf("RTCDisableAlarm1: 禁用闹钟1失败\r\n");
    while(1){}
  }

  //退出配置
  rtc_register_sync_wait();   //等待同步
  pmu_backup_write_disable(); //禁止对备份域寄存器的写访问
}
