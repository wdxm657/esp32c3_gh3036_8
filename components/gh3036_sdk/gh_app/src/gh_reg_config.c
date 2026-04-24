/**
 ****************************************************************************************
 *
 * @file    gh_reg_config.c
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing project reg config.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */
#include <stdint.h>
#include "gh_public_api.h"

/**
 * @brief default registers map
 */
const gh_config_reg_t g_reg_map[] =
{
    /* adt_hr_spo2_nadt_hrv */
    {0x0016,0x001f},// FASTEST_SAMPLE_RATE_DIVIDER:31,
    {0x0020,0x29d2},// FIFO_WATER_LINE:210, RG_FIFO_READ_INT_TIMER:0.4s,
    {0x0080,0x01b1},// RG_VCM_PULSEMODE:1,
    {0x0380,0x3210},// RG_SLOT_INDEX0:0 : ppg_cfg_0, RG_SLOT_INDEX1:1 : ppg_cfg_1, RG_SLOT_INDEX2:2 : ppg_cfg_2, RG_SLOT_INDEX3:3 : ppg_cfg_3,
    {0x0382,0xf654},// RG_SLOT_INDEX4:4 : ppg_cfg_4, RG_SLOT_INDEX5:5 : ppg_cfg_5, RG_SLOT_INDEX6:6 : ppg_cfg_6, RG_SLOT_INDEX7:15: close,
    {0x038a,0x0093},// cfg0 SAMPLE_RATE_DIVIDER:9, cfg0 FIFO_CTRL:0, cfg0 RX_EN:1,
    {0x038c,0x0273},// cfg1 SAMPLE_RATE_DIVIDER:39, cfg1 FIFO_CTRL:0, cfg1 RX_EN:1,
    {0x038e,0x0273},// cfg2 SAMPLE_RATE_DIVIDER:39, cfg2 FIFO_CTRL:0, cfg2 RX_EN:1,
    {0x0390,0x0273},// cfg3 SAMPLE_RATE_DIVIDER:39, cfg3 FIFO_CTRL:0, cfg3 RX_EN:1,
    {0x0392,0x0273},// cfg4 SAMPLE_RATE_DIVIDER:39, cfg4 FIFO_CTRL:0, cfg4 RX_EN:1,
    {0x0394,0x0273},// cfg5 SAMPLE_RATE_DIVIDER:39, cfg5 FIFO_CTRL:0, cfg5 RX_EN:1,
    {0x0396,0x0c73},// cfg6 SAMPLE_RATE_DIVIDER:199, cfg6 FIFO_CTRL:0, cfg6 RX_EN:1,
    {0x03aa,0x4463},// cfg0 RX0_TIA:250k, cfg0 RX1_TIA:250k, cfg0 BG CANCEL ENABLE:1, cfg0 BG_LV0_POW_ENABLE:0, cfg0 DC CANCEL ENABLE:0, cfg0 ADC_INT:39.4us,
    {0x03ac,0x4463},// cfg1 RX0_TIA:250k, cfg1 RX1_TIA:250k, cfg1 BG CANCEL ENABLE:1, cfg1 BG_LV0_POW_ENABLE:0, cfg1 DC CANCEL ENABLE:0, cfg1 ADC_INT:39.4us,
    {0x03ae,0x1167},// cfg2 RX0_TIA:25k, cfg2 RX1_TIA:25k, cfg2 BG CANCEL ENABLE:1, cfg2 BG_LV0_POW_ENABLE:0, cfg2 DC CANCEL ENABLE:0, cfg2 ADC_INT:157.5us,
    {0x03b0,0x1167},// cfg3 RX0_TIA:25k, cfg3 RX1_TIA:25k, cfg3 BG CANCEL ENABLE:1, cfg3 BG_LV0_POW_ENABLE:0, cfg3 DC CANCEL ENABLE:0, cfg3 ADC_INT:157.5us,
    {0x03b2,0x2260},// cfg4 RX0_TIA:50k, cfg4 RX1_TIA:50k, cfg4 BG CANCEL ENABLE:1, cfg4 BG_LV0_POW_ENABLE:0, cfg4 DC CANCEL ENABLE:0, cfg4 ADC_INT:9.85us,
    {0x03b4,0x2200},// cfg5 BG_LEVEL:0 level, cfg5 RX0_TIA:50k, cfg5 RX1_TIA:50k, cfg5 BG CANCEL ENABLE:0, cfg5 BG_LV0_POW_ENABLE:0, cfg5 DC CANCEL ENABLE:0, cfg5 ADC_INT:9.85us,
    {0x03b6,0x2240},// cfg6 RX0_TIA:50k, cfg6 RX1_TIA:50k, cfg6 BG CANCEL ENABLE:0, cfg6 BG_LV0_POW_ENABLE:0, cfg6 DC CANCEL ENABLE:0, cfg6 ADC_INT:9.85us,
    {0x03c2,0x30f0},// cfg4 RX0_BG_CANCEL_RANGE:256uA, cfg4 RX1_BG_CANCEL_RANGE:256uA, cfg4 TIA_SET_TIME:8µs,
    {0x03da,0x5d20},// cfg0 SLOT_POST_DELAY:rg_t3_set1, cfg0 *LED_SELECT:LED_4|LED_5,
    {0x03dc,0x5d20},// cfg1 SLOT_POST_DELAY:rg_t3_set1, cfg1 *LED_SELECT:LED_4|LED_5,
    {0x03de,0x5220},// cfg2 SLOT_POST_DELAY:rg_t3_set1, cfg2 *LED_SELECT:LED_7,
    {0x03e0,0x5020},// cfg3 SLOT_POST_DELAY:rg_t3_set1, cfg3 *LED_SELECT:LED_6,
    {0x03e2,0x5020},// cfg4 SLOT_POST_DELAY:rg_t3_set1, cfg4 *LED_SELECT:LED_6,
    {0x03e4,0x4020},// cfg5 SLOT_POST_DELAY:rg_t3_set1, cfg5 *LED_SELECT:,
    {0x03e6,0x5020},// cfg6 SLOT_POST_DELAY:rg_t3_set1, cfg6 *LED_SELECT:LED_6,
    {0x03ea,0x1010},// cfg0 DRV0_CURRENT(mA):10.0392, cfg0 DRV1_CURRENT(mA):10.0392,
    {0x03ec,0x1010},// cfg1 DRV0_CURRENT(mA):10.0392, cfg1 DRV1_CURRENT(mA):10.0392,
    {0x03ee,0x0020},// cfg2 DRV0_CURRENT(mA):20.0784, cfg2 DRV1_CURRENT(mA):0,
    {0x03f0,0x0020},// cfg3 DRV0_CURRENT(mA):20.0784, cfg3 DRV1_CURRENT(mA):0,
    {0x03f2,0x0010},// cfg4 DRV0_CURRENT(mA):10.0392, cfg4 DRV1_CURRENT(mA):0,
    {0x03f6,0x0010},// cfg6 DRV0_CURRENT(mA):10.0392, cfg6 DRV1_CURRENT(mA):0,
    {0x0484,0x0001},// RG_ADC_XP_SL:0:3.25MHz,
    {0x0488,0x022f},// LED_DRV0_FS:160mA, LED_DRV1_FS:160mA,
    {0x0496,0x0ccd},// RG_DRE_DC_THR:3277, RG_DRE_DC_THR_CNT:1,
    {0x049a,0x00b0},// RG_T3_SET1:80us,
    {0x0500,0x000b},// FIFO_UP_OVERFLOW_EN:1, FIFO_DOWN_OVERLOWE_EN:0, FIFO_WATERLINE_EN:1, TIMER_INTERRUPT_EN:0, USER_INTERRUPT_EN:0, FRAME_DONE_EN:0, SAMPLE_RATE_ERROR_EN:0, CAP_CANCEL_DONE_EN:0, LDO_OC_EN:0, FRAME_SYNC_OUT_SAMPLE_RATE_ERR_EN:0,
    {0x0680,0x0328},// RG_SPIS_CKGT_EN:1, RG_I2CS_CKGT_EN:1, RG_I2C_TIMEOUT_EN:0,
    {0x1000,0x0002},// work mode:2,
    {0x1006,0xa563},// timestamp low 16 bit  2025-03-13 17:29:07,
    {0x1008,0x67d2},// timestamp high 16 bit,
    {0x1120,0x0258},// *FIFO INTERVAL:600,
    {0x1282,0x0019},// *FS:25,
    {0x1284,0x0064},// *FS:100,
    {0x128c,0x0019},// *FS:25,
    {0x1292,0x0019},// *FS:25,
    {0x1300,0x0008},// ppg cfg0 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1304,0x0008},// ppg cfg0 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1310,0x0080},// ppg cfg0 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1312,0x0080},// ppg cfg0 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1350,0x0012},// ppg cfg1 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1354,0x0002},// ppg cfg1 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1360,0x0080},// ppg cfg1 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1362,0x0080},// ppg cfg1 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x13a0,0x0004},// ppg cfg2 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x13a4,0x0004},// ppg cfg2 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x13b0,0x0080},// ppg cfg2 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x13b2,0x0080},// ppg cfg2 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x13f0,0x0004},// ppg cfg3 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x13f4,0x0004},// ppg cfg3 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1400,0x0080},// ppg cfg3 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1402,0x0080},// ppg cfg3 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1440,0x0010},// ppg cfg4 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1444,0x0010},// ppg cfg4 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1450,0x0080},// ppg cfg4 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1452,0x0080},// ppg cfg4 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1490,0x0010},// ppg cfg5 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1494,0x0010},// ppg cfg5 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x14a0,0x0080},// ppg cfg5 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x14a2,0x0080},// ppg cfg5 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x14e0,0x0001},// ppg cfg6 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x14e4,0x0001},// ppg cfg6 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x14f0,0x0080},// ppg cfg6 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x14f2,0x0080},// ppg cfg6 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x3400,0x000f},// cfg0 *AGC enable, cfg1 *AGC enable, cfg2 *AGC enable, cfg3 *AGC enable, cfg4 *AGC disable, cfg5 *AGC disable, cfg6 *AGC disable,
    {0x3404,0x4556},// *GAIN_LIMIT_BG_32_UA:6, *GAIN_LIMIT_BG_64_UA:5, *GAIN_LIMIT_BG_128_UA:5, *GAIN_LIMIT_BG_256_UA:4,
    {0x3406,0x0004},// cfg2 *AGC SPO2 RED, cfg3 *AGC SPO2 disable, cfg4 *AGC SPO2 disable, cfg5 *AGC SPO2 disable, cfg6 *AGC SPO2 disable,
    {0x340a,0x0008},// cfg3 *AGC SPO2 IR, cfg4 *AGC SPO2 disable, cfg5 *AGC SPO2 disable, cfg6 *AGC SPO2 disable,
    {0x341e,0x0030},// cfg0 *AGC_DRV:drv0|drv1,
    {0x3420,0x0030},// cfg1 *AGC_DRV:drv0|drv1,
    {0x3422,0x0010},// cfg2 *AGC_DRV:drv0,
    {0x3424,0x0010},// cfg3 *AGC_DRV:drv0,
    {0x345c,0x10ef},// cfg0 *AGC_DRV_UP_LIMIT:239, cfg0 *AGC_DRV_DOWN_LIMIT:16,
    {0x345e,0x10ef},// cfg1 *AGC_DRV_UP_LIMIT:239, cfg1 *AGC_DRV_DOWN_LIMIT:16,
    {0x3460,0x20ef},// cfg2 *AGC_DRV_UP_LIMIT:239, cfg2 *AGC_DRV_DOWN_LIMIT:32,
    {0x3462,0x20ef},// cfg3 *AGC_DRV_UP_LIMIT:239, cfg3 *AGC_DRV_DOWN_LIMIT:32,
    {0x3600,0x0030},// cfg0 *LED_SELECT:LED_4|LED_5,
    {0x3602,0x0030},// cfg1 *LED_SELECT:LED_4|LED_5,
    {0x3604,0x0080},// cfg2 *LED_SELECT:LED_7,
    {0x3606,0x0040},// cfg3 *LED_SELECT:LED_6,
    {0x3608,0x0040},// cfg4 *LED_SELECT:LED_6,
    {0x360c,0x0040},// cfg6 *LED_SELECT:LED_6,
    {0x75c0,0x0002},// *ALGO_CHNL_NUM:2,
    {0x75c4,0x0100},// *ALGO_GREEN_CHNL0:0, *ALGO_GREEN_CHNL1:1,
    {0x78c0,0x0001},// *ALGO_CHNL_NUM:1,
    {0x78c4,0xff00},// *ALGO_GREEN_CHNL0:0, *ALGO_GREEN_CHNL1:255,
    {0x84c0,0x0001},// *ALGO_CHNL_NUM:1,
    {0x84cc,0xff02},// *ALGO_IR_CHNL0:2, *ALGO_IR_CHNL1:255,
    {0x84d4,0xff00},// *ALGO_RED_CHNL0:0, *ALGO_RED_CHNL1:255,
    {0x8dc0,0x0001},// *ALGO_CHNL_NUM:1,
    {0x8dc4,0xff00},// *ALGO_GREEN_CHNL0:0, *ALGO_GREEN_CHNL1:255,
    {0x8dcc,0x0301},// *ALGO_IR_CHNL0:1, *ALGO_IR_CHNL1:3,
    {0xffff,0x0001},// virtual reg,

};

const uint16_t g_reg_map_szie = sizeof(g_reg_map) / sizeof(g_reg_map[0]);

