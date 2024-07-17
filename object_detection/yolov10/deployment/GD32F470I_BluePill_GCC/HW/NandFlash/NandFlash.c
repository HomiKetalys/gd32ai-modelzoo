/*********************************************************************************************************
* 模块名称：NandFlash.c
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

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "NandFlash.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void NandDelay(u32 time); //Nand延时函数
static u32  NANDWaitRB(u8 rb);

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：NandDelay
* 函数功能：Nand延时函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void NandDelay(u32 time)
{
  while(time > 0)
  {
    time--;
  }
}

/*********************************************************************************************************
* 函数名称：NAND_WaitRB
* 函数功能：等待RB信号为某个电平
* 输入参数：rb:0,等待RB==0
*             1,等待RB==1
* 输出参数：void
* 返 回 值：0-成功，1-超时
* 创建日期：2018年01月01日
* 注    意：如果MCU主频较低，则不建议用NANDWaitRB函数校验NWAIT信号下降沿和上升沿，因为有可能NWAIT信号变化太快，
*           MCU检测不到，此时直接用延时就好
*********************************************************************************************************/
static u32 NANDWaitRB(u8 rb)
{
  u32 time = 0;
  while(time < 0X1FFFFFF)
  {
    time++;
    if(gpio_input_bit_get(GPIOD, GPIO_PIN_13) == rb)
    {
      return 0;
    }
  }
  return 1;
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitNandFlash
* 函数功能：初始化NandFlash驱动模块
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，其它-失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 InitNandFlash(void)
{
  u32 id;
  exmc_nand_parameter_struct nand_init_struct;
  exmc_nand_pccard_timing_parameter_struct nand_timing_init_struct;

  //使能EXMC时钟
  rcu_periph_clock_enable(RCU_EXMC);
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_GPIOE);

  //D0
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_14);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);

  //D1
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_15);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);

  //D2
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_0);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //D3
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_1);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //D4
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_7);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //D5
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_8);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //D6
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_9);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);

  //D7
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_10);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //CLE(EXMC_A16)
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_11);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

  //ALE(EXMC_A17)
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_12);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

  //NWE(PD5)
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_5);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //NOE(PD4)
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_4);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_4);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //NCE1(PD7)
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_7);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //NWAIT
  gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP  , GPIO_PIN_13);

  //配置EXMC（Nand Flash、SDRAM和TLILCD共用EXMC总线，Nand Flash读写大量数据时可能会导致屏幕花屏，这是正常现象，读写完成后就正常了）
  // nand_timing_init_struct.setuptime       = 3; //建立时间
  // nand_timing_init_struct.waittime        = 3; //等待时间
  // nand_timing_init_struct.holdtime        = 2; //保持时间
  // nand_timing_init_struct.databus_hiztime = 1; //高阻态时间
  nand_timing_init_struct.setuptime       = 5; //建立时间
  nand_timing_init_struct.waittime        = 4; //等待时间
  nand_timing_init_struct.holdtime        = 2; //保持时间
  nand_timing_init_struct.databus_hiztime = 2; //高阻态时间
  nand_init_struct.nand_bank              = EXMC_BANK1_NAND;            //NAND挂在BANK1上
  nand_init_struct.ecc_size               = EXMC_ECC_SIZE_2048BYTES;    //ECC页大小为2048字节
  nand_init_struct.atr_latency            = EXMC_ALE_RE_DELAY_1_HCLK;   //设置TAR
  nand_init_struct.ctr_latency            = EXMC_CLE_RE_DELAY_1_HCLK;   //设置TCLR
  nand_init_struct.ecc_logic              = ENABLE;                     //使能ECC
  nand_init_struct.databus_width          = EXMC_NAND_DATABUS_WIDTH_8B; //8位数据宽度
  nand_init_struct.wait_feature           = DISABLE;                    //关闭等待特性
  nand_init_struct.common_space_timing    = &nand_timing_init_struct;   //通用时序配置
  nand_init_struct.attribute_space_timing = &nand_timing_init_struct;   //空间时序配置
  exmc_nand_init(&nand_init_struct); //根据参数初始化Nand Flash
  exmc_nand_enable(EXMC_BANK1_NAND); //使能Nand Flash

  NandReset();       //复位NAND
  DelayNms(100);     //等待100ms
  id = NandReadID(); //读取ID
  printf("InitNandFlash: Nand Flash ID: 0x%08X\r\n", id);
  if(0x1D80F1AD != id)
  {
    printf("InitNandFlash: fail to check nand flash ID\r\n");
    while(1);
  }

  return 0;
}

/*********************************************************************************************************
* 函数名称：NandReadID
* 函数功能：读器件ID
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandReadID(void)
{
  u32 id = 0;

  //发送读器件ID命令
  NAND_CMD_AREA = NAND_CMD_READID;
  NAND_ADDR_AREA = 0x00;

  //tAR等待延迟
  NandDelay(NAND_TAR_DELAY);

  //tREA等待延时
  NandDelay(NAND_TREA_DELAY);

  //读取器件ID
  id = *(__IO u32 *)(BANK_NAND_ADDR | EXMC_DATA_AREA);
  return id;
}

/*********************************************************************************************************
* 函数名称：NandReadStatus
* 函数功能：读器件状态
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandReadStatus(void)
{
  u8 data;
  u8 status = NAND_BUSY;

  //发送读取状态命令
  NAND_CMD_AREA = NAND_CMD_STATUS;

  //tWHR等待延迟
  NandDelay(NAND_TWHR_DELAY);

  //tREA等待延时
  NandDelay(NAND_TREA_DELAY);

  //读取器件状态
  data = NAND_DATA_AREA;

  //出错
  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  }

  //器件就绪
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }

  //忙碌
  else
  {
    status = NAND_BUSY;
  }

  //返回器件状态
  return status;
}

/*********************************************************************************************************
* 函数名称：NandWaitReady
* 函数功能：等待器件就绪
* 输入参数：void
* 输出参数：void
* 返 回 值：状态标志位
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandWaitReady(void)
{
  u32 timeout = 0x10000;
  u32 status = NAND_READY;

  //获取器件状态
  status = NandReadStatus();

  //超时等待
  while((status != NAND_READY) && (timeout != 0x00))
  {
    status = NandReadStatus();
    timeout--;
  }

  //等待超时
  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  return status;
}

/*********************************************************************************************************
* 函数名称：NandWritePage
* 函数功能：页写(main区)
* 输入参数：block ：块编号
*          page  ：块内页编号
*          buf   ：缓冲区地址
* 输出参数：void
* 返 回 值：0-成功，1-写入失败
* 创建日期：2021年07月01日
* 注    意：
*          1、只读写入Main区，只能按页读写，写入之前需手动擦除页所在块
*          2、第一个字节：CA7  CA6  CA5  CA4  CA3  CA2  CA1  CA0  （低8位列地址）
*          3、第二个字节：0    0    0    0    CA11 CA10 CA9  CA8  （高4位列地址）
*          4、第三个字节：BA1  BA0  PA5  PA4  PA3  PA2  PA1  PA0  （低2位块地址和页地址）
*          5、第四个字节：BA9  BA8  BA7  BA6  BA5  BA4  BA3  BA2  （高8位块地址）
*          6、ECC值分别储存在[16:19]
*********************************************************************************************************/
u32 NandWritePage(u32 block, u32 page, u8* buf)
{
  u32 i;        //循环变量
  u32 ecc;      //硬件ECC值
  u32 eccAddr;  //ECC存储地址
  u8  check;    //成功写入标志位

  //设置写入地址
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //发送写命令
  NAND_ADDR_AREA = 0;                            //列地址低位
  NAND_ADDR_AREA = 0;                            //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和页地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NandDelay(NAND_TADL_DELAY);                    //tADL等待延迟

  //复位ECC
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

  //写入数据
  for(i = 0; i < NAND_PAGE_SIZE; i++)
  {
    //写入数据
    NAND_DATA_AREA = buf[i];
  }

  //获取ECC
  //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
  DelayNus(10);
  ecc = exmc_ecc_get(EXMC_BANK1_NAND);

  //计算写入ECC的spare区地址
  eccAddr = NAND_PAGE_SIZE + 16;

  //设置写入Spare位置
  NandDelay(NAND_TADL_DELAY);             //tADL等待延迟
  NAND_CMD_AREA  = 0x85;                  //随机写命令
  NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //随机写地址低位
  NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //随机写地址高位
  NandDelay(NAND_TADL_DELAY);             //tADL等待延迟

  //将ECC写入Spare区指定位置
  NAND_DATA_AREA = (ecc >> 0)  & 0xFF;
  NAND_DATA_AREA = (ecc >> 8)  & 0xFF;
  NAND_DATA_AREA = (ecc >> 16) & 0xFF;
  NAND_DATA_AREA = (ecc >> 24) & 0xFF;

  //发送写入结束命令
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB等待延迟
  NandDelay(NAND_TWB_DELAY);

  //tPROG等待延迟
  DelayNus(NAND_TPROG_DELAY);
  
  //检查器件状态
  if(NAND_READY != NandWaitReady())
  {
    return NAND_FAIL;
  }

  //校验写入是否成功
  check = NAND_DATA_AREA;
  if(check & 0x01)
  {
    return NAND_FAIL;
  }

  //写入成功
  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandReadPage
* 函数功能：页读(main区)
* 输入参数：block ：块编号
*          page  ：块内页编号
*          buf   ：缓冲区地址
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：
*          1、只读写入Main区，只能按页读写，写入之前需手动擦除页所在块
*          2、第一个字节：CA7  CA6  CA5  CA4  CA3  CA2  CA1  CA0  （低8位列地址）
*          3、第二个字节：0    0    0    0    CA11 CA10 CA9  CA8  （高4位列地址）
*          4、第三个字节：BA1  BA0  PA5  PA4  PA3  PA2  PA1  PA0  （低2位块地址和页地址）
*          5、第四个字节：BA9  BA8  BA7  BA6  BA5  BA4  BA3  BA2  （高8位块地址）
*          6、ECC值分别储存在[16:19]
*********************************************************************************************************/
u32 NandReadPage(u32 block, u32 page, u8* buf)
{
  u32 i;        //循环变量
  u32 eccHard;  //硬件ECC值
  u32 eccFlash; //Flash中的ECC值
  u32 eccAddr;  //ECC存储地址
  u8  spare[4]; //读写Spare缓冲区

  //设置读取地址
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //发送读命令
  NAND_ADDR_AREA = 0;                            //列地址低位
  NAND_ADDR_AREA = 0;                            //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //读命令结束
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //等待RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //等待RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR延时等待

  //复位ECC
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

  //读取数据
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);
  for(i = 0; i < NAND_PAGE_SIZE; i++)
  {
    //读取数据
    buf[i] = NAND_DATA_AREA;
  }

  //获取ECC
  //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
  DelayNus(10);
  eccHard = exmc_ecc_get(EXMC_BANK1_NAND);

  //计算读取ECC的spare区地址
  eccAddr = NAND_PAGE_SIZE + 16;

  //设置读取Spare位置
  NandDelay(NAND_TWHR_DELAY);             //tWHR等待延迟
  NAND_CMD_AREA  = 0x05;                  //随机读命令
  NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //随机读地址低位
  NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //随机读地址高位
  NAND_CMD_AREA  = 0xE0;                  //命令结束
  NandDelay(NAND_TWHR_DELAY);             //tWHR等待延迟
  NandDelay(NAND_TREA_DELAY);             //tREA等待延时

  //从Spare区指定位置读出之前写入的ECC
  spare[0] = NAND_DATA_AREA;
  spare[1] = NAND_DATA_AREA;
  spare[2] = NAND_DATA_AREA;
  spare[3] = NAND_DATA_AREA;
  eccFlash = ((u32)spare[3] << 24) | ((u32)spare[2] << 16) | ((u32)spare[1] << 8) | ((u32)spare[0] << 0);

  //校验并尝试修复数据
  //printf("NandReadPage: Ecc hard: 0x%08X, flash 0x%08X\r\n", eccHard[i], eccFlash[i]);
  if(eccHard != eccFlash)
  {
    printf("NandReadPage: Ecc err hard:0x%x, flash:0x%x\r\n", eccHard, eccFlash); 
    printf("NandReadPage: block:%d, page: %d\r\n", block, page);
    if(0 != NandECCCorrection(buf, eccFlash, eccHard))
    {
      return NAND_FAIL;
    }
  }

  //读取成功
  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandWriteSpare
* 函数功能：写Spare区
* 输入参数：block ：块编号
*          page  ：块内页编号
*          column：Spare区内列地址
*          buf   ：缓冲区地址
*          len   ：数据量
* 输出参数：void
* 返 回 值：0-成功，1-写入失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandWriteSpare(u32 block, u32 page, u32 column, u8* buf, u32 len)
{
  u32 i;     //循环变量
  u8  check; //成功写入标志位

  //数据量过多
  if((column >= NAND_PAGE_SIZE) && (len > NAND_SPARE_AREA_SIZE)){return NAND_FAIL;}

  //设置写入地址
  column = column + NAND_PAGE_SIZE;              //设置列地址偏移量
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //发送写命令
  NAND_ADDR_AREA = (column >> 0) & 0xFF;         //列地址低位
  NAND_ADDR_AREA = (column >> 8) & 0xFF;         //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NandDelay(NAND_TADL_DELAY);                    //tADL等待延迟

  //循环写入spare区
  for(i = 0; i < len; i++)
  {
    NAND_DATA_AREA = buf[i];
  }

  //发送结束命令
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB等待延迟
  NandDelay(NAND_TWB_DELAY);

  //tPROG等待延迟
  DelayNus(NAND_TPROG_DELAY);

  //检查器件状态
  if(NAND_READY == NandWaitReady())
  {
    //校验写入是否成功
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      return NAND_FAIL;
    }
    else
    {
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandReadSpare
* 函数功能：读Spare区
* 输入参数：block ：块编号
*          page  ：块内页编号
*          column：Spare区内列地址
*          buf   ：缓冲区地址
*          len   ：数据量
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandReadSpare(u32 block, u32 page, u32 column, u8* buf, u32 len)
{
  u32 i;

  //数据量过多
  if((column >= NAND_PAGE_SIZE) && (len > NAND_SPARE_AREA_SIZE)){return NAND_FAIL;}

  //设置读取地址
  column = column + NAND_PAGE_SIZE;              //设置列地址偏移量
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //发送读命令
  NAND_ADDR_AREA = (column >> 0) & 0xFF;         //列地址低位
  NAND_ADDR_AREA = (column >> 8) & 0xFF;         //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //读命令结束
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //等待RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //等待RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR延时等待

  //读取数据
  for(i = 0; i < len; i++)
  {
    buf[i] = NAND_DATA_AREA;
  }

  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandEraseBlock
* 函数功能：块擦除
* 输入参数：blockNum：块编号
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：
*          1、擦除块时发送的是页地址
*          2、坏块的分类：出厂坏块FBB和使用坏块GBB
*          3、FBB：Factory Bad Block，出厂坏块，刚出厂时就有的坏块，又被成为Masked bad block或
*             initial bad/invalid block。NAND Flash出厂时，因为制造工艺的关系，会存在一定量的坏块。
*             同时，出厂前原厂会对NAND FLASH进行测试，测试所用的块也有可能被原厂标记为坏块。
*             在出厂之前，原厂就会做对坏块进行标记，具体标记的地方是，对于现在常见的页大小为2K的NAND Flash，
*             对于NAND flash, 坏块标记一般位于每个block的第一个page页spare区的第一个字节。
*             具体可以查询不同product的datasheet。如果不是0xFF，就说明是坏块。相对应的是，所有正常的块，好的块，
*             里面所有数据都是0xFF的。
*          4、GBB：Grown Bad Block，使用坏块。在NAND Flash使用过程中，因为NAND Flash的擦写寿命有限
*            （一般不超过10万次），当使用到一定时限后也会产生坏块。如果发现Block Erase或者Page Program错误，
*             可以将把这个块标记为坏块。为了和固有坏块信息保持一致，spare area的第1个byte（字节）也会被标记为非0xFF的其他值。
*          5、实际上我们也可以对标记坏块进行擦除。这样强行擦除后，坏块信息就不复存在了。对于出厂坏块，
*             一般是不建议将标记好的信息擦除掉的。NAND FLASH工厂生产时，在一个比较宽的温度和电压范围内测试并标记了nand坏块，
*             这些坏块在一定的温度或电压下仍然可以工作，但可能在另一时刻条件发生改变后便会失效，成为一个潜在的定时炸弹。
*             如果把原厂标记的坏块进行擦除，在写入和保存数据时很容易遇到数据丢失的问题。所以对于已经标记坏块的块，不擦除
*********************************************************************************************************/
u32 NandEraseBlock(u32 blockNum)
{
  u8 backBlockFlag; //坏块标志
  u8 check;         //擦除成功校验

  if(NAND_OK == NandReadSpare(blockNum, 0, 0, (u8*)&backBlockFlag, 1))
  {
    //好快，可以擦除
    if(0xFF == backBlockFlag)
    {
      //将块地址转换为页地址
      blockNum <<= PAGE_BIT;

      //发送擦除命令
      NAND_CMD_AREA = NAND_CMD_ERASE_1ST;      //块自动擦除启动命令
      NAND_ADDR_AREA = (blockNum >> 0) & 0xFF; //页地址低位
      NAND_ADDR_AREA = (blockNum >> 8) & 0xFF; //页地址高位
      NAND_CMD_AREA = NAND_CMD_ERASE_2ND;      //擦除命令
      NandDelay(NAND_TWB_DELAY);               //tWB等待延迟

      //等待器件就绪
      if(NAND_READY == NandWaitReady())
      {
        check = NAND_DATA_AREA;
        if(check & 0x01)
        {
          //擦除失败
          return NAND_FAIL;
        }
        else
        {
          //擦除成功
          return NAND_OK;
        }
      }
      else
      {
        return NAND_FAIL;
      }
    }

    //坏块，不擦除
    else
    {
      return NAND_FAIL;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandEraseChip
* 函数功能：擦除全片
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandEraseChip(void)
{
  u32 i;

  //循环擦除所有Block
  for(i = 0; i < NAND_BLOCK_COUNT; i++)
  {
    if(NAND_OK != NandEraseBlock(i))
    {
      printf("NandEraseChip: back block: %d\r\n", i);
    }
  }
  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandReset
* 函数功能：复位
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandReset(void)
{
  //发送复位命令
  NAND_CMD_AREA = NAND_CMD_RESET;

  //等待复位完成
  if(NAND_READY == NandWaitReady())
  {
    return NAND_OK;
  }
  return NAND_FAIL;
}

/*********************************************************************************************************
* 函数名称：NandECCGetOE
* 函数功能：获取ECC的奇数位/偶数位
* 输入参数：oe:0,偶数位
*             1,奇数位
*          eccval:输入的ecc值
* 输出参数：void
* 返 回 值：计算后的ecc值
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 NandECCGetOE(u8 oe, u32 eccval)
{
  u8 i;
  u32 ecctemp = 0;

  for(i = 0; i < 32; i++)
  {
    if((i % 2) == oe)
    {
      if((eccval >> i) & 0x01)
      {
        ecctemp += 1 << (i >> 1);
      }
    }
  }
  return ecctemp;
}

/*********************************************************************************************************
* 函数名称：NandECCCorrection
* 函数功能：ECC校正函数
* 输入参数：data： 数据缓冲区
*          eccrd：读取出来，原来保存的ECC值
*          ecccl：读取数据时，硬件计算的ECC值
* 输出参数：void
* 返 回 值：0-错误已修正，其他-ECC错误(有大于2个bit的错误,无法恢复)
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 NandECCCorrection(u8* data, u32 eccrd, u32 ecccl)
{
  u16 eccrdo,eccrde,eccclo,ecccle;
  u16 eccchk = 0;
  u16 errorpos = 0;
  u32 bytepos = 0;

  eccrdo = NandECCGetOE(1, eccrd); //获取eccrd的奇数位
  eccrde = NandECCGetOE(0, eccrd); //获取eccrd的偶数位
  eccclo = NandECCGetOE(1, ecccl); //获取ecccl的奇数位
  ecccle = NandECCGetOE(0, ecccl); //获取ecccl的偶数位
  eccchk = eccrdo ^ eccrde ^ eccclo ^ ecccle;

  //全1,说明只有1bit ECC错误
  if(eccchk == 0xFFF) 
  {
    errorpos = eccrdo ^ eccclo; 
    printf("NandECCCorrection: errorpos:%d\r\n", errorpos); 
    bytepos = errorpos / 8; 
    data[bytepos] ^= 1 << (errorpos % 8);
  }

  //不是全1,说明至少有2bit ECC错误,无法修复
  else
  {
    printf("NandECCCorrection: 2bit ecc error or more\r\n");
    return 1;
  } 
  return 0;
}

/*********************************************************************************************************
* 函数名称：NandCopyPageWithoutWrite
* 函数功能：将一页数据拷贝到另一页,不写入新数据
* 输入参数：sourceBlock：源块地址
*          sourcePageNum：源页地址
*          destBlock：目的块地址
*          destPageNum：目标页地址
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandCopyPageWithoutWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage)
{
  u8 check; //成功拷贝标志位

  //判断源块和目的块奇偶是否相同
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //设置源页地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD0;                       //发送设置源页地址命令
  NAND_ADDR_AREA = 0;                                        //列地址低位
  NAND_ADDR_AREA = 0;                                        //列地址高位
  NAND_ADDR_AREA = (sourceBlock << 6) | (sourcePage & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (sourceBlock >> 2) & 0xFF;                //剩余块地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD1;                       //设置源页地址命令结束
  //if(NANDWaitRB(0)) {return NAND_FAIL;}                      //等待RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}                      //等待RB = 1

  //设置目的地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD2;                   //发送设置目的页地址命令
  NAND_ADDR_AREA = 0;                                    //列地址低位
  NAND_ADDR_AREA = 0;                                    //列地址高位
  NAND_ADDR_AREA = (destBlock << 6) | (destPage & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (destBlock >> 2) & 0xFF;              //剩余块地址
  NandDelay(NAND_TADL_DELAY);                            //tADL等待延迟
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD3;                   //设置目的页地址命令结束
  NandDelay(NAND_TWB_DELAY);                             //tWB等待延迟
  DelayNus(NAND_TPROG_DELAY);                            //tPROG等待延迟

  //校验
  if(NAND_READY == NandWaitReady())
  {
    //校验写入是否成功
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      return NAND_FAIL;
    }
    else
    {
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandCopyPageWithWrite
* 函数功能：将一页数据拷贝到另一页,并写入新数据
* 输入参数：sourceBlock：源块地址
*          sourcePageNum：源页地址
*          destBlock：目的块地址
*          destPageNum：目标页地址
*          buf：数据缓冲区
*          column：页内列地址
*          len：写入长度
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：
*          1、拷贝时块编号的奇偶性要相同
*          2、拷贝是页编号的奇偶性也要相同
*          3、写入Main区时只能按页写入
*********************************************************************************************************/
u32 NandCopyPageWithWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage, u8* buf, u32 column, u32 len)
{
  u32 i;       //循环变量
  u32 ecc;     //硬件ECC值
  u32 eccAddr; //ECC存储地址
  u8  check;   //成功拷贝标志位

  //写入main区地址校验，页内列地址不是0则返回失败
  if((column < NAND_PAGE_SIZE) && (0 != column)){return NAND_FAIL;}

  //写入main区数据量校验，写入数据量不是1页数据则返回失败
  if((column < NAND_PAGE_SIZE) && (NAND_PAGE_SIZE != len)){return NAND_FAIL;}

  //写入Spare区但数据量过多
  if((column >= NAND_PAGE_SIZE) && (column + len > NAND_PAGE_TOTAL_SIZE)){return NAND_FAIL;}

  //判断源块和目的块奇偶是否相同，以及源页与目的页的奇偶是否相同
  if(((sourceBlock % 2) != (destBlock % 2)) || ((sourcePage % 2) != (destPage % 2)))
  {
    return NAND_FAIL;
  }

  //设置源页地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD0;                       //发送设置源页地址命令
  NAND_ADDR_AREA = 0;                                        //列地址低位
  NAND_ADDR_AREA = 0;                                        //列地址高位
  NAND_ADDR_AREA = (sourceBlock << 6) | (sourcePage & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (sourceBlock >> 2) & 0xFF;                //剩余块地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD1;                       //设置源页地址命令结束
  //if(NANDWaitRB(0)) {return NAND_FAIL;}                      //等待RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}                      //等待RB = 1

  //设置目的地址
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD2;                   //发送设置目的页地址命令
  NAND_ADDR_AREA = (column >> 0) & 0xFF;                 //列地址低位
  NAND_ADDR_AREA = (column >> 8) & 0xFF;                 //列地址高位
  NAND_ADDR_AREA = (destBlock << 6) | (destPage & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (destBlock >> 2) & 0xFF;              //剩余块地址
  NandDelay(NAND_TADL_DELAY);                            //tADL等待延迟

  //写入Spare区，不需要计算ECC
  if(column >= NAND_PAGE_SIZE)
  {
    for(i = 0; i < len; i++)
    {
      //写入数据
      NAND_DATA_AREA = buf[i];
    }
  }

  //写入Main区，需要计算ECC
  else
  {
    //复位ECC
    exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
    exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

    //写入数据，按页写
    for(i = 0; i < NAND_PAGE_SIZE; i++)
    {
      //写入数据
      NAND_DATA_AREA = buf[i];
    }

    //获取ECC
    //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
    DelayNus(10);
    ecc = exmc_ecc_get(EXMC_BANK1_NAND);

    //计算写入ECC的spare区地址
    eccAddr = NAND_PAGE_SIZE + 16;

    //设置写入Spare位置
    NandDelay(NAND_TADL_DELAY);             //tADL等待延迟
    NAND_CMD_AREA  = 0x85;                  //随机写命令
    NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //随机写地址低位
    NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //随机写地址高位
    NandDelay(NAND_TADL_DELAY);             //tADL等待延迟

    //写入ECC
    NAND_DATA_AREA = (ecc >> 0)  & 0xFF;
    NAND_DATA_AREA = (ecc >> 8)  & 0xFF;
    NAND_DATA_AREA = (ecc >> 16) & 0xFF;
    NAND_DATA_AREA = (ecc >> 24) & 0xFF;
  }

  //写入结束
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD3;

  //tWB等待延迟
  NandDelay(NAND_TWB_DELAY);

  //tPROG等待延迟
  DelayNus(NAND_TPROG_DELAY);

  //等待器件就绪
  if(NAND_READY == NandWaitReady())
  {
    //校验写入是否成功
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      return NAND_FAIL;
    }
    else
    {
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandCopyBlockWithoutWrite
* 函数功能：块拷贝，不写入数据
* 输入参数：sourceBlock：源块地址
*          destBlock：目的块地址
*          startPage：开始拷贝的页编号
*          pageNum：拷贝的页数
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：拷贝时块编号的奇偶性要相同
*********************************************************************************************************/
u32 NandCopyBlockWithoutWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 pageNum)
{
  u32 i; //循环变量

  //校验块编号的奇偶性
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //校验页数
  if((startPage + pageNum) > NAND_BLOCK_SIZE)
  {
    return NAND_FAIL;
  }

  //循环拷贝所有页
  for(i = 0; i < pageNum; i++)
  {
    if(NAND_OK != NandCopyPageWithoutWrite(sourceBlock, i + startPage, destBlock, i + startPage))
    {
      return NAND_FAIL;
    }
  }

  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandCopyBlockWithWrite
* 函数功能：块拷贝，并写入新数据
* 输入参数：sourceBlock：源块地址
*          destBlock：目的块地址
*          startPage：开始写入页编号
*          column：页内列地址
*          buf：数据缓冲区
*          len：数据长度
* 输出参数：void
* 返 回 值：0-成功，1-读取失败
* 创建日期：2021年07月01日
* 注    意：
*          1、拷贝时块编号的奇偶性要相同
*          2、写入列地址要512字节对齐
*          3、写入量要512字节对齐
*          4、不能将数据写入到Spare区
*********************************************************************************************************/
u32 NandCopyBlockWithWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 column, u8* buf, u32 len)
{
  u32 ret;           //返回值
  u32 availableSize; //块内可用内存大小
  u32 copyPage;      //当前拷贝的页编号
  u32 copyColumn;    //页内拷贝列编号
  u32 copyLen;       //页内拷贝长度
  u8* copyBuf;       //页内拷贝数据缓冲区

  //校验源块与目的块奇偶性
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //校验列地址和写入量
  if((column >= NAND_PAGE_SIZE) || (0 != column % 512) || (0 != len % 512))
  {
    return NAND_FAIL;
  }

  //校验写入量是否过多
  availableSize = NAND_BLOCK_SIZE * NAND_PAGE_SIZE - NAND_PAGE_SIZE * startPage - column;
  if(len > availableSize)
  {
    return NAND_FAIL;
  }

  //先复制前边不需要写入的页
  if(0 != startPage)
  {
    ret = NandCopyBlockWithoutWrite(sourceBlock, destBlock, 0, startPage);
    if(NAND_OK != ret)
    {
      return NAND_FAIL;
    }
  }

  //复制需要写入的页
  copyBuf    = buf;
  copyPage   = startPage;
  copyColumn = column;
  copyLen    = NAND_PAGE_SIZE - column;
  if(copyLen > len){copyLen = len;}
  while(0 != copyLen)
  {
    //页拷贝
    ret = NandCopyPageWithWrite(sourceBlock, copyPage, destBlock, copyPage, copyBuf, copyColumn, copyLen);
    if(NAND_OK != ret)
    {
      return NAND_FAIL;
    }

    //统计剩余数据量
    len = len - copyLen;

    //剩余数据量为0，拷贝结束
    if(0 == len)
    {
      copyLen = 0;
    }

    //剩余的数据量大于等于1页数据
    else if(len >= NAND_PAGE_SIZE)
    {
      copyLen    = NAND_PAGE_SIZE; //设置拷贝一整页
      copyPage   = copyPage + 1;   //更新下一页
      copyColumn = 0;              //页内拷贝列编号清零
    }

    //剩余数据量不足一页数据
    else if(len < NAND_PAGE_SIZE)
    {
      copyLen    = len;          //拷贝剩余数据
      copyPage   = copyPage + 1; //更新下一页
      copyColumn = 0;            //页内拷贝列编号清零
    }
  }

  //复制剩余的页
  copyPage = copyPage + 1;
  if(copyPage < NAND_BLOCK_SIZE)
  {
    ret = NandCopyBlockWithoutWrite(sourceBlock, destBlock, copyPage, NAND_BLOCK_SIZE - copyPage);
    if(NAND_OK != ret)
    {
      return NAND_FAIL;
    }
  }

  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandCheckPage
* 函数功能：页校验，将一整页数据与某一个值比较，可用于检查页是否被写入过
* 输入参数：block：块编号，page：页编号，value：比较值，mode：0-只检查Main区，其它-检查Main区和Sapre区
* 输出参数：void
* 返 回 值：0-当前页尚未写入，1-当前页有写入过
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandCheckPage(u32 block, u32 page, u32 value, u32 mode)
{
  u32 i;    //循环变量
  u8  data; //临时变量
  u32 num;  //检查数量

  //设置读取地址
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //发送读命令
  NAND_ADDR_AREA = 0;                            //列地址低位
  NAND_ADDR_AREA = 0;                            //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //读命令结束
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //等待RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //等待RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR延时等待

  //设置检查数量
  if(0 == mode)
  {
    //只检查Main区
    num = NAND_PAGE_SIZE;
  }
  else if(1 == mode)
  {
    //检查Main区和Sapre区
    num = NAND_PAGE_TOTAL_SIZE;
  }

  //校验Main区，Spare区不校验
  for(i = 0; i < num; i++)
  {
    data = NAND_DATA_AREA;
    if(value != data)
    {
      return NAND_FAIL;
    }
  }

  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandPageFillValue
* 函数功能：页填充，将整页填充某一数值
* 输入参数：block：块编号，page：页编号，value：填充值，mode：0-只填充Main区，其它-填充Main区和Sapre区
* 输出参数：void
* 返 回 值：0-当前页尚未写入，1-当前页有写入过
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 NandPageFillValue(u32 block, u32 page, u32 value, u32 mode)
{
  u32 i;     //循环变量
  u8  check; //成功写入标志位
  u32 num;   //填充数量

  //设置写入地址
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //发送写命令
  NAND_ADDR_AREA = 0;                            //列地址低位
  NAND_ADDR_AREA = 0;                            //列地址高位
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //块地址和列地址
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //剩余块地址
  NandDelay(NAND_TADL_DELAY);                    //tADL等待延迟

  //设置填充数量
  if(0 == mode)
  {
    //只填充Main区
    num = NAND_PAGE_SIZE;
  }
  else if(1 == mode)
  {
    //填充Main区和Sapre区
    num = NAND_PAGE_TOTAL_SIZE;
  }

  //循环写入
  for(i = 0; i < num; i++)
  {
    NAND_DATA_AREA = value;
  }

  //发送结束命令
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB等待延迟
  NandDelay(NAND_TWB_DELAY);

  //tPROG等待延迟
  DelayNus(NAND_TPROG_DELAY);

  //检查器件状态
  if(NAND_READY == NandWaitReady())
  {
    //校验写入是否成功
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      return NAND_FAIL;
    }
    else
    {
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandForceEraseBlock
* 函数功能：强制块擦除
* 输入参数：blockNum：块编号
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：此函数会将官方原厂的坏块标记清除，谨慎使用
*********************************************************************************************************/
u32 NandForceEraseBlock(u32 blockNum)
{
  u8 check;

  //将块地址转换为页地址
  blockNum <<= PAGE_BIT;

  //发送擦除命令
  NAND_CMD_AREA = NAND_CMD_ERASE_1ST;      //块自动擦除启动命令
  NAND_ADDR_AREA = (blockNum >> 0) & 0xFF; //页地址低位
  NAND_ADDR_AREA = (blockNum >> 8) & 0xFF; //页地址高位
  NAND_CMD_AREA = NAND_CMD_ERASE_2ND;      //擦除命令
  NandDelay(NAND_TWB_DELAY);               //tWB等待延迟

  //等待器件就绪
  if(NAND_READY == NandWaitReady())
  {
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      //擦除失败
      return NAND_FAIL;
    }
    else
    {
      //擦除成功
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* 函数名称：NandForceEraseChip
* 函数功能：擦除全片
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：此函数会将官方原厂的坏块标记清除，谨慎使用
*********************************************************************************************************/
u32 NandForceEraseChip(void)
{
  u32 i;

  //循环擦除所有Block
  for(i = 0; i < NAND_BLOCK_COUNT; i++)
  {
    NandEraseBlock(i);
  }
  return NAND_OK;
}

/*********************************************************************************************************
* 函数名称：NandMarkAllBadBlock
* 函数功能：扫描并标记所有坏块，用于强制擦除或错误标记坏块后检查所有的Block
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，1-失败
* 创建日期：2021年07月01日
* 注    意：
*          1、使用擦-写-读方式，因为官方原厂是在比较宽的温度范围下测试，所以此函数不一定能找出所有的坏块
*          2、此函数会将官方原厂的坏块标记清除，谨慎使用
*          3、此函数执行时间会比较长，请耐心等待
*********************************************************************************************************/
u32 NandMarkAllBadBlock(void)
{
  u32 i, j;         //循环变量
  u32 ret;          //返回值
  u32 badBlockFlag; //坏块标志位
  u8  mark;         //坏块标记

  //设定标记值，不是0xFF就行
  mark = 0xCC;

  //循环检查所有的Block
  for(i = 0; i < NAND_BLOCK_COUNT; i++)
  {
    //强制擦除Block
    ret = NandForceEraseBlock(i);

    //擦除失败，表示这是一个坏块
    if(NAND_OK != ret)
    {
      printf("NandMarkAllBadBlock: bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //全1检查
    badBlockFlag = 0;
    for(j = 0; j < NAND_BLOCK_SIZE; j++)
    {
      //校验Main区和Spare区是否都是1
      ret = NandCheckPage(i, j, 0xFF, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }
    }
    
    //校验不合格，标记坏块
    if(0 != badBlockFlag)
    {
      printf("NandMarkAllBadBlock: fail to check 1, bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //全0检查
    badBlockFlag = 0;
    for(j = 0; j < NAND_BLOCK_SIZE; j++)
    {
      //首先写入0，包括Main区和Spare区
      ret = NandPageFillValue(i, j, 0x00, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }

      //校验Main区和Spare区是否都是0
      ret = NandCheckPage(i, j, 0x00, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }
    }

    //校验不合格，标记坏块
    if(0 != badBlockFlag)
    {
      printf("NandMarkAllBadBlock: fail to check 0, bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //校验成功，重新擦除该块
    ret = NandForceEraseBlock(i);

    //第二次擦除失败，表示这是一个坏块
    if(NAND_OK != ret)
    {
      printf("NandMarkAllBadBlock: bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //擦除成功，这是一个好块
    else
    {
      printf("NandMarkAllBadBlock: good block: %d\r\n", i);
    }
  }

  return NAND_OK;
}
