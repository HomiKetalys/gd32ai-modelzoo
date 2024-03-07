/*!
    \file    gd32h7xx_cmp.c
    \brief   CMP driver

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

#include "gd32h7xx_cmp.h"

#define CMP_MODE_DEFAULT                         ((uint32_t)0xFFFFCF83U)        /*!< cmp mode default */
#define CMP_OUTPUT_DEFAULT                       ((uint32_t)0xFFFFF0FFU)        /*!< cmp output default */
/*!
    \brief      deinitialize comparator 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_deinit(void)
{
    rcu_periph_reset_enable(RCU_CMPRST);
    rcu_periph_reset_disable(RCU_CMPRST);
}

/*!
    \brief      initialize comparator mode 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  operating_mode
      \arg        CMP_HIGHSPEED: high speed mode
      \arg        CMP_MIDDLESPEED: medium speed mode
      \arg        CMP_VERYLOWSPEED: very-low speed mode
    \param[in]  inverting_input 
      \arg        CMP_1_4VREFINT: VREFINT *1/4 input
      \arg        CMP_1_2VREFINT: VREFINT *1/2 input
      \arg        CMP_3_4VREFINT: VREFINT *3/4 input
      \arg        CMP_VREFINT: VREFINT input
      \arg        CMP_DACOUT0: DAC_OUT0 input
      \arg        CMP_DACOUT1: DAC_OUT1 input
      \arg        CMP_INV_INPUT1: selecte PB1 for CMP0, PE10 for CMP1 as input
      \arg        CMP_INV_INPUT2: selecte PC4 for CMP0, PE7 for CMP1 as input
    \param[in]  output_hysteresis
      \arg        CMP_HYSTERESIS_NO: output no hysteresis
      \arg        CMP_HYSTERESIS_LOW: output low hysteresis
      \arg        CMP_HYSTERESIS_MIDDLE: output middle hysteresis
      \arg        CMP_HYSTERESIS_HIGH: output high hysteresis
    \param[out] none
    \retval     none
*/
void cmp_mode_init(uint32_t cmp_periph, operating_mode_enum operating_mode, inverting_input_enum inverting_input, cmp_hysteresis_enum output_hysteresis)
{
    uint32_t CMPx_CS = 0U;
    if(CMP0 == cmp_periph){
        /* initialize comparator 0 mode */
        CMPx_CS = CMP0_CS;
        CMPx_CS &= ~(uint32_t)(CMP0_CS_PM | CMP0_CS_MISEL | CMP0_CS_HST ); 
        CMPx_CS |= (uint32_t)(CS_CMP0PM(operating_mode) | CS_CMP0MSEL(inverting_input) | CS_CMP0HST(output_hysteresis));
        CMP0_CS = CMPx_CS;
    }else if(CMP1 == cmp_periph){
        /* initialize comparator 1 mode */
        CMPx_CS = CMP1_CS;
        CMPx_CS &= ~(uint32_t)(CMP1_CS_PM | CMP1_CS_MISEL | CMP1_CS_HST );
        CMPx_CS |= (uint32_t)(CS_CMP1PM(operating_mode) | CS_CMP1MSEL(inverting_input) | CS_CMP1HST(output_hysteresis));          
        CMP1_CS = CMPx_CS;
    }else{
    }
}

/*!
    \brief      selecte the plus input for CMP
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  plus_input
      \arg        CMP_PB0_PE9: selecte PB0 for CMP0, PE9 for CMP1 as input
      \arg        CMP_PB2_PE11: selecte PB2 for CMP0, PE11 for CMP1 as input
    \param[out] none
    \retval     none
*/
void cmp_plus_selection(uint32_t cmp_periph, plus_input_enum plus_input)
{
    uint32_t CMPx_CS = 0U;
    if(CMP0 == cmp_periph){
        CMPx_CS = CMP0_CS;
        CMPx_CS &= ~(uint32_t)(CMP0_CS_PSEL);
        CMPx_CS |= CS_CMP0PSEL(plus_input);
        CMP0_CS = CMPx_CS;
    }else if(CMP1 == cmp_periph){
        CMPx_CS = CMP1_CS;
        CMPx_CS &= ~(uint32_t)(CMP1_CS_PSEL);
        CMPx_CS |= CS_CMP1PSEL(plus_input);
        CMP1_CS = CMPx_CS;
    }else{
    }
}

/*!
    \brief      initialize comparator output
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1 
    \param[in]  output_polarity 
      \arg        CMP_OUTPUT_POLARITY_INVERTED: output is inverted
      \arg        CMP_OUTPUT_POLARITY_NOINVERTED: output is not inverted
    \param[out] none
    \retval     none
*/
void cmp_output_init(uint32_t cmp_periph, cmp_output_inv_enum output_polarity)
{
    uint32_t CMPx_CS = 0U;
    if(CMP0 == cmp_periph){
        /* initialize comparator 0 output */
        CMPx_CS = CMP0_CS;
        /* output polarity */
        if(CMP_OUTPUT_POLARITY_INVERTED == output_polarity){
            CMPx_CS |= (uint32_t)CMP0_CS_PL;
        }else if(CMP_OUTPUT_POLARITY_NOINVERTED == output_polarity){
            CMPx_CS &= ~(uint32_t)CMP0_CS_PL;
        }else{
        }
        CMP0_CS = CMPx_CS;
    }else if(CMP1 == cmp_periph){
        /* initialize comparator 1 output */
        CMPx_CS = CMP1_CS;
        /* output polarity */
        if(CMP_OUTPUT_POLARITY_INVERTED == output_polarity){
            CMPx_CS |= (uint32_t)CMP1_CS_PL;
        }else if(CMP_OUTPUT_POLARITY_NOINVERTED == output_polarity){
            CMPx_CS &= ~(uint32_t)CMP1_CS_PL;
        }else{
        }
        CMP1_CS = CMPx_CS;
    }else{
    }
}

/*!
    \brief      initialize comparator blanking function
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  blanking_source_selection 
      \arg        CMP_BLANKING_NONE: output no selection
      \arg        CMP_BLANKING_TIMER0_OC5: TIMER 0 output channel5
      \arg        CMP_BLANKING_TIMER1_OC2: TIMER 1 output channel2
      \arg        CMP_BLANKING_TIMER2_OC2: TIMER 2 output channel2
      \arg        CMP_BLANKING_TIMER2_OC3: TIMER 2 output channel3
      \arg        CMP_BLANKING_TIMER7_OC4: TIMER 7 output channel4
      \arg        CMP_BLANKING_TIMER14_OC0: TIMER 14 output channel0
    \param[out] none
    \retval     none
*/
void cmp_blanking_init(uint32_t cmp_periph, blanking_source_enum blanking_source_selection)
{
    uint32_t CMPx_CS = 0U;
    if(CMP0 == cmp_periph){
        CMPx_CS = CMP0_CS;
        CMPx_CS &= ~(uint32_t)CMP0_CS_BLK;
        CMPx_CS |= (uint32_t)CS_CMP0BLK(blanking_source_selection);
        CMP0_CS = CMPx_CS;
    }else if(CMP1 == cmp_periph){
        CMPx_CS = CMP1_CS;
        CMPx_CS &= ~(uint32_t)CMP1_CS_BLK;
        CMPx_CS |= (uint32_t)CS_CMP1BLK(blanking_source_selection);
        CMP1_CS = CMPx_CS;
    }else{
    }
}

/*!
    \brief      enable comparator 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS |= (uint32_t)CMP0_CS_EN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS |= (uint32_t)CMP1_CS_EN;
    }else{
    }
}

/*!
    \brief      disable comparator 
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_disable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS &= ~(uint32_t)CMP0_CS_EN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS &= ~(uint32_t)CMP1_CS_EN;
    }else{
    }
}

/*!
    \brief      enable the window mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_window_enable(void)
{
    CMP1_CS |= (uint32_t)CS_CMP1WEN_ENABLE;
}

/*!
    \brief      disable the window mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_window_disable(void)
{
    CMP1_CS &= ~(uint32_t)CS_CMP1WEN_ENABLE;
}

/*!
    \brief      enable the voltage scaler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_voltage_scaler_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS |= (uint32_t)CMP0_CS_VSCEN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS |= (uint32_t)CMP1_CS_VSCEN;
    }else{
    }
}
/*!
    \brief      disable the voltage scaler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_voltage_scaler_disable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS &= ~(uint32_t)CMP0_CS_VSCEN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS &= ~(uint32_t)CMP1_CS_VSCEN;
    }else{
    }
}
/*! 
    \brief      enable the scaler bridge
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_scaler_bridge_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS |= (uint32_t)CMP0_CS_BEN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS |= (uint32_t)CMP1_CS_BEN;
    }else{
    }
}
/*!
    \brief      disable the scaler bridge
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_scaler_bridge_disable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        CMP0_CS &= ~(uint32_t)CMP0_CS_BEN;
    }else if(CMP1 == cmp_periph){
        CMP1_CS &= ~(uint32_t)CMP1_CS_BEN;
    }else{
    }
}

/*!
    \brief      lock the comparator
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     none
*/
void cmp_lock_enable(uint32_t cmp_periph)
{
    if(CMP0 == cmp_periph){
        /* lock CMP0 */
        CMP0_CS |= (uint32_t)CMP0_CS_LK;
    }else if(CMP1 == cmp_periph){
        /* lock CMP1 */
        CMP1_CS |= (uint32_t)CMP1_CS_LK;
    }else{
    }
}

/*!
    \brief      config comparator output port
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  cmp_output_sel
      \arg        CMP_AFSE_PA6:  select PA6 as comparator altemate function output port
      \arg        CMP_AFSE_PA8:  select PA8 as comparator altemate function output port
      \arg        CMP_AFSE_PB12:  select PB12 as comparator altemate function output port
      \arg        CMP_AFSE_PE6:  select PE6 as comparator altemate function output port
      \arg        CMP_AFSE_PE15:  select PE15 as comparator altemate function output port
      \arg        CMP_AFSE_PG2:  select PG2 as comparator altemate function output port
      \arg        CMP_AFSE_PG3:  select PG3 as comparator altemate function output port
      \arg        CMP_AFSE_PG4:  select PG4 as comparator altemate function output port
      \arg        CMP_AFSE_PK0:  select PK0 as comparator altemate function output port
      \arg        CMP_AFSE_PK1:  select PK1 as comparator altemate function output port
      \arg        CMP_AFSE_PK2:  select PK2 as comparator altemate function output port
    \param[out] none
    \retval     none
*/
void cmp_output_mux_config(uint32_t cmp_periph, output_select_enum cmp_output_sel)
{
    if(CMP0 == cmp_periph){
        CMP_SR &= ~(uint32_t)cmp_output_sel;
    }else if(CMP1 == cmp_periph){
        CMP_SR |= (uint32_t)cmp_output_sel;
    }else{
    }
}

/*!
    \brief      get output level
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[out] none
    \retval     the output level
*/
uint32_t cmp_output_level_get(uint32_t cmp_periph)
{
    uint32_t output_level = 0U;
    if(CMP0 == cmp_periph){
        /* get output level of CMP0 */
        if(CMP_STAT & CMP_STAT_CMP0OT){
            output_level = CMP_OUTPUTLEVEL_HIGH;
        }else{
            output_level = CMP_OUTPUTLEVEL_LOW;
        }
    }else if(CMP1 == cmp_periph){
        /* get output level of CMP1 */
        if(CMP_STAT & CMP_STAT_CMP1OT){
            output_level = CMP_OUTPUTLEVEL_HIGH;
        }else{
            output_level = CMP_OUTPUTLEVEL_LOW;
        }
    }else{
    }
    
    return output_level;
}

/*!
    \brief      get CMP flag
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  flag: CMP flag
      \arg        CMP_FLAG_COMPARE: CMP compare flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus cmp_flag_get(uint32_t cmp_periph, uint32_t flag)
{
    FlagStatus reval = RESET;
    
    if(CMP0 == cmp_periph){
        if(CMP_FLAG_COMPARE == flag){
            if(0U != (CMP_STAT & CMP_STAT_CMP0IF)){
                reval = SET;
            }
        }
    }else if(CMP1 == cmp_periph){
        if(CMP_FLAG_COMPARE == flag){
            if(0U != (CMP_STAT & CMP_STAT_CMP1IF)){
                reval = SET;
            }
        }
    }else{
    }
    return reval;
}

/*!
    \brief      clear CMP flag
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  flag: CMP flag
      \arg        CMP_FLAG_COMPARE: CMP compare flag
    \param[out] none
    \retval     none
*/
void cmp_flag_clear(uint32_t cmp_periph, uint32_t flag)
{
    if(CMP0 == cmp_periph){
        if(CMP_FLAG_COMPARE == flag){
            CMP_IFC |= (uint32_t)CMP_IFC_CMP0IC;
        }
    }else if(CMP1 == cmp_periph){
        if(CMP_FLAG_COMPARE == flag){
            CMP_IFC |= (uint32_t)CMP_IFC_CMP1IC;
        }
    }else{
    }
}

/*!
    \brief      enable CMP interrupt
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  interrupt: CMP interrupt
                only one parameter can be selected which is shown as below:
      \arg        CMP_INT_COMPARE: CMP compare interrupt
    \param[out] none
    \retval     none
*/
void cmp_interrupt_enable(uint32_t cmp_periph, uint32_t interrupt)
{
    if(CMP0 == cmp_periph){
        /* enable CMP0 interrupt */
        CMP0_CS |= (uint32_t)interrupt;
    }else if(CMP1 == cmp_periph){
        /* enable CMP1 interrupt */
        CMP1_CS |= (uint32_t)interrupt;
    }else{
    }
}

/*!
    \brief      disable CMP interrupt
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  interrupt: CMP interrupt
                only one parameter can be selected which is shown as below:
      \arg        CMP_INT_COMPARE: CMP compare interrupt
    \param[out] none
    \retval     none
*/
void cmp_interrupt_disable(uint32_t cmp_periph, uint32_t interrupt)
{
    if(CMP0 == cmp_periph){
        /* disable CMP0 interrupt */
        CMP0_CS &= ~(uint32_t)interrupt;
    }else if(CMP1 == cmp_periph){
        /* disable CMP1 interrupt */
        CMP1_CS &= ~(uint32_t)interrupt;
    }else{
    }
}

/*!
    \brief      get CMP interrupt flag
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  flag: CMP interrupt flag
      \arg        CMP_INT_FLAG_COMPARE: CMP compare interrupt flag
    \param[out] none
    \retval     none
*/
FlagStatus cmp_interrupt_flag_get(uint32_t cmp_periph, uint32_t flag)
{
    uint32_t intstatus = 0U, flagstatus = 0U;

    if(CMP0 == cmp_periph){
        if(CMP_INT_FLAG_COMPARE == flag){
            /* get the corresponding flag bit status */
            flagstatus = CMP_STAT & CMP_STAT_CMP0IF;
            /* get the interrupt enable bit status */
            intstatus = CMP0_CS & CMP0_CS_INTEN;
        }
    }else if(CMP1 == cmp_periph){
        if(CMP_INT_FLAG_COMPARE == flag){
            /* get the corresponding flag bit status */
            flagstatus = CMP_STAT & CMP_STAT_CMP1IF;
            /* get the interrupt enable bit status */
            intstatus = CMP1_CS & CMP1_CS_INTEN;
        }
    }else{
    }

    if((0U != flagstatus) && (0U != intstatus)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear CMP interrupt flag
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
      \arg        CMP1: comparator 1
    \param[in]  flag: CMP interrupt flag
      \arg        CMP_INT_FLAG_COMPARE: CMP compare interrupt flag
    \param[out] none
    \retval     none
*/
void cmp_interrupt_flag_clear(uint32_t cmp_periph, uint32_t flag)
{
    /* clear CMP interrupt flag */
    if(CMP0 == cmp_periph){
        if(CMP_INT_FLAG_COMPARE == flag){
            CMP_IFC |= (uint32_t)CMP_IFC_CMP0IC;
        }
    }else if(CMP1 == cmp_periph){
        if(CMP_INT_FLAG_COMPARE == flag){
            CMP_IFC |= (uint32_t)CMP_IFC_CMP1IC;
        }
    }else{
    }
}
