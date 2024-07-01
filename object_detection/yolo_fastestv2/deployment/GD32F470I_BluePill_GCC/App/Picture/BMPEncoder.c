/*********************************************************************************************************
* 模块名称：BMPEncoder.c
* 摘    要：BMP编码模块
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
#include "BMPEncoder.h"
#include "ff.h"
#include "Malloc.h"
#include "stdio.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void CheckDir(char* dir);                             //校验目标是否存在，若不存在则新建该目录
static void GetNewName(char* dir, char* prefix, char* name); //获得新的文件名

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CheckDir
* 函数功能：校验目标是否存在，若不存在则新建该目录
* 输入参数：dir：目标路径，不含"/"
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void CheckDir(char* dir)
{
  DIR     recDir; //目标路径
  FRESULT result; //文件操作返回变量

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
* 函数名称：GetNewName
* 函数功能：获得新的文件名
* 输入参数：dir：指定路径，不含"/"，prefix：文件名前缀，name：名字缓冲区，包好完整路径
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：打开文件时FIL必须是静态变量或动态内存分配的内存
*********************************************************************************************************/
static void GetNewName(char* dir, char* prefix, char* name)
{
  FIL*    recFile; //歌曲文件
  FRESULT result;  //文件操作返回变量
  u32     index;   //录音文件计数

  //为FIL申请内存
  recFile = MyMalloc(SRAMIN, sizeof(FIL));
  if(NULL == recFile)
  {
    printf("GetNewRecName：申请内存失败\r\n");
  }

  index = 0;
  while(index < 0xFFFFFFFF)
  {
    //生成新的名字
    sprintf((char*)name, "%s/%s%d.bmp", dir, prefix, index);

    //检查当前文件是否已经存在（若是能成功打开则说明文件已存在）
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

  //释放内存
  MyFree(SRAMIN, recFile);
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：BMPEncodeWithRGB565
* 函数功能：屏幕截图并生成BMP文件到指定位置
* 输入参数：x0：起始横坐标（像素点）
*          y0：起始纵坐标（像素点）
*          width：宽度（像素点）
*          height：高度（像素点）
*          path：储存路径，一般为"0:/photo"，若路径不存在则自动创建该路径，不含"/"
*          prefix：文件名前缀，最终会生成"xxx01.bmp"位图文件，文件自动编号
* 输出参数：void
* 返 回 值：0-成功，1-申请内存失败，2-创建文件失败，3-写入头文件信息失败，4-写入像素数据失败
* 创建日期：2021年07月01日
* 注    意：
*          1、位图固定生成RGB565格式数据
*          2、每读入一行像素数据就写入FatFs一次
*          3、位图每行的数据（字节）都需要是4的倍数
*          4、FatFs每次写入数据量必须是4字节对齐，否则下次写入相同文件将会卡死，所以BMP文件头信息必须做4字节对齐
*********************************************************************************************************/
u32 BMPEncodeWithRGB565(u32 x0, u32 y0, u32 width, u32 height, const char* path, const char* prefix)
{
  FIL*           bmpFile;  //位图文件
  u16*           imageBuf; //数据缓冲区
  char*          bmpName;  //位图文件名（含路径）
  StructBMPInfo* bmpInfo;  //BMP文件头信息
  FRESULT        result;   //文件操作返回变量
  u32            writeNum; //成功写入数据量
  u32            dataSize; //数据区大小（字节）
  u32            row;      //按4字节对齐的列数
  u32            i, x, y;  //循环变量
  u32            ret;      //返回值

  //预设返回值
  ret = 0;

  //申请内存
  bmpFile  = MyMalloc(SRAMIN, sizeof(FIL));
  imageBuf = MyMalloc(SRAMIN, BMP_IMAGE_BUF_SIZE * 2);
  bmpName  = MyMalloc(SRAMIN, BMP_NAME_LEN_MAX);
  bmpInfo  = MyMalloc(SRAMIN, sizeof(StructBMPInfo));
  if((NULL == bmpFile) || (NULL == imageBuf) || (NULL == bmpName))
  {
    printf("BMPEncode：申请动态内存失败\r\n");
    ret = 0;
    goto BMP_ENCODE_EXIT_MARK;
  }

  //校验路径，若路径不存在则新建路径
  CheckDir((char*)path);

  //生成新的文件名
  GetNewName((char*)path, (char*)prefix, bmpName);

  //创建位图文件
  result = f_open(bmpFile, (const TCHAR*)bmpName, FA_CREATE_ALWAYS | FA_WRITE);
  if(FR_OK != result)
  {
    printf("BMPEncode：创建文件失败\r\n");
    ret = 2;
    goto BMP_ENCODE_EXIT_MARK;
  }
  
  //计算数据区大小
  row = width;
  while(0 != (row % 2))
  {
    row++;
  }
  dataSize = row * height * 2;
  
  //完善文件头
  bmpInfo->bmFileHeader.bfType      = 0x4D42;                           //BM格式标志
  bmpInfo->bmFileHeader.bfSize      = dataSize + sizeof(StructBMPInfo); //整个BMP大小
  bmpInfo->bmFileHeader.bfReserved1 = 0;                                //保留区1
  bmpInfo->bmFileHeader.bfReserved2 = 0;                                //保留区2
  bmpInfo->bmFileHeader.bfOffBits   = sizeof(StructBMPInfo);            //到数据区的偏移

  //完善信息头
  bmpInfo->bmInfoHeader.biSize          = sizeof(StructBMPInfoHeader); //信息头大小
  bmpInfo->bmInfoHeader.biWidth         = width;                       //BMP的宽度
  bmpInfo->bmInfoHeader.biHeight        = height;                      //BMP的高度
  bmpInfo->bmInfoHeader.biPlanes        = 1;                           //标设备说明颜色平面数，总被设置为1
  bmpInfo->bmInfoHeader.biBitCount      = 16;                          //bmp为16位色bmp
  bmpInfo->bmInfoHeader.biCompression   = BI_BITFIELDS;                //每个象素的比特由指定的掩码决定
  bmpInfo->bmInfoHeader.biSizeImage     = dataSize;                    //BMP数据区大小
  bmpInfo->bmInfoHeader.biXPelsPerMeter = 8600;                        //水平分辨率，象素/米
  bmpInfo->bmInfoHeader.biYPelsPerMeter = 8600;                        //垂直分辨率，象素/米
  bmpInfo->bmInfoHeader.biClrUsed       = 0;                           //没有使用到调色板
  bmpInfo->bmInfoHeader.biClrImportant  = 0;                           //没有使用到调色板

  //RGB掩码
  bmpInfo->rgbMask[0] = 0x00F800; //红色掩码
  bmpInfo->rgbMask[1] = 0x0007E0; //绿色掩码
  bmpInfo->rgbMask[2] = 0x00001F; //蓝色掩码
  
  //保留区
  bmpInfo->reserved1 = 0;
  bmpInfo->reserved2 = 0;

  //将文件读写指针偏移到起始地址
  f_lseek(bmpFile, 0);

  //写入BMP头文件信息
  result = f_write(bmpFile, (const void*)bmpInfo, sizeof(StructBMPInfo), &writeNum);
  if((FR_OK != result) || (writeNum != sizeof(StructBMPInfo)))
  {
    printf("BMPEncode：写入头文件信息失败\r\n");
    f_close(bmpFile);
    ret = 3;
    goto BMP_ENCODE_EXIT_MARK;
  }

  //读取屏幕数据并保存到文件中
  y = y0 + height - 1;
  while(1)
  {
    //读入一行数据到缓冲区，每读入一行就存入文件一次
    i = 0;
    for(x = x0; x < (x0 + width); x++)
    {
      imageBuf[i++] = LCDReadPoint(x, y);
    }

    //4字节对齐，用0填充空位
    while(0 != (i % 2))
    {
      imageBuf[i++] = 0x0000;
    }

    //保存一行数据到文件中
    result = f_write(bmpFile, (const void*)imageBuf, i * 2, &writeNum);
    if((FR_OK != result) && (writeNum != i * 2))
    {
      printf("BMPEncode：写入像素数据失败\r\n");
      f_close(bmpFile);
      ret = 4;
      goto BMP_ENCODE_EXIT_MARK;
    }

    //判断是否读完
    if(y0 == y)
    {
      break;
    }
    else
    {
      y = y - 1;
    }
  }

  //关闭文件
  f_close(bmpFile);
  printf("BMPEncode：成功保存图像：%s\r\n", bmpName);
  
//函数返回位置，返回前要释放动态内存
BMP_ENCODE_EXIT_MARK:
  MyFree(SRAMIN, bmpFile);
  MyFree(SRAMIN, imageBuf);
  MyFree(SRAMIN, bmpName);
  MyFree(SRAMIN, bmpInfo);
  return ret;
}

/*********************************************************************************************************
* 函数名称: ScreeShot
* 函数功能: 截图
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2019年08月01日
* 注    意:
*********************************************************************************************************/
void ScreeShot(void)
{
  BMPEncodeWithRGB565(0, 0, 
  g_structTLILCDDev.width[g_structTLILCDDev.currentLayer], 
  g_structTLILCDDev.height[g_structTLILCDDev.currentLayer], 
  "0:/photo",
  "ScreeShot");
}
