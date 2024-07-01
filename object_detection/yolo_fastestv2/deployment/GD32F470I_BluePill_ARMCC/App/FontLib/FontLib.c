/*********************************************************************************************************
* ģ�����ƣ�FontLib.c
* ժ    Ҫ���ֿ����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
* ��    �ݣ�
* ע    �⣺
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
#include "FontLib.h"
#include "GD25QXX.h"
#include "ff.h"
#include "Malloc.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define FONT_FIL_BUF_SIZE (1024 * 4) //�ֿ��ļ���������С��4k�ֽڣ�

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static u8 CheckFontLib(void); //У���ֿ�

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CheckFontLib
* �������ܣ�У���ֿ�
* ���������void
* ���������void
* �� �� ֵ��0-У���ֿ�ɹ�������-�ֿ��𻵣���Ҫ���¸����ֿ�
* �������ڣ�2021��07��01��
* ע    �⣺printf������'\r'�������ã�ԭ��δ֪
*********************************************************************************************************/
static u8 CheckFontLib(void)
{
  static FIL s_filFont;  //�ֿ��ļ������Ǿ�̬������
  FRESULT    result;     //�ļ��������ر���
  u8*        fileBuf;    //�ֿ⻺�������ļ�ϵͳ�е��ֿ��ļ����ɶ�̬�ڴ����
  u8*        flashBuf;   //�ֿ⻺������Flash�е��ֿ��ļ����ɶ�̬�ڴ����
  u32        readNum;    //ʵ�ʶ�ȡ���ļ�����
  u32        ReadAddr;   //����SPI Flash��ַ
  u32        i;          //ѭ������
  u32        progress;   //����

  //���붯̬�ڴ�
  fileBuf  = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  flashBuf = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  if((NULL == fileBuf) || (NULL == flashBuf))
  {
    MyFree(SDRAMEX, fileBuf);
    MyFree(SDRAMEX, flashBuf);
    printf("CheckFontLib�����붯̬�ڴ�ʧ��\r\n");
    return 0;
  }

  //���ļ�
  result = f_open(&s_filFont, "0:/font/GBK24.FON", FA_OPEN_EXISTING | FA_READ);
  if (result !=  FR_OK)
  {
    //�ͷ��ڴ�
    MyFree(SDRAMEX, fileBuf);
    MyFree(SDRAMEX, flashBuf);

    //��ӡ������Ϣ
    printf("CheckFontLib�����ֿ��ļ�ʧ��\r\n");
    return 0;
  }

  //��ȡ���ݲ���һ�Ƚ������ֿ⣬������һ����ͬ���ʾ�ֿ���
  printf("CheckFontLib����ʼУ���ֿ�\r\n");
  ReadAddr = 0;
  progress = 0;
  while(1)
  {
    //�������
    if((100 * s_filFont.fptr / s_filFont.fsize) >= (progress + 15))
    {
      progress = 100 * s_filFont.fptr / s_filFont.fsize;
      printf("CheckFontLib��У����ȣ�%%%d\r\n", progress);
    }

    //���ļ��ж�ȡ���ݵ�������
    result = f_read(&s_filFont, fileBuf, FONT_FIL_BUF_SIZE, &readNum);
    if (result !=  FR_OK)
    {
      //�ر��ļ�
      f_close(&s_filFont);

      //�ͷ��ڴ�
      MyFree(SDRAMEX, fileBuf);
      MyFree(SDRAMEX, flashBuf);

      //��ӡ������Ϣ
      printf("CheckFontLib����ȡ����ʧ��\r\n");
      return 0;
    }

    //��SPI Flash�ж�ȡ����
    GD25Q16Read(flashBuf, readNum, ReadAddr);

    //��һ�Ƚ�
    for(i = 0; i < readNum; i++)
    {
      //�����ֿ���
      if(flashBuf[i] != fileBuf[i])
      {
        //�ر��ļ�
        f_close(&s_filFont);

        //�ͷ��ڴ�
        MyFree(SDRAMEX, fileBuf);
        MyFree(SDRAMEX, flashBuf);
        return 1;
      }
    }

    //���¶�ȡ��ַ
    ReadAddr = ReadAddr + readNum;

    //�ж��ļ��Ƿ��д���
    if((s_filFont.fptr >= s_filFont.fsize) || (FONT_FIL_BUF_SIZE != readNum))
    {
      printf("CheckFontLib��У����ȣ�%%100\r\n");
      break;
    }
  }

  //�ر��ļ�
  f_close(&s_filFont);

  //�ͷ��ڴ�
  MyFree(SDRAMEX, fileBuf);
  MyFree(SDRAMEX, flashBuf);

  return 0;
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitFontLib
* �������ܣ���ʼ���ֿ����ģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺�Զ��������ֿ⣬�����ֿ��������SD���и����ֿ�
*********************************************************************************************************/
void InitFontLib(void)
{
  //��ʼ��SPI Flash
  InitGD25QXX();

  //�����ֿ�
  if(0 != CheckFontLib())
  {
    printf("FontLib���ֿ��𻵣���Ҫ�����ֿ�!!!\r\n");
    UpdataFontLib();
  }
  else
  {
    printf("FontLib���ֿ�У��ɹ�\r\n");
  }
}

/*********************************************************************************************************
* �������ƣ�UpdataFontLib
* �������ܣ������ֿ�
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1���ֿ�·����0:/font/GBK24.FON
*          2��ʹ������SRAM������ļ�ϵͳ��ȡ����ʧ�ܣ�Ӧ������DMA�����йأ�SD������ʹ�õ���DMA�������ݣ�
*          3��printf������'\r'�������ã�ԭ��δ֪
*********************************************************************************************************/
void UpdataFontLib(void)
{
  static FIL s_filFont;  //�ֿ��ļ������Ǿ�̬������
  FRESULT    result;     //�ļ��������ر���
  u8*        fontBuf;    //�ֿ⻺�������ɶ�̬�ڴ����
  u32        readNum;    //ʵ�ʶ�ȡ���ļ�����
  u32        writeAddr;  //д��SPI Flash��ַ
  u32        progress;   //����

  //����4k�ڴ�
  fontBuf = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  if(NULL == fontBuf)
  {
    printf("UpdataFontLib�����붯̬�ڴ�ʧ��\r\n");
    return;
  }

  //���ļ�
  result = f_open(&s_filFont, "0:/font/GBK24.FON", FA_OPEN_EXISTING | FA_READ);
  if (result !=  FR_OK)
  {
    //�ͷ��ڴ�
    MyFree(SDRAMEX, fontBuf);

    //��ӡ������Ϣ
    printf("UpdataFontLib�����ֿ��ļ�ʧ��\r\n");
    return;
  }

  //�����ζ�ȡ���ݲ�д��SPI Flash��
  printf("UpdataFontLib����ʼ�����ֿ�\r\n");
  writeAddr = 0;
  progress = 0;
  while(1)
  {
    //�������
    if((100 * s_filFont.fptr / s_filFont.fsize) >= (progress + 15))
    {
      progress = 100 * s_filFont.fptr / s_filFont.fsize;
      printf("UpdataFontLib�����½��ȣ�%%%d\r\n", progress);
    }

    //���ļ��ж�ȡ���ݵ�������
    result = f_read(&s_filFont, fontBuf, FONT_FIL_BUF_SIZE, &readNum);
    if (result !=  FR_OK)
    {
      //�ر��ļ�
      f_close(&s_filFont);

      //�ͷ��ڴ�
      MyFree(SDRAMEX, fontBuf);

      //��ӡ������Ϣ
      printf("UpdataFontLib����ȡ����ʧ��\r\n");
      return;
    }

    //���ֿ�����д��SPI Flash
    GD25Q16Write(fontBuf, readNum, writeAddr);

    //����д���ַ
    writeAddr = writeAddr + readNum;

    //�ж��ļ��Ƿ��д���
    if((s_filFont.fptr >= s_filFont.fsize) || (FONT_FIL_BUF_SIZE != readNum))
    {
      printf("UpdataFontLib�����½��ȣ�%%100\r\n");
      printf("UpdataFontLib�������ֿ����\r\n");
      break;
    }
  }

  //�ر��ļ�
  f_close(&s_filFont);

  //�ͷ��ڴ�
  MyFree(SDRAMEX, fontBuf);

  //���³ɹ���ʾ
  printf("UpdataFontLib�������ֿ�ɹ�\r\n");
}

/*********************************************************************************************************
* �������ƣ�GetCNFont24x24
* �������ܣ���ȡ24x24���ֵ�������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��24x24���ֵ������ݹ̶�Ϊ72�ֽ�
*          2��ʹ��GBK��
*          3��ɨ�跽ʽ���ϵ��¡��������ң��ֽڸ�λ��ǰ����λ�ں�
*********************************************************************************************************/
void GetCNFont24x24(u32 code, u8* buf)
{
  u8  gbkH, gbkL; //GBK���λ����λ
  u32 addr;       //����������SPI Flash�еĵ�ַ
  u32 i;          //ѭ������

  //���GBK���λ����λ
  gbkH = code >> 8;
  gbkL = code & 0xFF;

  //У���λ
  if((gbkH < 0x81) || (gbkH > 0xFE))
  {
    for(i = 0; i < 72; i++)
    {
      buf[i] = 0;
    }
    return;
  }

  //��λ����0x40~0x7E��Χ
  if((gbkL >= 0x40) && (gbkL <= 0x7E))
  {
    addr = ((gbkH - 0x81) * 190 + (gbkL - 0x40)) * 72;
    GD25Q16Read(buf, 72, addr);
  }

  //��λ����0x80~0xFE��Χ
  else if((gbkL >= 0x80) && (gbkL <= 0xFE))
  {
    addr = ((gbkH - 0x81) * 190 + (gbkL - 0x41)) * 72;
    GD25Q16Read(buf, 72, addr);
  }

  //����
  else
  {
    for(i = 0; i < 72; i++)
    {
      buf[i] = 0;
    }
  }
}
