/*********************************************************************************************************
* ģ�����ƣ�GUIButton.c
* ժ    Ҫ��GUI����ģ��
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

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "GUIButton.h"
#include "TLILCD.h"
#include "ButtonWidget.h"
#include "BMP.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //��������
static EnumGUIButtons     s_enumButtonPress;           //��������Ǹ���������

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ButtonPressCallback(void* button);  //�����ص�����

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ButtonPressCallback
* �������ܣ������ص�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ButtonPressCallback(void* button)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_enumButtonPress = i;
      return;
    }
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitGUIButton
* �������ܣ���ʼ��GUI����ģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitGUIButton(void)
{
  //����ް�������
  s_enumButtonPress = GUI_BUTTON_NONE;

//������һ�ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PREVIOUS]);
  s_arrButton[GUI_BUTTON_PREVIOUS].x0 = 52;
  s_arrButton[GUI_BUTTON_PREVIOUS].y0 = 733;
  s_arrButton[GUI_BUTTON_PREVIOUS].width = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].height = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].text = "";
  s_arrButton[GUI_BUTTON_PREVIOUS].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PREVIOUS]);

  //������һ�ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_NEXT]);
  s_arrButton[GUI_BUTTON_NEXT].x0 = 373;
  s_arrButton[GUI_BUTTON_NEXT].y0 = 733;
  s_arrButton[GUI_BUTTON_NEXT].width = 53;
  s_arrButton[GUI_BUTTON_NEXT].height = 53;
  s_arrButton[GUI_BUTTON_NEXT].text = "";
  s_arrButton[GUI_BUTTON_NEXT].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_NEXT]);

  //�������հ�ť
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PHOTO]);
  s_arrButton[GUI_BUTTON_PHOTO].x0 = 183;
  s_arrButton[GUI_BUTTON_PHOTO].y0 = 622;
  s_arrButton[GUI_BUTTON_PHOTO].width = 115;
  s_arrButton[GUI_BUTTON_PHOTO].height = 115;
  s_arrButton[GUI_BUTTON_PHOTO].text = "";
  s_arrButton[GUI_BUTTON_PHOTO].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PHOTO]);
}

/*********************************************************************************************************
* �������ƣ�ScanGUIButton
* �������ܣ�����ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ÿ��20����ִ��һ��
*********************************************************************************************************/
EnumGUIButtons ScanGUIButton(void)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_enumButtonPress = GUI_BUTTON_NONE;
    ScanButtonWidget(&s_arrButton[i]);

    if(GUI_BUTTON_NONE != s_enumButtonPress)
    {
      return s_enumButtonPress;
    }
  }

  return GUI_BUTTON_NONE;
}

