/**
  ****************************************************************************************
  * @file    gh_wear_detector.c
  * @author  GHealth Driver Team
  * @brief   wear detector module
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
#include "gh_app_common.h"
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_wear_detector.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_ALGO_LOG_EN
#define DEBUG_LOG(...)          GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)        GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)          GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define GH_CTR_PA_TO_DOT_1NA       (100)
#define GH_DEFAULT_LED_FS_MA       (160)
#define GH_LED_DRV_CODE_MAX        (256)
#define GH_LED_DRV_64_RADIO        (64)
#define GH_UINT16_MAX              (65535)
#define GH_CTR_MEAN_WINDOW_NUM     (5)
#define GH_GHEALTH_CTR_THD_ON      (2285)   //unit: 0.1nA/mA
#define GH_GHEALTH_CTR_THD_OFF     (2066)    //unit: 0.1nA/mA

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
gh_wear_det_ret_e gh_wear_detector_init(gh_wear_detector_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return GH_WEAR_DET_PTR_NULL;
    }
    gh_memset((void *)this, 0, sizeof(gh_wear_detector_t));

    return GH_WEAR_DET_OK;
}

gh_wear_det_ret_e gh_wear_detector_deinit(gh_wear_detector_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return GH_WEAR_DET_PTR_NULL;
    }
    gh_memset((void*)this, 0, sizeof(gh_wear_detector_t));

    return GH_WEAR_DET_OK;
}

gh_wear_det_ret_e gh_wear_detector_pro(gh_wear_detector_t *this,
                                       gh_func_frame_t *p_frame,
                                       uint8_t index,
                                       gh_adt_result_t *p_res)
{
    uint32_t cur_ctr = 0;
    uint32_t total_led_current; // uint: 1 / 64 mA
    uint32_t ctr_sum;

    if ((GH_NULL_PTR == this) || (GH_NULL_PTR == p_frame) || (GH_NULL_PTR == p_res))
    {
        return GH_WEAR_DET_PTR_NULL;
    }

    // calculate total current
    total_led_current = 0;
    total_led_current += (uint32_t)p_frame->p_data[index].agc_info.led_drv0
                        * (uint32_t)p_frame->led_drv_fs[0]
                        * GH_LED_DRV_64_RADIO / GH_LED_DRV_CODE_MAX;
    total_led_current += (uint32_t)p_frame->p_data[index].agc_info.led_drv1
                        * (uint32_t)p_frame->led_drv_fs[1]
                        * GH_LED_DRV_64_RADIO / GH_LED_DRV_CODE_MAX;

    //DEBUG_LOG("[gh_wear_detector_pro] total_led_current = %d \r\n", (int)total_led_current/GH_LED_DRV_64_RADIO);
    // calculate CTR at this frame
    if (p_frame->p_data[index].ipd_pa > 0)
    {
        cur_ctr = p_frame->p_data[index].ipd_pa / GH_CTR_PA_TO_DOT_1NA
                * GH_LED_DRV_64_RADIO / total_led_current;
    }

    // calculate mean
    ctr_sum = this->ctr_sum;
    ctr_sum += cur_ctr;
    if (ctr_sum >= GH_UINT16_MAX)
    {
        ctr_sum = GH_UINT16_MAX;
    }
    this->ctr_sum = (uint16_t)ctr_sum;
    this->mean_cnt++;

    if (this->mean_cnt >= GH_CTR_MEAN_WINDOW_NUM)
    {
        this->ctr_sum = this->ctr_sum / GH_CTR_MEAN_WINDOW_NUM;
        this->mean_cnt = 0;
        p_res->update = 1;
        p_res->evt_id = 0;  //null event
        p_res->ctr = this->ctr_sum;
        DEBUG_LOG("[gh_wear_detector_pro]ctr_sum :%d \r\n", this->ctr_sum);
        if ((GH_GHEALTH_WEAR_DET_ON == this->det_state)
                && (this->ctr_sum > GH_GHEALTH_CTR_THD_ON))
        {
            p_res->evt_id = GH_WEAR_EVT_ID_GHEALTH_ON;
            this->det_state = GH_GHEALTH_WEAR_DET_OFF;
            DEBUG_LOG("[gh_wear_detector_pro] new event: wear on! \r\n");
        }
        if ((GH_GHEALTH_WEAR_DET_OFF == this->det_state)
                && (this->ctr_sum < GH_GHEALTH_CTR_THD_OFF))
        {
            p_res->evt_id = GH_WEAR_EVT_ID_GHEALTH_OFF;
            this->det_state = GH_GHEALTH_WEAR_DET_ON;
            DEBUG_LOG("[gh_wear_detector_pro] new event: wear off! \r\n");
        }
        p_res->det_state = (uint32_t)this->det_state;
        this->ctr_sum = 0;
    }

    return GH_WEAR_DET_OK;
}

