/*!
    \file    gd32h7xx_dac.h
    \brief   definitions for the DAC

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

#ifndef GD32H7XX_DAC_H
#define GD32H7XX_DAC_H

#include "gd32h7xx.h"

/* DACx(x=0,1) definitions */
#define DAC                               DAC_BASE
#define DAC_OUT_0                         0U
#define DAC_OUT_1                         1U

/* registers definitions */
#define DAC_CTL0                          REG32(DAC + 0x00000000U)    /*!< DAC control register */
#define DAC_SWT                           REG32(DAC + 0x00000004U)    /*!< DAC software trigger register */
#define OUT0_R12DH                        REG32(DAC + 0x00000008U)    /*!< DAC_OUT_0 12-bit right-aligned data holding register */
#define OUT0_L12DH                        REG32(DAC + 0x0000000CU)    /*!< DAC_OUT_0 12-bit left-aligned data holding register */
#define OUT0_R8DH                         REG32(DAC + 0x00000010U)    /*!< DAC_OUT_0 8-bit right-aligned data holding register */
#define OUT1_R12DH                        REG32(DAC + 0x00000014U)    /*!< DAC_OUT_1 12-bit right-aligned data holding register */
#define OUT1_L12DH                        REG32(DAC + 0x00000018U)    /*!< DAC_OUT_1 12-bit left-aligned data holding register */
#define OUT1_R8DH                         REG32(DAC + 0x0000001CU)    /*!< DAC_OUT_1 8-bit right-aligned data holding register */
#define DACC_R12DH                        REG32(DAC + 0x00000020U)    /*!< DAC concurrent mode 12-bit right-aligned data holding register */
#define DACC_L12DH                        REG32(DAC + 0x00000024U)    /*!< DAC concurrent mode 12-bit left-aligned data holding register */
#define DACC_R8DH                         REG32(DAC + 0x00000028U)    /*!< DAC concurrent mode 8-bit right-aligned data holding register */
#define OUT0_DO                           REG32(DAC + 0x0000002CU)    /*!< DAC_OUT_0 data output register */
#define OUT1_DO                           REG32(DAC + 0x00000030U)    /*!< DAC_OUT_1 data output register */
#define DAC_STAT0                         REG32(DAC + 0x00000034U)    /*!< DAC status register */
#define DAC_CALR                          REG32(DAC + 0x00000038U)    /*!< DAC calibration register */
#define DAC_MDCR                          REG32(DAC + 0x0000003CU)    /*!< DAC mode control register */
#define DAC_SKSTR0                        REG32(DAC + 0x00000040U)    /*!< DAC sample and keep sample time register 0 */
#define DAC_SKSTR1                        REG32(DAC + 0x00000044U)    /*!< DAC sample and keep sample time register 1 */
#define DAC_SKKTR                         REG32(DAC + 0x00000048U)    /*!< DAC sample and keep sample time register */
#define DAC_SKRTR                         REG32(DAC + 0x0000004CU)    /*!< DAC sample and keep refresh time register */

/* bits definitions */
/* DAC_CTL */
#define DAC_CTL0_DEN0                     BIT(0)                      /*!< DAC_OUT_0 enable/disable bit */
#define DAC_CTL0_DTEN0                    BIT(1)                      /*!< DAC_OUT_0 trigger enable/disable bit */
#define DAC_CTL0_DTSEL0                   BITS(2,3)                   /*!< DAC_OUT_0 trigger source selection enable/disable bits */
#define DAC_CTL0_DWM0                     BITS(6,7)                   /*!< DAC_OUT_0 noise wave mode */
#define DAC_CTL0_DWBW0                    BITS(8,11)                  /*!< DAC_OUT_0 noise wave bit width */
#define DAC_CTL0_DDMAEN0                  BIT(12)                     /*!< DAC_OUT_0 DMA enable/disable bit */
#define DAC_CTL0_DDUDRIE0                 BIT(13)                     /*!< DAC_OUT_0 DMA underrun interrupt enable/disable bit */
#define DAC_CTL0_CALEN0                   BIT(14)                     /*!< DAC_OUT_0 output calibration enable/disable bit */
#define DAC_CTL0_DEN1                     BIT(16)                     /*!< DAC_OUT_1 enable/disable bit */ 
#define DAC_CTL0_DTEN1                    BIT(17)                     /*!< DAC_OUT_1 trigger enable/disable bit */
#define DAC_CTL0_DTSEL1                   BITS(18,19)                 /*!< DAC_OUT_1 trigger source selection enable/disable bits */
#define DAC_CTL0_DWM1                     BITS(22,23)                 /*!< DAC_OUT_1 noise wave mode */
#define DAC_CTL0_DWBW1                    BITS(24,27)                 /*!< DAC_OUT_1 noise wave bit width */
#define DAC_CTL0_DDMAEN1                  BIT(28)                     /*!< DAC_OUT_1 DMA enable/disable bit */
#define DAC_CTL0_DDUDRIE1                 BIT(29)                     /*!< DAC_OUT_1 DMA underrun interrupt enable/disable bit */
#define DAC_CTL0_CALEN1                   BIT(30)                     /*!< DAC_OUT_1 output calibration enable/disable bit */

/* DAC_SWT */
#define DAC_SWT_SWTR0                     BIT(0)                      /*!< DAC_OUT_0 software trigger bit, cleared by hardware */
#define DAC_SWT_SWTR1                     BIT(1)                      /*!< DAC_OUT_1 software trigger bit, cleared by hardware */

/* OUT0_R12DH */
#define OUT0_R12DH_OUT0_DH                BITS(0,11)                  /*!< DAC_OUT_0 12-bit right-aligned data bits */

/* OUT0_L12DH */
#define OUT0_L12DH_OUT0_DH                BITS(4,15)                  /*!< DAC_OUT_0 12-bit left-aligned data bits */

/* OUT0_R8DH */
#define OUT0_R8DH_OUT0_DH                 BITS(0,7)                   /*!< DAC_OUT_0 8-bit right-aligned data bits */

/* OUT1_R12DH */
#define OUT1_R12DH_OUT1_DH                BITS(0,11)                  /*!< DAC_OUT_1 12-bit right-aligned data bits */

/* OUT1_L12DH */
#define OUT1_L12DH_OUT1_DH                BITS(4,15)                  /*!< DAC_OUT_1 12-bit left-aligned data bits */

/* OUT1_R8DH */
#define OUT1_R8DH_OUT1_DH                 BITS(0,7)                   /*!< DAC_OUT_1 8-bit right-aligned data bits */

/* DACC_R12DH */
#define DACC_R12DH_OUT0_DH                BITS(0,11)                  /*!< DAC concurrent mode DAC_OUT_0 12-bit right-aligned data bits */
#define DACC_R12DH_OUT1_DH                BITS(16,27)                 /*!< DAC concurrent mode DAC_OUT_1 12-bit right-aligned data bits */

/* DACC_L12DH */
#define DACC_L12DH_OUT0_DH                BITS(4,15)                  /*!< DAC concurrent mode DAC_OUT_0 12-bit left-aligned data bits */
#define DACC_L12DH_OUT1_DH                BITS(20,31)                 /*!< DAC concurrent mode DAC_OUT_1 12-bit left-aligned data bits */

/* DACC_R8DH */
#define DACC_R8DH_OUT0_DH                 BITS(0,7)                   /*!< DAC concurrent mode DAC_OUT_0 8-bit right-aligned data bits */
#define DACC_R8DH_OUT1_DH                 BITS(8,15)                  /*!< DAC concurrent mode DAC_OUT_1 8-bit right-aligned data bits */

/* OUT0_DO */
#define OUT0_DO_OUT0_DO                   BITS(0,11)                  /*!< DAC_OUT_0 12-bit output data bits */

/* OUT1_DO */
#define OUT1_DO_OUT1_DO                   BITS(0,11)                  /*!< DAC_OUT_1 12-bit output data bits */

/* DAC_STAT0 */
#define DAC_STAT0_DDUDR0                  BIT(13)                     /*!< DAC_OUT_0 DMA underrun flag */
#define DAC_STAT0_CALF0                   BIT(14)                     /*!< DAC_OUT_0 calibration offset flag */
#define DAC_STAT0_BWT0                    BIT(15)                     /*!< DAC_OUT_0 SKSTR0 writing flag */
#define DAC_STAT0_DDUDR1                  BIT(29)                     /*!< DAC_OUT_1 DMA underrun flag */
#define DAC_STAT0_CALF1                   BIT(30)                     /*!< DAC_OUT_1 calibration offset flag */
#define DAC_STAT0_BWT1                    BIT(31)                     /*!< DAC_OUT_1 SKSTR1 writing flag */

/* DAC_CALR */
#define DAC_CALR_OTV0                     BITS(0,4)                   /*!< DAC_OUT_0 offset trimming value */
#define DAC_CALR_OTV1                     BITS(16,20)                 /*!< DAC_OUT_1 offset trimming value */

/* DAC_MDCR */
#define DAC_MDCR_MODE0                    BITS(0,2)                   /*!< DAC_OUT_0 mode */
#define DAC_MDCR_MODE1                    BITS(16,18)                 /*!< DAC_OUT_1 mode */

/* DAC_SKSTR0 */
#define DAC_SKSTR0_TSAMP0                 BITS(0,9)                   /*!< DAC_OUT_0 sample time */

/* DAC_SKSTR1 */
#define DAC_SKSTR1_TSAMP1                 BITS(0,9)                   /*!< DAC_OUT_1 sample time */

/* DAC_SKKTR */
#define DAC_SKKTR_TKEEP0                  BITS(0,9)                   /*!< DAC_OUT_0 keep time */
#define DAC_SKKTR_TKEEP1                  BITS(16,25)                 /*!< DAC_OUT_1 keep time */

/* DAC_SKRTR */
#define DAC_SKRTR_TREF0                   BITS(0,7)                   /*!< DAC_OUT_0 refresh time */
#define DAC_SKRTR_TREF1                   BITS(16,23)                 /*!< DAC_OUT_1 refresh time */

/* constants definitions */
/* DAC trigger source */
#define CTL0_DTSEL(regval)                (BITS(2,3) & ((uint32_t)(regval) << 2U))
#define DAC_TRIGGER_EXTERNAL              CTL0_DTSEL(0)               /*!< external trigger selected by TRIGSEL */
#define DAC_TRIGGER_SOFTWARE              CTL0_DTSEL(1)               /*!< software trigger */


/* DAC noise wave mode */
#define CTL0_DWM(regval)                  (BITS(6,7) & ((uint32_t)(regval) << 6U))
#define DAC_WAVE_DISABLE                  CTL0_DWM(0)                 /*!< wave disable */
#define DAC_WAVE_MODE_LFSR                CTL0_DWM(1)                 /*!< LFSR noise mode */
#define DAC_WAVE_MODE_TRIANGLE            CTL0_DWM(2)                 /*!< triangle noise mode */

/* DAC noise wave bit width */
#define DWBW(regval)                      (BITS(8,11) & ((uint32_t)(regval) << 8U))
#define DAC_WAVE_BIT_WIDTH_1              DWBW(0)                     /*!< bit width of the wave signal is 1 */
#define DAC_WAVE_BIT_WIDTH_2              DWBW(1)                     /*!< bit width of the wave signal is 2 */
#define DAC_WAVE_BIT_WIDTH_3              DWBW(2)                     /*!< bit width of the wave signal is 3 */
#define DAC_WAVE_BIT_WIDTH_4              DWBW(3)                     /*!< bit width of the wave signal is 4 */
#define DAC_WAVE_BIT_WIDTH_5              DWBW(4)                     /*!< bit width of the wave signal is 5 */
#define DAC_WAVE_BIT_WIDTH_6              DWBW(5)                     /*!< bit width of the wave signal is 6 */
#define DAC_WAVE_BIT_WIDTH_7              DWBW(6)                     /*!< bit width of the wave signal is 7 */
#define DAC_WAVE_BIT_WIDTH_8              DWBW(7)                     /*!< bit width of the wave signal is 8 */
#define DAC_WAVE_BIT_WIDTH_9              DWBW(8)                     /*!< bit width of the wave signal is 9 */
#define DAC_WAVE_BIT_WIDTH_10             DWBW(9)                     /*!< bit width of the wave signal is 10 */
#define DAC_WAVE_BIT_WIDTH_11             DWBW(10)                    /*!< bit width of the wave signal is 11 */
#define DAC_WAVE_BIT_WIDTH_12             DWBW(11)                    /*!< bit width of the wave signal is 12 */

/* unmask LFSR bits in DAC LFSR noise mode */
#define DAC_LFSR_BIT0                     DAC_WAVE_BIT_WIDTH_1        /*!< unmask the LFSR bit0 */
#define DAC_LFSR_BITS1_0                  DAC_WAVE_BIT_WIDTH_2        /*!< unmask the LFSR bits[1:0] */
#define DAC_LFSR_BITS2_0                  DAC_WAVE_BIT_WIDTH_3        /*!< unmask the LFSR bits[2:0] */
#define DAC_LFSR_BITS3_0                  DAC_WAVE_BIT_WIDTH_4        /*!< unmask the LFSR bits[3:0] */
#define DAC_LFSR_BITS4_0                  DAC_WAVE_BIT_WIDTH_5        /*!< unmask the LFSR bits[4:0] */
#define DAC_LFSR_BITS5_0                  DAC_WAVE_BIT_WIDTH_6        /*!< unmask the LFSR bits[5:0] */
#define DAC_LFSR_BITS6_0                  DAC_WAVE_BIT_WIDTH_7        /*!< unmask the LFSR bits[6:0] */
#define DAC_LFSR_BITS7_0                  DAC_WAVE_BIT_WIDTH_8        /*!< unmask the LFSR bits[7:0] */
#define DAC_LFSR_BITS8_0                  DAC_WAVE_BIT_WIDTH_9        /*!< unmask the LFSR bits[8:0] */
#define DAC_LFSR_BITS9_0                  DAC_WAVE_BIT_WIDTH_10       /*!< unmask the LFSR bits[9:0] */
#define DAC_LFSR_BITS10_0                 DAC_WAVE_BIT_WIDTH_11       /*!< unmask the LFSR bits[10:0] */
#define DAC_LFSR_BITS11_0                 DAC_WAVE_BIT_WIDTH_12       /*!< unmask the LFSR bits[11:0] */

/* DAC data alignment */
#define DATA_ALIGN(regval)                (BITS(0,1) & ((uint32_t)(regval)))
#define DAC_ALIGN_12B_R                   DATA_ALIGN(0)               /*!< data right 12 bit alignment */
#define DAC_ALIGN_12B_L                   DATA_ALIGN(1)               /*!< data left 12 bit alignment */
#define DAC_ALIGN_8B_R                    DATA_ALIGN(2)               /*!< data right 8 bit alignment */

/* triangle amplitude in DAC triangle noise mode */
#define DAC_TRIANGLE_AMPLITUDE_1          DAC_WAVE_BIT_WIDTH_1        /*!< triangle amplitude is 1 */
#define DAC_TRIANGLE_AMPLITUDE_3          DAC_WAVE_BIT_WIDTH_2        /*!< triangle amplitude is 3 */
#define DAC_TRIANGLE_AMPLITUDE_7          DAC_WAVE_BIT_WIDTH_3        /*!< triangle amplitude is 7 */
#define DAC_TRIANGLE_AMPLITUDE_15         DAC_WAVE_BIT_WIDTH_4        /*!< triangle amplitude is 15 */
#define DAC_TRIANGLE_AMPLITUDE_31         DAC_WAVE_BIT_WIDTH_5        /*!< triangle amplitude is 31 */
#define DAC_TRIANGLE_AMPLITUDE_63         DAC_WAVE_BIT_WIDTH_6        /*!< triangle amplitude is 63 */
#define DAC_TRIANGLE_AMPLITUDE_127        DAC_WAVE_BIT_WIDTH_7        /*!< triangle amplitude is 127 */
#define DAC_TRIANGLE_AMPLITUDE_255        DAC_WAVE_BIT_WIDTH_8        /*!< triangle amplitude is 255 */
#define DAC_TRIANGLE_AMPLITUDE_511        DAC_WAVE_BIT_WIDTH_9        /*!< triangle amplitude is 511 */
#define DAC_TRIANGLE_AMPLITUDE_1023       DAC_WAVE_BIT_WIDTH_10       /*!< triangle amplitude is 1023 */
#define DAC_TRIANGLE_AMPLITUDE_2047       DAC_WAVE_BIT_WIDTH_11       /*!< triangle amplitude is 2047 */
#define DAC_TRIANGLE_AMPLITUDE_4095       DAC_WAVE_BIT_WIDTH_12       /*!< triangle amplitude is 4095 */

/* DAC data alignment */
#define MODE(regval)                      (BITS(0,2) & ((uint32_t)(regval)))
#define NORMAL_PIN_BUFFON                 MODE(0)                     /*!< DAC_OUT_x work in normal mode and connect to external pin with buffer enable */
#define NORMAL_PIN_PERIPHERAL_BUFFON      MODE(1)                     /*!< DAC_OUT_x work in normal mode and connect to external pin and on chip peripherals with buffer enable */
#define NORMAL_PIN_BUFFOFF                MODE(2)                     /*!< DAC_OUT_x work in normal mode and connect to external pin with buffer disable */
#define NORMAL_PIN_PERIPHERAL_BUFFOFF     MODE(3)                     /*!< DAC_OUT_x work in normal mode and connect to on chip peripherals with buffer disable */
#define SAMPLEKEEP_PIN_BUFFON             MODE(4)                     /*!< DAC_OUT_x work in sample and keep mode and connect to external pin with buffer enable */
#define SAMPLEKEEP_PIN_PERIPHERAL_BUFFON  MODE(5)                     /*!< DAC_OUT_x work in sample and keep mode and connect to external pin and on chip peripherals with buffer enable */
#define SAMPLEKEEP_PIN_BUFFOFF            MODE(6)                     /*!< DAC_OUT_x work in sample and keep mode and connect to external pin and on chip peripherals with buffer disable */
#define SAMPLEKEEP_PIN_PERIPHERAL_BUFFOFF MODE(7)                     /*!< DAC_OUT_x work in sample and keep mode and connect to on chip peripherals with buffer disable */

/* DAC flags */
#define DAC_FLAG_DDUDR0                   DAC_STAT0_DDUDR0             /*!< DAC_OUT_0 DMA underrun flag */
#define DAC_FLAG_CALF0                    DAC_STAT0_CALF0              /*!< DAC_OUT_0 calibration offset flag */
#define DAC_FLAG_BWT0                     DAC_STAT0_BWT0               /*!< DAC_OUT_0 sample and keep wtire enable flag */
#define DAC_FLAG_DDUDR1                   DAC_STAT0_DDUDR1             /*!< DAC_OUT_1 DMA underrun flag */
#define DAC_FLAG_CALF1                    DAC_STAT0_CALF1              /*!< DAC_OUT_1 calibration offset flag */
#define DAC_FLAG_BWT1                     DAC_STAT0_BWT1               /*!< DAC_OUT_1 sample and keep wtire enable flag */

/* function declarations */
/* initialization functions */
/* deinitialize DAC */
void dac_deinit(void);
/* enable DAC */
void dac_enable(uint32_t dac_out);
/* disable DAC */
void dac_disable(uint32_t dac_out);
/* enable DAC DMA */
void dac_dma_enable(uint32_t dac_out);
/* disable DAC DMA */
void dac_dma_disable(uint32_t dac_out); 
/* configure DAC mode */
void dac_mode_config(uint32_t dac_out, uint32_t mode);
/* get the last data output value */
uint16_t dac_output_value_get(uint32_t dac_out);
/* set DAC data holding register value */
void dac_data_set(uint32_t dac_out, uint32_t dac_align, uint16_t data);

/* DAC trigger configuration */
/* enable DAC trigger */
void dac_trigger_enable(uint32_t dac_out);
/* disable DAC trigger */
void dac_trigger_disable(uint32_t dac_out);
/* configure DAC trigger source */
void dac_trigger_source_config(uint32_t dac_out, uint32_t triggersource);
/* enable DAC software trigger */
void dac_software_trigger_enable(uint32_t dac_out);
/* disable DAC software trigger */
void dac_software_trigger_disable(uint32_t dac_out);

/* DAC wave mode configuration */
/* configure DAC wave mode */
void dac_wave_mode_config(uint32_t dac_out, uint32_t wave_mode);
/* configure DAC wave bit width */
void dac_wave_bit_width_config(uint32_t dac_out, uint32_t bit_width);
/* configure DAC LFSR noise mode */
void dac_lfsr_noise_config(uint32_t dac_out, uint32_t unmask_bits);
/* configure DAC triangle noise mode */
void dac_triangle_noise_config(uint32_t dac_out, uint32_t amplitude);

/* DAC concurrent mode configuration */
/* enable DAC concurrent mode */
void dac_concurrent_enable(void);
/* disable DAC concurrent mode */
void dac_concurrent_disable(void);
/* enable DAC concurrent software trigger */
void dac_concurrent_software_trigger_enable(void);
/* disable DAC concurrent software trigger */
void dac_concurrent_software_trigger_disable(void);
/* set DAC concurrent mode data holding register value */
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data0, uint16_t data1);
/* set DAC keep and sample time value */
void dac_sample_time_set(uint32_t dac_out, uint32_t sample_time, uint32_t keep_time, uint32_t refresh_time);
/* get the DAC trimming value */
uint32_t dac_trimming_value_get(uint32_t dac_out);
/* set the DACx trimming value */
void dac_trimming_value_set(uint32_t dac_out, uint32_t trim_value);
/* enable the DACx trimming */
void dac_trimming_enable(uint32_t dac_out);

/* DAC interrupt configuration */
/* enable DAC concurrent interrupt */
void dac_concurrent_interrupt_enable(void);
/* disable DAC concurrent interrupt */
void dac_concurrent_interrupt_disable(void);
/* enable DAC interrupt */
void dac_interrupt_enable(uint32_t dac_out);
/* disable DAC interrupt */
void dac_interrupt_disable(uint32_t dac_out);
/* get the DAC flag */
FlagStatus dac_flag_get(uint32_t flag);
/* clear the DAC flag */
void dac_flag_clear(uint32_t dac_out);
/* get the DAC interrupt flag */
FlagStatus dac_interrupt_flag_get(uint32_t dac_out);
/* clear the DAC interrupt flag */
void dac_interrupt_flag_clear(uint32_t dac_out);

#endif /* GD32H7XX_DAC_H */
