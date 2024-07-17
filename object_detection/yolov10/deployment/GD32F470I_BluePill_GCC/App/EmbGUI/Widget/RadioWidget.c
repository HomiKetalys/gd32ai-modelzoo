/*********************************************************************************************************
* ģ�����ƣ�RadioWidget.c
* ժ    Ҫ����ѡ�ؼ�ģ��
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
#include "RadioWidget.h"

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
static void DrawBackground(StructRadioWidget* widget); //���Ʊ���
static void CallBackProc(StructRadioWidget* widget);   //�ص�����
static u8   IsPress(StructRadioWidget* widget);        //�жϿؼ��Ƿ񱻰���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DrawBackground
* �������ܣ����Ʊ�����ͼƬ
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawBackground(StructRadioWidget* widget)
{
  u32 x0, y0, x1, y1;

  //��������յ�����
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //���Ʊ���
  if(NULL != widget->background)
  {
    GUIDrawBackground(x0, y0, widget->width, widget->height, (u32)widget->background);
  }

  //ʹ��ͼƬ��ʶ�ؼ����޽���
  if(RADIO_TYPE_IMAGE == widget->type)
  {
    //�ؼ���ý���
    if(RADIO_HAS_FOCUS == widget->hasFocus)
    {
      if(GUI_IMAGE_TYPE_NULL == widget->hasFocusImageType)
      {
        //���ƴ�ɫ���
        GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
      }
      else if(GUI_IMAGE_TYPE_CLEAR != widget->hasFocusImageType)
      {
        //���ƴ�ɫ���
        if(NULL == widget->hasFocusBackgroundImage)
        {
          GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
        }

        //����ͼƬ
        else
        {
          GUIDrawImage(x0, y0, (u32)widget->hasFocusBackgroundImage, widget->hasFocusImageType);
        }
      }
    }

    //�ؼ��޽���
    else if(RADIO_NO_FOCUS == widget->hasFocus)
    {
      if(GUI_IMAGE_TYPE_NULL == widget->noFocusImageType)
      {
        //���ƴ�ɫ���
        GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
      }
      else if(GUI_IMAGE_TYPE_CLEAR != widget->noFocusImageType)
      {
        //���ƴ�ɫ���
        if(NULL == widget->noFocusBackgroundImage)
        {
          GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
        }

        //����ͼƬ
        else
        {
          GUIDrawImage(x0, y0, (u32)widget->noFocusBackgroundImage, widget->noFocusImageType);
        }
      }
    }
  }
}

/*********************************************************************************************************
* �������ƣ�DrawFocus
* �������ܣ������н���ʱ�ľ����������
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawFocus(StructRadioWidget* widget)
{
  u32 lineSize, x0, y0, x1, y1;

  //�ؼ��޽�������Ͳ��ԣ�ֱ�ӷ���
  if((RADIO_TYPE_IMAGE == widget->type) || (RADIO_HAS_FOCUS != widget->hasFocus))
  {
    return;
  }

  //��ȡֱ���߿��һ��
  lineSize = widget->lineSize / 2;
  if(0 == lineSize)
  {
    return;
  }

  //������������յ�
  x0 = widget->x0 + widget->lineSize;
  y0 = widget->y0 + widget->lineSize;
  x1 = widget->x0 + widget->width  - widget->lineSize - 1;
  y1 = widget->y0 + widget->height - widget->lineSize - 1;

  //���ƾ���
  if(RADIO_TYPE_RECT == widget->type)
  {
    GUIDrawLine(x0, y0, x1, y0, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x0, y0, x0, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x1, y0, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x0, y1, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
  }

  //���ƺ���
  else if(RADIO_TYPE_LINE == widget->type)
  {
    GUIDrawLine(x0, y1, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
  }
}

/*********************************************************************************************************
* �������ƣ�CallBackProc
* �������ܣ��ص�����
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CallBackProc(StructRadioWidget* widget)
{
  if(NULL != widget->pressCallback)
  {
    widget->pressCallback(widget);
  }
}

/*********************************************************************************************************
* �������ƣ�IsPress
* �������ܣ��жϿؼ��Ƿ���
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��1-�ؼ������£�0-�ؼ�δ������
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 IsPress(StructRadioWidget* widget)
{
  u32 tx, ty;         //����������Ϣ
  u8  pointState;     //���㴥��״̬
  u32 x0, y0, x1, y1; //������㡢�յ�����λ��

  //��ȡ��Ļ������Ϣ
  pointState = GUIGetTouch(&tx, &ty, 0);

  //����1��δ����
  if(0 == pointState)
  {
    return 0;
  }

  //����1����
  else if(1 == pointState)
  {
    //��������յ�����
    x0 = widget->x0;
    y0 = widget->y0;
    x1 = widget->x0 + widget->width;
    y1 = widget->y0 + widget->height;

    //�ж��Ƿ���
    if((tx >= x0) && (tx <= x1) && (ty >= y0) && (ty <= y1))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  
  return 0;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitRadioWidgetStruct
* �������ܣ����õ�ѡ�ؼ��ṹ��Ĭ��ֵ
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitRadioWidgetStruct(StructRadioWidget* widget)
{
  widget->x0                      = 0;                   //��������Ĭ�����Ͻ�
  widget->y0                      = 0;                   //���������Ĭ�����Ͻ�
  widget->width                   = 50;                  //���
  widget->height                  = 50;                  //�߶�
  widget->hasFocus                = RADIO_NO_FOCUS;      //�޽���
  widget->type                    = RADIO_TYPE_RECT;     //ʹ�ð�ɫ���α�ʶ���޽���
  widget->lineColor               = GUI_COLOR_WHITE;     //ʹ�ð�ɫ����
  widget->lineSize                = 2;                   //������С
  widget->background              = NULL;                //�ؼ�����������
  widget->hasFocusBackgroundImage = NULL;                //�н���ʱ��ʹ��ͼƬ
  widget->noFocusBackgroundImage  = NULL;                //�޽���ʱ��ʹ��ͼƬ
  widget->hasFocusImageType       = GUI_IMAGE_TYPE_JPEG; //�н���ʱ����ͼƬ��ʽĬ��ΪJPEG
  widget->noFocusImageType        = GUI_IMAGE_TYPE_JPEG; //�޽���ʱ����ͼƬ��ʽĬ��ΪJPEG
  widget->hasFocusColor           = GUI_COLOR_GREEN;     //��ʹ�ñ���ͼƬʱ�н�������ɫ���
  widget->noFocusColor            = GUI_COLOR_BLUE;      //��ʹ�ñ���ͼƬʱ�޽�������ɫ���
  widget->pressCallback           = NULL;                //��ʹ�ûص�
}

/*********************************************************************************************************
* �������ƣ�CreateRadioWidget
* �������ܣ���ʼ����ѡ�ؼ�
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateRadioWidget(StructRadioWidget* widget)
{
  //�ؼ�Ĭ���޽���
  widget->hasFocus = RADIO_NO_FOCUS;

  //����ؼ�����
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //ˢ�±���
  DrawBackground(widget);
}

/*********************************************************************************************************
* �������ƣ�ScanRadioWidget
* �������ܣ���ѡ�ؼ�ɨ��
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void ScanRadioWidget(StructRadioWidget* widget)
{
  //�ؼ�������
  if(1 == IsPress(widget))
  {
    //�Զ���ȡ�ؼ�����
    if(RADIO_NO_FOCUS == widget->hasFocus)
    {
      //�ص�����
      CallBackProc(widget);

      //�ؼ���ý���
      widget->hasFocus = RADIO_HAS_FOCUS;

      //�����ػ�
      DrawBackground(widget);

      //���ƾ����������
      DrawFocus(widget);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�ClearRadioWidgetFocus
* �������ܣ������ѡ�ؼ�����
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void ClearRadioWidgetFocus(StructRadioWidget* widget)
{
  if(RADIO_HAS_FOCUS == widget->hasFocus)
  {
    //�����ѡ�ؼ�����
    widget->hasFocus = RADIO_NO_FOCUS;

    //�����ػ�
    DrawBackground(widget);
  }
}

/*********************************************************************************************************
* �������ƣ�DeleteRadioWidget
* �������ܣ�ɾ����ѡ�ؼ�
* ���������widget����ѡ�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteRadioWidget(StructRadioWidget* widget)
{
  //��ر���
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
