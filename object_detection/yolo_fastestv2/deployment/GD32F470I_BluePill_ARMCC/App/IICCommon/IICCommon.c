/*********************************************************************************************************
* 模块名称：IICCommon.c
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

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "IICCommon.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量定义
*********************************************************************************************************/

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
* 函数名称：IICCommonStart
* 函数功能：IIC发送起始信号
* 输入参数：dev：IIC设备 
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：闲时SCL拉高，SDA为输入状态
*********************************************************************************************************/
void IICCommonStart(StructIICCommonDev *dev)
{
  //拉低时钟线，做预先准备
  dev->SetSCL(0);
  dev->Delay(1);

  //将SDA设为输出，并且拉高
  dev->ConfigSDAMode(IIC_COMMON_OUTPUT);
  dev->SetSDA(1);
  dev->Delay(1);

  //拉高SCL，为起始信号做准备
  dev->SetSCL(1);
  dev->Delay(1);

  //SCL为高电平时拉低SDA表示起始信号
  dev->SetSDA(0);
  dev->Delay(1);
  
  //拉低时钟信号
  dev->SetSCL(0);
  dev->Delay(1);
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
}

/*********************************************************************************************************
* 函数名称：IICCommonEnd
* 函数功能：IIC发送终止信号
* 输入参数：dev：IIC设备 
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
void IICCommonEnd(StructIICCommonDev *dev)
{
  //将SDA配置成输出状态，并且拉低
  dev->ConfigSDAMode(IIC_COMMON_OUTPUT);
  dev->SetSDA(0);
  dev->Delay(1);

  //拉高SCL，为输出终止信号做准备
  dev->SetSCL(1);
  dev->Delay(1);

  //SCL为高电平时拉高SDA表示终止信号
  dev->SetSDA(1);
  dev->Delay(1);
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
}

/*********************************************************************************************************
* 函数名称：IICCommonSendAck
* 函数功能：IIC发送ACK信号
* 输入参数：dev：IIC设备 
* 输出参数：void
* 返 回 值：ack
* 创建日期：2018年01月01日
* 注    意：调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
void IICCommonSendAck(StructIICCommonDev *dev)
{
  //将SDA配置成输出状态，并且拉低
  dev->ConfigSDAMode(IIC_COMMON_OUTPUT);
  dev->SetSDA(0);
  dev->Delay(1);

  //拉高SCL，输出ACK
  dev->SetSCL(1);
  dev->Delay(1);

  //拉低SCL，发送结束
  dev->SetSCL(0);
  dev->Delay(1);
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
}

/*********************************************************************************************************
* 函数名称：IICCommonSendNAck
* 函数功能：IIC发送NACK信号
* 输入参数：dev：IIC设备 
* 输出参数：void
* 返 回 值：ack
* 创建日期：2018年01月01日
* 注    意：调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
void IICCommonSendNAck(StructIICCommonDev *dev)
{
  //将SDA配置成输出状态，并且拉高
  dev->ConfigSDAMode(IIC_COMMON_OUTPUT);
  dev->SetSDA(1);
  dev->Delay(1);

  //拉高SCL，输出NACK
  dev->SetSCL(1);
  dev->Delay(1);

  //拉低SCL，发送结束
  dev->SetSCL(0);
  dev->Delay(1);
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
}

/*********************************************************************************************************
* 函数名称：IICCommonReadACK
* 函数功能：IIC接收Slave的ACK信号
* 输入参数：dev：IIC设备 
* 输出参数：void
* 返 回 值：ack
* 创建日期：2018年01月01日
* 注    意：调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
u8 IICCommonReadACK(StructIICCommonDev *dev)
{
  u8 ack = 1;

  //将SDA设为输入状态，准备读取ACK
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
  dev->Delay(1);
  
  //拉高SCL
  dev->SetSCL(1);
  dev->Delay(1);

  //读取ACK
  ack = dev->GetSDA();

  //拉低SCL，接收结束
  dev->SetSCL(0);
  dev->Delay(1);

  return ack;
}

/*********************************************************************************************************
* 函数名称：IICCommonSendOneByte
* 函数功能：IIC发送1字节数据
* 输入参数：dev：IIC设备，data：需要发送的字节 
* 输出参数：void
* 返 回 值：1-发送失败，0-发送成功
* 创建日期：2018年01月01日
* 注    意：先发送高字节，后发送低字节
*          调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
u8 IICCommonSendOneByte(StructIICCommonDev *dev, u8 data)
{
  u8 i = 0;
  u8 ack = 1;

  dev->ConfigSDAMode(IIC_COMMON_OUTPUT);
  for(i = 0; i < 8; i++)
  {
    //设置发送数据
    if(data & 0x80)
    {
      dev->SetSDA(1);
    }
    else
    {
      dev->SetSDA(0);
    }
    data = data << 1;
    
    //延时1个时钟单位
    dev->Delay(1);

    //拉高时钟信号线，发送数据
    dev->SetSCL(1);

    //延时1个时钟单位
    dev->Delay(1);

    //拉低时钟信号线，准备发送下一位
    dev->SetSCL(0);

    //延时1个时钟单位
    dev->Delay(1);
  }
  
  //接收Slave的ACK
  ack = IICCommonReadACK(dev);
  return ack;
}

/*********************************************************************************************************
* 函数名称：IICCommonReadOneByte
* 函数功能：IIC接收1字节数据
* 输入参数：dev：IIC设备，type：0-发送ACK，1-发送NACK
* 输出参数：void
* 返 回 值：1字节数据
* 创建日期：2018年01月01日
* 注    意：先接收高字节，后接收低字节
*          调用函数前SCL处于低电平状态至少1个时钟单位
*********************************************************************************************************/
u8 IICCommonReadOneByte(StructIICCommonDev *dev, u8 type)
{
  u8 i = 0;
  u8 data = 0;

  //接收数据
  dev->ConfigSDAMode(IIC_COMMON_INPUT);
  for(i = 0; i < 8; i++)
  {
    //拉高时钟信号线，准备接收数据
    dev->SetSCL(1);
    
    //延时1个时钟单位
    dev->Delay(1);

    //接收数据
    data = (data << 1) | dev->GetSDA();
    
    //拉低时钟信号线，准备接收下一位
    dev->SetSCL(0);
    
    //延时1个时钟单位
    dev->Delay(1);
  }

  //发送应答信号
  if(IIC_COMMON_ACK == type)
  {
    IICCommonSendAck(dev);
  }
  else
  {
    IICCommonSendNAck(dev);
  }

  return data;
}

/*********************************************************************************************************
* 函数名称：IICCommonWriteBytes
* 函数功能：IIC连续写
* 输入参数：dev：IIC设备，addr：写入地址，buf：数据缓冲区，len：数据量
* 输出参数：void
* 返 回 值：0-成功，其它-出错
* 创建日期：2018年01月01日
* 注    意：先发送低地址数据，再发送高地址数据
*********************************************************************************************************/
u8 IICCommonWriteBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len)
{
  //发送起始信号
  IICCommonStart(dev);
 
  //发送写命令，要确保最低位为0
  if(1 == IICCommonSendOneByte(dev, dev->deviceID & ~(1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送写入地址
  if(1 == IICCommonSendOneByte(dev, addr))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //连续写入
  while(len--)
  {
    if(1 == IICCommonSendOneByte(dev, *buf++))
    {
      IICCommonEnd(dev);
      return 1;
    }
  }

  //完成写入
  IICCommonEnd(dev);
  return 0;
}

/*********************************************************************************************************
* 函数名称：IICommonCReadBytes
* 函数功能：IIC连续读
* 输入参数：dev：IIC设备，addr：写入地址，buf：数据缓冲区，len：数据量，type：0-最后发送ACK，1-NACK
* 输出参数：void
* 返 回 值：0-成功，其它-出错
* 创建日期：2018年01月01日
* 注    意：读取时先写入低地址，再写入高地址
*          读取最后一个字节有时需要发送NACK
*********************************************************************************************************/
u8 IICCommonReadBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len, u8 type)
{
  //发送写命令，要确保最低位为0
  IICCommonStart(dev);
  if(1 == IICCommonSendOneByte(dev, dev->deviceID & ~(1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送读取地址
  if(1 == IICCommonSendOneByte(dev, addr))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送结束命令
  IICCommonEnd(dev);

  //发送读命令，要确保最低位为1
  IICCommonStart(dev);
  if(1 == IICCommonSendOneByte(dev, dev->deviceID | (1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //连续读取
  while(len)
  {
    if(1 == len)
    {
      *buf = IICCommonReadOneByte(dev, type);
    }
    else
    {
      *buf = IICCommonReadOneByte(dev, IIC_COMMON_ACK);
    }
    buf++;
    len--;
  }

  //完成写入
  IICCommonEnd(dev);
  return 0;
}

/*********************************************************************************************************
* 函数名称：IICCommonWriteBytesEx
* 函数功能：IIC连续写，写入地址为16位
* 输入参数：dev：IIC设备，addr：写入地址，buf：数据缓冲区，len：数据量
* 输出参数：void
* 返 回 值：0-成功，其它-出错
* 创建日期：2018年01月01日
* 注    意：先发送低地址数据，再发送高地址数据
*********************************************************************************************************/
u8 IICCommonWriteBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len)
{
  //发送起始信号
  IICCommonStart(dev);
 
  //发送写命令，要确保最低位为0
  if(1 == IICCommonSendOneByte(dev, dev->deviceID & ~(1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送写入地址高字节
  if(1 == IICCommonSendOneByte(dev, addr >> 8))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送写入地址低字节
  if(1 == IICCommonSendOneByte(dev, addr & 0xFF))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //连续写入
  while(len--)
  {
    if(1 == IICCommonSendOneByte(dev, *buf++))
    {
      IICCommonEnd(dev);
      return 1;
    }
  }

  //完成写入
  IICCommonEnd(dev);
  return 0;
}

/*********************************************************************************************************
* 函数名称：IICCommonReadBytesEx
* 函数功能：IIC连续读，读取地址为16位
* 输入参数：dev：IIC设备，addr：写入地址，buf：数据缓冲区，len：数据量，type：0-最后发送ACK，1-NACK
* 输出参数：void
* 返 回 值：0-成功，其它-出错
* 创建日期：2018年01月01日
* 注    意：读取时先写入低地址，再写入高地址
*          读取最后一个字节有时需要发送NACK
*********************************************************************************************************/
u8 IICCommonReadBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len, u8 type)
{
  //发送写命令，要确保最低位为0
  IICCommonStart(dev);
  if(1 == IICCommonSendOneByte(dev, dev->deviceID & ~(1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送读取地址高字节
  if(1 == IICCommonSendOneByte(dev, addr >> 8))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送读取地址低字节
  if(1 == IICCommonSendOneByte(dev, addr & 0xFF))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //发送结束命令
  IICCommonEnd(dev);

  //发送读命令，要确保最低位为1
  IICCommonStart(dev);
  if(1 == IICCommonSendOneByte(dev, dev->deviceID | (1 << 0)))
  {
    IICCommonEnd(dev);
    return 1;
  }

  //连续读取
  while(len)
  {
    if(1 == len)
    {
      *buf = IICCommonReadOneByte(dev, type);
    }
    else
    {
      *buf = IICCommonReadOneByte(dev, IIC_COMMON_ACK);
    }
    buf++;
    len--;
  }

  //完成写入
  IICCommonEnd(dev);
  return 0;
}
