/*!
    \file    main.c
    \brief   DCI display demo

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
#include "gd32h7xx.h"
#include "systick.h"
#include <stdio.h>
#include "gd32h759i_eval.h"
#include "exmc_sdram.h"
#include "picture.h"
#include "dci_ov2640.h"
#include "main.h"
#include "TLILCD.h"
#include "ai_model.h"
void cache_enable(void);
void timer_config();
extern u32 s_iGetFrameFlag;

bool time_start=false;
u32 time=0;
tli_parameter_struct               tli_initstruct;
tli_layer_parameter_struct         tli_layer0_initstruct;
tli_layer_parameter_struct         tli_layer1_initstruct;


#define LCD_X 272
#define LCD_Y 480
#define SHOW_CX (LCD_X/2-1)
#define SHOW_CY (LCD_Y-LCD_X/2-1)
#define SHOW_X0 (SHOW_CX-(INPUT_WIDTH-1)/2)
#define SHOW_Y0 (SHOW_CY-(INPUT_HEIGHT-1)/2)
#define SHOW_X1 (SHOW_CX+(INPUT_WIDTH-1)/2)
#define SHOW_Y1 (SHOW_CY+(INPUT_HEIGHT-1)/2)
#define time_start() time_start=1
#define get_time_stamp() time
#define get_frame_flag() s_iGetFrameFlag
#define clear_frame_flag() s_iGetFrameFlag=0
#define frame_addr 0xC0000000

extern ObjectResult results[];
extern u32 object_num;

const char* activities[80] = {"person","bicycle","car","motorbike","aeroplane","bus","train","truck","boat","traffic light","fire hydrant","stop sign",
    "parking meter","bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase",
    "frisbee","skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle","wine glass","cup",
    "fork","knife","spoon","bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair","sofa","pottedplant",
    "bed","diningtable","toilet","tvmonitor","laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink","refrigerator","book",
    "clock","vase","scissors","teddy bear","hair drier","toothbrush"
};

//检测一次
void detect_once(void)
{
    while(!((1 == get_frame_flag())));
    u32 i,x, y,t0,t1,t2;
    char str[128];
    u16 *frame=(u16 *) frame_addr;
    
    time_start();
    
    t0=get_time_stamp();
    AI_Run((u8 *)frame);
    t1=get_time_stamp();
    t0=t1-t0;


    //将结果显示在屏幕
    t1=get_time_stamp();
    for(y = 24; y <= LCD_Y-1; y++)
    {
        for(x = 0; x <= LCD_X-1; x++)
        {
            if((x>=SHOW_X0)&&(x<=SHOW_X1)&&(y>=SHOW_Y0)&&(y<=SHOW_Y1))
                LCDDrawPoint(x, y, frame[IMAGE_HEIGHT*IMAGE_WIDTH-1-(y-SHOW_Y0)*IMAGE_WIDTH-(x-SHOW_X0)]);
            else
                LCDDrawPoint(x, y, 0xffff);
        }
    }
#ifndef TEST_TIME
    for(i=0;i<object_num;i++)
    {
        LCDShowString(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *) activities[results[i].cls_index]);
        LCDDrawRectangle(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min,SHOW_X0+(u32)results[i].bbox.x_max,SHOW_Y0+(u32)results[i].bbox.y_max,0xfff0);
    }
#endif
    t2=get_time_stamp();
    t1=t2-t1;
    
    u32 sys=rcu_clock_freq_get(CK_SYS)/1000000;
    u32 ahb=rcu_clock_freq_get(CK_AHB)/1000000;
    u32 apb1=rcu_clock_freq_get(CK_APB1)/1000000;
    u32 apb2=rcu_clock_freq_get(CK_APB2)/1000000;

 
    sprintf(str,"infer_time:%dms",t0);
    LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    sprintf(str,"display_time:%dms",t1);
    LCDShowString(0,24*2,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    sprintf(str,"SYS:%dMHz",sys);
    LCDShowString(0,24*3,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    sprintf(str,"AHB:%dMHz",ahb);
    LCDShowString(0,24*4,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    sprintf(str,"APB1:%dMHz",apb1);
    LCDShowString(0,24*5,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    sprintf(str,"APB2:%dMHz",apb2);
    LCDShowString(0,24*6,24*10-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    
    //开启下一帧传输
    clear_frame_flag();
    ConfigDCI();
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    ov2640_id_struct ov2640id;

    /* enable the CPU cache */
    cache_enable();
    
    systick_config();

    /* SDRAM initialization */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    delay_1ms(1000);

    /* camera initialization */
    dci_ov2640_init();
    dci_ov2640_id_read(&ov2640id);


    /* LCD configure and TLI enable */
    InitLCD();
    LCDDisplayDir(LCD_SCREEN_VERTICAL);
    /* Timer Init */
    timer_config();
    /* AI Init */
    AI_Init(IMAGE_WIDTH,IMAGE_HEIGHT,0);
    LCDClear(0xffff);
    LCDShowString(0,0,LCD_X-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");

    void detect_once();
    while(1)
    {
        detect_once();
    }
}

/*!
    \brief      enable the CPU cache
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cache_enable(void)
{
    /* enable I-Cache and D-Cache */
    SCB_EnableICache();
    SCB_EnableDCache();
//      SCB->CACR|=1<<2;
//    SCB_DisableICache();
//    SCB_DisableDCache();
}

/*!
    \brief      LCD configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_config(void)
{
    rcu_periph_clock_enable(RCU_TLI);
    tli_gpio_config();

    /* configure PLL2 to generate TLI clock 25MHz/25*216/3 = 72MHz */
    rcu_pll_input_output_clock_range_config(IDX_PLL2, RCU_PLL2RNG_1M_2M, RCU_PLL2VCO_150M_420M);
    if(ERROR == rcu_pll2_config(25, 188, 3, 3, 3)) {
        while(1) {
        }
    }
    rcu_pll_clock_output_enable(RCU_PLL2R);
    rcu_tli_clock_div_config(RCU_PLL2R_DIV8);
    rcu_osci_on(RCU_PLL2_CK);

    if(ERROR == rcu_osci_stab_wait(RCU_PLL2_CK)) {
        while(1) {
        }
    }

    /* TLI initialization */
    /* initialize the horizontal synchronization polarity as active low */
    tli_initstruct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    /* initialize the vertical synchronization polarity as active low */
    tli_initstruct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    /* initialize the data enable polarity as active low */
    tli_initstruct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    /* initialize the pixel clock polarity as input pixel clock */
    tli_initstruct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;

    /* timing configuration */
    /* configure horizontal synchronization width */
    tli_initstruct.synpsz_hpsz = 40;
    /* configure vertical synchronization height */
    tli_initstruct.synpsz_vpsz = 9;
    /* configure accumulated horizontal back porch */
    tli_initstruct.backpsz_hbpsz = 42;
    /* configure accumulated vertical back porch */
    tli_initstruct.backpsz_vbpsz = 11;
    /* configure accumulated active width */
    tli_initstruct.activesz_hasz = 522;
    /* configure accumulated active height */
    tli_initstruct.activesz_vasz = 283;
    /* configure total width */
    tli_initstruct.totalsz_htsz = 524;
    /* configure total height */
    tli_initstruct.totalsz_vtsz = 285;

    /* configure R,G,B component values for LCD background color */
    tli_initstruct.backcolor_red = 0xFF;
    tli_initstruct.backcolor_green = 0xFF;
    tli_initstruct.backcolor_blue = 0xFF;

    tli_init(&tli_initstruct);

    /* layer0 windowing configuration */
    tli_layer0_initstruct.layer_window_leftpos = 45;
    tli_layer0_initstruct.layer_window_rightpos = (43 + 480 - 1);
    tli_layer0_initstruct.layer_window_toppos =  12;
    tli_layer0_initstruct.layer_window_bottompos = (12 + 272 - 1);

    /* pixel format configuration */
    tli_layer0_initstruct.layer_ppf = LAYER_PPF_RGB565;

    /* alpha constant configuration : the constant alpha for layer 0 is decreased
    to see the layer 0 in the intersection zone*/
    tli_layer0_initstruct.layer_sa = 255;

    /* default color configuration (configure A,R,G,B component values) */
    tli_layer0_initstruct.layer_default_blue = 0xFF;
    tli_layer0_initstruct.layer_default_green = 0xFF;
    tli_layer0_initstruct.layer_default_red = 0xFF;
    tli_layer0_initstruct.layer_default_alpha = 0xFF;

    /* blending factors */
    tli_layer0_initstruct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer0_initstruct.layer_acf2 = LAYER_ACF1_PASA;

    /* configure input address : frame buffer is located at FLASH memory */
    tli_layer0_initstruct.layer_frame_bufaddr = (uint32_t)image_background0;
    tli_layer0_initstruct.layer_frame_line_length = ((480 * 2) + 3);
    tli_layer0_initstruct.layer_frame_buf_stride_offset = (480 * 2);
    tli_layer0_initstruct.layer_frame_total_line_number = 272;
    tli_layer_init(LAYER0, &tli_layer0_initstruct);

    /* layer1 windowing configuration */
    tli_layer1_initstruct.layer_window_leftpos = 162;
    tli_layer1_initstruct.layer_window_rightpos = (160 + 240 - 1);
    tli_layer1_initstruct.layer_window_toppos =  12;
    tli_layer1_initstruct.layer_window_bottompos = (12 + 272 - 1);

    /* pixel format configuration */
    tli_layer1_initstruct.layer_ppf = LAYER_PPF_RGB565;

    /* alpha constant configuration : the constant alpha for layer 1 is decreased
    to see the layer 0 in the intersection zone*/
    tli_layer1_initstruct.layer_sa = 255;

    /* default color configuration (configure A,R,G,B component values) */
    tli_layer1_initstruct.layer_default_blue = 0xFF;
    tli_layer1_initstruct.layer_default_green = 0xFF;
    tli_layer1_initstruct.layer_default_red = 0xFF;
    tli_layer1_initstruct.layer_default_alpha = 0;

    /* blending factors */
    tli_layer1_initstruct.layer_acf1 = LAYER_ACF1_PASA;
    tli_layer1_initstruct.layer_acf2 = LAYER_ACF1_PASA;

    /* configure input address : frame buffer is located at memory */
    tli_layer1_initstruct.layer_frame_bufaddr = (uint32_t)0xC1000000;

    tli_layer1_initstruct.layer_frame_line_length = ((240 * 2) + 3);
    tli_layer1_initstruct.layer_frame_buf_stride_offset = (240 * 2);

    tli_layer1_initstruct.layer_frame_total_line_number = 272;

    tli_layer_init(LAYER1, &tli_layer1_initstruct);
    tli_dither_config(TLI_DITHER_ENABLE);
}



/*!
    \brief      key configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* configure wakeup key interrupt*/
    {
        /* configure button pin as input */
        gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN0);

        /* configure key EXTI line0 */
        exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_0);

        /* enable and set key EXTI interrupt priority */
        nvic_irq_enable(EXTI0_IRQn, 1U, 1U);
    }

    /* configure tamper key interrupt*/
    {
        /* configure PC13 pin */
        gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_13);

        /* connect EXTI line13 to PC13 pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOC, EXTI_SOURCE_PIN13);

        /* configure key EXTI line13 */
        exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_13);
        /* enable and set key EXTI interrupt priority */
        nvic_irq_enable(EXTI10_15_IRQn, 1U, 2U);
    }
    /* configure user key interrupt*/
    {
        /* configure PF8 pin */
        gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);

        /* connect EXTI line8 to PF8 pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOF, EXTI_SOURCE_PIN8);

        /* configure EXTI line8 */
        exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(EXTI_8);

        nvic_irq_enable(EXTI5_9_IRQn, 1U, 0U);
    }
}

/*!
    \brief      save image to sdram
    \param[in]  none
    \param[out] none
    \retval     none
*/
void image_save(void)
{
    uint32_t i = 0;

    dma_interrupt_disable(DMA1, DMA_CH7, DMA_CHXCTL_FTFIE);
    dma_channel_disable(DMA1, DMA_CH7);
    dci_capture_disable();

    /* save image to sdram */
    for(i = 0; i < 32640; i++) {
        *(uint32_t *)(0xC0800000 + 4 * i) = *(uint32_t *)(0xC1000000 + 4 * i);
    }
}

/*!
    \brief      display image to lcd
    \param[in]  display_image_addr: image display address
    \param[out] none
    \retval     none
*/
void image_display(uint32_t display_image_addr)
{
    /* input address configuration */
    tli_layer1_initstruct.layer_frame_bufaddr = (uint32_t)display_image_addr;

    tli_layer_init(LAYER1, &tli_layer1_initstruct);
    /* enable layer0 */
    tli_layer_enable(LAYER0);
    /* enable layer1 */
    tli_layer_enable(LAYER1);
    /* reload configuration */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);

    /* enable TLI */
    tli_enable();
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    /* enable DMA1 channel 7 */
    nvic_irq_enable(DMA1_Channel7_IRQn, 0U, 1U);
}

/*!
    \brief      usart configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void usart_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(EVAL_COM_CLK);

    /* connect port to USART0 TX */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, EVAL_COM_TX_PIN);

    /* connect port to USART0 RX */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, EVAL_COM_RX_PIN);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, EVAL_COM_TX_PIN);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, EVAL_COM_TX_PIN);

    /* configure USART RX as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, EVAL_COM_RX_PIN);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_60MHZ, EVAL_COM_RX_PIN);

    /* USART configure */
    usart_deinit(EVAL_COM);
    usart_word_length_set(EVAL_COM, USART_WL_8BIT);
    usart_stop_bit_set(EVAL_COM, USART_STB_1BIT);
    usart_parity_config(EVAL_COM, USART_PM_NONE);
    usart_baudrate_set(EVAL_COM, 115200U);
    usart_receive_config(EVAL_COM, USART_RECEIVE_ENABLE);
    usart_transmit_config(EVAL_COM, USART_TRANSMIT_ENABLE);
    usart_enable(EVAL_COM);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}

void timer_config(void)
{
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = 300MHz / (299+1) = 1MHz */

    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 299;
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
//    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH0 configuration in PWM mode */
//    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
//    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
//    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
//    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
//    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
//    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
//    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

//    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 250);
//    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
//    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

//    timer_primary_output_config(TIMER0, ENABLE);
//    /* auto-reload preload enable */
//    timer_auto_reload_shadow_enable(TIMER0);
        
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);         //使能定时器的更新中断
    nvic_irq_enable(TIMER1_IRQn, 1, 0);                   //配置NVIC设置优先级
        
    timer_enable(TIMER1);
}

void TIMER1_IRQHandler(void)  
{

  if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)    //判断定时器更新中断是否发生
  {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);           //清除定时器更新中断标志 
  }

  //系统运行时间加一
  if(time_start)
        time++;
}
