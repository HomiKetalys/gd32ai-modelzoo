/*********************************************************************************************************
* 模块名称：GUIWave.h
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
#ifndef _GUI_WAVE_H_
#define _GUI_WAVE_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void CreateGUIWave(void); //创建波形显示模块
void DeleteGUIWave(void); //删除波形显示模块
void GUIWavePoll(void);   //波形显示模块轮询任务

#endif
