/*********************************************************************************************************
* 模块名称: Malloc.h
* 摘    要: Malloc模块
* 当前版本: 1.0.0
* 作    者: SZ-BMQ(COPYRIGHT 2018 SZ-BMQ. All rights reserved.)
* 完成日期: 2018年01月01日 
* 内    容:
* 注    意: none                                                                  
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/
#ifndef _MALLOC_H_
#define _MALLOC_H_

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "DataType.h"
#include "stdio.h"
#include "gd32f470x_conf.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//定义三个内存池
#define SRAMTCM                 0                               //TCMSRAM，只能由Cortex-M4内核通过数据总线访问
#define SRAMIN                  1                               //内部内存池
#define SDRAMEX                  2                               //外部内存池(SDRAM)

#define SRAMBANK                3                               //定义支持的SRAM块数

//mem0内存参数设定，mem0完全处于TCMSRAM里面
#define MEM0_BLOCK_SIZE         32                              //内存块大小为32字节
#define MEM0_MAX_SIZE           32//1 * 1024                       //最大管理内存60K
#define MEM0_ALLOC_TABLE_SIZE   MEM0_MAX_SIZE/MEM0_BLOCK_SIZE   //内存表大小

//mem1内存参数设定.mem1完全处于内部SRAM里面
#define MEM1_BLOCK_SIZE         32                              //内存块大小为32字节
#define MEM1_MAX_SIZE           32//1 * 1024                      //最大管理内存200K
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   //内存表大小

//mem2内存参数设定.mem2的内存池处于外部SRAM里面
#define MEM2_BLOCK_SIZE         32                              //内存块大小为32字节
#define MEM2_MAX_SIZE           11 * 1024 * 1024               //最大管理内存11M
#define MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   //内存表大小

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/
//内存管理控制器
typedef struct
{
  void (*init)(u8);            //初始化
  u16  (*perused)(u8);         //内存使用率
  u8    *memoryBase[SRAMBANK]; //内存池 管理SRAMBANK个区域的内存
  u32   *memoryMap[SRAMBANK];  //内存管理状态表
  u8     memoryRdy[SRAMBANK];  //内存管理是否就绪
}StructMallocDeviceDef;

/*********************************************************************************************************
*                                              API函数声明
*********************************************************************************************************/
void  InitMemory(u8 memx);                      //初始化Malloc模块
u16   MemoryPerused(u8 memx);                   //获得内存使用率(外/内部调用) 
void  MyFree(u8 memx, void* ptr);               //内存释放
void* MyMalloc(u8 memx, u32 size);              //内存分配
void* MyRealloc(u8 memx, void* ptr, u32 size);  //重新分配内存


#endif


