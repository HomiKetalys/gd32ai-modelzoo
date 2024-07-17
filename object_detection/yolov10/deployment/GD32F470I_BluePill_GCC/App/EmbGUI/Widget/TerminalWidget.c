/*********************************************************************************************************
* 模块名称：TerminalWidget.c
* 摘    要：终端控件模块
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
#include "TerminalWidget.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void DrawBackground(StructTerminalWidget* widget);   //绘制背景
static void UpdateStringShow(StructTerminalWidget* widget); //更新字符显示

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DrawBackground
* 函数功能：绘制背景
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawBackground(StructTerminalWidget* widget)
{
  u32 x0, y0, x1, y1;

  //绘制背景图片
  if(NULL != widget->backImage)
  {
    GUIDrawImage(widget->x0, widget->y0, (u32)widget->backImage, widget->imageType);
  }

  //使用纯色背景
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
* 函数名称：UpdateStringShow
* 函数功能：更新字符显示
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void UpdateStringShow(StructTerminalWidget* widget)
{
  u32 i, head, stringNum;
  u32 x0, y0, x1, y1;

  head = widget->stringHead;
  stringNum = widget->stringShowNum;

  //行数记录为零，直接返回
  if(0 == stringNum)
  {
    return;
  }

  //行数不足LINE_NUM行，则从缓冲区0行开始显示
  else if(stringNum < widget->maxLineNum)
  {
    for(i = 0; i <= stringNum; i++)
    {
      //清背景
      x0 = widget->showX0;
      y0 = widget->showY0 + widget->textHeight * i;
      x1 = widget->showX1;
      y1 = y0 + widget->textHeight - 1;
      GUIFillColor(x0, y0, x1, y1, widget->textBackColor);

      //显示字符串
      GUIDrawTextLine(x0, y0, (u32)(widget->stringBuf + (widget->lineCharNum + 1) * i), 
      widget->textFont, NULL, widget->textColor, 1, widget->lineCharNum);
    }
  }

  //行数已满，需要考虑从哪行开始显示
  else
  {
    for(i = 0; i < widget->maxLineNum; i++)
    {
      //清背景
      x0 = widget->showX0;
      y0 = widget->showY0 + widget->textHeight * i;
      x1 = widget->showX1;
      y1 = y0 + widget->textHeight - 1;
      GUIFillColor(x0, y0, x1, y1, widget->textBackColor);

      //显示字符串
      GUIDrawTextLine(x0, y0, (u32)(widget->stringBuf + (widget->lineCharNum + 1) * head), 
      widget->textFont, NULL, widget->textColor, 1, widget->lineCharNum);
      head = (head + 1) % widget->maxLineNum;
    }
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitTerminalWidgetStruct
* 函数功能：初始化终端设备结构体
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
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
* 函数名称：CreateTerminalWidget
* 函数功能：创建终端控件
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateTerminalWidget(StructTerminalWidget* widget)
{
  u32 i;

  //标记已创建
  widget->hasCreate = 1;

  //保存控件背景
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //绘制终端背景
  DrawBackground(widget);

  //获取终端字体背景颜色
  widget->textBackColor = GUIReadPoint(widget->x0 + widget->width / 2, widget->y0 + widget->height / 2);

  //计算自动参数
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

  //清空字符串
  for(i = 0; i < TERMINAL_MAX_CHAR_NUM; i++)
  {
    widget->stringBuf[i] = 0;
  }
}

/*********************************************************************************************************
* 函数名称：TerminalWidgetClear
* 函数功能：清空终端显示
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void TerminalWidgetClear(StructTerminalWidget* widget)
{
  u32 i;

  for(i = 0; i < TERMINAL_MAX_CHAR_NUM; i++)
  {
    widget->stringBuf[i] = 0;
  }

  //清背景
  GUIFillColor(widget->showX0, widget->showY0, widget->showX1, widget->showY1, widget->textBackColor);

  //清空标记
  widget->stringHead = 0;
  widget->stringShowNum = 0;
}

/*********************************************************************************************************
* 函数名称：TerminalWidgetShowLine
* 函数功能：终端中显示一行字符串
* 输入参数：widget：控件设备结构体地址，string：字符串首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void TerminalWidgetShowLine(StructTerminalWidget* widget, char* string)
{
  u32 i;

  //保存字符串到缓冲区
  i = 0;
  while((*(string + i) <= '~') && (*(string + i) >= ' ') && (i < widget->lineCharNum))
  {
    widget->stringBuf[widget->stringHead * (widget->lineCharNum + 1) + i] = *(string + i);
    i++;
  }
  widget->stringBuf[widget->stringHead * (widget->lineCharNum + 1) + i] = 0;

  //更新新行插入位置
  widget->stringHead = (widget->stringHead + 1) % widget->maxLineNum;

  //记录行数
  if(widget->stringShowNum < widget->maxLineNum)
  {
    widget->stringShowNum = widget->stringShowNum + 1;
  }
  else
  {
    widget->stringShowNum = widget->maxLineNum;
  }

  //更新终端显示
  UpdateStringShow(widget);
}

/*********************************************************************************************************
* 函数名称：DeleteTerminalWidget
* 函数功能：删除终端控件
* 输入参数：widget：控件设备结构体地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteTerminalWidget(StructTerminalWidget* widget)
{
  //还原背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
