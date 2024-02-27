/*********************************************************************************************************
* ģ�����ƣ�GUIPlatform.c
* ժ    Ҫ��GUIƽ̨����ģ��
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
#include "GUIPlatform.h"
#include "TLILCD.h"
#include "Touch.h"
#include "BMP.h"
#include "JPEG.h"
#include "Timer.h"

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

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�GUIDrawPoint
* �������ܣ����㺯��
* ���������x,y�������꣬size���뾶��color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIDrawPoint(u32 x, u32 y, u32 size, u32 color)
{
  int a, b;
  int di;
  int i, p;
  if(0 == size){return;}
  a = 0; b = size;
  di = 3 - (size << 1);
  while(a <= b)
  {
    i = a, p = b;
    while(i > 0)
    {
      LCDDrawPoint(x + b,y - i, color);
      LCDDrawPoint(x - i,y + b, color);
      i--;
    }
    while( p > 0)
    {
      LCDDrawPoint(x - a, y - p, color);
      LCDDrawPoint(x - p, y - a, color);
      LCDDrawPoint(x + a, y - p, color);
      LCDDrawPoint(x - p, y + a, color);
      LCDDrawPoint(x + a, y + p, color);
      LCDDrawPoint(x + p, y + a, color);
      p--;
    }
    a++;

    //Bresenham�㷨��Բ
    if(di < 0)
    {
      di += 4 * a + 6;
    }
    else
    {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
  LCDDrawPoint(x, y, color); //Բ������
}

/*********************************************************************************************************
* �������ƣ�GUIReadPoint
* �������ܣ����㺯��
* ���������x,y��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 GUIReadPoint(u32 x, u32 y)
{
  return LCDReadPoint(x, y);
}

/*********************************************************************************************************
* �������ƣ�GUIDrawPoint
* �������ܣ����ߺ���
* ���������x0,y0��������꣬x1,y1���յ����꣬size���߿�color����ɫ��type����������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1����������ΪGUI_LINE_SQUAREʱ�����ٶȿ飬ΪGUI_LINE_CIRCLE�ٶȽ������߿�Խ���ٶ�Խ��
*          2����������ΪGUI_LINE_CIRCLEʱ���߿�ֻ��2����������Ч����1��2��4��8�ȣ�������ʱ����3��5��7��������ȡ��
*********************************************************************************************************/
void GUIDrawLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 size, u32 color, EnumGUILineType type)
{
  //�����Ƿ��α���
  u32 xAbs, yAbs;                     //�����ꡢ����������յ����ֵ
  u32 lineX0, lineY0, lineX1, lineY1; //����ʱ����յ�

  //������Բ�α���
  u16 t; 
  int xerr = 0, yerr = 0, deltaX, deltaY, distance; 
  int incx, incy, uRow, uCol;

  //���Ϊ1
  if(1 == size)
  {
    LCDDrawLine(x0, y0, x1, y1, color);
    return;
  }

  //�����Ƿ��Σ�ͨ���������»�����ƽ�Ƶõ�һ������
  if(GUI_LINE_SQUARE == type)
  {
    //���������ƫ����
    if(x1 > x0)
    {
      xAbs = x1 - x0;
    }
    else
    {
      xAbs = x0 - x1;
    }

    //����������ƫ����
    if(y1 > y0)
    {
      yAbs = y1 - y0;
    }
    else
    {
      yAbs = y0 - y1;
    }

    //������㡢�յ�����
    lineX0 = x0;
    lineX1 = x1;
    lineY0 = y0;
    lineY1 = y1;

    //�������ֵ�����������ֱ������ƽ�Ƶõ�һ������
    if(xAbs >= yAbs)
    {
      size = size + (size * yAbs / xAbs) / 2;
      lineY0 = lineY0 - size / 2;
      lineY1 = lineY1 - size / 2;
      while(size > 0)
      {
        LCDDrawLine(lineX0, lineY0, lineX1, lineY1, color);
        lineY0++;
        lineY1++;
        size--;
      }
    }

    //�������ֵ�Ⱥ��������ֱ������ƽ�Ƶõ�һ������
    else
    {
      size = size + (size * xAbs / yAbs) / 2;
      lineX0 = lineX0 - size / 2;
      lineX1 = lineX1 - size / 2;
      while(size > 0)
      {
        LCDDrawLine(lineX0, lineY0, lineX1, lineY1, color);
        lineX0++;
        lineX1++;
        size--;
      }
    }
  }

  //������Բ�Σ�ͨ�����ֵ�õ�һ������
  else if(GUI_LINE_CIRCLE == type)
  {
    size = size / 2;
    deltaX = x1 - x0;
    deltaY = y1 - y0;
    uRow = x0; 
    uCol = y0; 

    //���õ������� 
    if(deltaX > 0)
    {
      incx = 1;
    }

    //��ֱ��
    else if(deltaX == 0)
    {
      incx = 0;
    }
    else
    {
      incx = -1;
      deltaX = -deltaX;
    } 

    if(deltaY > 0)
    {
      incy = 1;
    }

    //ˮƽ��
    else if(deltaY == 0)
    {
      incy = 0;
    }
    else
    {
      incy = -1;
      deltaY = -deltaY;
    } 

    //ѡȡ��������������
    if(deltaX > deltaY)
    {
      distance = deltaX;
    }
    else 
    {
      distance = deltaY;
    }

    //�������
    for(t = 0; t <= distance + 1; t++ )
    {
      GUIDrawPoint(uRow, uCol, size, color);
      xerr += deltaX;
      yerr += deltaY;
      if(xerr > distance)
      { 
        xerr -= distance;
        uRow += incx;
      } 
      if(yerr > distance)
      { 
        yerr -= distance;
        uCol += incy;
      }
    }
  }
}

/*********************************************************************************************************
* �������ƣ�GUIFillColor
* �������ܣ���ɫ���
* ���������x0,y0��������꣬x1,y1���յ����꣬color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIFillColor(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  LCDFillPixel(x0, y0, x1, y1, color);
}

/*********************************************************************************************************
* �������ƣ�GUIDrawChar
* �������ܣ���ʾ�ַ�
* ���������x,y�����꣬code���ַ����룬font�����壬backColor��������ɫ��textColor���ַ���ɫ��mode��1-͸����0-��͸��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIDrawChar(u32 x, u32 y, u32 code, EnumGUIFont font, u32 backColor,u32 textColor, u32 mode)
{
  EnumTLILCDFont charFont;
  EnumTLILCDTextMode charMode;
  
  if(GUI_FONT_ASCII_12 == font)
  {
    charFont = LCD_FONT_12;
  }
  else if(GUI_FONT_ASCII_16 == font)
  {
    charFont = LCD_FONT_16;
  }
  else if(GUI_FONT_ASCII_24 == font)
  {
    charFont = LCD_FONT_24;
  }

  if(0 == mode)
  {
    charMode = LCD_TEXT_NORMAL;
  }
  else
  {
    charMode = LCD_TEXT_TRANS;
  }
  
  //�ַ���ʾ
  LCDShowChar(x, y, charFont, charMode, textColor, backColor, code);
}

/*********************************************************************************************************
* �������ƣ�GUIDrawTextLine
* �������ܣ���ʾһ���ַ���
* ���������x,y�����꣬string���ַ����׵�ַ��font�����壬backColor��������ɫ��textColor���ַ���ɫ��mode��1-͸����0-��͸����len����ʾ����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIDrawTextLine(u32 x, u32 y, u32 string, EnumGUIFont font, u32 backColor,u32 textColor, u32 mode, u32 len)
{
  //�ַ����׵�ַ
  u8* text;

  //�ַ���Ⱥ͸߶�
  u32 width;

  //���ֱ���
  u16 code;

  //��ȡ�ַ����
  width = GetFontWidth(font);

  //�����ַ���
  text = (u8*)string;
  while(len)
  {
    //ASCII��
    if((*text <= '~') && (*text >= ' '))
    {
      GUIDrawChar(x, y, *text, font, backColor, textColor, mode);
      text = text + 1;
      x = x + width;
      len = len - 1;
    }

    //����
    else if(*text >= 0x81)
    {
      code = (*(text) << 8) | *(text + 1);
      GUIDrawChar(x, y, code, font, backColor, textColor, mode);
      text = text + 2;
      x = x + width * 2;
      if(len >= 2)
      {
        len = len - 2;
      }
      else
      {
        len = 0;
      }
    }
    else
    {
      break;
    }
  }
}

/*********************************************************************************************************
* �������ƣ�GUISaveBackground
* �������ܣ����汳��
* ���������x0,y0������ԭ�㣬width����ȣ�height���߶ȣ�huf�������������׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��Ĭ�Ͽؼ�����������SRAM
*          2������ѡ�񽫱������浽�ڲ�Flash�������ⲿ�洢�豸���Լ����ڲ�SRAM����
*********************************************************************************************************/
void GUISaveBackground(u32 x0, u32 y0, u32 width, u32 height, u32 buf)
{
  u32 x, y, i;
  
#if ((GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565) || (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB555))
  u16* background;
#endif

#if ((GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888) || (GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888))
  u32* background;
#endif

  //����ָ��ĵ�ַΪ�գ�ֱ�ӷ���
  if(NULL == buf)
  {
    return;
  }

  //RGB565��RGB555
#if ((GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB565) || (GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB555))
  background = (u16*)buf;
  i = 0;
  for(y = y0; y < (y0 + height); y++)
  {
    for(x = x0; x < (x0 + width); x++)
    {
      background[i] = (u16)GUIReadPoint(x, y);
      i++;
    }
  }
#endif

  //RGB888��BGR888
#if ((GUI_COLOR_TYPE == GUI_COLOR_TYPE_RGB888) || (GUI_COLOR_TYPE == GUI_COLOR_TYPE_BGR888))
  background = (u32*)buf;
  i = 0;
  for(y = y0; y < (y0 + height); y++)
  {
    for(x = x0; x < (x0 + width); x++)
    {
      background[i] = GUIReadPoint(x, y);
      i++;
    }
  }
#endif
}

/*********************************************************************************************************
* �������ƣ�GUIDrawBackground
* �������ܣ����Ʊ���
* ���������x0,y0������ԭ�㣬width����ȣ�height���߶ȣ�huf�������������׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��Ĭ�Ͽؼ�����������SRAM
*          2������ѡ�񽫱������浽�ڲ�Flash�������ⲿ�洢�豸���Լ����ڲ�SRAM����
*********************************************************************************************************/
void GUIDrawBackground(u32 x0, u32 y0, u32 width, u32 height, u32 buf)
{
  //���
  LCDColorFill(x0, y0, width, height, (u16*)buf);
}

/*********************************************************************************************************
* �������ƣ�GUIDrawImage
* �������ܣ�����ͼƬ
* ���������x0,y0��ͼƬԭ�㣬image��ͼƬ�豸�ṹ���׵�ַ��ͼƬ�׵�ַ��type��ͼƬ��ʽ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GUIDrawImage(u32 x0, u32 y0, u32 image, EnumGUIImageType type)
{
  //λͼ
  if(GUI_IMAGE_TYPE_BMP == type)
  {
    DisplayBMP((StructBmpImage*)image, x0, y0);
  }

  //JPEGͼƬ
  else if(GUI_IMAGE_TYPE_JPEG == type)
  {
    DisplayJPEGInFlash((StructJpegImage*)image, x0, y0);
  }
}

/*********************************************************************************************************
* �������ƣ�GUIGetSysTime
* �������ܣ�ϵͳʱ��
* ���������void
* ���������void
* �� �� ֵ��ϵͳ����ʱ�䣨ms��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u64 GUIGetSysTime(void)
{
  return GetSysTime();
}

/*********************************************************************************************************
* �������ƣ�GUIGetTouch
* �������ܣ���ȡ��������
* ���������x,y�����ڷ���x��y���꣬cnt���ڼ�������,0-n
* ���������void
* �� �� ֵ��0-�޴��㰴�£�1-�д��㰴��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u8 GUIGetTouch(u32* x, u32* y, u32 cnt)
{
  StructTouchDev* touchDev;

  //�жϴ��㷶Χ�Ƿ񳬹����ֵ
  if(cnt >= POINT_NUM_MAX)
  {
    *x = 0xFFFFFFFF;
    *y = 0xFFFFFFFF;
    return 0;
  }

  // //����ɨ��
  // ScanTouch();

  //��ȡ������ɨ����
  touchDev = GetTouchDev();

  //�жϸô����Ƿ���
  if(1 == touchDev->pointFlag[cnt])
  {
    *x = touchDev->point[cnt].x;
    *y = touchDev->point[cnt].y;
    return 1;
  }
  else
  {
    *x = 0xFFFFFFFF;
    *y = 0xFFFFFFFF;
    return 0;
  }
}

/*********************************************************************************************************
* �������ƣ�GetFontWidth
* �������ܣ���ȡ�ַ����
* ���������void
* ���������void
* �� �� ֵ���ַ���ȣ����ص㣩
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 GetFontWidth(EnumGUIFont font)
{
  if(GUI_FONT_ASCII_12 == font)
  {
    return 6;
  }
  else if(GUI_FONT_ASCII_16 == font)
  {
    return 8;
  }
  else if(GUI_FONT_ASCII_24 == font)
  {
    return 12;
  }
  
  return 0;
}

/*********************************************************************************************************
* �������ƣ�GetFontHeight
* �������ܣ���ȡ�ַ��߶�
* ���������void
* ���������void
* �� �� ֵ���ַ���ȣ����ص㣩
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 GetFontHeight(EnumGUIFont font)
{
  if(GUI_FONT_ASCII_12 == font)
  {
    return 12;
  }
  else if(GUI_FONT_ASCII_16 == font)
  {
    return 16;
  }
  else if(GUI_FONT_ASCII_24 == font)
  {
    return 24;
  }
  
  return 0;
}
