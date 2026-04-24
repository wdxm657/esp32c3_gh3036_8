/**
  ****************************************************************************************
  * @file    gh_hal_agc.c
  * @author  GHealth Driver Team
  * @brief   AGC file
  ****************************************************************************************
  * @attention
  #####Copyright (c) 2024 GOODIX
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gh_hal_agc.h"
#include "gh_hal_utils.h"
#include "gh_hal_chip.h"
#include "gh_hal_config.h"
#include "gh_hal_log.h"
#include "gh_hal_service.h"
#if GH_HAL_AGC_DRE_EN
#include "gh_hal_agc_dre.h"
#endif

#if GH_SUPPORT_SOFT_AGC
/*
 * DEFINES
 *****************************************************************************************
 */
#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    (GH_HAL_AGC_ID << 24 | (internal_err) << 8 | (interface_err))

#if GH_AGC_LOG_EN
#define DEBUG_LOG(...)                               GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                             GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                               GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void gh_agc_extremum_gain_cal(gh_hal_agc_t* p_agc, int32_t high_thd, int32_t low_thd, uint8_t gain_limit,
                                      uint8_t* new_tia_gain_code, int32_t* p_max_rawdata, int32_t* p_min_rawdata);

static void gh_agc_extremum_get(uint16_t discard_length, uint16_t analysis_cnt,
                                int32_t rawdata, int32_t* p_max_rawdata, int32_t* p_min_rawdata);

uint32_t gh_agc_reset(gh_hal_agc_t* p_agc)
{
    if (GH_NULL_PTR == p_agc)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_PTR_NULL);
    }

    if (GH_NULL_PTR == p_agc->led_drv_write || GH_NULL_PTR == p_agc->led_drv_write || GH_NULL_PTR == p_agc->p_settings)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_RESET_ERR);
    }

    gh_memset((void*)(&p_agc->agc_confg), 0, sizeof(gh_agc_confg_t));
    gh_memset((void*)(&p_agc->agc_mainchnl_config), 0, GH_AGC_SLOT_NUM * sizeof(gh_agc_mainchnl_config_t));
    gh_memset((void*)(&p_agc->agc_mainchnl_info), 0, GH_AGC_SLOT_NUM * sizeof(gh_agc_mainchnl_info_t));
    gh_memset((void*)(&p_agc->agc_ideal_adj_info), 0, GH_AGC_SLOT_NUM * sizeof(gh_agc_ideal_adj_info_t));
    gh_memset((void*)(&p_agc->gh_agc_subchnl_info), 0, GH_AGC_SUB_CHNL_NUM * sizeof(gh_agc_subchnl_info_t));

    uint8_t i;
    for (i = 0; i < GH_SLOT_NUM; i++)
    {
        p_agc->agc_chip_info.slotcfg_agc_param[i].gain0_code = PPG_TIA_GAIN_DEFAULT;
        p_agc->agc_chip_info.slotcfg_agc_param[i].gain1_code = PPG_TIA_GAIN_DEFAULT;
    }

    p_agc->agc_slot_num = GH_AGC_SLOT_NUM;
    p_agc->agc_subchnl_num = GH_AGC_SUB_CHNL_NUM;

    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

uint32_t gh_agc_init(gh_hal_agc_t* p_agc)
{
    if (GH_NULL_PTR == p_agc)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_PTR_NULL);
    }
    gh_agc_reset(p_agc);

    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

uint32_t gh_agc_reg_parse(gh_hal_agc_t* p_agc, uint16_t reg_addr, uint16_t reg_val)
{
    uint32_t ret = 0;
    uint8_t slot_cfg_num;
    uint8_t gain0_code;
    uint8_t gain1_code;
    uint8_t led_drv0_code;
    uint8_t led_drv1_code;
#if GH_HAL_AGC_DRE_EN
    uint8_t dc_cancel_code0;
    uint8_t dc_cancel_code1;
    uint8_t dre_enable;
#endif
    for (uint8_t i = 0; i < sizeof(p_agc->agc_chip_info.slotcfg_agc_param) / sizeof(p_agc->agc_chip_info.slotcfg_agc_param[0]); i++)
    {
        slot_cfg_num = i;
        gain0_code = p_agc->agc_chip_info.slotcfg_agc_param[i].gain0_code;
        gain1_code = p_agc->agc_chip_info.slotcfg_agc_param[i].gain1_code;
        led_drv0_code = p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv0_code;
        led_drv1_code = p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv1_code;
#if GH_HAL_AGC_DRE_EN
        dc_cancel_code0 = p_agc->agc_chip_info.slotcfg_agc_param[i].dc_cancel_code0;
        dc_cancel_code1 = p_agc->agc_chip_info.slotcfg_agc_param[i].dc_cancel_code1;
        dre_enable = p_agc->agc_chip_info.slotcfg_agc_param[i].dre_en;

        ret |= gh3036_agc_reg_parse(reg_addr, reg_val, slot_cfg_num,
                                &gain0_code, &gain1_code,
                                &led_drv0_code, &led_drv1_code,
                                &dc_cancel_code0, &dc_cancel_code1, &dre_enable);
#else
        ret |= gh3036_agc_reg_parse(reg_addr, reg_val, slot_cfg_num,
                                &gain0_code, &gain1_code,
                                &led_drv0_code, &led_drv1_code);
#endif

        p_agc->agc_chip_info.slotcfg_agc_param[i].gain0_code = gain0_code;
        p_agc->agc_chip_info.slotcfg_agc_param[i].gain1_code = gain1_code;
#if GH_HAL_AGC_DRE_EN
		p_agc->agc_chip_info.slotcfg_agc_param[i].dc_cancel_code0 = dc_cancel_code0;
        p_agc->agc_chip_info.slotcfg_agc_param[i].dc_cancel_code1 = dc_cancel_code1;
        p_agc->agc_chip_info.slotcfg_agc_param[i].dre_en = dre_enable;
#endif
		p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv0_code = led_drv0_code;
        p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv1_code = led_drv1_code;
    }

    return ret;
}

uint32_t gh_agc_vir_reg_config(gh_hal_agc_t* p_agc, uint16_t reg_val, uint16_t reg_idx, uint16_t byte_base)
{
    uint32_t ret = 0;
    uint16_t byte_offset;
    uint8_t byte_value;
    uint8_t byte_cnt;
    for(byte_cnt = 0; byte_cnt < 2; byte_cnt ++)
    {
        byte_value = ((reg_val >> (8 * byte_cnt)) & GH_AGC_LOW8BIT_MASK);
        byte_offset = (reg_idx * 2) + byte_cnt;
        if(byte_offset < sizeof(p_agc->agc_confg))
        {
            gh_memcpy(((uint8_t*)(&p_agc->agc_confg)) + byte_offset + byte_base, (uint8_t*)(&byte_value), 1);
        }
    }

    return ret;
}

static uint8_t gh_agc_subchl_info_set(gh_hal_agc_t* p_agc, uint8_t slotcfg, uint8_t sub_rx_en)
{
    uint8_t ret = 1;
    for (uint8_t rx_cnt = 0; rx_cnt < GH_RX_NUM; rx_cnt++)
    {
        if (sub_rx_en & (1 << rx_cnt))
        {
            ret = 0;
            for(uint8_t chnl_cnt = 0; chnl_cnt < p_agc->agc_subchnl_num; chnl_cnt++)
            {
                gh_agc_subchnl_info_t* p_gh_agc_subchnl_info = &(p_agc->gh_agc_subchnl_info[chnl_cnt]);
                if(0 ==  p_gh_agc_subchnl_info->agc_en)
                {
                    p_gh_agc_subchnl_info->agc_en = 1;
                    p_gh_agc_subchnl_info->slotcfg_rx = (slotcfg << GH_SLOTCFG_SHIFT_NUM) + rx_cnt;
                    //current gain
                    if (0 == rx_cnt)
                    {
                        p_gh_agc_subchnl_info->current_gain = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].gain0_code;
                    }
                    else if (1 == rx_cnt)
                    {
                        p_gh_agc_subchnl_info->current_gain = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].gain1_code;
                    }
#if GH_HAL_AGC_DRE_EN
                    //current dc cancel code
                    if (0 == rx_cnt)
                    {
                        p_gh_agc_subchnl_info->last_dc_cancel_code = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dc_cancel_code0;
                    }
                    else if (1 == rx_cnt)
                    {
                        p_gh_agc_subchnl_info->last_dc_cancel_code = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dc_cancel_code1;
                    }
                    // p_gh_agc_subchnl_info->dre_en = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dre_en;
#endif
                    DEBUG_LOG("[Agc] Sub chnl current Gain = %d, Slot = %d, Rx = %d\r\n",p_gh_agc_subchnl_info->current_gain,slotcfg,rx_cnt);
                    ret = 1;
                    break;
                }
            }
            if(0 == ret)
            {
                DEBUG_LOG("[Agc] Set sub chnl fail !!! Slot = %d, Rx = %d\r\n", slotcfg, rx_cnt);
                while(1);
            }
        }
    }
    return ret;
}

void gh_agc_leddrv_store(gh_hal_agc_t* p_agc, uint8_t slotcfg_idx, uint8_t drv_idx, uint8_t drv_code)
{
    if (GH_SLOT_NUM <= slotcfg_idx)
    {
        return;
    }

    if (GH_LED_DRV0_IDX == drv_idx)
    {
        p_agc->agc_chip_info.slotcfg_agc_param[slotcfg_idx].led_drv0_code = drv_code;
    }
    else if (GH_LED_DRV1_IDX == drv_idx)
    {
        p_agc->agc_chip_info.slotcfg_agc_param[slotcfg_idx].led_drv1_code = drv_code;
    }
}

void gh_agc_gain_store(gh_hal_agc_t* p_agc, uint8_t slotcfg_idx, uint8_t rx_idx, uint8_t gain_code)
{
    if (GH_SLOT_NUM < slotcfg_idx)
    {
        return;
    }

    if (GH_TIA_GIAN0_IDX == rx_idx)
    {
        p_agc->agc_chip_info.slotcfg_agc_param[slotcfg_idx].gain0_code = gain_code;
    }
    else if (GH_TIA_GIAN1_IDX == rx_idx)
    {
        p_agc->agc_chip_info.slotcfg_agc_param[slotcfg_idx].gain1_code = gain_code;
    }
}


uint32_t gh_agc_info_set(gh_hal_agc_t* p_agc)
{
    uint32_t ret = 0;
    if (0 == p_agc->agc_slot_num)
    {
        return 0;
    }

    // threshold config
    if (0 == p_agc->agc_confg.agc_gene_high_ipd)
    {
        p_agc->agc_confg.agc_gene_high_ipd = GH_AGC_GENE_H_IPD;
    }
    if (0 == p_agc->agc_confg.agc_gene_low_ipd)
    {
        p_agc->agc_confg.agc_gene_low_ipd = GH_AGC_GENE_L_IPD;
    }
    if (0 == p_agc->agc_confg.agc_spo2_high_ipd)
    {
        p_agc->agc_confg.agc_spo2_high_ipd = GH_AGC_SPO2_H_IPD;
    }
    if (0 == p_agc->agc_confg.agc_spo2_low_ipd)
    {
        p_agc->agc_confg.agc_spo2_low_ipd = GH_AGC_SPO2_L_IPD;
    }

    if (0 == p_agc->agc_confg.agc_gene_high_thld)
    {
        p_agc->agc_confg.agc_gene_high_thld = GH_AGC_GENE_H_THD;
    }
    if (0 == p_agc->agc_confg.agc_gene_low_thld)
    {
        p_agc->agc_confg.agc_gene_low_thld = GH_AGC_GENE_L_THD;
    }
    if (0 == p_agc->agc_confg.agc_spo2_high_thld)
    {
        p_agc->agc_confg.agc_spo2_high_thld = GH_AGC_SPO2_H_THD;
    }
    if (0 == p_agc->agc_confg.agc_spo2_low_thld)
    {
        p_agc->agc_confg.agc_spo2_low_thld = GH_AGC_SPO2_L_THD;
    }

    uint8_t agc_en_slot_cnt = 0;
    for (uint8_t slotcfg_idx = 0; slotcfg_idx < GH_SLOT_NUM; slotcfg_idx ++)
    {
        if ((p_agc->agc_confg.agc_adjust_en & (1 << slotcfg_idx)))
        {
            if (agc_en_slot_cnt < p_agc->agc_slot_num)
            {
                p_agc->agc_mainchnl_config[agc_en_slot_cnt].agc_drv_en = 1;
                p_agc->agc_mainchnl_config[agc_en_slot_cnt].slotcfg_rx = (slotcfg_idx << GH_SLOTCFG_SHIFT_NUM);

                /******* slot function Distinguish   START******************************************************/
                if ((p_agc->agc_confg.agc_spo2_red_slotcfg & (1 << slotcfg_idx))||(p_agc->agc_confg.agc_spo2_ir_slotcfg & (1 << slotcfg_idx)))
                {
                    p_agc->agc_mainchnl_config[agc_en_slot_cnt].spo2_chnl_flag =  1;
                    DEBUG_LOG("[NewAgc] uchNewAgcSpo2SlotCnt = %d\r\n",agc_en_slot_cnt);
                }
                /******* slot function Distinguish   END******************************************************/
                DEBUG_LOG("[NewAgc] agc_en_slot_cnt = %d\r\n",agc_en_slot_cnt);
                agc_en_slot_cnt++;
            }
            else
            {
                DEBUG_LOG("[NewAgc] error !!! main chnl info is no enough! \r\n");
                while (1);
            }
        }
    }

    for (uint8_t agc_slot_cnt = 0; agc_slot_cnt < p_agc->agc_slot_num; agc_slot_cnt++)
    {
        gh_agc_mainchnl_config_t* p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[agc_slot_cnt]);
        DEBUG_LOG("AGC_InitValue %d: Enable = %d, AgcAdjEn = %d\r\n",agc_slot_cnt,
                   p_agc_mainchnl_config->agc_drv_en, p_agc->agc_confg.agc_adjust_en);

        if (p_agc_mainchnl_config->agc_drv_en)
        {
            uint8_t slotcfg = p_agc_mainchnl_config->slotcfg_rx >> GH_SLOTCFG_SHIFT_NUM;
            DEBUG_LOG("AGC_InitFlag%d: SlotCfgX=%d, Vaule = %d\r\n",agc_slot_cnt, slotcfg, p_agc_mainchnl_config->reg_backed_flag);

            uint8_t bg_cancel_range = 0;
            uint8_t gain_limit;
            uint16_t sample_rate;

            //bak reg
            p_agc_mainchnl_config->agc_drv_en = p_agc->agc_confg.agc_scr_drv_slotcfg[slotcfg].agc_drv_cfg;
            p_agc_mainchnl_config->agc_src = p_agc->agc_confg.agc_scr_drv_slotcfg[slotcfg].agc_src_cfg;
            p_agc_mainchnl_config->slotcfg_rx = (slotcfg << GH_SLOTCFG_SHIFT_NUM) + p_agc_mainchnl_config->agc_src;
            p_agc_mainchnl_config->max_led_drv = p_agc->agc_confg.agc_drv_limit_slotcfg[slotcfg].agc_drv_up_limit;
            p_agc_mainchnl_config->min_led_drv = p_agc->agc_confg.agc_drv_limit_slotcfg[slotcfg].agc_drv_down_limit;

            if (GH_TIA_GIAN0_IDX == p_agc_mainchnl_config->agc_src)
            {
                p_agc_mainchnl_config->current_gain = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].gain0_code;
            }
            else if (GH_TIA_GIAN1_IDX == p_agc_mainchnl_config->agc_src)
            {
                p_agc_mainchnl_config->current_gain = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].gain1_code;
            }
#if GH_HAL_AGC_DRE_EN
            if (GH_AGC_DC0_IDX == p_agc_mainchnl_config->agc_src)
            {
                p_agc_mainchnl_config->dc_cancel_code = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dc_cancel_code0;
            }
            else if (GH_AGC_DC1_IDX == p_agc_mainchnl_config->agc_src)
            {
                p_agc_mainchnl_config->dc_cancel_code = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dc_cancel_code1;
            }
            p_agc_mainchnl_config->dre_en = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dre_en;
#endif
            if (p_agc_mainchnl_config->agc_drv_en & 0x02)
            {
                p_agc_mainchnl_config->current_drv = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].led_drv1_code;
            }
            else if(p_agc_mainchnl_config->agc_drv_en & 0x01)
            {
                p_agc_mainchnl_config->current_drv = p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].led_drv0_code;
            }
            else if(0 == p_agc_mainchnl_config->agc_drv_en)
            {
                DEBUG_LOG("Error config no agc source !!!\n");
                return RETURN_VALUE_ASSEMBLY(0, GH_AGC_CONFIG_ERR);
            }

            if(0 == bg_cancel_range)
            {
                gain_limit = p_agc->agc_confg.agc_gain_limit.gain_limit_bg32ua;
            }
            else if(1 == bg_cancel_range)
            {
                gain_limit = p_agc->agc_confg.agc_gain_limit.gain_limit_bg64ua;
            }
            else if(2 == bg_cancel_range)
            {
                gain_limit = p_agc->agc_confg.agc_gain_limit.gain_limit_bg128ua;
            }
            else if(3 == bg_cancel_range)
            {
                gain_limit = p_agc->agc_confg.agc_gain_limit.gain_limit_bg256ua;
            }

            DEBUG_LOG("[NewAgc] gain_limit = %d, slotcfg = %d\r\n",gain_limit, slotcfg);
            if (GH_GAIN_GEAR <= gain_limit)
            {
                gain_limit = (GH_GAIN_GEAR - 1);
            }
            p_agc_mainchnl_config->agc_gain_limit = gain_limit;

            uint8_t ppg_rx_en = p_agc->p_settings->ppg_cfg_param[slotcfg].rx_en;
            ppg_rx_en &= (~(1 << p_agc_mainchnl_config->agc_src));//clear main channel RX enable bit

            uint16_t fastest_sample_rate =  p_agc->p_settings->global_cfg_param.fastest_sample_rate;
            uint16_t sr_multiplier =  p_agc->p_settings->ppg_cfg_param[slotcfg].multiplier;
            sample_rate = GH_SYS_SAMPLE_RATE/(fastest_sample_rate + 1)/(sr_multiplier + 1);
            p_agc->agc_mainchnl_info[agc_slot_cnt].sample_rate = sample_rate;
            //set sub channel slot
#if GH_AGC_SUB_CHNL_NUM > 0
            gh_agc_subchl_info_set(p_agc, slotcfg, ppg_rx_en);
#endif
        }
    }

    return ret;
}

uint32_t gh_agc_config(gh_hal_agc_t* p_agc, uint16_t reg_addr, uint16_t reg_val)
{
    uint16_t reg_idx;
    uint16_t byte_base;

    if ( reg_addr < GH_VIRTURAL_REG_BEGIN_ADDR)
    {
        gh_agc_reg_parse(p_agc,reg_addr, reg_val);
    }
    else
    {
        switch (reg_addr)
        {
        case GH_AGC_REG_EN_LOW_ADRS:
        case GH_AGC_REG_EN_HIGH_ADRS:
        case GH_AGC_REG_GAIN_LIMIT_ADRS:
        case GH_AGC_REG_SOP2_RED_LOW_ADRS:
        case GH_AGC_REG_SOP2_RED_HIGH_ADRS:
        case GH_AGC_REG_SOP2_IR_LOW_ADRS:
        case GH_AGC_REG_SOP2_IR_HIGH_ADRS:
        case GH_AGC_REG_GENE_HIGH_IPD_ADRS:
        case GH_AGC_REG_GENE_LOW_IPD_ADRS:
        case GH_AGC_REG_SPO2_HIGH_IPD_ADRS:
        case GH_AGC_REG_SPO2_LOW_IPD_ADRS:
        case GH_AGC_REG_GENE_HIGH_THLD_ADRS:
        case GH_AGC_REG_GENE_LOW_THLD_ADRS:
        case GH_AGC_REG_SPO2_HIGH_THLD_ADRS:
        case GH_AGC_REG_SPO2_LOW_THLD_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG0_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG1_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG2_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG3_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG4_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG5_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG6_ADRS:
        case GH_AGC_REG_SCR_DRV_SLOTCFG7_ADRS:
            reg_idx = (reg_addr - GH_AGC_REG_BASE_ADRS) / GH_REG_ADDR_SIZE;
            gh_agc_vir_reg_config(p_agc, reg_val, reg_idx, 0);
            break;
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG0_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG1_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG2_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG3_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG4_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG5_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG6_ADRS:
        case GH_AGC_REG_DRV_LIMIT_SLOTCFG7_ADRS:
            reg_idx = (reg_addr - GH_AGC_REG_DRV_LIMIT_SLOTCFG0_ADRS) / GH_REG_ADDR_SIZE;
            byte_base = (GH_AGC_REG_SCR_DRV_SLOTCFG7_ADRS + GH_REG_ADDR_SIZE - GH_AGC_REG_BASE_ADRS);
            gh_agc_vir_reg_config(p_agc, reg_val, reg_idx, byte_base);
            break;
        default: // do nothing
            break;
        }
    }

    if (GH_VIRTURAL_REG_END_ADDR == reg_addr)
    {
        gh_agc_info_set(p_agc);
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

static uint8_t gh_agc_subchnl_slot_get(gh_hal_agc_t* p_agc, uint8_t slotcfg)
{
    for (uint8_t agc_slot_cnt = 0; agc_slot_cnt < p_agc->agc_subchnl_num; agc_slot_cnt++)
    {
        gh_agc_mainchnl_config_t* p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[agc_slot_cnt]);
        if (p_agc_mainchnl_config->agc_drv_en)
        {
            if (slotcfg == (p_agc_mainchnl_config->slotcfg_rx >> GH_SLOTCFG_SHIFT_NUM))
            {
                return agc_slot_cnt;
            }
        }
    }
    return 0;
}

static int32_t gh_med_select(uint32_t data_1st, uint32_t data_2nd, uint32_t data_3rd)
{
    if((data_1st >= data_2nd)&&(data_1st <= data_3rd))
    {
        return data_1st;
    }
    if((data_1st >= data_3rd)&&(data_1st <= data_2nd))
    {
        return data_1st;
    }
    if((data_2nd >= data_1st)&&(data_2nd <= data_3rd))
    {
        return data_2nd;
    }
    if((data_2nd >= data_3rd)&&(data_2nd <= data_1st))
    {
        return data_2nd;
    }
    return data_3rd;
}

static void gh_agc_thd_get(gh_hal_agc_t* p_agc, uint8_t spo2_chnl_flag, int32_t* p_high_thd, int32_t* p_low_thd)
{
   if (spo2_chnl_flag) // is  spo2 slot
   {
       *p_high_thd = p_agc->agc_confg.agc_spo2_high_thld * GH_AGC_THD24_FACTOR;
       *p_low_thd = p_agc->agc_confg.agc_spo2_low_thld * GH_AGC_THD24_FACTOR;
   }
   else
   {
       *p_high_thd = p_agc->agc_confg.agc_gene_high_thld * GH_AGC_THD24_FACTOR;
       *p_low_thd = p_agc->agc_confg.agc_gene_low_thld * GH_AGC_THD24_FACTOR;
   }
}

static void gh_agc_extremum_get(uint16_t discard_length, uint16_t analysis_cnt,
                                int32_t rawdata, int32_t* p_max_rawdata, int32_t* p_min_rawdata)
{
    if (discard_length >= analysis_cnt)
    {
        (*p_max_rawdata) =  GH_MIN_RAWDATA;
        (*p_min_rawdata) =  GH_MAX_RAWDATA;
    }
    else
    {
        (*p_max_rawdata) = ((rawdata > (*p_max_rawdata)) ? rawdata : (*p_max_rawdata)); // Max of  rawdata
        (*p_min_rawdata) = ((rawdata < (*p_min_rawdata)) ? rawdata : (*p_min_rawdata)); // Min of  rawdata
    }
}

static void gh_agc_satflag_set(int32_t high_thd, int32_t rawdata, uint16_t* p_sat_checked_cnt, uint16_t* p_sat_cnt,
                               uint8_t* p_satflag, uint16_t sample_rate)
{
    uint16_t max_sat_check_num = GH_AGC_SAT_SEC * sample_rate; //5 seconds window
    (*p_sat_checked_cnt)++;
    if (rawdata > high_thd)
    {
        (*p_sat_cnt)++;
    }
    //// as soon as posible set the Saturation flag
    if ((*p_sat_cnt) > (max_sat_check_num >> 3)) // about 12.5% of all checked data
    {
        (*p_satflag) = 1;
        (*p_sat_cnt) = 0;
    }

    if ((*p_sat_checked_cnt) >= max_sat_check_num)
    {
        (*p_sat_cnt) = 0;
        (*p_sat_checked_cnt) = 0;
    }
}

#if GH_AGC_SUB_CHNL_NUM > 0
static void gh_agc_sub_chnl_process(gh_hal_agc_t* p_agc,  gh_data_t *p_gh_data, uint16_t data_len)
{
    for (uint8_t chnl_cnt = 0; chnl_cnt < p_agc->agc_subchnl_num; chnl_cnt++)
    {
        uint16_t discard_length;
        uint16_t analyse_len;
        uint8_t  slotcfg;
        uint8_t  rx_idx;
        int32_t high_thd;
        int32_t low_thd;

        gh_agc_subchnl_info_t *p_gh_agc_subchnl_info = &(p_agc->gh_agc_subchnl_info[chnl_cnt]);
        slotcfg = p_gh_agc_subchnl_info->slotcfg_rx >> GH_SLOTCFG_SHIFT_NUM;
        rx_idx = (p_gh_agc_subchnl_info->slotcfg_rx & 0x03);
        uint8_t agc_slot_cnt = gh_agc_subchnl_slot_get(p_agc, slotcfg);
        uint8_t gain_limit = p_agc->agc_mainchnl_config[agc_slot_cnt].agc_gain_limit;
        uint16_t sample_rate = p_agc->agc_mainchnl_info[agc_slot_cnt].sample_rate;
        discard_length = (sample_rate) / GH_AGC_DROP_FACTOR;    // Fs/5
        analyse_len = discard_length + sample_rate * GH_AGC_SUB_CHNNL_ANAYLSIS_SEC;

        uint8_t spo2_chnl_flag = p_agc->agc_mainchnl_config[agc_slot_cnt].spo2_chnl_flag;
        gh_agc_thd_get(p_agc, spo2_chnl_flag, &high_thd, &low_thd);

        if (p_gh_agc_subchnl_info->agc_en)
        {
#if GH_HAL_AGC_DRE_EN
            if (p_agc->agc_chip_info.slotcfg_agc_param[slotcfg].dre_en)
            {
                DEBUG_LOG("[AGC]: slot %d DRE enable, rx %d sub agc process return.\r\n", slotcfg, rx_idx);
                continue;
            }
            DEBUG_LOG("[AGC]: slot %d rx %d sub agc process start.\r\n", slotcfg, rx_idx);
#endif

            uint8_t  current_gain = p_gh_agc_subchnl_info->current_gain;
            uint8_t  new_tia_gain_code = current_gain;
            DEBUG_LOG("Subchnl infomaton: ChnlCnt = %d, SlotIdx = %d, AGCSlotCnt = %d, rx = %d\r\n",
                      chnl_cnt, slotcfg, agc_slot_cnt, rx_idx);
            for (uint16_t data_idx = 0; data_idx < data_len; data_idx++)
            {
                if (GH_PPG_DATA != p_gh_data[data_idx].data_channel.data_type)
                {
                    continue;
                }

                gh_hal_ppg_data_t ppg_data = p_gh_data[data_idx].ppg_data;
                uint8_t slot_cfg_id = ppg_data.data_channel.channel_ppg.slot_cfg_id;
                uint8_t rx_id = ppg_data.data_channel.channel_ppg.rx_id;
                uint8_t slotcfg_rx = ((slot_cfg_id << GH_SLOTCFG_SHIFT_NUM) + rx_id);

                if (slotcfg_rx == p_gh_agc_subchnl_info->slotcfg_rx)
                {
                    int32_t rawdata = ppg_data.rawdata;

                    if (discard_length == p_gh_agc_subchnl_info->analysis_cnt)//init
                    {
                        p_gh_agc_subchnl_info->last2_rwdata = rawdata;
                        p_gh_agc_subchnl_info->last_rwdata = rawdata;
                    }

                    if( p_gh_agc_subchnl_info->analysis_cnt < analyse_len )//analysising
                    {
                        int32_t tem_rawdata = gh_med_select(p_gh_agc_subchnl_info->last2_rwdata,
                                                            p_gh_agc_subchnl_info->last_rwdata, rawdata);
                        p_gh_agc_subchnl_info->last2_rwdata = p_gh_agc_subchnl_info->last_rwdata;
                        p_gh_agc_subchnl_info->last_rwdata = rawdata;
                        gh_agc_extremum_get(discard_length, p_gh_agc_subchnl_info->analysis_cnt, tem_rawdata,
                             &(p_gh_agc_subchnl_info->max_rawdata), &(p_gh_agc_subchnl_info->min_rawdata) );
                        p_gh_agc_subchnl_info->analysis_cnt ++;
                    }
                    //// Complete adjust as soon as possible
                    if (p_gh_agc_subchnl_info->analysis_cnt == analyse_len ) //anlysis finished
                    {
                        p_gh_agc_subchnl_info->analysis_cnt = 0;
                        gh_agc_extremum_gain_cal(p_agc, high_thd, low_thd, gain_limit, &new_tia_gain_code,
                                     &(p_gh_agc_subchnl_info->max_rawdata), &(p_gh_agc_subchnl_info->min_rawdata));
                        DEBUG_LOG("SubChnl_adj:SlotAdcNo=%d, H_Thd=%d, L_Thd=%d, CurGain=%d,\
                                  NewGain=%d, Max=%d, Min=%d \r\n", p_gh_agc_subchnl_info->slotcfg_rx,
                                  high_thd, low_thd, current_gain, new_tia_gain_code, p_gh_agc_subchnl_info->max_rawdata,
                                  p_gh_agc_subchnl_info->min_rawdata);
                        if (current_gain != new_tia_gain_code)
                        {
                            p_agc->tia_gain_write(slotcfg, rx_idx, new_tia_gain_code);
                            gh_agc_gain_store(p_agc, slotcfg, rx_idx, new_tia_gain_code);
                            p_gh_agc_subchnl_info->current_gain = new_tia_gain_code;
                        }

                        break;//Data after completing a mean calculation is discarded
                    }
                }
            }
        }
    }
}
#endif


static int32_t gh_agc_ipd_calc(int32_t rawdata, uint16_t gain_value) //
{
     int32_t mean_ipd;

     //Rawdata stretch  (The principle of magnification: the mean divided by the ideal target value is the data reduction factor )
     if (rawdata < GH_AGC_RAWDATA_LEV0)
     {
     }
     else if (rawdata < GH_AGC_RAWDATA_LEV1)
     {
         rawdata = GH_AGC_RAWDATA_LEV0 + ((rawdata - GH_AGC_RAWDATA_LEV0) << 1);
     }
     else
     {
         // The more saturated the data, the bigger the stretch, the key is that the data is really saturated
         rawdata = GH_AGC_RAWDATA_LEV0 + ((rawdata - GH_AGC_RAWDATA_LEV0) << 2);
     }
     mean_ipd = (int64_t)(rawdata) * GH_VOLT_REF_UA / ((int64_t)gain_value * GH_MAX_RAWDATA)
                * GH_AGC_IPD_NA_FACTOR;//Ipd unit:nA

     if (mean_ipd == 0)
     {
         mean_ipd = 1; //Avoid the division is 0
     }

     return mean_ipd;
}


/****************************************************************
* Description: calculate key value of main channel
* Input:  rawdata
* Return: none
******************************************************************/
static void gh_agc_mainchnl_info_cal(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len,
                                     int32_t* p_mean_ipd, uint32_t *p_agc_falg)
{
    for (uint8_t agc_slot_cnt = 0; agc_slot_cnt < p_agc->agc_slot_num; agc_slot_cnt ++)
    {
        gh_agc_mainchnl_config_t *p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[agc_slot_cnt]);

        if (p_agc_mainchnl_config->agc_drv_en)// is  enabled  slot
        {

#if GH_HAL_AGC_DRE_EN
            if (p_agc_mainchnl_config->dre_en)
            {
                DEBUG_LOG("[AGC]: agc slot %d DRE enable, main agc process return.\r\n", agc_slot_cnt);
                continue;
            }
            DEBUG_LOG("[AGC]: agc slot %d mian agc process start.\r\n", agc_slot_cnt);
#endif
            int32_t high_thd;
            int32_t high_ipd_thd;
            int32_t low_ipd_thd;
            int32_t ideal_rawdata;
            uint16_t analyse_len;
            uint16_t discard_len = 0;
            uint8_t gain_code = p_agc_mainchnl_config->current_gain;
            uint16_t gain_value = p_agc->agc_chip_info.p_gain_value[gain_code];//uint: 0.1 kohm
            if (p_agc_mainchnl_config->spo2_chnl_flag)// is  spo2 slot
            {
                high_thd = p_agc->agc_confg.agc_spo2_high_thld * GH_AGC_THD24_FACTOR;
                high_ipd_thd = p_agc->agc_confg.agc_spo2_high_ipd * GH_AGC_IPD_NA_FACTOR;
                low_ipd_thd = p_agc->agc_confg.agc_spo2_low_ipd * GH_AGC_IPD_NA_FACTOR;
                ideal_rawdata= GH_AGC_SPO2_IDEAL_VALUE * GH_AGC_THD24_FACTOR;
            }
            else
            {
                high_thd = p_agc->agc_confg.agc_gene_high_thld * GH_AGC_THD24_FACTOR;
                high_ipd_thd = p_agc->agc_confg.agc_gene_high_ipd * GH_AGC_IPD_NA_FACTOR;
                low_ipd_thd = p_agc->agc_confg.agc_gene_low_ipd * GH_AGC_IPD_NA_FACTOR;
                ideal_rawdata= GH_AGC_GENE_IDEAL_VALUE * GH_AGC_THD24_FACTOR;
            }

            gh_agc_mainchnl_info_t *p_agc_mainchnl_info = &(p_agc->agc_mainchnl_info[agc_slot_cnt]);
            gh_agc_ideal_adj_info_t *p_agc_ideal_adj_info = &(p_agc->agc_ideal_adj_info[agc_slot_cnt]);
            uint16_t sample_rate =  p_agc_mainchnl_info->sample_rate;
            analyse_len = sample_rate;

            //have agc motion at last fifo read period or first init
            if (p_agc_mainchnl_info->agc_trigger)
            {
                discard_len = sample_rate / GH_AGC_DROP_FACTOR;// drop the first 1/5 data
                //// After AGC, the previous data is drop to calcluate precise mean
                if (!p_agc_mainchnl_info->drop_flag)
                {  // The frame count is only cleared for the first time to prevent data num cannot to be SampleRate
                    p_agc_mainchnl_info->analysis_cnt = 0;
                    p_agc_mainchnl_info->drop_flag = 1;
                    p_agc_mainchnl_info->sat_chceked_cnt = 0;
                    p_agc_mainchnl_info->sat_cnt = 0;
                }
            }

            //The data fluctuates when initializing the early stage
            if (!p_agc_ideal_adj_info->inite_flag)
            {
                discard_len = sample_rate / GH_AGC_DROP_FACTOR;// drop the first 1/5 data
            }

            for (uint16_t data_idx = 0; data_idx < data_len; data_idx++)
            {
                if (GH_PPG_DATA != p_gh_data[data_idx].data_channel.data_type)
                {
                    continue;
                }

                gh_hal_ppg_data_t ppg_data = p_gh_data[data_idx].ppg_data;
                uint8_t slot_cfg_id = ppg_data.data_channel.channel_ppg.slot_cfg_id;
                uint8_t rx_id = ppg_data.data_channel.channel_ppg.rx_id;
                uint8_t slotcfg_rx = ((slot_cfg_id << GH_SLOTCFG_SHIFT_NUM) + rx_id);
                if (p_agc_mainchnl_config->slotcfg_rx == slotcfg_rx)
                {
                    int32_t rawdata = ppg_data.rawdata;
                    // begin Saturation judge
                    if (discard_len <= p_agc_mainchnl_info->analysis_cnt)
                    {
                       gh_agc_satflag_set(high_thd, rawdata, &(p_agc_mainchnl_info->sat_chceked_cnt),\
                            &(p_agc_mainchnl_info->sat_cnt), &(p_agc_mainchnl_info->sat_flag), sample_rate);
//                       DEBUG_LOG("Sat_Ipd:SlotAdcNo=%d,Satflag=%d,AnaCnt=%d\r\n", slotcfg_rx,
//                                  p_agc_mainchnl_info->sat_flag, p_agc_mainchnl_info->analysis_cnt);
                    }

                    //// begin mean calculate
                    if (discard_len == p_agc_mainchnl_info->analysis_cnt)//init
                    {
                        p_agc_mainchnl_info->rawdata_sum = 0;
                        p_agc_mainchnl_info->last2_rwdata = rawdata;
                        p_agc_mainchnl_info->last_rwdata = rawdata;
                    }

                    if (p_agc_mainchnl_info->analysis_cnt < analyse_len)//analysising
                    {
                        int32_t med_rawdata = gh_med_select(p_agc_mainchnl_info->last2_rwdata,
                                                            p_agc_mainchnl_info->last_rwdata, rawdata);
                        p_agc_mainchnl_info->last2_rwdata = p_agc_mainchnl_info->last_rwdata;
                        p_agc_mainchnl_info->last_rwdata = rawdata;
                        gh_agc_extremum_get(discard_len, p_agc_mainchnl_info->analysis_cnt, med_rawdata,
                            &(p_agc_mainchnl_info->max_rawdata), &(p_agc_mainchnl_info->min_rawdata));
                        p_agc_mainchnl_info->rawdata_sum += med_rawdata;
                        p_agc_mainchnl_info->analysis_cnt++;
                        DEBUG_LOG("slotCnt=%d, SlotAdc=%d, Raw=%d, AnaCnt=%d, SumAdjust=%lld\r\n",
                                   agc_slot_cnt, slotcfg_rx, med_rawdata,
                                   p_agc_mainchnl_info->analysis_cnt, p_agc_mainchnl_info->rawdata_sum);
                    }
                    //Anlysis finished calculate mean and judge whether AGC (Complete AGC as soon as possible)
                    if (p_agc_mainchnl_info->analysis_cnt == analyse_len)
                    {
                         //mean value to calculate gain and current
                         if (analyse_len > discard_len)
                         {
                            p_agc_mainchnl_info->mean_rawdata = p_agc_mainchnl_info->rawdata_sum / (analyse_len - discard_len);
                         }
                         else//The normal case will not be executed, in case the division by 0 in abnormal case
                         {
                           p_agc_mainchnl_info->mean_rawdata = rawdata;
                         }
                         p_mean_ipd[agc_slot_cnt] = gh_agc_ipd_calc(p_agc_mainchnl_info->mean_rawdata, gain_value);

                         p_agc_mainchnl_info->analysis_cnt = 0;
                         p_agc_mainchnl_info->agc_trigger = 0;//// cleer after mean calculate finished
                         //// end Ipd calculate

                         if (p_agc_mainchnl_info->sat_flag)
                         {
                            *p_agc_falg |= (1 << agc_slot_cnt);  //need adjust gain and current
                         }

                         ///////////////////// whether AGC  and adjust the mean value to calculate gain and current
                         if ((p_mean_ipd[agc_slot_cnt] < low_ipd_thd) || (p_mean_ipd[agc_slot_cnt] > high_ipd_thd))
                         {
                             *p_agc_falg |= (1 << agc_slot_cnt);//need adjust gain and current
                         }

                         //////////////begin ideal value adjust
                         if ((!((*p_agc_falg) & (1 << agc_slot_cnt))) && (!(p_agc_mainchnl_info->sat_flag)))//if the channel not adjust and satuartion
                         {
                             //// The deviation of the rawdata and   Data fluctuation is less than a certain value
                             uint8_t  conver_level = GH_AGC_CONVERGENCE_LEVEL;
                             if (((p_agc_mainchnl_info->mean_rawdata * conver_level < (ideal_rawdata * (conver_level-1)))
                                || (p_agc_mainchnl_info->mean_rawdata * conver_level > (ideal_rawdata * (conver_level+1))))
                                 && (p_agc_mainchnl_info->max_rawdata < (p_agc_mainchnl_info->min_rawdata << 1)))
                             {
                                 if (!p_agc_ideal_adj_info->inite_flag)//Inite First time
                                 {
                                     p_agc_ideal_adj_info->inite_flag = 1;
                                     *p_agc_falg |= (1 << agc_slot_cnt); //need adjust gain and current
                                 }
                                 else
                                 {
                                     p_agc_ideal_adj_info->window_cnt++;
                                     if (p_agc_ideal_adj_info->window_cnt >= GH_AGC_ANALYSE_SEC)
                                     {
                                         *p_agc_falg |= (1 << agc_slot_cnt);  //need adjust gain and current
                                         p_agc_ideal_adj_info->window_cnt  = 0;
                                     }
                                 }
                             }
                             else
                             {
                                  p_agc_ideal_adj_info->window_cnt  = 0;
                             }
                         }
                         else
                         {
                              p_agc_ideal_adj_info->window_cnt  = 0;
                         }

                         p_agc_ideal_adj_info->inite_flag = 1;  //// Must be set to 1 after the first second!!!
                         DEBUG_LOG("Mean cal: Slot_num = %d, Rawmean = %d, gain = %d, Ipd = %d\r\n",agc_slot_cnt,p_agc_mainchnl_info->mean_rawdata, gain_value,p_mean_ipd[agc_slot_cnt]);
                         DEBUG_LOG("Sat flag: SatCnt = %d, SatCheck = %d, Satflag = %d\r\n", p_agc_mainchnl_info->sat_cnt, p_agc_mainchnl_info->sat_chceked_cnt, p_agc_mainchnl_info->sat_flag);
                         DEBUG_LOG("Adj flag: Slot_num = %d, Ipd = %d, Adjflag = %d\r\n", agc_slot_cnt, p_mean_ipd[agc_slot_cnt], *p_agc_falg);
                         DEBUG_LOG("Ideal adjust: Slot_num = %d, WindowCnt = %d\r\n", agc_slot_cnt, p_agc_ideal_adj_info->window_cnt);
                         break; //   Data after completing a mean calculation is discarded
                         //end ideal value adjust
                   }
                }
             }
        }
    }
}

/**
 * @fn     gh_agc_led_drv_calc(int32_t mean_ipd, int32_t ideal_ipd, uint8_t current_drv, uint8_t min_led_drv_code, uint8_t max_led_drv_code, uint8_t* p_led_drv_code )
 *
 * @brief  Calculate DrvCurrent
 *
 * @attention
 *
 * @param[in]   mean_ipd, ideal_ipd, current_drv, min_led_drv_code, max_led_drv_code
 * @param[out]  p_led_drv_code
 *
 * @return
 */
static void gh_agc_led_drv_calc(int32_t mean_ipd, int32_t ideal_ipd, uint8_t current_drv, uint8_t min_led_drv_code, uint8_t max_led_drv_code, uint8_t* p_led_drv_code)
{
    int32_t temp_led_drv = current_drv * (ideal_ipd) / (mean_ipd);
    if ( temp_led_drv < min_led_drv_code)
    {
        temp_led_drv = min_led_drv_code;
    }
    else if (temp_led_drv > max_led_drv_code)
    {
       temp_led_drv = max_led_drv_code;
    }

    *p_led_drv_code = temp_led_drv;
}

/**
 * @fn     void gh_agc_extremum_cal( uint8_t current_drv, uint8_t temp_current_drv,  int32_t* p_max_rawdata, int32_t* p_min_rawdata )
 *
 * @brief  Calculate extremum after drv current change
 *
 * @attention
 *
 * @param[in]    current_drv, temp_current_drv
 * @param[out]   p_max_rawdata, p_min_rawdata
 *
 * @return
 */
static void gh_agc_extremum_cal(uint8_t current_drv, uint8_t temp_current_drv,  int32_t* p_max_rawdata, int32_t* p_min_rawdata)
{
     if ((*p_min_rawdata) == GH_MAX_RAWDATA)//
     {
        return;
     }

     if (temp_current_drv != current_drv)
     {
         if (0 == current_drv)
         {
             DEBUG_LOG(" Warning: Default current is zero!! \r\n");
             return;
         }

         *p_max_rawdata = temp_current_drv * (*p_max_rawdata) / current_drv;
         *p_min_rawdata = temp_current_drv * (*p_min_rawdata) / current_drv;
     }
}

/**
 * @fn   void gh_agc_extremum_gain_cal(gh_hal_agc_t* p_agc,  int32_t high_thd, int32_t low_thd, uint8_t* new_tia_gain_code, int32_t* p_max_rawdata, int32_t* p_min_rawdata  )
 *
 * @brief    adjust gain by extremum
 *
 * @attention
 *
 * @param[in]    high_thd low_thd, p_max_rawdata, p_min_rawdata
 * @param[out]   uint8_t* new_tia_gain_code,
 *
 * @return
 */
static void gh_agc_extremum_gain_cal(gh_hal_agc_t* p_agc, int32_t high_thd, int32_t low_thd, uint8_t gain_limit,
                                      uint8_t* new_tia_gain_code, int32_t* p_max_rawdata, int32_t* p_min_rawdata)
{
    int32_t max_rawdata =  *p_max_rawdata;
    int32_t min_rawdata =  *p_min_rawdata;

    if ( min_rawdata == GH_MAX_RAWDATA)
    {
       return;
    }
    uint32_t  times = 1 * GH_AGC_TIMES_FACTOR;
    if ((*new_tia_gain_code) < gain_limit)
    {
        times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code) + 1])
                 / p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code)];
    }

    int32_t temp_max_rawdata = (int64_t)max_rawdata * times / GH_AGC_TIMES_FACTOR;

    if (max_rawdata > high_thd)
    {
        while ((max_rawdata > high_thd) && ((*new_tia_gain_code) > 0 ))
        {
            (*new_tia_gain_code)--;
            times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code)])
                               / p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code) + 1];
            max_rawdata = (int64_t)max_rawdata * times / GH_AGC_TIMES_FACTOR;
            min_rawdata = (int64_t)min_rawdata * times / GH_AGC_TIMES_FACTOR;
        }
    }
    else if (min_rawdata < low_thd )
    {   // In order to prevent nonlinearity from causing repeated adjustments, a certain margin is left in the gain increase judgment.
        while (( 8 * temp_max_rawdata < 7 * high_thd) && ((*new_tia_gain_code) < gain_limit) )
        {
            (*new_tia_gain_code)++;
            times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code)])
                              / p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code) - 1];
            max_rawdata = (int64_t)max_rawdata * times / GH_AGC_TIMES_FACTOR;
            min_rawdata = (int64_t)min_rawdata * times / GH_AGC_TIMES_FACTOR;

            times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code) + 1])
                              / p_agc->agc_chip_info.p_gain_value[(*new_tia_gain_code)];
            temp_max_rawdata = (int64_t)max_rawdata * times / GH_AGC_TIMES_FACTOR;
        }
    }

    *p_max_rawdata = max_rawdata;
    *p_min_rawdata = min_rawdata;
}

 /**
 * @fn     uint8_t gh_agc_led_drv_gain_cal(int32_t rawdata, uint32_t IdealRawdata, gh_agc_mainchnl_config_t *p_agc_mainchnl_config, uint8_t* p_ideal_led_drv, uint8_t* p_ideal_gain)
 *
 * @brief  Calculate DrvCurrent and Gain
 *
 * @attention
 *
 * @param[in]   mean_ipd, IdealRawdata, uchMainChnlInfoIndex
 * @param[out]  p_ideal_led_drv, p_ideal_gain
 *
 * @return  0:DrvCurrent or gain will NOT change  1: DrvCurrent or gain will  change
 */
static uint8_t gh_agc_led_drv_gain_cal(gh_hal_agc_t* p_agc, int32_t mean_ipd, uint8_t agc_slot_cnt,
                                       uint8_t* p_ideal_led_drv, uint8_t* p_ideal_gain)
{
    gh_agc_mainchnl_config_t *p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[agc_slot_cnt]);
    gh_agc_mainchnl_info_t *p_agc_mainchnl_info = &(p_agc->agc_mainchnl_info[agc_slot_cnt]);
    gh_agc_ideal_adj_info_t *p_agc_ideal_adj_info = &(p_agc->agc_ideal_adj_info[agc_slot_cnt]);

    uint8_t gain_limit = p_agc_mainchnl_config->agc_gain_limit;
    uint8_t min_led_drv_code = p_agc_mainchnl_config->min_led_drv;
    uint8_t max_led_drv_code = p_agc_mainchnl_config->max_led_drv;
    uint8_t current_drv = p_agc_mainchnl_config->current_drv;
    uint8_t current_gain = p_agc_mainchnl_config->current_gain;

    int32_t max_rawdata = p_agc_mainchnl_info->max_rawdata;
    int32_t min_rawdata = p_agc_mainchnl_info->min_rawdata;
    uint8_t sat_flag = p_agc_mainchnl_info->sat_flag;//rawdata Saturation flag

    uint8_t temp_current_drv = current_drv;
    uint8_t new_tia_gain_code = current_gain;
    uint8_t adjust_flag = 0;//bit0: drv current flag  bit1: gain flag
    uint8_t calc_finish = 0;

    int32_t high_thd;
    int32_t low_thd;
    int32_t ideal_ipd;
    int32_t high_ipd_thd;
    int32_t low_ipd_thd;

    if (p_agc_mainchnl_config->spo2_chnl_flag) // is  spo2 slot
    {
        high_ipd_thd = p_agc->agc_confg.agc_spo2_high_ipd * GH_AGC_IPD_NA_FACTOR;
        low_ipd_thd = p_agc->agc_confg.agc_spo2_low_ipd * GH_AGC_IPD_NA_FACTOR;
        ideal_ipd = ((high_ipd_thd + low_ipd_thd) / 2);
    }
    else
    {
        high_ipd_thd = p_agc->agc_confg.agc_gene_high_ipd * GH_AGC_IPD_NA_FACTOR;
        low_ipd_thd = p_agc->agc_confg.agc_gene_low_ipd * GH_AGC_IPD_NA_FACTOR;
        ideal_ipd = ((high_ipd_thd + low_ipd_thd) / 2);
    }
    gh_agc_thd_get(p_agc, p_agc_mainchnl_config->spo2_chnl_flag, &high_thd, &low_thd);

    DEBUG_LOG("Drv cal %d: UpLimit = %d, Mean = %d, LastMean = %d\r\n",agc_slot_cnt, high_thd,
              p_agc_mainchnl_info->mean_rawdata, p_agc_mainchnl_info->last_mean_rawdata);
    // reduce rawdata as soon as posible
    if ((high_thd < p_agc_mainchnl_info->mean_rawdata) && (high_thd < p_agc_mainchnl_info->last_mean_rawdata))
    {
       new_tia_gain_code = 0;
       temp_current_drv = min_led_drv_code;
       calc_finish = 1;
       p_agc_ideal_adj_info->inite_flag = 0; //
       p_agc_mainchnl_info->analysis_cnt = 0;
    }
    p_agc_mainchnl_info ->last_mean_rawdata = p_agc_mainchnl_info ->mean_rawdata;

    if (0 == calc_finish)
    {
        //find new drv current and gain
        if (mean_ipd < low_ipd_thd)
        {
            if (0==sat_flag)//main channel sat flag
            {
                gh_agc_led_drv_calc(mean_ipd, ideal_ipd, current_drv, min_led_drv_code, max_led_drv_code, &temp_current_drv);
                DEBUG_LOG("Drv cal0: SlotCnt = %d, Ipd = %d, Ideal_Ipd = %d, CurDrv = %d, TempDrv = %d, Max = %d, Min = %d\r\n",agc_slot_cnt, mean_ipd,
                                     ideal_ipd, current_drv, temp_current_drv, max_rawdata, min_rawdata);

                gh_agc_extremum_cal( current_drv, temp_current_drv, &max_rawdata, &min_rawdata);
                DEBUG_LOG("Drv cal0: SlotCnt = %d ,Max = %d, Min = %d\r\n",agc_slot_cnt, max_rawdata, min_rawdata);

                gh_agc_extremum_gain_cal(p_agc,  high_thd, low_thd, gain_limit, &new_tia_gain_code, &max_rawdata, &min_rawdata);
                DEBUG_LOG("Drv cal0: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",agc_slot_cnt, high_thd, low_thd,
                                     current_gain, new_tia_gain_code, max_rawdata, min_rawdata);
            }
            else if (new_tia_gain_code > 0)
            {
               new_tia_gain_code--;
            }
        }
        else if ( mean_ipd <= high_ipd_thd )
        {
            gh_agc_extremum_gain_cal(p_agc,  high_thd, low_thd, gain_limit, &new_tia_gain_code, &max_rawdata, &min_rawdata);
            DEBUG_LOG("Drv cal1: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",agc_slot_cnt, high_thd, low_thd,
                                 current_gain, new_tia_gain_code, max_rawdata, min_rawdata);
        }
        else////mean_ipd > high_ipd_thd
        {
            gh_agc_led_drv_calc(mean_ipd, ideal_ipd, current_drv, min_led_drv_code, max_led_drv_code, &temp_current_drv);
            DEBUG_LOG("Drv cal2: SlotCnt = %d, Ipd = %d, Ideal_Ipd = %d, CurDrv = %d, TempDrv = %d, Max = %d, Min = %d\r\n",agc_slot_cnt, mean_ipd,
                     ideal_ipd, current_drv, temp_current_drv, max_rawdata, min_rawdata);

            gh_agc_extremum_cal( current_drv, temp_current_drv, &max_rawdata, &min_rawdata);
            DEBUG_LOG("Drv cal2: SlotCnt = %d ,Max = %d, Min = %d\r\n",agc_slot_cnt, max_rawdata, min_rawdata);

            gh_agc_extremum_gain_cal(p_agc, high_thd, low_thd, gain_limit, &new_tia_gain_code, &max_rawdata, &min_rawdata);
            DEBUG_LOG("Drv cal2: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",agc_slot_cnt, high_thd, low_thd,
                      current_gain, new_tia_gain_code, max_rawdata, min_rawdata);
        }
    }

    p_ideal_led_drv[0] = (uint8_t)temp_current_drv;
    p_ideal_gain[0] = (uint8_t)new_tia_gain_code;

    if (p_ideal_led_drv[0] != current_drv)
    {
        adjust_flag |= 0x01;
    }
    if (p_ideal_gain[0] != current_gain)
    {
        adjust_flag |= 0x02;
    }

    p_agc_mainchnl_info->sat_flag = 0;
    return adjust_flag;
}

#if GH_AGC_SUB_CHNL_NUM > 0
static void gh_agc_subchn_gain_set_cnt_clr(gh_hal_agc_t* p_agc, uint8_t slotcfg, uint8_t rx_en, uint8_t current_drv,uint8_t new_led_drv_code)
{
    for (uint8_t rx_cnt = 0; rx_cnt < GH_RX_NUM; rx_cnt ++)
    {
        if (rx_en & (1 << rx_cnt))
        {
            for (uint8_t chnl_cnt = 0; chnl_cnt <  p_agc->agc_subchnl_num; chnl_cnt++)
            {
                gh_agc_subchnl_info_t *p_gh_agc_subchnl_info = &(p_agc->gh_agc_subchnl_info[chnl_cnt]);
                if (p_gh_agc_subchnl_info->agc_en)
                {
                    if (((slotcfg << GH_SLOTCFG_SHIFT_NUM) + rx_cnt) == p_gh_agc_subchnl_info->slotcfg_rx)
                    {
                        int32_t high_thd;
                        int32_t low_thd;
                        uint8_t current_gain = p_gh_agc_subchnl_info->current_gain;
                        uint8_t new_tia_gain_code = current_gain;
                        int32_t max_rawdata = p_gh_agc_subchnl_info->max_rawdata;
                        int32_t min_rawdata = p_gh_agc_subchnl_info->min_rawdata;

                        uint8_t agc_slot_cnt = gh_agc_subchnl_slot_get(p_agc, slotcfg);
                        uint8_t gain_limit = p_agc->agc_mainchnl_config[agc_slot_cnt].agc_gain_limit;
                        uint8_t spo2_chnl_flag = p_agc->agc_mainchnl_config[agc_slot_cnt].spo2_chnl_flag;

                        p_gh_agc_subchnl_info->analysis_cnt = 0;
                        gh_agc_thd_get(p_agc, spo2_chnl_flag, &high_thd, &low_thd);

                        DEBUG_LOG("SubChnlAdj: SlotCfg = %d, RxCnt = %d, Max = %d, Min = %d\r\n",slotcfg,rx_cnt,max_rawdata, min_rawdata);
                        gh_agc_extremum_cal(current_drv, new_led_drv_code, &max_rawdata, &min_rawdata);

                        gh_agc_extremum_gain_cal(p_agc, high_thd, low_thd, gain_limit, &new_tia_gain_code, &max_rawdata, &min_rawdata);
                        DEBUG_LOG("SubChnlAdj: SlotAdcNo = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",p_gh_agc_subchnl_info->slotcfg_rx, \
                                             high_thd, low_thd,current_gain, new_tia_gain_code, max_rawdata, min_rawdata);

                        if (current_gain != new_tia_gain_code)
                        {
                            p_agc->tia_gain_write(slotcfg, rx_cnt, new_tia_gain_code);
                            p_gh_agc_subchnl_info ->current_gain = new_tia_gain_code;
                            gh_agc_gain_store(p_agc, slotcfg, rx_cnt, new_tia_gain_code);
                        }
                    }
                }
            }
        }
    }
}
#endif

#define DRE_TEST     (0)

static void gh_agc_main_chnl_process(gh_hal_agc_t* p_agc, gh_data_t* p_gh_data, uint16_t data_len)
{
    int32_t mean_ipd_ary[GH_SLOT_NUM] = {0};
    uint32_t agc_flag = 0;// Local variables used as judgments must be initialized to 0
    uint8_t new_led_drv_code;
    uint8_t new_tia_gain_code;
    uint8_t adjust_flag = 0;  //bit0: drv current flag  bit1: gain flag

    if (0 == p_agc->agc_slot_num)
    {
        return;
    }

     gh_agc_mainchnl_info_cal(p_agc, p_gh_data, data_len, mean_ipd_ary, &agc_flag); //caculate Ipdmean and whther AGC
     DEBUG_LOG("AGC_Flag: agc_flag = %d\r\n", agc_flag);
     for (uint8_t agc_slot_cnt = 0; agc_slot_cnt < p_agc->agc_slot_num; agc_slot_cnt ++)
     {
         if ( agc_flag & (1 << agc_slot_cnt)) // need to AGC
         {
            int32_t mean_ipd = mean_ipd_ary[agc_slot_cnt];
            gh_agc_mainchnl_config_t *p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[agc_slot_cnt]);

#if GH_HAL_AGC_DRE_EN
            if (p_agc_mainchnl_config->dre_en)
            {
                continue;
            }
#endif
           adjust_flag = gh_agc_led_drv_gain_cal(p_agc, mean_ipd, agc_slot_cnt, &new_led_drv_code, &new_tia_gain_code);
           uint8_t  slotcfg = ((p_agc_mainchnl_config->slotcfg_rx >> 2) & GH_AGC_SLOTCFG_MASK);
           if (adjust_flag & 0x02)//write tia gain to chip
           {
               DEBUG_LOG("[ SoftAGC Change Gain] Slotcfg = %d, NewGain = %d, CurGain = %d\r\n", slotcfg, new_tia_gain_code, p_agc_mainchnl_config->current_gain);
               p_agc->tia_gain_write(slotcfg, p_agc_mainchnl_config->agc_src, new_tia_gain_code);
               p_agc_mainchnl_config->current_gain = new_tia_gain_code;
               gh_agc_gain_store(p_agc, slotcfg, p_agc_mainchnl_config->agc_src, new_tia_gain_code);
           }
           if (adjust_flag & 0x01)//write drv current to chip
           {
               //change drv0/1
               uint8_t current_drv =  p_agc_mainchnl_config->current_drv;
               p_agc_mainchnl_config->current_drv = new_led_drv_code;
               if ((p_agc_mainchnl_config->agc_drv_en) & 0x01)
               {
                   p_agc->led_drv_write(slotcfg, GH_LED_DRV0_IDX, new_led_drv_code);
                   gh_agc_leddrv_store(p_agc, slotcfg, GH_LED_DRV0_IDX, new_led_drv_code);
#if DRE_TEST > 0
                       if (p_agc_mainchnl_config->spo2_chnl_flag) // is  spo2 slot
                       {
                           if (slotcfg == 0)
                           {
                               p_agc->led_drv_write(4, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 4, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                           }
                           if (slotcfg == 1)
                           {
                               p_agc->led_drv_write(5, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 5, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                           }
                       }
                       else
                       {
                           if (slotcfg == 0)
                           {
                               p_agc->led_drv_write(4, GH_LED_DRV0_IDX, (new_led_drv_code - 1));
                               gh_agc_leddrv_store(p_agc, 4, GH_LED_DRV0_IDX, (new_led_drv_code - 1));
                           }
                           if (slotcfg == 1)
                           {
                               p_agc->led_drv_write(5, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 5, GH_LED_DRV0_IDX, (new_led_drv_code / 2));
                           }
                       }
#endif
               }
               if ((p_agc_mainchnl_config->agc_drv_en) & 0x02)
               {
                   p_agc->led_drv_write(slotcfg, GH_LED_DRV1_IDX, new_led_drv_code);
                   gh_agc_leddrv_store(p_agc, slotcfg, GH_LED_DRV1_IDX, new_led_drv_code);
#if DRE_TEST > 0
                       if (p_agc_mainchnl_config->spo2_chnl_flag) // is  spo2 slot
                       {
                           if (slotcfg == 0)
                           {
                               p_agc->led_drv_write(4, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 4, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                           }
                           if (slotcfg == 1)
                           {
                               p_agc->led_drv_write(5, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 5, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                           }
                       }
                       else
                       {
                           if (slotcfg == 0)
                           {
                               p_agc->led_drv_write(4, GH_LED_DRV1_IDX, (new_led_drv_code - 1));
                               gh_agc_leddrv_store(p_agc, 4, GH_LED_DRV1_IDX, (new_led_drv_code - 1));
                           }
                           if (slotcfg == 1)
                           {
                               p_agc->led_drv_write(5, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                               gh_agc_leddrv_store(p_agc, 5, GH_LED_DRV1_IDX, (new_led_drv_code / 2));
                           }
                       }
#endif
               }

               #if GH_AGC_SUB_CHNL_NUM > 0
               gh_agc_subchn_gain_set_cnt_clr(p_agc, slotcfg, 0x0F,current_drv, new_led_drv_code); //clear all rx analysis cnt and adjast gain
               #endif
            }
            if (adjust_flag)
            {
                p_agc->agc_mainchnl_info[agc_slot_cnt].agc_trigger = 1; // current or  gain change at last time
                p_agc->agc_mainchnl_info[agc_slot_cnt].drop_flag = 0;
                //NewAgcMainChelFlag |= (1<<slotcfg); // ADjFlag write to CSV file
            }
         }
     }
}

uint32_t gh_agc_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len)
{
    if (GH_NULL_PTR == p_agc || GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_PTR_NULL);
    }

    if (0 == data_len || 0 == p_agc->agc_slot_num)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
    }

#if GH_HAL_AGC_DRE_EN
    // DEBUG_LOG("[AGC PROCESs] start agc, datatype = %d\r\n", p_gh_data->ppg_data.data_channel.data_type);
    gh_agc_dre_process(p_agc, p_gh_data, data_len);
#endif

#if GH_AGC_SUB_CHNL_NUM > 0
    gh_agc_sub_chnl_process(p_agc, p_gh_data, data_len);
#endif
    gh_agc_main_chnl_process(p_agc, p_gh_data, data_len);

    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

uint32_t gh_agc_disable(gh_hal_agc_t *p_agc)
{
    if (GH_NULL_PTR == p_agc)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_PTR_NULL);
    }

    p_agc->agc_slot_num = 0;
    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

uint32_t gh_agc_slot_cfg_restore(gh_hal_agc_t *p_agc, uint8_t slot_cfg_id,
                                 uint8_t led_drv[GH_LED_DRV_NUM],
                                 uint8_t tia_gain[GH_PPG_RX_NUM])
{
    for (uint8_t slot_cnt = 0; slot_cnt < p_agc->agc_slot_num; slot_cnt++)
    {
        if (slot_cfg_id == (p_agc->agc_mainchnl_config[slot_cnt].slotcfg_rx >> GH_SLOTCFG_SHIFT_NUM)
            && p_agc->agc_mainchnl_config[slot_cnt].agc_drv_en)
        {
            gh_agc_mainchnl_config_t* p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[slot_cnt]);

            p_agc->agc_mainchnl_info[slot_cnt].analysis_cnt = 0; //clear analysis cnt
            for (uint8_t led_cnt = 0; led_cnt < GH_LED_DRV_NUM; led_cnt++)
            {
                if (p_agc_mainchnl_config->agc_drv_en & (1 << led_cnt))
                {
                    p_agc_mainchnl_config->current_drv = led_drv[led_cnt];
                }
                // agc information drv restore
                gh_agc_leddrv_store(p_agc, slot_cfg_id, led_cnt, led_drv[led_cnt]);
            }

            for (uint8_t rx_cnt = 0; rx_cnt < GH_PPG_RX_NUM; rx_cnt++)
            {
                // main channel gain restore
                if (rx_cnt == p_agc_mainchnl_config->agc_src)
                {
                    p_agc_mainchnl_config->current_gain = tia_gain[rx_cnt];
                }
                // agc information gain restore
                gh_agc_gain_store(p_agc, slot_cfg_id, rx_cnt, tia_gain[rx_cnt]);

                //sub channel restore
                uint8_t sub_rx_en = (p_agc->p_settings->ppg_cfg_param[slot_cfg_id].rx_en
                                     & (~(1 << p_agc_mainchnl_config->agc_src)));//clear main channel RX enable bit
                if (sub_rx_en & (1 << rx_cnt))
                {
                    for (uint8_t chnl_cnt = 0; chnl_cnt <  p_agc->agc_subchnl_num; chnl_cnt++)
                    {
                        gh_agc_subchnl_info_t *p_gh_agc_subchnl_info = &(p_agc->gh_agc_subchnl_info[chnl_cnt]);
                        if (p_gh_agc_subchnl_info->agc_en)
                        {
                            if (((slot_cfg_id << GH_SLOTCFG_SHIFT_NUM) + rx_cnt) == p_gh_agc_subchnl_info->slotcfg_rx)
                            {
                                p_gh_agc_subchnl_info->analysis_cnt = 0; //clear analysis cnt
                                p_gh_agc_subchnl_info->current_gain = tia_gain[rx_cnt];
                            }
                        }
                    }
                }
            }
        }
    }

#if GH_HAL_AGC_DRE_EN
    gh_agc_dre_common_reset();
#endif

    return 0;
}
#endif
