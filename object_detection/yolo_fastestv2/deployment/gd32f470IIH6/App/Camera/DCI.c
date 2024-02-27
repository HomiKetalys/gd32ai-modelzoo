/*********************************************************************************************************
* ģ�����ƣ�DCI.c
* ժ    Ҫ����������ͷ�ӿ�����ģ��
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
#include "DCI.h"
#include "gd32f470x_conf.h"
#include "stdio.h"
#include "TLILCD.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/
#define DCI_DR_ADDRESS     (0x50050028U)
#define DCI_TIMEOUT        20000

/*********************************************************************************************************
*                                              ö�ٽṹ��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/
u32 s_iFrameAddr, s_iFrameSize;        //֡��ַ�Լ�֡��С
u32 s_iX0, s_iY0, s_iWidth, s_iHeight; //ͼ����ʾ��Ϣ
u32 s_iMode;                           //��ʾģʽ��0-ֱ�Ӽ��ص��Դ棬1-���ص���ʱ������������ʾ
u32 s_iGetFrameFlag;                   //���1֡ͼ���־λ

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void ConfigDCIGPIO(void); //��ʼ��DCI��GPIO
static void ConfigCKOut0(void);  //����CKOut0���
void ConfigDCI(void);     //����DCI

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�ConfigDCIGPIO
* �������ܣ���ʼ��DCI��GPIO
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void ConfigDCIGPIO(void)
{
  //ʹ��RCC���ʱ��
  rcu_periph_clock_enable(RCU_DCI);    //ʹ��DCIʱ��
  rcu_periph_clock_enable(RCU_GPIOA);  //ʹ��GPIOA��ʱ��
  rcu_periph_clock_enable(RCU_GPIOB);  //ʹ��GPIOB��ʱ��
  rcu_periph_clock_enable(RCU_GPIOC);  //ʹ��GPIOC��ʱ��
  rcu_periph_clock_enable(RCU_GPIOD);  //ʹ��GPIOD��ʱ��
  rcu_periph_clock_enable(RCU_GPIOH);  //ʹ��GPIOH��ʱ��
  rcu_periph_clock_enable(RCU_GPIOI);  //ʹ��GPIOI��ʱ��

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
* �������ƣ�ConfigCKOut0
* �������ܣ�����CKOut0���
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
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
* �������ƣ�ConfigDCI
* �������ܣ�����DCI
* ���������frameAddr��ͼ��֡�����ַ��frameSize��1֡ͼ���С�����ֽڼ��㣩
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺Ҫ����DCI��������жϲ���������DCI��ֱ�ӹҵ�������
*********************************************************************************************************/
void ConfigDCI(void)
{
  dci_parameter_struct dci_struct;
  dma_single_data_parameter_struct dma_single_struct;

  //ʹ��RCC���ʱ��
  rcu_periph_clock_enable(RCU_DCI);    //ʹ��DCIʱ��
  rcu_periph_clock_enable(RCU_DMA1);   //ʹ��DMA1ʱ��

  //DCI����
  dci_deinit();                                              //��λDCI
  if(0 == s_iMode)
  {
    dci_struct.capture_mode   = DCI_CAPTURE_MODE_CONTINUOUS; //��������ģʽ��������ͼ������ֱ�ӱ��浽LCD�Դ�
  }
  else
  {
    dci_struct.capture_mode   = DCI_CAPTURE_MODE_SNAPSHOT;   //ʹ�ÿ���ģʽ��ͨ�����㺯����ͼ����ʾ��LCD��
  }
  dci_struct.clock_polarity   = DCI_CK_POLARITY_RISING;      //ʱ�Ӽ���ѡ��
  dci_struct.hsync_polarity   = DCI_HSYNC_POLARITY_LOW;      //ˮƽͬ������ѡ��
  dci_struct.vsync_polarity   = DCI_VSYNC_POLARITY_LOW;      //��ֱͬ������ѡ��
  dci_struct.frame_rate       = DCI_FRAME_RATE_ALL;          //��������֡
  dci_struct.interface_format = DCI_INTERFACE_FORMAT_8BITS;  //ÿ������ʱ�Ӳ��� 8 λ����
  dci_init(&dci_struct);                                     //���ݲ�������DCI
  nvic_irq_enable(DCI_IRQn, 2, 0);                           //ʹ��DCI NVIC�ж�
  dci_interrupt_enable(DCI_INT_OVR);                         //ʹ��DCI FIFO����жϣ��ǳ���Ҫ��������
  dci_interrupt_flag_clear(DCI_INT_OVR);                     //�������жϱ�־λ
  if(0 != s_iMode)
  {
    dci_interrupt_enable(DCI_INT_EF);                        //֡�����ж�ʹ�ܣ����Ի���һ֡ͼ��LCD�ϲ�������һ�β���
    dci_interrupt_flag_clear(DCI_INT_EF);                    //���֡�����жϱ�־λ
  }

  //DMA����
  dma_deinit(DMA1, DMA_CH7);                                           //��λDMA
  dma_single_struct.periph_addr         = (u32)&DCI_DATA;              //���������ַ
  dma_single_struct.memory0_addr        = (u32)s_iFrameAddr;           //���ô洢����ַ
  dma_single_struct.direction           = DMA_PERIPH_TO_MEMORY;        //���䷽��Ϊ���赽�ڴ�
  dma_single_struct.number              = s_iFrameSize / 4;            //���ô���������
  dma_single_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE; //��ֹ�����ַ����
  dma_single_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;  //�����ڴ��ַ����
  dma_single_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;      //�������ݿ��Ϊ32λ
  dma_single_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;     //�߳����ȼ�
  if(0 == s_iMode)
  {
    dma_single_struct.circular_mode       = DMA_CIRCULAR_MODE_ENABLE;  //DCI��������ģʽ�¿���ѭ��ģʽ
  }
  else
  {
    dma_single_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE; //DCI����ģʽ�½���ѭ��ģʽ
  }
  dma_single_data_mode_init(DMA1, DMA_CH7, &dma_single_struct);        //���ݲ�������DMAͨ��
  dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI1);       //��DMAͨ������Ӧ����
  dma_flow_controller_config(DMA1, DMA_CH7, DMA_FLOW_CONTROLLER_DMA);  //DMA��Ϊ���������
  dma_channel_enable(DMA1, DMA_CH7);                                   //ʹ��DMA����

  dci_enable();         //ʹ��DCI
  dci_capture_enable(); //��������
}

/*********************************************************************************************************
* �������ƣ�DCI_IRQHandler
* �������ܣ�DCI�жϷ�����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DCI_IRQHandler(void)
{
  //֡����
  if(SET == dci_interrupt_flag_get(DCI_INT_FLAG_EF))
  {
    //��ǻ��1֡����
    s_iGetFrameFlag = 1;

    //����жϱ�־λ
    dci_interrupt_flag_clear(DCI_INT_EF);
  }

  //FIFO�����������������ǰ֡��ֱ�ӿ�����һ֡ͼ����
  if(SET == dci_interrupt_flag_get(DCI_INT_FLAG_OVR))
  {
    dci_interrupt_flag_clear(DCI_INT_OVR);
    ConfigDCI();
  }
}

/*********************************************************************************************************
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitDCI
* �������ܣ���ʼ����������ͷ�ӿ�����ģ��
* ���������frameAddr��ͼ��֡�����ַ���������ڲ�SRAM��Ҳ�������ⲿSDRAM
*          frameSize��1֡ͼ���С�����ֽڼ��㣩
*          x0��Y0��ͼ����ʾ��㣬mode����ʱʹ��
*          width��heigh��ͼ���С��mode����ʱʹ��
*          mode��0-�Ƽ�����ʱʹ�ã�����DMAֱ�ӽ�DCI���յ������ݴ��䵽�Դ棬���������ڴ�ռ䣬ˢ���ٶȿ�
*                1-��DCI���յ������ݱ��浽��ʱ��������Ȼ����ͨ�����㺯����һ֡ͼ����Ƴ������ٶȽ���
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺����ʱ�Ƽ�����˫����ʾ��ѡ��ģʽ0��DMAֱ�ӽ����ݴ��䵽�Դ棬�������ˢ����
*********************************************************************************************************/
void InitDCI(u32 frameAddr, u32 frameSize, u32 x0, u32 y0, u32 width, u32 height, u32 mode)
{
  //�������
  s_iFrameAddr = frameAddr;
  s_iFrameSize = frameSize;
  s_iX0 = x0;
  s_iY0 = y0;
  s_iWidth = width;
  s_iHeight = height;
  s_iMode = mode;

  //����DCI��GPIO
  ConfigDCIGPIO();

  //����CK_OUT0���
  ConfigCKOut0();

  //����DCI����
  s_iGetFrameFlag = 0;
  ConfigDCI();
}

/*********************************************************************************************************
* �������ƣ�DeInitDCI
* �������ܣ�ע����������ͷ�ӿ�����ģ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DeInitDCI(void)
{
  //�ر�DMA����
  dma_channel_disable(DMA1, DMA_CH7);

  //��λDCI
  dci_deinit();

  //��ֹDCI_XCLK���
  gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
}

/*********************************************************************************************************
* �������ƣ�DCIShowImage
* �������ܣ���ʾһ֡ͼ����Ļ��
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
void DCIShowImage(void)
{
  u32 x0, y0, x1, y1, x, y, i;
  u16* frame;

  if((0 != s_iMode) && (1 == s_iGetFrameFlag))
  {
    //��һ֡ͼ����ʾ����Ļ��
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

    //������һ֡����
    s_iGetFrameFlag = 0;
    ConfigDCI();
  }
}
