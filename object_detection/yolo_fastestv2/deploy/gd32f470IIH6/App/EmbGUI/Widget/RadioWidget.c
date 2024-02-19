/*********************************************************************************************************
* 模块名称：RadioWidget.c
* 摘    要：单选控件模块
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
#include "RadioWidget.h"

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
static void DrawBackground(StructRadioWidget* widget); //绘制背景
static void CallBackProc(StructRadioWidget* widget);   //回调处理
static u8   IsPress(StructRadioWidget* widget);        //判断控件是否被按下

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DrawBackground
* 函数功能：绘制背景和图片
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawBackground(StructRadioWidget* widget)
{
  u32 x0, y0, x1, y1;

  //计算起点终点坐标
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //绘制背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(x0, y0, widget->width, widget->height, (u32)widget->background);
  }

  //使用图片标识控件有无焦点
  if(RADIO_TYPE_IMAGE == widget->type)
  {
    //控件获得焦点
    if(RADIO_HAS_FOCUS == widget->hasFocus)
    {
      if(GUI_IMAGE_TYPE_NULL == widget->hasFocusImageType)
      {
        //绘制纯色填充
        GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
      }
      else if(GUI_IMAGE_TYPE_CLEAR != widget->hasFocusImageType)
      {
        //绘制纯色填充
        if(NULL == widget->hasFocusBackgroundImage)
        {
          GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
        }

        //绘制图片
        else
        {
          GUIDrawImage(x0, y0, (u32)widget->hasFocusBackgroundImage, widget->hasFocusImageType);
        }
      }
    }

    //控件无焦点
    else if(RADIO_NO_FOCUS == widget->hasFocus)
    {
      if(GUI_IMAGE_TYPE_NULL == widget->noFocusImageType)
      {
        //绘制纯色填充
        GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
      }
      else if(GUI_IMAGE_TYPE_CLEAR != widget->noFocusImageType)
      {
        //绘制纯色填充
        if(NULL == widget->noFocusBackgroundImage)
        {
          GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
        }

        //绘制图片
        else
        {
          GUIDrawImage(x0, y0, (u32)widget->noFocusBackgroundImage, widget->noFocusImageType);
        }
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称：DrawFocus
* 函数功能：绘制有焦点时的矩形外框或横线
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawFocus(StructRadioWidget* widget)
{
  u32 lineSize, x0, y0, x1, y1;

  //控件无焦点或类型不对，直接返回
  if((RADIO_TYPE_IMAGE == widget->type) || (RADIO_HAS_FOCUS != widget->hasFocus))
  {
    return;
  }

  //获取直线线宽的一半
  lineSize = widget->lineSize / 2;
  if(0 == lineSize)
  {
    return;
  }

  //计算线条起点终点
  x0 = widget->x0 + widget->lineSize;
  y0 = widget->y0 + widget->lineSize;
  x1 = widget->x0 + widget->width  - widget->lineSize - 1;
  y1 = widget->y0 + widget->height - widget->lineSize - 1;

  //绘制矩形
  if(RADIO_TYPE_RECT == widget->type)
  {
    GUIDrawLine(x0, y0, x1, y0, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x0, y0, x0, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x1, y0, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
    GUIDrawLine(x0, y1, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
  }

  //绘制横线
  else if(RADIO_TYPE_LINE == widget->type)
  {
    GUIDrawLine(x0, y1, x1, y1, lineSize, widget->lineColor, GUI_LINE_CIRCLE);
  }
}

/*********************************************************************************************************
* 函数名称：CallBackProc
* 函数功能：回调处理
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CallBackProc(StructRadioWidget* widget)
{
  if(NULL != widget->pressCallback)
  {
    widget->pressCallback(widget);
  }
}

/*********************************************************************************************************
* 函数名称：IsPress
* 函数功能：判断控件是否按下
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：1-控件被按下，0-控件未被按下
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 IsPress(StructRadioWidget* widget)
{
  u32 tx, ty;         //触点坐标信息
  u8  pointState;     //触点触摸状态
  u32 x0, y0, x1, y1; //按键起点、终点坐标位置

  //获取屏幕触点信息
  pointState = GUIGetTouch(&tx, &ty, 0);

  //触点1并未按下
  if(0 == pointState)
  {
    return 0;
  }

  //触点1按下
  else if(1 == pointState)
  {
    //计算起点终点坐标
    x0 = widget->x0;
    y0 = widget->y0;
    x1 = widget->x0 + widget->width;
    y1 = widget->y0 + widget->height;

    //判断是否按下
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
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitRadioWidgetStruct
* 函数功能：设置单选控件结构体默认值
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitRadioWidgetStruct(StructRadioWidget* widget)
{
  widget->x0                      = 0;                   //起点横坐标默认左上角
  widget->y0                      = 0;                   //起点纵坐标默认左上角
  widget->width                   = 50;                  //宽度
  widget->height                  = 50;                  //高度
  widget->hasFocus                = RADIO_NO_FOCUS;      //无焦点
  widget->type                    = RADIO_TYPE_RECT;     //使用白色矩形标识有无焦点
  widget->lineColor               = GUI_COLOR_WHITE;     //使用白色线条
  widget->lineSize                = 2;                   //线条大小
  widget->background              = NULL;                //控件背景缓冲区
  widget->hasFocusBackgroundImage = NULL;                //有焦点时不使用图片
  widget->noFocusBackgroundImage  = NULL;                //无焦点时不使用图片
  widget->hasFocusImageType       = GUI_IMAGE_TYPE_JPEG; //有焦点时背景图片格式默认为JPEG
  widget->noFocusImageType        = GUI_IMAGE_TYPE_JPEG; //无焦点时背景图片格式默认为JPEG
  widget->hasFocusColor           = GUI_COLOR_GREEN;     //不使用背景图片时有焦点用绿色填充
  widget->noFocusColor            = GUI_COLOR_BLUE;      //不使用背景图片时无焦点用蓝色填充
  widget->pressCallback           = NULL;                //不使用回调
}

/*********************************************************************************************************
* 函数名称：CreateRadioWidget
* 函数功能：初始化单选控件
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateRadioWidget(StructRadioWidget* widget)
{
  //控件默认无焦点
  widget->hasFocus = RADIO_NO_FOCUS;

  //保存控件背景
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //刷新背景
  DrawBackground(widget);
}

/*********************************************************************************************************
* 函数名称：ScanRadioWidget
* 函数功能：单选控件扫描
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ScanRadioWidget(StructRadioWidget* widget)
{
  //控件被按下
  if(1 == IsPress(widget))
  {
    //自动获取控件焦点
    if(RADIO_NO_FOCUS == widget->hasFocus)
    {
      //回调处理
      CallBackProc(widget);

      //控件获得焦点
      widget->hasFocus = RADIO_HAS_FOCUS;

      //背景重绘
      DrawBackground(widget);

      //绘制矩形外框或横线
      DrawFocus(widget);
    }
  }
}

/*********************************************************************************************************
* 函数名称：ClearRadioWidgetFocus
* 函数功能：清除单选控件焦点
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ClearRadioWidgetFocus(StructRadioWidget* widget)
{
  if(RADIO_HAS_FOCUS == widget->hasFocus)
  {
    //清除单选控件焦点
    widget->hasFocus = RADIO_NO_FOCUS;

    //背景重绘
    DrawBackground(widget);
  }
}

/*********************************************************************************************************
* 函数名称：DeleteRadioWidget
* 函数功能：删除单选控件
* 输入参数：widget：单选控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteRadioWidget(StructRadioWidget* widget)
{
  //填回背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
