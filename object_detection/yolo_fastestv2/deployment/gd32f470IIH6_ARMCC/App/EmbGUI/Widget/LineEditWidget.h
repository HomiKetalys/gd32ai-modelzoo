/*********************************************************************************************************
* ģ�����ƣ�LineEditWidget.h
* ժ    Ҫ���б༭�ؼ�ģ��
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
#ifndef _LINE_EDIT_WIDGET_H_
#define _LINE_EDIT_WIDGET_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define LINE_EDIT_MAX_LEN ((GUI_X_PIXEL_SIZE / (GUI_SMALLEST_FONT / 2)) + 1) //һ����ʾ������ַ���

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/
typedef enum
{
  EDIT_LINE_NO_FOCUS = 0, //�޽���
  EDIT_LINE_HAS_FOCUS = 1, //�н���
}EnumEditLineFocus;

//�б༭�ؼ�
typedef struct
{
  u32               x0, y0, width, height;        //�ؼ�����ʹ�С
  u32               textX0;                       //��ʾ�ַ���������ؼ���ʼ�������ƫ����
  u32               textY0;                       //��ʾ�ַ���������ؼ���ʼ�������ƫ����
  EnumGUIFont       textFont;                     //��ʾ�ַ������С
  u32               textColor;                    //��ʾ�ַ���ɫ
  EnumEditLineFocus hasFocus;                     //����״̬���ؼ������º��Զ���ȡ����
  void*             background;                   //����������
  void*             hasFocusBackgroundImage;      //�н���ʱ�ı���ͼƬ����ʹ�ñ���ͼƬ������NULL
  void*             noFocusBackgroundImage;       //�޽���ʱ�ı���ͼƬ����ʹ��������NULL
  EnumGUIImageType  hasFocusImageType;            //�н���ʱ����ͼƬ�ĸ�ʽ��Ĭ��ʹ��JPEGͼƬ
  EnumGUIImageType  noFocusImageType;             //�޽���ʱ����ͼƬ�ĸ�ʽ��Ĭ��ʹ��JPEGͼƬ
  u32               hasFocusColor;                //�н���ʱ��ɫ��������ʹ�ñ���ͼƬ��ʹ�ã�Ĭ����ɫ
  u32               noFocusColor;                 //�޽���ʱ��ɫ��������ʹ�ñ���ͼƬ��ʹ�ã�Ĭ����ɫ
  void              (*pressCallback)(void* this); //�ؼ���������Ӧ�ص������������º�ؼ��Զ���ȡ����

  //˽�б������û������޸�
  char              preText[LINE_EDIT_MAX_LEN];   //ǰ׺
  char              text[LINE_EDIT_MAX_LEN];      //�ַ���
  u32               editNum;                      //����ַ�������
}StructLineEditWidget;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void InitLineEditWidgetStruct(StructLineEditWidget* widget);          //�����б༭�ؼ��ṹ��Ĭ��ֵ
void CreateLineEditWidget(StructLineEditWidget* widget);              //��ʼ���б༭�ؼ�ģ��
void ScanLineEditWidget(StructLineEditWidget* widget);                //ɨ���б༭�ؼ�ģ��
void ClearLineEditWidgetFocus(StructLineEditWidget* widget);          //ȡ������
void AddrCharToLineEdit(StructLineEditWidget* widget, u32 code);      //�����ַ�
void DeleteCodeFromLineEdit(StructLineEditWidget* widget);            //ɾ���ַ�
void SetLineEditPreText(StructLineEditWidget* widget, char* preTexy); //�����б༭ǰ׺
void SetLineEditMaxLen(StructLineEditWidget* widget, u32 len);        //�����б༭�ַ�������
void DeleteLineEditWidget(StructLineEditWidget* widget);              //ɾ���б༭�ؼ�

#endif
