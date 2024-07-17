/*********************************************************************************************************
* 模块名称：UART0.h
* 摘    要：串口模块，包括串口模块初始化，以及中断服务函数处理，以及读写串口函数实现
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日 
* 内    容：
* 注    意：                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _UART0_H_
#define _UART0_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define UART0_BUF_SIZE 512           //设置缓冲区的大小

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void InitUART0(u32 bound);          //初始化UART0模块
u32  WriteUART0(u8 *pBuf, u32 len); //写串口，返回已写入数据的个数
u32  ReadUART0(u8 *pBuf, u32 len);  //读串口，返回读到数据的个数

#endif
