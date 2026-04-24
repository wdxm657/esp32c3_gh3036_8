/**
  ****************************************************************************************
  * @file    gh_application.c
  * @author  GHealth Driver Team
  * @brief   gh application file
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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "gh_app_common.h"
#include "gh_app.h"
#include "gh_app_manager.h"
#include "gh_sample_ctrl.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_LOG_EN
#define DEBUG_LOG(...)                      GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                    GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                      GH_LOG_LVL_ERROR(__VA_ARGS__)
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
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
/**
 * @brief Set channel info to hal
 *
 * @param p_manager: pointer of manager module
 * @param p_inf: pointer of app interface
 * @param idx: function index
 * @return gh_sample_ret_e
 *
 * @note None
 */
static gh_sample_ret_e gh_sample_ctrl_chm_set(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_func_index_e idx);


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static gh_sample_ret_e gh_sample_ctrl_chm_set(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_func_index_e idx)
{
    gh_hal_sample_ctrl_t sample;
    gh_hal_data_channel_t *p_ch_map  = GH_NULL_PTR;
    uint8_t cnt = 0;

    p_ch_map = p_manager->frame_inst.func_frame[idx].p_ch_map;
    cnt = p_manager->frame_inst.func_frame[idx].ch_num;
    while (cnt)
    {
        if (p_inf->sample_ctrl)
        {
            sample.op = GH_HAL_SAMPLE_CHN_EN;
            sample.ch_map.channel = p_ch_map->channel;
            p_inf->sample_ctrl(&sample);
        }
        p_ch_map++;
        cnt--;
    }

    return GH_CHECK_OK;
}

gh_sample_ret_e gh_sample_ctrl_reset(void)
{
    return GH_CHECK_OK;
}

gh_sample_ret_e gh_sample_ctrl_config(uint16_t addr, uint16_t val)
{
    return GH_CHECK_OK;
}

gh_sample_ret_e gh_sample_ctrl(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_function_en_union_t *p_function_en)
{
    if (GH_NULL_PTR == p_inf || GH_NULL_PTR == p_function_en)
    {
        return GH_CHECK_PTR_NULL;
    }

    uint32_t err_cnt = 0;
    uint32_t func_id = 0;
    gh_hal_sample_ctrl_t sample;

    DEBUG_LOG("[%s] enable bits:0x%x\r\n", __FUNCTION__, p_function_en->bits);

    if (p_inf->sample_ctrl)
    {
        sample.op = GH_HAL_SAMPLE_TOP_STOP;
        p_inf->sample_ctrl(&sample);
    }

    if (p_function_en->func.function_adt_en)
    {
#if (GH_FUNC_ADT_EN)
        func_id = func_id | GH_FUNC_FIX_ADT;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_ADT_IDX);
        DEBUG_LOG("[%s] ADT function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_ADT;
        ERROR_LOG("[%s] fail!, ADT function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] ADT function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_hr_en)
    {
#if (GH_FUNC_HR_EN)
        func_id = func_id | GH_FUNC_FIX_HR;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_HR_IDX);
        DEBUG_LOG("[%s] HR function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_HR;
        ERROR_LOG("[%s] fail!, HR function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] HR function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_spo2_en)
    {
#if (GH_FUNC_SPO2_EN)
        func_id = func_id | GH_FUNC_FIX_SPO2;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_SPO2_IDX);
        DEBUG_LOG("[%s] SPO2 function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_SPO2;
        ERROR_LOG("[%s] fail!, SPO2 function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] SPO2 function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_hrv_en)
    {
#if (GH_FUNC_HRV_EN)
        func_id = func_id | GH_FUNC_FIX_HRV;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_HRV_IDX);
        DEBUG_LOG("[%s] HRV function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_HRV;
        ERROR_LOG("[%s] fail!, HRV function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] HRV function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_gnadt_en)
    {
#if (GH_FUNC_GNADT_EN)
        func_id = func_id | GH_FUNC_FIX_GNADT;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_GNADT_IDX);
        DEBUG_LOG("[%s] GNADT function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_GNADT;
        ERROR_LOG("[%s] fail!, GNADT function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] GNADT function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_irnadt_en)
    {
#if (GH_FUNC_IRNADT_EN)
        func_id = func_id | GH_FUNC_FIX_IRNADT;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_IRNADT_IDX);
        DEBUG_LOG("[%s] IRNADT function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_IRNADT;
        ERROR_LOG("[%s] fail!, IRNADT function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] IRNADT function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_test1_en)
    {
#if (GH_FUNC_TEST1_EN)
        func_id = func_id | GH_FUNC_FIX_TEST1;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_TEST1_IDX);
        DEBUG_LOG("[%s] TEST1 function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_TEST1;
        ERROR_LOG("[%s] fail!, TEST1 function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] TEST1 function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_test2_en)
    {
#if (GH_FUNC_TEST2_EN)
        func_id = func_id | GH_FUNC_FIX_TEST2;
        gh_sample_ctrl_chm_set(p_manager, p_inf, GH_FUNC_ID_TEST2_IDX);
        DEBUG_LOG("[%s] TEST2 function enable!\r\n", __FUNCTION__);
#else
        err_cnt = err_cnt | GH_FUNC_FIX_TEST2;
        ERROR_LOG("[%s] fail!, TEST2 function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    }
    else
    {
        DEBUG_LOG("[%s] TEST2 function disable!\r\n", __FUNCTION__);
    }

    if (p_function_en->func.function_slot_en)
    {
#if (GH_FUNC_SLOT_EN)
        gh_slot_enable_t slot_en = {0};
        gh_func_fix_idx_e ppg_fix_idx = GH_FUNC_FIX_IDX_PPG_CFG0;

        gh_app_manager_slot_map_check(p_manager, &slot_en);

        for (gh_func_index_e ppg_idx = GH_FUNC_ID_PPG_CFG0_IDX; ppg_idx < GH_FUNC_ID_CAP_CFG_IDX; ppg_idx++)
        {
            if (1 == slot_en.ppg_cfg[ppg_idx - GH_FUNC_ID_PPG_CFG0_IDX])
            {
                func_id = func_id | (1 << ppg_fix_idx);
                gh_sample_ctrl_chm_set(p_manager, p_inf, ppg_idx);
                DEBUG_LOG("[%s] PPG CFG%d slot enable!\r\n", __FUNCTION__, (ppg_fix_idx - GH_FUNC_FIX_IDX_PPG_CFG0));
            }
            ppg_fix_idx++;
        }

        gh_func_fix_idx_e cap_fix_idx = GH_FUNC_FIX_IDX_CAP_CFG;
        for (gh_func_index_e cap_idx = GH_FUNC_ID_CAP_CFG_IDX; cap_idx < GH_FUNC_ID_MAX; cap_idx++)
        {
            if (1 == slot_en.cap_cfg[cap_idx - GH_FUNC_ID_CAP_CFG_IDX])
            {
                func_id = func_id | (1 << cap_fix_idx);
                gh_sample_ctrl_chm_set(p_manager, p_inf, cap_idx);
                DEBUG_LOG("[%s] CAP CFG%d slot enable!\r\n", __FUNCTION__, (cap_fix_idx - GH_FUNC_FIX_IDX_CAP_CFG));
            }
            cap_fix_idx++;
        }
#else
        err_cnt = err_cnt | (GH_FUNC_FIX_IDX_PPG_CFG0 | GH_FUNC_FIX_IDX_PPG_CFG1 | GH_FUNC_FIX_IDX_PPG_CFG2
                | GH_FUNC_FIX_IDX_PPG_CFG3 | GH_FUNC_FIX_IDX_PPG_CFG4 | GH_FUNC_FIX_IDX_PPG_CFG5
                | GH_FUNC_FIX_IDX_PPG_CFG6 | GH_FUNC_FIX_IDX_PPG_CFG7 | GH_FUNC_FIX_IDX_CAP_CFG);
        ERROR_LOG("[%s] fail!, slot function is set to disable! 0x%x\r\n", __FUNCTION__, err_cnt);
#endif
    } // if (p_function_en->func.function_slot_en)

    gh_app_manager_func_set(p_manager, func_id);

    if (p_inf->sample_ctrl)
    {
        gh_hal_sample_ctrl_t sample;

        sample.op = GH_HAL_SAMPLE_TOP_START;
        p_inf->sample_ctrl(&sample);
    }

    if (0 != err_cnt)
    {
        return GH_CHECK_FUNC_DISABLE;
    }

    return GH_CHECK_OK;
}

