/**
  ****************************************************************************************
  * @file    gh_hal_agc_dre.c
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
#include "gh_hal_agc_dre.h"
#include "gh_hal_utils.h"
#include "gh_hal_chip.h"
#include "gh_hal_config.h"
#include "gh_hal_log.h"
#include "gh_hal_service.h"

#if GH_SUPPORT_SOFT_AGC
#if GH_HAL_AGC_DRE_EN
/*
 * DEFINES
 *****************************************************************************************
 */
#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    (GH_HAL_AGC_ID << 24 | (internal_err) << 8 | (interface_err))

#if GH_AGC_LOG_EN
#define GH_DRE_DATA_PROCESS_LOG_EN               (1)
#define GH_DRE_SAT_PROCESS_LOG_EN                (1)
#define GH_DRE_ADJ_PROCESS_LOG_EN                (1)
#define GH_DRE_UPDATE_PROCESS_LOG_EN             (1)
#define DEBUG_LOG(...)                           GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                         GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                           GH_LOG_LVL_ERROR(__VA_ARGS__)

#if GH_DRE_DATA_PROCESS_LOG_EN
#define DEBUG_DP_LOG(...)                        GH_LOG_LVL_DEBUG(__VA_ARGS__)
#else
#define DEBUG_DP_LOG(...)
#endif
#if GH_DRE_SAT_PROCESS_LOG_EN
#define DEBUG_SP_LOG(...)                        GH_LOG_LVL_DEBUG(__VA_ARGS__)
#else
#define DEBUG_SP_LOG(...)
#endif
#if GH_DRE_ADJ_PROCESS_LOG_EN
#define DEBUG_AP_LOG(...)                        GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define DEBUG_AP_DRV_LOG(...)                    GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define DEBUG_AP_GAIN_LOG(...)                   GH_LOG_LVL_DEBUG(__VA_ARGS__)
#else
#define DEBUG_AP_LOG(...)
#define DEBUG_AP_DRV_LOG(...)
#define DEBUG_AP_GAIN_LOG(...)
#endif
#if GH_DRE_UPDATE_PROCESS_LOG_EN
#define DEBUG_UP_LOG(...)                        GH_LOG_LVL_DEBUG(__VA_ARGS__)
#else
#define DEBUG_UP_LOG(...)
#endif
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#define DEBUG_DP_LOG(...)
#define DEBUG_SP_LOG(...)
#define DEBUG_AP_LOG(...)
#define DEBUG_AP_DRV_LOG(...)
#define DEBUG_AP_GAIN_LOG(...)
#define DEBUG_UP_LOG(...)
#endif

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
gh_agc_common_info_t g_agc_common_info = {0};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void gh_agc_extremum_get(int32_t rawdata, int32_t* p_max_rawdata, int32_t* p_min_rawdata);

/**
 * @fn     gh_get_med(int32_t data_1st, int32_t data_2nd, int32_t data_3rd)
 *
 * @brief  get med of three data
 *
 * @attention
 *
 * @param[in]   data_1st, data_2nd, data_3rd
 * @param[out]  med data
 *
 * @return
 */
static int32_t gh_get_med(int32_t data_1st, int32_t data_2nd, int32_t data_3rd)
{
    if ((data_1st >= data_2nd && data_1st <= data_3rd)
        || (data_1st <= data_2nd && data_1st >= data_3rd))
    {
        return data_1st;
    }
    else if ((data_2nd >= data_1st && data_2nd <= data_3rd)
        || (data_2nd <= data_1st && data_2nd >= data_3rd))
    {
        return data_2nd;
    }
    else
    {
        return data_3rd;
    }
}

/**
 * @fn     gh_agc_extremum_get(int32_t rawdata, int32_t* p_max_rawdata, int32_t* p_min_rawdata)
 *
 * @brief  get max and min data
 *
 * @attention
 *
 * @param[in]   rawdata, p_max_rawdata, p_min_rawdata
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_extremum_get(int32_t rawdata, int32_t* p_max_rawdata, int32_t* p_min_rawdata)
{
    (*p_max_rawdata) = ((rawdata > (*p_max_rawdata)) ? rawdata : (*p_max_rawdata)); // Max of  rawdata
    (*p_min_rawdata) = ((rawdata < (*p_min_rawdata)) ? rawdata : (*p_min_rawdata)); // Min of  rawdata
}

/**
 * @fn     gh_agc_dre_satflag_set(int32_t high_thd, int32_t rawdata, uint8_t mix_satflag,
 *                                   uint16_t* p_sat_checked_cnt, uint16_t* p_sat_cnt,
 *                                   uint8_t* p_satflag, uint16_t sample_rate)
 *
 * @brief  set sat flag
 *
 * @attention
 *
 * @param[in]   high_thd, rawdata, mix_satflag, p_sat_checked_cnt, p_sat_cnt, p_satflag, sample_rate
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_satflag_set(int32_t high_thd, int32_t rawdata, uint8_t mix_satflag,
                                   uint16_t* p_sat_checked_cnt, uint16_t* p_sat_cnt,
                                   uint8_t* p_satflag, uint16_t sample_rate)
{
    uint16_t max_sat_check_num = GH_AGC_SAT_SEC * sample_rate; //5 seconds window
    (*p_sat_checked_cnt)++;
    if (rawdata > high_thd || rawdata < -high_thd || mix_satflag)
    {
        (*p_sat_cnt)++;
    }

    //// as soon as posible set the Saturation flag
    if ((*p_sat_cnt) > (max_sat_check_num >> GH_AGC_SAT_CHECK_FATOR)) // about 12.5% of all checked data
    {
        (*p_satflag) = GH_PPG_SAT;
        (*p_sat_cnt) = 0;
        (*p_sat_checked_cnt) = 0;
    }

    if ((*p_sat_checked_cnt) >= max_sat_check_num)
    {
        (*p_sat_cnt) = 0;
        (*p_sat_checked_cnt) = 0;
    }
}

/**
 * @fn     gh_agc_dre_fre_flag_set(int32_t dre_update, uint16_t* p_dre_checked_cnt, uint16_t* p_dre_cnt,
 *                                   uint8_t* p_satflag, uint16_t sample_rate)
 * @brief  set dre frequenty adjust flag
 *
 * @attention
 *
 * @param[in]   dre_update, p_dre_checked_cnt, p_dre_cnt, p_satflag, sample_rate
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_fre_flag_set(int32_t dre_update, uint16_t* p_dre_checked_cnt, uint16_t* p_dre_cnt,
                                    uint8_t* p_satflag, uint16_t sample_rate)
{
    uint16_t max_sat_check_num = GH_AGC_DRE_FRE_SEC * sample_rate; //2 seconds window
    (*p_dre_checked_cnt)++;
    if (dre_update)
    {
        (*p_dre_cnt)++;
    }

    //// as soon as posible set the Saturation flag
    if ((*p_dre_cnt) > (max_sat_check_num >> GH_AGC_DRE_FRE_CHECK_FATOR)) // about 25% of all checked data
    {
        (*p_satflag) = GH_PPG_DRE_FRE;
        (*p_dre_cnt) = 0;
    }

    if ((*p_dre_checked_cnt) >= max_sat_check_num)
    {
        (*p_dre_cnt) = 0;
        (*p_dre_checked_cnt) = 0;
    }
}

/**
 * @fn     gh_agc_dre_ipd_calc(int32_t rawdata, uint16_t gain_value)
 *
 * @brief  caculate data ipd
 *
 * @attention
 *
 * @param[in]   rawdata, gain_value
 * @param[out]  mean_ipd
 *
 * @return
 */
static int32_t gh_agc_dre_ipd_calc(int32_t rawdata, uint16_t gain_value)
{
     int32_t mean_ipd;

     mean_ipd = (int32_t)((int64_t)(rawdata) * GH_VOLT_REF_UA / ((int64_t)gain_value * GH_MAX_RAWDATA)
                * GH_AGC_IPD_NA_FACTOR);//Ipd unit:nA

     if (0 >= mean_ipd)
     {
         mean_ipd = 1; //Avoid the division is 0
     }

     return mean_ipd;
}

/**
 * @fn     gh_agc_dre_led_drv_calc(int32_t mean_ipd, int32_t ideal_ipd, uint8_t current_drv, uint8_t min_led_drv_code, uint8_t max_led_drv_code)
 *
 * @brief  Calculate DrvCurrent
 *
 * @attention
 *
 * @param[in]   mean_ipd, ideal_ipd, current_drv, min_led_drv_code, max_led_drv_code
 * @param[out]  new_led_drv_code
 *
 * @return
 */
static uint8_t gh_agc_dre_led_drv_calc(int32_t mean_ipd, int32_t ideal_ipd, uint8_t current_drv,
                                       uint8_t min_led_drv_code, uint8_t max_led_drv_code)
{
    int32_t new_led_drv_code = current_drv * (ideal_ipd) / (mean_ipd);
    if (new_led_drv_code < min_led_drv_code)
    {
        new_led_drv_code = min_led_drv_code;
    }
    else if (new_led_drv_code > max_led_drv_code)
    {
       new_led_drv_code = max_led_drv_code;
    }

    return (uint8_t)new_led_drv_code;
}

/**
 * @fn     gh_get_data_agc_type(gh_hal_ppg_data_t ppg_dre_dc_info, gh_dre_agc_info_t *p_dre_agc_info, gh_hal_agc_t* p_agc)
 *
 * @brief  get data type(main or sub) and index
 *
 * @attention
 *
 * @param[in]   ppg_dre_dc_info, p_dre_agc_info, p_agc
 * @param[out]  none
 *
 * @return
 */
static void gh_get_data_agc_type(gh_hal_ppg_data_t *ppg_dre_dc_info, gh_dre_agc_info_t *p_dre_agc_info,
                                 gh_hal_agc_t* p_agc)
{
    uint8_t slot_cfg_id = ppg_dre_dc_info->data_channel.channel_ppg_dre_dc_info.slot_cfg_id;
    uint8_t rx_id = ppg_dre_dc_info->data_channel.channel_ppg_dre_dc_info.rx_id;
    uint8_t slotcfg_rx = ((slot_cfg_id << GH_SLOTCFG_SHIFT_NUM) + rx_id);
    p_dre_agc_info->agc_main_sub_type = GH_PPG_AGC_SUB;

    for (uint8_t agc_slot_cnt = 0; agc_slot_cnt < p_agc->agc_slot_num; agc_slot_cnt ++)
    {
        if (p_agc->agc_mainchnl_config[agc_slot_cnt].agc_drv_en)
        {
            if (slot_cfg_id == p_agc->agc_mainchnl_config[agc_slot_cnt].slotcfg_rx >> GH_SLOTCFG_SHIFT_NUM)
            {
                p_dre_agc_info->main_idex = agc_slot_cnt;
                p_dre_agc_info->slot_idex = slot_cfg_id;
            }

            if (slotcfg_rx == p_agc->agc_mainchnl_config[agc_slot_cnt].slotcfg_rx)
            {
                p_dre_agc_info->agc_slot_idex = agc_slot_cnt;
                p_dre_agc_info->agc_main_sub_type = GH_PPG_AGC_MAIN;
                break;
            }
        }
    }

    if (GH_PPG_AGC_SUB == p_dre_agc_info->agc_main_sub_type)
    {
        for (uint8_t agc_sub_cnt = 0; agc_sub_cnt < p_agc->agc_subchnl_num; agc_sub_cnt ++)
        {
            if (p_agc->gh_agc_subchnl_info[agc_sub_cnt].agc_en
                && slotcfg_rx == p_agc->gh_agc_subchnl_info[agc_sub_cnt].slotcfg_rx)
            {
                p_dre_agc_info->agc_slot_idex = agc_sub_cnt;
                p_dre_agc_info->agc_main_sub_type = GH_PPG_AGC_SUB;
                break;
            }
        }
    }
}

/**
 * @fn     gh_agc_dre_data_create(gh_hal_agc_t* p_agc, gh_dre_agc_info_t *p_dre_agc_info)
 *
 * @brief  get dre agc info data
 *
 * @attention
 *
 * @param[in]   p_agc, p_dre_agc_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_data_create(gh_hal_agc_t* p_agc, gh_dre_agc_info_t *p_dre_agc_info)
{
    //create dre agc data
    //get common data
    gh_agc_mainchnl_config_t *p_agc_mainchnl_config = &(p_agc->agc_mainchnl_config[p_dre_agc_info->main_idex]);
    gh_agc_ideal_adj_info_t *p_agc_ideal_adj_info = &(p_agc->agc_ideal_adj_info[p_dre_agc_info->main_idex]);

    //get sub and main chnl data
    gh_agc_mainchnl_info_t *p_agc_mainchnl_info = &(p_agc->agc_mainchnl_info[p_dre_agc_info->agc_slot_idex]);
    gh_agc_subchnl_info_t *p_agc_subchnl_info = &(p_agc->gh_agc_subchnl_info[p_dre_agc_info->agc_slot_idex]);

    // agc_current_param:common agc current param
    p_dre_agc_info->agc_current_param.current_drv = &p_agc_mainchnl_config->current_drv;
    p_dre_agc_info->agc_current_param.dc_cancel_code = &p_agc_mainchnl_config->dc_cancel_code;

    // agc_configure:common agc configure
    p_dre_agc_info->agc_configure.agc_drv_en = p_agc_mainchnl_config->agc_drv_en;
    p_dre_agc_info->agc_configure.agc_src = p_agc_mainchnl_config->agc_src;
    p_dre_agc_info->agc_configure.sample_rate = p_agc_mainchnl_info->sample_rate;
    p_dre_agc_info->agc_configure.min_led_drv = p_agc_mainchnl_config->min_led_drv;
    p_dre_agc_info->agc_configure.max_led_drv = p_agc_mainchnl_config->max_led_drv;
    p_dre_agc_info->agc_configure.agc_gain_limit = p_agc_mainchnl_config->agc_gain_limit;

    //agc_flag: flag
    p_dre_agc_info->agc_flag.init_flag = &p_agc_ideal_adj_info->inite_flag;

    //gain value
    p_dre_agc_info->p_gain_value = p_agc->agc_chip_info.p_gain_value;

    // agc_configure:dre en
    p_dre_agc_info->agc_configure.dre_en = p_agc_mainchnl_config->dre_en;

    if (GH_PPG_AGC_MAIN == p_dre_agc_info->agc_main_sub_type) // create p_dre_agc_info data for main agc chnl
    {
        // agc_store_data:agc stored data
        p_dre_agc_info->agc_store_data.rawdata_sum = &p_agc_mainchnl_info->rawdata_sum;
        p_dre_agc_info->agc_store_data.mean_rawdata = &p_agc_mainchnl_info->mean_rawdata;
        p_dre_agc_info->agc_store_data.last_mean_rawdata = &p_agc_mainchnl_info->last_mean_rawdata;
        p_dre_agc_info->agc_store_data.last_rawdata = &p_agc_mainchnl_info->last_rwdata;
        p_dre_agc_info->agc_store_data.last2_rawdata = &p_agc_mainchnl_info->last2_rwdata;
        p_dre_agc_info->agc_store_data.max_rawdata = &p_agc_mainchnl_info->max_rawdata;
        p_dre_agc_info->agc_store_data.min_rawdata = &p_agc_mainchnl_info->min_rawdata;
        p_dre_agc_info->agc_store_data.last_dc_cancel_code = &p_agc_mainchnl_info->last_dc_cancel_code;

        // agc_current_param:agc current param
        p_dre_agc_info->agc_current_param.current_gain = &p_agc_mainchnl_config->current_gain;

        // agc_configure:agc configure
        p_dre_agc_info->agc_configure.agc_en = p_agc_mainchnl_config->agc_drv_en;
        p_dre_agc_info->agc_configure.slotcfg_rx = p_agc_mainchnl_config->slotcfg_rx;

        // p_dre_agc_info:agc cnt
        p_dre_agc_info->agc_cnt.window_cnt = &p_agc_ideal_adj_info->window_cnt;
        p_dre_agc_info->agc_cnt.sat_cnt = &p_agc_mainchnl_info->sat_cnt;
        p_dre_agc_info->agc_cnt.sat_chceked_cnt = &p_agc_mainchnl_info->sat_chceked_cnt;
        p_dre_agc_info->agc_cnt.dre_fre_chceked_cnt = &p_agc_mainchnl_info->dre_fre_chceked_cnt;
        p_dre_agc_info->agc_cnt.dre_fre_cnt = &p_agc_mainchnl_info->dre_fre_cnt;
        p_dre_agc_info->agc_cnt.ipd_tcnt = &p_agc_mainchnl_info->ipd_tcnt;
        p_dre_agc_info->agc_cnt.gain_tcnt = &p_agc_mainchnl_info->gain_tcnt;
        p_dre_agc_info->agc_cnt.fluc_tcnt = &p_agc_mainchnl_info->fluc_tcnt;
        p_dre_agc_info->agc_cnt.analysis_cnt = &p_agc_mainchnl_info->analysis_cnt;
    }// if (GH_PPG_AGC_MAIN == p_dre_agc_info->agc_main_sub_type)
    else // create p_dre_agc_info data for sub agc chnl
    {
        // agc_store_data:agc stored data
        p_dre_agc_info->agc_store_data.rawdata_sum = &p_agc_subchnl_info->rawdata_sum;
        p_dre_agc_info->agc_store_data.mean_rawdata = &p_agc_subchnl_info->mean_rawdata;
        p_dre_agc_info->agc_store_data.last_mean_rawdata = &p_agc_subchnl_info->last_mean_rawdata;
        p_dre_agc_info->agc_store_data.last_rawdata = &p_agc_subchnl_info->last_rwdata;
        p_dre_agc_info->agc_store_data.last2_rawdata = &p_agc_subchnl_info->last2_rwdata;
        p_dre_agc_info->agc_store_data.max_rawdata = &p_agc_subchnl_info->max_rawdata;
        p_dre_agc_info->agc_store_data.min_rawdata = &p_agc_subchnl_info->min_rawdata;
        p_dre_agc_info->agc_store_data.last_dc_cancel_code = &p_agc_subchnl_info->last_dc_cancel_code;

        // agc_current_param:agc current param
        p_dre_agc_info->agc_current_param.current_gain = &p_agc_subchnl_info->current_gain;

        // agc_configure:agc configure
        p_dre_agc_info->agc_configure.agc_en = p_agc_subchnl_info->agc_en;
        p_dre_agc_info->agc_configure.slotcfg_rx = p_agc_subchnl_info->slotcfg_rx;

        // p_dre_agc_info:agc cnt
        p_dre_agc_info->agc_cnt.window_cnt = &p_agc_subchnl_info->window_cnt;
        p_dre_agc_info->agc_cnt.sat_cnt = &p_agc_subchnl_info->sat_cnt;
        p_dre_agc_info->agc_cnt.sat_chceked_cnt = &p_agc_subchnl_info->sat_chceked_cnt;
        p_dre_agc_info->agc_cnt.dre_fre_chceked_cnt = &p_agc_subchnl_info->dre_fre_chceked_cnt;
        p_dre_agc_info->agc_cnt.dre_fre_cnt = &p_agc_subchnl_info->dre_fre_cnt;
        p_dre_agc_info->agc_cnt.ipd_tcnt = &p_agc_subchnl_info->ipd_tcnt;
        p_dre_agc_info->agc_cnt.gain_tcnt = &p_agc_subchnl_info->gain_tcnt;
        p_dre_agc_info->agc_cnt.fluc_tcnt = &p_agc_subchnl_info->fluc_tcnt;
        p_dre_agc_info->agc_cnt.analysis_cnt = &p_agc_subchnl_info->analysis_cnt;
    } // else

    DEBUG_DP_LOG("[DRE AGC][DATA PRE]create data: slot=%d, main_sub=%d, agc chnl idx =%d.\r\n",
                 p_dre_agc_info->slot_idex, p_dre_agc_info->agc_main_sub_type,
                 p_dre_agc_info->agc_slot_idex);
    DEBUG_DP_LOG("[DRE AGC][DATA PRE]create data: raw=%d, agc_en=%d, drv_en=%d, agc src=%d.\r\n",
                 p_dre_agc_info->rawdata, p_dre_agc_info->agc_configure.agc_en,
                 p_dre_agc_info->agc_configure.agc_drv_en, p_dre_agc_info->agc_configure.agc_src);
    DEBUG_DP_LOG("[DRE AGC][DATA PRE]create data: drv=%d, gain=%d, dc cancel=%d, sat flag=%d, dre update=%d.\r\n",
                 *p_dre_agc_info->agc_current_param.current_drv, *p_dre_agc_info->agc_current_param.current_gain,
                 *p_dre_agc_info->agc_current_param.dc_cancel_code, p_dre_agc_info->agc_flag.sat_flag,
                 p_dre_agc_info->agc_flag.dre_update_flag);
    DEBUG_DP_LOG("[DRE AGC][DATA PRE]create data: slot=%d, win cnt=%d, analysis cnt=%d, g_cnt, f_cnt = %d, %d.\r\n",
                 p_dre_agc_info->slot_idex, *p_dre_agc_info->agc_cnt.window_cnt,
                 *p_dre_agc_info->agc_cnt.analysis_cnt, *p_dre_agc_info->agc_cnt.gain_tcnt,
                 *p_dre_agc_info->agc_cnt.fluc_tcnt);
}

/**
 * @fn     gh_agc_dre_get_dc_rawdata(uint16_t gain_value, uint8_t dc_cancel_code, uint8_t dc_cancel_range, int32_t rawdata, int32_t* dc_rawdata)
 *
 * @brief  get dc rawdata
 *
 * @attention
 *
 * @param[in]   gain_value, dc_cancel_code, dc_cancel_range, rawdata, dc_rawdata
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_get_dc_rawdata(uint16_t gain_value, uint8_t dc_cancel_code, uint8_t dc_cancel_range,
                                      int32_t rawdata, int32_t* dc_rawdata)
{
    //idc(ua) to rawdata(lsb)
    int32_t idc = (int32_t)dc_cancel_code * ((int32_t)dc_cancel_range + 1) * (int32_t)GH_DC_CANCEL_RANGE_BASE
                  * GH_AGC_UA_TO_NA / (int32_t)GH_8BIT_MAX;

    if (dc_cancel_code)
    {
        *dc_rawdata = rawdata + (int32_t)(((int64_t)idc * (int64_t)(GH_MAX_RAWDATA)
                    * ((int64_t)(gain_value) / (int64_t)(GH_AGC_IDC_FACTOR))
                    / ((int64_t)(GH_VOLT_REF_MA * GH_AGC_UA_TO_NA))));
    }
    else
    {
        *dc_rawdata = rawdata;
    }
}

/**
 * @fn     gh_agc_dre_convert_common_index(uint8_t agc_slot_index)
 *
 * @brief  convert agc_slot_index to agc common index
 *
 * @attention
 *
 * @param[in]   slot_cfg_rx
 * @param[out]  agc_common_index
 *
 * @return
 */
static uint8_t gh_agc_dre_convert_common_index(uint8_t slot_cfg_rx)
{
    //get agc slot index
    uint8_t agc_common_index = 0;
    uint8_t rx_id = slot_cfg_rx & (uint8_t)(GH_AGC_RX_BIT);
    uint8_t slot_cfg_id = slot_cfg_rx >> (uint8_t)(GH_SLOTCFG_SHIFT_NUM);
    agc_common_index = slot_cfg_id * GH_RX_NUM + rx_id;
    return agc_common_index;
}

/**
 * @fn     gh_agc_dre_store_data_update(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data,
                                       gh_dre_agc_info_t *p_dre_agc_info,
                                       gh_agc_common_info_t *p_agc_common_info)
 *
 * @brief  dre agc data pre process
 *
 * @attention
 *
 * @param[in]   p_agc, p_gh_data, p_dre_agc_info, p_agc_common_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_data_update_and_process(gh_hal_agc_t* p_agc, gh_dre_agc_info_t *p_dre_agc_info,
                                               gh_hal_ppg_data_t* p_ppg_dre_dc_info,
                                               gh_agc_common_info_t *p_agc_common_info)
{
    uint16_t gain_value = p_agc->agc_chip_info.p_gain_value[p_ppg_dre_dc_info->gain_code];
    uint8_t dc_cancel_code = (uint8_t)p_ppg_dre_dc_info->dc_cancel_code;
    uint8_t dc_cancel_range = (uint8_t)p_ppg_dre_dc_info->dc_cancel_range;
    int32_t rawdata = p_ppg_dre_dc_info->rawdata;
    int32_t dc_rawdata = 0;

    // reset store data when inite or param change
    if (1 == *p_dre_agc_info->agc_cnt.analysis_cnt
        || (dc_cancel_code != *p_dre_agc_info->agc_store_data.last_dc_cancel_code))
    {
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] Reset store data.\r\n");
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] cnt = %d, dre update = %d, dc cancel = %d, last dc = %d.\r\n",
                     *p_dre_agc_info->agc_cnt.analysis_cnt, p_ppg_dre_dc_info->dre_update, dc_cancel_code,
                     *p_dre_agc_info->agc_store_data.last_dc_cancel_code);
        *p_dre_agc_info->agc_cnt.analysis_cnt = 1;
        gh_agc_dre_get_dc_rawdata(gain_value, dc_cancel_code, dc_cancel_range, rawdata, &dc_rawdata);
        *p_dre_agc_info->agc_store_data.rawdata_sum = (int64_t)dc_rawdata;
        *p_dre_agc_info->agc_store_data.last_rawdata = rawdata;
        *p_dre_agc_info->agc_store_data.last2_rawdata = rawdata;
        *p_dre_agc_info->agc_store_data.max_rawdata = (int32_t)GH_MIN_RAWDATA;
        *p_dre_agc_info->agc_store_data.min_rawdata = (int32_t)GH_MAX_RAWDATA;
        *p_dre_agc_info->agc_store_data.last_dc_cancel_code = dc_cancel_code;
        p_dre_agc_info->agc_store_data.dc_cancel_code_rawdata = dc_rawdata - rawdata;
    }
    else // update store data when normal data
    {
        int32_t med_rawdata = gh_get_med(*p_dre_agc_info->agc_store_data.last2_rawdata,
                                         *p_dre_agc_info->agc_store_data.last_rawdata, rawdata);
        gh_agc_dre_get_dc_rawdata(gain_value, dc_cancel_code, dc_cancel_range, med_rawdata, &dc_rawdata);

        *p_dre_agc_info->agc_store_data.rawdata_sum += (int64_t)dc_rawdata;
        *p_dre_agc_info->agc_store_data.last2_rawdata = *p_dre_agc_info->agc_store_data.last_rawdata;
        *p_dre_agc_info->agc_store_data.last_rawdata = rawdata;
        gh_agc_extremum_get(med_rawdata, p_dre_agc_info->agc_store_data.max_rawdata,
                            p_dre_agc_info->agc_store_data.min_rawdata);
        *p_dre_agc_info->agc_store_data.last_dc_cancel_code = dc_cancel_code;
        p_dre_agc_info->agc_store_data.dc_cancel_code_rawdata = dc_rawdata - rawdata;
    }

    p_dre_agc_info->agc_flow.gain_drv_flow = GH_PPG_GAIN_ADJ; //gain adj flow enable

    if (p_dre_agc_info->agc_configure.sample_rate == *p_dre_agc_info->agc_cnt.analysis_cnt)
    {
        *p_dre_agc_info->agc_store_data.mean_rawdata = (int32_t)(*p_dre_agc_info->agc_store_data.rawdata_sum
                                                        / (int64_t)p_dre_agc_info->agc_configure.sample_rate);
        *p_dre_agc_info->agc_cnt.analysis_cnt = 0;
        p_dre_agc_info->agc_flow.adj_full_data_flow = 1;
        p_dre_agc_info->agc_flow.gain_drv_flow |= GH_PPG_DRV_ADJ; //drv adj flow enable
        *p_dre_agc_info->agc_flag.init_flag = 1; // set inite flag = 1 after firset 1 second data
        p_agc_common_info->mean_ipd_ary[p_dre_agc_info->slot_idex] =
            gh_agc_dre_ipd_calc(*p_dre_agc_info->agc_store_data.mean_rawdata,
                                p_agc->agc_chip_info.p_gain_value[p_ppg_dre_dc_info->gain_code]);
        p_agc_common_info->drv_en[p_dre_agc_info->agc_common_index] = p_dre_agc_info->agc_configure.agc_drv_en;
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] get full data, update ipd\r\n");
    }

    DEBUG_AP_DRV_LOG("[DRE AGC][DATA PRE] cnt = %d, raw = %d, dc raw = %d, raw sum = %lld, dc cancel code = %d\r\n",
                     *p_dre_agc_info->agc_cnt.analysis_cnt, rawdata, dc_rawdata,
                     *p_dre_agc_info->agc_store_data.rawdata_sum, dc_cancel_code);
}

/**
 * @fn     gh_agc_dre_pre_data_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data,
                                       gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 *
 * @brief  dre agc data pre process
 *
 * @attention
 *
 * @param[in]   p_agc, p_gh_data, p_dre_agc_info, p_agc_common_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_pre_data_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data,
                                        gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    DEBUG_LOG("[DRE AGC][DATA PRE]start data pre process.\r\n");
    uint8_t agc_common_index = 0;
    gh_hal_ppg_data_t ppg_dre_dc_info = p_gh_data[p_dre_agc_info->data_index].ppg_dre_dc_info;

    //step 1:find data agc type(main/sub agc chnl) and response chnl index
    gh_get_data_agc_type(&ppg_dre_dc_info, p_dre_agc_info, p_agc);

    //step 2:convert main/sub data to agc dre data
    //agc_rawdata: rawdata
    p_dre_agc_info->rawdata = ppg_dre_dc_info.rawdata;
    p_dre_agc_info->agc_flag.sat_flag = ppg_dre_dc_info.sa_flag;
    p_dre_agc_info->agc_flag.dre_update_flag = ppg_dre_dc_info.dre_update;
    gh_agc_dre_data_create(p_agc, p_dre_agc_info);
    agc_common_index = gh_agc_dre_convert_common_index(p_dre_agc_info->agc_configure.slotcfg_rx);
    p_dre_agc_info->agc_common_index = agc_common_index;
    *p_dre_agc_info->agc_current_param.dc_cancel_code = (uint8_t)ppg_dre_dc_info.dc_cancel_code;

    //init flag
    // p_agc_common_info->init_flag[agc_common_index] = *p_dre_agc_info->agc_flag.init_flag;
    uint8_t init_flag = p_agc_common_info->init_flag[agc_common_index];

    //step 3:data process
    if (0 == p_dre_agc_info->agc_configure.agc_en || 0 == p_dre_agc_info->agc_configure.agc_drv_en)
    {
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] agc disable, return\r\n");
        return;
    }

    (*p_dre_agc_info->agc_cnt.analysis_cnt)++;

    // do not use data during inite and param change
    if (0 == init_flag)
    {
        p_dre_agc_info->drop_len =  p_dre_agc_info->agc_configure.sample_rate / (uint8_t)GH_AGC_DROP_FACTOR;
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] init, drop 1/5 data\r\n");
    }

    if (ppg_dre_dc_info.param_change_flag
        || (p_dre_agc_info->drop_len == *p_dre_agc_info->agc_cnt.analysis_cnt && 0 < p_dre_agc_info->drop_len))
    {
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] change flag = %d, cnt = %d drop len = %d, rest agc cnt\r\n",
                     ppg_dre_dc_info.param_change_flag, *p_dre_agc_info->agc_cnt.analysis_cnt,
                     p_dre_agc_info->drop_len);
        *p_dre_agc_info->agc_cnt.analysis_cnt = MAX(1, p_dre_agc_info->drop_len);
        *p_dre_agc_info->agc_cnt.sat_cnt = 0;
        *p_dre_agc_info->agc_cnt.sat_chceked_cnt = 0;
        *p_dre_agc_info->agc_cnt.dre_fre_cnt = 0;
        *p_dre_agc_info->agc_cnt.dre_fre_chceked_cnt = 0;
        *p_dre_agc_info->agc_cnt.ipd_tcnt = 0;
        *p_dre_agc_info->agc_cnt.gain_tcnt = 0;
        *p_dre_agc_info->agc_cnt.fluc_tcnt = 0;
        *p_dre_agc_info->agc_flag.init_flag = 1;
    }

    // process saturation only during normal data and no inite data
    if (0 == p_dre_agc_info->drop_len
        || (p_dre_agc_info->drop_len < *p_dre_agc_info->agc_cnt.analysis_cnt && 0 < p_dre_agc_info->drop_len))
    {
        DEBUG_DP_LOG("[DRE AGC][DATA PRE] set sat flow\r\n");
        p_dre_agc_info->agc_flow.sat_flow = 1;
    }

    // reset store data when inite or param change
    gh_agc_dre_data_update_and_process(p_agc, p_dre_agc_info, &ppg_dre_dc_info, p_agc_common_info);

    p_agc_common_info->init_flag[agc_common_index] = *p_dre_agc_info->agc_flag.init_flag;

    DEBUG_AP_DRV_LOG("[DRE AGC][DATA PRE] cnt = %d, mean raw = %d, mean ipd = %d\r\n",
                     *p_dre_agc_info->agc_cnt.analysis_cnt, *p_dre_agc_info->agc_store_data.mean_rawdata,
                     p_agc_common_info->mean_ipd_ary[p_dre_agc_info->slot_idex]);
    DEBUG_AP_DRV_LOG("[DRE AGC][DATA PRE] cnt = %d, sat flow = %d, gain drv flow = %d, full data flow = %d\r\n",
                     *p_dre_agc_info->agc_cnt.analysis_cnt, p_dre_agc_info->agc_flow.sat_flow,
                     p_dre_agc_info->agc_flow.gain_drv_flow,
                     p_dre_agc_info->agc_flow.adj_full_data_flow);
}

/**
 * @fn     gh_agc_dre_sat_adj_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info, uint8_t agc_sat_flag)
 *
 * @brief  dre agc sat adj process
 *
 * @attention
 *
 * @param[in]   p_dre_agc_info, p_agc_common_info, agc_sat_flag
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_sat_adj_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info,
                                       uint8_t agc_sat_flag)
{
    uint8_t agc_common_index = p_dre_agc_info->agc_common_index;
    int32_t mean_rawdata = *p_dre_agc_info->agc_store_data.mean_rawdata;
    uint8_t gain = *p_dre_agc_info->agc_current_param.current_gain;
    switch (agc_sat_flag)
    {
        case GH_PPG_NO_SAT:
            p_agc_common_info->agc_flag[agc_common_index] |= GH_PPG_NO_AGC;
            break;
        case GH_PPG_SERIOUS_SAT:
        case GH_PPG_SERIOUS_DRE_FRE:
            *p_dre_agc_info->agc_current_param.current_gain = MAX(gain - 1, 0);
            *p_dre_agc_info->agc_current_param.current_drv = p_dre_agc_info->agc_configure.min_led_drv;
            p_agc_common_info->agc_flag[agc_common_index] |= (1 << GH_PPG_GAIN_ADJ) | (1 << GH_PPG_DRV_ADJ);
            break;
        case GH_PPG_SAT:
        case GH_PPG_DRE_FRE:
            *p_dre_agc_info->agc_current_param.current_gain = MAX(gain - 1, 0);
            p_agc_common_info->agc_flag[agc_common_index] |= (1 << GH_PPG_GAIN_ADJ);
            break;
        default:
            break;
    }

    DEBUG_SP_LOG("[DRE AGC][SAT] sat adj process: agc_sat_flag = %d, agc_common_index = %d, agc_flag = %d.\r\n",
                 agc_sat_flag, agc_common_index, p_agc_common_info->agc_flag[agc_common_index]);

    if (agc_sat_flag)
    {
        DEBUG_SP_LOG("[DRE AGC][SAT] sat adj process: sat reset cnt.\r\n");
        p_dre_agc_info->agc_flow.adj_full_data_flow = 0;
        *p_dre_agc_info->agc_cnt.window_cnt = 0;
        *p_dre_agc_info->agc_cnt.ipd_tcnt = 0;
        p_agc_common_info->gain[agc_common_index] = *p_dre_agc_info->agc_current_param.current_gain;
        p_agc_common_info->drv_code[agc_common_index] = *p_dre_agc_info->agc_current_param.current_drv;
    }
    else
    {
        if (p_dre_agc_info->agc_flow.adj_full_data_flow)
        {
            uint8_t  conver_level = GH_AGC_CONVERGENCE_LEVEL;
            int32_t ideal_rawdata = GH_AGC_GENE_IDEAL_VALUE * GH_AGC_THD24_FACTOR;
            int32_t dc_cancel_raw = p_dre_agc_info->agc_store_data.dc_cancel_code_rawdata;

            if ((((mean_rawdata + dc_cancel_raw) * conver_level < (ideal_rawdata * (conver_level-1)))
                || ((mean_rawdata + dc_cancel_raw) * conver_level > (ideal_rawdata * (conver_level+1))))
                && ((*p_dre_agc_info->agc_store_data.max_rawdata + dc_cancel_raw)
                < ((*p_dre_agc_info->agc_store_data.min_rawdata + dc_cancel_raw) << 1)))
            {
                if (p_dre_agc_info->agc_flag.init_flag)//Inite First time
                {
                    p_dre_agc_info->agc_flow.gain_drv_flow = GH_PPG_GAIN_ADJ | GH_PPG_DRV_ADJ;
                }
                else
                {
                    (*p_dre_agc_info->agc_cnt.window_cnt)++;
                    if (*p_dre_agc_info->agc_cnt.window_cnt >= GH_AGC_ANALYSE_SEC)
                    {
                        p_dre_agc_info->agc_flow.gain_drv_flow = GH_PPG_GAIN_ADJ | GH_PPG_DRV_ADJ;
                    }
                }
            }
            else
            {
                *p_dre_agc_info->agc_cnt.window_cnt  = 0;
            }
        }//if (p_dre_agc_info->agc_flow.adj_full_data_flow)
    }//else (!agc_sat_flag)
}

/**
 * @fn     gh_agc_dre_sat_main_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info
 *                                      uint8_t* p_satflag)
 *
 * @brief  dre agc sat process
 *
 * @attention
 *
 * @param[in]   p_dre_agc_info, p_agc_common_info, p_satflag
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_sat_main_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info,
                                        uint8_t* p_satflag)
{
    // if get full data, process data saturation process
    int32_t last_mean_rawdata = *p_dre_agc_info->agc_store_data.last_mean_rawdata;
    int8_t sat_flag_last = last_mean_rawdata & (1 << GH_AGC_LAST_SAT_BIT);
    int8_t dre_fre_flag_last = last_mean_rawdata & (1 << GH_AGC_LAST_DRE_BIT);
    int32_t mean_rawdata = *p_dre_agc_info->agc_store_data.mean_rawdata;

    DEBUG_SP_LOG("[DRE AGC][SAT] last sat = %d, last dre = %d, before sat flag = %d.\r\n",
                 sat_flag_last, dre_fre_flag_last, *p_satflag);

    // seriously saturation
    if (GH_PPG_SAT == (*p_satflag))
    {
        if (sat_flag_last)
        {
            DEBUG_SP_LOG("[DRE AGC][SAT] serious sat condition !!!\r\n");
            (*p_satflag) = GH_PPG_SERIOUS_SAT;
            *p_dre_agc_info->agc_flag.init_flag = 0;
        }
        sat_flag_last = 1 << GH_AGC_LAST_SAT_BIT;
    }

    // seriously frequent adjustment of dre
    if (GH_PPG_DRE_FRE == (*p_satflag))
    {
        if (dre_fre_flag_last)
        {
            DEBUG_SP_LOG("[DRE AGC][SAT] serious fre dre adj condition !!!\r\n");
            (*p_satflag) = GH_PPG_SERIOUS_DRE_FRE;
            *p_dre_agc_info->agc_flag.init_flag = 0;
        }
        dre_fre_flag_last = 1 << GH_AGC_LAST_DRE_BIT;
    }

    if (0 == *p_dre_agc_info->agc_cnt.sat_cnt && GH_PPG_SAT != (*p_satflag)
        && GH_PPG_SERIOUS_SAT != (*p_satflag))
    {
        sat_flag_last = 0;
    }

    if (0 == *p_dre_agc_info->agc_cnt.dre_fre_cnt && GH_PPG_DRE_FRE != (*p_satflag)
        && GH_PPG_SERIOUS_DRE_FRE != (*p_satflag))
    {
        dre_fre_flag_last = 0;
    }

    *p_dre_agc_info->agc_store_data.last_mean_rawdata = (mean_rawdata & GH_AGC_LAST_RAW_MASK)
                                                        | sat_flag_last
                                                        | dre_fre_flag_last;

    DEBUG_SP_LOG("[DRE AGC][SAT] sat: sat cnt = %d, mix sat flag = %d, check = %d, mean rawd = %d, thr = %d.\r\n",
                 *p_dre_agc_info->agc_cnt.sat_cnt, p_dre_agc_info->agc_flag.sat_flag,
                 *p_dre_agc_info->agc_cnt.sat_chceked_cnt, mean_rawdata, p_agc_common_info->agc_thres.high_thd);
    DEBUG_SP_LOG("[DRE AGC][SAT] dre: dre update flag = %d, dre cnt = %d, dre check = %d.\r\n",
                 p_dre_agc_info->agc_flag.dre_update_flag, *p_dre_agc_info->agc_cnt.dre_fre_cnt,
                 *p_dre_agc_info->agc_cnt.dre_fre_chceked_cnt);
    DEBUG_SP_LOG("[DRE AGC][SAT] last sat = %d, last dre = %d, final sat flag = %d.\r\n",
                 sat_flag_last, dre_fre_flag_last, *p_satflag);
}

/**
 * @fn     gh_agc_dre_sat_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 *
 * @brief  dre agc sat process
 *
 * @attention
 *
 * @param[in]   p_dre_agc_info, p_agc_common_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_sat_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    if (!p_dre_agc_info->agc_flow.sat_flow || !p_dre_agc_info->agc_configure.agc_en
        || 0 == p_dre_agc_info->agc_configure.agc_drv_en
        || p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index])
    {
        DEBUG_LOG("[DRE AGC][SAT] sat_flow = %d, agc_en = %d, drv_en = %d, agc_flag = %d, sat process return.\r\n",
                  p_dre_agc_info->agc_flow.sat_flow, p_dre_agc_info->agc_configure.agc_en,
                  p_dre_agc_info->agc_configure.agc_drv_en,
                  p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index]);
        return;
    }

    DEBUG_LOG("[DRE AGC][SAT]start data sat process.\r\n");

    int32_t rawdata = p_dre_agc_info->rawdata;
    int32_t high_thd = p_agc_common_info->agc_thres.high_thd;
    uint8_t agc_sat_flag = GH_PPG_NO_SAT;

    // determine whether data is saturated
    gh_agc_dre_satflag_set(high_thd, rawdata, p_dre_agc_info->agc_flag.sat_flag,
                           p_dre_agc_info->agc_cnt.sat_chceked_cnt,
                           p_dre_agc_info->agc_cnt.sat_cnt, &agc_sat_flag,
                           p_dre_agc_info->agc_configure.sample_rate);

    // determine whether data is frequent adjustment of dre
    if (GH_PPG_NO_SAT == agc_sat_flag)
    {
        gh_agc_dre_fre_flag_set(p_dre_agc_info->agc_flag.dre_update_flag, p_dre_agc_info->agc_cnt.dre_fre_chceked_cnt,
                                p_dre_agc_info->agc_cnt.dre_fre_cnt, &agc_sat_flag,
                                p_dre_agc_info->agc_configure.sample_rate);
    }

    // if get full data, process data saturation process
    gh_agc_dre_sat_main_process(p_dre_agc_info, p_agc_common_info, &agc_sat_flag);

    //change gain an drv base on sat flag:
    // for normal sat and dre fre: gain--;
    // for serious condition: gain--, drv = min
    gh_agc_dre_sat_adj_process(p_dre_agc_info, p_agc_common_info, agc_sat_flag);

    p_dre_agc_info->agc_flow.sat_flow = 0;
}

/**
 * @fn   void gh_agc_gain_increase_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 * @brief    increase gain adj
 *
 * @attention
 *
 * @param[in]    *p_dre_agc_info, *p_agc_common_info
 * @param[out]   current_gain
 *
 * @return
 */
static uint8_t gh_agc_gain_increase_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    int32_t high_thd = p_agc_common_info->agc_thres.high_thd;
    uint8_t gain_limit = MAX(p_dre_agc_info->agc_configure.agc_gain_limit - 1, 0);
    uint8_t current_gain = *p_dre_agc_info->agc_current_param.current_gain;
    uint8_t agc_common_index = p_dre_agc_info->agc_common_index;

    DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain process start, increase.\r\n");

    //adj gain for little ctr
    int32_t mean_ipd = p_agc_common_info->mean_ipd_ary[p_dre_agc_info->slot_idex];
    if ((mean_ipd < p_agc_common_info->agc_thres.low_ipd_thd) && (current_gain == gain_limit))
    {
        current_gain = gain_limit + (uint8_t)1;
        p_agc_common_info->agc_flag[agc_common_index] |= (1 << GH_PPG_GAIN_ADJ);
    }

    //adj gain increase
    *p_dre_agc_info->agc_cnt.gain_tcnt = 0;
    uint32_t  times = 1 * GH_AGC_TIMES_FACTOR;
    int32_t temp_max_rawdata = 0;
    int32_t temp_min_rawdata = 0;

    if (*p_dre_agc_info->agc_store_data.min_rawdata > -high_thd
        && *p_dre_agc_info->agc_store_data.min_rawdata < high_thd)
    {
        DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain increase enter.\r\n");
        if ((current_gain) < gain_limit)
        {
            times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_dre_agc_info->p_gain_value[(current_gain) + 1])
                    / p_dre_agc_info->p_gain_value[(current_gain)];
        }

        temp_max_rawdata = (int64_t)(*p_dre_agc_info->agc_store_data.min_rawdata) * times / GH_AGC_TIMES_FACTOR;
        temp_min_rawdata = (int64_t)(*p_dre_agc_info->agc_store_data.min_rawdata) * times / GH_AGC_TIMES_FACTOR;

        //In order to prevent nonlinearity from causing repeated adjustments
        //a certain margin is left in the gain increase judgment.
        while ((GH_AGC_EXTREMUM_FATOR * temp_max_rawdata < GH_AGC_THD_FATOR * high_thd)
                && (GH_AGC_EXTREMUM_FATOR * temp_min_rawdata > GH_AGC_THD_FATOR * (-high_thd))
                && (current_gain < gain_limit))
        {
            if (GH_AGC_GAIN_CHANGE_GEAR_MAX > current_gain - current_gain)
            {
                times = (uint32_t)(GH_AGC_TIMES_FACTOR * p_dre_agc_info->p_gain_value[(current_gain) + 1])
                            / p_dre_agc_info->p_gain_value[(current_gain)];
                temp_max_rawdata = (int64_t)temp_max_rawdata * times / GH_AGC_TIMES_FACTOR;
                temp_min_rawdata = (int64_t)temp_min_rawdata * times / GH_AGC_TIMES_FACTOR;
                (current_gain)++;
                p_agc_common_info->agc_flag[agc_common_index] |= (1 << GH_PPG_GAIN_ADJ);
            }
            else
            {
                break;
            }
        }
    }//if (min_rawdata > -high_thd && max_rawdata < high_thd)

    return current_gain;
}

/**
 * @fn   void gh_agc_gain_decrease_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 * @brief    decrease gain adj
 *
 * @attention
 *
 * @param[in]    *p_dre_agc_info, *p_agc_common_info
 * @param[out]   current_gain
 *
 * @return
 */
static uint8_t gh_agc_gain_decrease_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    int32_t high_thd = p_agc_common_info->agc_thres.high_thd;
    uint8_t current_gain = *p_dre_agc_info->agc_current_param.current_gain;

    DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain process start, decrease.\r\n");

    if (((*p_dre_agc_info->agc_store_data.min_rawdata) - (*p_dre_agc_info->agc_store_data.min_rawdata))
              > (GH_AGC_EXTREMUM_FATOR * high_thd / GH_AGC_THD_FATOR))
    {
        DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain decrease process, add cnt.\r\n");
        (*p_dre_agc_info->agc_cnt.fluc_tcnt)++;
    }
    else
    {
        if (0 == *p_dre_agc_info->agc_cnt.gain_tcnt % p_dre_agc_info->agc_configure.sample_rate)
        {
            *p_dre_agc_info->agc_cnt.fluc_tcnt = 0;
        }
    }

    if ((p_dre_agc_info->agc_configure.sample_rate >> 1) < *p_dre_agc_info->agc_cnt.fluc_tcnt)
    {
        *p_dre_agc_info->agc_cnt.fluc_tcnt = 0;
        current_gain = MAX(0, current_gain-1);
        p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index] |= (1 << GH_PPG_GAIN_ADJ);
    }

    return current_gain;
}

/**
 * @fn   void gh_agc_extrem_gain_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 * @brief    adjust gain by extremum
 *
 * @attention
 *
 * @param[in]    *p_dre_agc_info, *p_agc_common_info
 * @param[out]   none
 *
 * @return
 */
static void gh_agc_extrem_gain_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    uint8_t current_gain = *p_dre_agc_info->agc_current_param.current_gain;
    uint8_t agc_common_index = p_dre_agc_info->agc_common_index;

    if (GH_MAX_RAWDATA == *p_dre_agc_info->agc_store_data.min_rawdata)
    {
       return;
    }

    (*p_dre_agc_info->agc_cnt.gain_tcnt)++;

    if (*p_dre_agc_info->agc_cnt.gain_tcnt >= GH_AGC_GAIN_CHECK_SEC * p_dre_agc_info->agc_configure.sample_rate)
    {
        //increase gain
        current_gain = gh_agc_gain_increase_cal(p_dre_agc_info, p_agc_common_info);
    }
    else
    {
        //decrease gain
        current_gain = gh_agc_gain_decrease_cal(p_dre_agc_info, p_agc_common_info);
    }

    DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] common idx = %d, g_cnt = %d, f_cnt = %d.\r\n",
                      p_dre_agc_info->agc_common_index,
                      *p_dre_agc_info->agc_cnt.gain_tcnt, *p_dre_agc_info->agc_cnt.fluc_tcnt);
    DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] common idx = %d,  mix max = %d %d, thrs = %d %d, g = %d, new g = %d.\r\n",
                      p_dre_agc_info->agc_common_index,
                      *p_dre_agc_info->agc_store_data.min_rawdata, *p_dre_agc_info->agc_store_data.min_rawdata,
                      -p_agc_common_info->agc_thres.high_thd, p_agc_common_info->agc_thres.high_thd,
                      *p_dre_agc_info->agc_current_param.current_gain, current_gain);

    *p_dre_agc_info->agc_current_param.current_gain = current_gain;
    p_agc_common_info->gain[agc_common_index] = current_gain;
}

/**
 * @fn   uint8_t gh_agc_extremum_drv_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 * @brief    calculate agc drv
 *
 * @attention
 *
 * @param[in]    *p_dre_agc_info, *p_agc_common_info
 * @param[out]   none
 *
 * @return
 */
static uint8_t gh_agc_extremum_drv_cal(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    int32_t mean_ipd = p_agc_common_info->mean_ipd_ary[p_dre_agc_info->slot_idex];
    int32_t ideal_ipd = ((p_agc_common_info->agc_thres.high_ipd_thd + p_agc_common_info->agc_thres.low_ipd_thd)
                        >> 1);
    uint8_t min_led_drv_code = p_dre_agc_info->agc_configure.min_led_drv;
    uint8_t max_led_drv_code = p_dre_agc_info->agc_configure.max_led_drv;
    uint8_t current_drv = *p_dre_agc_info->agc_current_param.current_drv;
    uint8_t new_drv_code = 0;

    (*p_dre_agc_info->agc_cnt.ipd_tcnt)++;
    if (mean_ipd < p_agc_common_info->agc_thres.low_ipd_thd)
    {
        DEBUG_AP_DRV_LOG("[DRE AGC][ADJ][DRV] drv cal: increase drv.\r\n");
        new_drv_code = gh_agc_dre_led_drv_calc(mean_ipd, ideal_ipd, current_drv, min_led_drv_code, max_led_drv_code);
        *p_dre_agc_info->agc_current_param.current_drv = new_drv_code;
        p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index] |= (1 << GH_PPG_DRV_ADJ);
    }
    else
    {
        if (*p_dre_agc_info->agc_cnt.ipd_tcnt >= GH_AGC_DRV_CHECK_SEC)
        {
            *p_dre_agc_info->agc_cnt.ipd_tcnt = 0;
             if (mean_ipd > p_agc_common_info->agc_thres.high_ipd_thd)
            {
                DEBUG_AP_DRV_LOG("[DRE AGC][ADJ][DRV] drv cal: decrease drv.\r\n");
                new_drv_code = gh_agc_dre_led_drv_calc(mean_ipd, ideal_ipd, current_drv,
                                                       min_led_drv_code, max_led_drv_code);
                *p_dre_agc_info->agc_current_param.current_drv = new_drv_code;
                p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index] |= (1 << GH_PPG_DRV_ADJ);
            }
        }
    }

    if (new_drv_code)
    {
        p_agc_common_info->drv_code[p_dre_agc_info->agc_common_index] = new_drv_code;
    }
    DEBUG_AP_DRV_LOG("[DRE AGC][ADJ][DRV] drv cal: ipd cnt = %d, cnt trs = %d, drv trs = %d %d, ipd trs = %d %d.\r\n",
                     *p_dre_agc_info->agc_cnt.ipd_tcnt, GH_AGC_DRV_CHECK_SEC, min_led_drv_code, max_led_drv_code,
                     p_agc_common_info->agc_thres.low_ipd_thd, p_agc_common_info->agc_thres.high_ipd_thd);
    DEBUG_AP_DRV_LOG("[DRE AGC][ADJ][DRV] drv cal: mean ipd = %d, ideal ipd = %d, current drv = %d, new drv = %d.\r\n",
                     mean_ipd, ideal_ipd, current_drv, new_drv_code);
    return new_drv_code;
}

/**
 * @fn     gh_agc_dre_adj_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
 *
 * @brief  dre agc gain and drv adj process
 *
 * @attention
 *
 * @param[in]   p_dre_agc_info, p_agc_common_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_adj_process(gh_dre_agc_info_t *p_dre_agc_info, gh_agc_common_info_t *p_agc_common_info)
{
    if (!p_dre_agc_info->agc_flow.gain_drv_flow || !p_dre_agc_info->agc_configure.agc_en
        || 0 == p_dre_agc_info->agc_configure.agc_drv_en)
    {
        DEBUG_LOG("[DRE AGC][ADJ] sat flow = %d, agc_en = %d, agc_drv_en = %d, adj process return.\r\n",
                  p_dre_agc_info->agc_flow.sat_flow, p_dre_agc_info->agc_configure.agc_en,
                  p_dre_agc_info->agc_configure.agc_drv_en);
        return;
    }

    DEBUG_LOG("[DRE AGC][ADJ] start data adj process.\r\n");

    uint8_t new_drv_code = 0;

#if GH_AGC_DRE_DRV_ADJ_EN
    // drv adj
    if ((p_dre_agc_info->agc_flow.gain_drv_flow & GH_PPG_DRV_ADJ)
        && (GH_PPG_AGC_MAIN == p_dre_agc_info->agc_main_sub_type))
    {
        new_drv_code = gh_agc_extremum_drv_cal(p_dre_agc_info, p_agc_common_info);
    }
#endif

#if GH_AGC_DRE_GAIN_ADJ_EN
    // gain adj
    if ((p_dre_agc_info->agc_flow.gain_drv_flow & GH_PPG_GAIN_ADJ) != 0)
    {
        int32_t rawdata_max = *p_dre_agc_info->agc_store_data.max_rawdata;
        int32_t rawdata_min = *p_dre_agc_info->agc_store_data.min_rawdata;

        if ((p_agc_common_info->agc_flag[p_dre_agc_info->agc_common_index] & (1 << GH_PPG_DRV_ADJ))
            != 0)
        {
            *p_dre_agc_info->agc_store_data.max_rawdata = rawdata_max * new_drv_code
                                                        / (*p_dre_agc_info->agc_current_param.current_drv);
            *p_dre_agc_info->agc_store_data.min_rawdata = rawdata_min * new_drv_code
                                                        / (*p_dre_agc_info->agc_current_param.current_drv);
            DEBUG_LOG("[DRE AGC][ADJ][GAIN] gain adj process, update max and min due to drv change.\r\n");
            DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain adj, new drv= %d, drv = %d.\r\n",
                              new_drv_code, *p_dre_agc_info->agc_current_param.current_drv);
            DEBUG_AP_GAIN_LOG("[DRE AGC][ADJ][GAIN] gain adj, new max min = %d %d, max min %d %d.\r\n",
                              *p_dre_agc_info->agc_store_data.max_rawdata,
                              *p_dre_agc_info->agc_store_data.min_rawdata,
                              rawdata_max, rawdata_min);
        }

        gh_agc_extrem_gain_cal(p_dre_agc_info, p_agc_common_info);
    }
#endif

    DEBUG_AP_LOG("[DRE AGC][ADJ] adj flow = %d, drv = %d, gain = %d.\r\n",
                 p_dre_agc_info->agc_flow.gain_drv_flow, *p_dre_agc_info->agc_current_param.current_drv,
                 *p_dre_agc_info->agc_current_param.current_gain);

    p_dre_agc_info->agc_flow.gain_drv_flow = 0;
}

/**
 * @fn     gh_agc_dre_update_process(gh_hal_agc_t* p_agc, gh_agc_common_info_t *p_agc_common_info)
 *
 * @brief  update dre param
 *
 * @attention
 *
 * @param[in]   p_agc, p_agc_common_info
 * @param[out]  none
 *
 * @return
 */
static void gh_agc_dre_update_process(gh_hal_agc_t* p_agc, gh_agc_common_info_t *p_agc_common_info)
{
    DEBUG_LOG("[DRE AGC]start param update process.\r\n");
    uint8_t param_updata_flag = 0;
    for (uint8_t slotcfg = 0; slotcfg < GH_AGC_DRE_CHNL_NUM; slotcfg++)
    {
        uint8_t agc_flag = p_agc_common_info->agc_flag[slotcfg];

        if (agc_flag)
        {
            uint8_t slot_num = slotcfg / (uint8_t)GH_RX_NUM;
            uint8_t rx_num = slotcfg % (uint8_t)GH_RX_NUM;
            uint8_t drv1_en = p_agc_common_info->drv_en[slotcfg] >> (uint8_t)1;
            uint8_t drv0_en = p_agc_common_info->drv_en[slotcfg] & (uint8_t)1;

            if (0 == param_updata_flag)
            {
                param_updata_flag = 1;

                // write param start
                p_agc->param_update_start();
            }

            if (((1 << GH_PPG_GAIN_ADJ) & agc_flag) != 0)// update gain
            {
                p_agc->tia_gain_write(slot_num, rx_num, p_agc_common_info->gain[slotcfg]);

                if (GH_TIA_GIAN0_IDX == rx_num)
                {
                    p_agc->agc_chip_info.slotcfg_agc_param[slot_num].gain0_code = p_agc_common_info->gain[slotcfg];
                }
                else if (GH_TIA_GIAN1_IDX == rx_num)
                {
                    p_agc->agc_chip_info.slotcfg_agc_param[slot_num].gain1_code = p_agc_common_info->gain[slotcfg];
                }
            }

            if (((1 << GH_PPG_DRV_ADJ) & agc_flag) != 0)//update drv
            {
                if (drv0_en)
                {
                    DEBUG_UP_LOG("[DRE AGC][UPDATE] drv update, slot = %d, rx = %d, drv num = 0, before drv = %d,\r\n",
                                 slot_num, rx_num, p_agc->agc_chip_info.slotcfg_agc_param[slot_num].led_drv0_code);

                    p_agc->led_drv_write(slot_num, GH_LED_DRV0_IDX, p_agc_common_info->drv_code[slotcfg]);
                    p_agc->agc_chip_info.slotcfg_agc_param[slot_num].led_drv0_code =
                                                                             p_agc_common_info->drv_code[slotcfg];
                }

                if (drv1_en)
                {
                    DEBUG_UP_LOG("[DRE AGC][UPDATE] drv update, slot = %d, rx = %d, drv num = 1, before drv = %d,\r\n",
                                 slot_num, rx_num, p_agc->agc_chip_info.slotcfg_agc_param[slot_num].led_drv0_code);

                    p_agc->led_drv_write(slot_num, GH_LED_DRV1_IDX, p_agc_common_info->drv_code[slotcfg]);
                    p_agc->agc_chip_info.slotcfg_agc_param[slot_num].led_drv1_code =
                                                                             p_agc_common_info->drv_code[slotcfg];
                }
                DEBUG_UP_LOG("[DRE AGC][UPDATE] drv update, slot = %d, rx = %d, drv en = %d, after drv = %d,\r\n",
                             slot_num, rx_num, p_agc_common_info->drv_en[slotcfg],
                             p_agc->agc_chip_info.slotcfg_agc_param[slot_num].led_drv0_code);
            }
            p_agc_common_info->agc_flag[slotcfg] = 0;
        }//if (agc_flag)
    }//for (uint8_t slotcfg = 0; slotcfg < GH_AGC_DRE_CHNL_NUM; slotcfg++)

    if (param_updata_flag)
    {
        // write param end
        p_agc->param_update_end();
    }
}

/**
 * @fn     gh_agc_dre_get_condition(gh_data_t *p_gh_data, gh_agc_common_info_t *p_agc_common_info,
 *                                  uint16_t data_idx)
 *
 * @brief  get agc conditon
 *
 * @attention
 *
 * @param[in]   p_gh_data, p_agc_common_info, data_idx
 * @param[out]  already_agc
 *
 * @return
 */
uint8_t gh_agc_dre_get_agc_condition(gh_data_t *p_gh_data, gh_agc_common_info_t *p_agc_common_info,
                                     uint16_t data_idx)
{
    uint8_t already_agc = 0;
    uint8_t slot_cfg_id = p_gh_data[data_idx].data_channel.channel_ppg_dre_dc_info.slot_cfg_id;
    uint8_t rx_id = p_gh_data[data_idx].data_channel.channel_ppg_dre_dc_info.rx_id;
    uint8_t common_index = slot_cfg_id * GH_RX_NUM + rx_id;
    if (p_agc_common_info->agc_flag[common_index])
    {
        DEBUG_LOG("[DRE AGC] return dre agc, already agc, wait for param upadate.\r\n");
        DEBUG_LOG("[DRE AGC] idx = %d, slot = %d, rx = %d, agc flag = %d.\r\n",
                  data_idx, slot_cfg_id, rx_id, g_agc_common_info.agc_flag[common_index]);
        already_agc = 1;
    }
    else
    {
        already_agc = 0;
    }
    return already_agc;
}

/**
 * @fn     gh_agc_dre_common_reset(void)
 *
 * @brief  reset dre agc_common_info
 *
 * @attention
 *
 * @param[in]   none
 * @param[out]  none
 *
 * @return
 */
void gh_agc_dre_common_reset(void)
{
    gh_memset(&g_agc_common_info, 0, sizeof(gh_agc_common_info_t));
}

/**
 * @fn     gh_agc_dre_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len)
 *
 * @brief  dre agc process
 *
 * @attention
 *
 * @param[in]   p_agc, p_gh_data, data_len
 * @param[out]  error code
 *
 * @return
 */
int32_t gh_agc_dre_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len)
{
    if (GH_NULL_PTR == p_agc || GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_PTR_NULL);
    }

    if (0 == data_len || 0 == p_agc->agc_slot_num)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_AGC_LEN_ERR);
    }

    g_agc_common_info.agc_thres.high_thd = (int32_t)p_agc->agc_confg.agc_gene_high_thld
                                           * (int32_t)GH_AGC_THD24_FACTOR;
    g_agc_common_info.agc_thres.low_thd = (int32_t)p_agc->agc_confg.agc_gene_low_thld
                                          * (int32_t)GH_AGC_THD24_FACTOR;
    g_agc_common_info.agc_thres.high_ipd_thd = (int32_t)p_agc->agc_confg.agc_gene_high_ipd
                                               * (int32_t)GH_AGC_IPD_NA_FACTOR;
    g_agc_common_info.agc_thres.low_ipd_thd = (int32_t)p_agc->agc_confg.agc_gene_low_ipd
                                              * (int32_t)GH_AGC_IPD_NA_FACTOR;
    g_agc_common_info.agc_thres.ideal_rawdata = (int32_t)GH_AGC_GENE_IDEAL_VALUE
                                                * (int32_t)GH_AGC_THD24_FACTOR;
    uint8_t datatype = 0;
    uint8_t fifoend = 0;
    uint8_t already_agc = 0;

    for (uint16_t data_idx = 0; data_idx < data_len; data_idx++)
    {
        datatype = p_gh_data[data_idx].data_channel.data_type;

        if (GH_PPG_DRE_DC_INFO != datatype)
        {
            //DEBUG_LOG("[DRE AGC] return dre agc, datatype = %d, idx = %d \r\n", datatype, data_idx);
            continue;
        }

        fifoend = p_gh_data[data_idx].common_data.fifo_end;

        already_agc = gh_agc_dre_get_agc_condition(p_gh_data, &g_agc_common_info, data_idx);

        if (!already_agc)
        {
            DEBUG_LOG("[DRE AGC] start dre agc, datatype = %d, idx = %d, slot = %d, rx = %d, fifo end = %d\r\n",
                      datatype, data_idx, p_gh_data[data_idx].data_channel.channel_ppg_dre_dc_info.slot_cfg_id,
                      p_gh_data[data_idx].data_channel.channel_ppg_dre_dc_info.rx_id, fifoend);

            gh_dre_agc_info_t dre_agc_info = {0};
            dre_agc_info.data_index = data_idx;

            //DRE AGC data Pre Process
            gh_agc_dre_pre_data_process(p_agc, p_gh_data, &dre_agc_info, &g_agc_common_info);

            //DRE AGC Main Process
            //DRE SAT Process
            gh_agc_dre_sat_process(&dre_agc_info, &g_agc_common_info);

            //DRE ADJ Process
            gh_agc_dre_adj_process(&dre_agc_info, &g_agc_common_info);
        }

        //Param Update Process
        if (fifoend)
        {
            gh_agc_dre_update_process(p_agc, &g_agc_common_info);
        }
    }//for (uint16_t data_idx = 0; data_idx < data_len; data_idx++)
    return RETURN_VALUE_ASSEMBLY(0, GH_AGC_OK);
}

#endif
#endif
