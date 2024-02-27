/*********************************************************************************************************
* ģ�����ƣ�BMPEncoder.c
* ժ    Ҫ��BMP����ģ��
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
#include "BMPEncoder.h"
#include "ff.h"
#include "Malloc.h"
#include "stdio.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void CheckDir(char* dir);                             //У��Ŀ���Ƿ���ڣ������������½���Ŀ¼
static void GetNewName(char* dir, char* prefix, char* name); //����µ��ļ���

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�CheckDir
* �������ܣ�У��Ŀ���Ƿ���ڣ������������½���Ŀ¼
* ���������dir��Ŀ��·��������"/"
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void CheckDir(char* dir)
{
  DIR     recDir; //Ŀ��·��
  FRESULT result; //�ļ��������ر���

  result = f_opendir(&recDir, dir);
  if(FR_NO_PATH == result)
  {
    f_mkdir(dir);
  }
  else
  {
    f_closedir(&recDir);
  }
}

/*********************************************************************************************************
* �������ƣ�GetNewName
* �������ܣ�����µ��ļ���
* ���������dir��ָ��·��������"/"��prefix���ļ���ǰ׺��name�����ֻ���������������·��
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺���ļ�ʱFIL�����Ǿ�̬������̬�ڴ������ڴ�
*********************************************************************************************************/
static void GetNewName(char* dir, char* prefix, char* name)
{
  FIL*    recFile; //�����ļ�
  FRESULT result;  //�ļ��������ر���
  u32     index;   //¼���ļ�����

  //ΪFIL�����ڴ�
  recFile = MyMalloc(SRAMIN, sizeof(FIL));
  if(NULL == recFile)
  {
    printf("GetNewRecName�������ڴ�ʧ��\r\n");
  }

  index = 0;
  while(index < 0xFFFFFFFF)
  {
    //�����µ�����
    sprintf((char*)name, "%s/%s%d.bmp", dir, prefix, index);

    //��鵱ǰ�ļ��Ƿ��Ѿ����ڣ������ܳɹ�����˵���ļ��Ѵ��ڣ�
    result = f_open(recFile, (const TCHAR*)name, FA_OPEN_EXISTING | FA_READ);
    if(FR_NO_FILE == result)
    {
      break;
    }
    else
    {
      f_close(recFile);
    }

    index++;
  }

  //�ͷ��ڴ�
  MyFree(SRAMIN, recFile);
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�BMPEncodeWithRGB565
* �������ܣ���Ļ��ͼ������BMP�ļ���ָ��λ��
* ���������x0����ʼ�����꣨���ص㣩
*          y0����ʼ�����꣨���ص㣩
*          width����ȣ����ص㣩
*          height���߶ȣ����ص㣩
*          path������·����һ��Ϊ"0:/photo"����·�����������Զ�������·��������"/"
*          prefix���ļ���ǰ׺�����ջ�����"xxx01.bmp"λͼ�ļ����ļ��Զ����
* ���������void
* �� �� ֵ��0-�ɹ���1-�����ڴ�ʧ�ܣ�2-�����ļ�ʧ�ܣ�3-д��ͷ�ļ���Ϣʧ�ܣ�4-д����������ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��λͼ�̶�����RGB565��ʽ����
*          2��ÿ����һ���������ݾ�д��FatFsһ��
*          3��λͼÿ�е����ݣ��ֽڣ�����Ҫ��4�ı���
*          4��FatFsÿ��д��������������4�ֽڶ��룬�����´�д����ͬ�ļ����Ῠ��������BMP�ļ�ͷ��Ϣ������4�ֽڶ���
*********************************************************************************************************/
u32 BMPEncodeWithRGB565(u32 x0, u32 y0, u32 width, u32 height, const char* path, const char* prefix)
{
  FIL*           bmpFile;  //λͼ�ļ�
  u16*           imageBuf; //���ݻ�����
  char*          bmpName;  //λͼ�ļ�������·����
  StructBMPInfo* bmpInfo;  //BMP�ļ�ͷ��Ϣ
  FRESULT        result;   //�ļ��������ر���
  u32            writeNum; //�ɹ�д��������
  u32            dataSize; //��������С���ֽڣ�
  u32            row;      //��4�ֽڶ��������
  u32            i, x, y;  //ѭ������
  u32            ret;      //����ֵ

  //Ԥ�践��ֵ
  ret = 0;

  //�����ڴ�
  bmpFile  = MyMalloc(SRAMIN, sizeof(FIL));
  imageBuf = MyMalloc(SRAMIN, BMP_IMAGE_BUF_SIZE * 2);
  bmpName  = MyMalloc(SRAMIN, BMP_NAME_LEN_MAX);
  bmpInfo  = MyMalloc(SRAMIN, sizeof(StructBMPInfo));
  if((NULL == bmpFile) || (NULL == imageBuf) || (NULL == bmpName))
  {
    printf("BMPEncode�����붯̬�ڴ�ʧ��\r\n");
    ret = 0;
    goto BMP_ENCODE_EXIT_MARK;
  }

  //У��·������·�����������½�·��
  CheckDir((char*)path);

  //�����µ��ļ���
  GetNewName((char*)path, (char*)prefix, bmpName);

  //����λͼ�ļ�
  result = f_open(bmpFile, (const TCHAR*)bmpName, FA_CREATE_ALWAYS | FA_WRITE);
  if(FR_OK != result)
  {
    printf("BMPEncode�������ļ�ʧ��\r\n");
    ret = 2;
    goto BMP_ENCODE_EXIT_MARK;
  }
  
  //������������С
  row = width;
  while(0 != (row % 2))
  {
    row++;
  }
  dataSize = row * height * 2;
  
  //�����ļ�ͷ
  bmpInfo->bmFileHeader.bfType      = 0x4D42;                           //BM��ʽ��־
  bmpInfo->bmFileHeader.bfSize      = dataSize + sizeof(StructBMPInfo); //����BMP��С
  bmpInfo->bmFileHeader.bfReserved1 = 0;                                //������1
  bmpInfo->bmFileHeader.bfReserved2 = 0;                                //������2
  bmpInfo->bmFileHeader.bfOffBits   = sizeof(StructBMPInfo);            //����������ƫ��

  //������Ϣͷ
  bmpInfo->bmInfoHeader.biSize          = sizeof(StructBMPInfoHeader); //��Ϣͷ��С
  bmpInfo->bmInfoHeader.biWidth         = width;                       //BMP�Ŀ��
  bmpInfo->bmInfoHeader.biHeight        = height;                      //BMP�ĸ߶�
  bmpInfo->bmInfoHeader.biPlanes        = 1;                           //���豸˵����ɫƽ�������ܱ�����Ϊ1
  bmpInfo->bmInfoHeader.biBitCount      = 16;                          //bmpΪ16λɫbmp
  bmpInfo->bmInfoHeader.biCompression   = BI_BITFIELDS;                //ÿ�����صı�����ָ�����������
  bmpInfo->bmInfoHeader.biSizeImage     = dataSize;                    //BMP��������С
  bmpInfo->bmInfoHeader.biXPelsPerMeter = 8600;                        //ˮƽ�ֱ��ʣ�����/��
  bmpInfo->bmInfoHeader.biYPelsPerMeter = 8600;                        //��ֱ�ֱ��ʣ�����/��
  bmpInfo->bmInfoHeader.biClrUsed       = 0;                           //û��ʹ�õ���ɫ��
  bmpInfo->bmInfoHeader.biClrImportant  = 0;                           //û��ʹ�õ���ɫ��

  //RGB����
  bmpInfo->rgbMask[0] = 0x00F800; //��ɫ����
  bmpInfo->rgbMask[1] = 0x0007E0; //��ɫ����
  bmpInfo->rgbMask[2] = 0x00001F; //��ɫ����
  
  //������
  bmpInfo->reserved1 = 0;
  bmpInfo->reserved2 = 0;

  //���ļ���дָ��ƫ�Ƶ���ʼ��ַ
  f_lseek(bmpFile, 0);

  //д��BMPͷ�ļ���Ϣ
  result = f_write(bmpFile, (const void*)bmpInfo, sizeof(StructBMPInfo), &writeNum);
  if((FR_OK != result) || (writeNum != sizeof(StructBMPInfo)))
  {
    printf("BMPEncode��д��ͷ�ļ���Ϣʧ��\r\n");
    f_close(bmpFile);
    ret = 3;
    goto BMP_ENCODE_EXIT_MARK;
  }

  //��ȡ��Ļ���ݲ����浽�ļ���
  y = y0 + height - 1;
  while(1)
  {
    //����һ�����ݵ���������ÿ����һ�оʹ����ļ�һ��
    i = 0;
    for(x = x0; x < (x0 + width); x++)
    {
      imageBuf[i++] = LCDReadPoint(x, y);
    }

    //4�ֽڶ��룬��0����λ
    while(0 != (i % 2))
    {
      imageBuf[i++] = 0x0000;
    }

    //����һ�����ݵ��ļ���
    result = f_write(bmpFile, (const void*)imageBuf, i * 2, &writeNum);
    if((FR_OK != result) && (writeNum != i * 2))
    {
      printf("BMPEncode��д����������ʧ��\r\n");
      f_close(bmpFile);
      ret = 4;
      goto BMP_ENCODE_EXIT_MARK;
    }

    //�ж��Ƿ����
    if(y0 == y)
    {
      break;
    }
    else
    {
      y = y - 1;
    }
  }

  //�ر��ļ�
  f_close(bmpFile);
  printf("BMPEncode���ɹ�����ͼ��%s\r\n", bmpName);
  
//��������λ�ã�����ǰҪ�ͷŶ�̬�ڴ�
BMP_ENCODE_EXIT_MARK:
  MyFree(SRAMIN, bmpFile);
  MyFree(SRAMIN, imageBuf);
  MyFree(SRAMIN, bmpName);
  MyFree(SRAMIN, bmpInfo);
  return ret;
}

/*********************************************************************************************************
* ��������: ScreeShot
* ��������: ��ͼ
* �������: void
* �������: void
* �� �� ֵ: void
* ��������: 2019��08��01��
* ע    ��:
*********************************************************************************************************/
void ScreeShot(void)
{
  BMPEncodeWithRGB565(0, 0, 
  g_structTLILCDDev.width[g_structTLILCDDev.currentLayer], 
  g_structTLILCDDev.height[g_structTLILCDDev.currentLayer], 
  "0:/photo",
  "ScreeShot");
}
