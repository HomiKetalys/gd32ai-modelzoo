/*!
    \file    dci_ov2640.c
    \brief   DCI config file

    \version 2023-06-21, V1.0.0, demo for GD32H7xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "dci_ov2640.h"
#include "dci_ov2640_init_table.h"
#include "gd32h7xx.h"
#include "systick.h"
#include "DataType.h"
#include "TLILCD.h"

void ConfigDCI();
/*!
    \brief      configure the DCI to interface with the camera module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dci_config(void)
{
	
    dci_parameter_struct dci_struct;
    dma_single_data_parameter_struct  dma_single_struct;;
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_DCI);

    /* DCI GPIO AF configuration */
    /* configure DCI_PIXCLK(PE3), DCI_VSYNC(PB7), DCI_HSYNC(PA4) */
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_3);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4);

    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_7);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4);

    /* configure  DCI_D0(PC6), DCI_D1(PH10) DCI_D2(PC8), DCI_D3(PC9), DCI_D4(PE4), DCI_D5(PB6), DCI_D6(PE5), DCI_D7(PE6) */
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_8);
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_9);
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_6);
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_4);
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_5);
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_6);
    gpio_af_set(GPIOH, GPIO_AF_13, GPIO_PIN_10);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_10);

    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);

    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_4);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_5);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, GPIO_PIN_6);
		
		ckout0_init();
    
		ConfigDCI();
}


void ConfigDCI()
{
		dci_parameter_struct dci_struct;
		dma_single_data_parameter_struct dma_single_struct;
		//使能RCC相关时钟
		rcu_periph_clock_enable(RCU_DCI);    //使能DCI时钟
		rcu_periph_clock_enable(RCU_DMA1);   //使能DMA1时钟
		//DCI配置
		dci_deinit();                                              //复位DCI
    /* DCI configuration */
//    dci_struct.capture_mode = DCI_CAPTURE_MODE_CONTINUOUS;
	  dci_struct.capture_mode = DCI_CAPTURE_MODE_SNAPSHOT;
    dci_struct.clock_polarity =  DCI_CK_POLARITY_RISING;
    dci_struct.hsync_polarity = DCI_HSYNC_POLARITY_LOW;
    dci_struct.vsync_polarity = DCI_VSYNC_POLARITY_LOW;
    dci_struct.frame_rate = DCI_FRAME_RATE_ALL;
    dci_struct.interface_format = DCI_INTERFACE_FORMAT_8BITS;
    dci_init(&dci_struct);
		nvic_irq_enable(DCI_IRQn, 2, 0);                           //使能DCI NVIC中断
		dci_interrupt_enable(DCI_INT_OVR);                         //使能DCI FIFO溢出中断（非常重要！！！）
		dci_interrupt_flag_clear(DCI_INT_OVR);                     //清除溢出中断标志位
    dci_interrupt_enable(DCI_INT_EF);                        //帧结束中断使能，用以绘制一帧图像到LCD上并触发下一次捕获
    dci_interrupt_flag_clear(DCI_INT_EF);                    //清除帧结束中断标志位
    /* DCI DMA configuration */
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_DMAMUX);
    dma_single_data_para_struct_init(&dma_single_struct);
    dma_deinit(DMA1, DMA_CH7);
    dma_single_struct.request = DMA_REQUEST_DCI;
    dma_single_struct.periph_addr = (uint32_t)DCI_DATA_ADDRESS;
    dma_single_struct.memory0_addr = (uint32_t)0xC0000000;
    dma_single_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_struct.number = IMAGE_HEIGHT*IMAGE_WIDTH/2;
    dma_single_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_struct.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
//    dma_single_struct.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_single_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_struct.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH7, &dma_single_struct);
//		dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI1);       //绑定DMA通道与相应外设
//		dma_flow_controller_config(DMA1, DMA_CH7, DMA_FLOW_CONTROLLER_DMA);  //DMA作为传输控制器
		dma_channel_enable(DMA1, DMA_CH7);                                   //使能DMA传输
		dci_enable();         //使能DCI
		dci_capture_enable(); //开启传输
}



/*!
    \brief      DCI camera outsize set
    \param[in]  width: outsize width
    \param[in]  height: outsize height
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t ov2640_outsize_set(uint16_t width, uint16_t height)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp;
    if(width % 4) {
        return 0xFF;
    }
    if(height % 4) {
        return 0xFF;
    }
    outw = width / 4;
    outh = height / 4;
    dci_byte_write(0xFF, 0x00);
    dci_byte_write(0xE0, 0x04);
    dci_byte_write(0x5A, outw & 0xFF);
    dci_byte_write(0x5B, outh & 0xFF);
    temp = (outw >> 8) & 0x03;
    temp |= (outh >> 6) & 0x04;
    dci_byte_write(0x5C, temp);
    dci_byte_write(0xE0, 0x00);
    return 0;
}

/*!
    \brief      DCI camera initialization
    \param[in]  none
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t dci_ov2640_init(void)
{
    uint8_t i;
    sccb_config();
    
    dci_config();
	
    delay_1ms(100);
    /* OV2640 reset */
    if(dci_byte_write(0xFF, 0x01) != 0) {
        return 0xFF;
    }
    if(dci_byte_write(0x12, 0x80) != 0) {
        return 0xFF;
    }
    delay_1ms(10);
    for(i = 0; i < sizeof(ov2640_svga_init_reg_tbl) / 2; i++) {
        if(0 != dci_byte_write(ov2640_svga_init_reg_tbl[i][0], ov2640_svga_init_reg_tbl[i][1])) {
            return 0xFF;
        }
    }

    delay_1ms(100);
    for(i = 0; i < (sizeof(ov2640_rgb565_reg_tbl) / 2); i++) {
        if(0 != dci_byte_write(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1])) {
            return 0xFF;
        }
    }
    delay_1ms(100);
    ov2640_outsize_set(IMAGE_WIDTH, IMAGE_HEIGHT);
		
    return 0;
}

/*!
    \brief      ckout0 initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ckout0_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_af_set(GPIOA, GPIO_AF_CKOUT, GPIO_PIN_8);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, GPIO_PIN_8);

    rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV2);
}

/*!
    \brief      read the ov2640 manufacturer identifier
    \param[in]  ov2640id: pointer to the ov2640 manufacturer struct
    \param[out] none
    \retval     0x00 or 0xFF
*/
uint8_t dci_ov2640_id_read(ov2640_id_struct *ov2640id)
{
    uint8_t temp;
    dci_byte_write(0xFF, 0x01);
    if(dci_byte_read(OV2640_MIDH, &temp) != 0) {
        return 0xFF;
    }
    ov2640id->manufacturer_id1 = temp;
    if(dci_byte_read(OV2640_MIDL, &temp) != 0) {
        return 0xFF;
    }
    ov2640id->manufacturer_id2 = temp;
    if(dci_byte_read(OV2640_VER, &temp) != 0) {
        return 0xFF;
    }
    ov2640id->version = temp;
    if(dci_byte_read(OV2640_PID, &temp) != 0) {
        return 0xFF;
    }
    ov2640id->pid = temp;

    return 0x00;
}



u32 s_iGetFrameFlag;                   //获得1帧图像标志位

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
		if(s_iGetFrameFlag==0)
		{
			ConfigDCI();
		}
  }
}

void DCIShowImage(void)
{
  u32 x, y;
  u16* frame;

  if((1 == s_iGetFrameFlag))
  {
    //将一帧图像显示到屏幕上
    frame = (u16*)(0xC0000000);
    for(y = 0; y < IMAGE_WIDTH; y++)
    {
      for(x = 0; x < IMAGE_HEIGHT; x++)
      {
        LCDDrawPoint(x, y, frame[IMAGE_WIDTH*IMAGE_HEIGHT-1-(IMAGE_WIDTH * y + x)]);
      }
    }

    //开启下一帧传输
    s_iGetFrameFlag = 0;
    ConfigDCI();
  }
}
