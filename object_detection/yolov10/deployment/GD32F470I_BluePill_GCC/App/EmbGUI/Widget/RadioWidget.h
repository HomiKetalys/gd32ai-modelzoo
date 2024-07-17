/*********************************************************************************************************
* 模块名称：RadioWidget.h
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
#ifndef _RADIO_WIDGET_H_
#define _RADIO_WIDGET_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "GUIConf.h"
#include "GUIPlatform.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//单选控件焦点枚举
typedef enum
{
  RADIO_NO_FOCUS  = 0, //无焦点
  RADIO_HAS_FOCUS = 1, //有焦点
}EnumRadioFocus;

//单选控件类型枚举
typedef enum
{
  RADIO_TYPE_IMAGE, //使用两张图标区分有焦点和无焦点
  RADIO_TYPE_RECT,  //使用白色方框标识控件有无焦点，此时需要保存控件背景，有/无焦点背景图片（或纯色背景）被忽略
  RADIO_TYPE_LINE,  //在控件下方绘制一条白线标识控件有无焦点，与RADIO_TYPE_RECT使用方法类似
}EnumRadioType;

//单选控件
typedef struct
{
  u32               x0;         //原点横坐标
  u32               y0;         //原点纵坐标
  u32               width;      //宽度
  u32               height;     //高度
  EnumRadioFocus    hasFocus;   //焦点状态，控件被按下后自动获取焦点
  EnumRadioType     type;       //控件类型
  u32               lineColor;  //矩形外框或横线的颜色
  u32               lineSize;   //矩形外框或横线的线宽
  void*             background; //控件背景缓冲区，用户提供
  void*             hasFocusBackgroundImage;      //有焦点时的图片，不使用请填入NULL
  void*             noFocusBackgroundImage;       //无焦点时的图片，不使用请填入NULL
  EnumGUIImageType  hasFocusImageType;            //有焦点时图片的格式，默认使用JPEG图片
  EnumGUIImageType  noFocusImageType;             //无焦点时图片的格式，默认使用JPEG图片
  u32               hasFocusColor;                //有焦点时纯色填充，不使用图片下使用，默认绿色
  u32               noFocusColor;                 //无焦点时纯色填充，不使用图片下使用，默认蓝色
  void              (*pressCallback)(void* this); //控件被按下响应回调函数，被按下后控件自动获取焦点
}StructRadioWidget;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void InitRadioWidgetStruct(StructRadioWidget* widget);          //设置单选控件结构体默认值
void CreateRadioWidget(StructRadioWidget* widget);              //初始化单选控件
void ScanRadioWidget(StructRadioWidget* widget);                //扫描单选控件
void ClearRadioWidgetFocus(StructRadioWidget* widget);          //取消焦点
void DeleteRadioWidget(StructRadioWidget* widget);              //删除单选控件

#endif
