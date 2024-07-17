/*********************************************************************************************************
* 模块名称：DCI.c
* 摘    要：数字摄像头接口驱动模块
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
#include "DCI.h"
#include "gd32f470x_conf.h"
#include "stdio.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
#define DCI_DR_ADDRESS     (0x50050028U)
#define DCI_TIMEOUT        20000

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
u32 s_iFrameAddr, s_iFrameSize;        //帧地址以及帧大小
u32 s_iX0, s_iY0, s_iWidth, s_iHeight; //图像显示信息
u32 s_iMode;                           //显示模式，0-直接加载到显存，1-加载到临时缓冲区后再显示
u32 s_iGetFrameFlag;                   //获得1帧图像标志位

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void ConfigDCIGPIO(void); //初始化DCI的GPIO
static void ConfigCKOut0(void);  //配置CKOut0输出
void ConfigDCI(void);     //配置DCI

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ConfigDCIGPIO
* 函数功能：初始化DCI的GPIO
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ConfigDCIGPIO(void)
{
  //使能RCC相关时钟
  rcu_periph_clock_enable(RCU_DCI);    //使能DCI时钟
  rcu_periph_clock_enable(RCU_GPIOA);  //使能GPIOA的时钟
  rcu_periph_clock_enable(RCU_GPIOB);  //使能GPIOB的时钟
  rcu_periph_clock_enable(RCU_GPIOC);  //使能GPIOC的时钟
  rcu_periph_clock_enable(RCU_GPIOD);  //使能GPIOD的时钟
  rcu_periph_clock_enable(RCU_GPIOH);  //使能GPIOH的时钟
  rcu_periph_clock_enable(RCU_GPIOI);  //使能GPIOI的时钟

  //DCI D0
  gpio_af_set(GPIOH, GPIO_AF_13, GPIO_PIN_9);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);

  //DCI D1
  gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_7);
  gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //DCI D2
  gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_8);
  gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //DCI D3
  gpio_af_set(GPIOH, GPIO_AF_13, GPIO_PIN_12);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

  //DCI D4
  gpio_af_set(GPIOH, GPIO_AF_13, GPIO_PIN_14);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);

  //DCI D5
  gpio_af_set(GPIOD, GPIO_AF_13, GPIO_PIN_3);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);

  //DCI D6
  gpio_af_set(GPIOI, GPIO_AF_13, GPIO_PIN_6);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);

  //DCI D7
  gpio_af_set(GPIOI, GPIO_AF_13, GPIO_PIN_7);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //DCI VSYNC
  gpio_af_set(GPIOI, GPIO_AF_13, GPIO_PIN_5);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //DCI HSYNC
  gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4);
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //DCI PIXCLK
  gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_6);
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);
}

/*********************************************************************************************************
* 函数名称：ConfigCKOut0
* 函数功能：配置CKOut0输出
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ConfigCKOut0(void)
{
  rcu_periph_clock_enable(RCU_GPIOA);
  gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_8);
  rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV3);
}

/*********************************************************************************************************
* 函数名称：ConfigDCI
* 函数功能：配置DCI
* 输入参数：frameAddr：图像帧储存地址，frameSize：1帧图像大小（按字节计算）
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：要处理DCI溢出错误中断并处理，否则DCI会直接挂掉！！！
*********************************************************************************************************/
void ConfigDCI(void)
{
  dci_parameter_struct dci_struct;
  dma_single_data_parameter_struct dma_single_struct;

  //使能RCC相关时钟
  rcu_periph_clock_enable(RCU_DCI);    //使能DCI时钟
  rcu_periph_clock_enable(RCU_DMA1);   //使能DMA1时钟

  //DCI配置
  dci_deinit();                                              //复位DCI
  if(0 == s_iMode)
  {
    dci_struct.capture_mode   = DCI_CAPTURE_MODE_CONTINUOUS; //连续捕获模式，持续将图像数据直接保存到LCD显存
  }
  else
  {
    dci_struct.capture_mode   = DCI_CAPTURE_MODE_SNAPSHOT;   //使用快照模式，通过画点函数将图像显示到LCD上
  }
  dci_struct.clock_polarity   = DCI_CK_POLARITY_RISING;      //时钟极性选择
  dci_struct.hsync_polarity   = DCI_HSYNC_POLARITY_LOW;      //水平同步极性选择
  dci_struct.vsync_polarity   = DCI_VSYNC_POLARITY_LOW;      //垂直同步极性选择
  dci_struct.frame_rate       = DCI_FRAME_RATE_ALL;          //捕获所有帧
  dci_struct.interface_format = DCI_INTERFACE_FORMAT_8BITS;  //每个像素时钟捕获 8 位数据
  dci_init(&dci_struct);                                     //根据参数配置DCI
  nvic_irq_enable(DCI_IRQn, 2, 0);                           //使能DCI NVIC中断
  dci_interrupt_enable(DCI_INT_OVR);                         //使能DCI FIFO溢出中断（非常重要！！！）
  dci_interrupt_flag_clear(DCI_INT_OVR);                     //清除溢出中断标志位
  if(0 != s_iMode)
  {
    dci_interrupt_enable(DCI_INT_EF);                        //帧结束中断使能，用以绘制一帧图像到LCD上并触发下一次捕获
    dci_interrupt_flag_clear(DCI_INT_EF);                    //清除帧结束中断标志位
  }

  //DMA配置
  dma_deinit(DMA1, DMA_CH7);                                           //复位DMA
  dma_single_struct.periph_addr         = (u32)&DCI_DATA;              //设置外设地址
  dma_single_struct.memory0_addr        = (u32)s_iFrameAddr;           //设置存储器地址
  dma_single_struct.direction           = DMA_PERIPH_TO_MEMORY;        //传输方向为外设到内存
  dma_single_struct.number              = s_iFrameSize / 4;            //设置传输数据量
  dma_single_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE; //禁止外设地址增长
  dma_single_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;  //允许内存地址增长
  dma_single_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;      //传输数据宽度为32位
  dma_single_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;     //高超优先级
  if(0 == s_iMode)
  {
    dma_single_struct.circular_mode       = DMA_CIRCULAR_MODE_ENABLE;  //DCI连续捕获模式下开启循环模式
  }
  else
  {
    dma_single_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE; //DCI快照模式下禁用循环模式
  }
  dma_single_data_mode_init(DMA1, DMA_CH7, &dma_single_struct);        //根据参数配置DMA通道
  dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI1);       //绑定DMA通道与相应外设
  dma_flow_controller_config(DMA1, DMA_CH7, DMA_FLOW_CONTROLLER_DMA);  //DMA作为传输控制器
  dma_channel_enable(DMA1, DMA_CH7);                                   //使能DMA传输

  dci_enable();         //使能DCI
  dci_capture_enable(); //开启传输
}

/*********************************************************************************************************
* 函数名称：DCI_IRQHandler
* 函数功能：DCI中断服务函数
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DCI_IRQHandler(void)
{
  //帧结束
  if(SET == dci_interrupt_flag_get(DCI_INT_FLAG_EF))
  {
    //标记获得1帧数据
    s_iGetFrameFlag = 1;

    //清除中断标志位
    dci_interrupt_flag_clear(DCI_INT_EF);
  }

  //FIFO缓冲区溢出，抛弃当前帧，直接开启下一帧图像传输
  if(SET == dci_interrupt_flag_get(DCI_INT_FLAG_OVR))
  {
    dci_interrupt_flag_clear(DCI_INT_OVR);
    ConfigDCI();
  }
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitDCI
* 函数功能：初始化数字摄像头接口驱动模块
* 输入参数：frameAddr：图像帧储存地址，可以是内部SRAM，也可以是外部SDRAM
*          frameSize：1帧图像大小（按字节计算）
*          x0，Y0：图像显示起点，mode非零时使用
*          width，heigh：图像大小，mode非零时使用
*          mode：0-推荐横屏时使用，利用DMA直接将DCI接收到的数据传输到显存，无需额外的内存空间，刷新速度快
*                1-将DCI接收到的数据保存到临时缓冲区，然后再通过画点函数将一帧图像绘制出来，速度较慢
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：横屏时推荐开启双层显示，选择模式0，DMA直接将数据传输到显存，可以提高刷新率
*********************************************************************************************************/
void InitDCI(u32 frameAddr, u32 frameSize, u32 x0, u32 y0, u32 width, u32 height, u32 mode)
{
  //保存参数
  s_iFrameAddr = frameAddr;
  s_iFrameSize = frameSize;
  s_iX0 = x0;
  s_iY0 = y0;
  s_iWidth = width;
  s_iHeight = height;
  s_iMode = mode;

  //配置DCI的GPIO
  ConfigDCIGPIO();

  //配置CK_OUT0输出
  ConfigCKOut0();

  //开启DCI传输
  s_iGetFrameFlag = 0;
  ConfigDCI();
}

/*********************************************************************************************************
* 函数名称：DeInitDCI
* 函数功能：注销数字摄像头接口驱动模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DeInitDCI(void)
{
  //关闭DMA传输
  dma_channel_disable(DMA1, DMA_CH7);

  //复位DCI
  dci_deinit();

  //禁止DCI_XCLK输出
  gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
}

/*********************************************************************************************************
* 函数名称：DCIShowImage
* 函数功能：显示一帧图像到屏幕上
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void DCIShowImage(void)
{
  u32 x0, y0, x1, y1, x, y, i;
  u16* frame;

  if((0 != s_iMode) && (1 == s_iGetFrameFlag))
  {
    //将一帧图像显示到屏幕上
    x0 = s_iX0;
    y0 = s_iY0;
    x1 = x0 + s_iWidth - 1;
    y1 = y0 + s_iHeight - 1;
    i = 0;
    frame = (u16*)s_iFrameAddr;
    for(y = y0; y <= y1; y++)
    {
      for(x = x0; x <= x1; x++)
      {
        LCDDrawPoint(x, y, frame[s_iWidth*s_iHeight-1-i++]);
      }
    }

    //开启下一帧传输
    s_iGetFrameFlag = 0;
    ConfigDCI();
  }
}
