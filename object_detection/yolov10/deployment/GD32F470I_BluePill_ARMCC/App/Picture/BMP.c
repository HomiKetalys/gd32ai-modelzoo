/*********************************************************************************************************
* ģ�����ƣ�BMP.c
* ժ    Ҫ��λͼ��ʾģ��
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
#include "BMP.h"
#include "BMPImage.h"
#include "TLILCD.h"
#include "stdio.h"
#include "ff.h"
#include "Malloc.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/
//λͼ = �ļ�ͷ + ��Ϣͷ + ��ɫ��(��ѡ) + ������

//BMP�ļ�ͷ
typedef __packed struct
{
  u16 type;      //�ļ���־��ֻΪ'BM'��������BM��ʮ������������0x424D��
  u32 size;      //�ļ���С����λΪ�ֽ�
  u16 reserved1; //����������Ϊ0
  u16 reserved2; //����������Ϊ0
  u32 offBits;   //λͼ���ݵ���ʼλ��������ļ�ͷ��ƫ��������λΪ�ֽڡ�
}StructBmpFileHeader;

//BMP��Ϣͷ
typedef __packed struct
{
  u32 infoSize;       //�ýṹ��������ֽ���
  u32 width;          //λͼ�Ŀ�ȣ����ص�λ��
  u32 height;         //λͼ�ĸ߶ȣ����ص�λ��
  u16 planes;         //Ŀ���豸�ļ��𣬱���Ϊ1
  u16 colorSize;      //ÿ�����������λ��
  u32 compression;    //˵��ͼ������ѹ�������ͣ�λͼѹ�����ͣ�������0����ѹ����
  u32 imageSize;      //λͼ�Ĵ�С(���а�����Ϊ�˲���������4�ı�������ӵĿ��ֽ�)�����ֽ�Ϊ��λ
  u32 xPelsPerMeter;  //λͼˮƽ�ֱ��ʣ�ÿ��������
  u32 yPelsPerMeter;  //λͼ��ֱ�ֱ��ʣ�ÿ��������
  u32 colorUsed;      //λͼʵ��ʹ�õ���ɫ���е���ɫ��
  u32 colorImportant; //λͼ��ʾ��������Ҫ����ɫ��������� 0����ʾ����Ҫ
}StructBmpInfoHeader;

//��ɫ��
typedef __packed struct
{
  u8 blue;     //ָ����ɫǿ��
  u8 green;    //ָ����ɫǿ��
  u8 red;      //ָ����ɫǿ��
  u8 reserved; //����������Ϊ0
}StructRGB;

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
static FIL s_fBmpFile;     //BMP�ļ�
static u8* s_arrBmpBuffer; //�ļ���ȡ��ʱ������

static StructBmpFileHeader s_structFileHeader;
static StructBmpInfoHeader s_structInfoHeader;

static FATFS s_arrFatFs[2];  //0-Nand Flash, 1-SD Card

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ForceMountNandFlash(void);															//ǿ�й���Nand Flash
static void ForceMountSDCard(void);																	//ǿ�й���SD��
static u8   GetHeaderInMem(const unsigned char *image);              //��ȡ�ļ�ͷ����Ϣͷ
static u8*  GetLineInMem(const unsigned char *image, u32 line);      //��ȡһ������
static void BMPDisplayInMem(StructBmpImage* image, u32 x, u32 y);    //��ȡ�ڴ��е�λͼ����ʾ
static u16  CalcAlphaRGB555(u16 backColor, u16 foreColor, u8 alpha); //͸���ȵ��Ӽ���
static u8   GetHeaderInFatFs(void);                                  //��ȡ�ļ�ͷ����Ϣͷ
static u8*  GetLineInFatFs(u32 line);                                //��ȡһ������
static void BMPDisplayInFatFs(StructBmpImage* image, u32 x, u32 y);  //��ȡ�ļ�ϵͳ�е�λͼ����ʾ

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ForceMountNandFlash
* �������ܣ�ǿ�й���Nand Flash
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��Nand Flash��δ�����ļ�ϵͳ��Ῠ��
*********************************************************************************************************/
static void ForceMountNandFlash(void)
{
  FRESULT result;
  while(1)
  {
    result = f_mount(&s_arrFatFs[0], FS_VOLUME_NAND, 1);
    if(FR_OK != result)
    {
      printf("ForceMountNandFlash: Mount Nand Flash fail!!!\r\n");
      printf("ForceMountNandFlash: retry...\r\n");
      DelayNms(500);
    }
    else
    {
      break;
    }
  }
  printf("ForceMountNandFlash: Mount Nand Flash success\r\n");
}
/*********************************************************************************************************
* �������ƣ�ForceMountSDCard
* �������ܣ�ǿ�й���SD��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��SD��δ������Ῠ���ڸú���
*********************************************************************************************************/
 static void ForceMountSDCard(void)
{
  FRESULT result;
  while(1)
  {
    result = f_mount(&s_arrFatFs[1], FS_VOLUME_SD, 1);
    if(FR_OK != result)
    {
      printf("ForceMountSDCard: Mount SD card fail!!!\r\n");
      printf("ForceMountSDCard: retry...\r\n");
      DelayNms(500);
    }
    else
    {
      break;
    }
  }
  printf("ForceMountSDCard: Mount SD card success\r\n");
}
/*********************************************************************************************************
* �������ƣ�GetHeaderInMem
* �������ܣ���ȡ�ļ�ͷ����Ϣͷ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 GetHeaderInMem(const unsigned char *image)
{ 
  u32 i;
  u8* buf;
  
  //��ȡBMP�ļ�ͷ
  buf = (u8*)image;
  for(i = 0; i < sizeof(StructBmpFileHeader); i++)
  {
    *((u8*)(&s_structFileHeader) + i) = buf[i];
  }

  //��ȡ��Ϣͷ
  buf = (u8*)image + sizeof(StructBmpFileHeader);
  for(i = 0; i < sizeof(StructBmpInfoHeader); i++)
  {
    *((u8*)(&s_structInfoHeader) + i) = buf[i];
  }
  
  return 0;
}

/*********************************************************************************************************
* �������ƣ�GetLineInMem
* �������ܣ���ȡһ������
* ���������line���кţ���0��ʼ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8* GetLineInMem(const unsigned char *image, u32 line)
{
  u32  readAddr;  //��ȡ��ַ�������ʼ��ַƫ����
  u32  lineSize;  //һ����������Ҫ4�ֽڶ���
  u8*  p;         //���ص�ַ
  
  //����32λͼƬһ�����ݴ�С
  if(32 == s_structInfoHeader.colorSize)
  {
    lineSize = s_structInfoHeader.width * 4;
  }
  
  //����24λͼƬһ�����ݴ�С
  else if(24 == s_structInfoHeader.colorSize)
  {
    //������������4�ı���
    lineSize = s_structInfoHeader.width * 3;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //����16λͼƬһ�����ݴ�С
  else if(16 == s_structInfoHeader.colorSize)
  {
    //������������4�ı���
    lineSize = s_structInfoHeader.width * 2;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //����������Ӧ�ĵ�ַ
  readAddr = s_structFileHeader.offBits + lineSize * line;

  //�õ�����λ��
  p = (u8*)image + readAddr;

  return p;
}

/*********************************************************************************************************
* �������ƣ�BMPDisplayInMem
* �������ܣ�ָ��λ����ʾλͼ
* ���������fileName���ļ�������������·����x�������ꣻy��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1����������Ͻǣ�λͼ�Ǵ������Ҵ��µ��ϵģ��ӵ�����һ�п�ʼ��ģ�
*          2��Ŀǰ��֧��32λ��24λ��ʽλͼ��ʾ
*          3��Ϊ�˼ӿ�λͼ��ʾ��͸���Ƚ�֧����ʾ�Ͳ���ʾ��͸���ȴ���0����ʾ�õ㣬͸���ȵ���0����ʾ
*********************************************************************************************************/
static void BMPDisplayInMem(StructBmpImage* image, u32 x, u32 y)
{
  u32     x0, y0, x1, y1; //��㡢�յ�
  u32     byteCnt;        //���ζ�ȡ�ֽڼ���
  u8      red;            //��ɫͨ����ֵ
  u8      green;          //��ɫͨ����ֵ
  u8      blue;           //��ɫͨ����ֵ
  u8      alpha;          //͸����
  u32     color;          //������ɫֵ
  u32     lineCnt;        //��ȡ��������
  u8*     line;           //�������׵�ַ
  u32     yn;             //ʵ��д��������
  u32     backColor;      //������ɫ��͸���ȵ���ʱʹ�ã�
  u32     alphaColor;     //͸���ȵ��ӽ��
  
  //��ȡBMP�ļ�ͷ����Ϣͷ
  GetHeaderInMem((const unsigned char*)image->addr);

  //У�顰BM��
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("BMPDisplay: this is not a BMP file\r\n");
    return;
  }

  //��֧��32λ��24��16λ��ʽλͼ
  if(!((32 == s_structInfoHeader.colorSize) || (24 == s_structInfoHeader.colorSize) || (16 == s_structInfoHeader.colorSize)))
  {
    printf("BMPDisplay: unsupported format!!!\r\n");
    return;
  }

  //�趨��ʾ��Χ
  x0 = x;
  y0 = y;
  x1 = x0 + s_structInfoHeader.width;
  y1 = y0 + s_structInfoHeader.height;
  if(x1 >= g_structTLILCDDev.width[g_structTLILCDDev.currentLayer])
  {
    x1 = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  }
  if(y1 >= g_structTLILCDDev.height[g_structTLILCDDev.currentLayer])
  {
    y1 = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
  }

  //λͼ�Ǵ������ң��������ϴ����
  yn = y1 - 1;

  //������������
  lineCnt = 0;

  //������ѭ��
  for(y = y0; y < y1; y++)
  {
    //��ȡһ��������
    line = GetLineInMem((const unsigned char*)image->addr, lineCnt);

    //�ֽڼ�������
    byteCnt = 0;

    //����������һ
    lineCnt++;

    //������ѭ��
    for(x = x0; x < x1; x++)
    {
      //32λ���ص�
      if(32 == s_structInfoHeader.colorSize)
      {
        //��ȡ����ֵ
        blue  = line[byteCnt + 0] >> 3; //��ɫ
        green = line[byteCnt + 1] >> 2; //��ɫ
        red   = line[byteCnt + 2] >> 3; //��ɫ
        alpha = line[byteCnt + 3];      //͸����
        
        //24λRGBת16λRGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //�ֽڼ�������
        byteCnt = byteCnt + 4;

        //͸���ȵ���
        if(BMP_UES_ALPHA == image->alphaType)
        {
          //���뱳����ɫ
          backColor = LCDReadPoint(x, yn);

          //͸���ȵ���
          alphaColor = CalcAlphaRGB555(backColor, color, alpha);

          //����
          LCDDrawPoint(x, yn, alphaColor);
        }

        //���˵�͸���Ƚϵ͵����ص㲻��ʾ
        else if(BMP_GATE_ALPHA == image->alphaType)
        {
          if(alpha >= image->alphaGate)
          {
            LCDDrawPoint(x, yn, color);
          }
       }

        //������͸���ȣ��������ص����ʾ
        else if(BMP_NO_ALPHA == image->alphaType)
        {
          LCDDrawPoint(x, yn, color);
        }
      }
      
      //24λ���ص�
      else if(24 == s_structInfoHeader.colorSize)
      {
        //��ȡ����ֵ
        blue  = line[byteCnt + 0] >> 3; //��ɫ
        green = line[byteCnt + 1] >> 2; //��ɫ
        red   = line[byteCnt + 2] >> 3; //��ɫ
        
        //24λRGBת16λRGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //�ֽڼ�������
        byteCnt = byteCnt + 3;

        //����
        LCDDrawPoint(x, yn, color);
      }

      //16λ���ص�
      else if(16 == s_structInfoHeader.colorSize)
      {
        color = (line[byteCnt + 1] << 8) | line[byteCnt + 0];

        //�ֽڼ�������
        byteCnt = byteCnt + 2;

        //����
        LCDDrawPoint(x, yn, color);
      }
    }

    //�������������ɨ��
    yn = yn - 1;
  }
}

/*********************************************************************************************************
* �������ƣ�CalcAlphaRGB555
* �������ܣ�͸���ȵ���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺͸���ȷ�Χ0-255
*********************************************************************************************************/
static u16 CalcAlphaRGB555(u16 backColor, u16 foreColor, u8 alpha)
{
  u16 backR, backG, backB, foreR, foreG, foreB, colorAlpha;
  u16 resultR, resultG, resultB;
  u16 result;

  //͸����̫С��ֱ�ӷ��ر���ɫ
  if(alpha < 5)
  {
    return backColor;
  }

  //��ǰ����RGBͨ������
  backR = ((backColor >> 11) & 0x1F);
  backG = ((backColor >> 5 ) & 0x3F);
  backB = ((backColor >> 0 ) & 0x1F);

  //��ȡǰ��RGBͨ������
  foreR = ((foreColor >> 11) & 0x1F);
  foreG = ((foreColor >> 5 ) & 0x3F);
  foreB = ((foreColor >> 0 ) & 0x1F);

  //��ȡ͸����
  colorAlpha = alpha;

  //RGBͨ��͸���ȵ���
  resultR = (foreR * colorAlpha + backR * (0xFF - colorAlpha)) / 0xFF;
  resultG = (foreG * colorAlpha + backG * (0xFF - colorAlpha)) / 0xFF;
  resultB = (foreB * colorAlpha + backB * (0xFF - colorAlpha)) / 0xFF;

  //��ϳ�RGB565��ʽ
  result = (((u16)resultR) << 11) | (((u16)resultG) << 5) | (((u16)resultB) << 0);

  return result;
}

/*********************************************************************************************************
* �������ƣ�GetHeaderInFatFs
* �������ܣ���ȡ�ļ�ͷ����Ϣͷ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static u8 GetHeaderInFatFs(void)
{ 
  u32 i;
  u8* buf;
  
  //��ȡBMP�ļ�ͷ
  buf = s_arrBmpBuffer;
  for(i = 0; i < sizeof(StructBmpFileHeader); i++)
  {
    *((u8*)(&s_structFileHeader) + i) = buf[i];
  }

  //��ȡ��Ϣͷ
  buf = s_arrBmpBuffer + sizeof(StructBmpFileHeader);
  for(i = 0; i < sizeof(StructBmpInfoHeader); i++)
  {
    *((u8*)(&s_structInfoHeader) + i) = buf[i];
  }

  return 0;
}

/*********************************************************************************************************
* �������ƣ�GetLineInFatFs
* �������ܣ���ȡһ������
* ���������line���кţ���0��ʼ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��SD���ж�ȡ����ʱ��ȡ��ַ����4����������һ������������4�ı�������
*********************************************************************************************************/
static u8* GetLineInFatFs(u32 line)
{
  u32  readAddr;  //��ȡ��ַ�������ʼ��ַƫ����
  u32  lineSize;  //һ����������Ҫ4�ֽڶ���
  u8*  p;         //���ص�ַ
  
  //����32λͼƬһ�����ݴ�С
  if(32 == s_structInfoHeader.colorSize)
  {
    lineSize = s_structInfoHeader.width * 4;
  }
  
  //����24λͼƬһ�����ݴ�С
  else if(24 == s_structInfoHeader.colorSize)
  {
    //������������4�ı���
    lineSize = s_structInfoHeader.width * 3;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //����16λͼƬһ�����ݴ�С
  else if(16 == s_structInfoHeader.colorSize)
  {
    //������������4�ı���
    lineSize = s_structInfoHeader.width * 2;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //����������Ӧ�ĵ�ַ
  readAddr = s_structFileHeader.offBits + lineSize * line;

  //�õ�����λ��
  p = (u8*)s_arrBmpBuffer + readAddr;

  return p;
}

/*********************************************************************************************************
* �������ƣ�BMPDisplayInFatFs
* �������ܣ���ȡ�ļ�ϵͳ�е�λͼ����ʾ
* ���������image���ļ�������������·����x�������ꣻy��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1����������Ͻǣ�λͼ�Ǵ������Ҵ��µ��ϵģ��ӵ�����һ�п�ʼ��ģ�
*          2��Ŀǰ��֧��32λ��24λ��ʽλͼ��ʾ
*          3��Ϊ�˼ӿ�λͼ��ʾ��͸���Ƚ�֧����ʾ�Ͳ���ʾ��͸���ȴ���0����ʾ�õ㣬͸���ȵ���0����ʾ
*********************************************************************************************************/
static void BMPDisplayInFatFs(StructBmpImage* image, u32 x, u32 y)
{
  FRESULT fresult;
  u32     x0, y0, x1, y1; //��㡢�յ�
  u32     byteCnt;        //���ζ�ȡ�ֽڼ���
  u8      red;            //��ɫͨ����ֵ
  u8      green;          //��ɫͨ����ֵ
  u8      blue;           //��ɫͨ����ֵ
  u8      alpha;          //͸����
  u32     color;          //������ɫֵ
  u32     lineCnt;        //��ȡ��������
  u8*     line;           //�������׵�ַ
  u32     yn;             //ʵ��д��������
  u32     backColor;      //������ɫ��͸���ȵ���ʱʹ�ã�
  u32     alphaColor;     //͸���ȵ��ӽ��
  u32     errorFlag;      //�����־λ
  u32     readNum;        //ʵ�ʶ�ȡ���ļ�����
  
  errorFlag = 0;
  s_arrBmpBuffer = NULL;

//���³��Խڵ�
DISPLAY_IN_FATFS_RETRY_MARK:

  //�ͷ��ڴ�
  if(NULL != s_arrBmpBuffer)
  {
    MyFree(SDRAMEX, s_arrBmpBuffer);
  }

  //���¹����ļ�ϵͳ
  if(0 != errorFlag)
  {
    errorFlag = 0;
    if('0' == *((u8*)image->addr + 0))
    {
      ForceMountSDCard();
    }
    else if('1' == *((u8*)image->addr + 0))
    {
      ForceMountNandFlash();
    }
    else
    {
      return;
    }
  }

  //���ļ�
  fresult = f_open(&s_fBmpFile, (const TCHAR*)image->addr, FA_READ);
  if(FR_OK != fresult)
  {
    printf("BMPDisplayInFatFs��Fail: Fail to open file %s\r\n", (const TCHAR*)image->addr);
    errorFlag = 1;
    goto DISPLAY_IN_FATFS_RETRY_MARK;
  }

  //ΪͼƬ�����������ڴ�
  s_arrBmpBuffer = MyMalloc(SDRAMEX, s_fBmpFile.fsize);
  if(NULL == s_arrBmpBuffer)
  {
    printf("BMPDisplayInFatFs��Fail to malloc for image\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //��ȡ����BMP�ļ�
  fresult = f_read(&s_fBmpFile, s_arrBmpBuffer, s_fBmpFile.fsize, &readNum);
  if((FR_OK != fresult) || (readNum != s_fBmpFile.fsize))
  {
    printf("BMPDisplayInFatFs��Fail: fail to read data from %s\r\n", (char*)image->addr);
    printf("BMPDisplayInFatFs��Fail: retry...\r\n");
    f_close(&s_fBmpFile);
    errorFlag = 1;
    goto DISPLAY_IN_FATFS_RETRY_MARK;
  }
  
  //��ȡBMP�ļ�ͷ����Ϣͷ
  GetHeaderInFatFs();

  //У�顰BM��
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("BMPDisplay: this is not a BMP file\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //��֧��32λ��24��16λ��ʽλͼ
  if(!((32 == s_structInfoHeader.colorSize) || (24 == s_structInfoHeader.colorSize) || (16 == s_structInfoHeader.colorSize)))
  {
    printf("BMPDisplay: unsupported format!!!\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //�趨��ʾ��Χ
  x0 = x;
  y0 = y;
  x1 = x0 + s_structInfoHeader.width;
  y1 = y0 + s_structInfoHeader.height;
  if(x1 >= g_structTLILCDDev.width[g_structTLILCDDev.currentLayer])
  {
    x1 = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  }
  if(y1 >= g_structTLILCDDev.height[g_structTLILCDDev.currentLayer])
  {
    y1 = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
  }

  //λͼ�Ǵ������ң��������ϴ����
  yn = y1 - 1;

  //������������
  lineCnt = 0;

  //������ѭ��
  for(y = y0; y < y1; y++)
  {
    //��ȡһ��������
    line = GetLineInFatFs(lineCnt);

    //�ֽڼ�������
    byteCnt = 0;

    //����������һ
    lineCnt++;

    //������ѭ��
    for(x = x0; x < x1; x++)
    {
      //32λ���ص�
      if(32 == s_structInfoHeader.colorSize)
      {
        //��ȡ����ֵ
        blue  = line[byteCnt + 0] >> 3; //��ɫ
        green = line[byteCnt + 1] >> 2; //��ɫ
        red   = line[byteCnt + 2] >> 3; //��ɫ
        alpha = line[byteCnt + 3];      //͸����
        
        //24λRGBת16λRGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //�ֽڼ�������
        byteCnt = byteCnt + 4;

        //͸���ȵ���
        if(BMP_UES_ALPHA == image->alphaType)
        {
          //���뱳����ɫ
          backColor = LCDReadPoint(x, yn);

          //͸���ȵ���
          alphaColor = CalcAlphaRGB555(backColor, color, alpha);

          //����
          LCDDrawPoint(x, yn, alphaColor);
        }

        //���˵�͸���Ƚϵ͵����ص㲻��ʾ
        else if(BMP_GATE_ALPHA == image->alphaType)
        {
          if(alpha >= image->alphaGate)
          {
            LCDDrawPoint(x, yn, color);
          }
        }

        //������͸���ȣ��������ص����ʾ
        else if(BMP_NO_ALPHA == image->alphaType)
        {
          LCDDrawPoint(x, yn, color);
        }
      }
      
      //24λ���ص�
      else if(24 == s_structInfoHeader.colorSize)
      {
        //��ȡ����ֵ
        blue  = line[byteCnt + 0] >> 3; //��ɫ
        green = line[byteCnt + 1] >> 2; //��ɫ
        red   = line[byteCnt + 2] >> 3; //��ɫ
        
        //24λRGBת16λRGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //�ֽڼ�������
        byteCnt = byteCnt + 3;

        //����
        LCDDrawPoint(x, yn, color);
      }

      //16λ���ص�
      else if(16 == s_structInfoHeader.colorSize)
      {
        color = (line[byteCnt + 1] << 8) | line[byteCnt + 0];

        //�ֽڼ�������
        byteCnt = byteCnt + 2;

        //����
        LCDDrawPoint(x, yn, color);
      }
    }

    //�������������ɨ��
    yn = yn - 1;
  }
  
//�˳��ڵ�
DISPLAY_IN_FATFS_EXIT_MARK:

  //�ر��ļ�
  f_close(&s_fBmpFile);

  //�ͷ��ڴ�
  MyFree(SDRAMEX, s_arrBmpBuffer);
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitBMP
* �������ܣ���ʼ��λͼ��ʾģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitBMP(void)
{
  
}

/*********************************************************************************************************
* �������ƣ�DisplayBMP
* �������ܣ�ָ��λ����ʾλͼ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DisplayBMP(StructBmpImage* image, u32 x, u32 y)
{
  //ͼƬ�������ڴ���
  if(BMP_IN_MEM == image->storageType)
  {
    BMPDisplayInMem(image, x, y);
  }
  
  //ͼƬ�������ļ�ϵͳ��
  else if(BMP_IN_FATFS == image->storageType)
  {
    BMPDisplayInFatFs(image, x, y);
  }
}

/*********************************************************************************************************
* �������ƣ�DisPlayBMPByIPA
* �������ܣ�ʹ��IPAͼ�������������λͼ������ʵ����ˢλͼ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��ͼƬ����Ϊ�����������Ҫ��ʾ����ͼƬ������Ҫ�ڵ��Խ�ͼƬ����������
*          2������ʱͼƬҪ���·�ת��������ʾʱ���᾵��
*          3���˺����������ʾ�����Ƿ񳬳���Ļ��Χ
*          4����ʽת��ʱ��Щ���ص�Ứ����Ӧ����SDRAM�ٶȲ���
*          5��Ĭ��ֻ��SD���ڵ�ͼƬ
*          6��ֻ�ʺ�����ˢ����ͼƬ��һЩ��ͼ
*********************************************************************************************************/
void DisPlayBMPByIPA(u32 x, u32 y, char* addr)
{
  //�ļ���ر���
  FIL*    file;    //λͼ�ļ�
  FRESULT result;  //�ļ��������ر���
  u8*     readBuf; //��ȡ���������ɶ�̬�ڴ����
  u32     readNum; //ʵ�ʶ�ȡ���ļ�����

  //IPA��ر���
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //ǰ��ͼ������ṹ��
  ipa_destination_parameter_struct ipa_destination_init_struct; //Ŀ��ͼ������ṹ��
  u32 destStartAddr;         //Ŀ��ͼ����ʼ��ַ
  u32 phyWidth, phyHeight;   //������Ⱥ͸߶�
  u32 phyX, phyY;            //����ʱ�����ʼ��ַ
  u32 lineOff;               //Ŀ����ƫ�ƣ����һ�����غ���һ�е�һ������֮���������Ŀ
  u32 swap;                  //��������ʱ����ʱ����
  u32 foregroundPixelFormat; //ǰ�����ص��ʽ
  u32 foregroundLineoff;     //ǰ����ƫ����

  //λͼ�ļ�ͷ����Ϣͷ
  StructBmpFileHeader* fileHeader;
  StructBmpInfoHeader* infoHeader;
  u32 width;
  u32 height;

  //�����־λ
  u32 error;

  //����ֵ
  file = NULL;
  readBuf = NULL;
  error = 0;

  //Ϊλͼ�ļ������ڴ�
  file = MyMalloc(SDRAMEX, sizeof(FIL));
  if(NULL == file)
  {
    printf("DisPlayBMPByIPA: fail to malloc for FIL\r\n");
    return;
  }


//���´��ļ�λ��
DISPLAY_BMP_IPA_RETRY_FILE_MARK:

  //�������¹����ļ�ϵͳ
  if(0 != error)
  {
    error = 0;
    if('0' == addr[0])
    {
      ForceMountSDCard();
    }
    else if('1' == addr[0])
    {
      ForceMountNandFlash();
    }
    else
    {
      return;
    }
  }

  //�ͷ��ϴ�������ڴ�
  if(NULL != readBuf)
  {
    MyFree(SDRAMEX, readBuf);
  }

  //��λͼ�ļ�
  result = f_open(file, (const TCHAR*)addr, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK)
  {
    printf("DisPlayBMPByIPA: fail to open file %s \r\n", addr);
    printf("DisPlayBMPByIPA: retry...\r\n");
    error = 1;
    goto DISPLAY_BMP_IPA_RETRY_FILE_MARK;
  }

  //��ȡ�������ڴ�����
  readBuf = MyMalloc(SDRAMEX, file->fsize);
  if(NULL == readBuf)
  {
    printf("DisPlayBMPByIPA: fail to malloc for BMP image\r\n");
    goto DISPLAY_BMP_IPA_EXIT_MARK;
  }

  //��ȡ����
  result = f_read(file, readBuf, file->fsize, &readNum);
  if((FR_OK != result) || (readNum != file->fsize))
  {
    printf("DisPlayBMPByIPA: fail to read data from %s\r\n", addr);
    printf("DisPlayBMPByIPA: retry...\r\n");
    f_close(file);
    error = 1;
    goto DISPLAY_BMP_IPA_RETRY_FILE_MARK;
  }

  //��ȡ�ļ�ͷ����Ϣͷ
  fileHeader = (StructBmpFileHeader*)readBuf;
  infoHeader = (StructBmpInfoHeader*)((u32)readBuf + sizeof(StructBmpFileHeader));

  //��ȡͼƬ��Ⱥ͸߶�
  width = infoHeader->width;
  height = infoHeader->height;

  //��ȡǰ�����ص��ʽ
  if(32 == infoHeader->colorSize)
  {
    foregroundPixelFormat = FOREGROUND_PPF_ARGB8888;
  }
  else if(24 == infoHeader->colorSize)
  {
    foregroundPixelFormat = FOREGROUND_PPF_RGB888;
  }
  else
  {
    goto DISPLAY_BMP_IPA_EXIT_MARK;
  }

  //��������¼��ȡ��ߵ��Լ���ʼ��ַ
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //��������Ϊ�ǰ��պ�����ʽ���ã�����Ҫ����ȡ��߶�ת��������������ʼ����ת��
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //��ȡ��Ļ��Ⱥ͸߶�
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //������Ļ��Ⱥ͸߶�
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //����ϵת��
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //����Ŀ��ͼ����ʼ��ַ
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //����Ŀ����ƫ��
  lineOff = phyWidth - width;

  //����ǰ����ƫ����
  foregroundLineoff = 0;
  while(0 != (width + foregroundLineoff) % 4)
  {
    foregroundLineoff++;
  }

//���·���λ��
DISPLAY_BMP_IPA_RETRY_IPA_MARK:

  //ʹ��IPAʱ��
  rcu_periph_clock_enable(RCU_IPA);

  //��λIPA
  ipa_deinit();

  //����ת��ģʽ
  ipa_pixel_format_convert_mode_set(IPA_FGTODE_PF_CONVERT); //����ĳһԴͼ��Ŀ��ͼ���в�ͬʱ�����ض��ĸ�ʽת��

  //����ǰ��ͼ�����
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = foregroundPixelFormat;              //ǰ�������ظ�ʽ
  ipa_fg_init_struct.foreground_memaddr         = (u32)readBuf + fileHeader->offBits; //ǰ����洢������ַ
  ipa_fg_init_struct.foreground_lineoff         = foregroundLineoff;                  //ǰ������ƫ��
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;                //ǰ����alphaֵ�����㷨����Ӱ��
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //ǰ����Ԥ����͸����
  ipa_fg_init_struct.foreground_prered          = 0x00; //ǰ����Ԥ�����ɫֵ
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_fg_init_struct.foreground_preblue         = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_foreground_init(&ipa_fg_init_struct);             //���ݲ�������ǰ��ͼ��

  //����Ŀ��ͼ�����
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;     //Ŀ��ͼ���ʽ
  ipa_destination_init_struct.destination_memaddr  = (u32)destStartAddr; //Ŀ��ͼ��洢��ַ
  ipa_destination_init_struct.destination_lineoff  = lineOff; //��ĩ����һ�п�ʼ֮������ص���
  ipa_destination_init_struct.destination_prealpha = 0xFF;    //Ŀ���Ԥ����͸����
  ipa_destination_init_struct.destination_prered   = 0x00;    //Ŀ���Ԥ�����ɫֵ
  ipa_destination_init_struct.destination_pregreen = 0x00;    //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.destination_preblue  = 0x00;    //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.image_width          = width;   //Ŀ��ͼ����
  ipa_destination_init_struct.image_height         = height;  //Ŀ��ͼ��߶�
  ipa_destination_init(&ipa_destination_init_struct);         //���ݲ�������Ŀ��ͼ��

  // //ʹ��IPA�ڲ���ʱ��
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //��������
  ipa_transfer_enable();

  //�ȴ��������
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //�����������
  if((SET == ipa_interrupt_flag_get(IPA_INT_FLAG_TAE)) || (SET == ipa_interrupt_flag_get(IPA_INT_FLAG_WCF)))
  {
    ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);
    goto DISPLAY_BMP_IPA_RETRY_IPA_MARK;
  }
  else
  {
    //�����־λ
    ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

    //�ر�IPA
    ipa_deinit();
  }

//�����˳�λ��
DISPLAY_BMP_IPA_EXIT_MARK:

  //�ر��ļ�
  f_close(file);

  //�ͷ��ڴ�
  MyFree(SDRAMEX, file);
  MyFree(SDRAMEX, readBuf);
}

/*********************************************************************************************************
* �������ƣ�GetBMPSize
* �������ܣ���ȡλͼ��Ϣ
* ���������image��λͼ��width����������height���߶����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void GetBMPSize(StructBmpImage* image, u32* width, u32* height)
{
  FRESULT fresult;   //�ļ���д�����־
  u32     errorFlag; //�����־λ
  u32     readNum;   //ʵ�ʶ�ȡ���ļ�����

  //λͼ�������ڴ���
  if(BMP_IN_MEM == image->storageType)
  {
    //��ȡ�ļ���Ϣ
    GetHeaderInMem(image->addr);

    //У�顰BM��
    if(s_structFileHeader.type != 0x4D42)
    {
      printf("GetBMPSize: this is not a BMP file\r\n");
      *width = 0xFFFFFFFF;
      *height = 0xFFFFFFFF;
      return;
    }

    //����ͼƬ��С
    *width = s_structInfoHeader.width;
    *height = s_structInfoHeader.height;
    return;
  }

  errorFlag = 0;
  s_arrBmpBuffer = NULL;

//���³��Խڵ�
GET_BMP_SIZE_RETRY_MARK:

  //�ͷ��ڴ�
  if(NULL != s_arrBmpBuffer)
  {
    MyFree(SDRAMEX, s_arrBmpBuffer);
  }

  //���¹����ļ�ϵͳ
  if(0 != errorFlag)
  {
    errorFlag = 0;
    if('0' == *((u8*)image->addr + 0))
    {
      ForceMountSDCard();
    }
    else if('1' == *((u8*)image->addr + 0))
    {
      ForceMountNandFlash();
    }
    else
    {
      return;
    }
  }

  //���ļ�
  fresult = f_open(&s_fBmpFile, (const TCHAR*)image->addr, FA_READ);
  if(FR_OK != fresult)
  {
    printf("GetBMPSize��Fail to open file %s\r\n", (const TCHAR*)image->addr);
    errorFlag = 1;
    goto GET_BMP_SIZE_RETRY_MARK;
  }

  //ΪͼƬ�����������ڴ�
  s_arrBmpBuffer = MyMalloc(SDRAMEX, s_fBmpFile.fsize);
  if(NULL == s_arrBmpBuffer)
  {
    printf("GetBMPSize��Fail to malloc for image\r\n");
    goto GET_BMP_SIZE_EXIT_MARK;
  }

  //��ȡ�ļ�ͷ����Ϣͷ
  fresult = f_read(&s_fBmpFile, s_arrBmpBuffer, sizeof(StructBmpFileHeader) + sizeof(StructBmpInfoHeader), &readNum);
  if((FR_OK != fresult) || (readNum != sizeof(StructBmpFileHeader) + sizeof(StructBmpInfoHeader)))
  {
    printf("GetBMPSize��Fail: fail to read data from %s\r\n", (char*)image->addr);
    printf("GetBMPSize��Fail: retry...\r\n");
    f_close(&s_fBmpFile);
    errorFlag = 1;
    goto GET_BMP_SIZE_RETRY_MARK;
  }
  
  //��ȡBMP�ļ�ͷ����Ϣͷ
  GetHeaderInFatFs();

  //У�顰BM��
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("GetBMPSize: this is not a BMP file\r\n");
    *width = 0xFFFFFFFF;
    *height = 0xFFFFFFFF;
    goto GET_BMP_SIZE_EXIT_MARK;
  }

  //����ͼƬ��С
  *width = s_structInfoHeader.width;
  *height = s_structInfoHeader.height;

//�˳��ڵ�
GET_BMP_SIZE_EXIT_MARK:

  //�ر��ļ�
  f_close(&s_fBmpFile);

  //�ͷ��ڴ�
  MyFree(SDRAMEX, s_arrBmpBuffer);
}
