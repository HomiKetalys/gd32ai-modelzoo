/*********************************************************************************************************
* 模块名称：ButtonWidget.c
* 摘    要：触摸按键控件模块
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
#include "ButtonWidget.h"

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
static void DrawBackground(StructButtonWidget* widget); //绘制按键背景
static void DrawText(StructButtonWidget* widget);       //显示控件字符
static void CallBackProc(StructButtonWidget* widget);   //回调处理
static u8   IsButtonPress(StructButtonWidget* widget);  //判断按键是否被按下

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DrawBackground
* 函数功能：绘制按键背景
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawBackground(StructButtonWidget* widget)
{
  u32 x0, y0, x1, y1;

  //计算起点终点坐标
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //绘制按键背景
  if(NULL != widget->buttonBackground)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }

  //绘制按键释放状态下背景
  if(BUTTON_WIDGET_RELEASE == widget->state)
  {
    if(GUI_IMAGE_TYPE_NULL == widget->releaseImageType)
    {
      //纯色背景
      GUIFillColor(x0, y0, x1, y1, widget->releaseBackColor);
    }
    else if(GUI_IMAGE_TYPE_CLEAR != widget->releaseImageType)
    {
      //纯色背景
      if(NULL == widget->releaseBackgroudImage)
      {
        GUIFillColor(x0, y0, x1, y1, widget->releaseBackColor);
      }
      
      //绘制背景图片
      else
      {
        GUIDrawImage(x0, y0, (u32)widget->releaseBackgroudImage, widget->releaseImageType);
      }
    }
  }

  //绘制按键按下状态背景
  else if(BUTTON_WIDGET_PRESS == widget->state)
  {
    if(GUI_IMAGE_TYPE_NULL == widget->pressImageType)
    {
      //纯色背景
      GUIFillColor(x0, y0, x1, y1, widget->pressBackColor);
    }
    else if(GUI_IMAGE_TYPE_CLEAR != widget->pressImageType)
    {
      //纯色背景
      if(NULL == widget->pressBackgroudImage)
      {
        GUIFillColor(x0, y0, x1, y1, widget->pressBackColor);
      }

      //绘制背景图片
      else
      {
        GUIDrawImage(x0, y0, (u32)widget->pressBackgroudImage, widget->pressImageType);
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称：DrawText
* 函数功能：显示控件字符
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawText(StructButtonWidget* widget)
{
  u32 x0, y0, len;

  //计算起点终点坐标
  x0 = widget->x0 + widget->textX0;
  y0 = widget->y0 + widget->textY0;

  //计算控件最大显示字符数量
  len = widget->width / GetFontWidth(widget->textFont);

  GUIDrawTextLine(x0, y0, (u32)widget->text, widget->textFont, NULL, widget->textColor, 1, len);
}

/*********************************************************************************************************
* 函数名称：CallBackProc
* 函数功能：回调处理
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CallBackProc(StructButtonWidget* widget)
{
  //按键按下响应
  if(BUTTON_WIDGET_PRESS == widget->state)
  {
    if(NULL != widget->pressCallback)
    {
      widget->pressCallback(widget);
    }
  }

  //按键释放响应
  else if(BUTTON_WIDGET_RELEASE == widget->state)
  {
    if(NULL != widget->releaseCallback)
    {
      widget->releaseCallback(widget);
    }
  }
}

/*********************************************************************************************************
* 函数名称：IsButtonPress
* 函数功能：判断按键是否被按下
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：0-按键释放，1-按键按下
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 IsButtonPress(StructButtonWidget* widget)
{
  u32 tx, ty;         //触点坐标信息
  u32 x0, y0, x1, y1; //按键起点、终点坐标位置
  u32 i;              //循环变量

  //计算起点终点坐标
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //依次检查所有触点，查看是否有触点触碰到控件
  for(i = 0; i < GUI_TOUCH_NUM_MAX; i++)
  {
    if(1 == GUIGetTouch(&tx, &ty, i))
    {
      //判断是否按下
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

  //无按键触碰到控件，返回0
  return 0;
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitButtonWidgetStruct
* 函数功能：设置触摸按键结构体默认值
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitButtonWidgetStruct(StructButtonWidget* widget)
{
  widget->x0                    = 0;                       //起点横坐标默认左上角
  widget->y0                    = 0;                       //起点纵坐标默认左上角
  widget->width                 = 130;                     //按键宽度
  widget->height                = 65;                      //按键高度
  widget->textX0                = 35;                      //字符与按键原点横坐标偏移量
  widget->textY0                = 20;                      //字符与按键原点纵坐标偏移量
  widget->textFont              = GUI_FONT_ASCII_24;       //12x24 ASCII码字体
  widget->textColor             = GUI_COLOR_WHITE;         //字体颜色默认为白色
  widget->text                  = "Button";                //默认显示Button
  widget->state                 = BUTTON_WIDGET_RELEASE;   //按键默认为释放状态
  widget->type                  = BUTTON_WIDGET_TYPE_EDGE; //默认边沿检测
  widget->buttonBackground      = NULL;                    //不保存按键背景
  widget->pressBackgroudImage   = NULL;                    //按键按下状态不使用背景图片
  widget->releaseBackgroudImage = NULL;                    //按键释放状态不使用背景图片
  widget->pressImageType        = GUI_IMAGE_TYPE_BMP;      //按键按下状态背景图片默认选用位图
  widget->releaseImageType      = GUI_IMAGE_TYPE_BMP;      //按键释放状态背景图片默认选用位图
  widget->pressBackColor        = GUI_COLOR_GREEN;         //不使用背景图片时默认按键按下时使用绿色填充
  widget->releaseBackColor      = GUI_COLOR_BLUE;          //不使用背景图片时默认按键释放时使用蓝色填充
  widget->pressCallback         = NULL;                    //按键按下时不使用回调
  widget->releaseCallback       = NULL;                    //按键释放时不使用回调
}

/*********************************************************************************************************
* 函数名称：CreateButtonWidget
* 函数功能：创建触摸按键控件
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateButtonWidget(StructButtonWidget* widget)
{
  //保存按键背景
  if(NULL != widget->buttonBackground)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }

  //按键默认为释放状态
  widget->state = BUTTON_WIDGET_RELEASE;

  //刷新背景
  DrawBackground(widget);

  //绘制控件字符
  DrawText(widget);
}

/*********************************************************************************************************
* 函数名称：ScanButtonWidget
* 函数功能：按键扫描
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ScanButtonWidget(StructButtonWidget* widget)
{
  u8 buttonUpdateFlag;    //按键状态更新标志位

  //清空按键状态更新标志位
  buttonUpdateFlag = 0;

  //触点被按下
  if(1 == IsButtonPress(widget))
  {
    //上一次扫描时按键为释放状态
    if(BUTTON_WIDGET_RELEASE == widget->state)
    {
      //设置按键为按下状态
      widget->state = BUTTON_WIDGET_PRESS;

      //标记按键状态改变
      buttonUpdateFlag = 1;
    }
  }

  //触点被释放
  else
  {
    //上一次扫描时按键为按下状态
    if(BUTTON_WIDGET_PRESS == widget->state)
    {
      //设置按键为释放状态
      widget->state = BUTTON_WIDGET_RELEASE;

      //标记按键状态改变
      buttonUpdateFlag = 1;
    }
  }

  //边沿检测下回调函数
  if(BUTTON_WIDGET_TYPE_EDGE == widget->type)
  {
    if(1 == buttonUpdateFlag)
    {
      CallBackProc(widget);
    }
  }

  //电平检测下回调函数
  else if(BUTTON_WIDGET_TYPE_LEVEL == widget->type)
  {
    CallBackProc(widget);
  }

  //刷新背景
  if(1 == buttonUpdateFlag)
  {
    //刷新背景
    DrawBackground(widget);

    //绘制控件字符
    DrawText(widget);
  }
}

/*********************************************************************************************************
* 函数名称：DeleteButtonWidget
* 函数功能：删除按键控件
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteButtonWidget(StructButtonWidget* widget)
{
  //将背景填回去
  if(NULL != widget->buttonBackground)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->buttonBackground);
  }
}

/*********************************************************************************************************
* 函数名称：UpdateButtonWidget
* 函数功能：手动更新显示
* 输入参数：widget：控件结构体
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：void
*********************************************************************************************************/
void UpdateButtonWidget(StructButtonWidget* widget)
{
  //刷新背景
  DrawBackground(widget);

  //绘制控件字符
  DrawText(widget);
}
