/*********************************************************************************************************
* 模块名称：RTC.h
* 摘    要：RTC驱动模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日 
* 内    容：
* 注    意：默认使用24小时制
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
//初始化RTC模块
void InitRTC(void); 

//时间日期配置
void RTCSet(u32 year, u32 month, u32 date, u32 week, u32 hour, u32 min, u32 sec);
void RTCSetDate(u32 year, u32 month, u32 date, u32 week);
void RTCSetTime(u32 hour, u32 min, u32 sec);

//获取时间日期
void RTCGetDate(u32* year, u32* month, u32* date, u32* week);
void RTCGetTime(u32* hour, u32* min, u32* sec);
void RTCPrintTime(void);

//设置闹钟
void RTCSetAlarm0(u32 hour, u32 min, u32 sec);
void RTCSetAlarm1(u32 hour, u32 min, u32 sec);
void RTCDisableAlarm0(void);
void RTCDisableAlarm1(void);

#endif
