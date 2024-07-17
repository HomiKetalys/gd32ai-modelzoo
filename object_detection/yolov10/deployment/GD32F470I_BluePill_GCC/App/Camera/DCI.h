/*********************************************************************************************************
* ģ�����ƣ�DCI.h
* ժ    Ҫ����������ͷ�ӿ�����ģ��
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
#ifndef _DCI_H_
#define _DCI_H_

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
//��ʼ����������ͷ�ӿ�����ģ��
void InitDCI(u32 frameAddr, u32 frameSize, u32 x0, u32 y0, u32 width, u32 height, u32 mode);

//ע����������ͷ�ӿ�����ģ��
void DeInitDCI(void);

//��ʾһ֡ͼ����Ļ��
void DCIShowImage(void);

void ConfigDCI(void);



#endif