/*********************************************************************************************************
* 模块名称：NandFlash.h
* 摘    要：NandFlash驱动模块
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
#ifndef _NAND_FLASH_H_
#define _NAND_FLASH_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//NAND掩码定义
#define EXMC_CMD_AREA       ((u32)(1 << 16)) //命令掩码
#define EXMC_ADDR_AREA      ((u32)(1 << 17)) //地址掩码
#define EXMC_DATA_AREA      ((u32)0x0000000) //数据掩码

//Nand Flash基址定义
#define BANK1_NAND_ADDR     ((u32)0x70000000)
#define BANK_NAND_ADDR      BANK1_NAND_ADDR

//Nand Flash操作定义
#define NAND_CMD_AREA       (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_CMD_AREA))  //写命令
#define NAND_ADDR_AREA      (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_ADDR_AREA)) //写地址
#define NAND_DATA_AREA      (*(__IO u8 *)(BANK_NAND_ADDR | EXMC_DATA_AREA)) //读写数据

//Block中页地址的位数（Block编号转页编号需要）
#define PAGE_BIT            (6)

//Nand Flash常规命令(GD9FU1G8F2AMG)
#define NAND_CMD_READ1_1ST         ((u8)0x00)
#define NAND_CMD_READ1_2ND         ((u8)0x30)
#define NAND_CMD_WRITE_1ST         ((u8)0x80)
#define NAND_CMD_WRITE_2ND         ((u8)0x10)
#define NAND_CMD_ERASE_1ST         ((u8)0x60)
#define NAND_CMD_ERASE_2ND         ((u8)0xD0)
#define NAND_CMD_READID            ((u8)0x90)
#define NAND_CMD_STATUS            ((u8)0x70)
#define NAND_CMD_LOCK_STATUS       ((u8)0x7A)
#define NAND_CMD_RESET             ((u8)0xFF)

//Nand Flash拷贝数据命令
#define NAND_MOVEDATA_CMD0         ((u8)0X00)
#define NAND_MOVEDATA_CMD1         ((u8)0X35)
#define NAND_MOVEDATA_CMD2         ((u8)0X85)
#define NAND_MOVEDATA_CMD3         ((u8)0X10)

//Nand Flash状态位定义
#define NAND_BUSY                  ((u8)0x00)
#define NAND_ERROR                 ((u8)0x01)
#define NAND_READY                 ((u8)0x40)
#define NAND_TIMEOUT_ERROR         ((u8)0x80)

//Nand Flash参数设定
#define NAND_BLOCK_COUNT           ((u16)1024)   //Block总数
#define NAND_ZONE_COUNT            ((u16)0x0001) //含有一个zone（plane）
#define NAND_ZONE_SIZE             ((u16)0x0400) //一个zone含有1024个Block
#define NAND_BLOCK_SIZE            ((u16)0x0040) //一个Block含有64个Page
#define NAND_PAGE_SIZE             ((u16)0x0800) //Page的Main区含2 * 1024字节数据
#define NAND_SPARE_AREA_SIZE       ((u16)0x0040) //Page的Spare区含64个字节
#define NAND_PAGE_TOTAL_SIZE       (NAND_PAGE_SIZE + NAND_SPARE_AREA_SIZE)
#define NAND_MAX_ADDRESS           (((NAND_ZONE_COUNT*NAND_ZONE_SIZE)*NAND_BLOCK_SIZE)*NAND_PAGE_SIZE)

//定义Nand Flash操作返回值
#define NAND_OK                    (0)
#define NAND_FAIL                  (1)

//NAND FLASH操作相关延时函数，38个空循环大约是1us，这里以40个空循环计算，向上取整，最小值取1
#define NAND_TADL_DELAY            (5)     //tADL等待延迟，最少100ns
#define NAND_TWHR_DELAY            (3)     //tWHR等待延迟，最少60ns
#define NAND_TWB_DELAY             (5)     //tWB等待延迟，最大100ns
#define NAND_TAR_DELAY             (1)     //tAR等待延迟，最小15ns
#define NAND_TREA_DELAY            (1)     //tREA等待延时，最大20ns
#define NAND_TRR_DELAY             (1)     //tRR延时等待，最小20ns
#define NAND_TPROG_DELAY           (700)   //tPROG等待延迟,典型值200us,最大需要700us，由微秒延时函数实现
#define NAND_TBERS_DELAY           (4)     //tBERS等待延迟,典型值2ms,最大需要3ms，由毫秒延时函数实现

/*********************************************************************************************************
*                                              结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
u32 InitNandFlash(void);                                               //初始化NandFlash驱动模块
u32 NandReadID(void);                                                  //读器件ID
u32 NandReadStatus(void);                                              //读器件状态
u32 NandWaitReady(void);                                               //等待器件就绪
u32 NandWritePage(u32 block, u32 page, u8* buf);                       //页写
u32 NandReadPage(u32 block, u32 page, u8* buf);                        //页读
u32 NandWriteSpare(u32 block, u32 page, u32 column, u8* buf, u32 len); //写Spare区
u32 NandReadSpare(u32 block, u32 page, u32 column, u8* buf, u32 len);  //读Spare区
u32 NandEraseBlock(u32 blockNum);                                      //块擦除
u32 NandEraseChip(void);                                               //擦除全片
u32 NandReset(void);                                                   //复位
u32 NandECCGetOE(u8 oe, u32 eccval);                                   //获取ECC的奇数位/偶数位
u32 NandECCCorrection(u8* data, u32 eccrd, u32 ecccl);                 //ECC校正函数

//页拷贝
u32 NandCopyPageWithoutWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage);
u32 NandCopyPageWithWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage, u8* buf, u32 column, u32 len);

//块拷贝
u32 NandCopyBlockWithoutWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 pageNum);
u32 NandCopyBlockWithWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 column, u8* buf, u32 len);

//页校验
u32 NandCheckPage(u32 block, u32 page, u32 value, u32 mode);
u32 NandPageFillValue(u32 block, u32 page, u32 value, u32 mode);

//强制擦除
u32 NandForceEraseBlock(u32 blockNum);
u32 NandForceEraseChip(void);
u32 NandMarkAllBadBlock(void);

#endif
