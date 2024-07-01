/*********************************************************************************************************
* 模块名称：OV2640.h
* 摘    要：OV2640摄像头驱动
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
#include "OV2640.h"
#include "OV2640Cfg.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "stdio.h"
#include "SCCB.h"

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
static void ConfigGPIO(void); //配置OV2640的GPIO

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：ConfigGPIO
* 函数功能：配置OV2640的GPIO
* 输入参数：void 
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
static void ConfigGPIO(void)
{
  //配置RCC时钟
  rcu_periph_clock_enable(RCU_GPIOA);  //使能GPIOA的时钟
  rcu_periph_clock_enable(RCU_GPIOB);  //使能GPIOB的时钟

  //PWDN
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

  //RESET
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
}

/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
/*********************************************************************************************************
* 函数名称：InitOV2640
* 函数功能：初始化OV2640
* 输入参数：void
* 输出参数：void
* 返 回 值：0-成功，其他-错误代码
* 创建日期：2021年07月01日
* 注    意：配置完以后,默认输出是800*600尺寸的图片
*********************************************************************************************************/
u8 InitOV2640(void)
{
  u16 i, reg;

  //配置GPIO
  ConfigGPIO();

  //POWER ON
  OV2640PWDNSet(0);
  DelayNms(10);

  //复位OV2640
  gpio_bit_reset(GPIOB, GPIO_PIN_3);
  DelayNms(10);
  gpio_bit_set(GPIOB, GPIO_PIN_3);
  DelayNms(50);

  //初始化SCCB
  InitSCCB();

  //配置OV2640工作模式
  SCCBWriteReg(OV2640_DSP_RA_DLMT, 0x01); //操作sensor寄存器
  SCCBWriteReg(OV2640_SENSOR_COM7, 0x80); //软复位OV2640
  DelayNms(50);

  //校验ID
  reg = SCCBReadReg(OV2640_SENSOR_MIDH);
  reg <<= 8;
  reg |= SCCBReadReg(OV2640_SENSOR_MIDL);
  if(reg != OV2640_MID)
  {
    printf("InitOV2640: MID:%d\r\n", reg);
    return 1;
  }
  reg = SCCBReadReg(OV2640_SENSOR_PIDH);
  reg <<= 8;
  reg |= SCCBReadReg(OV2640_SENSOR_PIDL);
  if(reg != OV2640_PID)
  {
    printf("InitOV2640: HID:%d\r\n", reg);
    return 2;
  }

  //初始化OV2640，采用SVGA分辨率800*600)
  for(i = 0; i < sizeof(ov2640_svga_init_reg_tbl) / 2; i++)
  {
    SCCBWriteReg(ov2640_svga_init_reg_tbl[i][0], ov2640_svga_init_reg_tbl[i][1]);
  }
  return 0x00;
}

/*********************************************************************************************************
* 函数名称：OV2640PWDNSet
* 函数功能：设置摄像头模块PWDN脚的状态
* 输入参数：sta：0-PWDN=0，上电；1-PWDN=1，掉电
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640PWDNSet(u8 sta)
{
  gpio_bit_write(GPIOA, GPIO_PIN_15, (bit_status)sta);
}

/*********************************************************************************************************
* 函数名称：OV2640PWDNSet
* 函数功能：OV2640切换为JPEG模式
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640JPEGMode(void) 
{
  u16 i;

  //设置:YUV422格式
  for(i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
  }

  //设置:输出JPEG数据
  for(i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);  
  }
}

/*********************************************************************************************************
* 函数名称：OV2640RGB565Mode
* 函数功能：OV2640切换为RGB565模式
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640RGB565Mode(void)
{
  u16 i = 0;

  //设置:RGB565输出
  for(i = 0; i < (sizeof(ov2640_rgb565_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
  }
}

/*********************************************************************************************************
* 函数名称：OV2640AutoExposure
* 函数功能：OV2640自动曝光等级设置
* 输入参数：level：0-4
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640AutoExposure(u8 level)
{
  //自动曝光设置参数表,支持5个等级
  const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8]=
  {
    {
      0xFF, 0x01,
      0x24, 0x20,
      0x25, 0x18,
      0x26, 0x60,
    },
    {
      0xFF, 0x01,
      0x24, 0x34,
      0x25, 0x1c,
      0x26, 0x00,
    },
    {
      0xFF, 0x01,
      0x24, 0x3e,
      0x25, 0x38,
      0x26, 0x81,
    },
    {
      0xFF, 0x01,
      0x24, 0x48,
      0x25, 0x40,
      0x26, 0x81,
    },
    {
      0xFF, 0x01,
      0x24, 0x58,
      0x25, 0x50,
      0x26, 0x92,
    },
  };

  u8 i;
  u8* p= (u8*)OV2640_AUTOEXPOSURE_LEVEL[level];
  for(i = 0; i < 4; i++)
  { 
    SCCBWriteReg(p[i * 2], p[i * 2 + 1]);
  }
}

/*********************************************************************************************************
* 函数名称：OV2640LightMode
* 函数功能：白平衡设置
* 输入参数：mode：
*             0:自动
*             1:太阳sunny
*             2,阴天cloudy
*             3,办公室office
*             4,家里home
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640LightMode(u8 mode)
{
  u8 regccval;
  u8 regcdval;
  u8 regceval;
  switch(mode)
  { 
    case 0://auto
      SCCBWriteReg(0xFF, 0x00);
      SCCBWriteReg(0xC7, 0x10);
      return;
    case 1://sunny
      regccval = 0x5E;
      regcdval = 0x41;
      regceval = 0x54;
      break;
    case 2://cloudy
      regccval = 0x65;
      regcdval = 0x41;
      regceval = 0x4F;
      break;
    case 3://office
      regccval = 0x52;
      regcdval = 0x41;
      regceval = 0x66;
      break;
    case 4://home
      regccval = 0x42;
      regcdval = 0x3F;
      regceval = 0x71;
      break;
  }
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0xC7, 0x40);
  SCCBWriteReg(0xCC, regccval);
  SCCBWriteReg(0xCD, regcdval);
  SCCBWriteReg(0xCE, regceval);
}

/*********************************************************************************************************
* 函数名称：OV2640ColorSaturation
* 函数功能：色度设置
* 输入参数：sat：
*            0:-2
*            1:-1
*            2,0
*            3,+1
*            4,+2
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640ColorSaturation(u8 sat)
{ 
  u8 reg7dval = ((sat + 2) << 4) | 0x08;
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0x7C, 0x00);
  SCCBWriteReg(0x7D, 0x02);
  SCCBWriteReg(0x7C, 0x03);
  SCCBWriteReg(0x7D, reg7dval);
  SCCBWriteReg(0x7D, reg7dval);
}

/*********************************************************************************************************
* 函数名称：OV2640Brightness
* 函数功能：亮度设置
* 输入参数：bright：
*                0:(0x00)-2
*                1:(0x10)-1
*                2:(0x20) 0
*                3:(0x30)+1
*                4:(0x40)+2
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640Brightness(u8 bright)
{
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0x7C, 0x00);
  SCCBWriteReg(0x7D, 0x04);
  SCCBWriteReg(0x7C, 0x09);
  SCCBWriteReg(0x7D, bright << 4);
  SCCBWriteReg(0x7D, 0x00); 
}

/*********************************************************************************************************
* 函数名称：OV2640Contrast
* 函数功能：对比度设置
* 输入参数：contrast：
*                 0:-2
*                 1:-1
*                 2:0
*                 3:+1
*                 4:+2
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640Contrast(u8 contrast)
{
  //默认为普通模式
  u8 reg7d0val = 0x20;
  u8 reg7d1val = 0x20;
  switch(contrast)
  {
    case 0://-2
      reg7d0val = 0x18;
      reg7d1val = 0x34;
      break;
    case 1://-1
      reg7d0val = 0x1C;
      reg7d1val = 0x2A;
      break;
    case 3://1
      reg7d0val = 0x24;
      reg7d1val = 0x16;
      break;
    case 4://2
      reg7d0val = 0x28;
      reg7d1val = 0x0C;
      break;
  }
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0x7C, 0x00);
  SCCBWriteReg(0x7D, 0x04);
  SCCBWriteReg(0x7C, 0x07);
  SCCBWriteReg(0x7D, 0x20);
  SCCBWriteReg(0x7D, reg7d0val);
  SCCBWriteReg(0x7D, reg7d1val);
  SCCBWriteReg(0x7D, 0x06);
}

/*********************************************************************************************************
* 函数名称：OV2640SpecialEffects
* 函数功能：特效设置
* 输入参数：eft：
*            0:普通模式
*            1:负片
*            2:黑白
*            3:偏红色
*            4:偏绿色
*            5:偏蓝色
*            6:复古
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640SpecialEffects(u8 eft)
{
  //默认为普通模式
  u8 reg7d0val = 0x00;
  u8 reg7d1val = 0x80;
  u8 reg7d2val = 0x80;
  switch(eft)
  {
    case 1://负片
      reg7d0val = 0x40;
      break;
    case 2://黑白
      reg7d0val = 0x18;
      break;
    case 3://偏红色
      reg7d0val = 0x18;
      reg7d1val = 0x40;
      reg7d2val = 0xC0;
      break;
    case 4://偏绿色
      reg7d0val = 0x18;
      reg7d1val = 0x40;
      reg7d2val = 0x40;
      break;
    case 5://偏蓝色
      reg7d0val = 0x18;
      reg7d1val = 0xA0;
      reg7d2val = 0x40;
      break;
    case 6://复古
      reg7d0val = 0x18;
      reg7d1val = 0x40;
      reg7d2val = 0xA6;
      break;
  }
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0x7C, 0x00);
  SCCBWriteReg(0x7D, reg7d0val);
  SCCBWriteReg(0x7C, 0x05);
  SCCBWriteReg(0x7D, reg7d1val);
  SCCBWriteReg(0x7D, reg7d2val); 
}

/*********************************************************************************************************
* 函数名称：OV2640ColorBar
* 函数功能：彩条测试
* 输入参数：sw：0-关闭彩条，1-开启彩条(注意OV2640的彩条是叠加在图像上面的)
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640ColorBar(u8 sw)
{
  u8 reg;
  SCCBWriteReg(0xFF, 0x01);
  reg = SCCBReadReg(0x12);
  reg &= ~(1 << 1);
  if(sw)
  {
    reg |= 1 << 1;
  }
  SCCBWriteReg(0x12, reg);
}

/*********************************************************************************************************
* 函数名称：OV2640WindowSet
* 函数功能：设置图像输出窗口
* 输入参数：sx,sy-起始地址，width,height:宽度(对应:horizontal)和高度(对应:vertical)
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void OV2640WindowSet(u16 sx,u16 sy,u16 width,u16 height)
{
  u16 endx;
  u16 endy;
  u8 temp; 
  endx = sx + width / 2;  //V*2
  endy = sy + height / 2;

  SCCBWriteReg(0xFF, 0x01);
  temp = SCCBReadReg(0x03); //读取Vref之前的值
  temp &= 0xF0;
  temp |= ((endy & 0x03) << 2) | (sy & 0x03);
  SCCBWriteReg(0x03, temp);       //设置Vref的start和end的最低2位
  SCCBWriteReg(0x19, sy >> 2);    //设置Vref的start高8位
  SCCBWriteReg(0x1A, endy >> 2);  //设置Vref的end的高8位
  temp = SCCBReadReg(0x32);       //读取Href之前的值
  temp &= 0xC0;
  temp |= ((endx & 0x07) << 3) | (sx & 0x07);
  SCCBWriteReg(0x32, temp);       //设置Href的start和end的最低3位
  SCCBWriteReg(0x17, sx >> 3);    //设置Href的start高8位
  SCCBWriteReg(0x18, endx >> 3);  //设置Href的end的高8位
}

/*********************************************************************************************************
* 函数名称：OV2640OutSizeSet
* 函数功能：设置图像输出大小
* 输入参数：width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
* 输出参数：void
* 返 回 值：0-设置成功，其他-设置失败
* 创建日期：2021年07月01日
* 注    意：OV2640输出图像的大小(分辨率),完全由改函数确定
*********************************************************************************************************/
u8 OV2640OutSizeSet(u16 width,u16 height)
{
  u16 outh;
  u16 outw;
  u8 temp; 
  if(width % 4)
  {
    return 1;
  }
  if(height % 4)
  {
    return 2;
  }
  outw = width / 4;
  outh = height / 4;
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0xE0, 0x04);
  SCCBWriteReg(0x5A, outw & 0xFF);  //设置OUTW的低八位
  SCCBWriteReg(0x5B, outh & 0xFF);  //设置OUTH的低八位
  temp = (outw >> 8) & 0x03;
  temp |= (outh >> 6) & 0x04;
  SCCBWriteReg(0x5C,temp);          //设置OUTH/OUTW的高位
  SCCBWriteReg(0xE0,0x00);
  return 0;
}

/*********************************************************************************************************
* 函数名称：OV2640ImageWinSet
* 函数功能：设置图像开窗大小
* 输入参数：width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
* 输出参数：void
* 返 回 值：0-设置成功，其他-设置失败
* 创建日期：2021年07月01日
* 注    意：OV2640ImageSizeSet确定传感器输出分辨率从大小，
*          该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
*          本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
*          OV2640OutSizeSet设置的宽度和高度,根据本函数设置的宽度和高度,由DSP自动计算缩放比例,输出给外部设备
*********************************************************************************************************/
u8 OV2640ImageWinSet(u16 offx,u16 offy,u16 width,u16 height)
{
  u16 hsize;
  u16 vsize;
  u8 temp; 
  if(width % 4)
  {
    return 1;
  }
  if(height % 4)
  {
    return 2;
  }
  hsize = width / 4;
  vsize = height / 4;
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0xE0, 0x04);
  SCCBWriteReg(0x51, hsize & 0xFF);  //设置H_SIZE的低八位
  SCCBWriteReg(0x52, vsize & 0xFF);  //设置V_SIZE的低八位
  SCCBWriteReg(0x53, offx & 0xFF);   //设置offx的低八位
  SCCBWriteReg(0x54, offy & 0xFF);   //设置offy的低八位
  temp = (vsize >> 1) & 0x80;
  temp |= (offy >> 4) & 0x70;
  temp |= (hsize >> 5) & 0x08;
  temp |= (offx >> 8) & 0x07;
  SCCBWriteReg(0x55, temp);                 //设置H_SIZE/V_SIZE/OFFX,OFFY的高位
  SCCBWriteReg(0x57, (hsize >> 2) & 0x80);  //设置H_SIZE/V_SIZE/OFFX,OFFY的高位
  SCCBWriteReg(0xE0, 0x00);
  return 0;
}

/*********************************************************************************************************
* 函数名称：OV2640ImageSizeSet
* 函数功能：该函数设置图像尺寸大小，也就是所选格式的输出分辨率
* 输入参数：width,height:图像宽度和图像高度
* 输出参数：void
* 返 回 值：0-设置成功，其他-设置失败
* 创建日期：2021年07月01日
* 注    意：UXGA:1600*1200,SVGA:800*600,CIF:352*288
*********************************************************************************************************/
u8 OV2640ImageSizeSet(u16 width,u16 height)
{ 
  u8 temp; 
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0xE0, 0x04);
  SCCBWriteReg(0xC0, (width >> 3) & 0xFF);  //设置HSIZE的10:3位
  SCCBWriteReg(0xC1, (height >> 3) & 0xFF); //设置VSIZE的10:3位
  temp = (width & 0x07) << 3;
  temp |= height & 0x07;
  temp |= (width >> 4) & 0x80; 
  SCCBWriteReg(0x8C, temp);
  SCCBWriteReg(0xE0, 0x00);
  return 0;
}
