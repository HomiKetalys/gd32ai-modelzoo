/*********************************************************************************************************
* 模块名称：SliderWidget.c
* 摘    要：滑条控件模块
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
#include "SliderWidget.h"

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
static void Repaint(StructSliderWidget* widget);                           //控件重绘

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：Repaint
* 函数功能：控件重绘
* 输入参数：widget：需要重绘的控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：圆点直径默认为控件高度
*********************************************************************************************************/
static void Repaint(StructSliderWidget* widget)
{
  static u64 s_arrlastTime = 0;
  u64 newTime;
  u32 x0, y0, x1, y1;
  u32 lineY, lineX0, lineX1, lineY0, lineY1;
  u32 circleR, circleX0, circleX1, circleX, circleY;

  newTime = GUIGetSysTime();

  if(newTime - s_arrlastTime >= 100)
  {
    s_arrlastTime = newTime;

    //计算起点终点
    x0 = widget->x0;
    y0 = widget->y0;
    x1 = widget->x0 + widget->width;
    y1 = widget->y0 + widget->height;

    //绘制背景
    GUIDrawBackground(x0, y0, widget->width, widget->height, widget->background);

    //绘制横线
    if(NULL != widget->lineImage)
    {
      GUIDrawImage(x0, y0, (u32)widget->lineImage, GUI_IMAGE_TYPE_BMP);
    }
    else
    {
      lineY = (y0 + y1) / 2;
      lineX0 = x0 + widget->lineSize;
      lineX1 = x1 - widget->lineSize;
      lineY0 = lineY - widget->lineSize;
      lineY1 = lineY + widget->lineSize;
      GUIFillColor(lineX0, lineY0, lineX1, lineY1, widget->lineColor);
      GUIDrawPoint(lineX0, lineY, widget->lineSize, widget->lineColor);
      GUIDrawPoint(lineX1, lineY, widget->lineSize, widget->lineColor);
    }

    //绘制圆点
    if(NULL != widget->circleImage)
    {
      circleR  = (widget->height / 2) - 1; //半径
      circleX0 = x0 + 3 + circleR;         //最小横坐标
      circleX1 = x1 - 3 - circleR;         //最大横坐标
      circleX0 = circleX0 + (circleX1 - circleX0) * widget->value / 100 - circleR;
      GUIDrawImage(circleX0, y0, (u32)widget->circleImage, GUI_IMAGE_TYPE_BMP);
    }
    else
    {
      circleR  = (widget->height / 2) - 1; //半径
      circleX0 = x0 + 1 + circleR;         //最小横坐标
      circleX1 = x1 - 1 - circleR;         //最大横坐标
      circleX  = circleX0 + (circleX1 - circleX0) * widget->value / 100; //圆心横坐标
      circleY  = (y0 + y1) / 2;      //圆心纵坐标
      GUIDrawPoint(circleX, circleY, circleR, widget->circleColor);
    }
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitSliderWidgetStruct
* 函数功能：设置滑条控件结构体默认值
* 输入参数：widget：滑条控件
* 输出参数：void
* 返 回 值：0-成功创建，其它-创建失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitSliderWidgetStruct(StructSliderWidget* widget)
{
  widget->x0          = 50;               //原点横坐标默认值
  widget->y0          = 100;              //原点纵坐标默认值
  widget->width       = 100;              //宽度
  widget->height      = 30;               //高度
  widget->value       = 50;               //滑条居中
  widget->lineImage   = NULL;             //横线不使用背景图片
  widget->circleImage = NULL;             //圆点不使用背景图片
  widget->lineSize    = 3;                //线条大小为3（宽度为6个像素点）
  widget->lineColor   = GUI_COLOR_ORANGE; //横线颜色为橘色
  widget->circleColor = GUI_COLOR_ORANGE; //圆点颜色为橘色
  widget->background  = NULL;             //背景缓冲区首地址
}

/*********************************************************************************************************
* 函数名称：CreateSliderWidget
* 函数功能：创建滑条控件
* 输入参数：widget：滑条控件
* 输出参数：void
* 返 回 值：0-成功创建，其它-创建失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u8 CreateSliderWidget(StructSliderWidget* widget)
{
  //背景缓冲区不存在
  if(NULL == widget->background)
  {
    return 1;
  }

  //将背景储存到缓冲区
  GUISaveBackground(widget->x0, widget->y0,widget->width, widget->height,widget->background);

  //绘制控件
  Repaint(widget);
  
  return 0;
}

/*********************************************************************************************************
* 函数名称：ScanSliderWidget
* 函数功能：扫描滑条控件
* 输入参数：widget：滑条控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ScanSliderWidget(StructSliderWidget* widget)
{
  u32 tx, ty;             //触点坐标信息
  u8  pointState;         //触点触摸状态
  u32 x0, y0, x1, y1;     //起点、终点坐标位置
  u8  value;              //滑条数值

  //获取屏幕触点信息
  pointState = GUIGetTouch(&tx, &ty, 0);

  //触点被按下
  if(1 == pointState)
  {
    //计算起点终点
    x0 = widget->x0;
    y0 = widget->y0;
    x1 = widget->x0 + widget->width;
    y1 = widget->y0 + widget->height;

    //判断控件所在区域是否被按下
    if((tx >= x0) && (tx <= x1) && (ty >= y0) && (ty <= y1))
    {
      value = 100 * (tx - x0) / (x1 - x0);

      //重绘
      if(value != widget->value)
      {
        widget->value = value;
        Repaint(widget);
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称：DeleteSliderWidget
* 函数功能：删除滑条控件
* 输入参数：widget：滑条控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteSliderWidget(StructSliderWidget* widget)
{
  //填回背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0,widget->width, widget->height,widget->background);
  }
}
