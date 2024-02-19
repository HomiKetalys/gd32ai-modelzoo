/*********************************************************************************************************
* 模块名称：BMP.c
* 摘    要：位图显示模块
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

/*********************************************************************************************************
*                                              包含头文件
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
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//位图 = 文件头 + 信息头 + 调色板(可选) + 数据体

//BMP文件头
typedef __packed struct
{
  u16 type;      //文件标志，只为'BM'，必须是BM，十六进制中则是0x424D；
  u32 size;      //文件大小，单位为字节
  u16 reserved1; //保留，必须为0
  u16 reserved2; //保留，必须为0
  u32 offBits;   //位图数据的起始位置相对于文件头的偏移量，单位为字节。
}StructBmpFileHeader;

//BMP信息头
typedef __packed struct
{
  u32 infoSize;       //该结构体所需的字节数
  u32 width;          //位图的宽度（像素单位）
  u32 height;         //位图的高度（像素单位）
  u16 planes;         //目标设备的级别，必须为1
  u16 colorSize;      //每个像素所需的位数
  u32 compression;    //说明图象数据压缩的类型，位图压缩类型，必须是0（不压缩）
  u32 imageSize;      //位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位
  u32 xPelsPerMeter;  //位图水平分辨率，每米像素数
  u32 yPelsPerMeter;  //位图垂直分辨率，每米像素数
  u32 colorUsed;      //位图实际使用的颜色表中的颜色数
  u32 colorImportant; //位图显示过程中重要的颜色数，如果是 0，表示都重要
}StructBmpInfoHeader;

//颜色表
typedef __packed struct
{
  u8 blue;     //指定蓝色强度
  u8 green;    //指定绿色强度
  u8 red;      //指定红色强度
  u8 reserved; //保留，设置为0
}StructRGB;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static FIL s_fBmpFile;     //BMP文件
static u8* s_arrBmpBuffer; //文件读取临时缓冲区

static StructBmpFileHeader s_structFileHeader;
static StructBmpInfoHeader s_structInfoHeader;

static FATFS s_arrFatFs[2];  //0-Nand Flash, 1-SD Card

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ForceMountNandFlash(void);															//强行挂载Nand Flash
static void ForceMountSDCard(void);																	//强行挂载SD卡
static u8   GetHeaderInMem(const unsigned char *image);              //获取文件头和信息头
static u8*  GetLineInMem(const unsigned char *image, u32 line);      //获取一行数据
static void BMPDisplayInMem(StructBmpImage* image, u32 x, u32 y);    //获取内存中的位图并显示
static u16  CalcAlphaRGB555(u16 backColor, u16 foreColor, u8 alpha); //透明度叠加计算
static u8   GetHeaderInFatFs(void);                                  //获取文件头和信息头
static u8*  GetLineInFatFs(u32 line);                                //获取一行数据
static void BMPDisplayInFatFs(StructBmpImage* image, u32 x, u32 y);  //获取文件系统中的位图并显示

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ForceMountNandFlash
* 函数功能：强行挂载Nand Flash
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：若Nand Flash中未存在文件系统则会卡死
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
* 函数名称：ForceMountSDCard
* 函数功能：强行挂载SD卡
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：若SD卡未插入则会卡死在该函数
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
* 函数名称：GetHeaderInMem
* 函数功能：获取文件头和信息头
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 GetHeaderInMem(const unsigned char *image)
{ 
  u32 i;
  u8* buf;
  
  //读取BMP文件头
  buf = (u8*)image;
  for(i = 0; i < sizeof(StructBmpFileHeader); i++)
  {
    *((u8*)(&s_structFileHeader) + i) = buf[i];
  }

  //读取信息头
  buf = (u8*)image + sizeof(StructBmpFileHeader);
  for(i = 0; i < sizeof(StructBmpInfoHeader); i++)
  {
    *((u8*)(&s_structInfoHeader) + i) = buf[i];
  }
  
  return 0;
}

/*********************************************************************************************************
* 函数名称：GetLineInMem
* 函数功能：获取一行数据
* 输入参数：line：行号，从0开始
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8* GetLineInMem(const unsigned char *image, u32 line)
{
  u32  readAddr;  //读取地址相对于起始地址偏移量
  u32  lineSize;  //一行数据量，要4字节对齐
  u8*  p;         //返回地址
  
  //计算32位图片一行数据大小
  if(32 == s_structInfoHeader.colorSize)
  {
    lineSize = s_structInfoHeader.width * 4;
  }
  
  //计算24位图片一行数据大小
  else if(24 == s_structInfoHeader.colorSize)
  {
    //行数据量需是4的倍数
    lineSize = s_structInfoHeader.width * 3;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //计算16位图片一行数据大小
  else if(16 == s_structInfoHeader.colorSize)
  {
    //行数据量需是4的倍数
    lineSize = s_structInfoHeader.width * 2;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //计算行数对应的地址
  readAddr = s_structFileHeader.offBits + lineSize * line;

  //得到行首位置
  p = (u8*)image + readAddr;

  return p;
}

/*********************************************************************************************************
* 函数名称：BMPDisplayInMem
* 函数功能：指定位置显示位图
* 输入参数：fileName：文件名，包含完整路径；x：横坐标；y：纵坐标
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、起点在左上角，位图是从左至右从下到上的（从倒数第一行开始存的）
*          2、目前仅支持32位和24位格式位图显示
*          3、为了加快位图显示，透明度仅支持显示和不显示，透明度大于0则显示该点，透明度等于0则不显示
*********************************************************************************************************/
static void BMPDisplayInMem(StructBmpImage* image, u32 x, u32 y)
{
  u32     x0, y0, x1, y1; //起点、终点
  u32     byteCnt;        //单次读取字节计数
  u8      red;            //红色通道数值
  u8      green;          //绿色通道数值
  u8      blue;           //蓝色通道数值
  u8      alpha;          //透明度
  u32     color;          //最终颜色值
  u32     lineCnt;        //读取行数计数
  u8*     line;           //行数据首地址
  u32     yn;             //实际写入纵坐标
  u32     backColor;      //背景颜色（透明度叠加时使用）
  u32     alphaColor;     //透明度叠加结果
  
  //读取BMP文件头和信息头
  GetHeaderInMem((const unsigned char*)image->addr);

  //校验“BM”
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("BMPDisplay: this is not a BMP file\r\n");
    return;
  }

  //仅支持32位、24、16位格式位图
  if(!((32 == s_structInfoHeader.colorSize) || (24 == s_structInfoHeader.colorSize) || (16 == s_structInfoHeader.colorSize)))
  {
    printf("BMPDisplay: unsupported format!!!\r\n");
    return;
  }

  //设定显示范围
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

  //位图是从左往右，从下至上储存的
  yn = y1 - 1;

  //行数计数清零
  lineCnt = 0;

  //纵坐标循环
  for(y = y0; y < y1; y++)
  {
    //读取一整行数据
    line = GetLineInMem((const unsigned char*)image->addr, lineCnt);

    //字节计数清零
    byteCnt = 0;

    //行数计数加一
    lineCnt++;

    //横坐标循环
    for(x = x0; x < x1; x++)
    {
      //32位像素点
      if(32 == s_structInfoHeader.colorSize)
      {
        //获取像素值
        blue  = line[byteCnt + 0] >> 3; //蓝色
        green = line[byteCnt + 1] >> 2; //绿色
        red   = line[byteCnt + 2] >> 3; //红色
        alpha = line[byteCnt + 3];      //透明度
        
        //24位RGB转16位RGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //字节计数增加
        byteCnt = byteCnt + 4;

        //透明度叠加
        if(BMP_UES_ALPHA == image->alphaType)
        {
          //读入背景颜色
          backColor = LCDReadPoint(x, yn);

          //透明度叠加
          alphaColor = CalcAlphaRGB555(backColor, color, alpha);

          //画点
          LCDDrawPoint(x, yn, alphaColor);
        }

        //过滤掉透明度较低的像素点不显示
        else if(BMP_GATE_ALPHA == image->alphaType)
        {
          if(alpha >= image->alphaGate)
          {
            LCDDrawPoint(x, yn, color);
          }
       }

        //不考虑透明度，所有像素点均显示
        else if(BMP_NO_ALPHA == image->alphaType)
        {
          LCDDrawPoint(x, yn, color);
        }
      }
      
      //24位像素点
      else if(24 == s_structInfoHeader.colorSize)
      {
        //获取像素值
        blue  = line[byteCnt + 0] >> 3; //蓝色
        green = line[byteCnt + 1] >> 2; //绿色
        red   = line[byteCnt + 2] >> 3; //红色
        
        //24位RGB转16位RGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //字节计数增加
        byteCnt = byteCnt + 3;

        //画点
        LCDDrawPoint(x, yn, color);
      }

      //16位像素点
      else if(16 == s_structInfoHeader.colorSize)
      {
        color = (line[byteCnt + 1] << 8) | line[byteCnt + 0];

        //字节计数增加
        byteCnt = byteCnt + 2;

        //画点
        LCDDrawPoint(x, yn, color);
      }
    }

    //从坐标从下往上扫描
    yn = yn - 1;
  }
}

/*********************************************************************************************************
* 函数名称：CalcAlphaRGB555
* 函数功能：透明度叠加
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：透明度范围0-255
*********************************************************************************************************/
static u16 CalcAlphaRGB555(u16 backColor, u16 foreColor, u8 alpha)
{
  u16 backR, backG, backB, foreR, foreG, foreB, colorAlpha;
  u16 resultR, resultG, resultB;
  u16 result;

  //透明度太小，直接返回背景色
  if(alpha < 5)
  {
    return backColor;
  }

  //提前背景RGB通道数据
  backR = ((backColor >> 11) & 0x1F);
  backG = ((backColor >> 5 ) & 0x3F);
  backB = ((backColor >> 0 ) & 0x1F);

  //提取前景RGB通道数据
  foreR = ((foreColor >> 11) & 0x1F);
  foreG = ((foreColor >> 5 ) & 0x3F);
  foreB = ((foreColor >> 0 ) & 0x1F);

  //获取透明度
  colorAlpha = alpha;

  //RGB通道透明度叠加
  resultR = (foreR * colorAlpha + backR * (0xFF - colorAlpha)) / 0xFF;
  resultG = (foreG * colorAlpha + backG * (0xFF - colorAlpha)) / 0xFF;
  resultB = (foreB * colorAlpha + backB * (0xFF - colorAlpha)) / 0xFF;

  //组合成RGB565格式
  result = (((u16)resultR) << 11) | (((u16)resultG) << 5) | (((u16)resultB) << 0);

  return result;
}

/*********************************************************************************************************
* 函数名称：GetHeaderInFatFs
* 函数功能：获取文件头和信息头
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static u8 GetHeaderInFatFs(void)
{ 
  u32 i;
  u8* buf;
  
  //读取BMP文件头
  buf = s_arrBmpBuffer;
  for(i = 0; i < sizeof(StructBmpFileHeader); i++)
  {
    *((u8*)(&s_structFileHeader) + i) = buf[i];
  }

  //读取信息头
  buf = s_arrBmpBuffer + sizeof(StructBmpFileHeader);
  for(i = 0; i < sizeof(StructBmpInfoHeader); i++)
  {
    *((u8*)(&s_structInfoHeader) + i) = buf[i];
  }

  return 0;
}

/*********************************************************************************************************
* 函数名称：GetLineInFatFs
* 函数功能：获取一行数据
* 输入参数：line：行号，从0开始
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：从SD卡中读取数据时读取地址需是4的整数倍，一行数据量不是4的倍数则补零
*********************************************************************************************************/
static u8* GetLineInFatFs(u32 line)
{
  u32  readAddr;  //读取地址相对于起始地址偏移量
  u32  lineSize;  //一行数据量，要4字节对齐
  u8*  p;         //返回地址
  
  //计算32位图片一行数据大小
  if(32 == s_structInfoHeader.colorSize)
  {
    lineSize = s_structInfoHeader.width * 4;
  }
  
  //计算24位图片一行数据大小
  else if(24 == s_structInfoHeader.colorSize)
  {
    //行数据量需是4的倍数
    lineSize = s_structInfoHeader.width * 3;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //计算16位图片一行数据大小
  else if(16 == s_structInfoHeader.colorSize)
  {
    //行数据量需是4的倍数
    lineSize = s_structInfoHeader.width * 2;
    while((lineSize % 4) != 0)
    {
      lineSize++;
    }
  }

  //计算行数对应的地址
  readAddr = s_structFileHeader.offBits + lineSize * line;

  //得到行首位置
  p = (u8*)s_arrBmpBuffer + readAddr;

  return p;
}

/*********************************************************************************************************
* 函数名称：BMPDisplayInFatFs
* 函数功能：获取文件系统中的位图并显示
* 输入参数：image：文件名，包含完整路径；x：横坐标；y：纵坐标
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、起点在左上角，位图是从左至右从下到上的（从倒数第一行开始存的）
*          2、目前仅支持32位和24位格式位图显示
*          3、为了加快位图显示，透明度仅支持显示和不显示，透明度大于0则显示该点，透明度等于0则不显示
*********************************************************************************************************/
static void BMPDisplayInFatFs(StructBmpImage* image, u32 x, u32 y)
{
  FRESULT fresult;
  u32     x0, y0, x1, y1; //起点、终点
  u32     byteCnt;        //单次读取字节计数
  u8      red;            //红色通道数值
  u8      green;          //绿色通道数值
  u8      blue;           //蓝色通道数值
  u8      alpha;          //透明度
  u32     color;          //最终颜色值
  u32     lineCnt;        //读取行数计数
  u8*     line;           //行数据首地址
  u32     yn;             //实际写入纵坐标
  u32     backColor;      //背景颜色（透明度叠加时使用）
  u32     alphaColor;     //透明度叠加结果
  u32     errorFlag;      //出错标志位
  u32     readNum;        //实际读取的文件数量
  
  errorFlag = 0;
  s_arrBmpBuffer = NULL;

//重新尝试节点
DISPLAY_IN_FATFS_RETRY_MARK:

  //释放内存
  if(NULL != s_arrBmpBuffer)
  {
    MyFree(SDRAMEX, s_arrBmpBuffer);
  }

  //重新挂载文件系统
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

  //打开文件
  fresult = f_open(&s_fBmpFile, (const TCHAR*)image->addr, FA_READ);
  if(FR_OK != fresult)
  {
    printf("BMPDisplayInFatFs：Fail: Fail to open file %s\r\n", (const TCHAR*)image->addr);
    errorFlag = 1;
    goto DISPLAY_IN_FATFS_RETRY_MARK;
  }

  //为图片缓冲区申请内存
  s_arrBmpBuffer = MyMalloc(SDRAMEX, s_fBmpFile.fsize);
  if(NULL == s_arrBmpBuffer)
  {
    printf("BMPDisplayInFatFs：Fail to malloc for image\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //读取整个BMP文件
  fresult = f_read(&s_fBmpFile, s_arrBmpBuffer, s_fBmpFile.fsize, &readNum);
  if((FR_OK != fresult) || (readNum != s_fBmpFile.fsize))
  {
    printf("BMPDisplayInFatFs：Fail: fail to read data from %s\r\n", (char*)image->addr);
    printf("BMPDisplayInFatFs：Fail: retry...\r\n");
    f_close(&s_fBmpFile);
    errorFlag = 1;
    goto DISPLAY_IN_FATFS_RETRY_MARK;
  }
  
  //读取BMP文件头和信息头
  GetHeaderInFatFs();

  //校验“BM”
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("BMPDisplay: this is not a BMP file\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //仅支持32位、24、16位格式位图
  if(!((32 == s_structInfoHeader.colorSize) || (24 == s_structInfoHeader.colorSize) || (16 == s_structInfoHeader.colorSize)))
  {
    printf("BMPDisplay: unsupported format!!!\r\n");
    goto DISPLAY_IN_FATFS_EXIT_MARK;
  }

  //设定显示范围
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

  //位图是从左往右，从下至上储存的
  yn = y1 - 1;

  //行数计数清零
  lineCnt = 0;

  //纵坐标循环
  for(y = y0; y < y1; y++)
  {
    //读取一整行数据
    line = GetLineInFatFs(lineCnt);

    //字节计数清零
    byteCnt = 0;

    //行数计数加一
    lineCnt++;

    //横坐标循环
    for(x = x0; x < x1; x++)
    {
      //32位像素点
      if(32 == s_structInfoHeader.colorSize)
      {
        //获取像素值
        blue  = line[byteCnt + 0] >> 3; //蓝色
        green = line[byteCnt + 1] >> 2; //绿色
        red   = line[byteCnt + 2] >> 3; //红色
        alpha = line[byteCnt + 3];      //透明度
        
        //24位RGB转16位RGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //字节计数增加
        byteCnt = byteCnt + 4;

        //透明度叠加
        if(BMP_UES_ALPHA == image->alphaType)
        {
          //读入背景颜色
          backColor = LCDReadPoint(x, yn);

          //透明度叠加
          alphaColor = CalcAlphaRGB555(backColor, color, alpha);

          //画点
          LCDDrawPoint(x, yn, alphaColor);
        }

        //过滤掉透明度较低的像素点不显示
        else if(BMP_GATE_ALPHA == image->alphaType)
        {
          if(alpha >= image->alphaGate)
          {
            LCDDrawPoint(x, yn, color);
          }
        }

        //不考虑透明度，所有像素点均显示
        else if(BMP_NO_ALPHA == image->alphaType)
        {
          LCDDrawPoint(x, yn, color);
        }
      }
      
      //24位像素点
      else if(24 == s_structInfoHeader.colorSize)
      {
        //获取像素值
        blue  = line[byteCnt + 0] >> 3; //蓝色
        green = line[byteCnt + 1] >> 2; //绿色
        red   = line[byteCnt + 2] >> 3; //红色
        
        //24位RGB转16位RGB
        color = (((u32)red) << 11) | (((u32)green) << 5) | blue;
        
        //字节计数增加
        byteCnt = byteCnt + 3;

        //画点
        LCDDrawPoint(x, yn, color);
      }

      //16位像素点
      else if(16 == s_structInfoHeader.colorSize)
      {
        color = (line[byteCnt + 1] << 8) | line[byteCnt + 0];

        //字节计数增加
        byteCnt = byteCnt + 2;

        //画点
        LCDDrawPoint(x, yn, color);
      }
    }

    //从坐标从下往上扫描
    yn = yn - 1;
  }
  
//退出节点
DISPLAY_IN_FATFS_EXIT_MARK:

  //关闭文件
  f_close(&s_fBmpFile);

  //释放内存
  MyFree(SDRAMEX, s_arrBmpBuffer);
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitBMP
* 函数功能：初始化位图显示模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitBMP(void)
{
  
}

/*********************************************************************************************************
* 函数名称：DisplayBMP
* 函数功能：指定位置显示位图
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DisplayBMP(StructBmpImage* image, u32 x, u32 y)
{
  //图片储存在内存中
  if(BMP_IN_MEM == image->storageType)
  {
    BMPDisplayInMem(image, x, y);
  }
  
  //图片储存在文件系统中
  else if(BMP_IN_FATFS == image->storageType)
  {
    BMPDisplayInFatFs(image, x, y);
  }
}

/*********************************************************************************************************
* 函数名称：DisPlayBMPByIPA
* 函数功能：使用IPA图像处理加速器绘制位图，可以实现秒刷位图
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、图片必须为横屏方向，如果要显示竖屏图片，则需要在电脑将图片调整至横向
*          2、横屏时图片要上下翻转，否则显示时将会镜像
*          3、此函数不检查显示区域是否超出屏幕范围
*          4、格式转换时有些像素点会花掉，应该是SDRAM速度不够
*          5、默认只打开SD卡内的图片
*          6、只适合拿来刷背景图片或一些大图
*********************************************************************************************************/
void DisPlayBMPByIPA(u32 x, u32 y, char* addr)
{
  //文件相关变量
  FIL*    file;    //位图文件
  FRESULT result;  //文件操作返回变量
  u8*     readBuf; //读取缓冲区，由动态内存分配
  u32     readNum; //实际读取的文件数量

  //IPA相关变量
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //前景图像参数结构体
  ipa_destination_parameter_struct ipa_destination_init_struct; //目标图像参数结构体
  u32 destStartAddr;         //目标图像起始地址
  u32 phyWidth, phyHeight;   //横屏宽度和高度
  u32 phyX, phyY;            //横屏时填充起始地址
  u32 lineOff;               //目标行偏移，最后一个像素和下一行第一个像素之间的像素数目
  u32 swap;                  //交换数据时的临时变量
  u32 foregroundPixelFormat; //前景像素点格式
  u32 foregroundLineoff;     //前景行偏移量

  //位图文件头和信息头
  StructBmpFileHeader* fileHeader;
  StructBmpInfoHeader* infoHeader;
  u32 width;
  u32 height;

  //出错标志位
  u32 error;

  //赋初值
  file = NULL;
  readBuf = NULL;
  error = 0;

  //为位图文件申请内存
  file = MyMalloc(SDRAMEX, sizeof(FIL));
  if(NULL == file)
  {
    printf("DisPlayBMPByIPA: fail to malloc for FIL\r\n");
    return;
  }


//重新打开文件位置
DISPLAY_BMP_IPA_RETRY_FILE_MARK:

  //出错，重新挂载文件系统
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

  //释放上次申请的内存
  if(NULL != readBuf)
  {
    MyFree(SDRAMEX, readBuf);
  }

  //打开位图文件
  result = f_open(file, (const TCHAR*)addr, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK)
  {
    printf("DisPlayBMPByIPA: fail to open file %s \r\n", addr);
    printf("DisPlayBMPByIPA: retry...\r\n");
    error = 1;
    goto DISPLAY_BMP_IPA_RETRY_FILE_MARK;
  }

  //读取缓冲区内存申请
  readBuf = MyMalloc(SDRAMEX, file->fsize);
  if(NULL == readBuf)
  {
    printf("DisPlayBMPByIPA: fail to malloc for BMP image\r\n");
    goto DISPLAY_BMP_IPA_EXIT_MARK;
  }

  //读取数据
  result = f_read(file, readBuf, file->fsize, &readNum);
  if((FR_OK != result) || (readNum != file->fsize))
  {
    printf("DisPlayBMPByIPA: fail to read data from %s\r\n", addr);
    printf("DisPlayBMPByIPA: retry...\r\n");
    f_close(file);
    error = 1;
    goto DISPLAY_BMP_IPA_RETRY_FILE_MARK;
  }

  //获取文件头和信息头
  fileHeader = (StructBmpFileHeader*)readBuf;
  infoHeader = (StructBmpInfoHeader*)((u32)readBuf + sizeof(StructBmpFileHeader));

  //获取图片宽度和高度
  width = infoHeader->width;
  height = infoHeader->height;

  //获取前景像素点格式
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

  //横屏，记录宽度、高低以及起始地址
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //竖屏，因为是按照横屏方式配置，所以要将宽度、高度转换过来，并做起始坐标转换
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //获取屏幕宽度和高度
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //交换屏幕宽度和高度
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //坐标系转换
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //计算目标图像起始地址
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //计算目标行偏移
  lineOff = phyWidth - width;

  //计算前景行偏移量
  foregroundLineoff = 0;
  while(0 != (width + foregroundLineoff) % 4)
  {
    foregroundLineoff++;
  }

//重新发送位置
DISPLAY_BMP_IPA_RETRY_IPA_MARK:

  //使能IPA时钟
  rcu_periph_clock_enable(RCU_IPA);

  //复位IPA
  ipa_deinit();

  //设置转换模式
  ipa_pixel_format_convert_mode_set(IPA_FGTODE_PF_CONVERT); //复制某一源图像到目标图像中并同时进行特定的格式转换

  //设置前景图像参数
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = foregroundPixelFormat;              //前景层像素格式
  ipa_fg_init_struct.foreground_memaddr         = (u32)readBuf + fileHeader->offBits; //前景层存储区基地址
  ipa_fg_init_struct.foreground_lineoff         = foregroundLineoff;                  //前景层行偏移
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;                //前景层alpha值计算算法，无影响
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //前景层预定义透明度
  ipa_fg_init_struct.foreground_prered          = 0x00; //前景层预定义红色值
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //前景层预定义绿色值
  ipa_fg_init_struct.foreground_preblue         = 0x00; //前景层预定义蓝色值
  ipa_foreground_init(&ipa_fg_init_struct);             //根据参数配置前景图像

  //设置目标图像参数
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;     //目标图像格式
  ipa_destination_init_struct.destination_memaddr  = (u32)destStartAddr; //目标图像存储地址
  ipa_destination_init_struct.destination_lineoff  = lineOff; //行末与下一行开始之间的像素点数
  ipa_destination_init_struct.destination_prealpha = 0xFF;    //目标层预定义透明度
  ipa_destination_init_struct.destination_prered   = 0x00;    //目标层预定义红色值
  ipa_destination_init_struct.destination_pregreen = 0x00;    //目标层预定义绿色值
  ipa_destination_init_struct.destination_preblue  = 0x00;    //目标层预定义蓝色值
  ipa_destination_init_struct.image_width          = width;   //目标图像宽度
  ipa_destination_init_struct.image_height         = height;  //目标图像高度
  ipa_destination_init(&ipa_destination_init_struct);         //根据参数配置目标图像

  // //使能IPA内部定时器
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //开启传输
  ipa_transfer_enable();

  //等待传输结束
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //发生传输错误
  if((SET == ipa_interrupt_flag_get(IPA_INT_FLAG_TAE)) || (SET == ipa_interrupt_flag_get(IPA_INT_FLAG_WCF)))
  {
    ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);
    goto DISPLAY_BMP_IPA_RETRY_IPA_MARK;
  }
  else
  {
    //清除标志位
    ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

    //关闭IPA
    ipa_deinit();
  }

//函数退出位置
DISPLAY_BMP_IPA_EXIT_MARK:

  //关闭文件
  f_close(file);

  //释放内存
  MyFree(SDRAMEX, file);
  MyFree(SDRAMEX, readBuf);
}

/*********************************************************************************************************
* 函数名称：GetBMPSize
* 函数功能：获取位图信息
* 输入参数：image：位图，width：宽度输出，height：高度输出
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void GetBMPSize(StructBmpImage* image, u32* width, u32* height)
{
  FRESULT fresult;   //文件读写错误标志
  u32     errorFlag; //出错标志位
  u32     readNum;   //实际读取的文件数量

  //位图储存在内存中
  if(BMP_IN_MEM == image->storageType)
  {
    //获取文件信息
    GetHeaderInMem(image->addr);

    //校验“BM”
    if(s_structFileHeader.type != 0x4D42)
    {
      printf("GetBMPSize: this is not a BMP file\r\n");
      *width = 0xFFFFFFFF;
      *height = 0xFFFFFFFF;
      return;
    }

    //返回图片大小
    *width = s_structInfoHeader.width;
    *height = s_structInfoHeader.height;
    return;
  }

  errorFlag = 0;
  s_arrBmpBuffer = NULL;

//重新尝试节点
GET_BMP_SIZE_RETRY_MARK:

  //释放内存
  if(NULL != s_arrBmpBuffer)
  {
    MyFree(SDRAMEX, s_arrBmpBuffer);
  }

  //重新挂载文件系统
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

  //打开文件
  fresult = f_open(&s_fBmpFile, (const TCHAR*)image->addr, FA_READ);
  if(FR_OK != fresult)
  {
    printf("GetBMPSize：Fail to open file %s\r\n", (const TCHAR*)image->addr);
    errorFlag = 1;
    goto GET_BMP_SIZE_RETRY_MARK;
  }

  //为图片缓冲区申请内存
  s_arrBmpBuffer = MyMalloc(SDRAMEX, s_fBmpFile.fsize);
  if(NULL == s_arrBmpBuffer)
  {
    printf("GetBMPSize：Fail to malloc for image\r\n");
    goto GET_BMP_SIZE_EXIT_MARK;
  }

  //读取文件头和信息头
  fresult = f_read(&s_fBmpFile, s_arrBmpBuffer, sizeof(StructBmpFileHeader) + sizeof(StructBmpInfoHeader), &readNum);
  if((FR_OK != fresult) || (readNum != sizeof(StructBmpFileHeader) + sizeof(StructBmpInfoHeader)))
  {
    printf("GetBMPSize：Fail: fail to read data from %s\r\n", (char*)image->addr);
    printf("GetBMPSize：Fail: retry...\r\n");
    f_close(&s_fBmpFile);
    errorFlag = 1;
    goto GET_BMP_SIZE_RETRY_MARK;
  }
  
  //读取BMP文件头和信息头
  GetHeaderInFatFs();

  //校验“BM”
  if(s_structFileHeader.type != 0x4D42)
  {
    printf("GetBMPSize: this is not a BMP file\r\n");
    *width = 0xFFFFFFFF;
    *height = 0xFFFFFFFF;
    goto GET_BMP_SIZE_EXIT_MARK;
  }

  //返回图片大小
  *width = s_structInfoHeader.width;
  *height = s_structInfoHeader.height;

//退出节点
GET_BMP_SIZE_EXIT_MARK:

  //关闭文件
  f_close(&s_fBmpFile);

  //释放内存
  MyFree(SDRAMEX, s_arrBmpBuffer);
}
