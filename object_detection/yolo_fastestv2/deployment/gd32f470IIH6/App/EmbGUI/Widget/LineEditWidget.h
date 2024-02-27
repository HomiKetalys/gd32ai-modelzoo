/*********************************************************************************************************
* 模块名称：LineEditWidget.h
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
#ifndef _LINE_EDIT_WIDGET_H_
#define _LINE_EDIT_WIDGET_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define LINE_EDIT_MAX_LEN ((GUI_X_PIXEL_SIZE / (GUI_SMALLEST_FONT / 2)) + 1) //一行显示的最多字符串

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/
typedef enum
{
  EDIT_LINE_NO_FOCUS = 0, //无焦点
  EDIT_LINE_HAS_FOCUS = 1, //有焦点
}EnumEditLineFocus;

//行编辑控件
typedef struct
{
  u32               x0, y0, width, height;        //控件坐标和大小
  u32               textX0;                       //显示字符横坐标与控件起始横坐标的偏移量
  u32               textY0;                       //显示字符纵坐标与控件起始纵坐标的偏移量
  EnumGUIFont       textFont;                     //显示字符字体大小
  u32               textColor;                    //显示字符颜色
  EnumEditLineFocus hasFocus;                     //焦点状态，控件被按下后自动获取焦点
  void*             background;                   //背景缓冲区
  void*             hasFocusBackgroundImage;      //有焦点时的背景图片，不使用背景图片请填入NULL
  void*             noFocusBackgroundImage;       //无焦点时的背景图片，不使用请填入NULL
  EnumGUIImageType  hasFocusImageType;            //有焦点时背景图片的格式，默认使用JPEG图片
  EnumGUIImageType  noFocusImageType;             //无焦点时背景图片的格式，默认使用JPEG图片
  u32               hasFocusColor;                //有焦点时纯色背景，不使用背景图片下使用，默认绿色
  u32               noFocusColor;                 //无焦点时纯色背景，不使用背景图片下使用，默认蓝色
  void              (*pressCallback)(void* this); //控件被按下响应回调函数，被按下后控件自动获取焦点

  //私有变量，用户不可修改
  char              preText[LINE_EDIT_MAX_LEN];   //前缀
  char              text[LINE_EDIT_MAX_LEN];      //字符串
  u32               editNum;                      //最大字符串长度
}StructLineEditWidget;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void InitLineEditWidgetStruct(StructLineEditWidget* widget);          //设置行编辑控件结构体默认值
void CreateLineEditWidget(StructLineEditWidget* widget);              //初始化行编辑控件模块
void ScanLineEditWidget(StructLineEditWidget* widget);                //扫描行编辑控件模块
void ClearLineEditWidgetFocus(StructLineEditWidget* widget);          //取消焦点
void AddrCharToLineEdit(StructLineEditWidget* widget, u32 code);      //增加字符
void DeleteCodeFromLineEdit(StructLineEditWidget* widget);            //删除字符
void SetLineEditPreText(StructLineEditWidget* widget, char* preTexy); //设置行编辑前缀
void SetLineEditMaxLen(StructLineEditWidget* widget, u32 len);        //设置行编辑字符串长度
void DeleteLineEditWidget(StructLineEditWidget* widget);              //删除行编辑控件

#endif
