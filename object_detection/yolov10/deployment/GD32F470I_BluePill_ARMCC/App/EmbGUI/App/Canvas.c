/*********************************************************************************************************
* ģ�����ƣ�Canvas.c
* ժ    Ҫ������ģ��
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
#include "Canvas.h"
#include "gd32f450x_conf.h"
#include "stdio.h"
#include "Touch.h"
#include "TLILCD.h"
#include "JPEG.h"
#include "TextWidget.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ�嶨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
//������ɫ
static const u16 s_arrLineColor[5] = {LCD_COLOR_YELLOW, LCD_COLOR_GREEN, LCD_COLOR_BLUE, LCD_COLOR_CYAN, LCD_COLOR_RED};
static StructTextWidget s_arrText[5];                                           //text�ؼ�����ʾ������Ϣ
static StructTouchDev*  s_pTouchDev;                                            //������ɨ����

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void DrawPoint(u16 x0,u16 y0, u16 r, u16 color);                    //����ʵ��Բ
static void DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 size, u16 color); //����ֱ��
static void DisplayBackground(void);                                       //���Ʊ���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�DrawPoint
* �������ܣ�����ʵ��Բ
* �����������x0��y0����Բ�����꣬r���뾶��color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void DrawPoint(u16 x0,u16 y0, u16 r, u16 color)
{
  int a, b;
  int di;
  a = 0; b = r;	  
  di = 3 - (r << 1);             //�ж��¸���λ�õı�־
  while(a <= b)
  {
    int i = a, p = b;
    while(i > 0)
    {
      LCDDrawPoint(x0+b,y0-i, color);
      LCDDrawPoint(x0-i,y0+b, color);
      i--;
    }
    while( p > 0)
    {
      LCDDrawPoint(x0 - a, y0 - p, color);
      LCDDrawPoint(x0 - p, y0 - a, color);
      LCDDrawPoint(x0 + a, y0 - p, color);
      LCDDrawPoint(x0 - p, y0 + a, color);
      LCDDrawPoint(x0 + a, y0 + p, color);
      LCDDrawPoint(x0 + p, y0 + a, color);
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
  LCDDrawPoint(x0, y0, color); //Բ������
}
/*********************************************************************************************************
* �������ƣ�DrawLine
* �������ܣ�����ֱ��
* �����������x0��y0����������꣬��x1��y1�����յ����꣬size��ֱ�ߴ�ϸ��color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺ֱ��̫�ֻᵼ�»��ƻ���
*********************************************************************************************************/
static void DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 size, u16 color)
{
  u16 t; 
  int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
  int incx, incy, uRow, uCol; 
  delta_x = x1 - x0; //������������ 
  delta_y = y1 - y0; 
  uRow = x0; 
  uCol = y0; 

  //���õ������� 
  if(delta_x > 0)
  {
    incx=1;
  }

  //��ֱ��
  else if(delta_x==0)
  {
    incx=0;
  }
  else 
  {
    incx = -1;
    delta_x =- delta_x;
  } 

  if(delta_y > 0)
  {
    incy = 1;
  }

  //ˮƽ��
  else if(delta_y == 0)
  {
    incy = 0;
  }
  else
  {
    incy = -1;
    delta_y =- delta_y;
  } 

  //ѡȡ��������������
  if( delta_x>delta_y)
  {
    distance = delta_x;
  }
  else 
  {
    distance = delta_y;
  }

  //�������
  for(t = 0; t <= distance + 1; t++ )
  { 
    DrawPoint(uRow, uCol, size, color);
    xerr += delta_x ; 
    yerr += delta_y ; 
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
/*********************************************************************************************************
* �������ƣ�DisplayBackground
* �������ܣ����Ʊ���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺����֮ǰҪȷ��LCD����Ϊ����
*********************************************************************************************************/
static void DisplayBackground(void)
{
  //����ͼƬ���ƽṹ��
  StructJpegImage backgroundImage;

  //��ʼ��backgroundImage
  backgroundImage.image = (unsigned char*)s_arrJpegBackgroundImage;
  backgroundImage.size  = sizeof(s_arrJpegBackgroundImage) / sizeof(unsigned char);

  //���벢��ʾͼƬ
  DisplayJPEGInFlash(&backgroundImage, 0, 0);
}
/*********************************************************************************************************
* �������ƣ�CreateText
* �������ܣ�����text�ؼ�������ʾ������
* ���������void
* ���������void
* �� �� ֵ��0-�����ɹ�������-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺����֮ǰҪȷ��LCD����Ϊ����
*********************************************************************************************************/
static u8 CreateText(void)
{
  static const u16 s_arrTextX0[5]    = {65, 224, 384, 543, 703}; //text�ؼ���ʼ������
  u16 i = 0; //ѭ������

  for(i = 0; i < 5; i++)
  {
    //����text
    s_arrText[i].x0     = s_arrTextX0[i]; //�ؼ���ʼ������
    s_arrText[i].y0     = 52;             //�ؼ���ʼ������
    s_arrText[i].width  = 83;             //�ؼ����
    s_arrText[i].height = 24;             //�ؼ��߶�
    s_arrText[i].size   = 24;             //�����С
    s_arrText[i].color  = LCD_COLOR_WHITE;          //������ɫ
    CreateTextWidget(&s_arrText[i]);      //����text�ؼ�
  }
  
  return 0;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitCanvas
* �������ܣ���ʼ������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitCanvas(void)
{
  //LCD������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);
  LCDClear(LCD_COLOR_BLUE);

  //���Ʊ���
  DisplayBackground();

  //����text�ؼ�
  CreateText();

  //��ȡ������ɨ���豸�ṹ���ַ
  s_pTouchDev = GetTouchDev();
}

/*********************************************************************************************************
* �������ƣ�CanvasTask
* �������ܣ���������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void CanvasTask(void)
{
  static char             s_arrString[20]   = {0};             //�ַ���ת��������
  static u8               s_arrFirstFlag[5] = {1, 1, 1, 1, 1}; //��������Ƿ��ѿ�ʼ����
  static StructTouchPoint s_arrLastPoints[5];                  //��һ���������
  
  u8  i;
  u16 x0, y0, x1, y1, size, color;

  //ѭ������5����
  for(i = 0; i < 5; i++)
  {
    //�м�⵽����
    if(1 == s_pTouchDev->pointFlag[i])
    {
      //�ַ���ת��
      sprintf(s_arrString, "%d,%d", s_pTouchDev->point[i].x, s_pTouchDev->point[i].y);
      
      //���µ�text��ʾ
      s_arrText[i].setText(&s_arrText[i], s_arrString);

      //�������յ����ꡢ��ɫ�ʹ����С
      x0    = s_arrLastPoints[i].x;
      y0    = s_arrLastPoints[i].y;
      x1    = s_pTouchDev->point[i].x;
      y1    = s_pTouchDev->point[i].y;
      color = s_arrLineColor[i];
      size  = s_pTouchDev->point[i].size;

      //����̫����Ҫ��С����
      size = size / 5;
      if(0 == size)
      {
        size = 1;
      }
      else if(size > 15)
      {
        size = 15;
      }
      
      //������һ�����û��㷽ʽ
      if(1 == s_arrFirstFlag[i])
      {
        //��������Ѿ���ʼ����
        s_arrFirstFlag[i] = 0;

        //����
        if(y0 > (90 + size))
        {
          DrawPoint(x0, y0, size, color);
        }

        //Խ��
        else
        {
          //��Ǹ�����δ��ʼ����
          s_arrFirstFlag[i] = 1;
        }
      }

      //��ߵ��û��߷�ʽ
      else
      {
        //����
        if((y0 > (90 + size)) && (y1 > (90 + size)))
        {
          DrawLine(x0, y0, x1, y1, size, color);
        }

        //Խ��
        else
        {
          //��Ǹ�����δ��ʼ����
          s_arrFirstFlag[i] = 1;
        }
      }

      //���浱ǰλ�ã�Ϊ������׼��
      s_arrLastPoints[i].x = s_pTouchDev->point[i].x;
      s_arrLastPoints[i].y = s_pTouchDev->point[i].y;
    }
    else
    {
      //δ��⵽�������������ʾ
      s_arrText[i].setText(&s_arrText[i], "");

      //��Ǹ�����δ��ʼ����
      s_arrFirstFlag[i] = 1;
    }
  }
}

