/*!
    \file    gd32h7xx_dac.c
    \brief   DAC driver

    \version 2023-06-21, V1.1.0, firmware for GD32H7xx
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

#include "gd32h7xx_dac.h"

/* DAC register bit offset */
#define OUT1_REG_OFFSET           ((uint32_t)0x00000010U)
#define DH_12BIT_OFFSET           ((uint32_t)0x00000010U)
#define DH_8BIT_OFFSET            ((uint32_t)0x00000008U)

/*!
    \brief      deinitialize DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_deinit(void)
{
    rcu_periph_reset_enable(RCU_DACRST);
    rcu_periph_reset_disable(RCU_DACRST);
}

/*!
    \brief      enable DAC
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DEN1;
    } else {
    }
}

/*!
    \brief      disable DAC
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_disable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DEN1;
    } else {
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DDMAEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DDMAEN1;
    } else {
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DDMAEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DDMAEN1;
    } else {
    }
}

/*!
    \brief      configure DAC mode
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  mode: DAC working mode
                only one parameter can be selected which is shown as below:
      \arg        NORMAL_PIN_BUFFON: DAC_OUT_x work in normal mode and connect to external pin with buffer enable
      \arg        NORMAL_PIN_PERIPHERAL_BUFFON: DAC_OUT_x work in normal mode and connect to external pin and on chip peripherals with buffer enable
      \arg        NORMAL_PIN_BUFFOFF: DAC_OUT_x work in normal mode and connect to external pin with buffer disable
      \arg        NORMAL_PIN_PERIPHERAL_BUFFOFF: DAC_OUT_x work in normal mode and connect to on chip peripherals with buffer disable
      \arg        SAMPLEKEEP_PIN_BUFFON: DAC_OUT_x work in sample and keep mode and connect to external pin with buffer enable
      \arg        SAMPLEKEEP_PIN_PERIPHERAL_BUFFON: DAC_OUT_x work in sample and keep mode and connect to external pin and on chip peripherals with buffer enable
      \arg        SAMPLEKEEP_PIN_BUFFOFF: DAC_OUT_x work in sample and keep mode and connect to external pin and on chip peripherals with buffer enable
      \arg        SAMPLEKEEP_PIN_PERIPHERAL_BUFFOFF: DAC_OUT_x work in sample and keep mode and connect to on chip peripherals with buffer disable
    \param[out] none
    \retval     none
*/
void dac_mode_config(uint32_t dac_out, uint32_t mode)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC0 mode */
        DAC_MDCR &= ~DAC_MDCR_MODE0;
        DAC_MDCR |= mode;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC1 mode */
        DAC_MDCR &= ~DAC_MDCR_MODE1;
        DAC_MDCR |= (mode << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     DAC output data
*/
uint16_t dac_output_value_get(uint32_t dac_out)
{
    uint16_t data = 0U;
    if(DAC_OUT_0 == dac_out) {
        /* store the DAC0 output value */
        data = (uint16_t)OUT0_DO;
    } else if(DAC_OUT_1 == dac_out) {
        /* store the DAC1 output value */
        data = (uint16_t)OUT1_DO;
    } else {
    }
    return data;
}

/*!
    \brief      set the DAC specified data holding register value
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  dac_align: data alignment
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_8B_R: data right 8 bit alignment
      \arg        DAC_ALIGN_12B_R: data right 12 bit alignment
      \arg        DAC_ALIGN_12B_L: data left 12 bit alignment
    \param[in]  data: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_data_set(uint32_t dac_out, uint32_t dac_align, uint16_t data)
{
    if(DAC_OUT_0 == dac_out) {
        switch(dac_align) {
            /* data right 12 bit alignment */
            case DAC_ALIGN_12B_R:
                OUT0_R12DH = data;
                break;
            /* data left 12 bit alignment */
            case DAC_ALIGN_12B_L:
                OUT0_L12DH = data;
                break;
            /* data right 8 bit alignment */
            case DAC_ALIGN_8B_R:
                OUT0_R8DH = data;
                break;
            default:
                break;
        }
    } else {
        switch(dac_align) {
            /* data right 12 bit alignment */
            case DAC_ALIGN_12B_R:
                OUT1_R12DH = data;
                break;
            /* data left 12 bit alignment */
            case DAC_ALIGN_12B_L:
                OUT1_L12DH = data;
                break;
            /* data right 8 bit alignment */
            case DAC_ALIGN_8B_R:
                OUT1_R8DH = data;
                break;
            default:
                break;
        }
    }
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DTEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DTEN1;
    } else {
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DTEN0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DTEN1;
    } else {
    }
}

/*!
    \brief      configure DAC trigger source
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  triggersource: external triggers of DAC
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIGGER_EXTERNAL: external trigger selected by TRIGSEL
      \arg        DAC_TRIGGER_SOFTWARE: software trigger
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint32_t dac_out, uint32_t triggersource)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC0 trigger source */
        DAC_CTL0 &= ~DAC_CTL0_DTSEL0;
        DAC_CTL0 |= triggersource;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC1 trigger source */
        DAC_CTL0 &= ~DAC_CTL0_DTSEL1;
        DAC_CTL0 |= (triggersource << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \retval     none
*/
void dac_software_trigger_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_SWT |= DAC_SWT_SWTR0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_SWT |= DAC_SWT_SWTR1;
    } else {
    }
}

/*!
    \brief      disable DAC software trigger
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_software_trigger_disable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_SWT &= ~DAC_SWT_SWTR0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_SWT &= ~DAC_SWT_SWTR1;
    } else {
    }
}

/*!
    \brief      configure DAC wave mode
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  wave_mode: noise wave mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_WAVE_DISABLE: wave disable
      \arg        DAC_WAVE_MODE_LFSR: LFSR noise mode
      \arg        DAC_WAVE_MODE_TRIANGLE: triangle noise mode
    \param[out] none
    \retval     none
*/
void dac_wave_mode_config(uint32_t dac_out, uint32_t wave_mode)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC_OUT_0 wave mode */
        DAC_CTL0 &= ~DAC_CTL0_DWM0;
        DAC_CTL0 |= wave_mode;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC_OUT_1 wave mode */
        DAC_CTL0 &= ~DAC_CTL0_DWM1;
        DAC_CTL0 |= (wave_mode << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      configure DAC wave bit width
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  bit_width: noise wave bit width
                only one parameter can be selected which is shown as below:
      \arg        DAC_WAVE_BIT_WIDTH_1: bit width of the wave signal is 1
      \arg        DAC_WAVE_BIT_WIDTH_2: bit width of the wave signal is 2
      \arg        DAC_WAVE_BIT_WIDTH_3: bit width of the wave signal is 3
      \arg        DAC_WAVE_BIT_WIDTH_4: bit width of the wave signal is 4
      \arg        DAC_WAVE_BIT_WIDTH_5: bit width of the wave signal is 5
      \arg        DAC_WAVE_BIT_WIDTH_6: bit width of the wave signal is 6
      \arg        DAC_WAVE_BIT_WIDTH_7: bit width of the wave signal is 7
      \arg        DAC_WAVE_BIT_WIDTH_8: bit width of the wave signal is 8
      \arg        DAC_WAVE_BIT_WIDTH_9: bit width of the wave signal is 9
      \arg        DAC_WAVE_BIT_WIDTH_10: bit width of the wave signal is 10
      \arg        DAC_WAVE_BIT_WIDTH_11: bit width of the wave signal is 11
      \arg        DAC_WAVE_BIT_WIDTH_12: bit width of the wave signal is 12
    \param[out] none
    \retval     none
*/
void dac_wave_bit_width_config(uint32_t dac_out, uint32_t bit_width)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC_OUT_0 wave bit width */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= bit_width;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC_OUT_1 wave bit width */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (bit_width << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      configure DAC LFSR noise mode
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  unmask_bits: unmask LFSR bits in DAC LFSR noise mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_LFSR_BIT0: unmask the LFSR bit0
      \arg        DAC_LFSR_BITS1_0: unmask the LFSR bits[1:0]
      \arg        DAC_LFSR_BITS2_0: unmask the LFSR bits[2:0]
      \arg        DAC_LFSR_BITS3_0: unmask the LFSR bits[3:0]
      \arg        DAC_LFSR_BITS4_0: unmask the LFSR bits[4:0]
      \arg        DAC_LFSR_BITS5_0: unmask the LFSR bits[5:0]
      \arg        DAC_LFSR_BITS6_0: unmask the LFSR bits[6:0]
      \arg        DAC_LFSR_BITS7_0: unmask the LFSR bits[7:0]
      \arg        DAC_LFSR_BITS8_0: unmask the LFSR bits[8:0]
      \arg        DAC_LFSR_BITS9_0: unmask the LFSR bits[9:0]
      \arg        DAC_LFSR_BITS10_0: unmask the LFSR bits[10:0]
      \arg        DAC_LFSR_BITS11_0: unmask the LFSR bits[11:0]
    \param[out] none
    \retval     none
*/
void dac_lfsr_noise_config(uint32_t dac_out, uint32_t unmask_bits)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC_OUT_0 LFSR noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= unmask_bits;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC_OUT_1 LFSR noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (unmask_bits << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      configure DAC triangle noise mode
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  amplitude: triangle amplitude in DAC triangle noise mode
                only one parameter can be selected which is shown as below:
      \arg        DAC_TRIANGLE_AMPLITUDE_1: triangle amplitude is 1
      \arg        DAC_TRIANGLE_AMPLITUDE_3: triangle amplitude is 3
      \arg        DAC_TRIANGLE_AMPLITUDE_7: triangle amplitude is 7
      \arg        DAC_TRIANGLE_AMPLITUDE_15: triangle amplitude is 15
      \arg        DAC_TRIANGLE_AMPLITUDE_31: triangle amplitude is 31
      \arg        DAC_TRIANGLE_AMPLITUDE_63: triangle amplitude is 63
      \arg        DAC_TRIANGLE_AMPLITUDE_127: triangle amplitude is 127
      \arg        DAC_TRIANGLE_AMPLITUDE_255: triangle amplitude is 255
      \arg        DAC_TRIANGLE_AMPLITUDE_511: triangle amplitude is 511
      \arg        DAC_TRIANGLE_AMPLITUDE_1023: triangle amplitude is 1023
      \arg        DAC_TRIANGLE_AMPLITUDE_2047: triangle amplitude is 2047
      \arg        DAC_TRIANGLE_AMPLITUDE_4095: triangle amplitude is 4095
    \param[out] none
    \retval     none
*/
void dac_triangle_noise_config(uint32_t dac_out, uint32_t amplitude)
{
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC_OUT_0 triangle noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW0;
        DAC_CTL0 |= amplitude;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC_OUT_1 triangle noise mode */
        DAC_CTL0 &= ~DAC_CTL0_DWBW1;
        DAC_CTL0 |= (amplitude << OUT1_REG_OFFSET);
    } else {
    }
}

/*!
    \brief      enable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL0_DEN0 | DAC_CTL0_DEN1;
    DAC_CTL0 |= (ctl);
}

/*!
    \brief      disable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL0_DEN0 | DAC_CTL0_DEN1;
    DAC_CTL0 &= (~ctl);
}

/*!
    \brief      enable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_enable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT |= (swt);
}

/*!
    \brief      disable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_disable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT &= (~swt);
}

/*!
    \brief      set DAC concurrent mode data holding register value
    \param[in]  dac_align: data alignment
                only one parameter can be selected which is shown as below:
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data0: data to be loaded
    \param[in]  data1: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data0, uint16_t data1)
{
    uint32_t data = 0U;
    switch(dac_align) {
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            data = ((uint32_t)data1 << DH_12BIT_OFFSET) | data0;
            DACC_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            data = ((uint32_t)data1 << DH_12BIT_OFFSET) | data0;
            DACC_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            data = ((uint32_t)data1 << DH_8BIT_OFFSET) | data0;
            DACC_R8DH = data;
            break;
        default:
            break;
    }
}

/*!
    \brief      set DAC keep and sample time value
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  sample_time: DAC sample time
    \param[in]  keep_time:  DAC keep time
    \param[in]  refresh_time: DAC refresh time
    \param[out] none
    \retval     none
*/
void dac_sample_time_set(uint32_t dac_out, uint32_t sample_time, uint32_t keep_time,
                         uint32_t refresh_time)
{
    uint32_t tmp = 0U;
    if(DAC_OUT_0 == dac_out) {
        /* configure DAC_OUT_0 Sample & Refresh mode */
        DAC_SKSTR0 |= (sample_time & DAC_SKSTR0_TSAMP0);
        tmp = (DAC_SKKTR & ~(uint32_t)DAC_SKKTR_TKEEP0) | (keep_time & DAC_SKKTR_TKEEP0);
        DAC_SKKTR = tmp;
        tmp = (DAC_SKRTR & ~(uint32_t)DAC_SKRTR_TREF0) | (refresh_time & DAC_SKRTR_TREF0);
        DAC_SKRTR = tmp;
    } else if(DAC_OUT_1 == dac_out) {
        /* configure DAC_OUT_1 Sample & Refresh mode */
        DAC_SKSTR1 |= (sample_time & DAC_SKSTR1_TSAMP1);
        tmp = (DAC_SKKTR & ~(uint32_t)DAC_SKKTR_TKEEP1) | ((keep_time << OUT1_REG_OFFSET) &
                                                           DAC_SKKTR_TKEEP1);
        DAC_SKKTR = tmp;
        tmp = (DAC_SKRTR & ~(uint32_t)DAC_SKRTR_TREF1) | ((refresh_time << OUT1_REG_OFFSET) &
                                                          DAC_SKRTR_TREF1);
        DAC_SKRTR = tmp;
    } else {
    }
}

/*!
    \brief      get the DAC_OUT_x trimming value
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     DACx trimming value
*/
uint32_t dac_trimming_value_get(uint32_t dac_out)
{
    uint32_t tmp = 0U;
    if(DAC_OUT_0 == dac_out) {
        /* get the DAC_OUT_0 trimming value */
        tmp = DAC_CALR & DAC_CALR_OTV0;
    } else if(DAC_OUT_1 == dac_out) {
        /* get the DAC_OUT_1 trimming value */
        tmp = (DAC_CALR & DAC_CALR_OTV1) >> OUT1_REG_OFFSET;
    } else {
    }

    return tmp;
}

/*!
    \brief      set the DAC_OUT_x trimming value
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[in]  trim_value: set new DAC trimming value
    \param[out] none
*/
void dac_trimming_value_set(uint32_t dac_out, uint32_t trim_value)
{
    uint32_t tmp = DAC_CALR;
    if(DAC_OUT_0 == dac_out) {
        /* set the DAC_OUT_0 trimming value */
        tmp &= ~(uint32_t)DAC_CALR_OTV0;
        tmp |= (trim_value & DAC_CALR_OTV0);
        DAC_CALR = tmp;
    } else if(DAC_OUT_1 == dac_out) {
        /* set the DAC_OUT_1 trimming value */
        tmp &= ~(uint32_t)DAC_CALR_OTV1;
        tmp |= ((trim_value << OUT1_REG_OFFSET) & DAC_CALR_OTV1);
        DAC_CALR = tmp;
    } else {
    }
}

/*!
    \brief      enable the DAC_OUT_x trimming
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
*/
void dac_trimming_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        /* enable the DAC_OUT_0 trimming */
        DAC_CTL0 |= DAC_CTL0_CALEN0;
    } else if(DAC_OUT_1 == dac_out) {
        /* enable the DAC_OUT_1 trimming */
        DAC_CTL0 |= DAC_CTL0_CALEN1;
    } else {
    }
}

/*!
    \brief      enable DAC concurrent interrupt funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_interrupt_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL0_DDUDRIE0 | DAC_CTL0_DDUDRIE1;
    DAC_CTL0 |= (ctl);
}

/*!
    \brief      disable DAC concurrent interrupt funcution
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_interrupt_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL0_DDUDRIE0 | DAC_CTL0_DDUDRIE1;
    DAC_CTL0 &= (~ctl);
}

/*!
    \brief      enable DAC interrupt(DAC DMA underrun interrupt)
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_enable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DDUDRIE0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 |= DAC_CTL0_DDUDRIE1;
    } else {
    }
}

/*!
    \brief      disable DAC interrupt(DAC DMA underrun interrupt)
    \param[in]  dac_out: DACx(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_disable(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DDUDRIE0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_CTL0 &= ~DAC_CTL0_DDUDRIE1;
    } else {
    }
}

/*!
    \brief      get the DAC flag
    \param[in]  flag: the DAC status flags, only one parameter can be selected which is shown
                as below:
        \arg        DAC_FLAG_DDUDR0: DAC_OUT_0 DMA underrun flag
        \arg        DAC_FLAG_CALF0: DAC_OUT_0 calibration offset flag
        \arg        DAC_FLAG_BWT0: DAC_OUT_0 sample and keep wtire enable flag
        \arg        DAC_FLAG_DDUDR1: DAC_OUT_1 DMA underrun flag
        \arg        DAC_FLAG_CALF1: DAC_OUT_1 calibration offset flag
        \arg        DAC_FLAG_BWT1: DAC_OUT_1 sample and keep wtire enable flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dac_flag_get(uint32_t flag)
{
    FlagStatus temp_flag = RESET;

    if(RESET != (DAC_STAT0 & flag)) {
        temp_flag = SET;
    }

    return temp_flag;
}

/*!
    \brief      clear the DAC flag (DAC DMA underrun flag)
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_flag_clear(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_STAT0 |= DAC_STAT0_DDUDR0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_STAT0 |= DAC_STAT0_DDUDR1;
    } else {
    }
}

/*!
    \brief      get the specified DAC interrupt flag (DAC DMA underrun interrupt flag)
    \param[in]  dac_out: DAC_OUT_x(x = 0,1)
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dac_interrupt_flag_get(uint32_t dac_out)
{
    FlagStatus temp_flag = RESET;
    uint32_t ddudr_flag = 0U, ddudrie_flag = 0U;

    if(DAC_OUT_0 == dac_out) {
        /* check the DMA underrun flag and DAC DMA underrun interrupt enable flag */
        ddudr_flag = DAC_STAT0 & DAC_STAT0_DDUDR0;
        ddudrie_flag = DAC_CTL0 & DAC_CTL0_DDUDRIE0;
        if((RESET != ddudr_flag) && (RESET != ddudrie_flag)) {
            temp_flag = SET;
        }
    } else if(DAC_OUT_1 == dac_out) {
        /* check the DMA underrun flag and DAC DMA underrun interrupt enable flag */
        ddudr_flag = DAC_STAT0 & DAC_STAT0_DDUDR1;
        ddudrie_flag = DAC_CTL0 & DAC_CTL0_DDUDRIE1;
        if((RESET != ddudr_flag) && (RESET != ddudrie_flag)) {
            temp_flag = SET;
        }
    } else {
    }
    return temp_flag;
}

/*!
    \brief      clear the specified DAC interrupt flag (DAC DMA underrun interrupt flag)
    \param[in]  dac_out: DACx(x = 0,1)
    \param[out] none
    \retval     none
*/
void dac_interrupt_flag_clear(uint32_t dac_out)
{
    if(DAC_OUT_0 == dac_out) {
        DAC_STAT0 |= DAC_STAT0_DDUDR0;
    } else if(DAC_OUT_1 == dac_out) {
        DAC_STAT0 |= DAC_STAT0_DDUDR1;
    } else {
    }
}
