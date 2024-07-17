/*********************************************************************************************************
* ģ�����ƣ�GUITop.h
* ժ    Ҫ������������
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
#ifndef _GUI_TOP_H_
#define _GUI_TOP_H_

#include "DataType.h"
#include "GUIRadio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//GUI�豸�ṹ�壬���Ǿ�̬�ṹ��
typedef struct
{
  void (*previousCallback)(EnumGUIRadio radio);    //ǰһ����ť�ص�����
  void (*nextCallback)(EnumGUIRadio radio);        //��һ����ť�ص�����
  void (*radioChangeCallback)(EnumGUIRadio radio); //��ѡ�ؼ��ı�ص�����
  void (*takePhotoCallback)(void);                 //���հ�ť�ص�����
  void (*showInfo)(char* string);                  //��ʾ��Ϣ��֧������
}StructGUIDev;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void InitGUI(StructGUIDev* dev);  //��ʼ��GUI
void GUITask(void);               //GUI����

#endif