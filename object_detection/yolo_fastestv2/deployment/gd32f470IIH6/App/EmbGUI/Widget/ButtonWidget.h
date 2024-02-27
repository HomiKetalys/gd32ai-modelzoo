/*********************************************************************************************************
* 模块名称：ButtonWidget.h
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
#ifndef _BUTTON_WIDGET_H_
#define _BUTTON_WIDGET_H_

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
typedef enum
{
  BUTTON_WIDGET_TYPE_EDGE,  //边沿触发（默认状态）
  BUTTON_WIDGET_TYPE_LEVEL, //电平触发
}EnumButtonWidgetType;

//按下、释放枚举
typedef enum
{
  BUTTON_WIDGET_RELEASE = 0, //按键释放
  BUTTON_WIDGET_PRESS   = 1, //按键按下
}EnumButtonWidgetFlag;

//触摸按键控件
typedef struct
{
  u32                  x0;        //控件起始横坐标
  u32                  y0;        //控件起始纵坐标
  u32                  width;     //按钮宽度
  u32                  height;    //按钮高度
  u32                  textX0;    //显示字符横坐标与控件起始横坐标的偏移量
  u32                  textY0;    //显示字符纵坐标与控件起始纵坐标的偏移量
  EnumGUIFont          textFont;  //显示字符字体大小
  u32                  textColor; //显示字符颜色
  const char*          text;      //控件字符
  EnumButtonWidgetFlag state;     //按键当前状态
  EnumButtonWidgetType type;      //按键类型
  void*                buttonBackground;      //按键最底下的背景缓冲区，不用请填入NULL
  void*                pressBackgroudImage;   //按键按下状态下背景图片，不用请填入NULL
  void*                releaseBackgroudImage; //按键释放状态下背景图片，不用请填入NULL
  EnumGUIImageType     pressImageType;        //按下背景图片格式，默认使用位图
  EnumGUIImageType     releaseImageType;      //释放背景图片格式，默认使用位图
  u32                  pressBackColor;        //纯色状态下按键按下背景颜色，默认绿色
  u32                  releaseBackColor;      //纯色状态下按键释放背景颜色，默认蓝色
  void                 (*pressCallback)(void* this);   //按键按下回调函数，不用请填入NULL
  void                 (*releaseCallback)(void* this); //按键释放回调函数，不用请填入NULL
}StructButtonWidget;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void InitButtonWidgetStruct(StructButtonWidget* widget); //设置触摸按键结构体默认值
void CreateButtonWidget(StructButtonWidget* widget);     //创建触摸按键控件
void ScanButtonWidget(StructButtonWidget* widget);       //按键扫描
void DeleteButtonWidget(StructButtonWidget* widget);     //删除按键控件
void UpdateButtonWidget(StructButtonWidget* widget);     //手动更新显示

#endif
