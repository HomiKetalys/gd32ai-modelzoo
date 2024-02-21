/*********************************************************************************************************
* ģ�����ƣ�GUIEthernet.h
* ժ    Ҫ��GUI��̫��ͨ��ģ��
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
#ifndef _GUI_ETHERNET_H_
#define _GUI_ETHERNET_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void CreateGUIEthernet(void); //����GUI Ethernet����
void DeleteGUIEthernet(void); //ע��GUI Ethernet����
void GUIEthernetPoll(void);   //GUI Ethernet��ѯ����

#endif