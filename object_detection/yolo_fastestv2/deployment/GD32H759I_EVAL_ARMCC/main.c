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
void usart_config();
void timer1_config();


#define LCD_X 272
#define LCD_Y 480
#define SHOW_CX (LCD_X/2-1)
#define SHOW_CY (LCD_Y-LCD_X/2-1)
#define SHOW_X0 (SHOW_CX-(INPUT_WIDTH-1)/2)
#define SHOW_Y0 (SHOW_CY-(INPUT_HEIGHT-1)/2)
#define SHOW_X1 (SHOW_CX+(INPUT_WIDTH-1)/2)
#define SHOW_Y1 (SHOW_CY+(INPUT_HEIGHT-1)/2)
#define frame_addr 0xC0000000
#define get_time_stamp() timer_counter_read(TIMER1)

extern ObjectResult results[];
extern u32 object_num;
extern const char *activities[];


void detect_once(void)
{
    u32 i,x, y,t0,t1,t2;
    char str[64];
    u16 *frame=(u16 *) frame_addr;

    t0=get_time_stamp();
    AI_Run();
    t1=get_time_stamp();
    t0=t1-t0;


    //将结果显示在屏幕
    t1=get_time_stamp();
    for(y = 24; y <= LCD_Y-1; y++)
    {
        for(x = 0; x <= LCD_X-1; x++)
        {
            if((x>=SHOW_X0)&&(x<=SHOW_X1)&&(y>=SHOW_Y0)&&(y<=SHOW_Y1))
                LCDDrawPoint(x, y, frame[(y-SHOW_Y0)*IMAGE_WIDTH+(IMAGE_WIDTH-(x-SHOW_X0))]);
            else
                LCDDrawPoint(x, y, 0xffff);
        }
    }
#ifndef TEST_TIME
    for(i=0;i<object_num;i++)
    {
        LCDShowString(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min-16,480-1,16,LCD_FONT_16,LCD_TEXT_TRANS,0x07f0,0xffff,(char *)activities[results[i].cls_index]);
        LCDDrawRectangle(SHOW_X0+(u32)results[i].bbox.x_min,SHOW_Y0+(u32)results[i].bbox.y_min,SHOW_X0+(u32)results[i].bbox.x_max,SHOW_Y0+(u32)results[i].bbox.y_max,0xfff0);
    }
#endif
    t2=get_time_stamp();
    t1=t2-t1;

    sprintf(str,"infer_time:%d.%dms",t0/1000,t0%1000);
    LCDShowString(0,24*1,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
    sprintf(str,"display_time:%d.%dms",t1/1000,t1%1000);
    LCDShowString(0,24*2,24*20-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,str);
    printf(str);
    printf("\r\n");
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable the CPU cache */
    cache_enable();
    
    timer1_config();
    
    /* SDRAM initialization */
    exmc_synchronous_dynamic_ram_init(EXMC_SDRAM_DEVICE0);
    delay_1ms(1000);

    /* Usart0 Init */
    usart_config();
    /* Timer Init */
    
    
    /* camera initialization */
    dci_ov2640_init();

    /* LCD configure and TLI enable */
    InitLCD();
    LCDDisplayDir(LCD_SCREEN_VERTICAL);


    /* AI Init */
    AI_Init();
    LCDClear(0xffff);
    LCDShowString(0,0,LCD_X-1,24,LCD_FONT_24,LCD_TEXT_NORMAL,0,0xffff,"results:");

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
}

/*!
    \brief      usart configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_config(void)
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

#if defined(__CC_ARM)
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}
#elif defined(__GNUC__)
int _write (int fd, char *pBuffer, int size)  
{
    int i;
    for (i = 0; i < size; i++)  
    {
        usart_data_transmit(EVAL_COM, (uint8_t)(*(pBuffer+i)));
        while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    }  
    return i;  
}
#else
#warning "Unsupported compiler, printf will be disabled"
#endif

void timer1_config(void)
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
    timer_initpara.period            = 0xffffffff;
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

//    timer_interrupt_enable(TIMER1, TIMER_INT_UP);         //使能定时器的更新中断
//    nvic_irq_enable(TIMER1_IRQn, 2, 0);                   //配置NVIC设置优先级
    timer_enable(TIMER1);
}

//void TIMER1_IRQHandler(void)  
//{
//    if (timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)    //判断定时器更新中断是否发生
//    {
//        time++;
//        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);           //清除定时器更新中断标志 
//    }
//}
