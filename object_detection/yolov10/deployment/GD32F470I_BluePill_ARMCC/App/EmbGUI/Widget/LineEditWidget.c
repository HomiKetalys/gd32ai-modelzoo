/*********************************************************************************************************
* 模块名称：LineEditWidget.c
* 摘    要：行编辑控件模块
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
#include "LineEditWidget.h"

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
static void DrawBackground(StructLineEditWidget* widget); //绘制背景
static void DrawText(StructLineEditWidget* widget);       //显示控件字符
static void CallBackProc(StructLineEditWidget* widget);   //回调处理
static u8   IsPress(StructLineEditWidget* widget);        //判断控件是否被按下

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：DrawBackground
* 函数功能：绘制背景
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawBackground(StructLineEditWidget* widget)
{
  u32 x0, y0, x1, y1;

  //计算起点终点坐标
  x0 = widget->x0;
  y0 = widget->y0;
  x1 = widget->x0 + widget->width;
  y1 = widget->y0 + widget->height;

  //控件获得焦点
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //绘制纯色背景
    if(NULL == widget->hasFocusBackgroundImage)
    {
      GUIFillColor(x0, y0, x1, y1, widget->hasFocusColor);
    }

    //绘制背景图片
    else
    {
      GUIDrawImage(x0, y0, (u32)widget->hasFocusBackgroundImage, widget->hasFocusImageType);
    }
  }

  //控件无焦点
  else if(EDIT_LINE_NO_FOCUS == widget->hasFocus)
  {
    //绘制纯色背景
    if(NULL == widget->noFocusBackgroundImage)
    {
      GUIFillColor(x0, y0, x1, y1, widget->noFocusColor);
    }

    //绘制背景图片
    else
    {
      GUIDrawImage(x0, y0, (u32)widget->noFocusBackgroundImage, widget->noFocusImageType);
    }
  }
}

/*********************************************************************************************************
* 函数名称：DrawText
* 函数功能：显示控件字符
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void DrawText(StructLineEditWidget* widget)
{
  static char s_arrString[LINE_EDIT_MAX_LEN];
  u32 x0, y0, len, i, cnt;

  //拷贝前缀到缓冲区
  i = 0;
  cnt = 0;
  while(0 != widget->preText[i])
  {
    s_arrString[cnt++] = widget->preText[i++];
  }
  
  //拷贝内容至缓冲区
  i = 0;
  while(0 != widget->text[i])
  {
    s_arrString[cnt++] = widget->text[i++];
  }
  s_arrString[cnt] = 0;

  //计算起点终点坐标
  x0 = widget->x0 + widget->textX0;
  y0 = widget->y0 + widget->textY0;

  //计算控件最大显示字符数量
  len = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);

  GUIDrawTextLine(x0, y0, (u32)s_arrString, widget->textFont, NULL, widget->textColor, 1, len);
}

/*********************************************************************************************************
* 函数名称：CallBackProc
* 函数功能：回调处理
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CallBackProc(StructLineEditWidget* widget)
{
  if(NULL != widget->pressCallback)
  {
    widget->pressCallback(widget);
  }
}

/*********************************************************************************************************
* 函数名称：IsPress
* 函数功能：判断控件是否按下
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：1-控件被按下，0-控件未被按下
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 IsPress(StructLineEditWidget* widget)
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
* 函数名称：InitLineEditWidgetStruct
* 函数功能：设置行编辑控件结构体默认值
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitLineEditWidgetStruct(StructLineEditWidget* widget)
{
  u32 i;
  widget->x0                      = 0;                   //起点横坐标默认左上角
  widget->y0                      = 0;                   //起点纵坐标默认左上角
  widget->width                   = 100;                 //宽度
  widget->height                  = 50;                  //高度
  widget->textX0                  = 75;                  //字符与控件原点横坐标偏移量
  widget->textY0                  = 10;                  //字符与控件原点纵坐标偏移量
  widget->textFont                = GUI_FONT_ASCII_24;   //12x24 ASCII码字体
  widget->textColor               = GUI_COLOR_WHITE;     //字体颜色默认为白色
  widget->background              = NULL;                //默认无背景
  widget->hasFocusBackgroundImage = NULL;                //有焦点时不使用背景图片
  widget->noFocusBackgroundImage  = NULL;                //无焦点时不使用背景图片
  widget->hasFocusImageType       = GUI_IMAGE_TYPE_JPEG; //有焦点时背景图片格式默认为JPEG
  widget->noFocusImageType        = GUI_IMAGE_TYPE_JPEG; //无焦点时背景图片格式默认为JPEG
  widget->hasFocusColor           = GUI_COLOR_GREEN;     //不使用背景图片时有焦点用绿色填充
  widget->noFocusColor            = GUI_COLOR_BLUE;      //不使用背景图片时无焦点用蓝色填充
  widget->pressCallback           = NULL;                //不使用回调
  widget->editNum                 = LINE_EDIT_MAX_LEN;   //默认限制最大长度
  for(i = 0; i < LINE_EDIT_MAX_LEN; i++)
  {
    widget->preText[i] = 0;
    widget->text[i] = 0;
  }
}

/*********************************************************************************************************
* 函数名称：CreateLineEditWidget
* 函数功能：初始化行编辑控件模块
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateLineEditWidget(StructLineEditWidget* widget)
{
  //统计显示最大长度
  widget->editNum = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);

  //保存控件背景
  if(NULL != widget->background)
  {
    GUISaveBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }

  //控件默认无焦点
  widget->hasFocus = EDIT_LINE_NO_FOCUS;

  //刷新背景
  DrawBackground(widget);

  //绘制显示字符
  DrawText(widget);
}

/*********************************************************************************************************
* 函数名称：ScanLineEditWidget
* 函数功能：行编辑控件扫描
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ScanLineEditWidget(StructLineEditWidget* widget)
{
  //控件被按下
  if(1 == IsPress(widget))
  {
    //自动获取控件焦点
    if(EDIT_LINE_NO_FOCUS == widget->hasFocus)
    {
      //回调处理
      CallBackProc(widget);

      //控件获得焦点
      widget->hasFocus = EDIT_LINE_HAS_FOCUS;

      //背景重绘
      DrawBackground(widget);

      //字符重绘
      DrawText(widget);
    }
  }
}

/*********************************************************************************************************
* 函数名称：ClearLineEditWidgetFocus
* 函数功能：清除行编辑控件焦点
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void ClearLineEditWidgetFocus(StructLineEditWidget* widget)
{
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //清除行编辑控件焦点
    widget->hasFocus = EDIT_LINE_NO_FOCUS;

    //背景重绘
    DrawBackground(widget);

    //字符重绘
    DrawText(widget);
  }
}

/*********************************************************************************************************
* 函数名称：AddrCharToLineEdit
* 函数功能：增加字符
* 输入参数：widget：行编辑控件，code：字符编码
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：仅在控件获得焦点的情况下更新字符显示
*********************************************************************************************************/
void AddrCharToLineEdit(StructLineEditWidget* widget, u32 code)
{
  u32 maxShowLen, preLen, editLen, stringLen; //字符串长度
  u32 i;

  //仅在控件获得焦点的情况下更新字符显示
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //显示最长字符串
    maxShowLen = widget->editNum;

    //统计前缀长度
    preLen = 0;
    i = 0;
    while(0 != widget->preText[i++])
    {
      preLen++;
    }

    //统计编辑字符串长度
    editLen = 0;
    i = 0;
    while(0 != widget->text[i++])
    {
      editLen++;
    }

    //计算当前字符串长度
    stringLen = preLen + editLen;

    //添加新的字符显示
    if(stringLen < maxShowLen)
    {
      //添加英文字符
      if(code < 0x80)
      {
        widget->text[editLen] = code;
        widget->text[editLen + 1] = 0;

        //背景重绘
        DrawBackground(widget);

        //字符重绘
        DrawText(widget);
      }

      //添加中文字符
      else if((stringLen + 1) < maxShowLen)
      {
        widget->text[editLen] = code >> 8;
        widget->text[editLen + 1] = code & 0xFF;
        widget->text[editLen + 2] = 0;

        //背景重绘
        DrawBackground(widget);

        //字符重绘
        DrawText(widget);
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称：DeleteCodeFromLineEdit
* 函数功能：删除字符
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：仅在控件获得焦点的情况下更新字符显示
*********************************************************************************************************/
void DeleteCodeFromLineEdit(StructLineEditWidget* widget)
{
  u32 i, editLen;

  //仅在控件获得焦点的情况下更新字符显示
  if(EDIT_LINE_HAS_FOCUS == widget->hasFocus)
  {
    //统计编辑字符串长度
    editLen = 0;
    i = 0;
    while(0 != widget->text[i++])
    {
      editLen++;
    }

    //删除字符
    if(editLen > 0)
    {
      widget->text[editLen - 1] = 0;

      //背景重绘
      DrawBackground(widget);

      //字符重绘
      DrawText(widget);
    }
  }
}

/*********************************************************************************************************
* 函数名称：SetLineEditPreText
* 函数功能：设置行编辑前缀
* 输入参数：widget：行编辑控件，preTexy：前缀字符串首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：仅在控件获得焦点的情况下更新字符显示，需在创建后调用
*********************************************************************************************************/
void SetLineEditPreText(StructLineEditWidget* widget, char* preTexy)
{
  u32 i;
  i = 0;
  while((0 != preTexy[i]) && (i < widget->editNum))
  {
    widget->preText[i] = preTexy[i];
    i++;
  }
  widget->preText[i] = 0;

  //背景重绘
  DrawBackground(widget);

  //字符重绘
  DrawText(widget);
}

/*********************************************************************************************************
* 函数名称：SetLineEditMaxLen
* 函数功能：设置行编辑字符串长度
* 输入参数：widget：行编辑控件，len：字符串长度，包括前缀和编辑区长度
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：仅在控件获得焦点的情况下更新字符显示，需在创建后调用
*********************************************************************************************************/
void SetLineEditMaxLen(StructLineEditWidget* widget, u32 len)
{
  u32 maxShowLen;
  maxShowLen = (widget->width - (2 * widget->textX0)) / GetFontWidth(widget->textFont);
  if(len <= maxShowLen)
  {
    widget->editNum = len;
  }
}

/*********************************************************************************************************
* 函数名称：DeleteLineEditWidget
* 函数功能：删除行编辑控件
* 输入参数：widget：行编辑控件
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteLineEditWidget(StructLineEditWidget* widget)
{
  //填回背景
  if(NULL != widget->background)
  {
    GUIDrawBackground(widget->x0, widget->y0, widget->width, widget->height, (u32)widget->background);
  }
}
