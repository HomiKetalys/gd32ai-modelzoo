/*********************************************************************************************************
* 模块名称：GT1151Q.c
* 摘    要：GT1151Q触摸屏驱动模块
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
#include "GT1151Q.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "IICCommon.h"
#include "stdio.h"
#include "Touch.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量定义
*********************************************************************************************************/
static StructIICCommonDev s_structIICDev; //IIC通信设备结构体

/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static  void  ConfigGT1151QGPIO(void);  //配置GT1151Q的GPIO

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ConfigGT1151QGPIO
* 函数功能：配置GT1151Q的GPIO
* 输入参数：void 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ConfigGT1151QGPIO(void)
{
  //使能RCU相关时钟
  rcu_periph_clock_enable(RCU_GPIOB);  //使能GPIOB的时钟
  
  //SCL
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);         //上拉输出
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_6); //推挽输出
  gpio_bit_set(GPIOB, GPIO_PIN_6);                                              //拉高SCL
  
  //SDA
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);         //上拉输出
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_7); //推挽输出
  gpio_bit_set(GPIOB, GPIO_PIN_7);                                              //拉高SCL

  //RST，低电平有效
  gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_9);         //上拉输出
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_9); //推挽输出
  gpio_bit_reset(GPIOG, GPIO_PIN_9);                                            //将SCL默认状态设置为拉低
}

/*********************************************************************************************************
* 函数名称：ConfigGT1151QAddr
* 函数功能：配置GT1151Q的设备地址为0x28/0x29
* 输入参数：void 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ConfigGT1151QAddr(void)
{
  //RST拉低，复位GT1151Q
  gpio_bit_reset(GPIOG, GPIO_PIN_9);

  //使能GPIOE的时钟
  rcu_periph_clock_enable(RCU_GPIOE);

  //INT设为输出
  gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_3);
  gpio_bit_reset(GPIOE, GPIO_PIN_3);

  //延时10毫秒
  DelayNms(10);

  //拉高INT
  gpio_bit_set(GPIOE, GPIO_PIN_3);

  //延时1毫秒
  DelayNms(1);

  //RST拉高，释放复位状态
  gpio_bit_set(GPIOG, GPIO_PIN_9);

  //延时10毫秒
  DelayNms(10);

  //INT设为浮空输入
  gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
}

/*********************************************************************************************************
* 函数名称：ConfigSDAMode
* 函数功能：配置SDA输入输出
* 输入参数：mode：IIC_COMMON_INPUT、IIC_COMMON_OUTPUT 
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static void ConfigSDAMode(u8 mode)
{
  rcu_periph_clock_enable(RCU_GPIOB);  //使能GPIOB的时钟
  
  //配置成输出
  if(IIC_COMMON_OUTPUT == mode)
  {
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);         //上拉输出
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_7); //推挽输出
  }
  
  //配置成输入
  else if(IIC_COMMON_INPUT == mode)
  {
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);          //上拉输入
  }
}

/*********************************************************************************************************
* 函数名称：SetSCL
* 函数功能：时钟信号线SCL控制
* 输入参数：state：0-输入低电平，1-输出高电平
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static void SetSCL(u8 state)
{
  if(1 == state)
  {
    gpio_bit_set(GPIOB, GPIO_PIN_6);
  }
  else
  {
    gpio_bit_reset(GPIOB, GPIO_PIN_6);
  }
}

/*********************************************************************************************************
* 函数名称：SetSDA
* 函数功能：数据信号线SDA控制
* 输入参数：state：0-输入低电平，1-输出高电平
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static void SetSDA(u8 state)
{
  if(1 == state)
  {
    gpio_bit_set(GPIOB, GPIO_PIN_7);
  }
  else
  {
    gpio_bit_reset(GPIOB, GPIO_PIN_7);
  }
}

/*********************************************************************************************************
* 函数名称：GetSDA
* 函数功能：获取SDA输入电平
* 输入参数：void 
* 输出参数：void
* 返 回 值：SDA输入电平
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static u8 GetSDA(void)
{
  if(RESET == gpio_input_bit_get(GPIOB, GPIO_PIN_7))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

/*********************************************************************************************************
* 函数名称：Delay
* 函数功能：延时函数
* 输入参数：time：延时时钟周期
* 输出参数：void
* 返 回 值：SDA输入电平
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
static void Delay(u8 time)
{
  DelayNus(5 * time);
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitGT1151Q
* 函数功能：初始化LED模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void InitGT1151Q(void)
{
  u8 id[5];

  //配置GT1151Q的GPIO
  ConfigGT1151QGPIO();

  //配置GT1151Q的设备地址为0x28/0x29
  ConfigGT1151QAddr();

  //配置IIC
  s_structIICDev.deviceID      = GT1151Q_DEVICE_ADDR; //设备ID
  s_structIICDev.SetSCL        = SetSCL;              //设置SCL电平值
  s_structIICDev.SetSDA        = SetSDA;              //设置SDA电平值
  s_structIICDev.GetSDA        = GetSDA;              //获取SDA输入电平
  s_structIICDev.ConfigSDAMode = ConfigSDAMode;       //配置SDA输入输出方向
  s_structIICDev.Delay         = Delay;               //延时函数

  //等待GT1151Q工作稳定
  DelayNms(100);

  //读产品ID
  if(0 != IICCommonReadBytesEx(&s_structIICDev, GT1151Q_PID_REG, id, 4, IIC_COMMON_NACK))
  {
    printf("InitGT1151Q: Fail to get id\r\n");
    return;
  }

  //打印产品ID
  id[4] = 0;
  printf("Touch ID: %s\r\n", id);
}

/*********************************************************************************************************
* 函数名称：ScanGT1151Q
* 函数功能：触屏扫描
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*          1、坐标范围480*800
*          2、触摸屏默认竖屏，且左上方为原点
*********************************************************************************************************/
void ScanGT1151Q(StructTouchDev* dev)
{
  static u16 s_arrRegAddr[5] = {GT1151Q_TP1_REG, GT1151Q_TP2_REG, GT1151Q_TP3_REG, GT1151Q_TP4_REG, GT1151Q_TP5_REG};
  u8  regValue;
  u8  buf[6];
  u8  i;
  u16 x0, y0;

  //读取状态寄存器
  IICCommonReadBytesEx(&s_structIICDev, GT1151Q_GSTID_REG, &regValue, 1, IIC_COMMON_NACK);
  regValue = regValue & 0x0F;

  //记录触摸点个数
  dev->pointNum = regValue;

  //清除状态寄存器
  regValue = 0;
  IICCommonWriteBytesEx(&s_structIICDev, GT1151Q_GSTID_REG, &regValue, 1);

  //循环获取5个触摸点数据
  for(i = 0; i < 5; i++)
  {
    //检测到触点
    if(dev->pointNum >= (i + 1))
    {
      IICCommonReadBytesEx(&s_structIICDev, s_arrRegAddr[i], buf, 6, IIC_COMMON_NACK);
      dev->point[i].x    = (buf[1] << 8) | buf[0];
      dev->point[i].y    = (buf[3] << 8) | buf[2];
      dev->point[i].size = (buf[5] << 8) | buf[4];
      
      //竖屏坐标转换
      if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
      {
        //暂存原始值
        x0 = dev->point[i].x;
        y0 = dev->point[i].y;

        //坐标系转换
        dev->point[i].x = y0;
        dev->point[i].y = LCD_PIXEL_WIDTH - x0;
      }

      //标记触摸点按下
      dev->pointFlag[i] = 1;
    }

    //未检测到触点
    else
    {
      dev->pointFlag[i] = 0;       //未检测到
      dev->point[i].x   = 0xFFFF;  //无效值
      dev->point[i].y   = 0xFFFF;  //无效值
    }
  }
}
