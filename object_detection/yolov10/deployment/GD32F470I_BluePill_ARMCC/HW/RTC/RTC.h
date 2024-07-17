/*********************************************************************************************************
* ģ�����ƣ�RTC.h
* ժ    Ҫ��RTC����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01�� 
* ��    �ݣ�
* ע    �⣺Ĭ��ʹ��24Сʱ��
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/
#ifndef _RTC_H_
#define _RTC_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
//��ʼ��RTCģ��
void InitRTC(void); 

//ʱ����������
void RTCSet(u32 year, u32 month, u32 date, u32 week, u32 hour, u32 min, u32 sec);
void RTCSetDate(u32 year, u32 month, u32 date, u32 week);
void RTCSetTime(u32 hour, u32 min, u32 sec);

//��ȡʱ������
void RTCGetDate(u32* year, u32* month, u32* date, u32* week);
void RTCGetTime(u32* hour, u32* min, u32* sec);
void RTCPrintTime(void);

//��������
void RTCSetAlarm0(u32 hour, u32 min, u32 sec);
void RTCSetAlarm1(u32 hour, u32 min, u32 sec);
void RTCDisableAlarm0(void);
void RTCDisableAlarm1(void);

#endif