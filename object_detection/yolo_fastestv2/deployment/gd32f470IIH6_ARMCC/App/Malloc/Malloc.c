/*********************************************************************************************************
* 模块名称: Malloc.c
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

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "Malloc.h"
#include "gd32f470x_conf.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量定义
*********************************************************************************************************/
//内存池(32字节对齐)
__align(32) u8 Memory0Base[MEM0_MAX_SIZE]; //__attribute__((at((u32)0x10000000)));                 //TCMSRAM内存池
__align(32) u8 Memory1Base[MEM1_MAX_SIZE];                                                      //内部SRAM内存池
__align(32) u8 Memory2Base[MEM2_MAX_SIZE] __attribute__((at((u32)0xD0000000 + 4 * 801 * 480))); //SDRAM内存池

//内存管理表
u32 Memory0MapBase[MEM0_ALLOC_TABLE_SIZE];// __attribute__((at((u32)0x10000000 + MEM0_MAX_SIZE)));                 //TCMSRAM内存池MAP
u32 Memory1MapBase[MEM1_ALLOC_TABLE_SIZE];                                                                      //内部SRAM内存池MAP
u32 Memory2MapBase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at((u32)0xD0000000 + 4 * 801 * 480 + MEM2_MAX_SIZE))); //SDRAM内存池MAP

//内存管理参数
const u32 c_iMemoryTblSize[SRAMBANK] = {MEM0_ALLOC_TABLE_SIZE, MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE}; //内存表大小
const u32 c_iMemoryBlkSize[SRAMBANK] = {MEM0_BLOCK_SIZE, MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE};                   //内存分块大小
const u32 c_iMemorySize[SRAMBANK]    = {MEM0_MAX_SIZE, MEM1_MAX_SIZE, MEM2_MAX_SIZE};                         //内存总大小

//内存管理控制器
static StructMallocDeviceDef s_structMallocDev =
{
  InitMemory,     //内存初始化
  MemoryPerused,  //内存使用率
  Memory0Base,    //内存池
  Memory1Base,    //内存池
  Memory2Base,    //内存池
  Memory0MapBase, //内存管理状态表
  Memory1MapBase, //内存管理状态表
  Memory2MapBase, //内存管理状态表
  0,              //内存管理未就绪
  0,              //内存管理未就绪
  0,              //内存管理未就绪
};

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  SetMemory(void* s, u8 c, u32 count);      //设置内存
static  void  CopyMemory(void* des, void* src, u32 n);  //复制内存
static  u32   MallocMemory(u8 memx, u32 size);          //内存分配
static  u8    FreeMemory(u8 memx, u32 offset);          //内存释放

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: SetMemory
* 函数功能: 设置内存
* 输入参数: *s：内存首地址；c：要设置的值；count：需要设置的内存大小（字节为单位）
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
static  void  SetMemory(void* s, u8 c, u32 count)
{
  u8 *xs = s;
	
  while(count--)
  {
    *xs++ = c;
  }
}

/*********************************************************************************************************
* 函数名称: CopyMemory
* 函数功能: 复制内存
* 输入参数: *des：目的地址；*src：源地址；n：需要复制的内存长度（字节为单位）
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
static void  CopyMemory(void* des, void* src, u32 n)
{
  u8 *xdes = des;
  u8 *xsrc = src; 
  while(n--)
  {
    *xdes++ = *xsrc++;
  }
}

/*********************************************************************************************************
* 函数名称: MallocMemory
* 函数功能: 内存分配
* 输入参数: memx：所属内存块；size：要分配的内存大小（字节）
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
static  u32   MallocMemory(u8 memx, u32 size)
{
  signed long offset = 0;
  u32 nmemb;     //需要的内存块数
  u32 cmemb = 0; //连续空内存块数
  u32 i;
  
  if(!s_structMallocDev.memoryRdy[memx])
  {
    s_structMallocDev.init(memx);//未初始化,先执行初始化
  }
  if(size == 0)
  {
    return 0XFFFFFFFF;//不需要分配
  }
  
  nmemb = size / c_iMemoryBlkSize[memx];  //获取需要分配的连续内存块数
  if(size % c_iMemoryBlkSize[memx])
  {
    nmemb++;
  }
  for(offset = c_iMemoryTblSize[memx] - 1; offset >= 0; offset--)//搜索整个内存控制区  
  {
    if(!s_structMallocDev.memoryMap[memx][offset])
    {
      cmemb++;//连续空内存块数增加
    }
    else
    {
      cmemb = 0;                  //连续内存块清零
    }
    if(cmemb == nmemb)            //找到了连续nmemb个空内存块
    {
      for(i = 0; i < nmemb; i++)  //标注内存块非空
      {
        s_structMallocDev.memoryMap[memx][offset + i] = nmemb;
      }
      return (offset * c_iMemoryBlkSize[memx]);//返回偏移地址
    }
  }
  return 0XFFFFFFFF;//未找到符合分配条件的内存块
}

/*********************************************************************************************************
* 函数名称: FreeMemory
* 函数功能: 内存释放
* 输入参数: memx：所属内存块；size：内存地址偏移
* 输出参数: void
* 返 回 值: 返回值：0，释放成功；1，释放失败；2：偏移超区
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
static  u8    FreeMemory(u8 memx, u32 offset)
{
  int i;
  int index;
  int nmemb;

  if(!s_structMallocDev.memoryRdy[memx])//未初始化,先执行初始化
	{
		s_structMallocDev.init(memx);
    return 1;//未初始化  
  }  
  
  if(offset < c_iMemorySize[memx])//偏移在内存池内
  {
    index = offset / c_iMemoryBlkSize[memx];  //偏移所在内存块号码
    nmemb = s_structMallocDev.memoryMap[memx][index]; //内存块数量
    for(i = 0; i < nmemb; i++)                //内存块清零
    {  
      s_structMallocDev.memoryMap[memx][index+i] = 0;
    }
    return 0;
  }
  else
  {
    return 2;//偏移超区了
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称: InitMemory
* 函数功能: 初始化Memory模块
* 输入参数: void
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
void InitMemory(u8 memx)
{
  //使能TCMSRAM时钟
  if(SRAMTCM == memx)
  {
    rcu_periph_clock_enable(RCU_TCMSRAM);
  }

  //内存状态表数据清零
  SetMemory(s_structMallocDev.memoryMap[memx], 0, c_iMemoryTblSize[memx] * 4);

  //内存管理初始化OK
  s_structMallocDev.memoryRdy[memx] = 1;                                   //内存管理初始化OK
}

/*********************************************************************************************************
* 函数名称: MemoryPerused
* 函数功能: 获得内存使用率
* 输入参数: memx：所属内存块
* 输出参数: void
* 返 回 值: 使用率（扩大了10倍，0~1000，代表0.0%~100.0%）
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
u16 MemoryPerused(u8 memx)
{
  u32 used = 0;
  u32 i;
  
  for(i = 0; i < c_iMemoryTblSize[memx]; i++)  
  {  
    if(s_structMallocDev.memoryMap[memx][i])
    {
      used++;
    }
  }
  
  return (used * 1000) / (c_iMemoryTblSize[memx]);  
}

/*********************************************************************************************************
* 函数名称: MyFree
* 函数功能: 内存释放
* 输入参数: memx：所属内存块；ptr：内存首地址 
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
void MyFree(u8 memx, void* ptr)
{
  u32 offset;
  if(ptr == NULL)
  {
    return;//地址为0
  }
  offset = (u32)ptr - (u32)s_structMallocDev.memoryBase[memx];
  FreeMemory(memx, offset); //释放内存
}

/*********************************************************************************************************
* 函数名称: MyMalloc
* 函数功能: 内存分配
* 输入参数: memx：所属内存块；size：内存大小（字节）
* 输出参数: void
* 返 回 值: 返回值：分配到的内存首地址
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
void* MyMalloc(u8 memx, u32 size)
{
  u32 offset;

  offset = MallocMemory(memx, size);
  if(offset == 0XFFFFFFFF)
  {
    return NULL;
  }
  else
  {
    return (void*)((u32)s_structMallocDev.memoryBase[memx] + offset);
  }
}

/*********************************************************************************************************
* 函数名称: MyRealloc
* 函数功能: 重新分配内存
* 输入参数: memx：所属内存块；ptr：内存首地址；size：内存大小（字节）
* 输出参数: void
* 返 回 值: 返回值：分配到的内存首地址
* 创建日期: 2018年01月01日
* 注    意:
*********************************************************************************************************/
void* MyRealloc(u8 memx, void* ptr, u32 size)
{
  u32 offset;

  offset = MallocMemory(memx, size);
  if(offset == 0XFFFFFFFF)
  {
    return NULL;
  }
  else
  {
    //拷贝旧内存内容到新内存
    CopyMemory((void*)((u32)s_structMallocDev.memoryBase[memx] + offset), ptr, size);

    //释放旧内存
    MyFree(memx, ptr);

    //返回新内存首地址
    return (void*)((u32)s_structMallocDev.memoryBase[memx] + offset);
  }
}



