/*********************************************************************************************************
* ģ�����ƣ�GUIKeyBoard.h
* ժ    Ҫ������ģ��
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
#ifndef _GUI_KEY_BOARD_H_
#define _GUI_KEY_BOARD_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define KEY_BOARD_SCAN_TIME 100

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void CreateGUIKeyBoard(void); //��������
void DeleteGUIKeyBoard(void); //ɾ������
void GUIKeyBoardPoll(void);   //������ѯ����

#endif