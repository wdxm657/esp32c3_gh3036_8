/**
  ****************************************************************************************
  * @file    gh_move_detector.c
  * @author  GHealth Driver Team
  * @brief   move detector module
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
#include "gh_move_detector.h"

#if (GH_GS_MOVE_DET_EN)

/*
 * DEFINES
 *****************************************************************************************
 */
#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
            (GH_APP_MOVE_DET_ID << 24 | (internal_err) << 8 | (interface_err))

#if GH_APP_MOVE_DET_LOG_EN
#define DEBUG_LOG(...)          GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)        GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)          GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define GH_UINT8_MAX            (255)

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static uint32_t gh_gs_diff_cal(int16_t *last_acc, int16_t *cur_acc)
{
    uint32_t diff = 0;

    for (uint8_t axis_cnt = 0; axis_cnt < GH_ACC_AXIS_NUM; axis_cnt++)
    {
        diff += (int32_t)(last_acc[axis_cnt] - cur_acc[axis_cnt])
                * (int32_t)(last_acc[axis_cnt] - cur_acc[axis_cnt]);
    }

    return diff;
}

static void gh_move_detector_prepare(gh_move_detector_t *this)
{
    this->first_point_got_flag = 0;
    this->move_cnt = 0;
    this->not_move_cnt = 0;
}

uint32_t gh_move_detector_init(gh_move_detector_t *this, gh_wear_evt_hook_t evt_hook)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_wear_detector_t));
    this->evt_hook = evt_hook;

    return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
}

uint32_t gh_move_detector_deinit(gh_move_detector_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_wear_detector_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
}

uint32_t gh_move_detector_ctrl(gh_move_detector_t *this, gh_move_det_ctrl_e ctrl_option)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_PTR_NULL);
    }

    this->state = ctrl_option;

    if (GH_MOVE_DET_CTRL_DIS == ctrl_option)
    {
        gh_move_detector_prepare(this);
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
}

uint32_t gh_move_detector_pro(gh_move_detector_t *this, gh_gsensor_ts_and_data_t *gs_data)
{
    uint32_t diff = 0;

#if GH_MOVE_DET_DEBUG_EN
    this->output_evt_id = GH_MOVE_EVT_ID_MAX;
    this->output_evt_ts = 0;
    this->downsample_flag = 0;
#endif

    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_PTR_NULL);
    }

    if (GH_MOVE_DET_CTRL_EN != this->state)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
    }

    if (0 == this->first_point_got_flag)
    {
        this->first_point_got_flag = 1;
        gh_memcpy((void*)(this->last_acc), (void *)(gs_data->data.acc), sizeof(this->last_acc));
        this->ideal_timestamp = gs_data->timestamp + GH_MOVE_DET_GS_SAMPLE_PERIOD_MS;
#if GH_MOVE_DET_DEBUG_EN
        this->downsample_flag = 1;
#endif
        return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
    }
    else
    {
        /* gs_data->timestamp is abnormal */
        if ((gs_data->timestamp <= (this->ideal_timestamp - GH_MOVE_DET_GS_SAMPLE_PERIOD_MS))
            || (this->ideal_timestamp + GH_GS_TS_INTERVAL_TOO_BIG_MS) < gs_data->timestamp)
        {
            gh_move_detector_prepare(this);

            return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_TS_ERR);
        }

        /* right timestamp */
        if (this->ideal_timestamp <= gs_data->timestamp)
        {
            diff = gh_gs_diff_cal(this->last_acc, gs_data->data.acc);
            gh_memcpy((void *)(this->last_acc), (void *)(gs_data->data.acc), sizeof(this->last_acc));

#if GH_MOVE_DET_DEBUG_EN
            this->gs_diff = diff;
            this->downsample_flag = 1;
#endif
            if (diff > GH_GS_DIFF_THRD)
            {
                this->not_move_cnt = 0;

                if (this->move_cnt < GH_GS_MOVE_CNT_THRD)
                {
                    this->move_cnt++;
                }
                if (this->move_cnt >= GH_GS_MOVE_CNT_THRD)
                {
#if GH_MOVE_DET_DEBUG_EN
                    this->output_evt_id = GH_MOVE_EVT_ID_MOVE;
                    this->output_evt_ts = gs_data->timestamp;
#endif
                    if (this->evt_hook)
                    {
                        this->evt_hook(GH_MOVE_EVT_ID_MOVE, gs_data->timestamp);
                    }
                }
            }
            else
            {
                if (this->not_move_cnt < GH_UINT8_MAX)
                {
                    this->not_move_cnt++;
                }
                if (this->not_move_cnt > GH_GS_NOT_MOVE_CNT_THRD)
                {
                    this->move_cnt = 0;

                    if (this->not_move_cnt > (GH_GS_MOVE_TIME_OUT_THRD / GH_MOVE_DET_GS_SAMPLE_PERIOD_MS))
                    {
#if GH_MOVE_DET_DEBUG_EN
                        this->output_evt_id = GH_MOVE_EVT_ID_MOVE_TIME_OUT;
                        this->output_evt_ts = gs_data->timestamp;
#endif
                        if (this->evt_hook)
                        {
                            this->evt_hook(GH_MOVE_EVT_ID_MOVE_TIME_OUT, gs_data->timestamp);
                        }
                        this->not_move_cnt = 0; // prevent calling too freqently for time out hook
                    }
                }
            }
            this->ideal_timestamp += GH_MOVE_DET_GS_SAMPLE_PERIOD_MS;
        }    // if: is right timestamp
    }    // else: is not first point

    return RETURN_VALUE_ASSEMBLY(0, GH_MOVE_DET_OK);
}

#endif

