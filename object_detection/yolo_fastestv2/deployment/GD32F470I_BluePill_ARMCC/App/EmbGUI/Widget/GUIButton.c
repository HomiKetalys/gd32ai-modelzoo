/*********************************************************************************************************
* 模块名称：GUIButton.c
* 摘    要：GUI按键模块
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
#include "GUIButton.h"
#include "TLILCD.h"
#include "ButtonWidget.h"
#include "BMP.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructButtonWidget s_arrButton[GUI_BUTTON_MAX]; //触摸按键
static EnumGUIButtons     s_enumButtonPress;           //用来标记那个按键按下

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ButtonPressCallback(void* button);  //按键回调函数

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ButtonPressCallback
* 函数功能：按键回调函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ButtonPressCallback(void* button)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    if((StructButtonWidget*)button == &s_arrButton[i])
    {
      s_enumButtonPress = i;
      return;
    }
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitGUIButton
* 函数功能：初始化GUI按键模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitGUIButton(void)
{
  //标记无按键按下
  s_enumButtonPress = GUI_BUTTON_NONE;

//创建上一项按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PREVIOUS]);
  s_arrButton[GUI_BUTTON_PREVIOUS].x0 = 52;
  s_arrButton[GUI_BUTTON_PREVIOUS].y0 = 733;
  s_arrButton[GUI_BUTTON_PREVIOUS].width = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].height = 53;
  s_arrButton[GUI_BUTTON_PREVIOUS].text = "";
  s_arrButton[GUI_BUTTON_PREVIOUS].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PREVIOUS].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PREVIOUS]);

  //创建下一项按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_NEXT]);
  s_arrButton[GUI_BUTTON_NEXT].x0 = 373;
  s_arrButton[GUI_BUTTON_NEXT].y0 = 733;
  s_arrButton[GUI_BUTTON_NEXT].width = 53;
  s_arrButton[GUI_BUTTON_NEXT].height = 53;
  s_arrButton[GUI_BUTTON_NEXT].text = "";
  s_arrButton[GUI_BUTTON_NEXT].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_NEXT].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_NEXT]);

  //创建拍照按钮
  InitButtonWidgetStruct(&s_arrButton[GUI_BUTTON_PHOTO]);
  s_arrButton[GUI_BUTTON_PHOTO].x0 = 183;
  s_arrButton[GUI_BUTTON_PHOTO].y0 = 622;
  s_arrButton[GUI_BUTTON_PHOTO].width = 115;
  s_arrButton[GUI_BUTTON_PHOTO].height = 115;
  s_arrButton[GUI_BUTTON_PHOTO].text = "";
  s_arrButton[GUI_BUTTON_PHOTO].pressImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].releaseImageType = GUI_IMAGE_TYPE_CLEAR;
  s_arrButton[GUI_BUTTON_PHOTO].pressCallback = ButtonPressCallback;
  CreateButtonWidget(&s_arrButton[GUI_BUTTON_PHOTO]);
}

/*********************************************************************************************************
* 函数名称：ScanGUIButton
* 函数功能：按键扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：每隔20毫秒执行一次
*********************************************************************************************************/
EnumGUIButtons ScanGUIButton(void)
{
  EnumGUIButtons i;

  for(i = (EnumGUIButtons)0; i < GUI_BUTTON_MAX; i++)
  {
    s_enumButtonPress = GUI_BUTTON_NONE;
    ScanButtonWidget(&s_arrButton[i]);

    if(GUI_BUTTON_NONE != s_enumButtonPress)
    {
      return s_enumButtonPress;
    }
  }

  return GUI_BUTTON_NONE;
}

