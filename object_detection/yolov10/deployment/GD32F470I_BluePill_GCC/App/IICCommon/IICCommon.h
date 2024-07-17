/*********************************************************************************************************
* 模块名称：IICCommon.h
* 摘    要：通用IIC模块
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2018年01月01日 
* 内    容：
* 注    意：                                                                  
**********************************************************************************************************
* 取代版本：
* 作    者：
* 完成日期：
* 修改内容：
* 修改文件：
*********************************************************************************************************/
#ifndef _IIC_COMMON_H_
#define _IIC_COMMON_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define IIC_COMMON_ACK  0
#define IIC_COMMON_NACK 1

#define IIC_COMMON_INPUT  0
#define IIC_COMMON_OUTPUT 1

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//通用IIC设备结构体
typedef struct 
{
  u8   deviceID;                  //设备地址
  void (*SetSCL)(u8 state);       //时钟信号线SCL控制
  void (*SetSDA)(u8 state);       //数据信号线SDA控制
  u8   (*GetSDA)(void);           //获取SDA输入电平
  void (*ConfigSDAMode)(u8 mode); //配置SDA输入输出
  void (*Delay)(u8 time);         //延时函数
}StructIICCommonDev;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void IICCommonStart(StructIICCommonDev *dev);                                            //IIC发送起始信号
void IICCommonEnd(StructIICCommonDev *dev);                                              //IIC发送终止信号
void IICCommonSendAck(StructIICCommonDev *dev);                                          //IIC发送ACK
void IICCommonSendNAck(StructIICCommonDev *dev);                                         //IIC发送NACK
u8   IICCommonReadACK(StructIICCommonDev *dev);                                          //IIC读取Ack
u8   IICCommonSendOneByte(StructIICCommonDev *dev, u8 data);                             //IIC发送1字节数据
u8   IICCommonReadOneByte(StructIICCommonDev *dev, u8 type);                             //IIC接收1字节数据
u8   IICCommonWriteBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len);            //IIC连续写
u8   IICCommonReadBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len, u8 type);    //IIC连续读
u8   IICCommonWriteBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len);         //IIC连续写
u8   IICCommonReadBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len, u8 type); //IIC连续读

#endif
