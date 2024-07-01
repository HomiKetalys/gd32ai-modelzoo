/*!
    \file    dci_ov2640.h
    \brief   definitions for ov2640

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

#ifndef DCI_OV2640_H
#define DCI_OV2640_H

#include "sccb.h"
#define SRAM_Address       0x20010000             /* 320*240*2 */
#define DCI_DATA_ADDRESS   ((uint32_t)&DCI_DATA)  /* 0x4C050028U */
#define DCI_TIMEOUT        20000
#define GPIO_AF_CKOUT      GPIO_AF_0

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 320

typedef struct {
    uint8_t manufacturer_id1;
    uint8_t manufacturer_id2;
    uint8_t version;
    uint8_t pid;
} ov2640_id_struct;

/* ov2640 registers definition */
#define OV2640_PID        0x0A
#define OV2640_VER        0x0B
#define OV2640_MIDH       0x1C
#define OV2640_MIDL       0x1D

/* function declarations */
void dci_config(void);
void ckout0_init(void);
uint8_t dci_ov2640_init(void);
uint8_t dci_ov2640_id_read(ov2640_id_struct *ov2640id);
uint8_t ov2640_outsize_set(uint16_t width, uint16_t height);
void DCIShowImage(void);

void ConfigDCI(void);

#endif /* DCI_OV2640_H */
