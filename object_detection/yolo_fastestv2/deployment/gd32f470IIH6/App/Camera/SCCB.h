/*********************************************************************************************************
* ģ�����ƣ�SCCB.h
* ժ    Ҫ��SCCBЭ������
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
#ifndef _SCCB_H_
#define _SCCB_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define SCCB_ID  0x60  //OV2640��ID

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void InitSCCB(void);               //��ʼ��SCCB�ӿ�
u8   SCCBWriteReg(u8 reg,u8 data); //д�Ĵ���
u8   SCCBReadReg(u8 reg);          //���Ĵ���

#endif