
#include "gh_demo_reg_lists.h"


const gh_config_reg_t g_reg_list0[] = {
    // GH_FUNC_SLOT_EN config
    {0x0016,0x001f},// FASTEST_SAMPLE_RATE_DIVIDER:31,
    {0x0020,0x2964},// FIFO_WATER_LINE:25, RG_FIFO_READ_INT_TIMER:0.4s,
    {0x0080,0x01b1},// RG_VCM_PULSEMODE:1,
    {0x0380,0xf210},// RG_SLOT_INDEX0:0 : ppg_cfg_0, RG_SLOT_INDEX1:1 : ppg_cfg_1, RG_SLOT_INDEX2:2 : ppg_cfg_2, RG_SLOT_INDEX3:15: close,
    {0x038a,0x0093},// cfg0 SAMPLE_RATE_DIVIDER:9, cfg0 FIFO_CTRL:0, cfg0 RX_EN:1,
    {0x038c,0x0093},// cfg1 SAMPLE_RATE_DIVIDER:9, cfg1 FIFO_CTRL:0, cfg1 RX_EN:1,
    {0x038e,0x0093},// cfg2 SAMPLE_RATE_DIVIDER:9, cfg2 FIFO_CTRL:0, cfg2 RX_EN:1,
    {0x03aa,0x00a8},// cfg0 BG_LEVEL:2 level, cfg0 RX0_TIA:10k, cfg0 RX1_TIA:10k, cfg0 BG CANCEL ENABLE:1, cfg0 BG_LV0_POW_ENABLE:0, cfg0 DC CANCEL ENABLE:0, cfg0 ADC_INT:315.1us,
    {0x03ac,0x00a8},// cfg1 BG_LEVEL:2 level, cfg1 RX0_TIA:10k, cfg1 RX1_TIA:10k, cfg1 BG CANCEL ENABLE:1, cfg1 BG_LV0_POW_ENABLE:0, cfg1 DC CANCEL ENABLE:0, cfg1 ADC_INT:315.1us,
    {0x03ae,0x00a8},// cfg2 BG_LEVEL:2 level, cfg2 RX0_TIA:10k, cfg2 RX1_TIA:10k, cfg2 BG CANCEL ENABLE:1, cfg2 BG_LV0_POW_ENABLE:0, cfg2 DC CANCEL ENABLE:0, cfg2 ADC_INT:315.1us,
    {0x03da,0x7500},// cfg0 *LED_SELECT:LED_0|LED_1,
    {0x03dc,0x7a00},// cfg1 *LED_SELECT:LED_2|LED_3,
    {0x03de,0x7000},// cfg2 *LED_SELECT:LED_4|LED_5,
    {0x03ea,0x2626},// cfg0 DRV0_CURRENT(mA):5.96078, cfg0 DRV1_CURRENT(mA):5.96078,
    {0x03ec,0x2626},// cfg1 DRV0_CURRENT(mA):5.96078, cfg1 DRV1_CURRENT(mA):5.96078,
    {0x03ee,0x2626},// cfg2 DRV0_CURRENT(mA):5.96078, cfg2 DRV1_CURRENT(mA):5.96078,
    {0x0484,0x0001},// RG_ADC_XP_SL:0:3.25MHz,
    {0x0500,0x000b},// FIFO_UP_OVERFLOW_EN:1, FIFO_DOWN_OVERLOWE_EN:0, FIFO_WATERLINE_EN:1, TIMER_INTERRUPT_EN:0, USER_INTERRUPT_EN:0, FRAME_DONE_EN:0, SAMPLE_RATE_ERROR_EN:0, CAP_CANCEL_DONE_EN:0, LDO_OC_EN:0, FRAME_SYNC_OUT_SAMPLE_RATE_ERR_EN:0,
    {0x0680,0x0328},// RG_SPIS_CKGT_EN:1, RG_I2CS_CKGT_EN:1, RG_I2C_TIMEOUT_EN:0,
    {0x1006,0x8402},// timestamp low 16 bit  2025-03-06 19:16:18,
    {0x1008,0x67c9},// timestamp high 16 bit,
    {0x1300,0x0100},// ppg cfg0 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1304,0x0100},// ppg cfg0 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1310,0x0080},// ppg cfg0 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1312,0x0080},// ppg cfg0 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1350,0x0100},// ppg cfg1 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1354,0x0100},// ppg cfg1 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x1360,0x0080},// ppg cfg1 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x1362,0x0080},// ppg cfg1 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x13a0,0x0100},// ppg cfg2 rx0 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x13a4,0x0100},// ppg cfg2 rx1 assign low 16bit, bit0~bit8:adt,hr,spo2,nrv,gnadr,irnadt,test1,test2,evk,
    {0x13b0,0x0080},// ppg cfg2 rx0 data type, bit0~bit8:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x13b2,0x0080},// ppg cfg2 rx1 data type, bit0~bit7:para,bg0,mix0,bg1,mix1,bg2,dre,raw,dredc,
    {0x3600,0x0003},// cfg0 *LED_SELECT:LED_0|LED_1,
    {0x3602,0x000c},// cfg1 *LED_SELECT:LED_2|LED_3,
    {0x3604,0x0030},// cfg2 *LED_SELECT:LED_4|LED_5,
};

const gh_config_reg_t g_reg_list1[] = {
};

const gh_config_reg_t g_reg_list2[] = {
};

const gh_reg_list_t g_stGhCfgListArr[] = {
    {g_reg_list0, sizeof(g_reg_list0) / sizeof(g_reg_list0[0])},
    {g_reg_list1, sizeof(g_reg_list1) / sizeof(g_reg_list1[0])},
    {g_reg_list2, sizeof(g_reg_list2) / sizeof(g_reg_list2[0])},
};

const uint8_t g_stGhCfgListNumber = sizeof(g_stGhCfgListArr) / sizeof(g_stGhCfgListArr[0]);
