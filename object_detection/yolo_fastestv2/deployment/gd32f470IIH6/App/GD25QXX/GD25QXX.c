/*********************************************************************************************************
* ģ�����ƣ�GD25QXX.c
* ժ    Ҫ��GD25QXXģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
* ��    �ݣ�
* ע    �⣺ע�⹴ѡOptions for Target 'Target1'->Code Generation->Use MicroLIB������printf�޷�ʹ��                                                                  
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "GD25QXX.h"
#include "gd32f470x_conf.h"
#include "SPI.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define SET_SPI_NSS()       gpio_bit_set(GPIOC, GPIO_PIN_0)
#define CLR_SPI_NSS()       gpio_bit_reset(GPIOC, GPIO_PIN_0)

#define W25Q16_PAGE_SIZE    256                            //һҳ�Ĵ�С��256�ֽ�
#define W25Q16_SECTOR_SIZE (4 * 1024)                      //������С���ֽ�
#define W25Q16_BLOCK_SIZE  (16 * W25Q16_SECTOR_SIZE)
#define W25Q16_SIZE        (32 * W25Q16_BLOCK_SIZE)

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
const unsigned char byDUMMY = 0xff;

//SPI Flash��д������
static unsigned char s_arrWriteBuf[W25Q16_SECTOR_SIZE];

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void GD25Q16WriteEnable(void);
static void GD25Q16WaitForWriteEnd(void);
static void GD25Q16PageProgram(unsigned char *buf, unsigned short len, unsigned int addr);

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�GD25Q16WriteEnable
* �������ܣ�дʹ��
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void GD25Q16WriteEnable(void)
{
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x06);
  SET_SPI_NSS();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16WaitForWriteEnd
* �������ܣ��ȴ�FLASH�ڲ�ʱ��������
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void GD25Q16WaitForWriteEnd(void)
{
  unsigned char status = 0;
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x05);
  
  do
  {
    status = SPI4ReadWriteByte(byDUMMY);
  }
  while((status & 0x01) == 1);
  
  SET_SPI_NSS();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16PageProgram
* �������ܣ�ҳд 
* ���������buf-Ҫд������,len-���ݳ���,addr-��ʼ��ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void GD25Q16PageProgram(unsigned char *buf, unsigned short len, unsigned int addr)
{
  GD25Q16WriteEnable();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x02);
  SPI4ReadWriteByte((addr & 0xFF0000) >> 16);
  SPI4ReadWriteByte((addr & 0x00FF00) >> 8);
  SPI4ReadWriteByte(addr & 0xFF);
  
  while (len--)
  {
    SPI4ReadWriteByte(*buf);
    buf++;
  }
  
  SET_SPI_NSS();
  GD25Q16WaitForWriteEnd();
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitGD25QXX
* �������ܣ�GD25QXXģ���ʼ��
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitGD25QXX(void)
{
  //����PE2
  //ʹ��RCC���ʱ��
  rcu_periph_clock_enable(RCU_GPIOC);  //ʹ��GPIOA��ʱ��
  gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);         //�������
  gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0); //�������
  gpio_bit_set(GPIOC, GPIO_PIN_0);                                              //ȡ��Ƭѡ

  //��ʼ��SPI4
  InitSPI4();
}

/*********************************************************************************************************
* �������ƣ�W25q64_Write
* �������ܣ����ݴ洢 
* ���������buf-Ҫд������,len-���ݳ���,addr-��ʼ��ַ 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GD25Q16Write(unsigned char *buf, unsigned int len, unsigned int addr)
{
  unsigned int   writeAddr;  //��ǰд���ַ
  unsigned int   dataRemain; //��ǰʣ��������
  unsigned char* dataBuf;    //д�����ݻ������׵�ַ
  unsigned int   sectorAddr; //��ǰ�����׵�ַ
  unsigned int   i, j;       //ѭ������

  //���ֵ
  writeAddr  = addr; //����д���ַ
  dataRemain = len;  //����ʣ��������
  dataBuf    = buf;  //�������ݻ������׵�ַ

  //ѭ��д�룬ÿ�β�д����������
  while(0 != dataRemain)
  {
    //���㵱ǰ�����׵�ַ
    sectorAddr = writeAddr - (writeAddr % W25Q16_SECTOR_SIZE);

    //д���ַ���������׵�ַ����Ҫ����ǰ���������ݶ��������޸ĺ���д��ȥ
    if(0 != writeAddr % W25Q16_SECTOR_SIZE)
    {
      //����һ������������
      GD25Q16Read(s_arrWriteBuf, W25Q16_SECTOR_SIZE, sectorAddr);
    }

    //д���ַΪ�����׵�ַ��д��������һ��������ҲҪ����ǰ���������ݶ��������޸ĺ���д��ȥ
    else if((0 == writeAddr % W25Q16_SECTOR_SIZE) && (dataRemain < W25Q16_SECTOR_SIZE))
    {
      //����һ������������
      GD25Q16Read(s_arrWriteBuf, W25Q16_SECTOR_SIZE, sectorAddr);
    }

    //����һ��������
    GD25Q16X4KErase(sectorAddr);

    //��ҳ�޸�һ������������
    for(i = (writeAddr % W25Q16_SECTOR_SIZE) / W25Q16_PAGE_SIZE; i < (W25Q16_SECTOR_SIZE / W25Q16_PAGE_SIZE); i++)
    {
      //������ȫ��д�룬������ѭ��
      if(0 == dataRemain)
      {
        break;
      }

      //�޸Ļ�������ĳһҳ������
      for(j = writeAddr % W25Q16_PAGE_SIZE; j < W25Q16_PAGE_SIZE; j++)
      {
        //������ȫ��д�룬������ѭ��
        if(0 == dataRemain)
        {
          break;
        }

        //�޸Ļ������е�����
        s_arrWriteBuf[i * W25Q16_PAGE_SIZE + j] = *dataBuf;

        //����д���ַ��ʣ���������Լ����ݻ������׵�ַ
        writeAddr = writeAddr + 1;
        dataRemain = dataRemain - 1;
        dataBuf = dataBuf + 1;
      }
    }

    //�޸ĺ�д��SPI Flash����ҳд
    for(i = 0; i < (W25Q16_SECTOR_SIZE / W25Q16_PAGE_SIZE); i++)
    {
      GD25Q16PageProgram(s_arrWriteBuf + i * W25Q16_PAGE_SIZE, W25Q16_PAGE_SIZE, sectorAddr + i * W25Q16_PAGE_SIZE);
    }
  }
}

/*********************************************************************************************************
* �������ƣ�GD25Q16Read
* �������ܣ�������
* ���������buf-���ݻ�����,len-����,addr-��ʼ��ַ 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GD25Q16Read(unsigned char *buf, unsigned int len, unsigned int addr)
{
  unsigned int i;
  
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x03);
  SPI4ReadWriteByte((addr & 0xFF0000) >> 16);
  SPI4ReadWriteByte((addr & 0x00FF00) >> 8);
  SPI4ReadWriteByte(addr & 0xFF);
  
  for (i = 0; i < len; i++)
  {
    buf[i] = SPI4ReadWriteByte(byDUMMY);
  }
  
  SET_SPI_NSS();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16ReadStatus
* �������ܣ���״̬�Ĵ��� 
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
unsigned short GD25Q16ReadStatus(void)
{
  unsigned short status=0;
  
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x05);
  status = SPI4ReadWriteByte(byDUMMY);
  SET_SPI_NSS();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x35);
  status |= SPI4ReadWriteByte(byDUMMY) << 8;
  SET_SPI_NSS();
  
  return status;
}

/*********************************************************************************************************
* �������ƣ�GD25Q16ReadDeviceID
* �������ܣ���оƬID
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
unsigned short GD25Q16ReadDeviceID(void)
{
  unsigned char DeviceID = 0;
  
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0xAB);
  SPI4ReadWriteByte(byDUMMY);
  SPI4ReadWriteByte(byDUMMY);
  SPI4ReadWriteByte(byDUMMY);
  DeviceID = SPI4ReadWriteByte(0X00);
  SET_SPI_NSS();
  
  return DeviceID;
}

/*********************************************************************************************************
* �������ƣ�GD25Q16ReadJEDEID
* �������ܣ���оƬID 
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
unsigned short GD25Q16ReadJEDEID(void)
{
  unsigned short DeviceID = 0;
  
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x9F);
  SPI4ReadWriteByte(byDUMMY);
  DeviceID = SPI4ReadWriteByte(byDUMMY) << 8;
  DeviceID |= SPI4ReadWriteByte(byDUMMY);
  SET_SPI_NSS();
  
  return DeviceID;
}

/*********************************************************************************************************
* �������ƣ�GD25Q16X4KErase
* �������ܣ�4KƬ����
* ���������addr-��ʼ��ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void  GD25Q16X4KErase(unsigned int addr)
{
  GD25Q16WriteEnable();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x20);
  SPI4ReadWriteByte((addr & 0xFF0000) >> 16);
  SPI4ReadWriteByte((addr & 0x00FF00) >> 8);
  SPI4ReadWriteByte(addr & 0xFF);
  SET_SPI_NSS();
  GD25Q16WaitForWriteEnd();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16X32KErase
* �������ܣ�32KƬ����
* ���������addr-��ʼ��ַ 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void  GD25Q16X32KErase(unsigned int addr)
{
  GD25Q16WriteEnable();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x52);
  SPI4ReadWriteByte((addr & 0xFF0000) >> 16);
  SPI4ReadWriteByte((addr & 0x00FF00) >> 8);
  SPI4ReadWriteByte(addr & 0xFF);
  SET_SPI_NSS();
  GD25Q16WaitForWriteEnd();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16X64KErase
* �������ܣ�64KƬ���� 
* ���������addr-��ʼ��ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void  GD25Q16X64KErase(unsigned int addr)
{
  GD25Q16WriteEnable();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0xD8);
  SPI4ReadWriteByte((addr & 0xFF0000) >> 16);
  SPI4ReadWriteByte((addr & 0x00FF00) >> 8);
  SPI4ReadWriteByte(addr & 0xFF);
  SET_SPI_NSS();
  GD25Q16WaitForWriteEnd();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16EraseChip
* �������ܣ�ȫƬ���� 
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GD25Q16EraseChip(void)
{
  GD25Q16WriteEnable();
  CLR_SPI_NSS();
  SPI4ReadWriteByte(0x60);
  SET_SPI_NSS();
  GD25Q16WaitForWriteEnd();
}

/*********************************************************************************************************
* �������ƣ�GD25Q16Test
* �������ܣ�SPI Flash����
* ���������void
* ���������void
* �� �� ֵ��0-���Գɹ���1-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺�˺������������оƬ�ڵ�����
*********************************************************************************************************/
u32 GD25Q16Test(void)
{
  u32 i;          //ѭ������
  u8  data[1024]; //��ʱ������

  //ȫƬ����
  printf("GD25Q16XXTest: Start erase chio\r\n");
  GD25Q16EraseChip();
  printf("GD25Q16XXTest: Erase chip finish\r\n");

  //У��
  GD25Q16Read(data, 1024, 0);
  for(i = 0; i < 1024; i++)
  {
    if(0xFF != data[i])
    {
      printf("GD25Q16XXTest: Fail to check 1\r\n");
      return 1;
    }
  }
  printf("GD25Q16XXTest: Check 1 OK\r\n");

  //ȫд0
  for(i = 0; i < 1024; i++)
  {
    data[i] = 0;
  }
  GD25Q16Write(data, 1024, 0);

  //У��
  GD25Q16Read(data, 1024, 0);
  for(i = 0; i < 1024; i++)
  {
    if(0x00 != data[i])
    {
      printf("GD25Q16XXTest: Fail to check 0\r\n");
      return 1;
    }
  }
  printf("GD25Q16XXTest: Check 0 OK\r\n");

  //У��ɹ�
  printf("GD25Q16XXTest: Successfully!!!\r\n");
  return 0;
}
