/*********************************************************************************************************
* ģ�����ƣ�IICCommon.h
* ժ    Ҫ��ͨ��IICģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2018��01��01�� 
* ��    �ݣ�
* ע    �⣺                                                                  
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/
#ifndef _IIC_COMMON_H_
#define _IIC_COMMON_H_

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define IIC_COMMON_ACK  0
#define IIC_COMMON_NACK 1

#define IIC_COMMON_INPUT  0
#define IIC_COMMON_OUTPUT 1

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//ͨ��IIC�豸�ṹ��
typedef struct 
{
  u8   deviceID;                  //�豸��ַ
  void (*SetSCL)(u8 state);       //ʱ���ź���SCL����
  void (*SetSDA)(u8 state);       //�����ź���SDA����
  u8   (*GetSDA)(void);           //��ȡSDA�����ƽ
  void (*ConfigSDAMode)(u8 mode); //����SDA�������
  void (*Delay)(u8 time);         //��ʱ����
}StructIICCommonDev;

/*********************************************************************************************************
*                                              API��������
*********************************************************************************************************/
void IICCommonStart(StructIICCommonDev *dev);                                            //IIC������ʼ�ź�
void IICCommonEnd(StructIICCommonDev *dev);                                              //IIC������ֹ�ź�
void IICCommonSendAck(StructIICCommonDev *dev);                                          //IIC����ACK
void IICCommonSendNAck(StructIICCommonDev *dev);                                         //IIC����NACK
u8   IICCommonReadACK(StructIICCommonDev *dev);                                          //IIC��ȡAck
u8   IICCommonSendOneByte(StructIICCommonDev *dev, u8 data);                             //IIC����1�ֽ�����
u8   IICCommonReadOneByte(StructIICCommonDev *dev, u8 type);                             //IIC����1�ֽ�����
u8   IICCommonWriteBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len);            //IIC����д
u8   IICCommonReadBytes(StructIICCommonDev *dev, u8 addr, u8 *buf, u32 len, u8 type);    //IIC������
u8   IICCommonWriteBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len);         //IIC����д
u8   IICCommonReadBytesEx(StructIICCommonDev *dev, u16 addr, u8 *buf, u32 len, u8 type); //IIC������

#endif
