/*********************************************************************************************************
* ģ�����ƣ�GT1151Q.c
* ժ    Ҫ��GT1151Q����������ģ��
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
#include "GT1151Q.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "IICCommon.h"
#include "stdio.h"
#include "Touch.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static StructIICCommonDev s_structIICDev; //IICͨ���豸�ṹ��

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static  void  ConfigGT1151QGPIO(void);  //����GT1151Q��GPIO

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigGT1151QGPIO
* �������ܣ�����GT1151Q��GPIO
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ConfigGT1151QGPIO(void)
{
  //ʹ��RCU���ʱ��
  rcu_periph_clock_enable(RCU_GPIOB);  //ʹ��GPIOB��ʱ��
  
  //SCL
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);         //�������
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_6); //�������
  gpio_bit_set(GPIOB, GPIO_PIN_6);                                              //����SCL
  
  //SDA
  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);         //�������
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_7); //�������
  gpio_bit_set(GPIOB, GPIO_PIN_7);                                              //����SCL

  //RST���͵�ƽ��Ч
  gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_9);         //�������
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_9); //�������
  gpio_bit_reset(GPIOG, GPIO_PIN_9);                                            //��SCLĬ��״̬����Ϊ����
}

/*********************************************************************************************************
* �������ƣ�ConfigGT1151QAddr
* �������ܣ�����GT1151Q���豸��ַΪ0x28/0x29
* ���������void 
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ConfigGT1151QAddr(void)
{
  //RST���ͣ���λGT1151Q
  gpio_bit_reset(GPIOG, GPIO_PIN_9);

  //ʹ��GPIOE��ʱ��
  rcu_periph_clock_enable(RCU_GPIOE);

  //INT��Ϊ���
  gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_3);
  gpio_bit_reset(GPIOE, GPIO_PIN_3);

  //��ʱ10����
  DelayNms(10);

  //����INT
  gpio_bit_set(GPIOE, GPIO_PIN_3);

  //��ʱ1����
  DelayNms(1);

  //RST���ߣ��ͷŸ�λ״̬
  gpio_bit_set(GPIOG, GPIO_PIN_9);

  //��ʱ10����
  DelayNms(10);

  //INT��Ϊ��������
  gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
}

/*********************************************************************************************************
* �������ƣ�ConfigSDAMode
* �������ܣ�����SDA�������
* ���������mode��IIC_COMMON_INPUT��IIC_COMMON_OUTPUT 
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static void ConfigSDAMode(u8 mode)
{
  rcu_periph_clock_enable(RCU_GPIOB);  //ʹ��GPIOB��ʱ��
  
  //���ó����
  if(IIC_COMMON_OUTPUT == mode)
  {
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);         //�������
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,GPIO_PIN_7); //�������
  }
  
  //���ó�����
  else if(IIC_COMMON_INPUT == mode)
  {
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);          //��������
  }
}

/*********************************************************************************************************
* �������ƣ�SetSCL
* �������ܣ�ʱ���ź���SCL����
* ���������state��0-����͵�ƽ��1-����ߵ�ƽ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
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
* �������ƣ�SetSDA
* �������ܣ������ź���SDA����
* ���������state��0-����͵�ƽ��1-����ߵ�ƽ
* ���������void
* �� �� ֵ��void
* �������ڣ�2018��01��01��
* ע    �⣺
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
* �������ƣ�GetSDA
* �������ܣ���ȡSDA�����ƽ
* ���������void 
* ���������void
* �� �� ֵ��SDA�����ƽ
* �������ڣ�2018��01��01��
* ע    �⣺
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
* �������ƣ�Delay
* �������ܣ���ʱ����
* ���������time����ʱʱ������
* ���������void
* �� �� ֵ��SDA�����ƽ
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
static void Delay(u8 time)
{
  DelayNus(5 * time);
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitGT1151Q
* �������ܣ���ʼ��LEDģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void InitGT1151Q(void)
{
  u8 id[5];

  //����GT1151Q��GPIO
  ConfigGT1151QGPIO();

  //����GT1151Q���豸��ַΪ0x28/0x29
  ConfigGT1151QAddr();

  //����IIC
  s_structIICDev.deviceID      = GT1151Q_DEVICE_ADDR; //�豸ID
  s_structIICDev.SetSCL        = SetSCL;              //����SCL��ƽֵ
  s_structIICDev.SetSDA        = SetSDA;              //����SDA��ƽֵ
  s_structIICDev.GetSDA        = GetSDA;              //��ȡSDA�����ƽ
  s_structIICDev.ConfigSDAMode = ConfigSDAMode;       //����SDA�����������
  s_structIICDev.Delay         = Delay;               //��ʱ����

  //�ȴ�GT1151Q�����ȶ�
  DelayNms(100);

  //����ƷID
  if(0 != IICCommonReadBytesEx(&s_structIICDev, GT1151Q_PID_REG, id, 4, IIC_COMMON_NACK))
  {
    printf("InitGT1151Q: Fail to get id\r\n");
    return;
  }

  //��ӡ��ƷID
  id[4] = 0;
  printf("Touch ID: %s\r\n", id);
}

/*********************************************************************************************************
* �������ƣ�ScanGT1151Q
* �������ܣ�����ɨ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*          1�����귶Χ480*800
*          2��������Ĭ�������������Ϸ�Ϊԭ��
*********************************************************************************************************/
void ScanGT1151Q(StructTouchDev* dev)
{
  static u16 s_arrRegAddr[5] = {GT1151Q_TP1_REG, GT1151Q_TP2_REG, GT1151Q_TP3_REG, GT1151Q_TP4_REG, GT1151Q_TP5_REG};
  u8  regValue;
  u8  buf[6];
  u8  i;
  u16 x0, y0;

  //��ȡ״̬�Ĵ���
  IICCommonReadBytesEx(&s_structIICDev, GT1151Q_GSTID_REG, &regValue, 1, IIC_COMMON_NACK);
  regValue = regValue & 0x0F;

  //��¼���������
  dev->pointNum = regValue;

  //���״̬�Ĵ���
  regValue = 0;
  IICCommonWriteBytesEx(&s_structIICDev, GT1151Q_GSTID_REG, &regValue, 1);

  //ѭ����ȡ5������������
  for(i = 0; i < 5; i++)
  {
    //��⵽����
    if(dev->pointNum >= (i + 1))
    {
      IICCommonReadBytesEx(&s_structIICDev, s_arrRegAddr[i], buf, 6, IIC_COMMON_NACK);
      dev->point[i].x    = (buf[1] << 8) | buf[0];
      dev->point[i].y    = (buf[3] << 8) | buf[2];
      dev->point[i].size = (buf[5] << 8) | buf[4];
      
      //��������ת��
      if(LCD_SCREEN_VERTICAL == g_structTLILCDDev.dir)
      {
        //�ݴ�ԭʼֵ
        x0 = dev->point[i].x;
        y0 = dev->point[i].y;

        //����ϵת��
        dev->point[i].x = y0;
        dev->point[i].y = LCD_PIXEL_WIDTH - x0;
      }

      //��Ǵ����㰴��
      dev->pointFlag[i] = 1;
    }

    //δ��⵽����
    else
    {
      dev->pointFlag[i] = 0;       //δ��⵽
      dev->point[i].x   = 0xFFFF;  //��Чֵ
      dev->point[i].y   = 0xFFFF;  //��Чֵ
    }
  }
}
