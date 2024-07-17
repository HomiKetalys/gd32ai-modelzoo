/*********************************************************************************************************
* 模块名称：GUIPlatform.c
* 摘    要：GUI平台配置模块
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
#include "GUIPlatform.h"
#include "TLILCD.h"
#include "Touch.h"
#include "BMP.h"
#include "JPEG.h"
#include "Timer.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：GUIDrawPoint
* 函数功能：画点函数
* 输入参数：x,y：点坐标，size：半径，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
  LCDDrawPoint(x, y, color); //圆心坐标
}

/*********************************************************************************************************
* 函数名称：GUIReadPoint
* 函数功能：读点函数
* 输入参数：x,y：点坐标
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 GUIReadPoint(u32 x, u32 y)
{
  return LCDReadPoint(x, y);
}

/*********************************************************************************************************
* 函数名称：GUIDrawPoint
* 函数功能：画线函数
* 输入参数：x0,y0：起点坐标，x1,y1：终点坐标，size：线宽，color：颜色，type：线条类型
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、线条类型为GUI_LINE_SQUARE时绘制速度块，为GUI_LINE_CIRCLE速度较慢。线宽越大，速度越慢
*          2、线条类型为GUI_LINE_CIRCLE时，线宽只对2的整数倍有效，即1、2、4、8等，不满足时（如3、5、7）则向下取整
*********************************************************************************************************/
void GUIDrawLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 size, u32 color, EnumGUILineType type)
{
  //两端是方形变量
  u32 xAbs, yAbs;                     //横坐标、纵坐标起点终点绝对值
  u32 lineX0, lineY0, lineX1, lineY1; //画线时起点终点

  //两端是圆形变量
  u16 t; 
  int xerr = 0, yerr = 0, deltaX, deltaY, distance; 
  int incx, incy, uRow, uCol;

  //宽度为1
  if(1 == size)
  {
    LCDDrawLine(x0, y0, x1, y1, color);
    return;
  }

  //两端是方形，通过线条上下或左右平移得到一条粗线
  if(GUI_LINE_SQUARE == type)
  {
    //计算横坐标偏移量
    if(x1 > x0)
    {
      xAbs = x1 - x0;
    }
    else
    {
      xAbs = x0 - x1;
    }

    //计算纵坐标偏移量
    if(y1 > y0)
    {
      yAbs = y1 - y0;
    }
    else
    {
      yAbs = y0 - y1;
    }

    //设置起点、终点坐标
    lineX0 = x0;
    lineX1 = x1;
    lineY0 = y0;
    lineY1 = y1;

    //横坐标差值比纵坐标大，则将直线上下平移得到一条粗线
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

    //纵坐标差值比横坐标大，则将直线左右平移得到一条粗线
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

  //两端是圆形，通过画粗点得到一条粗线
  else if(GUI_LINE_CIRCLE == type)
  {
    size = size / 2;
    deltaX = x1 - x0;
    deltaY = y1 - y0;
    uRow = x0; 
    uCol = y0; 

    //设置单步方向 
    if(deltaX > 0)
    {
      incx = 1;
    }

    //垂直线
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

    //水平线
    else if(deltaY == 0)
    {
      incy = 0;
    }
    else
    {
      incy = -1;
      deltaY = -deltaY;
    } 

    //选取基本增量坐标轴
    if(deltaX > deltaY)
    {
      distance = deltaX;
    }
    else 
    {
      distance = deltaY;
    }

    //画线输出
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
* 函数名称：GUIFillColor
* 函数功能：单色填充
* 输入参数：x0,y0：起点坐标，x1,y1：终点坐标，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIFillColor(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  LCDFillPixel(x0, y0, x1, y1, color);
}

/*********************************************************************************************************
* 函数名称：GUIDrawChar
* 函数功能：显示字符
* 输入参数：x,y：坐标，code：字符编码，font：字体，backColor：背景颜色，textColor：字符颜色，mode：1-透明，0-不透明
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
  
  //字符显示
  LCDShowChar(x, y, charFont, charMode, textColor, backColor, code);
}

/*********************************************************************************************************
* 函数名称：GUIDrawTextLine
* 函数功能：显示一行字符串
* 输入参数：x,y：坐标，string：字符串首地址，font：字体，backColor：背景颜色，textColor：字符颜色，mode：1-透明，0-不透明，len：显示长度
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIDrawTextLine(u32 x, u32 y, u32 string, EnumGUIFont font, u32 backColor,u32 textColor, u32 mode, u32 len)
{
  //字符串首地址
  u8* text;

  //字符宽度和高度
  u32 width;

  //汉字编码
  u16 code;

  //获取字符宽度
  width = GetFontWidth(font);

  //绘制字符串
  text = (u8*)string;
  while(len)
  {
    //ASCII码
    if((*text <= '~') && (*text >= ' '))
    {
      GUIDrawChar(x, y, *text, font, backColor, textColor, mode);
      text = text + 1;
      x = x + width;
      len = len - 1;
    }

    //汉字
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
* 函数名称：GUISaveBackground
* 函数功能：保存背景
* 输入参数：x0,y0：背景原点，width：宽度，height：高度，huf：背景缓冲区首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、默认控件背景储存在SRAM
*          2、可以选择将背景储存到内部Flash或其它外部存储设备，以减少内部SRAM消耗
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

  //背景指向的地址为空，直接返回
  if(NULL == buf)
  {
    return;
  }

  //RGB565和RGB555
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

  //RGB888和BGR888
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
* 函数名称：GUIDrawBackground
* 函数功能：绘制背景
* 输入参数：x0,y0：背景原点，width：宽度，height：高度，huf：背景缓冲区首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、默认控件背景储存在SRAM
*          2、可以选择将背景储存到内部Flash或其它外部存储设备，以减少内部SRAM消耗
*********************************************************************************************************/
void GUIDrawBackground(u32 x0, u32 y0, u32 width, u32 height, u32 buf)
{
  //填充
  LCDColorFill(x0, y0, width, height, (u16*)buf);
}

/*********************************************************************************************************
* 函数名称：GUIDrawImage
* 函数功能：绘制图片
* 输入参数：x0,y0：图片原点，image：图片设备结构体首地址或图片首地址，type：图片格式
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIDrawImage(u32 x0, u32 y0, u32 image, EnumGUIImageType type)
{
  //位图
  if(GUI_IMAGE_TYPE_BMP == type)
  {
    DisplayBMP((StructBmpImage*)image, x0, y0);
  }

  //JPEG图片
  else if(GUI_IMAGE_TYPE_JPEG == type)
  {
    DisplayJPEGInFlash((StructJpegImage*)image, x0, y0);
  }
}

/*********************************************************************************************************
* 函数名称：GUIGetSysTime
* 函数功能：系统时间
* 输入参数：void
* 输出参数：void
* 返 回 值：系统运行时间（ms）
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u64 GUIGetSysTime(void)
{
  return GetSysTime();
}

/*********************************************************************************************************
* 函数名称：GUIGetTouch
* 函数功能：获取触屏触点
* 输入参数：x,y：用于返回x、y坐标，cnt：第几个触点,0-n
* 输出参数：void
* 返 回 值：0-无触点按下，1-有触点按下
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u8 GUIGetTouch(u32* x, u32* y, u32 cnt)
{
  StructTouchDev* touchDev;

  //判断触点范围是否超过最大值
  if(cnt >= POINT_NUM_MAX)
  {
    *x = 0xFFFFFFFF;
    *y = 0xFFFFFFFF;
    return 0;
  }

  // //触屏扫描
  // ScanTouch();

  //获取触摸屏扫描结果
  touchDev = GetTouchDev();

  //判断该触点是否按下
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
* 函数名称：GetFontWidth
* 函数功能：获取字符宽度
* 输入参数：void
* 输出参数：void
* 返 回 值：字符宽度（像素点）
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：GetFontHeight
* 函数功能：获取字符高度
* 输入参数：void
* 输出参数：void
* 返 回 值：字符宽度（像素点）
* 创建日期：2021年07月01日
* 注    意：
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
