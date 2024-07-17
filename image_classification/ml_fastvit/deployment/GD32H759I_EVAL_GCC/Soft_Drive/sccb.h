/*!
    \file    sccb.h
    \brief   definitions for sccb

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

#ifndef SCCB_H
#define SCCB_H

#include "gd32h7xx.h"

/* configure I2C1 pins: PB11 and PH4 */
#define DCI_SCCB                       I2C1
#define DCI_SCCB_CLK                   RCU_I2C1

#define DCI_SCCB_SDA_PIN               GPIO_PIN_11
#define DCI_SCCB_SDA_GPIO_PORT         GPIOB
#define DCI_SCCB_SDA_GPIO_CLK          RCU_GPIOB
#define DCI_SCCB_SDA_AF                GPIO_AF_4

#define DCI_SCCB_SCL_PIN               GPIO_PIN_4
#define DCI_SCCB_SCL_GPIO_PORT         GPIOH
#define DCI_SCCB_SCL_GPIO_CLK          RCU_GPIOH
#define DCI_SCCB_SCL_AF                GPIO_AF_4

#define SCCB_FLAG_TIMEOUT              20000000
#define OV2640_DEVICE_WRITE_ADDRESS    0x60
#define OV2640_DEVICE_READ_ADDRESS     0x61

#define SCCB_SPEED                     100000
#define SCCB_SLAVE_ADDRESS7            0xFE

/* function declarations */
void sccb_config(void);
uint8_t dci_byte_write(uint8_t reg, uint8_t data);
uint8_t dci_byte_read(uint8_t reg, uint8_t *data);

#endif /* SCCB_H */
