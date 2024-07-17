/*********************************************************************************************************
* ģ�����ƣ�OV2640.h
* ժ    Ҫ��OV2640����ͷ����
* ��ǰ�汾��1.0.0
* ��    �ߣ�Leyutek(COPYRIGHT 2018 - 2021 Leyutek. All rights reserved.)
* ������ڣ�2021��07��01��
* ��    �ݣ�
* ע    �⣺
**********************************************************************************************************
* ȡ���汾��
* ��    �ߣ�
* ������ڣ�
* �޸����ݣ�
* �޸��ļ���
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "OV2640.h"
#include "OV2640Cfg.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "stdio.h"
#include "SCCB.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ConfigGPIO(void); //����OV2640��GPIO

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigGPIO
* �������ܣ�����OV2640��GPIO
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ConfigGPIO(void)
{
  //����RCCʱ��
  rcu_periph_clock_enable(RCU_GPIOA);  //ʹ��GPIOA��ʱ��
  rcu_periph_clock_enable(RCU_GPIOB);  //ʹ��GPIOB��ʱ��

  //PWDN
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

  //RESET
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitOV2640
* �������ܣ���ʼ��OV2640
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ�������-�������
* �������ڣ�2021��07��01��
* ע    �⣺�������Ժ�,Ĭ�������800*600�ߴ��ͼƬ
*********************************************************************************************************/
u8 InitOV2640(void)
{
  u16 i, reg;

  //����GPIO
  ConfigGPIO();

  //POWER ON
  OV2640PWDNSet(0);
  DelayNms(10);

  //��λOV2640
  gpio_bit_reset(GPIOB, GPIO_PIN_3);
  DelayNms(10);
  gpio_bit_set(GPIOB, GPIO_PIN_3);
  DelayNms(50);

  //��ʼ��SCCB
  InitSCCB();

  //����OV2640����ģʽ
  SCCBWriteReg(OV2640_DSP_RA_DLMT, 0x01); //����sensor�Ĵ���
  SCCBWriteReg(OV2640_SENSOR_COM7, 0x80); //��λOV2640
  DelayNms(50);

  //У��ID
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

  //��ʼ��OV2640������SVGA�ֱ���800*600)
  for(i = 0; i < sizeof(ov2640_svga_init_reg_tbl) / 2; i++)
  {
    SCCBWriteReg(ov2640_svga_init_reg_tbl[i][0], ov2640_svga_init_reg_tbl[i][1]);
  }
  return 0x00;
}

/*********************************************************************************************************
* �������ƣ�OV2640PWDNSet
* �������ܣ���������ͷģ��PWDN�ŵ�״̬
* ���������sta��0-PWDN=0���ϵ磻1-PWDN=1������
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640PWDNSet(u8 sta)
{
  gpio_bit_write(GPIOA, GPIO_PIN_15, (bit_status)sta);
}

/*********************************************************************************************************
* �������ƣ�OV2640PWDNSet
* �������ܣ�OV2640�л�ΪJPEGģʽ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640JPEGMode(void) 
{
  u16 i;

  //����:YUV422��ʽ
  for(i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
  }

  //����:���JPEG����
  for(i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);  
  }
}

/*********************************************************************************************************
* �������ƣ�OV2640RGB565Mode
* �������ܣ�OV2640�л�ΪRGB565ģʽ
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640RGB565Mode(void)
{
  u16 i = 0;

  //����:RGB565���
  for(i = 0; i < (sizeof(ov2640_rgb565_reg_tbl) / 2); i++)
  {
    SCCBWriteReg(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
  }
}

/*********************************************************************************************************
* �������ƣ�OV2640AutoExposure
* �������ܣ�OV2640�Զ��ع�ȼ�����
* ���������level��0-4
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640AutoExposure(u8 level)
{
  //�Զ��ع����ò�����,֧��5���ȼ�
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
* �������ƣ�OV2640LightMode
* �������ܣ���ƽ������
* ���������mode��
*             0:�Զ�
*             1:̫��sunny
*             2,����cloudy
*             3,�칫��office
*             4,����home
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�OV2640ColorSaturation
* �������ܣ�ɫ������
* ���������sat��
*            0:-2
*            1:-1
*            2,0
*            3,+1
*            4,+2
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�OV2640Brightness
* �������ܣ���������
* ���������bright��
*                0:(0x00)-2
*                1:(0x10)-1
*                2:(0x20) 0
*                3:(0x30)+1
*                4:(0x40)+2
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�OV2640Contrast
* �������ܣ��Աȶ�����
* ���������contrast��
*                 0:-2
*                 1:-1
*                 2:0
*                 3:+1
*                 4:+2
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640Contrast(u8 contrast)
{
  //Ĭ��Ϊ��ͨģʽ
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
* �������ƣ�OV2640SpecialEffects
* �������ܣ���Ч����
* ���������eft��
*            0:��ͨģʽ
*            1:��Ƭ
*            2:�ڰ�
*            3:ƫ��ɫ
*            4:ƫ��ɫ
*            5:ƫ��ɫ
*            6:����
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640SpecialEffects(u8 eft)
{
  //Ĭ��Ϊ��ͨģʽ
  u8 reg7d0val = 0x00;
  u8 reg7d1val = 0x80;
  u8 reg7d2val = 0x80;
  switch(eft)
  {
    case 1://��Ƭ
      reg7d0val = 0x40;
      break;
    case 2://�ڰ�
      reg7d0val = 0x18;
      break;
    case 3://ƫ��ɫ
      reg7d0val = 0x18;
      reg7d1val = 0x40;
      reg7d2val = 0xC0;
      break;
    case 4://ƫ��ɫ
      reg7d0val = 0x18;
      reg7d1val = 0x40;
      reg7d2val = 0x40;
      break;
    case 5://ƫ��ɫ
      reg7d0val = 0x18;
      reg7d1val = 0xA0;
      reg7d2val = 0x40;
      break;
    case 6://����
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
* �������ƣ�OV2640ColorBar
* �������ܣ���������
* ���������sw��0-�رղ�����1-��������(ע��OV2640�Ĳ����ǵ�����ͼ�������)
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�OV2640WindowSet
* �������ܣ�����ͼ���������
* ���������sx,sy-��ʼ��ַ��width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical)
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void OV2640WindowSet(u16 sx,u16 sy,u16 width,u16 height)
{
  u16 endx;
  u16 endy;
  u8 temp; 
  endx = sx + width / 2;  //V*2
  endy = sy + height / 2;

  SCCBWriteReg(0xFF, 0x01);
  temp = SCCBReadReg(0x03); //��ȡVref֮ǰ��ֵ
  temp &= 0xF0;
  temp |= ((endy & 0x03) << 2) | (sy & 0x03);
  SCCBWriteReg(0x03, temp);       //����Vref��start��end�����2λ
  SCCBWriteReg(0x19, sy >> 2);    //����Vref��start��8λ
  SCCBWriteReg(0x1A, endy >> 2);  //����Vref��end�ĸ�8λ
  temp = SCCBReadReg(0x32);       //��ȡHref֮ǰ��ֵ
  temp &= 0xC0;
  temp |= ((endx & 0x07) << 3) | (sx & 0x07);
  SCCBWriteReg(0x32, temp);       //����Href��start��end�����3λ
  SCCBWriteReg(0x17, sx >> 3);    //����Href��start��8λ
  SCCBWriteReg(0x18, endx >> 3);  //����Href��end�ĸ�8λ
}

/*********************************************************************************************************
* �������ƣ�OV2640OutSizeSet
* �������ܣ�����ͼ�������С
* ���������width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
* ���������void
* �� �� ֵ��0-���óɹ�������-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺OV2640���ͼ��Ĵ�С(�ֱ���),��ȫ�ɸĺ���ȷ��
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
  SCCBWriteReg(0x5A, outw & 0xFF);  //����OUTW�ĵͰ�λ
  SCCBWriteReg(0x5B, outh & 0xFF);  //����OUTH�ĵͰ�λ
  temp = (outw >> 8) & 0x03;
  temp |= (outh >> 6) & 0x04;
  SCCBWriteReg(0x5C,temp);          //����OUTH/OUTW�ĸ�λ
  SCCBWriteReg(0xE0,0x00);
  return 0;
}

/*********************************************************************************************************
* �������ƣ�OV2640ImageWinSet
* �������ܣ�����ͼ�񿪴���С
* ���������width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical),width��height������4�ı���
* ���������void
* �� �� ֵ��0-���óɹ�������-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺OV2640ImageSizeSetȷ������������ֱ��ʴӴ�С��
*          �ú������������Χ������п���,����OV2640_OutSize_Set�����
*          �������Ŀ�Ⱥ͸߶�,������ڵ���OV2640_OutSize_Set�����Ŀ�Ⱥ͸߶�
*          OV2640OutSizeSet���õĿ�Ⱥ͸߶�,���ݱ��������õĿ�Ⱥ͸߶�,��DSP�Զ��������ű���,������ⲿ�豸
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
  SCCBWriteReg(0x51, hsize & 0xFF);  //����H_SIZE�ĵͰ�λ
  SCCBWriteReg(0x52, vsize & 0xFF);  //����V_SIZE�ĵͰ�λ
  SCCBWriteReg(0x53, offx & 0xFF);   //����offx�ĵͰ�λ
  SCCBWriteReg(0x54, offy & 0xFF);   //����offy�ĵͰ�λ
  temp = (vsize >> 1) & 0x80;
  temp |= (offy >> 4) & 0x70;
  temp |= (hsize >> 5) & 0x08;
  temp |= (offx >> 8) & 0x07;
  SCCBWriteReg(0x55, temp);                 //����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
  SCCBWriteReg(0x57, (hsize >> 2) & 0x80);  //����H_SIZE/V_SIZE/OFFX,OFFY�ĸ�λ
  SCCBWriteReg(0xE0, 0x00);
  return 0;
}

/*********************************************************************************************************
* �������ƣ�OV2640ImageSizeSet
* �������ܣ��ú�������ͼ��ߴ��С��Ҳ������ѡ��ʽ������ֱ���
* ���������width,height:ͼ���Ⱥ�ͼ��߶�
* ���������void
* �� �� ֵ��0-���óɹ�������-����ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺UXGA:1600*1200,SVGA:800*600,CIF:352*288
*********************************************************************************************************/
u8 OV2640ImageSizeSet(u16 width,u16 height)
{ 
  u8 temp; 
  SCCBWriteReg(0xFF, 0x00);
  SCCBWriteReg(0xE0, 0x04);
  SCCBWriteReg(0xC0, (width >> 3) & 0xFF);  //����HSIZE��10:3λ
  SCCBWriteReg(0xC1, (height >> 3) & 0xFF); //����VSIZE��10:3λ
  temp = (width & 0x07) << 3;
  temp |= height & 0x07;
  temp |= (width >> 4) & 0x80; 
  SCCBWriteReg(0x8C, temp);
  SCCBWriteReg(0xE0, 0x00);
  return 0;
}
