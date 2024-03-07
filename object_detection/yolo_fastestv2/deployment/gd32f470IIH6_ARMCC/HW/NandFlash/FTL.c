/*********************************************************************************************************
* 模块名称：FTL.c
* 摘    要：FTL层算法
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
#include "FTL.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//FTL控制结构体
typedef struct
{
  u32  pageTotalSize;         //页总大小，main区和spare区总和
  u32  pageMainSize;          //页Main区大小
  u32  pageSpareSize;         //页Spare区大小
  u32  blockPageNum;          //块包含页数量
  u32  planeBlockNum;         //Plane包含Block数量
  u32  blockTotalNum;         //总的块数量
  u32  goodBlockNum;          //好块数量
  u32  validBlockNum;         //有效块数量(供文件系统使用的好块数量)
  u32  lut[NAND_BLOCK_COUNT]; //LUT表，用作逻辑块-物理块转换
  u32  sectorPerPage;         //每一页中含有多少个扇区
}StructFTLDev;

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructFTLDev s_structFTLDev;

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
* 函数名称：InitFTL
* 函数功能：初始化FTL层算法
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，其它-失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 InitFTL(void)
{
  //初始化FTL设备结构体
  s_structFTLDev.pageTotalSize = NAND_PAGE_TOTAL_SIZE;
  s_structFTLDev.pageMainSize  = NAND_PAGE_SIZE;
  s_structFTLDev.pageSpareSize = NAND_SPARE_AREA_SIZE;
  s_structFTLDev.blockPageNum  = NAND_BLOCK_SIZE;
  s_structFTLDev.planeBlockNum = NAND_ZONE_SIZE;
  s_structFTLDev.blockTotalNum = NAND_BLOCK_COUNT;

  //校验扇区大小，要保证扇区大小为1页数据
  if(s_structFTLDev.pageMainSize != FTL_SECTOR_SIZE)
  {
    printf("InitFTL: error sector size\r\n");
    while(1);
  }
  s_structFTLDev.sectorPerPage = s_structFTLDev.pageMainSize / FTL_SECTOR_SIZE;

  //初始化Nand Flash
  if(InitNandFlash())
  {
    return 1;
  }

  //创建LUI表
  if(0 != CreateLUT())
  {
    //生成LUT表失败，需要重新初始化NandFlash
    printf("InitFTL: format nand flash...\r\n");
    if(FTLFormat())
    {
      printf("InitFTL: format failed!\r\n");
      return 2;
    }
  }

  //创建LUT表成功，输出NandFlash块信息
  printf("\r\n");
  printf("InitFTL: total block num:%d\r\n", s_structFTLDev.blockTotalNum);
  printf("InitFTL: good block num:%d\r\n", s_structFTLDev.goodBlockNum);
  printf("InitFTL: valid block num:%d\r\n", s_structFTLDev.validBlockNum);
  printf("\r\n");

  //初始化成功
  return 0;
}

/*********************************************************************************************************
* 函数名称：FTL_BadBlockMark
* 函数功能：标记某一个块为坏块
* 输入参数：blockNum：块编号，范围：0~(blockTotalNum-1)
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void FTLBadBlockMark(u32 blockNum)
{
  //坏块标记mark，任意值都OK，只要不是0XFF
  //这里写前4个字节，方便FTL_FindUnusedBlock函数检查坏块(不检查备份区,以提高速度)
  u32 mark = 0xAAAAAAAA;

  //在第一个page的spare区，第一个字节做坏块标记(前4个字节都写)
  NandWriteSpare(blockNum, 0, 0, (u8*)&mark, 4);

  //在第二个page的spare区，第一个字节做坏块标记(备份用,前4个字节都写)
  NandWriteSpare(blockNum, 1, 0, (u8*)&mark, 4);
}

/*********************************************************************************************************
* 函数名称：FTLCheckBlockFlag
* 函数功能：检查坏块标志位
* 输入参数：blockNum：块编号，范围：0~(blockTotalNum-1)
* 输出参数：void
* 返 回 值：0-好块，其他-坏块
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 FTLCheckBlockFlag(u32 blockNum)
{
  u8 flag = 0;

  //校验第一页标志
  NandReadSpare(blockNum, 0, 0, (u8*)&flag, 1);
  if(0xFF == flag)
  {
    //校验第二页标志
    NandReadSpare(blockNum, 1, 0, (u8*)&flag, 1);
    if(0xFF == flag)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 2;
  }
}

/*********************************************************************************************************
* 函数名称：FTLSetBlockUseFlag
* 函数功能：标记某一个快已经使用
* 输入参数：blockNum：块编号，范围：0~(blockTotalNum-1)
* 输出参数：void
* 返 回 值：0-成功，其它-失败
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 FTLSetBlockUseFlag(u32 blockNum)
{
  u8 flag = 0xCC;
  return NandWriteSpare(blockNum, 0, 1, (u8*)&flag, 1);
}

/*********************************************************************************************************
* 函数名称：FTLLogicNumToPhysicalNum
* 函数功能：逻辑块号转换为物理块号
* 输入参数：logicNum：逻辑块编号
* 输出参数：void
* 返 回 值：物理块编号
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 FTLLogicNumToPhysicalNum(u32 logicNum)
{
  if(logicNum > s_structFTLDev.blockTotalNum)
  {
    return INVALID_ADDR;
  }
  else
  {
    return s_structFTLDev.lut[logicNum];
  }
}

/*********************************************************************************************************
* 函数名称：CreateLUT
* 函数功能：重新创建LUT表
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，其他-失败
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 CreateLUT(void)
{
  u32 i;        //循环变量i
  u8  spare[6]; //Spare前6个字节数据
  u32 logicNum; //逻辑块编号
  
  //清空LUT表
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    s_structFTLDev.lut[i] = INVALID_ADDR;
  }
  s_structFTLDev.goodBlockNum = 0;
  s_structFTLDev.validBlockNum = 0;

  //读取NandFlash中的LUT表
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    //读取Spare区
    NandReadSpare(i, 0, 0, spare, 6);
    if(0xFF == spare[0])
    {
      NandReadSpare(i, 1, 0, spare, 1);
    }

    //是好快
    if(0xFF == spare[0])
    {
      //得到逻辑块编号
      logicNum = ((u32)spare[5] << 24) | ((u32)spare[4] << 16) | ((u32)spare[3] << 8) | ((u32)spare[2] << 0);

      //逻辑块号肯定小于总的块数量
      if(logicNum < s_structFTLDev.blockTotalNum)
      {
        //更新LUT表
        s_structFTLDev.lut[logicNum] = i;
      }

      //好块计数
      s_structFTLDev.goodBlockNum++;
    }
    else
    {
      printf("CreateLUT: bad block index:%d\r\n",i);
    }
  }

  //LUT表建立完成以后检查有效块个数
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    if(s_structFTLDev.lut[i] < s_structFTLDev.blockTotalNum)
    {
      s_structFTLDev.validBlockNum++;
    }
  }

  //有效块数小于100,有问题.需要重新格式化
  if(s_structFTLDev.validBlockNum < 100)
  {
    return 1;
  }

  //LUT表创建完成
  return 0;
}

/*********************************************************************************************************
* 函数名称：FTLFormat
* 函数功能：格式化NAND 重建LUT表
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，其他-失败
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 FTLFormat(void)
{
  u32 i;         //循环变量
  u32 ret;       //返回值
  u32 goodBlock; //用于存储的好块数量
  u32 logicNum;  //逻辑块编号
  u8  spare[4];  //读写Spare区缓冲区

  //擦除所有好块
  s_structFTLDev.goodBlockNum = 0;
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    //检查一个块是否为坏块
    if(0 == FTLCheckBlockFlag(i))
    {
      //块擦除
      ret = NandEraseBlock(i);

      //擦除失败，则认为是坏块
      if(0 != ret)
      {
        printf("FTLFormat: back block: %d\r\n", i);
        FTLBadBlockMark(i);
      }
      else
      {
        //好块统计
        s_structFTLDev.goodBlockNum++;
      }
    }
  }

  //校验好块数量，如果好块的数量少于100，则NAND Flash报废
  printf("FTLFormat: good block num: %d\r\n", s_structFTLDev.goodBlockNum);
  if(s_structFTLDev.goodBlockNum < 100)
  {
    return 1;
  }

  //使用90%的好块用于存储数据
  goodBlock = s_structFTLDev.goodBlockNum * 90 / 100;

  //在好块中标记上逻辑块信息
  logicNum = 0;
  for(i = 0; i < s_structFTLDev.blockTotalNum; i++)
  {
    if(0 == FTLCheckBlockFlag(i))
    {
      //写入逻辑块编号
      spare[0] = (logicNum >> 0 ) & 0xFF;
      spare[1] = (logicNum >> 8 ) & 0xFF;
      spare[2] = (logicNum >> 16) & 0xFF;
      spare[3] = (logicNum >> 24) & 0xFF;
      NandWriteSpare(i, 0, 2, spare, 4);

      //逻辑块编号加一
      logicNum++;

      if(logicNum >= goodBlock)
      {
        break;
      }
    }
  }

  //重建LUT表
  if(CreateLUT())
  {
    return 2;
  }

  //格式化成功
  return 0;
}

/*********************************************************************************************************
* 函数名称：FineUnuseBlock
* 函数功能：查找未使用的块，指定奇偶
* 输入参数：startBlock：起始块编号，oddEven：奇偶状态（1-奇数，0-偶数）
* 输出参数：void
* 返 回 值：0-成功，其他-失败
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 FTLFineUnuseBlock(u32 startBlock, u32 oddEven)
{
  u32 i;
  u8  byte[6];

  //由当前位置往后查找
  for(i = startBlock + 1; i < s_structFTLDev.blockTotalNum; i++)
  {
    if((i % 2) == oddEven)
    {
      //读取Spare区前6个字节
      NandReadSpare(i, 0, 0, byte, 6);

      //找到一个空快，返回编号
      if((0xFF == byte[0]) && (0xFF == byte[1]) && (0xFF == byte[2]) && (0xFF == byte[3]) && (0xFF == byte[4]) && (0xFF == byte[5]))
      {
        return i;
      }
    }
  }

  //尚未找到，尝试从0开始查找
  for(i = 0; i < startBlock; i++)
  {
    if((i % 2) == oddEven)
    {
      //读取Spare区前6个字节
      NandReadSpare(i, 0, 0, byte, 6);

      //找到一个空快，返回编号
      if((0xFF == byte[0]) && (0xFF == byte[1]) && (0xFF == byte[2]) && (0xFF == byte[3]) && (0xFF == byte[4]) && (0xFF == byte[5]))
      {
        return i;
      }
    }
  }

  //查找不到空闲块，返回无效值
  return INVALID_ADDR;
}

/*********************************************************************************************************
* 函数名称：FTLWriteSectors
* 函数功能：写扇区(支持多扇区写)，FATFS文件系统使用
* 输入参数：pBuffer:数据缓存区
*          sectorNo:起始扇区号
*          sectorSize:扇区大小
*          sectorCount:要写入的扇区数量
* 输出参数：void
* 返 回 值：0-成功，其他-失败
* 创建日期：2018年01月01日
* 注    意：扇区大小固定为2048字节
*********************************************************************************************************/
u32 FTLWriteSectors(u8 *pBuffer, u32 sectorNo, u32 sectorSize, u32 sectorCount)
{
  u32 logicBlock;  //逻辑块编号
  u32 phyBlock;    //物理块编号
  u32 writePage;   //当前Page
  u32 writeColumn; //当前列
  u32 ret;         //返回值
  u32 emptyBlock;  //空闲块编号
  u32 copyLen;     //块复制时拷贝长度
  u32 sectorCnt;   //写入量计数

  //校验扇区大小
  if(FTL_SECTOR_SIZE != sectorSize)
  {
    return 1;
  }

  //循环写入所有扇区
  sectorCnt = 0;
  while(sectorCnt < sectorCount)
  {
RETRY1_MARK:

    //计算得到位置Block、Page以及Column
    logicBlock  = (sectorNo / s_structFTLDev.sectorPerPage) / s_structFTLDev.blockPageNum;
    writePage   = (sectorNo / s_structFTLDev.sectorPerPage) % s_structFTLDev.blockPageNum;
    writeColumn = (sectorNo % s_structFTLDev.sectorPerPage) * sectorSize;

    //逻辑块编号转物理块编号
    phyBlock = FTLLogicNumToPhysicalNum(logicBlock);
    if(INVALID_ADDR == phyBlock)
    {
      //超过了最大物理内存，写入失败
      return 1;
    }

    //校验当前页是否已被写入
    ret = NandCheckPage(phyBlock, writePage, 0xFF, 0);

    //页未被写入过，可以直接写入
    if(0 == ret)
    {
      //标记当前块已使用
      if(0 == writePage)
      {
        FTLSetBlockUseFlag(phyBlock);
      }

      //写入数据
      ret = NandWritePage(phyBlock, writePage, pBuffer);

      //写入失败，表示产生了坏块，需要将保留区中的一个块替换当前块
      if(0 != ret)
      {
        //从当前块往后查找空闲块
        emptyBlock = FTLFineUnuseBlock(phyBlock, (phyBlock % 2));
        if(INVALID_ADDR != emptyBlock)
        {
          //拷贝整个Bolck数据（包含Spare区）
          ret = NandCopyBlockWithoutWrite(phyBlock, emptyBlock, 0, s_structFTLDev.blockPageNum);

          //拷贝成功
          if(0 == ret)
          {
            //修改LUT表
            s_structFTLDev.lut[logicBlock] = emptyBlock;

            //标记当前块为坏块
            FTLBadBlockMark(phyBlock);
          }

          //拷贝失败，表示这个Block为坏块
          else
          {
            FTLBadBlockMark(emptyBlock);
          }

          //再次尝试写入数据
          goto RETRY1_MARK;
        }
        else
        {
          //标记当前块为坏块
          FTLBadBlockMark(phyBlock);

          //找不到空闲块，写入结束
          return 1;
        }
      }

      //写入成功，更新读取扇区号、缓冲区指针以及写入量计数
      else
      {
        sectorNo = sectorNo + 1;
        pBuffer = pBuffer + sectorSize;
        sectorCnt = sectorCnt + 1;
      }
    }

    //当前页有被写入过，则将数据拷贝到保留区中的Block，拷贝的同时写入新的数据
    else
    {
RETRY2_MARK:

      //从当前块往后查找空闲块
      emptyBlock = FTLFineUnuseBlock(phyBlock, (phyBlock % 2));
      if(INVALID_ADDR != emptyBlock)
      {
        //计算最大可以写入的数据量
        copyLen = s_structFTLDev.blockPageNum * s_structFTLDev.pageMainSize - s_structFTLDev.pageMainSize * writePage - writeColumn;

        //需要拷贝的数据量小于最大写入数据量，则以实际写入的为准
        if(copyLen >= ((sectorCount - sectorCnt) * sectorSize))
        {
          copyLen = (sectorCount - sectorCnt) * sectorSize;
        }

        //将当前块数据拷贝到保留区块数据，并写入数据
        ret = NandCopyBlockWithWrite(phyBlock, emptyBlock, writePage, writeColumn, pBuffer, copyLen);

        //拷贝成功
        if(0 == ret)
        {
          //修改LUT表
          s_structFTLDev.lut[logicBlock] = emptyBlock;

          //擦除当前块，使之变成保留区块
          ret = NandEraseBlock(phyBlock);
          if(0 != ret)
          {
            //擦除失败，标记为坏块
            FTLBadBlockMark(phyBlock);
          }

          //更新读取扇区号、缓冲区指针以及写入量计数
          sectorNo = sectorNo + copyLen / sectorSize;
          pBuffer = pBuffer + copyLen;
          sectorCnt = sectorCnt + copyLen / sectorSize;
        }

        //拷贝失败，表示这个Block为坏块
        else
        {
          //标记为坏块
          FTLBadBlockMark(emptyBlock);

          //再次尝试
          goto RETRY2_MARK;
        }
      }
      else
      {
        //找不到空闲块，NandFlash内存 已消耗完，写入失败
        return 1;
      }
    }
  }

  return 0;
}

/*********************************************************************************************************
* 函数名称：FTL_ReadSectors
* 函数功能：读扇区(支持多扇区读)，FATFS文件系统使用
* 输入参数：pBuffer:数据缓存区
*          sectorNo:起始扇区号
*          sectorSize:扇区大小
*          sectorCount:要读取的扇区数量
* 输出参数：void
* 返 回 值：0-成功，其他-失败
* 创建日期：2018年01月01日
* 注    意：扇区大小固定为2048字节
*********************************************************************************************************/
u32 FTLReadSectors(u8 *pBuffer, u32 sectorNo, u32 sectorSize, u32 sectorCount)
{
  u32 i;          //循环变量
  u32 readBlock;  //当前Block
  u32 readPage;   //当前Page
  //u32 readColumn; //当前列
  u32 ret;        //返回值

  //校验扇区大小
  if(FTL_SECTOR_SIZE != sectorSize)
  {
    return 1;
  }

  for(i = 0; i < sectorCount; i++)
  {
    //计算得到位置Block、Page以及Column
    readBlock  = (sectorNo / s_structFTLDev.sectorPerPage) / s_structFTLDev.blockPageNum;
    readPage   = (sectorNo / s_structFTLDev.sectorPerPage) % s_structFTLDev.blockPageNum;
    //readColumn = (sectorNo % s_structFTLDev.sectorPerPage) * sectorSize;

    //逻辑块编号转物理块编号
    readBlock = FTLLogicNumToPhysicalNum(readBlock);
    if(INVALID_ADDR == readBlock)
    {
      //超过了最大物理内存，读取失败
      return 1;
    }

    //读取数据
    ret = NandReadPage(readBlock, readPage, pBuffer);
    if(0 != ret)
    {
      //读取失败，再次尝试读取
      ret = NandReadPage(readBlock, readPage, pBuffer);
      if(0 != ret)
      {
        //尝试两次还不成功，读取失败
        return 1;
      }
    }

    //更新读取扇区号和缓冲区指针
    sectorNo = sectorNo + 1;
    pBuffer = pBuffer + sectorSize;
  }

  return 0;
}
