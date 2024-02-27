/*********************************************************************************************************
* 模块名称：SDRAM.c
* 摘    要：SDRAM模块
* 当前版本：1.0.0
* 作    者：SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* 完成日期：2020年01月01日
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
#include "SDRAM.h"
#include "gd32f4xx.h"
#include "SysTick.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
//突发长度定义
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0003)
#define SDRAM_MODEREG_BURST_FULL_PAGE            ((uint16_t)0x0007)

//突发传输方式定义
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)

//CAS潜伏期定义
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)

//突发方式写定义
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

//操作方式定义
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)

//超时退出定义
#define SDRAM_TIMEOUT                            ((uint32_t)0x0000FFFF)

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ConfigSDRAM(uint32_t sdramDevice); //配置SDRAM

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ConfigSDRAM
* 函数功能：配置SDRAM
* 输入参数：sdramDevice：SDRAM设备（EXMC_SDRAM_DEVICE0、EXMC_SDRAM_DEVICE1）
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static void ConfigSDRAM(uint32_t sdramDevice)
{
  exmc_sdram_parameter_struct         sdram_init_struct;         //SDRAM配置结构体
  exmc_sdram_timing_parameter_struct  sdram_timing_init_struct;  //SDRAM时序结构体
  exmc_sdram_command_parameter_struct sdram_command_init_struct; //SDRAM通用配置结构体

  uint32_t command_content = 0, bank_select;
  uint32_t timeout = SDRAM_TIMEOUT;

  //使能RCC
  rcu_periph_clock_enable(RCU_EXMC);
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_GPIOE);
  rcu_periph_clock_enable(RCU_GPIOF);
  rcu_periph_clock_enable(RCU_GPIOG);
  rcu_periph_clock_enable(RCU_GPIOH);
  rcu_periph_clock_enable(RCU_GPIOI);

  //A0
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_0);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //A1
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_1);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //A2
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_2);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);

  //A3
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_3);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);

  //A4
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_4);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //A5
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_5);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //A6
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_12);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

  //A7
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_13);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

  //A8
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_14);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);

  //A9
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_15);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);

  //A10
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_0);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //A11
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_1);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //A12
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_2);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);
  
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

  //D8
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_11);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

  //D9
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_12);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

  //D10
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_13);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

  //D11
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_14);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);

  //D12
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_15);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);

  //D13
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_8);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //D14
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_9);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);

  //D15
  gpio_af_set(GPIOD, GPIO_AF_12, GPIO_PIN_10);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //BA0
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_4);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //BA1
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_5);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //NBL0
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_0);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //NBL1
  gpio_af_set(GPIOE, GPIO_AF_12, GPIO_PIN_1);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //SDNWE
  gpio_af_set(GPIOH, GPIO_AF_12, GPIO_PIN_5);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //CAS
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_15);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);

  //RAS
  gpio_af_set(GPIOF, GPIO_AF_12, GPIO_PIN_11);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

  //NE
  gpio_af_set(GPIOH, GPIO_AF_12, GPIO_PIN_6);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);

  //CKE
  gpio_af_set(GPIOH, GPIO_AF_12, GPIO_PIN_7);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //CLK
  gpio_af_set(GPIOG, GPIO_AF_12, GPIO_PIN_8);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //选择Bank
  if(EXMC_SDRAM_DEVICE0 == sdramDevice)
  {
    bank_select = EXMC_SDRAM_DEVICE0_SELECT;
  }
  else
  {
    bank_select = EXMC_SDRAM_DEVICE1_SELECT;
  }

  //延时等待10ms，等待SDRAM就绪
  DelayNus(10000);

  //配置时序寄存器（以SDRAM时钟周期单元为单位）
  //80M
  sdram_timing_init_struct.load_mode_register_delay = 2; //LMRD(TMRD)：加载模式寄存器延迟（加载模式寄存器命令和激活或刷新命令之间的延迟）
  sdram_timing_init_struct.exit_selfrefresh_delay   = 6; //XSRD(TXSR)：退出自刷新延迟（从发出自刷新命令到发出激活命令之间的延迟）
  sdram_timing_init_struct.row_address_select_delay = 3; //RASD(TRAS)：行地址选择延迟（使能命令与预充电命令之间延迟多少SDRAM时钟周期单元）
  sdram_timing_init_struct.auto_refresh_delay       = 5; //ARFD(TRC)：自动刷新延迟（刷新命令和激活命令之间的延迟）（两个连续的刷新命令之间的延迟）
  sdram_timing_init_struct.write_recovery_delay     = 1; //WRD(TWR)：写恢复延迟（写命令和预充电命令之间的延迟）
  sdram_timing_init_struct.row_precharge_delay      = 1; //RPD(TRP)：行预充电延迟（预充电命令与其它命令之间的延迟）
  sdram_timing_init_struct.row_to_column_delay      = 1; //RCD(TRCD)：行到列的延迟（激活命令与读/写命令之间的延迟）
  
  //100M
//  sdram_timing_init_struct.load_mode_register_delay = 2; //LMRD：加载模式寄存器延迟（TMRD，加载模式寄存器命令和激活或刷新命令之间的延迟）
//  sdram_timing_init_struct.exit_selfrefresh_delay   = 7; //XSRD：退出自刷新延迟（TXSR，从发出自刷新命令到发出激活命令之间的延迟）
//  sdram_timing_init_struct.row_address_select_delay = 4; //RASD：行地址选择延迟（TRAS，使能命令与预充电命令之间延迟多少SDRAM时钟周期单元）
//  sdram_timing_init_struct.auto_refresh_delay       = 6; //ARFD：自动刷新延迟（TRC，刷新命令和激活命令之间的延迟）（两个连续的刷新命令之间的延迟）
//  sdram_timing_init_struct.write_recovery_delay     = 1; //WRD：写恢复延迟（TWR，写命令和预充电命令之间的延迟）
//  sdram_timing_init_struct.row_precharge_delay      = 1; //RPD：行预充电延迟（TRP，预充电命令与其它命令之间的延迟）
//  sdram_timing_init_struct.row_to_column_delay      = 1; //RCD：行到列的延迟（TRCD，激活命令与读/写命令之间的延迟）
  
  //120M
//  sdram_timing_init_struct.load_mode_register_delay = 2; //LMRD(TMRD)：加载模式寄存器延迟（加载模式寄存器命令和激活或刷新命令之间的延迟）
//  sdram_timing_init_struct.exit_selfrefresh_delay   = 9; //XSRD(TXSR)：退出自刷新延迟（从发出自刷新命令到发出激活命令之间的延迟）
//  sdram_timing_init_struct.row_address_select_delay = 5; //RASD(TRAS)：行地址选择延迟（使能命令与预充电命令之间延迟多少SDRAM时钟周期单元）
//  sdram_timing_init_struct.auto_refresh_delay       = 7; //ARFD(TRC)：自动刷新延迟（刷新命令和激活命令之间的延迟）（两个连续的刷新命令之间的延迟）
//  sdram_timing_init_struct.write_recovery_delay     = 1; //WRD(TWR)：写恢复延迟（写命令和预充电命令之间的延迟）
//  sdram_timing_init_struct.row_precharge_delay      = 2; //RPD(TRP)：行预充电延迟（预充电命令与其它命令之间的延迟）
//  sdram_timing_init_struct.row_to_column_delay      = 2; //RCD(TRCD)：行到列的延迟（激活命令与读/写命令之间的延迟）

  //配置SDRAM控制寄存器
  sdram_init_struct.sdram_device         = sdramDevice;                  //SDRAM设备
  sdram_init_struct.column_address_width = EXMC_SDRAM_COW_ADDRESS_9;     //列地址位宽
  sdram_init_struct.row_address_width    = EXMC_SDRAM_ROW_ADDRESS_13;    //行地址位宽
  sdram_init_struct.data_width           = EXMC_SDRAM_DATABUS_WIDTH_16B; //SDRAM数据总线宽度
  sdram_init_struct.internal_bank_number = EXMC_SDRAM_4_INTER_BANK;      //内部Bank的个数
  sdram_init_struct.cas_latency          = EXMC_CAS_LATENCY_2_SDCLK;     //CAS延迟
  sdram_init_struct.write_protection     = DISABLE;                      //写保护
  sdram_init_struct.sdclock_config       = EXMC_SDCLK_PERIODS_3_HCLK;    //SDRAM时钟配置
  sdram_init_struct.brust_read_switch    = ENABLE;                       //突发读开关
  sdram_init_struct.pipeline_read_delay  = EXMC_PIPELINE_DELAY_0_HCLK;   //流水线读数据延迟，指定在CAS延迟之后再延迟多少个HCLK时钟周期才去读数据
  sdram_init_struct.timing               = &sdram_timing_init_struct;    //SDRAM时序

  //根据参数配置SDRAM控制器
  exmc_sdram_init(&sdram_init_struct);

  //时钟使能
  sdram_command_init_struct.command               = EXMC_SDRAM_CLOCK_ENABLE;
  sdram_command_init_struct.bank_select           = bank_select;
  sdram_command_init_struct.auto_refresh_number   = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
  sdram_command_init_struct.mode_register_content = 0;
  timeout = SDRAM_TIMEOUT;
  while((exmc_flag_get(sdramDevice, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0))
  {
    timeout--;
  }
  exmc_sdram_command_config(&sdram_command_init_struct);
  DelayNus(10000);

  //所有存储区预充电
  sdram_command_init_struct.command               = EXMC_SDRAM_PRECHARGE_ALL;
  sdram_command_init_struct.bank_select           = bank_select;
  sdram_command_init_struct.auto_refresh_number   = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
  sdram_command_init_struct.mode_register_content = 0;
  timeout = SDRAM_TIMEOUT; 
  while((exmc_flag_get(sdramDevice, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0))
  {
    timeout--;
  }
  exmc_sdram_command_config(&sdram_command_init_struct);

  //连续8次刷新操作
  sdram_command_init_struct.command               = EXMC_SDRAM_AUTO_REFRESH;
  sdram_command_init_struct.bank_select           = bank_select;
  sdram_command_init_struct.auto_refresh_number   = EXMC_SDRAM_AUTO_REFLESH_8_SDCLK;
  sdram_command_init_struct.mode_register_content = 0;
  timeout = SDRAM_TIMEOUT; 
  while((exmc_flag_get(sdramDevice, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0))
  {
    timeout--;
  }
  exmc_sdram_command_config(&sdram_command_init_struct);

  //加载模式寄存器
  command_content = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          | //突发长度
                              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   | //突发传输方式
                              SDRAM_MODEREG_CAS_LATENCY_2           | //CAS潜伏期
                              SDRAM_MODEREG_OPERATING_MODE_STANDARD | //操作方式
                              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;   //写突发模式
  sdram_command_init_struct.command               = EXMC_SDRAM_LOAD_MODE_REGISTER;
  sdram_command_init_struct.bank_select           = bank_select;
  sdram_command_init_struct.auto_refresh_number   = EXMC_SDRAM_AUTO_REFLESH_1_SDCLK;
  sdram_command_init_struct.mode_register_content = command_content;
  timeout = SDRAM_TIMEOUT; 
  while((exmc_flag_get(sdramDevice, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0))
  {
    timeout--;
  }
  exmc_sdram_command_config(&sdram_command_init_struct);

  //设置自动刷新间隔寄存器
  //64ms, 8192-cycle refresh, 64ms/8192=7.81us
  //SDCLK_Freq = SYS_Freq/2
  //(7.81 us * SDCLK_Freq) - 20
  exmc_sdram_refresh_count_set(604); //80M
  //exmc_sdram_refresh_count_set(761); //100M
  //exmc_sdram_refresh_count_set(917); //120M

  //等待SDRAM控制器就绪
  timeout = SDRAM_TIMEOUT;
  while((exmc_flag_get(sdramDevice, EXMC_SDRAM_FLAG_NREADY) != RESET) && (timeout > 0))
  {
    timeout--;
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitSDRAM
* 函数功能：初始化SDRAM
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void InitSDRAM(void)
{
  ConfigSDRAM(EXMC_SDRAM_DEVICE1);
}
