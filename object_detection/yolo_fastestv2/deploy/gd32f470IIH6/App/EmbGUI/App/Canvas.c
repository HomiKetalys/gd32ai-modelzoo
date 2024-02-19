/*********************************************************************************************************
* 模块名称：Canvas.c
* 摘    要：画布模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日
* 内    容：
* 注    意：     
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "Canvas.h"
#include "gd32f450x_conf.h"
#include "stdio.h"
#include "Touch.h"
#include "TLILCD.h"
#include "JPEG.h"
#include "TextWidget.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
//线条颜色
static const u16 s_arrLineColor[5] = {LCD_COLOR_YELLOW, LCD_COLOR_GREEN, LCD_COLOR_BLUE, LCD_COLOR_CYAN, LCD_COLOR_RED};
static StructTextWidget s_arrText[5];                                           //text控件，显示坐标信息
static StructTouchDev*  s_pTouchDev;                                            //触摸屏扫描结果

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void DrawPoint(u16 x0,u16 y0, u16 r, u16 color);                    //绘制实心圆
static void DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 size, u16 color); //绘制直线
static void DisplayBackground(void);                                       //绘制背景

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DrawPoint
* 函数功能：绘制实心圆
* 输入参数：（x0、y0）：圆心坐标，r：半径，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawPoint(u16 x0,u16 y0, u16 r, u16 color)
{
  int a, b;
  int di;
  a = 0; b = r;	  
  di = 3 - (r << 1);             //判断下个点位置的标志
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

    //Bresenham算法画圆
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
  LCDDrawPoint(x0, y0, color); //圆心坐标
}
/*********************************************************************************************************
* 函数名称：DrawLine
* 函数功能：绘制直线
* 输入参数：（x0、y0）：起点坐标，（x1、y1）：终点坐标，size：直线粗细，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：直线太粗会导致绘制缓慢
*********************************************************************************************************/
static void DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 size, u16 color)
{
  u16 t; 
  int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
  int incx, incy, uRow, uCol; 
  delta_x = x1 - x0; //计算坐标增量 
  delta_y = y1 - y0; 
  uRow = x0; 
  uCol = y0; 

  //设置单步方向 
  if(delta_x > 0)
  {
    incx=1;
  }

  //垂直线
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

  //水平线
  else if(delta_y == 0)
  {
    incy = 0;
  }
  else
  {
    incy = -1;
    delta_y =- delta_y;
  } 

  //选取基本增量坐标轴
  if( delta_x>delta_y)
  {
    distance = delta_x;
  }
  else 
  {
    distance = delta_y;
  }

  //画线输出
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
* 函数名称：DisplayBackground
* 函数功能：绘制背景
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：调用之前要确保LCD方向为横屏
*********************************************************************************************************/
static void DisplayBackground(void)
{
  //背景图片控制结构体
  StructJpegImage backgroundImage;

  //初始化backgroundImage
  backgroundImage.image = (unsigned char*)s_arrJpegBackgroundImage;
  backgroundImage.size  = sizeof(s_arrJpegBackgroundImage) / sizeof(unsigned char);

  //解码并显示图片
  DisplayJPEGInFlash(&backgroundImage, 0, 0);
}
/*********************************************************************************************************
* 函数名称：CreateText
* 函数功能：创建text控件用于显示点坐标
* 输入参数：void
* 输出参数：void
* 返 回 值：0-创建成功，其他-创建失败
* 创建日期：2021年07月01日
* 注    意：调用之前要确保LCD方向为横屏
*********************************************************************************************************/
static u8 CreateText(void)
{
  static const u16 s_arrTextX0[5]    = {65, 224, 384, 543, 703}; //text控件起始横坐标
  u16 i = 0; //循环变量

  for(i = 0; i < 5; i++)
  {
    //创建text
    s_arrText[i].x0     = s_arrTextX0[i]; //控件起始横坐标
    s_arrText[i].y0     = 52;             //控件起始纵坐标
    s_arrText[i].width  = 83;             //控件宽度
    s_arrText[i].height = 24;             //控件高度
    s_arrText[i].size   = 24;             //字体大小
    s_arrText[i].color  = LCD_COLOR_WHITE;          //字体颜色
    CreateTextWidget(&s_arrText[i]);      //创建text控件
  }
  
  return 0;
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitCanvas
* 函数功能：初始化画布
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitCanvas(void)
{
  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);
  LCDClear(LCD_COLOR_BLUE);

  //绘制背景
  DisplayBackground();

  //创建text控件
  CreateText();

  //获取触摸屏扫描设备结构体地址
  s_pTouchDev = GetTouchDev();
}

/*********************************************************************************************************
* 函数名称：CanvasTask
* 函数功能：画布任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CanvasTask(void)
{
  static char             s_arrString[20]   = {0};             //字符串转换缓冲区
  static u8               s_arrFirstFlag[5] = {1, 1, 1, 1, 1}; //标记线条是否已开始绘制
  static StructTouchPoint s_arrLastPoints[5];                  //上一个点的坐标
  
  u8  i;
  u16 x0, y0, x1, y1, size, color;

  //循环绘制5根线
  for(i = 0; i < 5; i++)
  {
    //有检测到按下
    if(1 == s_pTouchDev->pointFlag[i])
    {
      //字符串转换
      sprintf(s_arrString, "%d,%d", s_pTouchDev->point[i].x, s_pTouchDev->point[i].y);
      
      //更新到text显示
      s_arrText[i].setText(&s_arrText[i], s_arrString);

      //获得起点终点坐标、颜色和触点大小
      x0    = s_arrLastPoints[i].x;
      y0    = s_arrLastPoints[i].y;
      x1    = s_pTouchDev->point[i].x;
      y1    = s_pTouchDev->point[i].y;
      color = s_arrLineColor[i];
      size  = s_pTouchDev->point[i].size;

      //触点太大，需要缩小处理
      size = size / 5;
      if(0 == size)
      {
        size = 1;
      }
      else if(size > 15)
      {
        size = 15;
      }
      
      //线条第一个点用画点方式
      if(1 == s_arrFirstFlag[i])
      {
        //标记线条已经开始绘制
        s_arrFirstFlag[i] = 0;

        //画点
        if(y0 > (90 + size))
        {
          DrawPoint(x0, y0, size, color);
        }

        //越界
        else
        {
          //标记该线条未开始绘制
          s_arrFirstFlag[i] = 1;
        }
      }

      //后边的用画线方式
      else
      {
        //画线
        if((y0 > (90 + size)) && (y1 > (90 + size)))
        {
          DrawLine(x0, y0, x1, y1, size, color);
        }

        //越界
        else
        {
          //标记该线条未开始绘制
          s_arrFirstFlag[i] = 1;
        }
      }

      //保存当前位置，为画线做准备
      s_arrLastPoints[i].x = s_pTouchDev->point[i].x;
      s_arrLastPoints[i].y = s_pTouchDev->point[i].y;
    }
    else
    {
      //未检测到触摸点则清空显示
      s_arrText[i].setText(&s_arrText[i], "");

      //标记该线条未开始绘制
      s_arrFirstFlag[i] = 1;
    }
  }
}

