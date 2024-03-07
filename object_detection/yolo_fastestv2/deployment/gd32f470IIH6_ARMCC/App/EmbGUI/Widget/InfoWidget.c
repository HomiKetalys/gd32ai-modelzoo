/*********************************************************************************************************
* ģ�����ƣ�InfoWidget.c
* ժ    Ҫ����Ϣ���ڿؼ�
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
#include "InfoWidget.h"

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
/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitInfoWidgetStruct
* �������ܣ����ø������Ĭ��ֵ
* ���������widget���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitInfoWidgetStruct(StructInfoWidget* widget)
{
  widget->x0 = 0;
  widget->y0 = 0;
  widget->width = 0;
  widget->height = 0;
  widget->topOff = 0;
  widget->bottomOff = 0;
  widget->leftOff = 0;
  widget->rightOff = 0;
  widget->textFont = GUI_FONT_ASCII_24;
  widget->textColor = GUI_COLOR_BLACK;
  widget->textHeight = 30;
  widget->background = NULL;
  widget->image = NULL;
  widget->imageType = GUI_IMAGE_TYPE_BMP;
  widget->defaultBackColor = GUI_COLOR_WHITE;
  widget->isExist = 0;
  widget->isShow = 0;
}

/*********************************************************************************************************
* �������ƣ�CreateInfoWidget
* �������ܣ������ؼ�
* ���������widget���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CreateInfoWidget(StructInfoWidget* widget)
{
  //���汳��
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //������ʾ�����С
  widget->showX0 = widget->x0 + widget->leftOff;
  widget->showY0 = widget->y0 + widget->topOff;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->rightOff;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->bottomOff;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;

  //���������ʾ
  widget->isExist = 1;

  //���δ����ʾ����
  widget->isShow = 0;
}

/*********************************************************************************************************
* �������ƣ�DeleteInfoWidget
* �������ܣ�ɾ���ؼ�
* ���������widget���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeleteInfoWidget(StructInfoWidget* widget)
{
  //��ԭ����
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //��Ǵ��ڷǻ״̬
  widget->isExist = 0;

  //���δ����ʾ����
  widget->isShow = 0;
}

/*********************************************************************************************************
* �������ƣ�SetInfoWidgetShow
* �������ܣ�������Ϣ����ʾ����
* ���������widget���ؼ��׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void SetInfoWidgetShow(StructInfoWidget* widget, char* text)
{
  u32 x, y, x0, y0, x1, y1;
  u32 stringLen, lineLen, lineNum, stringLineNum, stringCnt, stringRemain, lineCnt, code;
  u32 i, j;

  //�ǻ״̬�²���ʾ
  if(0 == widget->isExist)
  {
    return;
  }

  //���������ʾ��Ϣ
  widget->isShow = 1;

  //��ԭ����
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //��ʾ����ͼƬ
  if(NULL != widget->image)
  {
    GUIDrawImage(widget->x0, widget->y0, (u32)widget->image, widget->imageType);
  }
  else
  {
    x1 = widget->x0 + widget->width - 1;
    y1 = widget->y0 + widget->height - 1;
    GUIFillColor(widget->x0, widget->y0, x1, y1, widget->defaultBackColor);
  }

  //ͳ���ַ�������
  stringLen = 0;
  i = 0;
  while(0 != text[i++])
  {
    stringLen++;
  }

  //����һ������ʾ���ַ���
  lineLen = widget->showWidth / GetFontWidth(widget->textFont);

  //��������ʾ��������
  lineNum = widget->showHeight / widget->textHeight;

  //�����ַ�����ռ����
  stringLineNum = stringLen / lineLen;
  if(0 != (stringLen % lineLen))
  {
    stringLineNum = stringLineNum + 1;
  }
  if(stringLineNum > lineNum)
  {
    stringLineNum = lineNum;
  }

  //������ʾ��ʼ����
  y0 = widget->showY0 + (widget->showHeight - widget->textHeight * stringLineNum) / 2;
  x0 = widget->showX0 + (widget->showWidth - lineLen * GetFontWidth(widget->textFont)) / 2;

  //ѭ����ʾ�ַ�
  stringCnt = 0;
  x = x0;
  y = y0;
  stringRemain = stringLen;
  for(i = 0; i < lineNum; i++)
  {
    //������������
    if(stringRemain >= lineLen)
    {
      x = x0;
    }
    else
    {
      x = widget->showX0 + (widget->showWidth - stringRemain * GetFontWidth(widget->textFont)) / 2;
    }

    //��ʾһ���ַ���
    lineCnt = 0;
    for(j = 0; j < lineLen; j++)
    {
      //�����ַ�δ��ʾ
      if(0 != text[stringCnt])
      {
        //Ӣ���ַ�
        if((u8)text[stringCnt] < 0x80)
        {
          if(lineCnt < lineLen)
          {
            code = text[stringCnt];
            GUIDrawChar(x, y, code, widget->textFont, NULL, widget->textColor, 1);
            x = x + GetFontWidth(widget->textFont);
            stringCnt = stringCnt + 1;
            lineCnt = lineCnt + 1;
            stringRemain = stringRemain - 1;
          }
          else
          {
            break;
          }
        }

        //�����ַ�
        else
        {
          if(lineCnt < (lineLen - 1))
          {
            code = 0;
            code = (text[stringCnt] << 8) | text[stringCnt + 1];
            GUIDrawChar(x, y, code, widget->textFont, NULL, widget->textColor, 1);
            x = x + 2 * GetFontWidth(widget->textFont);
            stringCnt = stringCnt + 2;
            lineCnt = lineCnt + 2;
            stringRemain = stringRemain - 2;
          }
          else
          {
            break;
          }
        }
      }

      //��ʾ���
      else
      {
        return;
      }
    }

    //���º�����
    y = y + widget->textHeight;
  }
}

/*********************************************************************************************************
* �������ƣ�ScanInfoWidget
* �������ܣ��ؼ�ɨ��
* ���������widget���ؼ��׵�ַ
* ���������void
* �� �� ֵ��0-δ����������1-�ֱ�������
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 ScanInfoWidget(StructInfoWidget* widget)
{
  u32 tx, ty;         //����������Ϣ
  u32 x0, y0, x1, y1; //������㡢�յ�����λ��
  u32 i;              //ѭ������

  //��������յ�����
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width - 1;
  y1 = widget->y0 + widget->height - 1;

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
* �������ƣ�SetInfoWidgetOff
* �������ܣ����ÿؼ���ʾ�����С
* ���������widget���ؼ��׵�ַ��off��ƫ����
* ���������void
* �� �� ֵ��0-δ����������1-�ֱ�������
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void SetInfoWidgetOff(StructInfoWidget* widget, u32 off)
{
  //����ƫ����
  widget->topOff = off;
  widget->bottomOff = off;
  widget->leftOff = off;
  widget->rightOff = off;
  
  //������ʾ�����С
  widget->showX0 = widget->x0 + widget->leftOff;
  widget->showY0 = widget->y0 + widget->topOff;
  widget->showX1 = widget->x0 + widget->width - 1 - widget->rightOff;
  widget->showY1 = widget->y0 + widget->height - 1 - widget->bottomOff;
  widget->showWidth = widget->showX1 - widget->showX0 + 1;
  widget->showHeight = widget->showY1 - widget->showY0 + 1;
}
