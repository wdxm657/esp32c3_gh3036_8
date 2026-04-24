/**
  ****************************************************************************************
  * @file    gh_hal_std.c
  * @author  GHealth Driver Team
  * @brief   gh hal std file
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
#include <stdint.h>
#include "gh_hal_config.h"
#include "gh_hal_chip.h"
#include "gh_hal_service.h"
#include "gh_hal_settings.h"
#include "gh_hal_common.h"
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_hal_std.h"

/*
 * DEFINES
 *****************************************************************************************
 */

#if GH_HAL_STD_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    ((uint32_t)(GH_HAL_STD_ID << 24 | (internal_err) << 8 | (interface_err)))

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

/**
  * @brief gain(kohm)
  */
static const int32_t g_gain_phy[] = {10, 25, 50, 100, 250, 500, 1000};

/**
  * @brief dc cancel full scale(uA)
  */
static const int32_t g_dc_bg_cancel_range[] = {32, 64, 128, 256};

#if GH_HAL_STD_CALI_EN
#if GH_HAL_STD_CALI_DRV_EN
/**
  * @brief led driver full scale(mv)
  */
static const int32_t g_led_range[] = {20, 40, 80, 160};

/**
  * @brief get cali drv current(uA)
  */
static int32_t gh_hal_std_drv_cali(gh_std_cali_led_param_t led_cali_param, gh_std_cali_drv_data_t drv_data)
{
    int32_t cali_y;
    int32_t cali_n;
    uint8_t drv_code;
    uint8_t drv_fs;
    int32_t cali_er;
    int32_t drv_current_cali;

    for (uint8_t i = 0; i < LED_CALI_FS_CODE_MAX + 1; i++)
    {
        if (g_led_range[i] == drv_data.drv_fs)
        {
            drv_fs = i;
        }
    }

    if (0 == drv_data.drv_num)
    {
        cali_y = LED_CALI_Y0;
        cali_er = LED_CALI_EXTEND + LED_CALI_EXTEND * (int32_t)led_cali_param.led0_er / (EIGHT_BIT_MAX + 1);
    }
    else
    {
        cali_y = LED_CALI_Y1;
        cali_er = LED_CALI_EXTEND + LED_CALI_EXTEND * (int32_t)led_cali_param.led1_er / (EIGHT_BIT_MAX + 1);
    }

    cali_n = 1 << (LED_CALI_FS_CODE_MAX - drv_fs);
    drv_code = drv_data.drv_code;

    drv_current_cali = ((int64_t)LED_CALI_DRV * LED_CALI_EXTEND * ROUND
                       / (cali_er * EIGHT_BIT_MAX * cali_n / drv_code + cali_y) + HALFROUND) / ROUND;

    DEBUG_LOG("[%s] drv num = %d, y= %d, er = %d, cali_n = %d, drv code = %d, drv_cali =%d \r\n",
              __FUNCTION__,
              drv_data.drv_num,
              cali_y,
              cali_er,
              cali_n,
              drv_code,
              drv_current_cali);

    return drv_current_cali;
}
#endif

#if GH_HAL_STD_CALI_GAIN_EN
/**
  * @brief get cali gain(ohm)
  */
static uint32_t gh_hal_std_gain_cali(gh_std_cali_gain_param_t gain_param, gh_std_cali_gain_data_t gain_data)
{
    uint32_t gain_cali;
    uint32_t gain_er;
    uint32_t gain;
    if (0 == gain_data.rx_num)
    {
        gain_er = GAIN_CALI_EXTEND + (GAIN_CALI_EXTEND * gain_param.gain_rx0_offset) / GAIN_CALI_MAX;
    }
    else
    {
        gain_er = GAIN_CALI_EXTEND + (GAIN_CALI_EXTEND * gain_param.gain_rx1_offset) / GAIN_CALI_MAX;
    }
    gain = KOM2OM * g_gain_phy[gain_data.gain_code];
    gain_cali = (uint32_t)(((uint64_t)gain * (uint64_t)gain_er + GAIN_CALI_HALF_EXTEND)/GAIN_CALI_EXTEND);
    DEBUG_LOG("[%s] rx = %d, er= %d, gain = %d, gain_cali = %d \r\n",
              __FUNCTION__,
              gain_data.rx_num,
              gain_er,
              g_gain_phy[gain_data.gain_code],
              gain_cali);
    return gain_cali;
}
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
/**
  * @brief get cali dc cancel(pa)
  */
static int32_t gh_hal_std_dc_cancel_cali(gh_std_cali_dc_param_t dc_cancel_param, gh_std_cali_dc_data_t dc_cancel_data)
{
    int64_t dc_cancel_cali;
    int32_t dc_cancel_current;
    int32_t a_er;
    int32_t b_er;
    int32_t dc_range = (int32_t) g_dc_bg_cancel_range[dc_cancel_data.dc_cancel_fs];
    int32_t dc_code = (int32_t) dc_cancel_data.dc_cancel_code;

    a_er = ((DC_CANCEL_CALI_EXTEND + DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_a
           / DC_CANCEL_CALI_A_MAX) * ROUND + HALFROUND) / ROUND;
    if (dc_cancel_param.dc_cancel_b > 0)
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND + HALFROUND)
               / ROUND;
    }
    else
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND - HALFROUND)
               / ROUND;
    }

    dc_cancel_current = ((int64_t)dc_code * UA2PA * DC_CANCEL_CALI_EXTEND / EIGHT_BIT_MAX  * (int64_t)dc_range
                        + DC_CANCEL_CALI_HALF_EXTEND) / DC_CANCEL_CALI_EXTEND;
    dc_cancel_cali = ((uint64_t)dc_cancel_current * (uint64_t)a_er
                     + (uint64_t)b_er + DC_CANCEL_CALI_HALF_EXTEND) / DC_CANCEL_CALI_EXTEND;
    DEBUG_LOG("[%s] a = %d, b = %d, code = %d, range = %d, idc =%d, cali_idc =%lld \r\n", __FUNCTION__, a_er, b_er,
              dc_code, dc_range, dc_cancel_current, dc_cancel_cali);
    return (int32_t)dc_cancel_cali;
}
#endif

#if GH_HAL_STD_CALI_BG_CANCEL_EN
/**
  * @brief get cali bg cancel(pa)
  */
static int32_t gh_hal_std_bg_cancel_cali(gh_std_cali_bg_param_t bg_cancel_param, gh_std_cali_bg_data_t bg_cancel_data)
{
    int64_t bg_cancel_cali;
    int64_t bg_cancel_current;
    int32_t a_er;
    int32_t b_er;
    int32_t bg_range = (int32_t) g_dc_bg_cancel_range[bg_cancel_data.bg_cancel_fs];
    int32_t bg_code = (int32_t) bg_cancel_data.bg_cancel_code;

    a_er = ((DC_CANCEL_CALI_EXTEND + DC_CANCEL_CALI_EXTEND * bg_cancel_param.bg_cancel_a / DC_CANCEL_CALI_A_MAX)
           * ROUND + HALFROUND) / ROUND;
    if (bg_cancel_param.bg_cancel_b > 0)
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * bg_cancel_param.bg_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND + HALFROUND)
               / ROUND;
    }
    else
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * bg_cancel_param.bg_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND - HALFROUND)
               / ROUND;
    }

    bg_cancel_current = ((int64_t)bg_code * UA2PA * DC_CANCEL_CALI_EXTEND / EIGHT_BIT_MAX  * (int64_t)bg_range
                        + DC_CANCEL_CALI_HALF_EXTEND) / DC_CANCEL_CALI_EXTEND;
    bg_cancel_cali = (bg_cancel_current * (int64_t)a_er + (int64_t)b_er + DC_CANCEL_CALI_HALF_EXTEND)
                     / DC_CANCEL_CALI_EXTEND;
    DEBUG_LOG("[%s] a = %d, b = %d, code = %d, range = %d, ibg =%lld, cali_ibg =%lld \r\n", __FUNCTION__, a_er, b_er,
              bg_code, bg_range, bg_cancel_current, bg_cancel_cali);
    return (int32_t)bg_cancel_cali;
}
#endif

#if GH_HAL_STD_CALI_RX_OFFSET_EN
/**
  * @brief get cali rx offset(uv)
  */
static int32_t gh_hal_std_rx_offset_cali(gh_std_cali_rx_param_t rx_offset_param, gh_std_cali_rx_data_t rx_offset_data)
{
    int32_t rx_offset_cali;
    int32_t offset_er;

    if (0 == rx_offset_data.rx_num)
    {
        offset_er = rx_offset_param.rx0_offset;
    }
    else
    {
        offset_er = rx_offset_param.rx1_offset;
    }

    rx_offset_cali = offset_er * MV2UV / RX_OFFSET_CALI_MAX;
    DEBUG_LOG("[%s] rx = %d, offset_er= %d, rx_offset_cali = %d \r\n",
              __FUNCTION__,
              rx_offset_data.rx_num,
              offset_er,
              rx_offset_cali);
    return rx_offset_cali;
}
#endif

#if GH_HAL_STD_CALI_RES_GAIN_EN
/**
  * @brief cali gain for more accurate result
  */
static int32_t gh_hal_std_res_gain_cali(gh_std_cali_gain_param_t gain_param,
                                        gh_std_cali_gain_data_t gain_data,
                                        int32_t cali_result)
{
    int32_t gain_er;
    if (0 == gain_data.rx_num)
    {
        gain_er = GAIN_CALI_EXTEND + (GAIN_CALI_EXTEND * gain_param.gain_rx0_offset) / GAIN_CALI_MAX;
    }
    else
    {
        gain_er = GAIN_CALI_EXTEND + (GAIN_CALI_EXTEND * gain_param.gain_rx1_offset) / GAIN_CALI_MAX;
    }

    if (cali_result > 0)
    {
        cali_result = (int32_t)(((int64_t)cali_result * (int64_t)GAIN_CALI_EXTEND + GAIN_CALI_HALF_EXTEND) / gain_er);
    }
    else
    {
        cali_result = (int32_t)(((int64_t)cali_result * (int64_t)GAIN_CALI_EXTEND - GAIN_CALI_HALF_EXTEND) / gain_er);
    }
    DEBUG_LOG("[%s] rx = %d, gain_er = %d, cali_result = %d \r\n", __FUNCTION__, gain_data.rx_num, gain_er,
              cali_result);
    return cali_result;
}
#endif

/**
  * @brief cali process
  */
static int32_t gh_hal_std_cali_process(gh_hal_std_parser_t* p_std_parser, gh_hal_std_cali_op_e cali_op)
{
    int32_t cali_result;
    switch (cali_op)
    {
#if GH_HAL_STD_CALI_DRV_EN
    case GH_HAL_STD_CALI_LED:
        cali_result = gh_hal_std_drv_cali(p_std_parser->cali_param.led_param, p_std_parser->cali_data.drv_data);
        break;
#endif

#if GH_HAL_STD_CALI_GAIN_EN
    case GH_HAL_STD_CALI_GAIN:
        cali_result = gh_hal_std_gain_cali(p_std_parser->cali_param.gain_param, p_std_parser->cali_data.gain_data);
        break;
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
    case GH_HAL_STD_CALI_DC_CANCEL:
        cali_result = gh_hal_std_dc_cancel_cali(p_std_parser->cali_param.dc_cancel_param,
                                                p_std_parser->cali_data.dc_cancel_data);
#if GH_HAL_STD_CALI_RES_GAIN_EN
        cali_result = gh_hal_std_res_gain_cali(p_std_parser->cali_param.gain_param,
                                               p_std_parser->cali_data.gain_data, cali_result);
#endif
        break;
#endif

#if GH_HAL_STD_CALI_BG_CANCEL_EN
    case GH_HAL_STD_CALI_BG_CANCEL:
        cali_result = gh_hal_std_bg_cancel_cali(p_std_parser->cali_param.bg_cancel_param,
                                                p_std_parser->cali_data.bg_cancel_data);
#if GH_HAL_STD_CALI_RES_GAIN_EN
        cali_result = gh_hal_std_res_gain_cali(p_std_parser->cali_param.gain_param,
                                               p_std_parser->cali_data.gain_data, cali_result);
#endif
        break;
#endif

#if GH_HAL_STD_CALI_RX_OFFSET_EN
    case GH_HAL_STD_CALI_RX_OFFSET:
        cali_result = gh_hal_std_rx_offset_cali(p_std_parser->cali_param.rx_param,
                                                p_std_parser->cali_data.rx_offset_data);
#if GH_HAL_STD_CALI_RES_GAIN_EN
        cali_result = gh_hal_std_res_gain_cali(p_std_parser->cali_param.gain_param,
                                               p_std_parser->cali_data.gain_data, cali_result);
#endif
        break;
#endif
    default:
        break;
    }
    return cali_result;
}
#endif

/**
  * @brief calculate ipd for ppg data
  */
static void gh_hal_std_ppgdata_cal(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
{
    int32_t idc_cancel = 0;
    int32_t dc_cancel_code = 0;
    int32_t dc_cancel_range = 0;
    int32_t gain = 0;
    int64_t rawdata = (int64_t)p_gh_data->ppg_data.rawdata;
    uint8_t cfg_index = p_gh_data->data_channel.channel_ppg.slot_cfg_id;

#if GH_HAL_STD_CALI_GAIN_EN
    p_std_parser->cali_data.gain_data.gain_code = p_gh_data->ppg_data.gain_code;
    p_std_parser->cali_data.gain_data.rx_num = p_gh_data->data_channel.channel_ppg.rx_id;
    gain = gh_hal_std_cali_process(p_std_parser, GH_HAL_STD_CALI_GAIN);
#else
    gain = KOM2OM * g_gain_phy[p_gh_data->ppg_data.gain_code];
#endif

    // get dc cancel code and range
    if (p_std_parser->p_settings->ppg_cfg_param[cfg_index].dc_cancel_en)
    {
        if (p_std_parser->p_settings->ppg_cfg_param[cfg_index].dre_en)
        {
            if (0 == p_gh_data->data_channel.channel_ppg.rx_id)
            {
                dc_cancel_code = (int32_t)p_std_parser->slot_cfg_param[cfg_index].dc_cancel_code_0;
                dc_cancel_range = (int32_t)p_std_parser->slot_cfg_param[cfg_index].dc_cancel_range_0;
            }
            else
            {
                dc_cancel_code = (int32_t)p_std_parser->slot_cfg_param[cfg_index].dc_cancel_code_1;
                dc_cancel_range = (int32_t)p_std_parser->slot_cfg_param[cfg_index].dc_cancel_range_1;
            }
        }
        else
        {
            dc_cancel_code = (int32_t)p_gh_data->ppg_data.dc_cancel_code;
            dc_cancel_range = (int32_t)p_gh_data->ppg_data.dc_cancel_range;
        }
    }
#if GH_HAL_STD_CALI_DC_CANCEL_EN
    p_std_parser->cali_data.dc_cancel_data.dc_cancel_code = dc_cancel_code;
    p_std_parser->cali_data.dc_cancel_data.dc_cancel_fs = dc_cancel_range;
    idc_cancel = gh_hal_std_cali_process(p_std_parser, GH_HAL_STD_CALI_DC_CANCEL);
#else
    dc_cancel_range = (int32_t)g_dc_bg_cancel_range[dc_cancel_range];
    idc_cancel = (int32_t)(((int64_t)dc_cancel_code * UA2PA * DC_CANCEL_CALI_EXTEND / EIGHT_BIT_MAX
                  * (int64_t)dc_cancel_range + DC_CANCEL_CALI_HALF_EXTEND) / DC_CANCEL_CALI_EXTEND);
#endif
    DEBUG_LOG("[%s] Idc = %d, dc_cancel_code = %d, dc_cancel_range = %d \r\n", __FUNCTION__,
              idc_cancel, dc_cancel_code, dc_cancel_range);

    // get dre rawdata
    if (p_std_parser->p_settings->ppg_cfg_param[cfg_index].dre_en)
    {
        rawdata = rawdata << (p_std_parser->p_settings->ppg_cfg_param[cfg_index].dre_scale + 1);
    }

    // get ipd
    p_gh_data->ppg_data.ipd_pa = gh_hal_common_ipd_cal(rawdata, gain, idc_cancel);
    DEBUG_LOG("[%s] data type:0x%x, ipd = %d, rawdata = %d \r\n", __FUNCTION__, p_gh_data->data_channel.data_type,
              p_gh_data->ppg_data.ipd_pa, p_gh_data->ppg_data.rawdata);
}

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
/**
  * @brief calculate ipd for bg data
  */
static void gh_hal_std_bgdata_cal(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
{
    int32_t gain = 0;
    int32_t rawdata = p_gh_data->ppg_bg_data.rawdata;

#if GH_HAL_STD_CALI_GAIN_EN
    p_std_parser->cali_data.gain_data.gain_code = p_gh_data->ppg_bg_data.gain_code;
    p_std_parser->cali_data.gain_data.rx_num = p_gh_data->data_channel.channel_ppg_bg.rx_id;
    gain = gh_hal_std_cali_process(p_std_parser, GH_HAL_STD_CALI_GAIN);
#else
    gain = KOM2OM * g_gain_phy[p_gh_data->ppg_bg_data.gain_code];
#endif
    p_gh_data->ppg_bg_data.ipd_pa = gh_hal_common_ipd_cal(rawdata, gain, 0);
    DEBUG_LOG("[%s] data type:0x%x, ipd = %d, rawdata = %d \r\n", __FUNCTION__, p_gh_data->data_channel.data_type,
              p_gh_data->ppg_bg_data.ipd_pa, p_gh_data->ppg_bg_data.rawdata);
}

/**
  * @brief calculate ipd for mix data
  */
static void gh_hal_std_mixdata_cal(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
{
    int32_t idc_cancel = 0;
#if !GH_HAL_STD_CALI_DC_CANCEL_EN
    int32_t dc_cancel_code = 0;
    int32_t dc_cancel_range = 0;
#endif
    int32_t gain = 0;
    int32_t rawdata = p_gh_data->ppg_mixdata.rawdata;

#if GH_HAL_STD_CALI_GAIN_EN
    p_std_parser->cali_data.gain_data.gain_code = p_gh_data->ppg_mixdata.gain_code;
    p_std_parser->cali_data.gain_data.rx_num = p_gh_data->data_channel.channel_ppg_mix.rx_id;
    gain = gh_hal_std_cali_process(p_std_parser, GH_HAL_STD_CALI_GAIN);
#else
    gain = KOM2OM * g_gain_phy[p_gh_data->ppg_mixdata.gain_code];
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
    p_std_parser->cali_data.dc_cancel_data.dc_cancel_code = p_gh_data->ppg_mixdata.dc_cancel_code;
    p_std_parser->cali_data.dc_cancel_data.dc_cancel_fs = p_gh_data->ppg_mixdata.dc_cancel_range;
    idc_cancel = gh_hal_std_cali_process(p_std_parser, GH_HAL_STD_CALI_DC_CANCEL);
#else
    dc_cancel_code = (int32_t)p_gh_data->ppg_mixdata.dc_cancel_code;
    dc_cancel_range = (int32_t)g_dc_bg_cancel_range[p_gh_data->ppg_mixdata.dc_cancel_range];
    idc_cancel = (int32_t)(((int64_t)dc_cancel_code * UA2PA * DC_CANCEL_CALI_EXTEND / EIGHT_BIT_MAX
                 * (int64_t)dc_cancel_range + DC_CANCEL_CALI_HALF_EXTEND) / DC_CANCEL_CALI_EXTEND);
#endif

    p_gh_data->ppg_mixdata.ipd_pa = gh_hal_common_ipd_cal(rawdata, gain, idc_cancel);
    DEBUG_LOG("[%s] data type:0x%x, ipd = %d, rawdata = %d \r\n", __FUNCTION__, p_gh_data->data_channel.data_type,
              p_gh_data->ppg_mixdata.ipd_pa, p_gh_data->ppg_mixdata.rawdata);
}
#endif

#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
/**
  * @brief calculate ipd for dc info data
  */
static void gh_hal_std_dcinfodata_cal(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
{
    return;
}
#endif

uint32_t gh_hal_std_parse_config(gh_hal_std_parser_t* p_std_parser, uint16_t reg_addr, uint16_t reg_val)
{
    uint32_t ret = 0;

    uint8_t dc_cancel_range_0;
    uint8_t dc_cancel_range_1;
    uint8_t dc_cancel_code_0;
    uint8_t dc_cancel_code_1;

    for (uint8_t i = 0; i < sizeof(p_std_parser->slot_cfg_param) / sizeof(p_std_parser->slot_cfg_param[0]); i++)
    {

        dc_cancel_range_0 = p_std_parser->slot_cfg_param[i].dc_cancel_range_0;
        dc_cancel_range_1 = p_std_parser->slot_cfg_param[i].dc_cancel_range_1;
        dc_cancel_code_0 = p_std_parser->slot_cfg_param[i].dc_cancel_code_0;
        dc_cancel_code_1 = p_std_parser->slot_cfg_param[i].dc_cancel_code_1;

        gh3036_std_parse_config(reg_addr, reg_val, i,
                                &dc_cancel_range_0, &dc_cancel_range_1, &dc_cancel_code_0, &dc_cancel_code_1);


        p_std_parser->slot_cfg_param[i].dc_cancel_range_0 = dc_cancel_range_0;
        p_std_parser->slot_cfg_param[i].dc_cancel_range_1 = dc_cancel_range_1;
        p_std_parser->slot_cfg_param[i].dc_cancel_code_0 = dc_cancel_code_0;
        p_std_parser->slot_cfg_param[i].dc_cancel_code_1 = dc_cancel_code_1;
    }

    return ret;
}

uint32_t gh_hal_std_parse_reset(gh_hal_std_parser_t* p_std_parser)
{
#if GH_HAL_STD_CALI_EN
    gh_memset((void*)&p_std_parser->cali_data, 0, sizeof(gh_std_cali_data_t));
#endif
    gh_memset((void*)&p_std_parser->slot_cfg_param, 0, sizeof(gh_std_slot_cfg_param_t) * PPG_SLOT_CFG_MAX_NUM);
    return RETURN_VALUE_ASSEMBLY(0, GH_HAL_STD_OK);
}

uint32_t gh_hal_std_init(gh_hal_std_parser_t* p_std_parser)
{
    gh_hal_std_parse_reset(p_std_parser);
#if GH_HAL_STD_CALI_EN
    gh_hal_efuse_cali_param_get(&p_std_parser->cali_param);
    DEBUG_LOG("[%s] rx0 offset:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.rx_param.rx0_offset);
    DEBUG_LOG("[%s] rx1 offset:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.rx_param.rx1_offset);
    DEBUG_LOG("[%s] bg cancel a:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.bg_cancel_param.bg_cancel_a);
    DEBUG_LOG("[%s] bg cancel b:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.bg_cancel_param.bg_cancel_b);
    DEBUG_LOG("[%s] dc cancel a:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.dc_cancel_param.dc_cancel_a);
    DEBUG_LOG("[%s] dc cancel b:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.dc_cancel_param.dc_cancel_b);
    DEBUG_LOG("[%s] gain rx0:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.gain_param.gain_rx0_offset);
    DEBUG_LOG("[%s] gain rx1:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.gain_param.gain_rx1_offset);
    DEBUG_LOG("[%s] led0 er:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.led_param.led0_er);
    DEBUG_LOG("[%s] led1 er:0x%x\r\n", __FUNCTION__, p_std_parser->cali_param.led_param.led1_er);
#endif
    return RETURN_VALUE_ASSEMBLY(0, GH_HAL_STD_OK);
}

uint32_t gh_hal_std_process(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
{
    if (GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_HAL_STD_NULL);
    }

    if (GH_PPG_DATA == p_gh_data->data_channel.data_type)
    {
        gh_hal_std_ppgdata_cal(p_std_parser, p_gh_data);
    }
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    else if (GH_PPG_MIX_DATA == p_gh_data->data_channel.data_type)
    {
        gh_hal_std_mixdata_cal(p_std_parser, p_gh_data);
    }
    else if (GH_PPG_BG_DATA == p_gh_data->data_channel.data_type)
    {
        gh_hal_std_bgdata_cal(p_std_parser, p_gh_data);
    }
#endif
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    else if (GH_PPG_DRE_DC_INFO == p_gh_data->data_channel.data_type)
    {
        gh_hal_std_dcinfodata_cal(p_std_parser, p_gh_data);
    }
#endif
    return RETURN_VALUE_ASSEMBLY(0, GH_HAL_STD_OK);
}

#if GH_HAL_STD_CALI_DRV_EN
uint8_t gh_hal_std_set_drv_code(gh_std_cali_led_param_t led_cali_param, uint32_t drv_ua, uint8_t drv_fs,
                                uint8_t drv_num, uint8_t slot_cfg_num)
{
    int32_t cali_y;
    int32_t cali_n;
    int32_t cali_er;
    int32_t drv_code;
    uint8_t drv_fs_code;

    if (drv_ua > drv_fs * MV2UV)
    {
        drv_ua = (uint32_t)drv_fs * MV2UV;
    }

    for (uint8_t i = 0; i < LED_CALI_FS_CODE_MAX + 1; i++)
    {
        if (g_led_range[i] == drv_fs)
        {
            drv_fs_code = i;
        }
    }

    if (0 == drv_num)
    {
        cali_y = LED_CALI_Y0;
        cali_er = LED_CALI_EXTEND + LED_CALI_EXTEND * (int32_t)led_cali_param.led0_er / (EIGHT_BIT_MAX + 1);
    }
    else
    {
        cali_y = LED_CALI_Y1;
        cali_er = LED_CALI_EXTEND + LED_CALI_EXTEND * (int32_t)led_cali_param.led1_er / (EIGHT_BIT_MAX + 1);
    }

    cali_n = 1 << (LED_CALI_FS_CODE_MAX - drv_fs_code);
    drv_code = (int32_t)(((int64_t)cali_er * EIGHT_BIT_MAX * cali_n * ROUND
               / ((int64_t)LED_CALI_DRV * LED_CALI_EXTEND / drv_ua - cali_y) + HALFROUND) / ROUND);

    if (drv_code > EIGHT_BIT_MAX)
    {
        drv_code = EIGHT_BIT_MAX;
    }

    DEBUG_LOG("[%s] drv num = %d, y= %d, er = %d, cali_n = %d, drv_ua = %d, drv code =%d \r\n", __FUNCTION__, drv_num,
              cali_y, cali_er, cali_n, drv_ua, drv_code);
    return (uint8_t)drv_code;
}
#else

uint8_t gh_hal_std_set_drv_code(uint32_t drv_ua, uint8_t drv_fs, uint8_t drv_num, uint8_t slot_cfg_num)
{
    int32_t drv_code;

    if (drv_ua > drv_fs * MV2UV)
    {
        drv_ua = (uint32_t)drv_fs * MV2UV;
    }

    drv_code = (drv_ua * EIGHT_BIT_MAX * ROUND / drv_fs / MV2UV + HALFROUND) / ROUND;

    if (drv_code > EIGHT_BIT_MAX)
    {
        drv_code = EIGHT_BIT_MAX;
    }

    DEBUG_LOG("[%s] drv num = %d, drv_ua = %d, drv_fs = %d, drv code =%d \r\n", __FUNCTION__, drv_num,
              drv_fs, drv_ua, drv_code);
    return (uint8_t)drv_code;
}
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
uint8_t gh_hal_std_set_dc_cancel_code(gh_std_cali_dc_param_t dc_cancel_param, uint32_t dc_na, uint16_t dc_fs,
                                      uint8_t dc_num, uint8_t slot_cfg_num)
{
    int64_t dc_cancel_cali;
    int32_t dc_cancel_code;
    int32_t a_er;
    int32_t b_er;

    if (dc_na > dc_fs * UA2NA)
    {
        dc_na = (uint32_t)dc_fs * UA2NA;
    }

    a_er = ((DC_CANCEL_CALI_EXTEND + DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_a / DC_CANCEL_CALI_A_MAX)
           * ROUND + HALFROUND) / ROUND;
    if (dc_cancel_param.dc_cancel_b > 0)
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND + HALFROUND)
               / ROUND;
    }
    else
    {
        b_er = ((DC_CANCEL_CALI_EXTEND * dc_cancel_param.dc_cancel_b / DC_CANCEL_CALI_B_MAX) * ROUND - HALFROUND)
               / ROUND;
    }

    dc_cancel_cali = ((uint64_t)dc_na * UA2NA - (uint64_t)b_er) * UA2NA / (uint64_t)a_er;
    dc_cancel_code = (dc_cancel_cali * EIGHT_BIT_MAX * ROUND  / UA2NA / dc_fs + HALFROUND) / ROUND;

    if (dc_cancel_code > EIGHT_BIT_MAX)
    {
        dc_cancel_code = EIGHT_BIT_MAX;
    }

    DEBUG_LOG("[%s] drv num = %d, a_er= %d, b_er = %d, dc na = %d, dc_cancel_cali = %lld, dc code =%d \r\n",
              __FUNCTION__, dc_num, a_er, b_er, dc_na, dc_cancel_cali, dc_cancel_code);
    return (uint8_t)dc_cancel_code;
}
#else

uint8_t gh_hal_std_set_dc_cancel_code(uint32_t dc_na, uint16_t dc_fs, uint8_t dc_num, uint8_t slot_cfg_num)
{
    int32_t dc_cancel_code;

    if (dc_na > dc_fs * UA2NA)
    {
        dc_na = (uint32_t)dc_fs * UA2NA;
    }

    dc_cancel_code = (dc_na * EIGHT_BIT_MAX * ROUND  / UA2NA / dc_fs + HALFROUND) / ROUND;

    if (dc_cancel_code > EIGHT_BIT_MAX)
    {
        dc_cancel_code = EIGHT_BIT_MAX;
    }

    DEBUG_LOG("[%s] dc_num num = %d, dc_ua = %d, dc_fs = %d, dc code =%d \r\n", __FUNCTION__, dc_num, dc_na,
              dc_fs, dc_cancel_code);
    return (uint8_t)dc_cancel_code;
}
#endif
