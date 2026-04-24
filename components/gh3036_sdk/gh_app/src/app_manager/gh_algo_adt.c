/**
  ****************************************************************************************
  * @file    gh_algo_adt.c
  * @author  GHealth Driver Team
  * @brief   gh nadt algorithm
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
#include "gh_hal_log.h"
#include "gh_global_config.h"
#include "gh_app_common.h"
#include "gh_data_common.h"
#if (GH_USE_GOODIX_ADT_ALGO)
#include "gh_wear_detector.h"
#endif
#include "gh_algo_adapter_common.h"
#include "gh_algo_adt.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_ALGO_LOG_EN
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
#define GH_ADT_DEFAULT_CHANNEL_NUM          (1)

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
gh_algo_ret_e gh_algo_adt_init(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param)
{
    if (GH_NULL_PTR == p_param)
    {
        return GH_ALGO_NULL;
    }

#if (GH_USE_GOODIX_ADT_ALGO)
    DEBUG_LOG("[%s] enter\r\n", __FUNCTION__);

    gh_wear_det_ret_e ret = gh_wear_detector_init((gh_wear_detector_t *)p_param->p_algo_inst);

    if (GH_WEAR_DET_OK != ret)
    {
        return GH_ALGO_INIT_ERR;
    }
#endif

    return GH_ALGO_OK;
}

gh_algo_ret_e gh_algo_adt_deinit(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param)
{
    if (GH_NULL_PTR == p_param)
    {
        return GH_ALGO_NULL;
    }

#if (GH_USE_GOODIX_ADT_ALGO)
    DEBUG_LOG("[%s] enter\r\n", __FUNCTION__);

    gh_wear_det_ret_e ret = gh_wear_detector_deinit((gh_wear_detector_t *)p_param->p_algo_inst);

    if (GH_WEAR_DET_OK != ret)
    {
        return GH_ALGO_DEINIT_ERR;
    }
#endif

    return GH_ALGO_OK;
}

gh_algo_ret_e gh_algo_adt_exe(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param)
{
    if (GH_NULL_PTR == p_frame || GH_NULL_PTR == p_param)
    {
        return GH_ALGO_NULL;
    }

#if (GH_USE_GOODIX_ADT_ALGO)
    gh_hal_data_channel_t *p_ch_map = p_frame->p_ch_map;

    for (uint8_t idx = 0; idx < p_frame->ch_num; idx++)
    {
        if (GH_ADT_DEFAULT_CHANNEL_NUM == idx)
        {
            break;
        }

        switch (p_ch_map->data_type)
        {
            case GH_PPG_DATA:
            {
//                DEBUG_LOG("[%s] data type:%d need\r\n", __FUNCTION__, p_ch_map->data_type);
                gh_adt_result_t res = {0};
                gh_wear_det_ret_e ret = GH_WEAR_DET_OK;

                res.det_state = GH_ADT_WEAR_DET_UNKONW;
                ret = gh_wear_detector_pro((gh_wear_detector_t *)p_param->p_algo_inst, p_frame, idx, &res);

                if (GH_WEAR_DET_OK != ret)
                {
                    ERROR_LOG("[%s] pro error! ret:%d\r\n", __FUNCTION__, ret);
                    return GH_ALGO_EXE_ERR;
                }

                gh_algo_adt_result_t *p_res = (gh_algo_adt_result_t *)p_frame->p_algo_res;

                if (res.update)
                {
                    p_res->wear_evt = res.evt_id;
                    p_res->ctr = res.ctr;
                    p_res->det_status = res.det_state;
                    p_res->update = 1;
                    DEBUG_LOG("[%s]wear_evt:%d, ctr:%d\r\n", __FUNCTION__, p_res->wear_evt, p_res->ctr);
                }
                else
                {
                    p_res->wear_evt = GH_ADT_WEAR_IDLE;
                    p_res->update = 0;
                }
            }
            break;

            default:
                DEBUG_LOG("[%s] data type:%d no need\r\n", __FUNCTION__, p_ch_map->data_type);
            break;
        } // switch (p_ch_map->data_type)

        p_ch_map++;
    } // for (uint8_t idx = 0; idx < p_frame->ch_num; idx++)
#endif

    return GH_ALGO_OK;
}

