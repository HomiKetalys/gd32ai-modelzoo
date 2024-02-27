/*********************************************************************************************************
* ģ�����ƣ�NandFlash.c
* ժ    Ҫ��NandFlash����ģ��
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
#include "NandFlash.h"
#include "gd32f470x_conf.h"
#include "SysTick.h"
#include "stdio.h"

/*********************************************************************************************************
*                                              �궨��
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ�����
*********************************************************************************************************/

/*********************************************************************************************************
*                                              �ڲ���������
*********************************************************************************************************/
static void NandDelay(u32 time); //Nand��ʱ����
static u32  NANDWaitRB(u8 rb);

/*********************************************************************************************************
*                                              �ڲ�����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�NandDelay
* �������ܣ�Nand��ʱ����
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
static void NandDelay(u32 time)
{
  while(time > 0)
  {
    time--;
  }
}

/*********************************************************************************************************
* �������ƣ�NAND_WaitRB
* �������ܣ��ȴ�RB�ź�Ϊĳ����ƽ
* ���������rb:0,�ȴ�RB==0
*             1,�ȴ�RB==1
* ���������void
* �� �� ֵ��0-�ɹ���1-��ʱ
* �������ڣ�2018��01��01��
* ע    �⣺���MCU��Ƶ�ϵͣ��򲻽�����NANDWaitRB����У��NWAIT�ź��½��غ������أ���Ϊ�п���NWAIT�źű仯̫�죬
*           MCU��ⲻ������ʱֱ������ʱ�ͺ�
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
*                                              API����ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
* �������ƣ�InitNandFlash
* �������ܣ���ʼ��NandFlash����ģ��
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ�������-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 InitNandFlash(void)
{
  u32 id;
  exmc_nand_parameter_struct nand_init_struct;
  exmc_nand_pccard_timing_parameter_struct nand_timing_init_struct;

  //ʹ��EXMCʱ��
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

  //����EXMC��Nand Flash��SDRAM��TLILCD����EXMC���ߣ�Nand Flash��д��������ʱ���ܻᵼ����Ļ�����������������󣬶�д��ɺ�������ˣ�
  // nand_timing_init_struct.setuptime       = 3; //����ʱ��
  // nand_timing_init_struct.waittime        = 3; //�ȴ�ʱ��
  // nand_timing_init_struct.holdtime        = 2; //����ʱ��
  // nand_timing_init_struct.databus_hiztime = 1; //����̬ʱ��
  nand_timing_init_struct.setuptime       = 5; //����ʱ��
  nand_timing_init_struct.waittime        = 4; //�ȴ�ʱ��
  nand_timing_init_struct.holdtime        = 2; //����ʱ��
  nand_timing_init_struct.databus_hiztime = 2; //����̬ʱ��
  nand_init_struct.nand_bank              = EXMC_BANK1_NAND;            //NAND����BANK1��
  nand_init_struct.ecc_size               = EXMC_ECC_SIZE_2048BYTES;    //ECCҳ��СΪ2048�ֽ�
  nand_init_struct.atr_latency            = EXMC_ALE_RE_DELAY_1_HCLK;   //����TAR
  nand_init_struct.ctr_latency            = EXMC_CLE_RE_DELAY_1_HCLK;   //����TCLR
  nand_init_struct.ecc_logic              = ENABLE;                     //ʹ��ECC
  nand_init_struct.databus_width          = EXMC_NAND_DATABUS_WIDTH_8B; //8λ���ݿ��
  nand_init_struct.wait_feature           = DISABLE;                    //�رյȴ�����
  nand_init_struct.common_space_timing    = &nand_timing_init_struct;   //ͨ��ʱ������
  nand_init_struct.attribute_space_timing = &nand_timing_init_struct;   //�ռ�ʱ������
  exmc_nand_init(&nand_init_struct); //���ݲ�����ʼ��Nand Flash
  exmc_nand_enable(EXMC_BANK1_NAND); //ʹ��Nand Flash

  NandReset();       //��λNAND
  DelayNms(100);     //�ȴ�100ms
  id = NandReadID(); //��ȡID
  printf("InitNandFlash: Nand Flash ID: 0x%08X\r\n", id);
  if(0x1D80F1AD != id)
  {
    printf("InitNandFlash: fail to check nand flash ID\r\n");
    while(1);
  }

  return 0;
}

/*********************************************************************************************************
* �������ƣ�NandReadID
* �������ܣ�������ID
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandReadID(void)
{
  u32 id = 0;

  //���Ͷ�����ID����
  NAND_CMD_AREA = NAND_CMD_READID;
  NAND_ADDR_AREA = 0x00;

  //tAR�ȴ��ӳ�
  NandDelay(NAND_TAR_DELAY);

  //tREA�ȴ���ʱ
  NandDelay(NAND_TREA_DELAY);

  //��ȡ����ID
  id = *(__IO u32 *)(BANK_NAND_ADDR | EXMC_DATA_AREA);
  return id;
}

/*********************************************************************************************************
* �������ƣ�NandReadStatus
* �������ܣ�������״̬
* ���������void
* ���������void
* �� �� ֵ��void
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandReadStatus(void)
{
  u8 data;
  u8 status = NAND_BUSY;

  //���Ͷ�ȡ״̬����
  NAND_CMD_AREA = NAND_CMD_STATUS;

  //tWHR�ȴ��ӳ�
  NandDelay(NAND_TWHR_DELAY);

  //tREA�ȴ���ʱ
  NandDelay(NAND_TREA_DELAY);

  //��ȡ����״̬
  data = NAND_DATA_AREA;

  //����
  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  }

  //��������
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }

  //æµ
  else
  {
    status = NAND_BUSY;
  }

  //��������״̬
  return status;
}

/*********************************************************************************************************
* �������ƣ�NandWaitReady
* �������ܣ��ȴ���������
* ���������void
* ���������void
* �� �� ֵ��״̬��־λ
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandWaitReady(void)
{
  u32 timeout = 0x10000;
  u32 status = NAND_READY;

  //��ȡ����״̬
  status = NandReadStatus();

  //��ʱ�ȴ�
  while((status != NAND_READY) && (timeout != 0x00))
  {
    status = NandReadStatus();
    timeout--;
  }

  //�ȴ���ʱ
  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  return status;
}

/*********************************************************************************************************
* �������ƣ�NandWritePage
* �������ܣ�ҳд(main��)
* ���������block ������
*          page  ������ҳ���
*          buf   ����������ַ
* ���������void
* �� �� ֵ��0-�ɹ���1-д��ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��ֻ��д��Main����ֻ�ܰ�ҳ��д��д��֮ǰ���ֶ�����ҳ���ڿ�
*          2����һ���ֽڣ�CA7  CA6  CA5  CA4  CA3  CA2  CA1  CA0  ����8λ�е�ַ��
*          3���ڶ����ֽڣ�0    0    0    0    CA11 CA10 CA9  CA8  ����4λ�е�ַ��
*          4���������ֽڣ�BA1  BA0  PA5  PA4  PA3  PA2  PA1  PA0  ����2λ���ַ��ҳ��ַ��
*          5�����ĸ��ֽڣ�BA9  BA8  BA7  BA6  BA5  BA4  BA3  BA2  ����8λ���ַ��
*          6��ECCֵ�ֱ𴢴���[16:19]
*********************************************************************************************************/
u32 NandWritePage(u32 block, u32 page, u8* buf)
{
  u32 i;        //ѭ������
  u32 ecc;      //Ӳ��ECCֵ
  u32 eccAddr;  //ECC�洢��ַ
  u8  check;    //�ɹ�д���־λ

  //����д���ַ
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //����д����
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ��ҳ��ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NandDelay(NAND_TADL_DELAY);                    //tADL�ȴ��ӳ�

  //��λECC
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

  //д������
  for(i = 0; i < NAND_PAGE_SIZE; i++)
  {
    //д������
    NAND_DATA_AREA = buf[i];
  }

  //��ȡECC
  //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
  DelayNus(10);
  ecc = exmc_ecc_get(EXMC_BANK1_NAND);

  //����д��ECC��spare����ַ
  eccAddr = NAND_PAGE_SIZE + 16;

  //����д��Spareλ��
  NandDelay(NAND_TADL_DELAY);             //tADL�ȴ��ӳ�
  NAND_CMD_AREA  = 0x85;                  //���д����
  NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //���д��ַ��λ
  NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //���д��ַ��λ
  NandDelay(NAND_TADL_DELAY);             //tADL�ȴ��ӳ�

  //��ECCд��Spare��ָ��λ��
  NAND_DATA_AREA = (ecc >> 0)  & 0xFF;
  NAND_DATA_AREA = (ecc >> 8)  & 0xFF;
  NAND_DATA_AREA = (ecc >> 16) & 0xFF;
  NAND_DATA_AREA = (ecc >> 24) & 0xFF;

  //����д���������
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB�ȴ��ӳ�
  NandDelay(NAND_TWB_DELAY);

  //tPROG�ȴ��ӳ�
  DelayNus(NAND_TPROG_DELAY);
  
  //�������״̬
  if(NAND_READY != NandWaitReady())
  {
    return NAND_FAIL;
  }

  //У��д���Ƿ�ɹ�
  check = NAND_DATA_AREA;
  if(check & 0x01)
  {
    return NAND_FAIL;
  }

  //д��ɹ�
  return NAND_OK;
}

/*********************************************************************************************************
* �������ƣ�NandReadPage
* �������ܣ�ҳ��(main��)
* ���������block ������
*          page  ������ҳ���
*          buf   ����������ַ
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��ֻ��д��Main����ֻ�ܰ�ҳ��д��д��֮ǰ���ֶ�����ҳ���ڿ�
*          2����һ���ֽڣ�CA7  CA6  CA5  CA4  CA3  CA2  CA1  CA0  ����8λ�е�ַ��
*          3���ڶ����ֽڣ�0    0    0    0    CA11 CA10 CA9  CA8  ����4λ�е�ַ��
*          4���������ֽڣ�BA1  BA0  PA5  PA4  PA3  PA2  PA1  PA0  ����2λ���ַ��ҳ��ַ��
*          5�����ĸ��ֽڣ�BA9  BA8  BA7  BA6  BA5  BA4  BA3  BA2  ����8λ���ַ��
*          6��ECCֵ�ֱ𴢴���[16:19]
*********************************************************************************************************/
u32 NandReadPage(u32 block, u32 page, u8* buf)
{
  u32 i;        //ѭ������
  u32 eccHard;  //Ӳ��ECCֵ
  u32 eccFlash; //Flash�е�ECCֵ
  u32 eccAddr;  //ECC�洢��ַ
  u8  spare[4]; //��дSpare������

  //���ö�ȡ��ַ
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //���Ͷ�����
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //���������
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //�ȴ�RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //�ȴ�RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR��ʱ�ȴ�

  //��λECC
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

  //��ȡ����
  exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
  exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);
  for(i = 0; i < NAND_PAGE_SIZE; i++)
  {
    //��ȡ����
    buf[i] = NAND_DATA_AREA;
  }

  //��ȡECC
  //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
  DelayNus(10);
  eccHard = exmc_ecc_get(EXMC_BANK1_NAND);

  //�����ȡECC��spare����ַ
  eccAddr = NAND_PAGE_SIZE + 16;

  //���ö�ȡSpareλ��
  NandDelay(NAND_TWHR_DELAY);             //tWHR�ȴ��ӳ�
  NAND_CMD_AREA  = 0x05;                  //���������
  NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //�������ַ��λ
  NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //�������ַ��λ
  NAND_CMD_AREA  = 0xE0;                  //�������
  NandDelay(NAND_TWHR_DELAY);             //tWHR�ȴ��ӳ�
  NandDelay(NAND_TREA_DELAY);             //tREA�ȴ���ʱ

  //��Spare��ָ��λ�ö���֮ǰд���ECC
  spare[0] = NAND_DATA_AREA;
  spare[1] = NAND_DATA_AREA;
  spare[2] = NAND_DATA_AREA;
  spare[3] = NAND_DATA_AREA;
  eccFlash = ((u32)spare[3] << 24) | ((u32)spare[2] << 16) | ((u32)spare[1] << 8) | ((u32)spare[0] << 0);

  //У�鲢�����޸�����
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

  //��ȡ�ɹ�
  return NAND_OK;
}

/*********************************************************************************************************
* �������ƣ�NandWriteSpare
* �������ܣ�дSpare��
* ���������block ������
*          page  ������ҳ���
*          column��Spare�����е�ַ
*          buf   ����������ַ
*          len   ��������
* ���������void
* �� �� ֵ��0-�ɹ���1-д��ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandWriteSpare(u32 block, u32 page, u32 column, u8* buf, u32 len)
{
  u32 i;     //ѭ������
  u8  check; //�ɹ�д���־λ

  //����������
  if((column >= NAND_PAGE_SIZE) && (len > NAND_SPARE_AREA_SIZE)){return NAND_FAIL;}

  //����д���ַ
  column = column + NAND_PAGE_SIZE;              //�����е�ַƫ����
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //����д����
  NAND_ADDR_AREA = (column >> 0) & 0xFF;         //�е�ַ��λ
  NAND_ADDR_AREA = (column >> 8) & 0xFF;         //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NandDelay(NAND_TADL_DELAY);                    //tADL�ȴ��ӳ�

  //ѭ��д��spare��
  for(i = 0; i < len; i++)
  {
    NAND_DATA_AREA = buf[i];
  }

  //���ͽ�������
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB�ȴ��ӳ�
  NandDelay(NAND_TWB_DELAY);

  //tPROG�ȴ��ӳ�
  DelayNus(NAND_TPROG_DELAY);

  //�������״̬
  if(NAND_READY == NandWaitReady())
  {
    //У��д���Ƿ�ɹ�
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
* �������ƣ�NandReadSpare
* �������ܣ���Spare��
* ���������block ������
*          page  ������ҳ���
*          column��Spare�����е�ַ
*          buf   ����������ַ
*          len   ��������
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandReadSpare(u32 block, u32 page, u32 column, u8* buf, u32 len)
{
  u32 i;

  //����������
  if((column >= NAND_PAGE_SIZE) && (len > NAND_SPARE_AREA_SIZE)){return NAND_FAIL;}

  //���ö�ȡ��ַ
  column = column + NAND_PAGE_SIZE;              //�����е�ַƫ����
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //���Ͷ�����
  NAND_ADDR_AREA = (column >> 0) & 0xFF;         //�е�ַ��λ
  NAND_ADDR_AREA = (column >> 8) & 0xFF;         //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //���������
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //�ȴ�RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //�ȴ�RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR��ʱ�ȴ�

  //��ȡ����
  for(i = 0; i < len; i++)
  {
    buf[i] = NAND_DATA_AREA;
  }

  return NAND_OK;
}

/*********************************************************************************************************
* �������ƣ�NandEraseBlock
* �������ܣ������
* ���������blockNum������
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��������ʱ���͵���ҳ��ַ
*          2������ķ��ࣺ��������FBB��ʹ�û���GBB
*          3��FBB��Factory Bad Block���������飬�ճ���ʱ���еĻ��飬�ֱ���ΪMasked bad block��
*             initial bad/invalid block��NAND Flash����ʱ����Ϊ���칤�յĹ�ϵ�������һ�����Ļ��顣
*             ͬʱ������ǰԭ�����NAND FLASH���в��ԣ��������õĿ�Ҳ�п��ܱ�ԭ�����Ϊ���顣
*             �ڳ���֮ǰ��ԭ���ͻ����Ի�����б�ǣ������ǵĵط��ǣ��������ڳ�����ҳ��СΪ2K��NAND Flash��
*             ����NAND flash, ������һ��λ��ÿ��block�ĵ�һ��pageҳspare���ĵ�һ���ֽڡ�
*             ������Բ�ѯ��ͬproduct��datasheet���������0xFF����˵���ǻ��顣���Ӧ���ǣ����������Ŀ飬�õĿ飬
*             �����������ݶ���0xFF�ġ�
*          4��GBB��Grown Bad Block��ʹ�û��顣��NAND Flashʹ�ù����У���ΪNAND Flash�Ĳ�д��������
*            ��һ�㲻����10��Σ�����ʹ�õ�һ��ʱ�޺�Ҳ��������顣�������Block Erase����Page Program����
*             ���Խ����������Ϊ���顣Ϊ�˺͹��л�����Ϣ����һ�£�spare area�ĵ�1��byte���ֽڣ�Ҳ�ᱻ���Ϊ��0xFF������ֵ��
*          5��ʵ��������Ҳ���ԶԱ�ǻ�����в���������ǿ�в����󣬻�����Ϣ�Ͳ��������ˡ����ڳ������飬
*             һ���ǲ����齫��Ǻõ���Ϣ�������ġ�NAND FLASH��������ʱ����һ���ȽϿ���¶Ⱥ͵�ѹ��Χ�ڲ��Բ������nand���飬
*             ��Щ������һ�����¶Ȼ��ѹ����Ȼ���Թ���������������һʱ�����������ı����ʧЧ����Ϊһ��Ǳ�ڵĶ�ʱը����
*             �����ԭ����ǵĻ�����в�������д��ͱ�������ʱ�������������ݶ�ʧ�����⡣���Զ����Ѿ���ǻ���Ŀ飬������
*********************************************************************************************************/
u32 NandEraseBlock(u32 blockNum)
{
  u8 backBlockFlag; //�����־
  u8 check;         //�����ɹ�У��

  if(NAND_OK == NandReadSpare(blockNum, 0, 0, (u8*)&backBlockFlag, 1))
  {
    //�ÿ죬���Բ���
    if(0xFF == backBlockFlag)
    {
      //�����ַת��Ϊҳ��ַ
      blockNum <<= PAGE_BIT;

      //���Ͳ�������
      NAND_CMD_AREA = NAND_CMD_ERASE_1ST;      //���Զ�������������
      NAND_ADDR_AREA = (blockNum >> 0) & 0xFF; //ҳ��ַ��λ
      NAND_ADDR_AREA = (blockNum >> 8) & 0xFF; //ҳ��ַ��λ
      NAND_CMD_AREA = NAND_CMD_ERASE_2ND;      //��������
      NandDelay(NAND_TWB_DELAY);               //tWB�ȴ��ӳ�

      //�ȴ���������
      if(NAND_READY == NandWaitReady())
      {
        check = NAND_DATA_AREA;
        if(check & 0x01)
        {
          //����ʧ��
          return NAND_FAIL;
        }
        else
        {
          //�����ɹ�
          return NAND_OK;
        }
      }
      else
      {
        return NAND_FAIL;
      }
    }

    //���飬������
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
* �������ƣ�NandEraseChip
* �������ܣ�����ȫƬ
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandEraseChip(void)
{
  u32 i;

  //ѭ����������Block
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
* �������ƣ�NandReset
* �������ܣ���λ
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandReset(void)
{
  //���͸�λ����
  NAND_CMD_AREA = NAND_CMD_RESET;

  //�ȴ���λ���
  if(NAND_READY == NandWaitReady())
  {
    return NAND_OK;
  }
  return NAND_FAIL;
}

/*********************************************************************************************************
* �������ƣ�NandECCGetOE
* �������ܣ���ȡECC������λ/ż��λ
* ���������oe:0,ż��λ
*             1,����λ
*          eccval:�����eccֵ
* ���������void
* �� �� ֵ��������eccֵ
* �������ڣ�2018��01��01��
* ע    �⣺
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
* �������ƣ�NandECCCorrection
* �������ܣ�ECCУ������
* ���������data�� ���ݻ�����
*          eccrd����ȡ������ԭ�������ECCֵ
*          ecccl����ȡ����ʱ��Ӳ�������ECCֵ
* ���������void
* �� �� ֵ��0-����������������-ECC����(�д���2��bit�Ĵ���,�޷��ָ�)
* �������ڣ�2018��01��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandECCCorrection(u8* data, u32 eccrd, u32 ecccl)
{
  u16 eccrdo,eccrde,eccclo,ecccle;
  u16 eccchk = 0;
  u16 errorpos = 0;
  u32 bytepos = 0;

  eccrdo = NandECCGetOE(1, eccrd); //��ȡeccrd������λ
  eccrde = NandECCGetOE(0, eccrd); //��ȡeccrd��ż��λ
  eccclo = NandECCGetOE(1, ecccl); //��ȡecccl������λ
  ecccle = NandECCGetOE(0, ecccl); //��ȡecccl��ż��λ
  eccchk = eccrdo ^ eccrde ^ eccclo ^ ecccle;

  //ȫ1,˵��ֻ��1bit ECC����
  if(eccchk == 0xFFF) 
  {
    errorpos = eccrdo ^ eccclo; 
    printf("NandECCCorrection: errorpos:%d\r\n", errorpos); 
    bytepos = errorpos / 8; 
    data[bytepos] ^= 1 << (errorpos % 8);
  }

  //����ȫ1,˵��������2bit ECC����,�޷��޸�
  else
  {
    printf("NandECCCorrection: 2bit ecc error or more\r\n");
    return 1;
  } 
  return 0;
}

/*********************************************************************************************************
* �������ƣ�NandCopyPageWithoutWrite
* �������ܣ���һҳ���ݿ�������һҳ,��д��������
* ���������sourceBlock��Դ���ַ
*          sourcePageNum��Դҳ��ַ
*          destBlock��Ŀ�Ŀ��ַ
*          destPageNum��Ŀ��ҳ��ַ
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandCopyPageWithoutWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage)
{
  u8 check; //�ɹ�������־λ

  //�ж�Դ���Ŀ�Ŀ���ż�Ƿ���ͬ
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //����Դҳ��ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD0;                       //��������Դҳ��ַ����
  NAND_ADDR_AREA = 0;                                        //�е�ַ��λ
  NAND_ADDR_AREA = 0;                                        //�е�ַ��λ
  NAND_ADDR_AREA = (sourceBlock << 6) | (sourcePage & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (sourceBlock >> 2) & 0xFF;                //ʣ����ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD1;                       //����Դҳ��ַ�������
  //if(NANDWaitRB(0)) {return NAND_FAIL;}                      //�ȴ�RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}                      //�ȴ�RB = 1

  //����Ŀ�ĵ�ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD2;                   //��������Ŀ��ҳ��ַ����
  NAND_ADDR_AREA = 0;                                    //�е�ַ��λ
  NAND_ADDR_AREA = 0;                                    //�е�ַ��λ
  NAND_ADDR_AREA = (destBlock << 6) | (destPage & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (destBlock >> 2) & 0xFF;              //ʣ����ַ
  NandDelay(NAND_TADL_DELAY);                            //tADL�ȴ��ӳ�
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD3;                   //����Ŀ��ҳ��ַ�������
  NandDelay(NAND_TWB_DELAY);                             //tWB�ȴ��ӳ�
  DelayNus(NAND_TPROG_DELAY);                            //tPROG�ȴ��ӳ�

  //У��
  if(NAND_READY == NandWaitReady())
  {
    //У��д���Ƿ�ɹ�
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
* �������ƣ�NandCopyPageWithWrite
* �������ܣ���һҳ���ݿ�������һҳ,��д��������
* ���������sourceBlock��Դ���ַ
*          sourcePageNum��Դҳ��ַ
*          destBlock��Ŀ�Ŀ��ַ
*          destPageNum��Ŀ��ҳ��ַ
*          buf�����ݻ�����
*          column��ҳ���е�ַ
*          len��д�볤��
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1������ʱ���ŵ���ż��Ҫ��ͬ
*          2��������ҳ��ŵ���ż��ҲҪ��ͬ
*          3��д��Main��ʱֻ�ܰ�ҳд��
*********************************************************************************************************/
u32 NandCopyPageWithWrite(u32 sourceBlock, u32 sourcePage, u32 destBlock, u32 destPage, u8* buf, u32 column, u32 len)
{
  u32 i;       //ѭ������
  u32 ecc;     //Ӳ��ECCֵ
  u32 eccAddr; //ECC�洢��ַ
  u8  check;   //�ɹ�������־λ

  //д��main����ַУ�飬ҳ���е�ַ����0�򷵻�ʧ��
  if((column < NAND_PAGE_SIZE) && (0 != column)){return NAND_FAIL;}

  //д��main��������У�飬д������������1ҳ�����򷵻�ʧ��
  if((column < NAND_PAGE_SIZE) && (NAND_PAGE_SIZE != len)){return NAND_FAIL;}

  //д��Spare��������������
  if((column >= NAND_PAGE_SIZE) && (column + len > NAND_PAGE_TOTAL_SIZE)){return NAND_FAIL;}

  //�ж�Դ���Ŀ�Ŀ���ż�Ƿ���ͬ���Լ�Դҳ��Ŀ��ҳ����ż�Ƿ���ͬ
  if(((sourceBlock % 2) != (destBlock % 2)) || ((sourcePage % 2) != (destPage % 2)))
  {
    return NAND_FAIL;
  }

  //����Դҳ��ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD0;                       //��������Դҳ��ַ����
  NAND_ADDR_AREA = 0;                                        //�е�ַ��λ
  NAND_ADDR_AREA = 0;                                        //�е�ַ��λ
  NAND_ADDR_AREA = (sourceBlock << 6) | (sourcePage & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (sourceBlock >> 2) & 0xFF;                //ʣ����ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD1;                       //����Դҳ��ַ�������
  //if(NANDWaitRB(0)) {return NAND_FAIL;}                      //�ȴ�RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}                      //�ȴ�RB = 1

  //����Ŀ�ĵ�ַ
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD2;                   //��������Ŀ��ҳ��ַ����
  NAND_ADDR_AREA = (column >> 0) & 0xFF;                 //�е�ַ��λ
  NAND_ADDR_AREA = (column >> 8) & 0xFF;                 //�е�ַ��λ
  NAND_ADDR_AREA = (destBlock << 6) | (destPage & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (destBlock >> 2) & 0xFF;              //ʣ����ַ
  NandDelay(NAND_TADL_DELAY);                            //tADL�ȴ��ӳ�

  //д��Spare��������Ҫ����ECC
  if(column >= NAND_PAGE_SIZE)
  {
    for(i = 0; i < len; i++)
    {
      //д������
      NAND_DATA_AREA = buf[i];
    }
  }

  //д��Main������Ҫ����ECC
  else
  {
    //��λECC
    exmc_nand_ecc_config(EXMC_BANK1_NAND, DISABLE);
    exmc_nand_ecc_config(EXMC_BANK1_NAND, ENABLE);

    //д�����ݣ���ҳд
    for(i = 0; i < NAND_PAGE_SIZE; i++)
    {
      //д������
      NAND_DATA_AREA = buf[i];
    }

    //��ȡECC
    //while(RESET == exmc_flag_get(EXMC_BANK1_NAND, EXMC_NAND_PCCARD_FLAG_FIFOE));
    DelayNus(10);
    ecc = exmc_ecc_get(EXMC_BANK1_NAND);

    //����д��ECC��spare����ַ
    eccAddr = NAND_PAGE_SIZE + 16;

    //����д��Spareλ��
    NandDelay(NAND_TADL_DELAY);             //tADL�ȴ��ӳ�
    NAND_CMD_AREA  = 0x85;                  //���д����
    NAND_ADDR_AREA = (eccAddr >> 0) & 0xFF; //���д��ַ��λ
    NAND_ADDR_AREA = (eccAddr >> 8) & 0xFF; //���д��ַ��λ
    NandDelay(NAND_TADL_DELAY);             //tADL�ȴ��ӳ�

    //д��ECC
    NAND_DATA_AREA = (ecc >> 0)  & 0xFF;
    NAND_DATA_AREA = (ecc >> 8)  & 0xFF;
    NAND_DATA_AREA = (ecc >> 16) & 0xFF;
    NAND_DATA_AREA = (ecc >> 24) & 0xFF;
  }

  //д�����
  NAND_CMD_AREA  = NAND_MOVEDATA_CMD3;

  //tWB�ȴ��ӳ�
  NandDelay(NAND_TWB_DELAY);

  //tPROG�ȴ��ӳ�
  DelayNus(NAND_TPROG_DELAY);

  //�ȴ���������
  if(NAND_READY == NandWaitReady())
  {
    //У��д���Ƿ�ɹ�
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
* �������ƣ�NandCopyBlockWithoutWrite
* �������ܣ��鿽������д������
* ���������sourceBlock��Դ���ַ
*          destBlock��Ŀ�Ŀ��ַ
*          startPage����ʼ������ҳ���
*          pageNum��������ҳ��
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺����ʱ���ŵ���ż��Ҫ��ͬ
*********************************************************************************************************/
u32 NandCopyBlockWithoutWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 pageNum)
{
  u32 i; //ѭ������

  //У����ŵ���ż��
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //У��ҳ��
  if((startPage + pageNum) > NAND_BLOCK_SIZE)
  {
    return NAND_FAIL;
  }

  //ѭ����������ҳ
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
* �������ƣ�NandCopyBlockWithWrite
* �������ܣ��鿽������д��������
* ���������sourceBlock��Դ���ַ
*          destBlock��Ŀ�Ŀ��ַ
*          startPage����ʼд��ҳ���
*          column��ҳ���е�ַ
*          buf�����ݻ�����
*          len�����ݳ���
* ���������void
* �� �� ֵ��0-�ɹ���1-��ȡʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1������ʱ���ŵ���ż��Ҫ��ͬ
*          2��д���е�ַҪ512�ֽڶ���
*          3��д����Ҫ512�ֽڶ���
*          4�����ܽ�����д�뵽Spare��
*********************************************************************************************************/
u32 NandCopyBlockWithWrite(u32 sourceBlock, u32 destBlock, u32 startPage, u32 column, u8* buf, u32 len)
{
  u32 ret;           //����ֵ
  u32 availableSize; //���ڿ����ڴ��С
  u32 copyPage;      //��ǰ������ҳ���
  u32 copyColumn;    //ҳ�ڿ����б��
  u32 copyLen;       //ҳ�ڿ�������
  u8* copyBuf;       //ҳ�ڿ������ݻ�����

  //У��Դ����Ŀ�Ŀ���ż��
  if((sourceBlock % 2) != (destBlock % 2))
  {
    return NAND_FAIL;
  }

  //У���е�ַ��д����
  if((column >= NAND_PAGE_SIZE) || (0 != column % 512) || (0 != len % 512))
  {
    return NAND_FAIL;
  }

  //У��д�����Ƿ����
  availableSize = NAND_BLOCK_SIZE * NAND_PAGE_SIZE - NAND_PAGE_SIZE * startPage - column;
  if(len > availableSize)
  {
    return NAND_FAIL;
  }

  //�ȸ���ǰ�߲���Ҫд���ҳ
  if(0 != startPage)
  {
    ret = NandCopyBlockWithoutWrite(sourceBlock, destBlock, 0, startPage);
    if(NAND_OK != ret)
    {
      return NAND_FAIL;
    }
  }

  //������Ҫд���ҳ
  copyBuf    = buf;
  copyPage   = startPage;
  copyColumn = column;
  copyLen    = NAND_PAGE_SIZE - column;
  if(copyLen > len){copyLen = len;}
  while(0 != copyLen)
  {
    //ҳ����
    ret = NandCopyPageWithWrite(sourceBlock, copyPage, destBlock, copyPage, copyBuf, copyColumn, copyLen);
    if(NAND_OK != ret)
    {
      return NAND_FAIL;
    }

    //ͳ��ʣ��������
    len = len - copyLen;

    //ʣ��������Ϊ0����������
    if(0 == len)
    {
      copyLen = 0;
    }

    //ʣ������������ڵ���1ҳ����
    else if(len >= NAND_PAGE_SIZE)
    {
      copyLen    = NAND_PAGE_SIZE; //���ÿ���һ��ҳ
      copyPage   = copyPage + 1;   //������һҳ
      copyColumn = 0;              //ҳ�ڿ����б������
    }

    //ʣ������������һҳ����
    else if(len < NAND_PAGE_SIZE)
    {
      copyLen    = len;          //����ʣ������
      copyPage   = copyPage + 1; //������һҳ
      copyColumn = 0;            //ҳ�ڿ����б������
    }
  }

  //����ʣ���ҳ
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
* �������ƣ�NandCheckPage
* �������ܣ�ҳУ�飬��һ��ҳ������ĳһ��ֵ�Ƚϣ������ڼ��ҳ�Ƿ�д���
* ���������block�����ţ�page��ҳ��ţ�value���Ƚ�ֵ��mode��0-ֻ���Main��������-���Main����Sapre��
* ���������void
* �� �� ֵ��0-��ǰҳ��δд�룬1-��ǰҳ��д���
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandCheckPage(u32 block, u32 page, u32 value, u32 mode)
{
  u32 i;    //ѭ������
  u8  data; //��ʱ����
  u32 num;  //�������

  //���ö�ȡ��ַ
  NAND_CMD_AREA  = NAND_CMD_READ1_1ST;           //���Ͷ�����
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NAND_CMD_AREA  = NAND_CMD_READ1_2ND;           //���������
  //if(NANDWaitRB(0)) {return NAND_FAIL;}          //�ȴ�RB = 0
  if(NANDWaitRB(1)) {return NAND_FAIL;}          //�ȴ�RB = 1
  NandDelay(NAND_TRR_DELAY);                     //tRR��ʱ�ȴ�

  //���ü������
  if(0 == mode)
  {
    //ֻ���Main��
    num = NAND_PAGE_SIZE;
  }
  else if(1 == mode)
  {
    //���Main����Sapre��
    num = NAND_PAGE_TOTAL_SIZE;
  }

  //У��Main����Spare����У��
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
* �������ƣ�NandPageFillValue
* �������ܣ�ҳ��䣬����ҳ���ĳһ��ֵ
* ���������block�����ţ�page��ҳ��ţ�value�����ֵ��mode��0-ֻ���Main��������-���Main����Sapre��
* ���������void
* �� �� ֵ��0-��ǰҳ��δд�룬1-��ǰҳ��д���
* �������ڣ�2021��07��01��
* ע    �⣺
*********************************************************************************************************/
u32 NandPageFillValue(u32 block, u32 page, u32 value, u32 mode)
{
  u32 i;     //ѭ������
  u8  check; //�ɹ�д���־λ
  u32 num;   //�������

  //����д���ַ
  NAND_CMD_AREA  = NAND_CMD_WRITE_1ST;           //����д����
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = 0;                            //�е�ַ��λ
  NAND_ADDR_AREA = (block << 6) | (page & 0x3F); //���ַ���е�ַ
  NAND_ADDR_AREA = (block >> 2) & 0xFF;          //ʣ����ַ
  NandDelay(NAND_TADL_DELAY);                    //tADL�ȴ��ӳ�

  //�����������
  if(0 == mode)
  {
    //ֻ���Main��
    num = NAND_PAGE_SIZE;
  }
  else if(1 == mode)
  {
    //���Main����Sapre��
    num = NAND_PAGE_TOTAL_SIZE;
  }

  //ѭ��д��
  for(i = 0; i < num; i++)
  {
    NAND_DATA_AREA = value;
  }

  //���ͽ�������
  NAND_CMD_AREA = NAND_CMD_WRITE_2ND;

  //tWB�ȴ��ӳ�
  NandDelay(NAND_TWB_DELAY);

  //tPROG�ȴ��ӳ�
  DelayNus(NAND_TPROG_DELAY);

  //�������״̬
  if(NAND_READY == NandWaitReady())
  {
    //У��д���Ƿ�ɹ�
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
* �������ƣ�NandForceEraseBlock
* �������ܣ�ǿ�ƿ����
* ���������blockNum������
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺�˺����Ὣ�ٷ�ԭ���Ļ��������������ʹ��
*********************************************************************************************************/
u32 NandForceEraseBlock(u32 blockNum)
{
  u8 check;

  //�����ַת��Ϊҳ��ַ
  blockNum <<= PAGE_BIT;

  //���Ͳ�������
  NAND_CMD_AREA = NAND_CMD_ERASE_1ST;      //���Զ�������������
  NAND_ADDR_AREA = (blockNum >> 0) & 0xFF; //ҳ��ַ��λ
  NAND_ADDR_AREA = (blockNum >> 8) & 0xFF; //ҳ��ַ��λ
  NAND_CMD_AREA = NAND_CMD_ERASE_2ND;      //��������
  NandDelay(NAND_TWB_DELAY);               //tWB�ȴ��ӳ�

  //�ȴ���������
  if(NAND_READY == NandWaitReady())
  {
    check = NAND_DATA_AREA;
    if(check & 0x01)
    {
      //����ʧ��
      return NAND_FAIL;
    }
    else
    {
      //�����ɹ�
      return NAND_OK;
    }
  }
  else
  {
    return NAND_FAIL;
  }
}

/*********************************************************************************************************
* �������ƣ�NandForceEraseChip
* �������ܣ�����ȫƬ
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺�˺����Ὣ�ٷ�ԭ���Ļ��������������ʹ��
*********************************************************************************************************/
u32 NandForceEraseChip(void)
{
  u32 i;

  //ѭ����������Block
  for(i = 0; i < NAND_BLOCK_COUNT; i++)
  {
    NandEraseBlock(i);
  }
  return NAND_OK;
}

/*********************************************************************************************************
* �������ƣ�NandMarkAllBadBlock
* �������ܣ�ɨ�貢������л��飬����ǿ�Ʋ���������ǻ���������е�Block
* ���������void
* ���������void
* �� �� ֵ��0-�ɹ���1-ʧ��
* �������ڣ�2021��07��01��
* ע    �⣺
*          1��ʹ�ò�-д-����ʽ����Ϊ�ٷ�ԭ�����ڱȽϿ���¶ȷ�Χ�²��ԣ����Դ˺�����һ�����ҳ����еĻ���
*          2���˺����Ὣ�ٷ�ԭ���Ļ��������������ʹ��
*          3���˺���ִ��ʱ���Ƚϳ��������ĵȴ�
*********************************************************************************************************/
u32 NandMarkAllBadBlock(void)
{
  u32 i, j;         //ѭ������
  u32 ret;          //����ֵ
  u32 badBlockFlag; //�����־λ
  u8  mark;         //������

  //�趨���ֵ������0xFF����
  mark = 0xCC;

  //ѭ��������е�Block
  for(i = 0; i < NAND_BLOCK_COUNT; i++)
  {
    //ǿ�Ʋ���Block
    ret = NandForceEraseBlock(i);

    //����ʧ�ܣ���ʾ����һ������
    if(NAND_OK != ret)
    {
      printf("NandMarkAllBadBlock: bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //ȫ1���
    badBlockFlag = 0;
    for(j = 0; j < NAND_BLOCK_SIZE; j++)
    {
      //У��Main����Spare���Ƿ���1
      ret = NandCheckPage(i, j, 0xFF, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }
    }
    
    //У�鲻�ϸ񣬱�ǻ���
    if(0 != badBlockFlag)
    {
      printf("NandMarkAllBadBlock: fail to check 1, bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //ȫ0���
    badBlockFlag = 0;
    for(j = 0; j < NAND_BLOCK_SIZE; j++)
    {
      //����д��0������Main����Spare��
      ret = NandPageFillValue(i, j, 0x00, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }

      //У��Main����Spare���Ƿ���0
      ret = NandCheckPage(i, j, 0x00, 1);
      if(NAND_OK != ret)
      {
        badBlockFlag = 1;
        break;
      }
    }

    //У�鲻�ϸ񣬱�ǻ���
    if(0 != badBlockFlag)
    {
      printf("NandMarkAllBadBlock: fail to check 0, bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //У��ɹ������²����ÿ�
    ret = NandForceEraseBlock(i);

    //�ڶ��β���ʧ�ܣ���ʾ����һ������
    if(NAND_OK != ret)
    {
      printf("NandMarkAllBadBlock: bad block: %d\r\n", i);
      NandWriteSpare(i, 0, 0, &mark, 1);
      continue;
    }

    //�����ɹ�������һ���ÿ�
    else
    {
      printf("NandMarkAllBadBlock: good block: %d\r\n", i);
    }
  }

  return NAND_OK;
}
