/*********************************************************************************************************
* ģ�����ƣ�TerminalWidget.c
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

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "TerminalWidget.h"

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
static void DrawBackground(StructTerminalWidget* widget);   //���Ʊ���
static void UpdateStringShow(StructTerminalWidget* widget); //�����ַ���ʾ

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DrawBackground
* �������ܣ����Ʊ���
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawBackground(StructTerminalWidget* widget)
{
  u32 x0, y0, x1, y1;

  //���Ʊ���ͼƬ
  if(NULL != widget->backImage)
  {
    GUIDrawImage(widget->x0, widget->y0, (u32)widget->backImage, widget->imageType);
  }

  //ʹ�ô�ɫ����
  else
  {
    x0 = widget->x0;
    y0 = widget->y0;
    x1 = widget->x0 + widget->width - 1;
    y1 = widget->y0 + widget->height - 1;
    GUIFillColor(x0, y0, x1, y1, widget->defaultBackColor);
  }
}

/*********************************************************************************************************
* �������ƣ�UpdateStringShow
* �������ܣ������ַ���ʾ
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void UpdateStringShow(StructTerminalWidget* widget)
{
  u32 i, head, stringNum;
  u32 x0, y0, x1, y1;

  head = widget->stringHead;
  stringNum = widget->stringShowNum;

  //������¼Ϊ�㣬ֱ�ӷ���
  if(0 == stringNum)
  {
    return;
  }

  //��������LINE_NUM�У���ӻ�����0�п�ʼ��ʾ
  else if(stringNum < widget->maxLineNum)
  {
    for(i = 0; i <= stringNum; i++)
    {
      //�屳��
      x0 = widget->showX0;
      y0 = widget->showY0 + widget->textHeight * i;
      x1 = widget->showX1;
      y1 = y0 + widget->textHeight - 1;
      GUIFillColor(x0, y0, x1, y1, widget->textBackColor);

      //��ʾ�ַ���
      GUIDrawTextLine(x0, y0, (u32)(widget->stringBuf + (widget->lineCharNum + 1) * i), 
      widget->textFont, NULL, widget->textColor, 1, widget->lineCharNum);
    }
  }

  //������������Ҫ���Ǵ����п�ʼ��ʾ
  else
  {
    for(i = 0; i < widget->maxLineNum; i++)
    {
      //�屳��
      x0 = widget->showX0;
      y0 = widget->showY0 + widget->textHeight * i;
      x1 = widget->showX1;
      y1 = y0 + widget->textHeight - 1;
      GUIFillColor(x0, y0, x1, y1, widget->textBackColor);

      //��ʾ�ַ���
      GUIDrawTextLine(x0, y0, (u32)(widget->stringBuf + (widget->lineCharNum + 1) * head), 
      widget->textFont, NULL, widget->textColor, 1, widget->lineCharNum);
      head = (head + 1) % widget->maxLineNum;
    }
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitTerminalWidgetStruct
* �������ܣ���ʼ���ն��豸�ṹ��
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitTerminalWidgetStruct(StructTerminalWidget* widget)
{
  u32 i;
  
  widget->x0 = 0;
  widget->y0 = 0;
  widget->width = GUI_X_PIXEL_SIZE;
  widget->height = GUI_Y_PIXEL_SIZE;
  widget->topOff = 0;
  widget->bottomOff = 0;
  widget->showSpace = 0;
  widget->textColor = GUI_COLOR_WHITE;
  widget->textFont = GUI_FONT_ASCII_24;
  widget->textHeight = 30;
  widget->background = NULL;
  widget->backImage = NULL;
  widget->imageType = GUI_IMAGE_TYPE_JPEG;
  widget->defaultBackColor = GUI_COLOR_BLACK;
  widget->hasCreate = 0;
  widget->textBackColor = widget->defaultBackColor;
  widget->showX0 = widget->x0 + widget->showSpace;
  widget->showY0 = widget->y0 + widget->showSpace;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->showSpace;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->showSpace;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;
  widget->lineCharNum = widget->showWidth / GetFontWidth(widget->textFont);
  widget->maxLineNum = widget->showHeight / widget->textHeight;
  widget->stringHead = 0;
  widget->stringShowNum = 0;

  for(i = 0; i < TERMINAL_MAX_CHAR_NUM; i++)
  {
    widget->stringBuf[i] = 0;
  }
}

/*********************************************************************************************************
* �������ƣ�CreateTerminalWidget
* �������ܣ������ն˿ؼ�
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateTerminalWidget(StructTerminalWidget* widget)
{
  u32 i;

  //����Ѵ���
  widget->hasCreate = 1;

  //����ؼ�����
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //�����ն˱���
  DrawBackground(widget);

  //��ȡ�ն����屳����ɫ
  widget->textBackColor = GUIReadPoint(widget->x0 + widget->width / 2, widget->y0 + widget->height / 2);

  //�����Զ�����
  widget->showX0 = widget->x0 + widget->showSpace;
  widget->showY0 = widget->y0 + widget->showSpace + widget->topOff;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->showSpace;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->showSpace - widget->bottomOff;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;
  widget->lineCharNum = widget->showWidth / GetFontWidth(widget->textFont);
  widget->maxLineNum = widget->showHeight / widget->textHeight;
  widget->stringHead = 0;
  widget->stringShowNum = 0;

  //����ַ���
  for(i = 0; i < TERMINAL_MAX_CHAR_NUM; i++)
  {
    widget->stringBuf[i] = 0;
  }
}

/*********************************************************************************************************
* �������ƣ�TerminalWidgetClear
* �������ܣ�����ն���ʾ
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void TerminalWidgetClear(StructTerminalWidget* widget)
{
  u32 i;

  for(i = 0; i < TERMINAL_MAX_CHAR_NUM; i++)
  {
    widget->stringBuf[i] = 0;
  }

  //�屳��
  GUIFillColor(widget->showX0, widget->showY0, widget->showX1, widget->showY1, widget->textBackColor);

  //��ձ��
  widget->stringHead = 0;
  widget->stringShowNum = 0;
}

/*********************************************************************************************************
* �������ƣ�TerminalWidgetShowLine
* �������ܣ��ն�����ʾһ���ַ���
* ���������widget���ؼ��豸�ṹ���ַ��string���ַ����׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void TerminalWidgetShowLine(StructTerminalWidget* widget, char* string)
{
  u32 i;

  //�����ַ�����������
  i = 0;
  while((*(string + i) <= '~') && (*(string + i) >= ' ') && (i < widget->lineCharNum))
  {
    widget->stringBuf[widget->stringHead * (widget->lineCharNum + 1) + i] = *(string + i);
    i++;
  }
  widget->stringBuf[widget->stringHead * (widget->lineCharNum + 1) + i] = 0;

  //�������в���λ��
  widget->stringHead = (widget->stringHead + 1) % widget->maxLineNum;

  //��¼����
  if(widget->stringShowNum < widget->maxLineNum)
  {
    widget->stringShowNum = widget->stringShowNum + 1;
  }
  else
  {
    widget->stringShowNum = widget->maxLineNum;
  }

  //�����ն���ʾ
  UpdateStringShow(widget);
}

/*********************************************************************************************************
* �������ƣ�DeleteTerminalWidget
* �������ܣ�ɾ���ն˿ؼ�
* ���������widget���ؼ��豸�ṹ���ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteTerminalWidget(StructTerminalWidget* widget)
{
  //��ԭ����
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
