/*********************************************************************************************************
* 模块名称：TerminalWidget.h
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
#ifndef _TERMINAL_WIDGET_H_
#define _TERMINAL_WIDGET_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//屏幕能显示的最多字符串数量
#define TERMINAL_LINE_CHAR_NUM (GUI_X_PIXEL_SIZE / (GUI_SMALLEST_FONT / 2))
#define TERMINAL_MAX_LIME_MUN  (1 + (GUI_Y_PIXEL_SIZE / GUI_SMALLEST_FONT))
#define TERMINAL_MAX_CHAR_NUM  (TERMINAL_LINE_CHAR_NUM * TERMINAL_MAX_LIME_MUN)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef struct
{
  //控件基本参数
  u32              x0, y0, width, height; //位置，大小
  u32              topOff, bottomOff;     //显示区域顶部、底部偏移量
  u32              showSpace;             //显示区域与控件边沿之间的间距
  u32              textColor;             //字体颜色
  EnumGUIFont      textFont;              //字体大小
  u32              textHeight;            //一行高度，要大于等于字符高度
  void*            background;            //控件背景，用于删除控件时填回背景，不需要可以填入NULL
  void*            backImage;             //控件背景图片，终端背景图片显示区域需是纯色
  EnumGUIImageType imageType;             //控件背景图片类型
  u32              defaultBackColor;      //默认的背景颜色，backImage为NULL时使用

  //以下参数自动计算，用户不可更改
  //创建标志位，0-未创建，1-已创建
  u32 hasCreate;

  //字符串背景颜色
  u32 textBackColor;

  //显示区域大小
  u32 showX0, showY0, showX1, showY1, showWidth, showHeight;

  //一行显示字符串数以及最大行数
  u32 lineCharNum, maxLineNum;

  //字符串缓冲区，很大，所以StructTerminalWidget结构体最好由动态内存分配
  u8 stringBuf[TERMINAL_MAX_CHAR_NUM]; //一行有lineCharNum+1个元素

  //新行插入位置
  u32 stringHead;

  //以显示行数
  u32 stringShowNum;
}StructTerminalWidget;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void InitTerminalWidgetStruct(StructTerminalWidget* widget);             //初始化终端设备结构体
void CreateTerminalWidget(StructTerminalWidget* widget);                 //创建终端控件
void TerminalWidgetClear(StructTerminalWidget* widget);                  //清空终端显示
void TerminalWidgetShowLine(StructTerminalWidget* widget, char* string); //终端中显示一行字符串
void DeleteTerminalWidget(StructTerminalWidget* widget);                 //删除终端控件

#endif
