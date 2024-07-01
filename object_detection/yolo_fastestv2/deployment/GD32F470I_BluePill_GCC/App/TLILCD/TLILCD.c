/*********************************************************************************************************
* ģ�����ƣ�TLILCD.c
* ժ    Ҫ��LCD����ģ��
* ��ǰ�汾��1.0.0
* ��    �ߣ�SZLY(COPYRIGHT 2018 - 2020 SZLY. All rights reserved.)
* ������ڣ�2020��01��01��
* ��    �ݣ�
* ע    �⣺
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���Ĭ��ʹ��RGB565��ʽ��ɨ��ʱĬ�Ϻ���ɨ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "TLILCD.h"
#include "gd32f470x_conf.h"
#include "SDRAM.h"
#include "Font.h"
#include "FontLib.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
////�����Դ滺�����׵�ַ
//#define BACK_FRAME_START_ADDR SDRAM_DEVICE1_ADDR

////ǰ���Դ��׵�ַ
//#define FORE_FRAME_START_ADDR ((u32)(BACK_FRAME_START_ADDR + (LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT * 2))

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

//�����Դ滺�����׵�ַ
#define BACK_FRAME_START_ADDR (0xD1000000)

//ǰ���Դ��׵�ַ
#define FORE_FRAME_START_ADDR ((u32)(BACK_FRAME_START_ADDR + (LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT * 2))
//�����Դ�
static u16 s_arrBackgroundFrame[(LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT] __attribute__((section(".ARM.__at_0xD1000000")));

//ǰ���Դ�
static u16 s_arrForegroundFrame[(LCD_PIXEL_WIDTH + 1) * LCD_PIXEL_HEIGHT] __attribute__((section(".ARM.__at_0xD1200000")));

//LCD���ƽṹ��
StructTLILCDDev g_structTLILCDDev;

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitLCD
* �������ܣ���ʼ��LCDģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void InitLCD(void)
{
  u32 i;

  //TLI��ʼ���ṹ��
  tli_parameter_struct tli_init_struct;

  //��ʼ��LCD�豸�ṹ�壨Ĭ�Ϻ�������ǰ��Ϊ�����㣩
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

  //ʹ��RCUʱ��
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

  //LCD����
  gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
  gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
  gpio_bit_set(GPIOC,GPIO_PIN_13);

  //��λTLI
  tli_deinit();
  tli_struct_para_init(&tli_init_struct);

  //����TLIʱ�ӣ�ʱ��Ƶ��Ϊ1MHz * 200 / 5 / 4 = 10MHz�������ֲ���ĵ���ֵ��30MHz������10MHzˢ�����¸��ȶ���
  if(ERROR == rcu_pllsai_config(240, 4, 4)){while(1);}
  rcu_tli_clock_div_config(RCU_PLLSAIR_DIV2);
  // if(ERROR == rcu_pllsai_config(200, 4, 5)){while(1);}
  // rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);
  // if(ERROR == rcu_pllsai_config(50, 4, 5)){while(1);}
  // rcu_tli_clock_div_config(RCU_PLLSAIR_DIV4);
  rcu_osci_on(RCU_PLLSAI_CK);
  if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)){while(1);}

  //TLI��ʼ��
  //�źż�������
  tli_init_struct.signalpolarity_hs      = TLI_HSYN_ACTLIVE_LOW; //ˮƽͬ������͵�ƽ��Ч
  tli_init_struct.signalpolarity_vs      = TLI_VSYN_ACTLIVE_LOW; //��ֱͬ������͵�ƽ��Ч
  tli_init_struct.signalpolarity_de      = TLI_DE_ACTLIVE_LOW;   //����ʹ�ܵ͵�ƽ��Ч
  tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;  //����ʱ����TLIʱ��

  //��ʾʱ������
  tli_init_struct.synpsz_hpsz   = HORIZONTAL_SYNCHRONOUS_PULSE - 1; //ˮƽͬ��������
  tli_init_struct.synpsz_vpsz   = VERTICAL_SYNCHRONOUS_PULSE - 1;   //��ֱͬ��������
  tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1; //ˮƽ���ؼ�ͬ������Ŀ��
  tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;     //��ֱ���ؼ�ͬ������Ŀ��
  tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1; //ˮƽ��Ч��ȼӺ������غ�ˮƽͬ�����ؿ��
  tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;    //��ֱ��Ч��ȼӺ������غʹ�ֱͬ�����ؿ��
  tli_init_struct.totalsz_htsz  = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1; //��ʾ����ˮƽ�ܿ��
  tli_init_struct.totalsz_vtsz  = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;      //��ʾ���Ĵ�ֱ�ܿ��
  
  //����BG����ɫ����ɫ��
  tli_init_struct.backcolor_red = 0x00;
  tli_init_struct.backcolor_green = 0x00;
  tli_init_struct.backcolor_blue = 0x00;

  //���ݲ�������TLI
  tli_init(&tli_init_struct);

  //��ʼ���������ǰ����
  LCDLayerWindowSet(LCD_LAYER_BACKGROUND, 0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);
  LCDLayerWindowSet(LCD_LAYER_FOREGROUND, 0, 0, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);

  //ʹ�ܱ�����
  LCDLayerEnable(LCD_LAYER_BACKGROUND);

  //����ǰ����
  LCDLayerDisable(LCD_LAYER_FOREGROUND);

  //���ñ�����͸����
  LCDTransparencySet(LCD_LAYER_BACKGROUND, 0xFF);

  //����ǰ����͸����
  LCDTransparencySet(LCD_LAYER_FOREGROUND, 0xFF);

  //��������
  tli_dither_config(TLI_DITHER_ENABLE);

  // //�رն���
  // tli_dither_config(TLI_DITHER_DISABLE);

  //��������TLI�Ĵ���
  tli_reload_config(TLI_REQUEST_RELOAD_EN);

  //TLI����ʹ��
  tli_enable();

  //�л���������
  LCDLayerSwitch(LCD_LAYER_BACKGROUND);

  //������ʾ
  LCDDisplayDir(LCD_SCREEN_HORIZONTAL);

  //����
  LCDClear(LCD_COLOR_BLACK);
}

/*********************************************************************************************************
* �������ƣ�LCDLayerWindowSet
* �������ܣ����ò㴰��
* ���������layer��ǰ���򱳾��㶨�壨LCD_LAYER_FOREGROUND��LCD_LAYER_BACKGROUND��
*          x,y������λ��
*          width�����ڿ��
*          height�����ڸ߶�
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*          1�����ò㱻��ֹ��TLI_LxHPOS��TLI_LxVPOS����Ĵ���֮�⣬Ĭ����ɫֵ��Ч
*          2��LCD��Ļ����Ĭ���Ǻ���
*********************************************************************************************************/
void LCDLayerWindowSet(EnumTLILCDLayer layer, u32 x, u32 y, u32 width, u32 height)
{
  //TLI��ʼ���ṹ��
  tli_layer_parameter_struct  tli_layer_init_struct; 
  
  //��ʱ����
  u32 swap, x0, y0;
  
  //����Ĭ��ֵ
  tli_layer_struct_para_init(&tli_layer_init_struct);

  //�������⴦��
  if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //����ϵ�任
    x0 = x;
    y0 = y + height - 1;
    y = x0;
    x = LCD_PIXEL_WIDTH - 1 - y0;
    if(x >= LCD_PIXEL_WIDTH){return;}
    if(y >= LCD_PIXEL_HEIGHT){return;}

    //������Ⱥ͸߶�
    swap = width;
    width = height;
    height = swap;
  }

  //�������
  if((x + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - x;
  }

  //�����߶�
  if((y + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - y;
  }

  //��������
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_init_struct.layer_window_leftpos    = (x + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);             //�������λ��
    tli_layer_init_struct.layer_window_rightpos   = (x + width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1); //�����Ҳ�λ��
    tli_layer_init_struct.layer_window_toppos     = (y + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);                 //���ڶ���λ��
    tli_layer_init_struct.layer_window_bottompos  = (y + height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);    //���ڵײ�λ��
    tli_layer_init_struct.layer_ppf               = LAYER_PPF_RGB565;          //���ظ�ʽ
    tli_layer_init_struct.layer_sa                = 0xFF;                      //�㶨Aplhaֵ
    tli_layer_init_struct.layer_default_blue      = 0xFF;                      //Ĭ����ɫBֵ
    tli_layer_init_struct.layer_default_green     = 0xFF;                      //Ĭ����ɫGֵ
    tli_layer_init_struct.layer_default_red       = 0xFF;                      //Ĭ����ɫRֵ
    tli_layer_init_struct.layer_default_alpha     = 0xFF;                      //Ĭ����ɫAplhaֵ��Ĭ����ʾ
    tli_layer_init_struct.layer_acf1              = LAYER_ACF1_PASA;           //��һ��������Alpha���Թ�һ���ĺ㶨Alpha
    tli_layer_init_struct.layer_acf2              = LAYER_ACF2_PASA;           //��һ��������Alpha���Թ�һ���ĺ㶨Alpha
    tli_layer_init_struct.layer_frame_bufaddr     = (u32)s_arrBackgroundFrame; //�������׵�ַ
    tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);         //�г���
    tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);         //����ƫ�ƣ�ĳ����ʼ������һ����ʼ��֮����ֽ���
    tli_layer_init_struct.layer_frame_total_line_number = height;              //֡����
    tli_layer_init(LAYER0, &tli_layer_init_struct); //���ݲ�����ʼ��������
    tli_color_key_disable(LAYER0);                  //����ɫ��
    tli_lut_disable(LAYER0);                        //������ɫ���ұ�

    //���洰�ڿ�Ⱥ͸߶�
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
  
  //ǰ������
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_init_struct.layer_window_leftpos    = (x + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);             //�������λ��
    tli_layer_init_struct.layer_window_rightpos   = (x + width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1); //�����Ҳ�λ��
    tli_layer_init_struct.layer_window_toppos     = (y + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);                 //���ڶ���λ��
    tli_layer_init_struct.layer_window_bottompos  = (y + height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);    //���ڵײ�λ��
    tli_layer_init_struct.layer_ppf               = LAYER_PPF_RGB565;          //���ظ�ʽ
    tli_layer_init_struct.layer_sa                = 0xFF;                      //�㶨Aplhaֵ
    tli_layer_init_struct.layer_default_blue      = 0xFF;                      //Ĭ����ɫBֵ
    tli_layer_init_struct.layer_default_green     = 0xFF;                      //Ĭ����ɫGֵ
    tli_layer_init_struct.layer_default_red       = 0xFF;                      //Ĭ����ɫRֵ
    tli_layer_init_struct.layer_default_alpha     = 0x00;                      //Ĭ����ɫAplhaֵ��Ĭ�ϲ���ʾ
    tli_layer_init_struct.layer_acf1              = LAYER_ACF1_PASA;           //��һ��������Alpha���Թ�һ���ĺ㶨Alpha
    tli_layer_init_struct.layer_acf2              = LAYER_ACF2_PASA;           //��һ��������Alpha���Թ�һ���ĺ㶨Alpha
    tli_layer_init_struct.layer_frame_bufaddr     = (u32)s_arrForegroundFrame; //�������׵�ַ
    tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);         //�г���
    tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);         //����ƫ�ƣ�ĳ����ʼ������һ����ʼ��֮����ֽ���
    tli_layer_init_struct.layer_frame_total_line_number = height;              //֡����
    tli_layer_init(LAYER1, &tli_layer_init_struct); //���ݲ�����ʼ��ǰ����
    tli_color_key_disable(LAYER1);                  //����ɫ��
    tli_lut_disable(LAYER1);                        //������ɫ���ұ�

    //���洰�ڿ�Ⱥ͸߶�
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

  //��������TLI�Ĵ���
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* �������ƣ�LCDLayerEnable
* �������ܣ�ʹ�ܱ�����ǰ����
* ���������layer��ǰ���򱳾��㶨�壨LCD_LAYER_FOREGROUND��LCD_LAYER_BACKGROUND��
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDLayerEnable(EnumTLILCDLayer layer)
{
  //ʹ�ܱ�����
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_enable(LAYER0);
  }
  
  //ʹ��ǰ����
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_enable(LAYER1);
  }

  //��������TLI�Ĵ���
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* �������ƣ�LCDLayerDisable
* �������ܣ����ñ�����ǰ����
* ���������layer��ǰ���򱳾��㶨�壨LCD_LAYER_FOREGROUND��LCD_LAYER_BACKGROUND��
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDLayerDisable(EnumTLILCDLayer layer)
{
  //ʹ�ܱ�����
  if(LCD_LAYER_BACKGROUND == layer)
  {
    tli_layer_disable(LAYER0);
  }
  
  //ʹ��ǰ����
  else if(LCD_LAYER_FOREGROUND == layer)
  {
    tli_layer_disable(LAYER1);
  }

  //��������TLI�Ĵ���
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* �������ƣ�LCDLayerSwitch
* �������ܣ��л���
* ���������layer��ǰ���򱳾��㶨�壨LCD_LAYER_FOREGROUND��LCD_LAYER_BACKGROUND��
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDLayerSwitch(EnumTLILCDLayer layer)
{
  //�л���������
  if(LCD_LAYER_BACKGROUND == layer)
  {
    g_structTLILCDDev.frameBuf = s_arrBackgroundFrame;
    g_structTLILCDDev.currentLayer = LCD_LAYER_BACKGROUND;
  }
  
  //�л���ǰ����
  else
  {
    g_structTLILCDDev.frameBuf = s_arrForegroundFrame;
    g_structTLILCDDev.currentLayer = LCD_LAYER_FOREGROUND;
  }
}

/*********************************************************************************************************
* �������ƣ�LCDTransparencySet
* �������ܣ����õ�ǰ���͸����
* ���������trans��͸��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDTransparencySet(EnumTLILCDLayer layer, u8 trans)
{
  //���ñ���͸����
  if (LCD_LAYER_BACKGROUND == layer)
  {
    TLI_LxSA(LAYER0) &= ~(TLI_LxSA_SA);
    TLI_LxSA(LAYER0) = trans;
  }

  //����ǰ��͸����
  else
  {
    TLI_LxSA(LAYER1) &= ~(TLI_LxSA_SA);
    TLI_LxSA(LAYER1) = trans;
  }

  //��������TLI�Ĵ���
  tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

/*********************************************************************************************************
* �������ƣ�LCDDisplayOn
* �������ܣ�����LCD��ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDDisplayOn(void)
{
  tli_enable();
}

/*********************************************************************************************************
* �������ƣ�LCDDisplayOff
* �������ܣ��ر�LCD��ʾ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺��ΪRGB�����Դ��Դ棬���Թر�TLI��ͼ����ʾ������ʧ��ֱ������
*********************************************************************************************************/
void LCDDisplayOff(void)
{
  tli_disable();
}

/*********************************************************************************************************
* �������ƣ�LCDWaitHIdle
* �������ܣ��ȴ�LCD�д������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDWaitHIdle(void)
{
  while(RESET == tli_flag_get(TLI_FLAG_HDE));
  while(SET == tli_flag_get(TLI_FLAG_HDE));
}

/*********************************************************************************************************
* �������ƣ�LCDWaitVIdle
* �������ܣ��ȴ�LCD���������
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDWaitVIdle(void)
{
  while(RESET == tli_flag_get(TLI_FLAG_VDE));
  while(SET == tli_flag_get(TLI_FLAG_VDE));
}

/*********************************************************************************************************
* ��������: LCDDisplayDir
* ��������: ����LCD��ʾ����
* �������: dir����ʾ����LCD_SCREEN_HORIZONTAL��LCD_SCREEN_VERTICAL��
* �������: void
* �� �� ֵ: void
* ��������: 2018��01��01��
* ע    ��: 
*********************************************************************************************************/
void LCDDisplayDir(EnumTLILCDDir dir)
{
  u32 swap;

  //������Ⱥ͸߶�
  if(g_structTLILCDDev.dir != dir)
  {
    //������ʾ����
    g_structTLILCDDev.dir = dir;

    //�����������Ⱥ͸߶�
    swap = g_structTLILCDDev.width[LCD_LAYER_BACKGROUND];
    g_structTLILCDDev.width[LCD_LAYER_BACKGROUND] = g_structTLILCDDev.height[LCD_LAYER_BACKGROUND];
    g_structTLILCDDev.height[LCD_LAYER_BACKGROUND] = swap;

    //����ǰ�����Ⱥ͸߶�
    swap = g_structTLILCDDev.width[LCD_LAYER_FOREGROUND];
    g_structTLILCDDev.width[LCD_LAYER_FOREGROUND] = g_structTLILCDDev.height[LCD_LAYER_FOREGROUND];
    g_structTLILCDDev.height[LCD_LAYER_FOREGROUND] = swap;
  }
}

/*********************************************************************************************************
* �������ƣ�LCDClear
* �������ܣ�����
* ���������color�������ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDClear(u32 color)
{
  //Ŀ��ͼ������ṹ��
  ipa_destination_parameter_struct  ipa_destination_init_struct;

  //RGB
  u8 red, green, blue;

  //RGB565תRGB888
  RGB565ToRGB888A(color, &red, &green, &blue);

  //ʹ��IPAʱ��
  rcu_periph_clock_enable(RCU_IPA);

  //��λIPA
  ipa_deinit();

  //����ת��ģʽ
  ipa_pixel_format_convert_mode_set(IPA_FILL_UP_DE); //��ɫ���

  //����Ŀ��ͼ�����
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;                  //Ŀ��ͼ���ʽ
  ipa_destination_init_struct.destination_memaddr  = (u32)g_structTLILCDDev.frameBuf; //Ŀ��ͼ��洢��ַ
  ipa_destination_init_struct.destination_lineoff  = 0;                //��ĩ����һ�п�ʼ֮������ص���
  ipa_destination_init_struct.destination_prealpha = 0xFF;             //Ŀ���Ԥ����͸����
  ipa_destination_init_struct.destination_prered   = red;              //Ŀ���Ԥ�����ɫֵ
  ipa_destination_init_struct.destination_pregreen = green;            //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.destination_preblue  = blue;             //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.image_width          = LCD_PIXEL_WIDTH;  //Ŀ��ͼ����
  ipa_destination_init_struct.image_height         = LCD_PIXEL_HEIGHT; //Ŀ��ͼ��߶�
  ipa_destination_init(&ipa_destination_init_struct); //���ݲ�������Ŀ��ͼ��

  // //ʹ��IPA�ڲ���ʱ��
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //��������
  ipa_transfer_enable();

  //�ȴ��������
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //�����־λ
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //�ر�IPA
  ipa_deinit();
}

/*********************************************************************************************************
* �������ƣ�LCDDrawPoint
* �������ܣ�����
* ���������x, y�����꣬color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDDrawPoint(u32 x, u32 y, u32 color)
{
  u32 width, height;

  //��ȡ��ǰ���ڵĿ�Ⱥ͸߶�
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  height = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

  //���㷶Χ������Ļ����
  if(x >= width || y >= height)
  {
    return;
  }

  //����
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir) 
  {
    g_structTLILCDDev.frameBuf[width * y + x] = color;
  }

  //����
  else
  {
    g_structTLILCDDev.frameBuf[height * x + (height - 1 - y)] = color;
  }
}

/*********************************************************************************************************
* �������ƣ�LCDReadPoint
* �������ܣ�����
* ���������x, y������
* ���������void
* �� �� ֵ����ɫֵ
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 LCDReadPoint(u32 x, u32 y)
{
  u32 width, height;

  //��ȡ��ǰ���ڵĿ�Ⱥ͸߶�
  width = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
  height = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

  //���㷶Χ������Ļ���꣬Ĭ�Ϸ��غ�ɫ
  if(x >= width || y >= height)
  {
    return 0;
  }

  //����
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir) 
  {
    return g_structTLILCDDev.frameBuf[width * y + x];
  }

  //����
  else
  {
    return g_structTLILCDDev.frameBuf[height * x + (height - 1 - y)];
  }
}

/*********************************************************************************************************
* �������ƣ�LCDFill
* �������ܣ�LCD���
* ���������x, y����ʼ���꣬width����ȣ�height���߶ȣ�color�������ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺���ʱ���һ�Ż�Ī����������㣬ԭ��δ֪
*********************************************************************************************************/
void LCDFill(u32 x, u32 y, u32 width, u32 height, u32 color)
{
  ipa_destination_parameter_struct  ipa_destination_init_struct; //Ŀ��ͼ������ṹ��
  u32 destStartAddr;       //Ŀ��ͼ����ʼ��ַ
  u32 phyWidth, phyHeight; //������Ⱥ͸߶�
  u32 phyX, phyY;          //����ʱ�����ʼ��ַ
  u32 lineOff;             //Ŀ����ƫ�ƣ����һ�����غ���һ�е�һ������֮���������Ŀ
  u32 swap;                //��������ʱ����ʱ����
  u8  red, green, blue;    //RGB
  u32 x1, y1, x2, y2, color1, color2;

  //��������¼��ȡ��ߵ��Լ���ʼ��ַ
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //��������Ϊ�ǰ��պ�����ʽ���ã�����Ҫ����ȡ��߶�ת��������������ʼ����ת��
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //��ȡ��Ļ��Ⱥ͸߶�
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //������Ļ��Ⱥ͸߶�
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //������������Ⱥ͸߶�
    swap = width;
    width = height;
    height = swap;

    //����ϵת��
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //���������������ֹ������򳬳���Ļ��Χ
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //�����������Ϣ
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

  //����Ŀ��ͼ����ʼ��ַ
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //����Ŀ����ƫ��
  lineOff = phyWidth - width;

  //RGB565תRGB888
  RGB565ToRGB888A(color, &red, &green, &blue);

  //ʹ��IPAʱ��
  rcu_periph_clock_enable(RCU_IPA);

  //��λIPA
  ipa_deinit();

  //����ת��ģʽ
  ipa_pixel_format_convert_mode_set(IPA_FILL_UP_DE); //��ɫ���

  //����Ŀ��ͼ�����
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565; //Ŀ��ͼ���ʽ
  ipa_destination_init_struct.destination_memaddr  = destStartAddr;  //Ŀ��ͼ��洢��ַ
  ipa_destination_init_struct.destination_lineoff  = lineOff;        //��ĩ����һ�п�ʼ֮������ص���
  ipa_destination_init_struct.destination_prealpha = 0xFF;           //Ŀ���Ԥ����͸����
  ipa_destination_init_struct.destination_prered   = red;            //Ŀ���Ԥ�����ɫֵ
  ipa_destination_init_struct.destination_pregreen = green;          //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.destination_preblue  = blue;           //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.image_width          = width;          //Ŀ��ͼ����
  ipa_destination_init_struct.image_height         = height;         //Ŀ��ͼ��߶�
  ipa_destination_init(&ipa_destination_init_struct);                //���ݲ�������Ŀ��ͼ��

  // //ʹ��IPA�ڲ���ʱ��
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //��������
  ipa_transfer_enable();

  //�ȴ��������
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //�����־λ
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //�ر�IPA
  ipa_deinit();

  //������ص�
  LCDDrawPoint(x1, y1, color1);
  LCDDrawPoint(x2, y2, color2);
}

/*********************************************************************************************************
* �������ƣ�LCDFillPixel
* �������ܣ�LCD���
* ���������x0, y0����ʼ���꣬x1, y1���յ����꣬color�������ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  u32 swap, width, height;

  //У�������
  if(x0 > x1)
  {
    swap = x0;
    x0 = x1;
    x1 = swap;
  }

  //У��������
  if(y0 > y1)
  {
    swap = y0;
    y0 = y1;
    y1 = swap;
  }

  //������
  width = x1 - x0 + 1;

  //����߶�
  height = y1 - y0 + 1;

  //���
  LCDFill(x0, y0, width, height, color);
}

/*********************************************************************************************************
* �������ƣ�LCDColorFill
* �������ܣ�LCD���
* ���������x, y����ʼ���꣬width����ȣ�height���߶ȣ�color�������ɫ�������׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDColorFill(u32 x, u32 y, u32 width, u32 height, u16* color)
{
  u32 x0, y0, i;

  //������ʼ����
  x0 = x;
  y0 = y;

  //���
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
* �������ƣ�LCDFillPixel
* �������ܣ�LCD���
* ���������x0, y0����ʼ���꣬x1, y1���յ����꣬color�������ɫ�������׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDColorFillPixel(u32 x0, u32 y0, u32 x1, u32 y1, u16* color)
{
  u32 x, y, swap, i;

  //У�������
  if(x0 > x1)
  {
    swap = x0;
    x0 = x1;
    x1 = swap;
  }

  //У��������
  if(y0 > y1)
  {
    swap = y0;
    y0 = y1;
    y1 = swap;
  }

  //���
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
* �������ƣ�LCDDrawLine
* �������ܣ�����
* ���������x0��y0��������꣬x1��y1���յ����꣬color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
void LCDDrawLine(u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
  int x, y, dx, dy, dx2, dy2, xStep, yStep, swap, sum;

  //����x��y��������
  dx = x1 - x0;
  dy = y1 - y0;
  if(dx < 0){dx = -dx;}
  if(dy < 0){dy = -dy;}
  dx2 = dx << 1;
  dy2 = dy << 1;

  //б��С�ڵ���1���Ժ����������������
  if(dx >= dy)
  {
    //�������Ʒ��򣨰�����������ƣ�
    if(x0 > x1)
    {
      swap = x0; x0 = x1; x1 = swap;
      swap = y0; y0 = y1; y1 = swap;
    }

    //�ж�y��������
    if(y1 > y0){yStep = 1;}else{yStep = -1;}

    x = x0;
    y = y0;
    sum = -dx;
    while(x <= x1)
    {
      //����
      LCDDrawPoint(x, y, color);

      //����������
      x = x + 1;

      //�������б�ʽ
      sum = sum + dy2;

      //����������
      if(sum >= 0)
      {
        sum = sum - dx2;
        y = y + yStep;
      }
    }
  }

  //б�ʴ���1���������������������
  else
  {
    //�������Ʒ��򣨰�����������ƣ�
    if(y0 > y1)
    {
      swap = x0; x0 = x1; x1 = swap;
      swap = y0; y0 = y1; y1 = swap;
    }

    //�ж�x��������
    if(x1 > x0){xStep = 1;}else{xStep = -1;}

    x = x0;
    y = y0;
    sum = -dy;
    while(y <= y1)
    {
      //����
      LCDDrawPoint(x, y, color);

      //����������
      y = y + 1;

      //�������б�ʽ
      sum = sum + dx2;

      //����������
      if(sum >= 0)
      {
        sum = sum - dy2;
        x = x + xStep;
      }
    }
  }
}

/*********************************************************************************************************
* �������ƣ�LCDDrawRectangle
* �������ܣ�������
* ���������(x1,y1),(x2,y2):���εĶԽ����꣬color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void LCDDrawRectangle(u32 x1, u32 y1, u32 x2, u32 y2, u32 color)
{
  LCDDrawLine(x1, y1, x2, y1, color);
  LCDDrawLine(x1, y1, x1, y2, color);
  LCDDrawLine(x1, y2, x2, y2, color);
  LCDDrawLine(x2, y1, x2, y2, color);
}

/*********************************************************************************************************
* �������ƣ�LCDDrawCircle
* �������ܣ���Բ
* ���������x0��y0��Բ�����꣬r���뾶��color����ɫ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void LCDDrawCircle(u32 x0, u32 y0, u32 r, u32 color)
{
  int x, y, d;

  x = 0;
  y = r;
  d = 5 - (r << 2);
  while(x <= y)
  {
    //����Բ�İ˷ֶԳ��Ի���
    LCDDrawPoint(x0 + x, y0 + y, color);
    LCDDrawPoint(x0 + x, y0 - y, color);
    LCDDrawPoint(x0 - x, y0 + y, color);
    LCDDrawPoint(x0 - x, y0 - y, color);
    LCDDrawPoint(x0 + y, y0 + x, color);
    LCDDrawPoint(x0 - y, y0 + x, color);
    LCDDrawPoint(x0 + y, y0 - x, color);
    LCDDrawPoint(x0 - y, y0 - x, color);

    //����������
    x++;

    //ȡ�Ҳ��
    if(d < 0)
    {
      d = d + (x << 3) + 12;
    }

    //ȡ�Խǵ�
    else
    {
      d = d + ((x - y) << 3) + 20;
      y--;
    }
  }
}

/*********************************************************************************************************
* �������ƣ�LCDShowChar
* �������ܣ���ָ��λ����ʾһ���ַ�
* ���������x,y����ʾ����
*          font�����壬LCD_FONT_12��LCD_FONT_16��LCD_FONT_24
*          mode����ʾģʽ��͸�����͸����LCD_TEXT_NORMAL��LCD_TEXT_TRANS
*          textColor���ַ���ɫ
*          backColor��������ɫ
*          code���ַ����룬Ӣ�Ķ�ӦASCII�룬���ֶ�ӦGBK��������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺1. ɨ��˳���Ǹ�λ��ǰ�����ϵ��£��������ң�һ�������һ���ֽڵ�����
						2. ��ʾ�����ַ�ʱֻ��ʹ��LCD_FONT_24
*********************************************************************************************************/
void LCDShowChar(u32 x, u32 y, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, u32 code)
{
  u8  byte;      //��ǰ��������
  u32 i, j;      //ѭ������
  u32 y0;        //���������
  u32 height;    //�ַ��߶�
  u32 bufSize;   //���������ֽ���
  u8* enBuf;     //ASCII����������׵�ַ
  static u8  cnBuf[72]; //���ֵ������ݻ�����
  u8* codeBuf;   //�ַ����������׵�ַ����ѡһ

  //��ȡ�ַ��߶�
  switch (font)
  {
  case LCD_FONT_12: height = 12; break;
  case LCD_FONT_16: height = 16; break;
  case LCD_FONT_24: height = 24; break;
  default: return;
  }

  //������������ֽ���
  if(code < 0x80) //ASCII��
  {
    switch (font)
    {
    case LCD_FONT_12: bufSize = 12; break;
    case LCD_FONT_16: bufSize = 16; break;
    case LCD_FONT_24: bufSize = 36; break;
    default: return;
    }
  }
  else //����
  {
    switch (font)
    {
    case LCD_FONT_24: bufSize = 72; break;
    default: return;
    }
  }

  //��ȡ��������
  if(code < 0x80) //ASCII��
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
  else //����
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

  //��¼���������
  y0 = y;

  //���ո�λ��ǰ�����ϵ��£���������ɨ��˳����ʾ
  for(i = 0; i < bufSize; i++)
  {
    //��ȡ��������
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

      //��������
      if(y >= g_structTLILCDDev.height[g_structTLILCDDev.currentLayer])
      {
        return;
      }

      if((y - y0) >= height)
      {
        y = y0;
        x++;

        //��������
        if(x >= g_structTLILCDDev.width[g_structTLILCDDev.currentLayer])
        {
          return;
        }

        //�ֽ��е���һ���ֽ�
        break;
      }
    }
  }
}
/*********************************************************************************************************
* �������ƣ�LCDShowString
* �������ܣ���ʾ�ַ���
* ���������x,y����ʾ����
*          width����ʾ������
*          height����ʾ����߶�
*          font�����壬LCD_FONT_12��LCD_FONT_16��LCD_FONT_24
*          mode����ʾģʽ��͸�����͸����LCD_TEXT_NORMAL��LCD_TEXT_TRANS
*          textColor���ַ���ɫ
*          backColor��������ɫ
*          string���ַ����׵�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void LCDShowString(u32 x, u32 y, u32 width, u32 height, EnumTLILCDFont font, EnumTLILCDTextMode mode, u32 textColor, u32 backColor, char* string)
{
  u32 x0, x1, y1, cWidth, i, code;
  
  //��¼��ʼ����
  x0 = x;

  //�����յ�����
  x1 = x0 + width - 1;
  y1 = x1 + height - 1;

  //�����ַ���ȣ����Ŀ����ASCII�������
  switch (font)
  {
  case LCD_FONT_12: cWidth = 6; break;
  case LCD_FONT_16: cWidth = 8; break;
  case LCD_FONT_24: cWidth = 12; break;
  default: return;
  }

  //ѭ����ʾ�����ַ�
  i = 0;
  while(0 != string[i])
  {
    if((u8)string[i] < 0x80) //ASCII��
    {
      //��ʾ��ǰ�ַ�
      LCDShowChar(x, y, font, mode, textColor, backColor, string[i]);

      //����x��y����
      if((x + cWidth) > x1)
      {
        x = x0;
        y = y + height;

        //������Ҳ������Χ����д�����
        if(y > y1)
        {
          return;
        }
      }
      else
      {
        x = x + cWidth;
      }

      //�л�����һ���ַ�
      i = i + 1;
    }
    else
    {
      //��ȡ��������
      code = (string[i] << 8) | string[i + 1];

      //��ʾ��ǰ�ַ�
      LCDShowChar(x, y, font, mode, textColor, backColor, code);

      //����x��y����
      if((x + cWidth * 2) > x1)
      {
        x = x0;
        y = y + height;

        //������Ҳ������Χ����д�����
        if(y > y1)
        {
          return;
        }
      }
      else
      {
        x = x + cWidth * 2;
      }

      //�л�����һ���ַ�
      i = i + 2;
    }
  }
}

/*********************************************************************************************************
* �������ƣ�LCDWindowSave
* �������ܣ�������ָ������ͼ��
* ���������x,y����ʼ���꣬width����ȣ�height���߶ȣ�saveBuf��ͼ�񱣴��ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��Ϊ���淽���Ǻ�������������ʱ������LCDWindowFill����ʹ��
*********************************************************************************************************/
void LCDWindowSave(u32 x, u32 y, u32 width, u32 height, void* saveBuf)
{
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //ǰ��ͼ������ṹ��
  ipa_destination_parameter_struct ipa_destination_init_struct; //Ŀ��ͼ������ṹ��
  u32 foreStartAddr;       //ǰ��ͼ����ʼ��ַ
  u32 phyWidth, phyHeight; //������Ⱥ͸߶�
  u32 phyX, phyY;          //����ʱ�����ʼ��ַ
  u32 lineOff;             //Ŀ����ƫ�ƣ����һ�����غ���һ�е�һ������֮���������Ŀ
  u32 swap;                //��������ʱ����ʱ����

  //��������¼��ȡ��ߵ��Լ���ʼ��ַ
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //��������Ϊ�ǰ��պ�����ʽ���ã�����Ҫ����ȡ��߶�ת��������������ʼ����ת��
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //��ȡ��Ļ��Ⱥ͸߶�
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //������Ļ��Ⱥ͸߶�
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //������������Ⱥ͸߶�
    swap = width;
    width = height;
    height = swap;

    //����ϵת��
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //���������С��������ֹ�������򳬳���Ļ��Χ
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //����ǰ��ͼ����ʼ��ַ
  foreStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //����ǰ����ƫ��
  lineOff = phyWidth - width;

  //ʹ��IPAʱ��
  rcu_periph_clock_enable(RCU_IPA);

  //��λIPA
  ipa_deinit();

  //����ת��ģʽ
  ipa_pixel_format_convert_mode_set(IPA_FGTODE); //����ĳһԴͼ��Ŀ��ͼ����

  //����ǰ��ͼ�����
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = FOREGROUND_PPF_RGB565; //ǰ�������ظ�ʽ
  ipa_fg_init_struct.foreground_memaddr         = foreStartAddr;         //ǰ����洢������ַ
  ipa_fg_init_struct.foreground_lineoff         = lineOff;               //ǰ������ƫ��
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;   //ǰ����alphaֵ�����㷨����Ӱ��
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //ǰ����Ԥ����͸����
  ipa_fg_init_struct.foreground_prered          = 0x00; //ǰ����Ԥ�����ɫֵ
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_fg_init_struct.foreground_preblue         = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_foreground_init(&ipa_fg_init_struct);             //���ݲ�������ǰ��ͼ��

  //����Ŀ��ͼ�����
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565; //Ŀ��ͼ���ʽ
  ipa_destination_init_struct.destination_memaddr  = (u32)saveBuf;   //Ŀ��ͼ��洢��ַ
  ipa_destination_init_struct.destination_lineoff  = 0;              //��ĩ����һ�п�ʼ֮������ص���
  ipa_destination_init_struct.destination_prealpha = 0xFF;           //Ŀ���Ԥ����͸����
  ipa_destination_init_struct.destination_prered   = 0x00;           //Ŀ���Ԥ�����ɫֵ
  ipa_destination_init_struct.destination_pregreen = 0x00;           //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.destination_preblue  = 0x00;           //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.image_width          = width;          //Ŀ��ͼ����
  ipa_destination_init_struct.image_height         = height;         //Ŀ��ͼ��߶�
  ipa_destination_init(&ipa_destination_init_struct);                //���ݲ�������Ŀ��ͼ��

  // //ʹ��IPA�ڲ���ʱ��
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //��������
  ipa_transfer_enable();

  //�ȴ��������
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //�����־λ
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //�ر�IPA
  ipa_deinit();
}

/*********************************************************************************************************
* �������ƣ�LCDWindowFill
* �������ܣ�����ָ�������ڻ���ͼ��
* ���������x,y����ʼ���꣬width����ȣ�height���߶ȣ�imageBuf��ͼ�����ڵ�ַ
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺��Ϊ���淽���Ǻ�������������ʱ������LCDWindowSave����ʹ��
*********************************************************************************************************/
void LCDWindowFill(u32 x, u32 y, u32 width, u32 height, void* imageBuf)
{
  ipa_foreground_parameter_struct  ipa_fg_init_struct;          //ǰ��ͼ������ṹ��
  ipa_destination_parameter_struct ipa_destination_init_struct; //Ŀ��ͼ������ṹ��
  u32 destStartAddr;       //Ŀ��ͼ����ʼ��ַ
  u32 phyWidth, phyHeight; //������Ⱥ͸߶�
  u32 phyX, phyY;          //����ʱ�����ʼ��ַ
  u32 lineOff;             //Ŀ����ƫ�ƣ����һ�����غ���һ�е�һ������֮���������Ŀ
  u32 swap;                //��������ʱ����ʱ����

  //��������¼��ȡ��ߵ��Լ���ʼ��ַ
  if(LCD_SCREEN_HORIZONTAL == g_structTLILCDDev.dir)
  {
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];
    phyX = x;
    phyY = y;
  }

  //��������Ϊ�ǰ��պ�����ʽ���ã�����Ҫ����ȡ��߶�ת��������������ʼ����ת��
  else if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
  {
    //��ȡ��Ļ��Ⱥ͸߶�
    phyWidth = g_structTLILCDDev.width[g_structTLILCDDev.currentLayer];
    phyHeight = g_structTLILCDDev.height[g_structTLILCDDev.currentLayer];

    //������Ļ��Ⱥ͸߶�
    swap = phyWidth;
    phyWidth = phyHeight;
    phyHeight = swap;

    //������������Ⱥ͸߶�
    swap = width;
    width = height;
    height = swap;

    //����ϵת��
    phyX = phyWidth - y - width;
    phyY = x;
  }

  //���������С��������ֹ�������򳬳���Ļ��Χ
  if((phyX + width) > LCD_PIXEL_WIDTH)
  {
    width = LCD_PIXEL_WIDTH - phyX;
  }
  if((phyY + height) > LCD_PIXEL_HEIGHT)
  {
    height = LCD_PIXEL_HEIGHT - phyY;
  }

  //����Ŀ��ͼ����ʼ��ַ
  destStartAddr = (u32)g_structTLILCDDev.frameBuf + g_structTLILCDDev.pixelSize * (phyWidth * phyY + phyX);

  //����Ŀ����ƫ��
  lineOff = phyWidth - width;

  //ʹ��IPAʱ��
  rcu_periph_clock_enable(RCU_IPA);

  //��λIPA
  ipa_deinit();

  //����ת��ģʽ
  ipa_pixel_format_convert_mode_set(IPA_FGTODE); //����ĳһԴͼ��Ŀ��ͼ����

  //����ǰ��ͼ�����
  ipa_foreground_struct_para_init(&ipa_fg_init_struct);
  ipa_fg_init_struct.foreground_pf              = FOREGROUND_PPF_RGB565; //ǰ�������ظ�ʽ
  ipa_fg_init_struct.foreground_memaddr         = (u32)imageBuf;         //ǰ����洢������ַ
  ipa_fg_init_struct.foreground_lineoff         = 0;                     //ǰ������ƫ��
  ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;   //ǰ����alphaֵ�����㷨����Ӱ��
  ipa_fg_init_struct.foreground_prealpha        = 0xFF; //ǰ����Ԥ����͸����
  ipa_fg_init_struct.foreground_prered          = 0x00; //ǰ����Ԥ�����ɫֵ
  ipa_fg_init_struct.foreground_pregreen        = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_fg_init_struct.foreground_preblue         = 0x00; //ǰ����Ԥ������ɫֵ
  ipa_foreground_init(&ipa_fg_init_struct);             //���ݲ�������ǰ��ͼ��

  //����Ŀ��ͼ�����
  ipa_destination_struct_para_init(&ipa_destination_init_struct);
  ipa_destination_init_struct.destination_pf       = IPA_DPF_RGB565;     //Ŀ��ͼ���ʽ
  ipa_destination_init_struct.destination_memaddr  = (u32)destStartAddr; //Ŀ��ͼ��洢��ַ
  ipa_destination_init_struct.destination_lineoff  = lineOff; //��ĩ����һ�п�ʼ֮������ص���
  ipa_destination_init_struct.destination_prealpha = 0xFF;    //Ŀ���Ԥ����͸����
  ipa_destination_init_struct.destination_prered   = 0x00;    //Ŀ���Ԥ�����ɫֵ
  ipa_destination_init_struct.destination_pregreen = 0x00;    //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.destination_preblue  = 0x00;    //Ŀ���Ԥ������ɫֵ
  ipa_destination_init_struct.image_width          = width;   //Ŀ��ͼ����
  ipa_destination_init_struct.image_height         = height;  //Ŀ��ͼ��߶�
  ipa_destination_init(&ipa_destination_init_struct);         //���ݲ�������Ŀ��ͼ��

  // //ʹ��IPA�ڲ���ʱ��
  // ipa_interval_clock_num_config(0);
  // ipa_inter_timer_config(IPA_INTER_TIMER_ENABLE);

  //��������
  ipa_transfer_enable();

  //�ȴ��������
  while(RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF));

  //�����־λ
  ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);

  //�ر�IPA
  ipa_deinit();
}

/*********************************************************************************************************
* �������ƣ�RGB565ToRGB888A
* �������ܣ�RGB565תRGB888
* ���������rgb656��RGB565ֵ
*          r��g��b��ת�����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void RGB565ToRGB888A(u16 rgb565, u8* r, u8* g, u8* b)
{
  *r = ((0xF800 & rgb565) >> 11) & 0xFF;
  *g = ((0x07E0 & rgb565) >> 5 ) & 0xFF;
  *b = ((0x001F & rgb565) << 0 ) & 0xFF;
}

/*********************************************************************************************************
* �������ƣ�RGB565ToRGB888B
* �������ܣ�RGB565תRGB888
* ���������rgb656��RGB565ֵ
* ���������void
* �� �� ֵ��ת�����
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�RGB888ToRGB565A
* �������ܣ�RGB888תRGB565
* ���������r��g��b��RGB888
* ���������void
* �� �� ֵ��ת�����
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 RGB888ToRGB565A(u8 r, u8 g, u8 b)
{
  r = r >> 3;
  g = g >> 2;
  b = b >> 3;

  return ((r << 11) | (g << 5) | (b << 0));
}

/*********************************************************************************************************
* �������ƣ�RGB888ToRGB565A
* �������ܣ�RGB888תRGB565
* ���������r��g��b��RGB888
* ���������void
* �� �� ֵ��ת�����
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 RGB888ToRGB565B(u32 rgb888)
{
  u8 r, g, b;

  r = (rgb888 >> 16) & 0xFF;
  g = (rgb888 >> 8 ) & 0xFF;
  b = (rgb888 >> 0 ) & 0xFF;

  return RGB888ToRGB565A(r, g, b);
}
