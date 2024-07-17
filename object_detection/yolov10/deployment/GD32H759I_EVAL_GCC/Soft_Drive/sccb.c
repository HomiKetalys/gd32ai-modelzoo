/*!
    \file    sccb.c
    \brief   sccb driver

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

#include "sccb.h"
#include "systick.h"

/*!
    \brief      configure I2C gpio and I2C parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sccb_config(void)
{
    rcu_i2c_clock_config(IDX_I2C1,RCU_I2CSRC_IRC64MDIV);
    /* enable GPIOB and I2C clock */
    rcu_periph_clock_enable(DCI_SCCB_SCL_GPIO_CLK);
    rcu_periph_clock_enable(DCI_SCCB_SDA_GPIO_CLK);
    rcu_periph_clock_enable(DCI_SCCB_CLK);

    /* configure I2C GPIO pins AF */
    gpio_af_set(DCI_SCCB_SDA_GPIO_PORT, DCI_SCCB_SDA_AF, DCI_SCCB_SDA_PIN);
    gpio_af_set(DCI_SCCB_SCL_GPIO_PORT, DCI_SCCB_SCL_AF, DCI_SCCB_SCL_PIN);

    /* configure I2C1_SCL(PH4), I2C1_SDA(PB11) */
    gpio_mode_set(DCI_SCCB_SDA_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DCI_SCCB_SDA_PIN);
    gpio_output_options_set(DCI_SCCB_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_12MHZ, DCI_SCCB_SDA_PIN);

    gpio_mode_set(DCI_SCCB_SCL_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DCI_SCCB_SCL_PIN);
    gpio_output_options_set(DCI_SCCB_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_12MHZ, DCI_SCCB_SCL_PIN);

    /* configure I2C parameter */
    i2c_deinit(DCI_SCCB);
    i2c_timing_config(DCI_SCCB, 0, 6, 0);
    i2c_master_clock_config(DCI_SCCB, 0x26, 0x73);
    i2c_address_config(DCI_SCCB, SCCB_SLAVE_ADDRESS7, I2C_ADDFORMAT_7BITS);
    i2c_enable(DCI_SCCB);
}

/*!
    \brief      write a byte at a specific camera register
    \param[in]  reg:  camera register address
    \param[in]  data: data to be written to the specific register
    \param[out] none
    \retval     0x00 if write operation is OK.
*/
uint8_t dci_byte_write(uint8_t reg, uint8_t data)
{
    volatile uint32_t timeout = SCCB_FLAG_TIMEOUT;

    i2c_master_addressing(DCI_SCCB, OV2640_DEVICE_WRITE_ADDRESS, I2C_MASTER_TRANSMIT);
    i2c_transfer_byte_number_config(DCI_SCCB, 0x2);
    i2c_automatic_end_enable(DCI_SCCB);
    i2c_start_on_bus(DCI_SCCB);

    i2c_data_transmit(DCI_SCCB, reg);
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_TI)) {
    }
    i2c_data_transmit(DCI_SCCB, data);
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_TI)) {
    }
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_STPDET)) {
    }
    i2c_flag_clear(DCI_SCCB, I2C_FLAG_STPDET);

    /* If operation is OK, return 0 */
    return 0;
}

/*!
    \brief      read a byte from a specific camera register
    \param[in]  reg:  camera register address
    \param[out] data: read from the specific register
    \retval     0x00 if write operation is ok.
*/
uint8_t dci_byte_read(uint8_t reg, uint8_t *data)
{
    volatile uint32_t timeout = SCCB_FLAG_TIMEOUT;

    i2c_master_addressing(DCI_SCCB, OV2640_DEVICE_READ_ADDRESS, I2C_MASTER_TRANSMIT);
    i2c_transfer_byte_number_config(DCI_SCCB, 0x1);
    i2c_automatic_end_enable(DCI_SCCB);
    i2c_start_on_bus(DCI_SCCB);

    i2c_data_transmit(DCI_SCCB, reg);
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_TI)) {
    }
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_STPDET)) {
    }
    i2c_flag_clear(DCI_SCCB, I2C_FLAG_STPDET);

    i2c_master_addressing(DCI_SCCB,  OV2640_DEVICE_READ_ADDRESS, I2C_MASTER_RECEIVE);
    i2c_transfer_byte_number_config(DCI_SCCB, 0x1);
    i2c_automatic_end_enable(DCI_SCCB);
        delay_1ms(1);
    i2c_start_on_bus(DCI_SCCB);
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_RBNE)) {
    }
    *data = i2c_data_receive(DCI_SCCB);
    while(SET != i2c_flag_get(DCI_SCCB, I2C_FLAG_STPDET)) {
    }
    i2c_flag_clear(DCI_SCCB, I2C_FLAG_STPDET);

    /* If operation is OK, return 0 */
    return 0;
}
