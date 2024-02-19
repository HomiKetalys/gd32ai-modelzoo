/*********************************************************************************************************
* 模块名称：FontLib.c
* 摘    要：字库管理模块
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
#include "FontLib.h"
#include "GD25QXX.h"
#include "ff.h"
#include "Malloc.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define FONT_FIL_BUF_SIZE (1024 * 4) //字库文件缓冲区大小（4k字节）

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static u8 CheckFontLib(void); //校验字库

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：CheckFontLib
* 函数功能：校验字库
* 输入参数：void
* 输出参数：void
* 返 回 值：0-校验字库成功，其它-字库损坏，需要重新更新字库
* 创建日期：2021年07月01日
* 注    意：printf函数中'\r'不起作用，原因未知
*********************************************************************************************************/
static u8 CheckFontLib(void)
{
  static FIL s_filFont;  //字库文件（需是静态变量）
  FRESULT    result;     //文件操作返回变量
  u8*        fileBuf;    //字库缓冲区，文件系统中的字库文件，由动态内存分配
  u8*        flashBuf;   //字库缓冲区，Flash中的字库文件，由动态内存分配
  u32        readNum;    //实际读取的文件数量
  u32        ReadAddr;   //读入SPI Flash地址
  u32        i;          //循环变量
  u32        progress;   //进度

  //申请动态内存
  fileBuf  = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  flashBuf = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  if((NULL == fileBuf) || (NULL == flashBuf))
  {
    MyFree(SDRAMEX, fileBuf);
    MyFree(SDRAMEX, flashBuf);
    printf("CheckFontLib：申请动态内存失败\r\n");
    return 0;
  }

  //打开文件
  result = f_open(&s_filFont, "0:/font/GBK24.FON", FA_OPEN_EXISTING | FA_READ);
  if (result !=  FR_OK)
  {
    //释放内存
    MyFree(SDRAMEX, fileBuf);
    MyFree(SDRAMEX, flashBuf);

    //打印错误信息
    printf("CheckFontLib：打开字库文件失败\r\n");
    return 0;
  }

  //读取数据并逐一比较整个字库，若是有一个不同则表示字库损坏
  printf("CheckFontLib：开始校验字库\r\n");
  ReadAddr = 0;
  progress = 0;
  while(1)
  {
    //进度输出
    if((100 * s_filFont.fptr / s_filFont.fsize) >= (progress + 15))
    {
      progress = 100 * s_filFont.fptr / s_filFont.fsize;
      printf("CheckFontLib：校验进度：%%%d\r\n", progress);
    }

    //从文件中读取数据到缓冲区
    result = f_read(&s_filFont, fileBuf, FONT_FIL_BUF_SIZE, &readNum);
    if (result !=  FR_OK)
    {
      //关闭文件
      f_close(&s_filFont);

      //释放内存
      MyFree(SDRAMEX, fileBuf);
      MyFree(SDRAMEX, flashBuf);

      //打印错误信息
      printf("CheckFontLib：读取数据失败\r\n");
      return 0;
    }

    //从SPI Flash中读取数据
    GD25Q16Read(flashBuf, readNum, ReadAddr);

    //逐一比较
    for(i = 0; i < readNum; i++)
    {
      //发现字库损坏
      if(flashBuf[i] != fileBuf[i])
      {
        //关闭文件
        f_close(&s_filFont);

        //释放内存
        MyFree(SDRAMEX, fileBuf);
        MyFree(SDRAMEX, flashBuf);
        return 1;
      }
    }

    //更新读取地址
    ReadAddr = ReadAddr + readNum;

    //判断文件是否读写完成
    if((s_filFont.fptr >= s_filFont.fsize) || (FONT_FIL_BUF_SIZE != readNum))
    {
      printf("CheckFontLib：校验进度：%%100\r\n");
      break;
    }
  }

  //关闭文件
  f_close(&s_filFont);

  //释放内存
  MyFree(SDRAMEX, fileBuf);
  MyFree(SDRAMEX, flashBuf);

  return 0;
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitFontLib
* 函数功能：初始化字库管理模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：自动检测更新字库，若是字库有损坏则从SD卡中更新字库
*********************************************************************************************************/
void InitFontLib(void)
{
  //初始化SPI Flash
  InitGD25QXX();

  //更新字库
  if(0 != CheckFontLib())
  {
    printf("FontLib：字库损坏，需要更新字库!!!\r\n");
    UpdataFontLib();
  }
  else
  {
    printf("FontLib：字库校验成功\r\n");
  }
}

/*********************************************************************************************************
* 函数名称：UpdataFontLib
* 函数功能：更新字库
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、字库路径：0:/font/GBK24.FON
*          2、使用外拓SRAM会造成文件系统读取数据失败（应该是与DMA传输有关，SD卡驱动使用的是DMA传输数据）
*          3、printf函数中'\r'不起作用，原因未知
*********************************************************************************************************/
void UpdataFontLib(void)
{
  static FIL s_filFont;  //字库文件（需是静态变量）
  FRESULT    result;     //文件操作返回变量
  u8*        fontBuf;    //字库缓冲区，由动态内存分配
  u32        readNum;    //实际读取的文件数量
  u32        writeAddr;  //写入SPI Flash地址
  u32        progress;   //进度

  //申请4k内存
  fontBuf = MyMalloc(SRAMIN, FONT_FIL_BUF_SIZE);
  if(NULL == fontBuf)
  {
    printf("UpdataFontLib：申请动态内存失败\r\n");
    return;
  }

  //打开文件
  result = f_open(&s_filFont, "0:/font/GBK24.FON", FA_OPEN_EXISTING | FA_READ);
  if (result !=  FR_OK)
  {
    //释放内存
    MyFree(SDRAMEX, fontBuf);

    //打印错误信息
    printf("UpdataFontLib：打开字库文件失败\r\n");
    return;
  }

  //分批次读取数据并写到SPI Flash中
  printf("UpdataFontLib：开始更新字库\r\n");
  writeAddr = 0;
  progress = 0;
  while(1)
  {
    //进度输出
    if((100 * s_filFont.fptr / s_filFont.fsize) >= (progress + 15))
    {
      progress = 100 * s_filFont.fptr / s_filFont.fsize;
      printf("UpdataFontLib：更新进度：%%%d\r\n", progress);
    }

    //从文件中读取数据到缓冲区
    result = f_read(&s_filFont, fontBuf, FONT_FIL_BUF_SIZE, &readNum);
    if (result !=  FR_OK)
    {
      //关闭文件
      f_close(&s_filFont);

      //释放内存
      MyFree(SDRAMEX, fontBuf);

      //打印错误信息
      printf("UpdataFontLib：读取数据失败\r\n");
      return;
    }

    //将字库数据写入SPI Flash
    GD25Q16Write(fontBuf, readNum, writeAddr);

    //更新写入地址
    writeAddr = writeAddr + readNum;

    //判断文件是否读写完成
    if((s_filFont.fptr >= s_filFont.fsize) || (FONT_FIL_BUF_SIZE != readNum))
    {
      printf("UpdataFontLib：更新进度：%%100\r\n");
      printf("UpdataFontLib：更新字库完毕\r\n");
      break;
    }
  }

  //关闭文件
  f_close(&s_filFont);

  //释放内存
  MyFree(SDRAMEX, fontBuf);

  //更新成功提示
  printf("UpdataFontLib：更新字库成功\r\n");
}

/*********************************************************************************************************
* 函数名称：GetCNFont24x24
* 函数功能：获取24x24汉字点阵数据
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、24x24汉字点阵数据固定为72字节
*          2、使用GBK码
*          3、扫描方式从上到下、从左往右，字节高位在前，低位在后
*********************************************************************************************************/
void GetCNFont24x24(u32 code, u8* buf)
{
  u8  gbkH, gbkL; //GBK码高位、低位
  u32 addr;       //点阵数据在SPI Flash中的地址
  u32 i;          //循环变量

  //拆分GBK码高位、低位
  gbkH = code >> 8;
  gbkL = code & 0xFF;

  //校验高位
  if((gbkH < 0x81) || (gbkH > 0xFE))
  {
    for(i = 0; i < 72; i++)
    {
      buf[i] = 0;
    }
    return;
  }

  //低位处在0x40~0x7E范围
  if((gbkL >= 0x40) && (gbkL <= 0x7E))
  {
    addr = ((gbkH - 0x81) * 190 + (gbkL - 0x40)) * 72;
    GD25Q16Read(buf, 72, addr);
  }

  //低位处在0x80~0xFE范围
  else if((gbkL >= 0x80) && (gbkL <= 0xFE))
  {
    addr = ((gbkH - 0x81) * 190 + (gbkL - 0x41)) * 72;
    GD25Q16Read(buf, 72, addr);
  }

  //出错
  else
  {
    for(i = 0; i < 72; i++)
    {
      buf[i] = 0;
    }
  }
}
