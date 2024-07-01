/*!
    \file    gd32h7xx_cmp.h
    \brief   definitions for the CMP

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

#ifndef GD32H7XX_CMP_H
#define GD32H7XX_CMP_H

#include "gd32h7xx.h"

/* CMP definitions */
#define CMP                                       CMP_BASE                       /*!< CMP0 base address */  
#define CMP0                                      CMP_BASE + 0x0000000CU         /*!< CMP0 base address */  
#define CMP1                                      CMP_BASE + 0x00000010U         /*!< CMP1 base address */  

/* registers definitions */
#define CMP_STAT                                  REG32(CMP + 0x00000000U)       /*!< CMP status register */
#define CMP_IFC                                   REG32(CMP + 0x00000004U)       /*!< CMP interrupt flag clear register */
#define CMP_SR                                    REG32(CMP + 0x00000008U)       /*!< CMP alternate select register */
#define CMP0_CS                                   REG32(CMP0 + 0x00000000U)      /*!< CMP0 control and status register */
#define CMP1_CS                                   REG32(CMP1 + 0x00000000U)      /*!< CMP1 control and status register */

/* bits definitions */
/* CMP_STAT */      
#define CMP_STAT_CMP0OT                           BIT(0)                         /*!< CMP0 output */
#define CMP_STAT_CMP1OT                           BIT(1)                         /*!< CMP1 output */
#define CMP_STAT_CMP0IF                           BIT(16)                        /*!< CMP0 interrupt flag */
#define CMP_STAT_CMP1IF                           BIT(17)                        /*!< CMP1 interrupt flag */

/* CMP_IFC */
#define CMP_IFC_CMP0IC                            BIT(16)                        /*!< CMP0 interrupt flag clear */
#define CMP_IFC_CMP1IC                            BIT(17)                        /*!< CMP1 interrupt flag clear */

/* CMP_SR */
#define CMP_SR_AFSE_PA6                           BIT(0)                         /*!< PA6 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PA8                           BIT(1)                         /*!< PA8 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PB12                          BIT(2)                         /*!< PB12 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PE6                           BIT(3)                         /*!< PE6 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PE15                          BIT(4)                         /*!< PE15 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PG2                           BIT(5)                         /*!< PG2 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PG3                           BIT(6)                         /*!< PG3 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PG4                           BIT(7)                         /*!< PG4 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PK0                           BIT(8)                         /*!< PK0 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PK1                           BIT(9)                         /*!< PK1 alternate function select for CMPx_OUT */
#define CMP_SR_AFSE_PK2                           BIT(10)                        /*!< PK2 alternate function select for CMPx_OUT */

/* CMP0_CS */
#define CMP0_CS_EN                                BIT(0)                         /*!< CMP0 enable  */
#define CMP0_CS_BEN                               BIT(1)                         /*!< CMP0 scaler bridge enable bit */
#define CMP0_CS_VSCEN                             BIT(2)                         /*!< CMP0 voltage scaler enable bit */
#define CMP0_CS_PL                                BIT(3)                         /*!< polarity of CMP0 output */
#define CMP0_CS_INTEN                             BIT(6)                         /*!< CMP0 interrupt enable */
#define CMP0_CS_HST                               BITS(8,9)                      /*!< CMP0 hysteresis */
#define CMP0_CS_PM                                BITS(12,13)                    /*!< CMP0 mode */
#define CMP0_CS_MISEL                             BITS(16,18)                    /*!< CMP0_IM internal input selection */
#define CMP0_CS_PSEL                              BIT(20)                        /*!< CMP0_IP input selection */
#define CMP0_CS_BLK                               BITS(24,27)                    /*!< CMP0 output blanking source */
#define CMP0_CS_LK                                BIT(31)                        /*!< CMP0 lock */

/* CMP1_CS */
#define CMP1_CS_EN                                BIT(0)                         /*!< CMP1 enable  */
#define CMP1_CS_BEN                               BIT(1)                         /*!< CMP1 scaler bridge enable bit */
#define CMP1_CS_VSCEN                             BIT(2)                         /*!< CMP1 voltage scaler enable bit */
#define CMP1_CS_PL                                BIT(3)                         /*!< polarity of CMP1 output */
#define CMP1_CS_WNDEN                             BIT(4)                         /*!< CMP Window mode enable */
#define CMP1_CS_INTEN                             BIT(6)                         /*!< CMP1 interrupt enable bit */
#define CMP1_CS_HST                               BITS(8,9)                      /*!< CMP1 hysteresis */
#define CMP1_CS_PM                                BITS(12,13)                    /*!< CMP1 mode */
#define CMP1_CS_MISEL                             BITS(16,18)                    /*!< CMP1_IM internal input selection */
#define CMP1_CS_PSEL                              BIT(20)                        /*!< CMP1_IP input selection */
#define CMP1_CS_BLK                               BITS(24,27)                    /*!< CMP1 output blanking source */
#define CMP1_CS_LK                                BIT(31)                        /*!< CMP1 lock */

/* constants definitions */
/* operating mode */
typedef enum
{
    CMP_HIGHSPEED = 0,                                                           /*!< high speed mode */
    CMP_MIDDLESPEED = 1,                                                         /*!< medium speed mode */
    CMP_VERYLOWSPEED = 3                                                         /*!< very-low speed mode */
}operating_mode_enum;

/* inverting input */
typedef enum
{
    CMP_1_4VREFINT = 0,                                                          /*!< VREFINT /4 input */
    CMP_1_2VREFINT,                                                              /*!< VREFINT /2 input */
    CMP_3_4VREFINT,                                                              /*!< VREFINT *3/4 input */
    CMP_VREFINT,                                                                 /*!< VREFINT input */
    CMP_DACOUT0,                                                                 /*!< DAC_OUT_0 input */
    CMP_DACOUT1,                                                                 /*!< DAC_OUT_1 input */
    CMP_INV_INPUT1,                                                              /*!< inverting input source 1(PB1 for CMP0, PE10 for CMP1) */
    CMP_INV_INPUT2                                                               /*!< inverting input source 2(PC4 for CMP0, PE7 for CMP1) */
}inverting_input_enum;

/* plus input */
typedef enum
{
    CMP_PB0_PE9 = 0,                                                             /*!< PB0 for CMP0, PE9 for CMP1 */
    CMP_PB2_PE11                                                                 /*!< PB2 for CMP0, PE11 for CMP1 */
}plus_input_enum;

/* hysteresis */
typedef enum
{
    CMP_HYSTERESIS_NO = 0,                                                       /*!< output no hysteresis */
    CMP_HYSTERESIS_LOW,                                                          /*!< output low hysteresis */
    CMP_HYSTERESIS_MIDDLE,                                                       /*!< output middle hysteresis */
    CMP_HYSTERESIS_HIGH                                                          /*!< output high hysteresis */
}cmp_hysteresis_enum;

/* output inv */
typedef enum{
    CMP_OUTPUT_POLARITY_INVERTED = 0,                                            /*!< output is inverted */
    CMP_OUTPUT_POLARITY_NOINVERTED                                               /*!< output is not inverted */
}cmp_output_inv_enum;

/* blanking_sourc */  
typedef enum
{
    CMP_BLANKING_NONE = 0,                                                       /*!< output no selection */
    CMP_BLANKING_TIMER0_OC0 = 1,                                                 /*!< TIMER 0 output channel0 */
    CMP_BLANKING_TIMER1_OC2 = 2,                                                 /*!< TIMER 1 output channel2 */
    CMP_BLANKING_TIMER2_OC2 = 3,                                                 /*!< TIMER 2 output channel2 */
    CMP_BLANKING_TIMER2_OC3 = 4,                                                 /*!< TIMER 2 output channel3 */
    CMP_BLANKING_TIMER7_OC0 = 5,                                                 /*!< TIMER 7 output channel0 */
    CMP_BLANKING_TIMER14_OC0 = 6                                                 /*!< TIMER 14 output channel0 */
}blanking_source_enum;

/* comparator altemate function of output ports */  
typedef enum
{
    CMP_AFSE_PA6 = CMP_SR_AFSE_PA6,                                              /*!< PA6 alternate function select for CMPx_OUT */
    CMP_AFSE_PA8 = CMP_SR_AFSE_PA8,                                              /*!< PA8 alternate function select for CMPx_OUT */
    CMP_AFSE_PB12 = CMP_SR_AFSE_PB12,                                            /*!< PB12 alternate function select for CMPx_OUT */
    CMP_AFSE_PE6 = CMP_SR_AFSE_PE6,                                              /*!< PE6 alternate function select for CMPx_OUT */
    CMP_AFSE_PE15 = CMP_SR_AFSE_PE15,                                            /*!< PE15 alternate function select for CMPx_OUT */
    CMP_AFSE_PG2 = CMP_SR_AFSE_PG2,                                              /*!< PG2 alternate function select for CMPx_OUT */
    CMP_AFSE_PG3 = CMP_SR_AFSE_PG3,                                              /*!< PG3 alternate function select for CMPx_OUT */
    CMP_AFSE_PG4 = CMP_SR_AFSE_PG4,                                              /*!< PG4 alternate function select for CMPx_OUT */
    CMP_AFSE_PK0 = CMP_SR_AFSE_PK0,                                              /*!< PK0 alternate function select for CMPx_OUT */
    CMP_AFSE_PK1 = CMP_SR_AFSE_PK1,                                              /*!< PK1 alternate function select for CMPx_OUT */
    CMP_AFSE_PK2 = CMP_SR_AFSE_PK2                                               /*!< PK2 alternate function select for CMPx_OUT */
}output_select_enum;

/* output state */
typedef enum
{
    CMP_OUTPUTLEVEL_LOW = 0,                                                     /*!< the output is low */
    CMP_OUTPUTLEVEL_HIGH                                                         /*!< the output is high */
}cmp_output_state_enum;

/* CMP0 bridge enable*/
#define CS_CMP0BEN(regval)                        (BIT(1) & ((uint32_t)(regval) << 1U))
#define CS_CMP0BEN_DISABLE                        CS_CMP0BEN(0)                  /*!< CMP0 bridge disable */
#define CS_CMP0BEN_ENABLE                         CS_CMP0BEN(1)                  /*!< CMP0 bridge enable */

/* CMP0 voltage scaler*/
#define CS_CMP0SEN(regval)                        (BIT(2) & ((uint32_t)(regval) << 2U))
#define CS_CMP0SEN_DISABLE                        CS_CMP0SEN(0)                  /*!< CMP0 voltage scaler disable */
#define CS_CMP0SEN_ENABLE                         CS_CMP0SEN(1)                  /*!< CMP0 voltage scaler enable */

/* CMP0 output polarity*/
#define CS_CMP0PL(regval)                         (BIT(3) & ((uint32_t)(regval) << 3U))
#define CS_CMP0PL_NOL                             CS_CMP0PL(0)                   /*!< CMP0 output not inverted */
#define CS_CMP0PL_INV                             CS_CMP0PL(1)                   /*!< CMP0 output inverted */

/* CMP0 interrupt*/
#define CS_CMP0INTEN(regval)                      (BIT(6) & ((uint32_t)(regval) << 6U))
#define CS_CMP0INTEN_DISABLE                      CS_CMP0INTEN(0)                /*!< CMP0 interrupt enable */
#define CS_CMP0INTEN_ENABLE                       CS_CMP0INTEN(1)                /*!< CMP0 interrupt disable */

/* CMP0 hysteresis */
#define CS_CMP0HST(regval)                        (BITS(8,9) & ((uint32_t)(regval) << 8U))
#define CS_CMP0HST_HYSTERESIS_NO                  CS_CMP0HST(0)                  /*!< CMP0 output no hysteresis */
#define CS_CMP0HST_HYSTERESIS_LOW                 CS_CMP0HST(1)                  /*!< CMP0 output low hysteresis */
#define CS_CMP0HST_HYSTERESIS_MIDDLE              CS_CMP0HST(2)                  /*!< CMP0 output middle hysteresis */
#define CS_CMP0HST_HYSTERESIS_HIGH                CS_CMP0HST(3)                  /*!< CMP0 output high hysteresis */

/* CMP0 mode */
#define CS_CMP0PM(regval)                         (BITS(12,13) & ((uint32_t)(regval) << 12U))
#define CS_CMP0PM_HIGHSPEED                       CS_CMP0PM(0)                   /*!< CMP0 mode high speed */
#define CS_CMP0PM_MIDDLESPEED                     CS_CMP0PM(1)                   /*!< CMP0 mode middle speed */
#define CS_CMP0PM_VERYLOWSPEED                    CS_CMP0PM(3)                   /*!< CMP0 mode very low speed */

/* CMP0 inverting input */
#define CS_CMP0MSEL(regval)                       (BITS(16,18) & ((uint32_t)(regval) << 16U))
#define CS_CMP0MSEL_1_4VREFINT                    CS_CMP0MSEL(0)                 /*!< CMP0 inverting input 1/4 Vrefint */
#define CS_CMP0MSEL_1_2VREFINT                    CS_CMP0MSEL(1)                 /*!< CMP0 inverting input 1/2 Vrefint */
#define CS_CMP0MSEL_3_4VREFINT                    CS_CMP0MSEL(2)                 /*!< CMP0 inverting input 3/4 Vrefint */
#define CS_CMP0MSEL_VREFINT                       CS_CMP0MSEL(3)                 /*!< CMP0 inverting input Vrefint */
#define CS_CMP0MSEL_DACOUT0                       CS_CMP0MSEL(4)                 /*!< CMP0 inverting input DAC_OUT_0 */
#define CS_CMP0MSEL_DACOUT1                       CS_CMP0MSEL(5)                 /*!< CMP0 inverting input DAC_OUT_1 */
#define CS_CMP0MSEL_INPUT1                        CS_CMP0MSEL(6)                 /*!< CMP0 inverting input PB1 */
#define CS_CMP0MSEL_INPUT2                        CS_CMP0MSEL(7)                 /*!< CMP0 inverting input PC4 */

/* CMP0_IP input*/
#define CS_CMP0PSEL(regval)                       (BIT(20) & ((uint32_t)(regval) << 20U))
#define CS_CMP0PSEL_PB0                           CS_CMP0PSEL(0)                 /*!< CMP0_IP input PB0 */
#define CS_CMP0PSEL_PB2                           CS_CMP0PSEL(1)                 /*!< CMP0_IP input PB2 */

/* CMP0 Blanking suorce */
#define CS_CMP0BLK(regval)                        (BITS(24,27) & ((uint32_t)(regval) << 24U))
#define CS_CMP0BLK_NOBLK                          CS_CMP0BLK(0)                  /*!< CMP0 no blanking */
#define CS_CMP0BLK_TIMER0_OC0                     CS_CMP0BLK(1)                  /*!< CMP0 TIMER0 OC0 selected as blanking source */
#define CS_CMP0BLK_TIMER1_OC2                     CS_CMP0BLK(2)                  /*!< CMP0 TIMER1 OC2 selected as blanking source */
#define CS_CMP0BLK_TIMER2_OC2                     CS_CMP0BLK(3)                  /*!< CMP0 TIMER2 OC2 selected as blanking source */
#define CS_CMP0BLK_TIMER2_OC3                     CS_CMP0BLK(4)                  /*!< CMP0 TIMER2 OC3 selected as blanking source */
#define CS_CMP0BLK_TIMER7_OC4                     CS_CMP0BLK(5)                  /*!< CMP0 TIMER7 OC4 selected as blanking source */
#define CS_CMP0BLK_TIMER14_OC0                    CS_CMP0BLK(6)                  /*!< CMP0 TIMER14 OC0 selected as blanking source */

/* CMP0 lock bit*/
#define CS_CMP0LK(regval)                         (BIT(31) & ((uint32_t)(regval) << 31U))
#define CS_CMP0LK_DISABLE                         CS_CMP0LK(0)                   /*!< CMP0_CS bits are read-write */
#define CS_CMP0LK_ENABLE                          CS_CMP0LK(1)                   /*!< CMP0_CS bits are read-only */


/* CMP1 bridge enable*/
#define CS_CMP1BEN(regval)                        (BIT(1) & ((uint32_t)(regval) << 1U))
#define CS_CMP1BEN_DISABLE                        CS_CMP1BEN(0)                  /*!< CMP1 bridge disable */
#define CS_CMP1BEN_ENABLE                         CS_CMP1BEN(1)                  /*!< CMP1 bridge enable */

/* CMP1 voltage scaler*/
#define CS_CMP1SEN(regval)                        (BIT(2) & ((uint32_t)(regval) << 2U))
#define CS_CMP1SEN_DISABLE                        CS_CMP1SEN(0)                  /*!< CMP1 voltage scaler disable */
#define CS_CMP1SEN_ENABLE                         CS_CMP1SEN(1)                  /*!< CMP1 voltage scaler enable */

/* CMP1 output polarity*/
#define CS_CMP1PL(regval)                         (BIT(3) & ((uint32_t)(regval) << 3U))
#define CS_CMP1PL_NOL                             CS_CMP1PL(0)                   /*!< CMP1 output not inverted */
#define CS_CMP1PL_INV                             CS_CMP1PL(1)                   /*!< CMP1 output inverted */

/* CMP1 window mode*/
#define CS_CMP1WEN(regval)                        (BIT(4) & ((uint32_t)(regval) << 4U))
#define CS_CMP1WEN_DISABLE                        CS_CMP1WEN(0)                  /*!< CMP1_IP is connected to CMP1 non-inverting input */
#define CS_CMP1WEN_ENABLE                         CS_CMP1WEN(1)                  /*!< CMP1_IP is connected to CMP0_IP */

/* CMP1 interrupt*/
#define CS_CMP1INTEN(regval)                      (BIT(6) & ((uint32_t)(regval) << 6U))
#define CS_CMP1INTEN_DISABLE                      CS_CMP1INTEN(0)                /*!< CMP1 interrupt enable */
#define CS_CMP1INTEN_ENABLE                       CS_CMP1INTEN(1)                /*!< CMP1 interrupt disable */

/* CMP1 hysteresis */
#define CS_CMP1HST(regval)                        (BITS(8,9) & ((uint32_t)(regval) << 8U))
#define CS_CMP1HST_HYSTERESIS_NO                  CS_CMP1HST(0)                  /*!< CMP1 output no hysteresis */
#define CS_CMP1HST_HYSTERESIS_LOW                 CS_CMP1HST(1)                  /*!< CMP1 output low hysteresis */
#define CS_CMP1HST_HYSTERESIS_MIDDLE              CS_CMP1HST(2)                  /*!< CMP1 output middle hysteresis */
#define CS_CMP1HST_HYSTERESIS_HIGH                CS_CMP1HST(3)                  /*!< CMP1 output high hysteresis */

/* CMP1 mode */
#define CS_CMP1PM(regval)                         (BITS(12,13) & ((uint32_t)(regval) << 12U))
#define CS_CMP1PM_HIGHSPEED                       CS_CMP1PM(0)                   /*!< CMP1 mode high speed */
#define CS_CMP1PM_MIDDLESPEED                     CS_CMP1PM(1)                   /*!< CMP1 mode middle speed */
#define CS_CMP1PM_VERYLOWSPEED                    CS_CMP1PM(3)                   /*!< CMP1 mode very low speed */

/* CMP1 inverting input */
#define CS_CMP1MSEL(regval)                       (BITS(16,18) & ((uint32_t)(regval) << 16U))
#define CS_CMP1MSEL_1_4VREFINT                    CS_CMP1MSEL(0)                 /*!< CMP1 inverting input 1/4 Vrefint */
#define CS_CMP1MSEL_1_2VREFINT                    CS_CMP1MSEL(1)                 /*!< CMP1 inverting input 1/2 Vrefint */
#define CS_CMP1MSEL_3_4VREFINT                    CS_CMP1MSEL(2)                 /*!< CMP1 inverting input 3/4 Vrefint */
#define CS_CMP1MSEL_VREFINT                       CS_CMP1MSEL(3)                 /*!< CMP1 inverting input Vrefint */
#define CS_CMP1MSEL_DACOUT0                       CS_CMP1MSEL(4)                 /*!< CMP1 inverting input DAC_OUT_0 */
#define CS_CMP1MSEL_DACOUT1                       CS_CMP1MSEL(5)                 /*!< CMP1 inverting input DAC_OUT_1 */
#define CS_CMP1MSEL_INPUT1                        CS_CMP1MSEL(6)                 /*!< CMP1 inverting input PE10 */
#define CS_CMP1MSEL_INPUT2                        CS_CMP1MSEL(7)                 /*!< CMP1 inverting input PE7 */

/* CMP1_IP input*/
#define CS_CMP1PSEL(regval)                       (BIT(20) & ((uint32_t)(regval) << 20U))
#define CS_CMP1PSEL_PE9                           CS_CMP1PSEL(0)                 /*!< CMP1_IP input PE9 */
#define CS_CMP1PSEL_PE11                          CS_CMP1PSEL(1)                 /*!< CMP1_IP input PE11 */

/* CMP1 Blanking suorce */
#define CS_CMP1BLK(regval)                        (BITS(24,27) & ((uint32_t)(regval) << 24U))
#define CS_CMP1BLK_NOBLK                          CS_CMP1BLK(0)                  /*!< CMP1 no blanking */
#define CS_CMP1BLK_TIMER0_OC5                     CS_CMP1BLK(1)                  /*!< CMP1 TIMER0 OC5 selected as blanking source */
#define CS_CMP1BLK_TIMER1_OC2                     CS_CMP1BLK(2)                  /*!< CMP1 TIMER1 OC2 selected as blanking source */
#define CS_CMP1BLK_TIMER2_OC2                     CS_CMP1BLK(3)                  /*!< CMP1 TIMER2 OC2 selected as blanking source */
#define CS_CMP1BLK_TIMER2_OC3                     CS_CMP1BLK(4)                  /*!< CMP1 TIMER2 OC3 selected as blanking source */
#define CS_CMP1BLK_TIMER7_OC4                     CS_CMP1BLK(5)                  /*!< CMP1 TIMER7 OC4 selected as blanking source */
#define CS_CMP1BLK_TIMER14_OC0                    CS_CMP1BLK(6)                  /*!< CMP1 TIMER14 OC0 selected as blanking source */

/* CMP1 lock bit*/
#define CS_CMP1LK(regval)                         (BIT(31) & ((uint32_t)(regval) << 31U))
#define CS_CMP1LK_DISABLE                         CS_CMP1LK(0)                   /*!< CMP1_CS bits are read-write */
#define CS_CMP1LK_ENABLE                          CS_CMP1LK(1)                   /*!< CMP1_CS bits are read-only */

/* CMP flag definitions */
#define CMP_FLAG_COMPARE                          CMP_STAT_CMP0IF                /*!< CMP compare flag */

/* CMP interrupt definitions */
#define CMP_INT_COMPARE                           CMP0_CS_INTEN                  /*!< CMP compare interrupt */

/* CMP interrupt flag */
#define CMP_INT_FLAG_COMPARE                      CMP_STAT_CMP0IF                /*!< CMP interrupt flag */

/* function declarations */
/* initialization functions */
/* deinitialize comparator  */
void cmp_deinit(void);
/* initialize comparator mode */
void cmp_mode_init(uint32_t cmp_periph, operating_mode_enum operating_mode, inverting_input_enum inverting_input, cmp_hysteresis_enum output_hysteresis);
/* selecte the plus input for CMP */
void cmp_plus_selection(uint32_t cmp_periph, plus_input_enum plus_input);
/* initialize comparator output */
void cmp_output_init(uint32_t cmp_periph, cmp_output_inv_enum output_polarity);
/* initialize comparator blanking function */
void cmp_blanking_init(uint32_t cmp_periph, blanking_source_enum blanking_source_selection);

/* enable functions */
/* enable comparator */
void cmp_enable(uint32_t cmp_periph);
/* disable comparator */
void cmp_disable(uint32_t cmp_periph);
/* enable the window mode */
void cmp_window_enable(void);
/* disable the window mode */
void cmp_window_disable(void);
/* enable the voltage scaler */
void cmp_voltage_scaler_enable(uint32_t cmp_periph);
/* disable the voltage scaler */
void cmp_voltage_scaler_disable(uint32_t cmp_periph);
/* enable the scaler bridge */
void cmp_scaler_bridge_enable(uint32_t cmp_periph);
/* disable the scaler bridge */
void cmp_scaler_bridge_disable(uint32_t cmp_periph);
/* lock the comparator */
void cmp_lock_enable(uint32_t cmp_periph);
/* config comparator output port */
void cmp_output_mux_config(uint32_t cmp_periph, output_select_enum cmp_output_sel);

/* output functions */
/* get output level */
uint32_t cmp_output_level_get(uint32_t cmp_periph);

/* flag and interrupt functions */
/* get CMP flag */
FlagStatus cmp_flag_get(uint32_t cmp_periph, uint32_t flag);
/* clear CMP flag */
void cmp_flag_clear(uint32_t cmp_periph, uint32_t flag);
/* enable CMP interrupt */
void cmp_interrupt_enable(uint32_t cmp_periph, uint32_t interrupt);
/* disable CMP interrupt */
void cmp_interrupt_disable(uint32_t cmp_periph, uint32_t interrupt);
/* get CMP interrupt flag */
FlagStatus cmp_interrupt_flag_get(uint32_t cmp_periph, uint32_t flag);
/* clear CMP interrupt flag */
void cmp_interrupt_flag_clear(uint32_t cmp_periph, uint32_t flag);

#endif /* GD32H7XX_CMP_H */
