/*********************************************************************************************************
* 模块名称：TLILCD.c
* 摘    要：LCD驱动模块
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
* 修改文件：默认使用RGB565格式，扫描时默认横屏扫描
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "TLILCD.h"
#include "gd32f470x_conf.h"
#include "SDRAM.h"
#include "Font.h"
#include "FontLib.h"

/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/
////背景显存缓冲区首地址
//#define BACK_FRAME_START_ADDR SDRAM_DEVICE1_ADDR

////前景显存首地址
//#define FORE_FRAME_START_ADDR ((u32)(BACK_FRAME_START_ADDR + (LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT * 2))

/*********************************************************************************************************
*                                              枚举结构体
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/

//背景显存缓冲区首地址
#define BACK_FRAME_START_ADDR (0xD1000000)

//前景显存首地址
#define FORE_FRAME_START_ADDR ((u32)(BACK_FRAME_START_ADDR + (LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT * 2))
//背景显存
static u16 s_arrBackgroundFrame[(LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT] __attribute__((section(".ARM.__at_0xD1000000")));

//前景显存
static u16 s_arrForegroundFrame[(LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT] __attribute__((section(".ARM.__at_0xD1200000")));

//LCD控制结构体
StructTLILCDDev g_structTLILCDDev;

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
* 函数名称：InitLCD
* 函数功能：初始化LCD模块
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void InitLCD(void)
{
  u32 i;

  //TLI初始化结构体
  tli_parameter_struct tli_init_struct;

  //初始化LCD设备结构体（默认横屏，当前层为背景层）
  g_structTLILCDDev.pWidth = LCD_PIXEL_WIDTH;
  g_structTLILCDDev.pHeight = LCD_PIXEL_HEIGHT;
  g_structTLILCDDev.dir = LCD_SCREEN_HORIZONTAL;
  g_structTLILCDDev.currentLayer = LCD_LAYER_BACKGROUND;
  g_structTLILCDDev.frameBuf = s_arrBackgroundFrame;
  g_structTLILCDDev.backFrameAddr = BACK_FRAME_START_ADDR;
  g_structTLILCDDev.foreFrameAddr = FORE_FRAME_START_ADDR;
  g_structTLILCDDev.pixelSize = 2;
  for(i = 0; i < LCD_LAYER_MAX; i++)
  {
    g_structTLILCDDev.width[i] = g_structTLILCDDev.pWidth;
    g_structTLILCDDev.height[i] = g_structTLILCDDev.pHeight;
  }

  //使能RCU时钟
  rcu_periph_clock_enable(RCU_TLI);
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_GPIOD);
  rcu_periph_clock_enable(RCU_GPIOE);
  rcu_periph_clock_enable(RCU_GPIOF);
  rcu_periph_clock_enable(RCU_GPIOG);
  rcu_periph_clock_enable(RCU_GPIOH);
  rcu_periph_clock_enable(RCU_GPIOI);

  //R0
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_2);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);

  //R1
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_3);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);

  //R2
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_8);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //R3
  gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_0);
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //R4
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_10);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //R5
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_11);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

  //R6
  gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_1);
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //R7
  gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);

  //G0
  gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_5);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

  //G1
  gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_6);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);

  //G2
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_13);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

  //G3
  gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_10);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //G4
  gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_15);
  gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
  gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);

  //G5
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_0);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);

  //G6
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_1);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

  //G7
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_2);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);

  //B0
  gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_4);
  gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //B1
  gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_12);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

  //B2
  gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_6);
  gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);

  //B3
  gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_11);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

  //B4
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_4);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

  //B5
  gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_3);
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);

  //B6
  gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_8);
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);

  //B7
  gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_9);
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);

  //LCD_DE
  gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);
  gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //LCD_VSYNC
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_9);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);

  //LCD_HSYNC
  gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_10);
  gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
  gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);

  //LCD_CLK
  gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_7);
  gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

  //LCD背光
  gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
  gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
  gpio_bit_set(GPIOC,GPIO_PIN_13);

  //复位TLI
  tli_deinit();
  tli_struct_para_init(&tli_init_struct);

  //配置TLI时钟（时钟频率为1MHz * 200 / 5 / 4 = 10MHz，数据手册给的典型值是30MHz，但是10MHz刷新率下更稳定）
  if(ERROR == rcu_pllsai_config(240, 4, 4)){while(1);}
  rcu_tli_clock_div_config(RCU_PLLSAIR_DIV2);
  // if(ERROR == rcu_pllsai_config(200, 4, 5)){while(1);}
  // rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);
  // if(ERROR == rcu_pllsai_config(50, 4, 5)){while(1);}
  // rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);
  rcu_osci_on(RCU_PLLSAI_CK);
  if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)){while(1);}

  //TLI初始化
  //信号极性配置
  tli_init_struct.signalpolarity_hs      = TLI_HSYN_ACTLIVE_LOW; //水平同步脉冲低电平有效
  tli_init_struct.signalpolarity_vs      = TLI_VSYN_ACTLIVE_LOW; //垂直同步脉冲低电平有效
  tli_init_struct.signalpolarity_de      = TLI_DE_ACTLIVE_LOW;   //数据使能低电平有效
  tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;  //像素时钟是TLI时钟

  //显示时序配置
  tli_init_struct.synpsz_hpsz   = HORIZONTAL_SYNCHRONOUS_PULSE - 1; //水平同步脉冲宽度
  tli_init_struct.synpsz_vpsz   = VERTICAL_SYNCHRONOUS_PULSE - 1;   //垂直同步脉冲宽度
  tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1; //水平后沿加同步脉冲的宽度
  tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;     //垂直后沿加同步脉冲的宽度
  tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1; //水平有效宽度加后沿像素和水平同步像素宽度
  tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;    //垂直有效宽度加后沿像素和垂直同步像素宽度
  tli_init_struct.totalsz_htsz  = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1; //显示器的水平总宽度
  tli_init_struct.totalsz_vtsz  = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;      //显示器的垂直总宽度
  
  //配置BG层颜色（黑色）
  tli_init_struct.backcolor_red = 0x00;
  tli_init_struct.backcolor_green = 0x00;
  tli_init_struct.backcolor_blue = 0x00;

  //根据参数配置TLI
  tli_init(&tli_init_struct);

  //初始化背景层和前景层
  LCDLayerWindowSet(LCD_LAYER_BACKGROUND, 0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);
  LCDLayerWindowSet(LCD_LAYER_FOREGROUND, 0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);

  //使能背景层
  LCDLayerEnable(LCD_LAYER_BACKGROUND);

  //禁用前景层
  LCDLayerDisable(LCD_LAYER_FOREGROUND);

  //设置背景层透明度
  LCDTransparencySet(LCD_LAYER_BACKGROUND, 0xFF);

  //设置前景层透明度
  LCDTransparencySet(LCD_LAYER_FOREGROUND, 0xFF);

  //开启抖动
  tli_dither_config(TLI_DITHER_ENABLE);

  // //关闭抖动
  // tli_dither_config(TLI_DITHER_DISABLE);

  //立即更新TLI寄存器
  tli_reload_config(TLI_REQUEST_RELOAD_EN);

  //TLI外设使能
  tli_enable();

  //切换到背景层
  LCDLayerSwitch(LCD_LAYER_BACKGROUND);

  //横屏显示
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //清屏
  LCDClear(LCD_COLOR_BLACK);
}

/*********************************************************************************************************
* 函数名称：LCDLayerWindowSet
* 函数功能：设置层窗口
* 输入参数：layer：前景或背景层定义（LCD_LAYER_FOREGROUND、LCD_LAYER_BACKGROUND）
*          x,y：窗口位置
*          width：窗口宽度
*          height：窗口高度
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*          1、当该层被禁止或TLI_LxHPOS和TLI_LxVPOS定义的窗口之外，默认颜色值生效
*          2、LCD屏幕方向默认是横屏
*********************************************************************************************************/
void LCDLayerWindowSet(EnumTLILCDLayer layer, u32 x, u32 y, u32 width, u32 height)
{
  //TLI初始化结构体
  tli_layer_parameter_struct  tli_layer_init_struct; 
  
  //临时变量
  u32 swap, x0, y0;
  
  //配置默认值
  tli_layer_struct_para_init(&tli_layer_init_struct);

  //竖屏特殊处理
  if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //坐标系变换
    x0 = x;
    y0 = y + height - 1;
    y = x0;
    x = LCD_PIXEL_WIDTH - 1 - y0;
    if(x >= LCD_PIXEL_WIDTH){return;}
    if(y >= LCD_PIXEL_HEIGHT){return;}

    //交换宽度和高度
    swap = width;
    width = height;
    height = swap;
  }

  //矫正宽度
  if((x + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - x;
  }

  //矫正高度
  if((y + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - y;
  }

  //背景配置
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_init_struct.layer_window_leftpos    = (x + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);             //窗口左侧位置
    tli_layer_init_struct.layer_window_rightpos   = (x + width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1); //窗口右侧位置
    tli_layer_init_struct.layer_window_toppos     = (y + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);                 //窗口顶部位置
    tli_layer_init_struct.layer_window_bottompos  = (y + height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);    //窗口底部位置
    tli_layer_init_struct.layer_ppf               = LAYER_PPF_RGB565;          //像素格式
    tli_layer_init_struct.layer_sa                = 0xFF;                      //恒定Aplha值
    tli_layer_init_struct.layer_default_blue      = 0xFF;                      //默认颜色B值
    tli_layer_init_struct.layer_default_green     = 0xFF;                      //默认颜色G值
    tli_layer_init_struct.layer_default_red       = 0xFF;                      //默认颜色R值
    tli_layer_init_struct.layer_default_alpha     = 0xFF;                      //默认颜色Aplha值，默认显示
    tli_layer_init_struct.layer_acf1              = LAYER_ACF1_PASA;           //归一化的像素Alpha乘以归一化的恒定Alpha
    tli_layer_init_struct.layer_acf2              = LAYER_ACF2_PASA;           //归一化的像素Alpha乘以归一化的恒定Alpha
    tli_layer_init_struct.layer_frame_bufaddr     = (u32)s_arrBackgroundFrame; //缓冲区首地址
    tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);         //行长度
    tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);         //步幅偏移，某行起始处到下一行起始处之间的字节数
    tli_layer_init_struct.layer_frame_total_line_number = height;              //帧行数
    tli_layer_init(LAYER0, &tli_layer_init_struct); //根据参数初始化背景层
    tli_color_key_disable(LAYER0);                  //禁用色键
    tli_lut_disable(LAYER0);                        //禁用颜色查找表

    //保存窗口宽度和高度
    if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
    {
      g_structTLILCDDev.width[LCD_LAYER_BACKGROUND] = width;
      g_structTLILCDDev.height[LCD_LAYER_BACKGROUND] = height;
    }
    else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
    {
      g_structTLILCDDev.width[LCD_LAYER_BACKGROUND] = height;
      g_structTLILCDDev.height[LCD_LAYER_BACKGROUND] = width;
    }
  }
  
  //前景配置
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_init_struct.layer_window_leftpos    = (x + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);             //窗口左侧位置
    tli_layer_init_struct.layer_window_rightpos   = (x + width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1); //窗口右侧位置
    tli_layer_init_struct.layer_window_toppos     = (y + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);                 //窗口顶部位置
    tli_layer_init_struct.layer_window_bottompos  = (y + height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);    //窗口底部位置
    tli_layer_init_struct.layer_ppf               = LAYER_PPF_RGB565;          //像素格式
    tli_layer_init_struct.layer_sa                = 0xFF;                      //恒定Aplha值
    tli_layer_init_struct.layer_default_blue      = 0xFF;                      //默认颜色B值
    tli_layer_init_struct.layer_default_green     = 0xFF;                      //默认颜色G值
    tli_layer_init_struct.layer_default_red       = 0xFF;                      //默认颜色R值
    tli_layer_init_struct.layer_default_alpha     = 0x00;                      //默认颜色Aplha值，默认不显示
    tli_layer_init_struct.layer_acf1              = LAYER_ACF1_PASA;           //归一化的像素Alpha乘以归一化的恒定Alpha
    tli_layer_init_struct.layer_acf2              = LAYER_ACF2_PASA;           //归一化的像素Alpha乘以归一化的恒定Alpha
    tli_layer_init_struct.layer_frame_bufaddr     = (u32)s_arrForegroundFrame; //缓冲区首地址
    tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);         //行长度
    tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);         //步幅偏移，某行起始处到下一行起始处之间的字节数
    tli_layer_init_struct.layer_frame_total_line_number = height;              //帧行数
    tli_layer_init(LAYER1, &tli_layer_init_struct); //根据参数初始化前景层
    tli_color_key_disable(LAYER1);                  //禁用色键
    tli_lut_disable(LAYER1);                        //禁用颜色查找表

    //保存窗口宽度和高度
    if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
    {
      g_structTLILCDDev.width[LCD_LAYER_FOREGROUND] = width;
      g_structTLILCDDev.height[LCD_LAYER_FOREGROUND] = height;
    }
    else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
    {
      g_structTLILCDDev.width[LCD_LAYER_FOREGROUND] = height;
      g_structTLILCDDev.height[LCD_LAYER_FOREGROUND] = width;
    }
  }

  //立即更新TLI寄存器
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* 函数名称：LCDLayerEnable
* 函数功能：使能背景或前景层
* 输入参数：layer：前景或背景层定义（LCD_LAYER_FOREGROUND、LCD_LAYER_BACKGROUND）
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDLayerEnable(EnumTLILCDLayer layer)
{
  //使能背景层
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_enable(LAYER0);
  }
  
  //使能前景层
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_enable(LAYER1);
  }

  //立即更新TLI寄存器
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* 函数名称：LCDLayerDisable
* 函数功能：禁用背景或前景层
* 输入参数：layer：前景或背景层定义（LCD_LAYER_FOREGROUND、LCD_LAYER_BACKGROUND）
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDLayerDisable(EnumTLILCDLayer layer)
{
  //使能背景层
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_disable(LAYER0);
  }
  
  //使能前景层
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_disable(LAYER1);
  }

  //立即更新TLI寄存器
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* 函数名称：LCDLayerSwitch
* 函数功能：切换层
* 输入参数：layer：前景或背景层定义（LCD_LAYER_FOREGROUND、LCD_LAYER_BACKGROUND）
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDLayerSwitch(EnumTLILCDLayer layer)
{
  //切换到背景层
  if(LCD_LAYER_BACKGROUND == layer)
  {
    g_structTLILCDDev.frameBuf = s_arrBackgroundFrame;
    g_structTLILCDDev.currentLayer = LCD_LAYER_BACKGROUND;
  }
  
  //切换到前景层
  else
  {
    g_structTLILCDDev.frameBuf = s_arrForegroundFrame;
    g_structTLILCDDev.currentLayer = LCD_LAYER_FOREGROUND;
  }
}

/*********************************************************************************************************
* 函数名称：LCDTransparencySet
* 函数功能：设置当前层的透明度
* 输入参数：trans：透明度配置
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDTransparencySet(EnumTLILCDLayer layer, u8 trans)
{
  //设置背景透明度
  if (LCD_LAYER_BACKGROUND == layer)
  {
    TLI_LxSA(LAYER0) &= ~(TLI_LxSA_SA);
    TLI_LxSA(LAYER0) = trans;
  }

  //设置前景透明度
  else
  {
    TLI_LxSA(LAYER1) &= ~(TLI_LxSA_SA);
    TLI_LxSA(LAYER1) = trans;
  }

  //立即更新TLI寄存器
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* 函数名称：LCDDisplayOn
* 函数功能：开启LCD显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDDisplayOn(void)
{
  tli_enable();
}

/*********************************************************************************************************
* 函数名称：LCDDisplayOff
* 函数功能：关闭LCD显示
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：因为RGB屏不自带显存，所以关闭TLI后图案显示会逐渐消失，直至花屏
*********************************************************************************************************/
void LCDDisplayOff(void)
{
  tli_disable();
}

/*********************************************************************************************************
* 函数名称：LCDWaitHIdle
* 函数功能：等待LCD行传输空闲
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDWaitHIdle(void)
{
  while(RESET == tli_flag_get(TLI_FLAG_HDE));
  while(SET == tli_flag_get(TLI_FLAG_HDE));
}

/*********************************************************************************************************
* 函数名称：LCDWaitVIdle
* 函数功能：等待LCD场传输空闲
* 输入参数：void
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDWaitVIdle(void)
{
  while(RESET == tli_flag_get(TLI_FLAG_VDE));
  while(SET == tli_flag_get(TLI_FLAG_VDE));
}

/*********************************************************************************************************
* 函数名称: LCDDisplayDir
* 函数功能: 设置LCD显示方向
* 输入参数: dir：显示方向（LCD_SCREEN_HORIZONTAL，LCD_SCREEN_VERTICAL）
* 输出参数: void
* 返 回 值: void
* 创建日期: 2018年01月01日
* 注    意: 
*********************************************************************************************************/
void LCDDisplayDir(EnumTLILCDDir dir)
{
  u32 swap;

  //交换宽度和高度
  if(g_structTLILCDDev.dir != dir)
  {
    //保存显示方向
    g_structTLILCDDev.dir = dir;

    //交换背景层宽度和高度
    swap = g_structTLILCDDev.width[LCD_LAYER_BACKGROUND];
    g_structTLILCDDev.width[LCD_LAYER_BACKGROUND] = g_structTLILCDDev.height[LCD_LAYER_BACKGROUND];
    g_structTLILCDDev.height[LCD_LAYER_BACKGROUND] = swap;

    //交换前景层宽度和高度
    swap = g_structTLILCDDev.width[LCD_LAYER_FOREGROUND];
    g_structTLILCDDev.width[LCD_LAYER_FOREGROUND] = g_structTLILCDDev.height[LCD_LAYER_FOREGROUND];
    g_structTLILCDDev.height[LCD_LAYER_FOREGROUND] = swap;
  }
}

/*********************************************************************************************************
* 函数名称：LCDClear
* 函数功能：清屏
* 输入参数：color：填充颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDClear(u32 color)
{
  //目标图像参数结构体
  ipa_destination_parameter_struct  ipa_destination_init_struct;

  //RGB
  u8 red, green, blue;

  //RGB565转RGB888
  RGB565ToRGB888A(color, &red, &green, &blue);

  //使能IPA时钟
  rcu_periph_clock_enable(RCU_IPA);

  //复位IPA
  ipa_deinit();

  //设置转换模式
  ipa_pixel_format_convert_mode_set(IPA_FILL_UP_DE); //颜色填充

  //设置目标图像参数
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;                  //目标图像格式
  ipa_destination_init_struct.destination_memaddr  = (u32)g_structTLILCDDev.frameBuf; //目标图像存储地址
  ipa_destination_init_struct.destination_lineoff  = 0;                //行末与下一行开始之间的像素点数
  ipa_destination_init_struct.destination_prealpha = 0xFF;             //目标层预定义透明度
  ipa_destination_init_struct.destination_prered   = red;              //目标层预定义红色值
  ipa_destination_init_struct.destination_pregreen = green;            //目标层预定义绿色值
  ipa_destination_init_struct.destination_preblue  = blue;             //目标层预定义蓝色值
  ipa_destination_init_struct.image_width          = LCD_PIXEL_WIDTH;  //目标图像宽度
  ipa_destination_init_struct.image_height         = LCD_PIXEL_HEIGHT; //目标图像高度
  ipa_destination_init(&ipa_destination_init_struct); //根据参数配置目标图像

  // //使能IPA内部定时器
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //开启传输
  ipa_transfer_enable();

  //等待传输结束
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //清除标志位
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //关闭IPA
  ipa_deinit();
}

/*********************************************************************************************************
* 函数名称：LCDDrawPoint
* 函数功能：画点
* 输入参数：x, y：坐标，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDDrawPoint(u32 x, u32 y, u32 color)
{
  u32 width, height;

  //获取当前窗口的宽度和高度
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  height = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

  //画点范围超出屏幕坐标
  if(x >= width || y >= height)
  {
    return;
  }

  //横屏
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir) 
  {
    g_structTLILCDDev.frameBuf[width * y + x] = color;
  }

  //竖屏
  else
  {
    g_structTLILCDDev.frameBuf[height * x + (height - 1 - y)] = color;
  }
}

/*********************************************************************************************************
* 函数名称：LCDReadPoint
* 函数功能：读点
* 输入参数：x, y：坐标
* 输出参数：void
* 返 回 值：颜色值
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
u32 LCDReadPoint(u32 x, u32 y)
{
  u32 width, height;

  //获取当前窗口的宽度和高度
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  height = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

  //读点范围超出屏幕坐标，默认返回黑色
  if(x >= width || y >= height)
  {
    return 0;
  }

  //横屏
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir) 
  {
    return g_structTLILCDDev.frameBuf[width * y + x];
  }

  //竖屏
  else
  {
    return g_structTLILCDDev.frameBuf[height * x + (height - 1 - y)];
  }
}

/*********************************************************************************************************
* 函数名称：LCDFill
* 函数功能：LCD填充
* 输入参数：x, y：起始坐标，width：宽度，height：高度，color：填充颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：填充时最后一排会莫名多绘制两点，原因未知
*********************************************************************************************************/
void LCDFill(u32 x, u32 y, u32 width, u32 height, u32 color)
{
  ipa_destination_parameter_struct  ipa_destination_init_struct; //目标图像参数结构体
  u32 destStartAddr;       //目标图像起始地址
  u32 phyWidth, phyHeight; //横屏宽度和高度
  u32 phyX, phyY;          //横屏时填充起始地址
  u32 lineOff;             //目标行偏移，最后一个像素和下一行第一个像素之间的像素数目
  u32 swap;                //交换数据时的临时变量
  u8  red, green, blue;    //RGB
  u32 x1, y1, x2, y2, color1, color2;

  //横屏，记录宽度、高低以及起始地址
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //竖屏，因为是按照横屏方式配置，所以要将宽度、高度转换过来，并做起始坐标转换
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //获取屏幕宽度和高度
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //交换屏幕宽度和高度
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //交换填充区域宽度和高度
    swap = width;
    width = height;
    height = swap;

    //坐标系转换
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //填充区域修正，防止填充区域超出屏幕范围
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //保存坐标点信息
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    x1 = phyX;
    x2 = x1 + 1;
    y1 = phyY + height;
    y2 = y1;
  }
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    x1 = x + height;
    x2 = x1;
    y1 = y + width - 1;
    y2 = y1 - 1;
  }
  color1 = LCDReadPoint(x1, y1);
  color2 = LCDReadPoint(x2, y2);

  //计算目标图像起始地址
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //计算目标行偏移
  lineOff = phyWidth - width;

  //RGB565转RGB888
  RGB565ToRGB888A(color, &red, &green, &blue);

  //使能IPA时钟
  rcu_periph_clock_enable(RCU_IPA);

  //复位IPA
  ipa_deinit();

  //设置转换模式
  ipa_pixel_format_convert_mode_set(IPA_FILL_UP_DE); //颜色填充

  //设置目标图像参数
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565; //目标图像格式
  ipa_destination_init_struct.destination_memaddr  = destStartAddr;  //目标图像存储地址
  ipa_destination_init_struct.destination_lineoff  = lineOff;        //行末与下一行开始之间的像素点数
  ipa_destination_init_struct.destination_prealpha = 0xFF;           //目标层预定义透明度
  ipa_destination_init_struct.destination_prered   = red;            //目标层预定义红色值
  ipa_destination_init_struct.destination_pregreen = green;          //目标层预定义绿色值
  ipa_destination_init_struct.destination_preblue  = blue;           //目标层预定义蓝色值
  ipa_destination_init_struct.image_width          = width;          //目标图像宽度
  ipa_destination_init_struct.image_height         = height;         //目标图像高度
  ipa_destination_init(&ipa_destination_init_struct);                //根据参数配置目标图像

  // //使能IPA内部定时器
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //开启传输
  ipa_transfer_enable();

  //等待传输结束
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //清除标志位
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //关闭IPA
  ipa_deinit();

  //填回像素点
  LCDDrawPoint(x1, y1, color1);
  LCDDrawPoint(x2, y2, color2);
}

/*********************************************************************************************************
* 函数名称：LCDFillPixel
* 函数功能：LCD填充
* 输入参数：x0, y0：起始坐标，x1, y1：终点坐标，color：填充颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  u32 swap, width, height;

  //校验横坐标
  if(x0 > x1)
  {
    swap = x0;
    x0 = x1;
    x1 = swap;
  }

  //校验纵坐标
  if(y0 > y1)
  {
    swap = y0;
    y0 = y1;
    y1 = swap;
  }

  //计算宽度
  width = x1 - x0 + 1;

  //计算高度
  height = y1 - y0 + 1;

  //填充
  LCDFill(x0, y0, width, height, color);
}

/*********************************************************************************************************
* 函数名称：LCDColorFill
* 函数功能：LCD填充
* 输入参数：x, y：起始坐标，width：宽度，height：高度，color：填充颜色缓冲区首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDColorFill(u32 x, u32 y, u32 width, u32 height, u16* color)
{
  u32 x0, y0, i;

  //保存起始坐标
  x0 = x;
  y0 = y;

  //填充
  i = 0;
  for(y = y0; y < y0 + height; y++)
  {
    for(x = x0; x < x0 + width; x++)
    {
      LCDDrawPoint(x, y, color[i]);
      i++;
    }
  }
}

/*********************************************************************************************************
* 函数名称：LCDFillPixel
* 函数功能：LCD填充
* 输入参数：x0, y0：起始坐标，x1, y1：终点坐标，color：填充颜色缓冲区首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDColorFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u16* color)
{
  u32 x, y, swap, i;

  //校验横坐标
  if(x0 > x1)
  {
    swap = x0;
    x0 = x1;
    x1 = swap;
  }

  //校验纵坐标
  if(y0 > y1)
  {
    swap = y0;
    y0 = y1;
    y1 = swap;
  }

  //填充
  i = 0;
  for(y = y0; y <= y1; y++)
  {
    for(x = x0; x <= x1; x++)
    {
      LCDDrawPoint(x, y, color[i]);
      i++;
    }
  }
}

/*********************************************************************************************************
* 函数名称：LCDDrawLine
* 函数功能：画线
* 输入参数：x0，y0：起点坐标，x1，y1：终点坐标，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2018年01月01日
* 注    意：
*********************************************************************************************************/
void LCDDrawLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  int x, y, dx, dy, dx2, dy2, xStep, yStep, swap, sum;

  //计算x、y方向增量
  dx = x1 - x0;
  dy = y1 - y0;
  if(dx < 0){dx = -dx;}
  if(dy < 0){dy = -dy;}
  dx2 = dx << 1;
  dy2 = dy << 1;

  //斜率小于等于1，以横坐标增长方向计算
  if(dx >= dy)
  {
    //修正绘制方向（按增长方向绘制）
    if(x0 > x1)
    {
      swap = x0; x0 = x1; x1 = swap;
      swap = y0; y0 = y1; y1 = swap;
    }

    //判断y增长方向
    if(y1 > y0){yStep = 1;}else{yStep = -1;}

    x = x0;
    y = y0;
    sum = -dx;
    while(x <= x1)
    {
      //画点
      LCDDrawPoint(x, y, color);

      //横坐标自增
      x = x + 1;

      //纵坐标判别式
      sum = sum + dy2;

      //纵坐标自增
      if(sum >= 0)
      {
        sum = sum - dx2;
        y = y + yStep;
      }
    }
  }

  //斜率大于1，以纵坐标增长方向计算
  else
  {
    //修正绘制方向（按增长方向绘制）
    if(y0 > y1)
    {
      swap = x0; x0 = x1; x1 = swap;
      swap = y0; y0 = y1; y1 = swap;
    }

    //判断x增长方向
    if(x1 > x0){xStep = 1;}else{xStep = -1;}

    x = x0;
    y = y0;
    sum = -dy;
    while(y <= y1)
    {
      //画点
      LCDDrawPoint(x, y, color);

      //纵坐标自增
      y = y + 1;

      //横坐标判别式
      sum = sum + dx2;

      //横坐标自增
      if(sum >= 0)
      {
        sum = sum - dy2;
        x = x + xStep;
      }
    }
  }
}

/*********************************************************************************************************
* 函数名称：LCDDrawRectangle
* 函数功能：画矩形
* 输入参数：(x1,y1),(x2,y2):矩形的对角坐标，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void LCDDrawRectangle(u32 x1, u32 y1, u32 x2, u32 y2, u32 color)
{
  LCDDrawLine(x1, y1, x2, y1, color);
  LCDDrawLine(x1, y1, x1, y2, color);
  LCDDrawLine(x1, y2, x2, y2, color);
  LCDDrawLine(x2, y1, x2, y2, color);
}

/*********************************************************************************************************
* 函数名称：LCDDrawCircle
* 函数功能：画圆
* 输入参数：x0，y0：圆心坐标，r：半径，color：颜色
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void LCDDrawCircle(u32 x0, u32 y0, u32 r, u32 color)
{
  int x, y, d;

  x = 0;
  y = r;
  d = 5 - (r << 2);
  while(x <= y)
  {
    //根据圆的八分对称性画点
    LCDDrawPoint(x0 + x, y0 + y, color);
    LCDDrawPoint(x0 + x, y0 - y, color);
    LCDDrawPoint(x0 - x, y0 + y, color);
    LCDDrawPoint(x0 - x, y0 - y, color);
    LCDDrawPoint(x0 + y, y0 + x, color);
    LCDDrawPoint(x0 - y, y0 + x, color);
    LCDDrawPoint(x0 + y, y0 - x, color);
    LCDDrawPoint(x0 - y, y0 - x, color);

    //横坐标自增
    x++;

    //取右侧点
    if(d < 0)
    {
      d = d + (x << 3) + 12;
    }

    //取对角点
    else
    {
      d = d + ((x - y) << 3) + 20;
      y--;
    }
  }
}

/*********************************************************************************************************
* 函数名称：LCDShowChar
* 函数功能：在指定位置显示一个字符
* 输入参数：x,y：显示坐标
*          font：字体，LCD_FONT_12、LCD_FONT_16或LCD_FONT_24
*          mode：显示模式，透明或非透明，LCD_TEXT_NORMAL或LCD_TEXT_TRANS
*          textColor：字符颜色
*          backColor：背景颜色
*          code：字符编码，英文对应ASCII码，汉字对应GBK汉字内码
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：1. 扫描顺序是高位在前，从上到下，从左往右，一列最后不满一个字节的则补零
						2. 显示中文字符时只能使用LCD_FONT_24
*********************************************************************************************************/
void LCDShowChar(u32 x, u32 y, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, u32 code)
{
  u8  byte;      //当前点阵数据
  u32 i, j;      //循环变量
  u32 y0;        //纵坐标起点
  u32 height;    //字符高度
  u32 bufSize;   //点阵数据字节数
  u8* enBuf;     //ASCII码点阵数据首地址
  static u8  cnBuf[72]; //汉字点阵数据缓冲区
  u8* codeBuf;   //字符点阵数据首地址，二选一

  //获取字符高度
  switch (font)
  {
  case LCD_FONT_12: height = 12; break;
  case LCD_FONT_16: height = 16; break;
  case LCD_FONT_24: height = 24; break;
  default: return;
  }

  //计算点阵数据字节数
  if(code < 0x80) //ASCII码
  {
    switch (font)
    {
    case LCD_FONT_12: bufSize = 12; break;
    case LCD_FONT_16: bufSize = 16; break;
    case LCD_FONT_24: bufSize = 36; break;
    default: return;
    }
  }
  else //中文
  {
    switch (font)
    {
    case LCD_FONT_24: bufSize = 72; break;
    default: return;
    }
  }

  //获取点阵数据
  if(code < 0x80) //ASCII码
  {
    switch (font)
    {
    case LCD_FONT_12: enBuf = (u8*)asc2_1206[code - ' ']; break;
    case LCD_FONT_16: enBuf = (u8*)asc2_1608[code - ' ']; break;
    case LCD_FONT_24: enBuf = (u8*)asc2_2412[code - ' ']; break;
    default: return;
    }
    codeBuf = enBuf;
  }
  else //中文
  {
    if(LCD_FONT_24 == font)
    {
      GetCNFont24x24(code, cnBuf);
    }
    else
    {
      for(i = 0; i < bufSize; i++)
      {
        cnBuf[i] = 0xFF;
      }
    }
    codeBuf = cnBuf;
  }

  //记录纵坐标起点
  y0 = y;

  //按照高位在前，从上到下，从左往右扫描顺序显示
  for(i = 0; i < bufSize; i++)
  {
    //获取点阵数据
    byte = codeBuf[i];

    for(j = 0; j < 8; j++)
    {
      if(byte & 0x80)
      {
        LCDDrawPoint(x, y, textColor);
      }
      else if(LCD_TEXT_NORMAL == mode)
      {
        LCDDrawPoint(x, y, backColor);
      }

      byte <<= 1;
      y++;

      //超区域了
      if(y >= g_structTLILCDDev.height[g_structTLILCDDev.currentLayer])
      {
        return;
      }

      if((y - y0) >= height)
      {
        y = y0;
        x++;

        //超区域了
        if(x >= g_structTLILCDDev.width[g_structTLILCDDev.currentLayer])
        {
          return;
        }

        //字节切到下一个字节
        break;
      }
    }
  }
}
/*********************************************************************************************************
* 函数名称：LCDShowString
* 函数功能：显示字符串
* 输入参数：x,y：显示坐标
*          width：显示区域宽度
*          height：显示区域高度
*          font：字体，LCD_FONT_12、LCD_FONT_16或LCD_FONT_24
*          mode：显示模式，透明或非透明，LCD_TEXT_NORMAL或LCD_TEXT_TRANS
*          textColor：字符颜色
*          backColor：背景颜色
*          string：字符串首地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void LCDShowString(u32 x, u32 y, u32 width, u32 height, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, char* string)
{
  u32 x0, x1, y1, cWidth, i, code;
  
  //记录起始坐标
  x0 = x;

  //计算终点坐标
  x1 = x0 + width - 1;
  y1 = x1 + height - 1;

  //计算字符宽度，中文宽度是ASCII码的两倍
  switch (font)
  {
  case LCD_FONT_12: cWidth = 6; break;
  case LCD_FONT_16: cWidth = 8; break;
  case LCD_FONT_24: cWidth = 12; break;
  default: return;
  }

  //循环显示所有字符
  i = 0;
  while(0 != string[i])
  {
    if((u8)string[i] < 0x80) //ASCII码
    {
      //显示当前字符
      LCDShowChar(x, y, font, mode, textColor, backColor, string[i]);

      //更新x、y坐标
      if((x + cWidth) > x1)
      {
        x = x0;
        y = y + height;

        //纵坐标也超出范围，则写入结束
        if(y > y1)
        {
          return;
        }
      }
      else
      {
        x = x + cWidth;
      }

      //切换到下一个字符
      i = i + 1;
    }
    else
    {
      //获取汉字内码
      code = (string[i] << 8) | string[i + 1];

      //显示当前字符
      LCDShowChar(x, y, font, mode, textColor, backColor, code);

      //更新x、y坐标
      if((x + cWidth * 2) > x1)
      {
        x = x0;
        y = y + height;

        //纵坐标也超出范围，则写入结束
        if(y > y1)
        {
          return;
        }
      }
      else
      {
        x = x + cWidth * 2;
      }

      //切换到下一个字符
      i = i + 2;
    }
  }
}

/*********************************************************************************************************
* 函数名称：LCDWindowSave
* 函数功能：保存内指定区域图像
* 输入参数：x,y：起始坐标，width：宽度，height：高度，saveBuf：图像保存地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：因为保存方向是横屏，所以竖屏时必须与LCDWindowFill搭配使用
*********************************************************************************************************/
void LCDWindowSave(u32 x, u32 y, u32 width, u32 height, void* saveBuf)
{
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //前景图像参数结构体
  ipa_destination_parameter_struct ipa_destination_init_struct; //目标图像参数结构体
  u32 foreStartAddr;       //前景图像起始地址
  u32 phyWidth, phyHeight; //横屏宽度和高度
  u32 phyX, phyY;          //横屏时填充起始地址
  u32 lineOff;             //目标行偏移，最后一个像素和下一行第一个像素之间的像素数目
  u32 swap;                //交换数据时的临时变量

  //横屏，记录宽度、高低以及起始地址
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //竖屏，因为是按照横屏方式配置，所以要将宽度、高度转换过来，并做起始坐标转换
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //获取屏幕宽度和高度
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //交换屏幕宽度和高度
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //交换填充区域宽度和高度
    swap = width;
    width = height;
    height = swap;

    //坐标系转换
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //窗口区域大小修正，防止窗口区域超出屏幕范围
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //计算前景图像起始地址
  foreStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //计算前景行偏移
  lineOff = phyWidth - width;

  //使能IPA时钟
  rcu_periph_clock_enable(RCU_IPA);

  //复位IPA
  ipa_deinit();

  //设置转换模式
  ipa_pixel_format_convert_mode_set(IPA_FGTODE); //复制某一源图像到目标图像中

  //设置前景图像参数
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = FOREGROUND_PPF_RGB565; //前景层像素格式
  ipa_fg_init_struct.foreground_memaddr         = foreStartAddr;         //前景层存储区基地址
  ipa_fg_init_struct.foreground_lineoff         = lineOff;               //前景层行偏移
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;   //前景层alpha值计算算法，无影响
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //前景层预定义透明度
  ipa_fg_init_struct.foreground_prered          = 0x00; //前景层预定义红色值
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //前景层预定义绿色值
  ipa_fg_init_struct.foreground_preblue         = 0x00; //前景层预定义蓝色值
  ipa_foreground_init(&ipa_fg_init_struct);             //根据参数配置前景图像

  //设置目标图像参数
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565; //目标图像格式
  ipa_destination_init_struct.destination_memaddr  = (u32)saveBuf;   //目标图像存储地址
  ipa_destination_init_struct.destination_lineoff  = 0;              //行末与下一行开始之间的像素点数
  ipa_destination_init_struct.destination_prealpha = 0xFF;           //目标层预定义透明度
  ipa_destination_init_struct.destination_prered   = 0x00;           //目标层预定义红色值
  ipa_destination_init_struct.destination_pregreen = 0x00;           //目标层预定义绿色值
  ipa_destination_init_struct.destination_preblue  = 0x00;           //目标层预定义蓝色值
  ipa_destination_init_struct.image_width          = width;          //目标图像宽度
  ipa_destination_init_struct.image_height         = height;         //目标图像高度
  ipa_destination_init(&ipa_destination_init_struct);                //根据参数配置目标图像

  // //使能IPA内部定时器
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //开启传输
  ipa_transfer_enable();

  //等待传输结束
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //清除标志位
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //关闭IPA
  ipa_deinit();
}

/*********************************************************************************************************
* 函数名称：LCDWindowFill
* 函数功能：在内指定区域内绘制图像
* 输入参数：x,y：起始坐标，width：宽度，height：高度，imageBuf：图像所在地址
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：因为保存方向是横屏，所以竖屏时必须与LCDWindowSave搭配使用
*********************************************************************************************************/
void LCDWindowFill(u32 x, u32 y, u32 width, u32 height, void* imageBuf)
{
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //前景图像参数结构体
  ipa_destination_parameter_struct ipa_destination_init_struct; //目标图像参数结构体
  u32 destStartAddr;       //目标图像起始地址
  u32 phyWidth, phyHeight; //横屏宽度和高度
  u32 phyX, phyY;          //横屏时填充起始地址
  u32 lineOff;             //目标行偏移，最后一个像素和下一行第一个像素之间的像素数目
  u32 swap;                //交换数据时的临时变量

  //横屏，记录宽度、高低以及起始地址
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //竖屏，因为是按照横屏方式配置，所以要将宽度、高度转换过来，并做起始坐标转换
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //获取屏幕宽度和高度
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //交换屏幕宽度和高度
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //交换填充区域宽度和高度
    swap = width;
    width = height;
    height = swap;

    //坐标系转换
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //窗口区域大小修正，防止窗口区域超出屏幕范围
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //计算目标图像起始地址
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //计算目标行偏移
  lineOff = phyWidth - width;

  //使能IPA时钟
  rcu_periph_clock_enable(RCU_IPA);

  //复位IPA
  ipa_deinit();

  //设置转换模式
  ipa_pixel_format_convert_mode_set(IPA_FGTODE); //复制某一源图像到目标图像中

  //设置前景图像参数
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = FOREGROUND_PPF_RGB565; //前景层像素格式
  ipa_fg_init_struct.foreground_memaddr         = (u32)imageBuf;         //前景层存储区基地址
  ipa_fg_init_struct.foreground_lineoff         = 0;                     //前景层行偏移
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;   //前景层alpha值计算算法，无影响
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //前景层预定义透明度
  ipa_fg_init_struct.foreground_prered          = 0x00; //前景层预定义红色值
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //前景层预定义绿色值
  ipa_fg_init_struct.foreground_preblue         = 0x00; //前景层预定义蓝色值
  ipa_foreground_init(&ipa_fg_init_struct);             //根据参数配置前景图像

  //设置目标图像参数
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;     //目标图像格式
  ipa_destination_init_struct.destination_memaddr  = (u32)destStartAddr; //目标图像存储地址
  ipa_destination_init_struct.destination_lineoff  = lineOff; //行末与下一行开始之间的像素点数
  ipa_destination_init_struct.destination_prealpha = 0xFF;    //目标层预定义透明度
  ipa_destination_init_struct.destination_prered   = 0x00;    //目标层预定义红色值
  ipa_destination_init_struct.destination_pregreen = 0x00;    //目标层预定义绿色值
  ipa_destination_init_struct.destination_preblue  = 0x00;    //目标层预定义蓝色值
  ipa_destination_init_struct.image_width          = width;   //目标图像宽度
  ipa_destination_init_struct.image_height         = height;  //目标图像高度
  ipa_destination_init(&ipa_destination_init_struct);         //根据参数配置目标图像

  // //使能IPA内部定时器
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //开启传输
  ipa_transfer_enable();

  //等待传输结束
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //清除标志位
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //关闭IPA
  ipa_deinit();
}

/*********************************************************************************************************
* 函数名称：RGB565ToRGB888A
* 函数功能：RGB565转RGB888
* 输入参数：rgb656：RGB565值
*          r、g、b：转换结果
* 输出参数：void
* 返 回 值：void
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
void RGB565ToRGB888A(u16 rgb565, u8* r, u8* g, u8* b)
{
  *r = ((0xF800 & rgb565) >> 11) & 0xFF;
  *g = ((0x07E0 & rgb565) >> 5 ) & 0xFF;
  *b = ((0x001F & rgb565) << 0 ) & 0xFF;
}

/*********************************************************************************************************
* 函数名称：RGB565ToRGB888B
* 函数功能：RGB565转RGB888
* 输入参数：rgb656：RGB565值
* 输出参数：void
* 返 回 值：转换结果
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 RGB565ToRGB888B(u16 rgb565)
{
  u8 r, g, b;
  r = ((0xF800 & rgb565) >> 11) & 0xFF;
  g = ((0x07E0 & rgb565) >> 5 ) & 0xFF;
  b = ((0x001F & rgb565) >> 0 ) & 0xFF;
  return ((r << 16) | (g << 8) | (b << 0));
}

/*********************************************************************************************************
* 函数名称：RGB888ToRGB565A
* 函数功能：RGB888转RGB565
* 输入参数：r、g、b：RGB888
* 输出参数：void
* 返 回 值：转换结果
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 RGB888ToRGB565A(u8 r, u8 g, u8 b)
{
  r = r >> 3;
  g = g >> 2;
  b = b >> 3;

  return ((r << 11) | (g << 5) | (b << 0));
}

/*********************************************************************************************************
* 函数名称：RGB888ToRGB565A
* 函数功能：RGB888转RGB565
* 输入参数：r、g、b：RGB888
* 输出参数：void
* 返 回 值：转换结果
* 创建日期：2021年07月01日
* 注    意：
*********************************************************************************************************/
u32 RGB888ToRGB565B(u32 rgb888)
{
  u8 r, g, b;

  r = (rgb888 >> 16) & 0xFF;
  g = (rgb888 >> 8 ) & 0xFF;
  b = (rgb888 >> 0 ) & 0xFF;

  return RGB888ToRGB565A(r, g, b);
}
