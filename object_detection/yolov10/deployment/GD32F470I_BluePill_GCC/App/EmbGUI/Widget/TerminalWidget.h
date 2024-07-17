/*********************************************************************************************************
* ģ�����ƣ�TerminalWidget.h
* ժ    Ҫ���ն˿ؼ�ģ��
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
#ifndef _TERMINAL_WIDGET_H_
#define _TERMINAL_WIDGET_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
//��Ļ����ʾ������ַ�������
#define TERMINAL_LINE_CHAR_NUM (GUI_X_PIXEL_SIZE / (GUI_SMALLEST_FONT / 2))
#define TERMINAL_MAX_LIME_MUN  (1 + (GUI_Y_PIXEL_SIZE / GUI_SMALLEST_FONT))
#define TERMINAL_MAX_CHAR_NUM  (TERMINAL_LINE_CHAR_NUM * TERMINAL_MAX_LIME_MUN)

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef struct
{
  //�ؼ���������
  u32              x0, y0, width, height; //λ�ã���С
  u32              topOff, bottomOff;     //��ʾ���򶥲����ײ�ƫ����
  u32              showSpace;             //��ʾ������ؼ�����֮��ļ��
  u32              textColor;             //������ɫ
  EnumGUIFont      textFont;              //�����С
  u32              textHeight;            //һ�и߶ȣ�Ҫ���ڵ����ַ��߶�
  void*            background;            //�ؼ�����������ɾ���ؼ�ʱ��ر���������Ҫ��������NULL
  void*            backImage;             //�ؼ�����ͼƬ���ն˱���ͼƬ��ʾ�������Ǵ�ɫ
  EnumGUIImageType imageType;             //�ؼ�����ͼƬ����
  u32              defaultBackColor;      //Ĭ�ϵı�����ɫ��backImageΪNULLʱʹ��

  //���²����Զ����㣬�û����ɸ���
  //������־λ��0-δ������1-�Ѵ���
  u32 hasCreate;

  //�ַ���������ɫ
  u32 textBackColor;

  //��ʾ�����С
  u32 showX0, showY0, showX1, showY1, showWidth, showHeight;

  //һ����ʾ�ַ������Լ��������
  u32 lineCharNum, maxLineNum;

  //�ַ������������ܴ�����StructTerminalWidget�ṹ������ɶ�̬�ڴ����
  u8 stringBuf[TERMINAL_MAX_CHAR_NUM]; //һ����lineCharNum+1��Ԫ��

  //���в���λ��
  u32 stringHead;

  //����ʾ����
  u32 stringShowNum;
}StructTerminalWidget;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void InitTerminalWidgetStruct(StructTerminalWidget* widget);             //��ʼ���ն��豸�ṹ��
void CreateTerminalWidget(StructTerminalWidget* widget);                 //�����ն˿ؼ�
void TerminalWidgetClear(StructTerminalWidget* widget);                  //����ն���ʾ
void TerminalWidgetShowLine(StructTerminalWidget* widget, char* string); //�ն�����ʾһ���ַ���
void DeleteTerminalWidget(StructTerminalWidget* widget);                 //ɾ���ն˿ؼ�

#endif
