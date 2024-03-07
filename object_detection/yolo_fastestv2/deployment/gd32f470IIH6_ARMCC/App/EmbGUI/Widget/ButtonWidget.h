/*********************************************************************************************************
* ģ�����ƣ�ButtonWidget.h
* ժ    Ҫ�����������ؼ�ģ��
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
#ifndef _BUTTON_WIDGET_H_
#define _BUTTON_WIDGET_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
typedef enum
{
  BUTTON_WIDGET_TYPE_EDGE,  //���ش�����Ĭ��״̬��
  BUTTON_WIDGET_TYPE_LEVEL, //��ƽ����
}EnumButtonWidgetType;

//���¡��ͷ�ö��
typedef enum
{
  BUTTON_WIDGET_RELEASE = 0, //�����ͷ�
  BUTTON_WIDGET_PRESS   = 1, //��������
}EnumButtonWidgetFlag;

//���������ؼ�
typedef struct
{
  u32                  x0;        //�ؼ���ʼ������
  u32                  y0;        //�ؼ���ʼ������
  u32                  width;     //��ť���
  u32                  height;    //��ť�߶�
  u32                  textX0;    //��ʾ�ַ���������ؼ���ʼ�������ƫ����
  u32                  textY0;    //��ʾ�ַ���������ؼ���ʼ�������ƫ����
  EnumGUIFont          textFont;  //��ʾ�ַ������С
  u32                  textColor; //��ʾ�ַ���ɫ
  const char*          text;      //�ؼ��ַ�
  EnumButtonWidgetFlag state;     //������ǰ״̬
  EnumButtonWidgetType type;      //��������
  void*                buttonBackground;      //��������µı���������������������NULL
  void*                pressBackgroudImage;   //��������״̬�±���ͼƬ������������NULL
  void*                releaseBackgroudImage; //�����ͷ�״̬�±���ͼƬ������������NULL
  EnumGUIImageType     pressImageType;        //���±���ͼƬ��ʽ��Ĭ��ʹ��λͼ
  EnumGUIImageType     releaseImageType;      //�ͷű���ͼƬ��ʽ��Ĭ��ʹ��λͼ
  u32                  pressBackColor;        //��ɫ״̬�°������±�����ɫ��Ĭ����ɫ
  u32                  releaseBackColor;      //��ɫ״̬�°����ͷű�����ɫ��Ĭ����ɫ
  void                 (*pressCallback)(void* this);   //�������»ص�����������������NULL
  void                 (*releaseCallback)(void* this); //�����ͷŻص�����������������NULL
}StructButtonWidget;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void InitButtonWidgetStruct(StructButtonWidget* widget); //���ô��������ṹ��Ĭ��ֵ
void CreateButtonWidget(StructButtonWidget* widget);     //�������������ؼ�
void ScanButtonWidget(StructButtonWidget* widget);       //����ɨ��
void DeleteButtonWidget(StructButtonWidget* widget);     //ɾ�������ؼ�
void UpdateButtonWidget(StructButtonWidget* widget);     //�ֶ�������ʾ

#endif
