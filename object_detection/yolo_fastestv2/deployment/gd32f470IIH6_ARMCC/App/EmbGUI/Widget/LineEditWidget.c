/*********************************************************************************************************
* ģ�����ƣ�LineEditWidget.c
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

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "LineEditWidget.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void DrawBackground(StructLineEditWidget* widget); //���Ʊ���
static void DrawText(StructLineEditWidget* widget);       //��ʾ�ؼ��ַ�
static void CallBackProc(StructLineEditWidget* widget);   //�ص�����
static u8   IsPress(StructLineEditWidget* widget);        //�жϿؼ��Ƿ񱻰���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DrawBackground
* �������ܣ����Ʊ���
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawBackground(StructLineEditWidget* widget)
{
  u32 x0, y0, x1, y1;

  //��������յ�����
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //�ؼ���ý���
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //���ƴ�ɫ����
    if(NULL == widget->hasFocusBackgroundImage)
    {
      GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
    }

    //���Ʊ���ͼƬ
    else
    {
      GUIDrawImage(x0, y0, (u32)widget->hasFocusBackgroundImage, widget->hasFocusImageType);
    }
  }

  //�ؼ��޽���
  else if(EDIT_LINE_NO_FOCUS == widget->hasFocus)
  {
    //���ƴ�ɫ����
    if(NULL == widget->noFocusBackgroundImage)
    {
      GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
    }

    //���Ʊ���ͼƬ
    else
    {
      GUIDrawImage(x0, y0, (u32)widget->noFocusBackgroundImage, widget->noFocusImageType);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�DrawText
* �������ܣ���ʾ�ؼ��ַ�
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawText(StructLineEditWidget* widget)
{
  static char s_arrString[LINE_EDIT_MAX_LEN];
  u32 x0, y0, len, i, cnt;

  //����ǰ׺��������
  i = 0;
  cnt = 0;
  while(0 != widget->preText[i])
  {
    s_arrString[cnt++] = widget->preText[i++];
  }
  
  //����������������
  i = 0;
  while(0 != widget->text[i])
  {
    s_arrString[cnt++] = widget->text[i++];
  }
  s_arrString[cnt] = 0;

  //��������յ�����
  x0 = widget->x0 + widget->textX0;
  y0 = widget->y0 + widget->textY0;

  //����ؼ������ʾ�ַ�����
  len = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);

  GUIDrawTextLine(x0, y0, (u32)s_arrString, widget->textFont, NULL, widget->textColor, 1, len);
}

/*********************************************************************************************************
* �������ƣ�CallBackProc
* �������ܣ��ص�����
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CallBackProc(StructLineEditWidget* widget)
{
  if(NULL != widget->pressCallback)
  {
    widget->pressCallback(widget);
  }
}

/*********************************************************************************************************
* �������ƣ�IsPress
* �������ܣ��жϿؼ��Ƿ���
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��1-�ؼ������£�0-�ؼ�δ������
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 IsPress(StructLineEditWidget* widget)
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
* �������ƣ�InitLineEditWidgetStruct
* �������ܣ������б༭�ؼ��ṹ��Ĭ��ֵ
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitLineEditWidgetStruct(StructLineEditWidget* widget)
{
  u32 i;
  widget->x0                      = 0;                   //��������Ĭ�����Ͻ�
  widget->y0                      = 0;                   //���������Ĭ�����Ͻ�
  widget->width                   = 100;                 //���
  widget->height                  = 50;                  //�߶�
  widget->textX0                  = 75;                  //�ַ���ؼ�ԭ�������ƫ����
  widget->textY0                  = 10;                  //�ַ���ؼ�ԭ��������ƫ����
  widget->textFont                = GUI_FONT_ASCII_24;   //12x24 ASCII������
  widget->textColor               = GUI_COLOR_WHITE;     //������ɫĬ��Ϊ��ɫ
  widget->background              = NULL;                //Ĭ���ޱ���
  widget->hasFocusBackgroundImage = NULL;                //�н���ʱ��ʹ�ñ���ͼƬ
  widget->noFocusBackgroundImage  = NULL;                //�޽���ʱ��ʹ�ñ���ͼƬ
  widget->hasFocusImageType       = GUI_IMAGE_TYPE_JPEG; //�н���ʱ����ͼƬ��ʽĬ��ΪJPEG
  widget->noFocusImageType        = GUI_IMAGE_TYPE_JPEG; //�޽���ʱ����ͼƬ��ʽĬ��ΪJPEG
  widget->hasFocusColor           = GUI_COLOR_GREEN;     //��ʹ�ñ���ͼƬʱ�н�������ɫ���
  widget->noFocusColor            = GUI_COLOR_BLUE;      //��ʹ�ñ���ͼƬʱ�޽�������ɫ���
  widget->pressCallback           = NULL;                //��ʹ�ûص�
  widget->editNum                 = LINE_EDIT_MAX_LEN;   //Ĭ��������󳤶�
  for(i = 0; i < LINE_EDIT_MAX_LEN; i++)
  {
    widget->preText[i] = 0;
    widget->text[i] = 0;
  }
}

/*********************************************************************************************************
* �������ƣ�CreateLineEditWidget
* �������ܣ���ʼ���б༭�ؼ�ģ��
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateLineEditWidget(StructLineEditWidget* widget)
{
  //ͳ����ʾ��󳤶�
  widget->editNum = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);

  //����ؼ�����
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //�ؼ�Ĭ���޽���
  widget->hasFocus = EDIT_LINE_NO_FOCUS;

  //ˢ�±���
  DrawBackground(widget);

  //������ʾ�ַ�
  DrawText(widget);
}

/*********************************************************************************************************
* �������ƣ�ScanLineEditWidget
* �������ܣ��б༭�ؼ�ɨ��
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void ScanLineEditWidget(StructLineEditWidget* widget)
{
  //�ؼ�������
  if(1 == IsPress(widget))
  {
    //�Զ���ȡ�ؼ�����
    if(EDIT_LINE_NO_FOCUS == widget->hasFocus)
    {
      //�ص�����
      CallBackProc(widget);

      //�ؼ���ý���
      widget->hasFocus = EDIT_LINE_HAS_FOCUS;

      //�����ػ�
      DrawBackground(widget);

      //�ַ��ػ�
      DrawText(widget);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�ClearLineEditWidgetFocus
* �������ܣ�����б༭�ؼ�����
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void ClearLineEditWidgetFocus(StructLineEditWidget* widget)
{
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //����б༭�ؼ�����
    widget->hasFocus = EDIT_LINE_NO_FOCUS;

    //�����ػ�
    DrawBackground(widget);

    //�ַ��ػ�
    DrawText(widget);
  }
}

/*********************************************************************************************************
* �������ƣ�AddrCharToLineEdit
* �������ܣ������ַ�
* ���������widget���б༭�ؼ���code���ַ�����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ڿؼ���ý��������¸����ַ���ʾ
*********************************************************************************************************/
void AddrCharToLineEdit(StructLineEditWidget* widget, u32 code)
{
  u32 maxShowLen, preLen, editLen, stringLen; //�ַ�������
  u32 i;

  //���ڿؼ���ý��������¸����ַ���ʾ
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //��ʾ��ַ���
    maxShowLen = widget->editNum;

    //ͳ��ǰ׺����
    preLen = 0;
    i = 0;
    while(0 != widget->preText[i++])
    {
      preLen++;
    }

    //ͳ�Ʊ༭�ַ�������
    editLen = 0;
    i = 0;
    while(0 != widget->text[i++])
    {
      editLen++;
    }

    //���㵱ǰ�ַ�������
    stringLen = preLen + editLen;

    //����µ��ַ���ʾ
    if(stringLen < maxShowLen)
    {
      //���Ӣ���ַ�
      if(code < 0x80)
      {
        widget->text[editLen] = code;
        widget->text[editLen + 1] = 0;

        //�����ػ�
        DrawBackground(widget);

        //�ַ��ػ�
        DrawText(widget);
      }

      //��������ַ�
      else if((stringLen + 1) < maxShowLen)
      {
        widget->text[editLen] = code >> 8;
        widget->text[editLen + 1] = code & 0xFF;
        widget->text[editLen + 2] = 0;

        //�����ػ�
        DrawBackground(widget);

        //�ַ��ػ�
        DrawText(widget);
      }
    }
  }
}

/*********************************************************************************************************
* �������ƣ�DeleteCodeFromLineEdit
* �������ܣ�ɾ���ַ�
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ڿؼ���ý��������¸����ַ���ʾ
*********************************************************************************************************/
void DeleteCodeFromLineEdit(StructLineEditWidget* widget)
{
  u32 i, editLen;

  //���ڿؼ���ý��������¸����ַ���ʾ
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //ͳ�Ʊ༭�ַ�������
    editLen = 0;
    i = 0;
    while(0 != widget->text[i++])
    {
      editLen++;
    }

    //ɾ���ַ�
    if(editLen > 0)
    {
      widget->text[editLen - 1] = 0;

      //�����ػ�
      DrawBackground(widget);

      //�ַ��ػ�
      DrawText(widget);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�SetLineEditPreText
* �������ܣ������б༭ǰ׺
* ���������widget���б༭�ؼ���preTexy��ǰ׺�ַ����׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ڿؼ���ý��������¸����ַ���ʾ�����ڴ��������
*********************************************************************************************************/
void SetLineEditPreText(StructLineEditWidget* widget, char* preTexy)
{
  u32 i;
  i = 0;
  while((0 != preTexy[i]) && (i < widget->editNum))
  {
    widget->preText[i] = preTexy[i];
    i++;
  }
  widget->preText[i] = 0;

  //�����ػ�
  DrawBackground(widget);

  //�ַ��ػ�
  DrawText(widget);
}

/*********************************************************************************************************
* �������ƣ�SetLineEditMaxLen
* �������ܣ������б༭�ַ�������
* ���������widget���б༭�ؼ���len���ַ������ȣ�����ǰ׺�ͱ༭������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ڿؼ���ý��������¸����ַ���ʾ�����ڴ��������
*********************************************************************************************************/
void SetLineEditMaxLen(StructLineEditWidget* widget, u32 len)
{
  u32 maxShowLen;
  maxShowLen = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);
  if(len <= maxShowLen)
  {
    widget->editNum = len;
  }
}

/*********************************************************************************************************
* �������ƣ�DeleteLineEditWidget
* �������ܣ�ɾ���б༭�ؼ�
* ���������widget���б༭�ؼ�
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteLineEditWidget(StructLineEditWidget* widget)
{
  //��ر���
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
