/*********************************************************************************************************
* 模块名称：BookByteRead.h
* 摘    要：按字节从文本文件中读取数据模块
* 当前版本：1.0.0
* 作    者：Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* 完成日期：2021年07月01日 
* 内    容：
* 注    意：SD卡读写是以数据块的形式进行，每次读写512个字节，为了提高SD卡读写效率，需要用专门的模块控制SD卡读写
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _BOOK_BYTE_READ_H_
#define _BOOK_BYTE_READ_H_

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
u32 ReadBookByte(char* byte, u32* visi); //读取一个字节
u32 GetBytePosition(void);               //获取当前字节在文本文件中的位置
u32 SetPosision(u32 posision);           //设置读取位置
u32 GetBookSize(void);                   //获取书本大小

#endif
