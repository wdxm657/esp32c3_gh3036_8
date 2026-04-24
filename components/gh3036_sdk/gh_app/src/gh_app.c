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
#include "gh_global_config.h"
#include "gh_sample_ctrl.h"
#include "gh_config_process.h"
#include "gh_app_manager.h"
#include "gh_public_api.h"
#if (1 == GH_GS_MOVE_DET_EN)
#include "gh_move_detector.h"
#endif
#if (1 == GH_MTSS_EN)
#include "gh_mtss_evt_manager.h"
#endif
#if (1 == GH_APP_MIPS_STA_EN)
#include "gh_app_mips.h"
#endif
#include "gh_app.h"

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

#define GH_TO_STRING(x)                     #x                  /**< number to char */
#define GH_STR(x)                           GH_TO_STRING(x)     /**< number to char */

#define GH_FUNCTION_ENABLE_MASK             0xFF                /**< function enable mask */

#define GH_DEMO_CUSTOMER_VERSION_NUMBER     0                   /**< customer version number */
#define GH_DEMO_MAIN_VERSION_NUMBER         2                   /**< main version number */
#define GH_DEMO_MAJOR_VERSION_NUMBER        0                   /**< major version number */
#define GH_DEMO_MINOR_VERSION_NUMBER        1                   /**< minor version number */

#if (GH_LOG_DEBUG_ENABLE)
#define GH_DEBUG_FLAG_STRING                "_DEBUG"
#else
#define GH_DEBUG_FLAG_STRING                ""
#endif

/// FW version string
#define GH_FW_VERSION_STRING                "GH(M)_FW_"\
                                                "V"GH_STR(GH_DEMO_MAJOR_VERSION_NUMBER)\
                                                "."GH_STR(GH_DEMO_MINOR_VERSION_NUMBER)

/// VR version string
#define GH_VR_VERSION_STRING                "GH(M)_VR_"\
                                                "V"GH_STR(GH_DEMO_MAJOR_VERSION_NUMBER)\
                                                "."GH_STR(GH_DEMO_MINOR_VERSION_NUMBER)

/// demo version string
#define GH_DEMO_VERSION_STRING              "GH(M)3036_SDK_"\
                                                "V"GH_STR(GH_DEMO_CUSTOMER_VERSION_NUMBER)\
                                                "."GH_STR(GH_DEMO_MAIN_VERSION_NUMBER)\
                                                "."GH_STR(GH_DEMO_MAJOR_VERSION_NUMBER)\
                                                "."GH_STR(GH_DEMO_MINOR_VERSION_NUMBER)\
                                                GH_DEBUG_FLAG_STRING\
                                                "(build:"__DATE__"_"__TIME__")"

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */
/**
  * @brief GH public api struct.
  */
typedef struct
{
    uint32_t init_flag              : 1;        // sdk init flag
    uint32_t sample_flag            : 1;        // sdk start sample flag
    uint32_t function_flag          : 16;       // sdk start function flag
    uint32_t reserved               : 14;       // reserved
} __attribute__ ((packed))gh_api_t;

/**
  * @brief GH public struct.
  */
typedef struct
{
    gh_api_t api;
    gh_function_en_union_t func_en;
    gh_app_manager_t manager_inst;
#if (GH_GS_MOVE_DET_EN)
    gh_move_detector_t move_det_inst;
#endif
#if (GH_MTSS_EN)
    gh_mtss_evt_manager_t mtss_evt_inst;
#endif
    gh_app_inf_t app_inf;
} gh_sdk_t;

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
gh_sdk_t g_sdk = {0};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

static gh_app_func_ret_e gh_demo_mem_check(gh_sdk_t *p_sdk);
static void gh_demo_manager_cb(gh_func_frame_t *p_frame);
#if (GH_GS_MOVE_DET_EN)
static void gh_demo_move_det_cb(gh_move_evt_id_e evt_id, uint64_t timestamp);
#endif
#if (GH_MTSS_EN)
static void gh_demo_mtss_cb(gh_mtss_evt_id_e event_id,
                            uint64_t timestamp);
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
int8_t *gh_firmware_version_get(void)
{
    return (int8_t *)GH_FW_VERSION_STRING;
}

int8_t *gh_demo_version_get(void)
{
    return (int8_t *)GH_DEMO_VERSION_STRING;
}

int8_t *gh_sdk_version_get(void)
{
    return (int8_t *)GH_DEMO_VERSION_STRING;
}

int8_t *gh_virtualreg_version_get(void)
{
    return (int8_t *)GH_VR_VERSION_STRING;
}

/**
 * @fn gh_app_func_ret_e gh_demo_mem_check(gh_sdk_t *p_sdk)
 * @brief check application memory
 * @param[in] p_sdk: pointer of function
 * @return refer to gh_app_func_ret_e
 *
 * @note None
 */
static gh_app_func_ret_e gh_demo_mem_check(gh_sdk_t *p_sdk)
{
    DEBUG_LOG("[%s] app mem size:%d\r\n", __FUNCTION__, sizeof(gh_sdk_t));
    DEBUG_LOG("[%s] app api mem size:%d\r\n", __FUNCTION__, sizeof(p_sdk->api));
    DEBUG_LOG("[%s] app func_en mem size:%d\r\n", __FUNCTION__, sizeof(p_sdk->func_en));
    DEBUG_LOG("[%s] app manager_inst mem size:%d\r\n", __FUNCTION__, sizeof(p_sdk->manager_inst));
    DEBUG_LOG("[%s] app manager_inst.frame_buf mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.frame_buf));
    DEBUG_LOG("[%s] app manager_inst.frame_inst mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.frame_inst));
#if (1 == GH_ALGO_ADAPTER_EN)
    DEBUG_LOG("[%s] app manager_inst.adapter_inst mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.adapter_inst));
#endif
#if ((1 == GH_GOODIX_ALGO_ENABLE_MODE) && (1 == GH_ALGO_ADAPTER_EN))
    DEBUG_LOG("[%s] app manager_inst.adapter_inst.algo_mem size:%d\r\n", __FUNCTION__,
              GH_ALGOS_MEM_SIZE_FINAL);
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    DEBUG_LOG("[%s] app manager_inst.data_fusion mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.data_fusion));
    DEBUG_LOG("[%s] data_fusion.ghealth_buffer mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.data_fusion.sensor_buffer.ghealth_buffer));
#if  GH_GSENSOR_FUSION_EN
    DEBUG_LOG("[%s] data_fusion.gsensor_buffer mem size:%d\r\n", __FUNCTION__,
              sizeof(p_sdk->manager_inst.data_fusion.sensor_buffer.gsensor_buffer));
#endif
#endif
    DEBUG_LOG("[%s] app app_inf mem size:%d\r\n", __FUNCTION__, sizeof(p_sdk->app_inf));

    return GH_APP_FUNC_OK;
}

static void gh_demo_manager_cb(gh_func_frame_t *p_frame)
{
    gh_sdk_t *p_sdk = &g_sdk;

#if (1 == GH_ALGO_ADAPTER_EN)
    if (GH_NULL_PTR != p_frame->p_algo_res)
    {
        switch (p_frame->id)
        {
#if (GH_USE_GOODIX_ADT_ALGO)
            case GH_FUNC_FIX_IDX_ADT:
            {
    #if (GH_MTSS_EN)
                gh_mtss_ctrl_e state = GH_MTSS_CTRL_DIS;
                gh_mtss_evt_manager_state_get(&p_sdk->mtss_evt_inst, &state);
    #endif
                gh_algo_adt_result_t *p_adt_res = (gh_algo_adt_result_t *)p_frame->p_algo_res;

                if (GH_ADT_WEAR_ON == p_adt_res->wear_evt && p_adt_res->update
                    && GH_NULL_PTR != p_sdk->app_inf.evt_publish)
                {
                    // DEBUG_LOG("[%s] GH_MTSS_EVT_ID_GHEALTH_ON\r\n", __FUNCTION__);
    #if (GH_MTSS_EN)
                    if (GH_MTSS_CTRL_DIS == state)
                    {
                        p_sdk->app_inf.evt_publish(GH_ACTION_WEAR_ON, p_frame->timestamp);
                    }
                    else
                    {
                        gh_mtss_evt_manager_evt_push(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_GHEALTH_ON,
                                                     p_frame->timestamp);
                    }
    #else
                    p_sdk->app_inf.evt_publish(GH_ACTION_ADT_WEAR_ON, p_frame->timestamp);
    #endif

                }
                else if (GH_ADT_WEAR_OFF == p_adt_res->wear_evt && p_adt_res->update
                         && GH_NULL_PTR != p_sdk->app_inf.evt_publish)
                {
                    // DEBUG_LOG("[%s] GH_MTSS_EVT_ID_GHEALTH_OFF\r\n", __FUNCTION__);
    #if (GH_MTSS_EN)
                    if (GH_MTSS_CTRL_DIS == state)
                    {
                        p_sdk->app_inf.evt_publish(GH_ACTION_WEAR_OFF, p_frame->timestamp);
                    }
                    else
                    {
                        gh_mtss_evt_manager_evt_push(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_GHEALTH_OFF,
                                                     p_frame->timestamp);
                    }
    #else
                    p_sdk->app_inf.evt_publish(GH_ACTION_ADT_WEAR_OFF, p_frame->timestamp);
    #endif
                }
            }
            break;
#endif

#if (GH_USE_GOODIX_NADT_ALGO)
            case GH_FUNC_FIX_IDX_GNADT:
            case GH_FUNC_FIX_IDX_IRNADT:
            {
                gh_algo_nadt_result_t *p_nadt_res = (gh_algo_nadt_result_t *)p_frame->p_algo_res;
                if (p_nadt_res->nadt_out_flag && GH_NULL_PTR != p_sdk->app_inf.evt_publish)
                {
                    if (GH_ALGO_NADT_RES0_NON_LINVING == GH_ALGO_NADT_RES0_MASK(p_nadt_res->nadt_out))
                    {
    #if (GH_MTSS_EN)
                        p_sdk->app_inf.evt_publish(GH_ACTION_WEAR_OFF, p_frame->timestamp);
    #else
                        p_sdk->app_inf.evt_publish(GH_ACTION_NADT_WEAR_OFF, p_frame->timestamp);
    #endif
    #if (GH_MTSS_EN)
                        gh_mtss_evt_manager_ctrl(&p_sdk->mtss_evt_inst, GH_MTSS_CTRL_EN);
    #endif
    #if (GH_GS_MOVE_DET_EN)
                        gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_EN);
    #endif
                    }
    #if (0 == GH_MTSS_EN)
                    else if (GH_ALGO_NADT_RES0_WEAR_ON == GH_ALGO_NADT_RES0_MASK(p_nadt_res->nadt_out))
                    {
                        p_sdk->app_inf.evt_publish(GH_ACTION_NADT_WEAR_ON, p_frame->timestamp);
                    }
    #endif
                }
            }
            break;
#endif
            default:
            break;
        } // switch (p_frame->id)
    } // if (GH_NULL_PTR != p_frame->p_algo_res)
#endif

    if (p_sdk->app_inf.data_publish)
    {
        p_sdk->app_inf.data_publish(p_frame);
    }

    return;
}

#if (GH_GS_MOVE_DET_EN)
static void gh_demo_move_det_cb(gh_move_evt_id_e evt_id, uint64_t timestamp)
{
    gh_sdk_t *p_sdk = &g_sdk;

    switch (evt_id)
    {
        case GH_MOVE_EVT_ID_MOVE:
        {
            // DEBUG_LOG("[%s] GH_MOVE_EVT_ID_MOVE\r\n", __FUNCTION__);
#if (GH_MTSS_EN)
            gh_mtss_evt_manager_evt_push(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_MOVE, timestamp);
#else
            if (p_sdk->app_inf.evt_publish)
            {
                p_sdk->app_inf.evt_publish(GH_ACTION_MOVE, timestamp);
            }
#endif
        }
        break;

        case GH_MOVE_EVT_ID_MOVE_TIME_OUT:
        {
            // DEBUG_LOG("[%s] GH_MOVE_EVT_ID_MOVE_TIME_OUT\r\n", __FUNCTION__);
#if (GH_MTSS_EN)
            gh_mtss_evt_manager_evt_push(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_MOVE_TIME_OUT, timestamp);
#else
            if (p_sdk->app_inf.evt_publish)
            {
                p_sdk->app_inf.evt_publish(GH_ACTION_MOVE_TIME_OUT, timestamp);
            }
#endif
        }
        break;

        default:
        break;
    }

    return;
}
#endif

#if (GH_MTSS_EN)
static void gh_demo_mtss_cb(gh_mtss_evt_id_e event_id,
                            uint64_t timestamp)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (GH_NULL_PTR == p_sdk->app_inf.evt_publish)
    {
        return;
    }

    // move time out process
    switch (event_id)
    {
        case GH_MTSS_EVT_ID_MOVE:
        {
            p_sdk->app_inf.evt_publish(GH_ACTION_MOVE, timestamp);
        }
        break;

        case GH_MTSS_EVT_ID_MOVE_TIME_OUT:
        {
            p_sdk->app_inf.evt_publish(GH_ACTION_MOVE_TIME_OUT, timestamp);
        }
        break;

        default:
        break;
    }

    if (gh_mtss_evt_is_new_chk(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_GHEALTH_ON, GH_MTSS_EVT_ID_GHEALTH_OFF)
        && gh_mtss_evt_in_wnd_chk(&p_sdk->mtss_evt_inst, GH_MTSS_EVT_ID_MOVE, GH_MOVE_GH_WEAR_ON_ITVAL_TIME, 1))
    {
        gh_mtss_evt_manager_ctrl(&p_sdk->mtss_evt_inst, GH_MTSS_CTRL_DIS);
#if (GH_GS_MOVE_DET_EN)
        gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_DIS);
        DEBUG_LOG("[%s] GH_MOVE_DET_CTRL_DIS!!!\r\n", __FUNCTION__);
#endif
        DEBUG_LOG("[%s] GH_MTSS_CTRL_DIS!!!\r\n", __FUNCTION__);

        p_sdk->app_inf.evt_publish(GH_ACTION_WEAR_ON, timestamp);
    }

    return;
}
#endif

gh_public_ret_e gh_demo_init(void)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (1 == p_sdk->api.init_flag)
    {
        return GH_API_INIT_FAIL_ALREADY_INIT;
    }

    gh_demo_mem_check(p_sdk);

    /* init app manager */
    gh_app_manager_init(&p_sdk->manager_inst, gh_demo_manager_cb);

#if (GH_GS_MOVE_DET_EN)
    /* init move detector */
    gh_move_detector_init(&p_sdk->move_det_inst, gh_demo_move_det_cb);

    /* enable move detector */
    gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_DIS);
#endif

#if (GH_MTSS_EN)
    /* init mutil sensor event manager */
    gh_mtss_evt_manager_init(&p_sdk->mtss_evt_inst, gh_demo_mtss_cb);

    /* enable mutil sensor event manager */
    gh_mtss_evt_manager_ctrl(&p_sdk->mtss_evt_inst, GH_MTSS_CTRL_DIS);
#endif

    /* init interface */
    p_sdk->app_inf.data_publish = gh_demo_data_publish_func_get();
    p_sdk->app_inf.sample_ctrl = gh_demo_sample_ctrl_func_get();
    p_sdk->app_inf.config_ctrl = gh_demo_config_ctrl_func_get();
    p_sdk->app_inf.evt_publish = gh_demo_evt_publish_func_get();
    p_sdk->app_inf.ts_get = gh_demo_ts_get();

#if (GH_APP_MIPS_STA_EN)
    gh_app_mips_t *p_mips = gh_app_mips_inst_get();
    gh_app_mips_cal_init(p_mips, GH_APP_MAIN_FREQUENCY_MHZ,
                         GH_APP_TIMESTAMP_UNIT_US, (gh_app_mips_ts_t)p_sdk->app_inf.ts_get);
#endif

    p_sdk->api.init_flag = 1;
    p_sdk->api.function_flag = 0;

    return GH_API_OK;
}

gh_public_ret_e gh_demo_deinit(void)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (1 == p_sdk->api.sample_flag)
    {
        return GH_API_DEINIT_FAIL_NEED_STOP;
    }

    /* deinit app manager */
    gh_app_manager_deinit(&p_sdk->manager_inst);

#if (GH_GS_MOVE_DET_EN)
    /* deinit move detector */
    gh_move_detector_deinit(&p_sdk->move_det_inst);
#endif

#if (GH_MTSS_EN)
    /* deinit mutil sensor event manager */
    gh_mtss_evt_manager_deinit(&p_sdk->mtss_evt_inst);
#endif

    p_sdk->api.init_flag = 0;

    return GH_API_OK;
}

gh_public_ret_e gh_demo_function_ctrl(gh_function_en_union_t *p_function_en)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_SAMPLING_CTRL_NEED_INIT;
    }

    /* step1: Check if the function is actived */
    if (p_sdk->func_en.bits != p_function_en->bits)
    {
        p_sdk->func_en.bits = p_function_en->bits;
    }
    else
    {
        return GH_API_SAMPLING_CTRL_FAIL;
    }

    /* step2: Get the channel map of this function from the sensor manager */
    gh_sample_ctrl(&p_sdk->manager_inst, &p_sdk->app_inf, p_function_en);

    /* step3: End the process and set the flag */
    if (0 == p_sdk->func_en.bits)
    {
        p_sdk->api.sample_flag = 0;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_function_get(gh_function_en_union_t *p_function_en)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_STOP_SAMPLING_FAIL_NEED_INIT;
    }

    for (gh_func_fix_idx_e func = GH_FUNC_FIX_IDX_ADT; func < GH_FUNC_FIX_IDX_PPG_CFG0; func++)
    {
        if (p_sdk->manager_inst.frame_inst.p_func_frame[func]
            && 0 != p_sdk->manager_inst.frame_inst.p_func_frame[func]->ch_num)
        {
            p_function_en->bits |= (1 << func);
        }
    }

    for (gh_func_fix_idx_e func = GH_FUNC_FIX_IDX_PPG_CFG0; func < GH_FUNC_FIX_IDX_MAX; func++)
    {
        if (p_sdk->manager_inst.frame_inst.p_func_frame[func]
            && 0 != p_sdk->manager_inst.frame_inst.p_func_frame[func]->ch_num)
        {
            p_function_en->func.function_slot_en = 1;
            break;
        }
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_assist_config(gh_function_en_union_t *p_function_en,
                                      gh_assist_en_union_t *p_assist_en)
{
    if (GH_NULL_PTR == p_function_en || GH_NULL_PTR == p_assist_en)
    {
        return GH_API_NULL;
    }

    gh_sdk_t *p_sdk = &g_sdk;
    gh_function_en_union_t err_func;

    err_func.bits = 0;

    for (gh_func_fix_idx_e id = GH_FUNC_FIX_IDX_ADT; id < GH_FUNC_FIX_IDX_PPG_CFG0; id++)
    {
        if (1 == ((p_function_en->bits >> id) & 0x1))
        {
            if (GH_NULL_PTR != p_sdk->manager_inst.frame_inst.p_func_frame[id])
            {
                p_sdk->manager_inst.frame_inst.p_func_frame[id]->gsensor_en =
                    p_assist_en->assist.assist_gsensor_en;
                DEBUG_LOG("[%s] func id:%d gsensor %d\r\n", __FUNCTION__, id, p_assist_en->assist.assist_gsensor_en);
            }
            else
            {
                err_func.bits = err_func.bits & (1 << id);
            }
        }
    }

    if (1 == p_function_en->func.function_slot_en)
    {
        for (gh_func_fix_idx_e idx = GH_FUNC_FIX_IDX_PPG_CFG0; idx < GH_FUNC_FIX_IDX_MAX; idx++)
        {
            if (GH_NULL_PTR != p_sdk->manager_inst.frame_inst.p_func_frame[idx])
            {
                p_sdk->manager_inst.frame_inst.p_func_frame[idx]->gsensor_en =
                    p_assist_en->assist.assist_gsensor_en;
                DEBUG_LOG("[%s] slot idx:%d gsensor %d\r\n", __FUNCTION__, idx, p_assist_en->assist.assist_gsensor_en);
            }
        }
    }

    if (0 != err_func.bits)
    {
        ERROR_LOG("[%s] fail func: 0x%x\r\n", __FUNCTION__, err_func.bits);
        return GH_API_ASSIST_CFG_FAIL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_config_write(gh_config_reg_t *p_cfg, uint16_t number)
{
    if (GH_NULL_PTR == p_cfg || 0 == number)
    {
        return GH_API_NULL;
    }

    gh_sdk_t *p_sdk = &g_sdk;

#if (GH_GS_MOVE_DET_EN)
    gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_DIS);
#endif

#if (GH_MTSS_EN)
    gh_mtss_evt_manager_ctrl(&p_sdk->mtss_evt_inst, GH_MTSS_CTRL_DIS);
#endif

    gh_config_ret_e ret = gh_config_update(&p_sdk->manager_inst, &p_sdk->app_inf, p_cfg, number);

    if (GH_APP_CONFIG_OK != ret)
    {
        return GH_API_CONFIG_FAIL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_config_read(uint16_t r_addr, uint16_t r_len, uint16_t *p_r_val, uint16_t *p_r_len)
{
    if (GH_NULL_PTR == p_r_val || 0 == r_len || GH_NULL_PTR == p_r_len)
    {
        return GH_API_NULL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_ghealth_data_set(gh_data_t *p_data)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_NEED_INIT;
    }

    uint32_t ret = gh_app_manager_ghealth_data_set(&p_sdk->manager_inst, p_data);
    if (GH_RET_INERFACE_ERR_GET(ret))
    {
        ERROR_LOG("[%s] fail! ret = 0x%x\r\n", __FUNCTION__, ret);
        return GH_API_GHEALTH_DATA_FAIL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_gsensor_data_set(gh_gsensor_ts_and_data_t *p_data)
{
    gh_sdk_t *p_sdk = &g_sdk;
    uint32_t ret = 0;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_NEED_INIT;
    }

#if (GH_GS_MOVE_DET_EN)
    ret = gh_move_detector_pro(&p_sdk->move_det_inst, p_data);
    if (GH_MOVE_DET_OK != GH_RET_INERFACE_ERR_GET(ret))
    {
        // ERROR_LOG("[%s] move_detector fail! ret = 0x%x\r\n", __FUNCTION__, ret);
        return GH_API_MOVE_DETECTOR_PRO_FAIL;
    }
#endif

    ret = gh_app_manager_gsensor_data_set(&p_sdk->manager_inst, p_data);
    if (GH_MANAGER_OK != GH_RET_INERFACE_ERR_GET(ret))
    {
        ERROR_LOG("[%s] data_set fail! ret = 0x%x\r\n", __FUNCTION__, ret);
        return GH_API_GHEALTH_DATA_FAIL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_gsensor_data_sync_set(gh_gsensor_ts_and_data_t *p_data, uint16_t size)
{
    gh_sdk_t *p_sdk = &g_sdk;
    uint32_t ret = 0;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_NEED_INIT;
    }

#if (GH_GS_MOVE_DET_EN)
    for (uint32_t i = 0; i < size; i++)
    {
        ret = gh_move_detector_pro(&p_sdk->move_det_inst, &p_data[i]);
        if (GH_MOVE_DET_OK != GH_RET_INERFACE_ERR_GET(ret))
        {
            // ERROR_LOG("[%s] move_detector fail! ret = 0x%x\r\n", __FUNCTION__, ret);
            return GH_API_MOVE_DETECTOR_PRO_FAIL;
        }
    }
#endif

    ret = gh_app_manager_gsensor_data_sync_set(&p_sdk->manager_inst, p_data, size);
    if (GH_MANAGER_OK != GH_RET_INERFACE_ERR_GET(ret))
    {
        ERROR_LOG("[%s] data_set fail! ret = 0x%x\r\n", __FUNCTION__, ret);
        return GH_API_GHEALTH_DATA_FAIL;
    }

    return GH_API_OK;
}

gh_public_ret_e gh_demo_move_det_enable(uint8_t en)
{
    gh_sdk_t *p_sdk = &g_sdk;

    if (0 == p_sdk->api.init_flag)
    {
        return GH_API_NEED_INIT;
    }

#if (GH_GS_MOVE_DET_EN)
    uint32_t ret = 0;

    if (en)
    {
        ret = gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_EN);
    }
    else
    {
        ret = gh_move_detector_ctrl(&p_sdk->move_det_inst, GH_MOVE_DET_CTRL_DIS);
    }

    if (GH_MOVE_DET_OK != GH_RET_INERFACE_ERR_GET(ret))
    {
        return GH_API_GSENSOR_EN_FAIL;
    }
#endif

    return GH_API_OK;
}

