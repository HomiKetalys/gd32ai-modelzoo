/*********************************************************************************************************
* 模块名称：GUIWave.c
* 摘    要：波形显示模块
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
#include "GUIWave.h"
#include "TLILCD.h"
#include "GUIPlatform.h"
#include "GraphWidget.h"
#include "BMP.h"
#include "Common.h"
#include "ADC.h"
#include "DAC.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructGraphObj s_structGraph;

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CreateGUIWave
* 函数功能：创建波形显示模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void CreateGUIWave(void)
{
  //LCD横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //绘制背景
  DisPlayBMPByIPA(0, 0, MAIN_WINDOWN_BACKGROUND_DIR);

  //创建波形控件
  InitGraphWidgetStruct(&s_structGraph);
  CreateGraphWidget(10, 150, 780, 200, &s_structGraph);
  s_structGraph.startDraw = 1;

  //初始化ADC
  InitADC();

  //初始化DAC
  InitDAC();
}

/*********************************************************************************************************
* 函数名称：DeleteGUIWave
* 函数功能：删除波形显示模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeleteGUIWave(void)
{
  //注销DAC模块
  DeInitDAC();
}

/*********************************************************************************************************
* 函数名称：GUIWavePoll
* 函数功能：波形显示模块轮询任务
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GUIWavePoll(void)
{
  int wave;
  wave = GetWaveADC() * (s_structGraph.y1 - s_structGraph.y0) / 4095;
  GraphWidgetAddData(&s_structGraph, wave);
}
