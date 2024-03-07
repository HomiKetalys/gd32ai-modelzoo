/*********************************************************************************************************
* ģ�����ƣ�ButtonWidget.c
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

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "ButtonWidget.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void DrawBackground(StructButtonWidget* widget); //���ư�������
static void DrawText(StructButtonWidget* widget);       //��ʾ�ؼ��ַ�
static void CallBackProc(StructButtonWidget* widget);   //�ص�����
static u8   IsButtonPress(StructButtonWidget* widget);  //�жϰ����Ƿ񱻰���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DrawBackground
* �������ܣ����ư�������
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawBackground(StructButtonWidget* widget)
{
  u32 x0, y0, x1, y1;

  //��������յ�����
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //���ư�������
  if(NULL != widget->buttonBackground)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }

  //���ư����ͷ�״̬�±���
  if(BUTTON_WIDGET_RELEASE == widget->state)
  {
    if(GUI_IMAGE_TYPE_NULL == widget->releaseImageType)
    {
      //��ɫ����
      GUIFillColor(x0, y0, x1, y1, widget->releaseBackColor);
    }
    else if(GUI_IMAGE_TYPE_CLEAR != widget->releaseImageType)
    {
      //��ɫ����
      if(NULL == widget->releaseBackgroudImage)
      {
        GUIFillColor(x0, y0, x1, y1, widget->releaseBackColor);
      }
      
      //���Ʊ���ͼƬ
      else
      {
        GUIDrawImage(x0, y0, (u32)widget->releaseBackgroudImage, widget->releaseImageType);
      }
    }
  }

  //���ư�������״̬����
  else if(BUTTON_WIDGET_PRESS == widget->state)
  {
    if(GUI_IMAGE_TYPE_NULL == widget->pressImageType)
    {
      //��ɫ����
      GUIFillColor(x0, y0, x1, y1, widget->pressBackColor);
    }
    else if(GUI_IMAGE_TYPE_CLEAR != widget->pressImageType)
    {
      //��ɫ����
      if(NULL == widget->pressBackgroudImage)
      {
        GUIFillColor(x0, y0, x1, y1, widget->pressBackColor);
      }

      //���Ʊ���ͼƬ
      else
      {
        GUIDrawImage(x0, y0, (u32)widget->pressBackgroudImage, widget->pressImageType);
      }
    }
  }
}

/*********************************************************************************************************
* �������ƣ�DrawText
* �������ܣ���ʾ�ؼ��ַ�
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawText(StructButtonWidget* widget)
{
  u32 x0, y0, len;

  //��������յ�����
  x0 = widget->x0 + widget->textX0;
  y0 = widget->y0 + widget->textY0;

  //����ؼ������ʾ�ַ�����
  len = widget->width / GetFontWidth(widget->textFont);

  GUIDrawTextLine(x0, y0, (u32)widget->text, widget->textFont, NULL, widget->textColor, 1, len);
}

/*********************************************************************************************************
* �������ƣ�CallBackProc
* �������ܣ��ص�����
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CallBackProc(StructButtonWidget* widget)
{
  //����������Ӧ
  if(BUTTON_WIDGET_PRESS == widget->state)
  {
    if(NULL != widget->pressCallback)
    {
      widget->pressCallback(widget);
    }
  }

  //�����ͷ���Ӧ
  else if(BUTTON_WIDGET_RELEASE == widget->state)
  {
    if(NULL != widget->releaseCallback)
    {
      widget->releaseCallback(widget);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�IsButtonPress
* �������ܣ��жϰ����Ƿ񱻰���
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��0-�����ͷţ�1-��������
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 IsButtonPress(StructButtonWidget* widget)
{
  u32 tx, ty;         //����������Ϣ
  u32 x0, y0, x1, y1; //������㡢�յ�����λ��
  u32 i;              //ѭ������

  //��������յ�����
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //���μ�����д��㣬�鿴�Ƿ��д��㴥�����ؼ�
  for(i = 0; i < GUI_TOUCH_NUM_MAX; i++)
  {
    if(1 == GUIGetTouch(&tx, &ty, i))
    {
      //�ж��Ƿ���
      if((tx >= x0) && (tx <= x1) && (ty >= y0) && (ty <= y1))
      {
        return 1;
      }
      else
      {
        continue;
      }
    }
    else
    {
      continue;
    }
  }

  //�ް����������ؼ�������0
  return 0;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitButtonWidgetStruct
* �������ܣ����ô��������ṹ��Ĭ��ֵ
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitButtonWidgetStruct(StructButtonWidget* widget)
{
  widget->x0                    = 0;                       //��������Ĭ�����Ͻ�
  widget->y0                    = 0;                       //���������Ĭ�����Ͻ�
  widget->width                 = 130;                     //�������
  widget->height                = 65;                      //�����߶�
  widget->textX0                = 35;                      //�ַ��밴��ԭ�������ƫ����
  widget->textY0                = 20;                      //�ַ��밴��ԭ��������ƫ����
  widget->textFont              = GUI_FONT_ASCII_24;       //12x24 ASCII������
  widget->textColor             = GUI_COLOR_WHITE;         //������ɫĬ��Ϊ��ɫ
  widget->text                  = "Button";                //Ĭ����ʾButton
  widget->state                 = BUTTON_WIDGET_RELEASE;   //����Ĭ��Ϊ�ͷ�״̬
  widget->type                  = BUTTON_WIDGET_TYPE_EDGE; //Ĭ�ϱ��ؼ��
  widget->buttonBackground      = NULL;                    //�����水������
  widget->pressBackgroudImage   = NULL;                    //��������״̬��ʹ�ñ���ͼƬ
  widget->releaseBackgroudImage = NULL;                    //�����ͷ�״̬��ʹ�ñ���ͼƬ
  widget->pressImageType        = GUI_IMAGE_TYPE_BMP;      //��������״̬����ͼƬĬ��ѡ��λͼ
  widget->releaseImageType      = GUI_IMAGE_TYPE_BMP;      //�����ͷ�״̬����ͼƬĬ��ѡ��λͼ
  widget->pressBackColor        = GUI_COLOR_GREEN;         //��ʹ�ñ���ͼƬʱĬ�ϰ�������ʱʹ����ɫ���
  widget->releaseBackColor      = GUI_COLOR_BLUE;          //��ʹ�ñ���ͼƬʱĬ�ϰ����ͷ�ʱʹ����ɫ���
  widget->pressCallback         = NULL;                    //��������ʱ��ʹ�ûص�
  widget->releaseCallback       = NULL;                    //�����ͷ�ʱ��ʹ�ûص�
}

/*********************************************************************************************************
* �������ƣ�CreateButtonWidget
* �������ܣ��������������ؼ�
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateButtonWidget(StructButtonWidget* widget)
{
  //���水������
  if(NULL != widget->buttonBackground)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }

  //����Ĭ��Ϊ�ͷ�״̬
  widget->state = BUTTON_WIDGET_RELEASE;

  //ˢ�±���
  DrawBackground(widget);

  //���ƿؼ��ַ�
  DrawText(widget);
}

/*********************************************************************************************************
* �������ƣ�ScanButtonWidget
* �������ܣ�����ɨ��
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void ScanButtonWidget(StructButtonWidget* widget)
{
  u8 buttonUpdateFlag;    //����״̬���±�־λ

  //��հ���״̬���±�־λ
  buttonUpdateFlag = 0;

  //���㱻����
  if(1 == IsButtonPress(widget))
  {
    //��һ��ɨ��ʱ����Ϊ�ͷ�״̬
    if(BUTTON_WIDGET_RELEASE == widget->state)
    {
      //���ð���Ϊ����״̬
      widget->state = BUTTON_WIDGET_PRESS;

      //��ǰ���״̬�ı�
      buttonUpdateFlag = 1;
    }
  }

  //���㱻�ͷ�
  else
  {
    //��һ��ɨ��ʱ����Ϊ����״̬
    if(BUTTON_WIDGET_PRESS == widget->state)
    {
      //���ð���Ϊ�ͷ�״̬
      widget->state = BUTTON_WIDGET_RELEASE;

      //��ǰ���״̬�ı�
      buttonUpdateFlag = 1;
    }
  }

  //���ؼ���»ص�����
  if(BUTTON_WIDGET_TYPE_EDGE == widget->type)
  {
    if(1 == buttonUpdateFlag)
    {
      CallBackProc(widget);
    }
  }

  //��ƽ����»ص�����
  else if(BUTTON_WIDGET_TYPE_LEVEL == widget->type)
  {
    CallBackProc(widget);
  }

  //ˢ�±���
  if(1 == buttonUpdateFlag)
  {
    //ˢ�±���
    DrawBackground(widget);

    //���ƿؼ��ַ�
    DrawText(widget);
  }
}

/*********************************************************************************************************
* �������ƣ�DeleteButtonWidget
* �������ܣ�ɾ�������ؼ�
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteButtonWidget(StructButtonWidget* widget)
{
  //���������ȥ
  if(NULL != widget->buttonBackground)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }
}

/*********************************************************************************************************
* �������ƣ�UpdateButtonWidget
* �������ܣ��ֶ�������ʾ
* ���������widget���ؼ��ṹ��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺void
*********************************************************************************************************/
void UpdateButtonWidget(StructButtonWidget* widget)
{
  //ˢ�±���
  DrawBackground(widget);

  //���ƿؼ��ַ�
  DrawText(widget);
}
