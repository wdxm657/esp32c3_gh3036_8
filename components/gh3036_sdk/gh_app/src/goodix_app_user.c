/**
  ****************************************************************************************
  * @file    goodix_app_user.c
  * @author  GHealth Driver Team
  * @brief   gh application user file
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
#if (GOODIX_PLATFORM == 1)
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "grx_hal.h"
#include "app_io.h"
#include "app_log.h"
#include "gh_task_common.h"
#include "gh_global_config.h"
#include "gh_public_api.h"
#include "gh_algo_adapter_common.h"
#if (1 == GH_PROTOCOL_EN)
#include "gh_data_package.h"
#endif
#include "board_sk.h"
#if (GH_SINGLE_THREAD == 1)
#include "gh_hal_isr.h"
#include "gh_hal_interface.h"
#include "gh_hal_config_process.h"
#endif
#if (1 == GH3036_DEMO)
#include "gh_hr_page.h"
#include "ui_control.h"
#include "g_sensor_manager.h"
#endif
#if (1 == GH3036_EVK)
#include "gsensor_sim_task.h"
#endif
#if (1 == GH3036_DEMO) || (1 == GH3036_EVK)
#include "gh_app_task.h"
#endif
#include "gh_hal_log.h"
#include "gh_app_user.h"

/*
 * DEFINES
 *****************************************************************************************
 */


/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
extern volatile uint32_t g_direct_sample;

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
uint8_t g_adt_working_state = 0;
#if (GH_MTSS_EN)
static gh_function_en_union_t g_func_en = {0};
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

uint32_t gh_demo_int_process(void)
{
#if (GH_SINGLE_THREAD == 1)
    return gh_hal_isr();
#else
    return 0;
#endif
}

uint32_t gh_demo_config_ctrl(gh_hal_config_ctrl_t *p_config_ctrl)
{
#if (GH_MULTI_THREAD == 1)
    gh_common_msg_t msg;

    msg.src_id = GH_APP_TASK_ID;
    msg.dst_id = GH_HAL_TASK_ID;
    msg.msg_id = GH_HAL_MSG_CONFIG_CTRL;
    msg.payload = pvPortMalloc(sizeof(gh_hal_config_ctrl_t));
    memcpy((void *)msg.payload, (void *)p_config_ctrl, sizeof(gh_hal_config_ctrl_t));
    msg.size = sizeof(gh_hal_config_ctrl_t);

    GH_MSG_SEND_AND_FREE(&msg, 0);

#elif (GH_SINGLE_THREAD == 1)

    switch (p_config_ctrl->op)
    {
        case GH_HAL_CONFIG_SET:
        {
            APP_LOG_DEBUG("[%s] GH_HAL_CONFIG_SET, %u\r\n", __FUNCTION__, p_config_ctrl->number);
            gh_exit_lowpower_mode();
            gh_hal_service_cfg_download(p_config_ctrl->p_reg, p_config_ctrl->number);
            gh_enter_lowpower_mode();
        }
        break;

        default:
        break;
    }
#endif

    return 0;
}

uint32_t gh_demo_sample_ctrl(gh_hal_sample_ctrl_t *p_sample_ctrl)
{
#if (GH_MULTI_THREAD == 1)
    gh_common_msg_t msg;

    msg.src_id = GH_APP_TASK_ID;
    msg.dst_id = GH_HAL_TASK_ID;
    msg.msg_id = GH_HAL_MSG_SAMPLE_CTRL;
    msg.payload = pvPortMalloc(sizeof(gh_hal_sample_ctrl_t));
    memcpy((void *)msg.payload, (void *)p_sample_ctrl, sizeof(gh_hal_sample_ctrl_t));
    msg.size = sizeof(gh_hal_sample_ctrl_t);

    GH_MSG_SEND_AND_FREE(&msg, 0);

#elif (GH_SINGLE_THREAD == 1)

    switch (p_sample_ctrl->op)
    {
        case GH_HAL_SAMPLE_TOP_STOP:
        {
            APP_LOG_DEBUG("[%s] GH_HAL_SAMPLE_TOP_STOP!\r\n", __FUNCTION__);
            gh_hal_top_ctrl(0);
        }
        break;

        case GH_HAL_SAMPLE_TOP_START:
        {
            APP_LOG_DEBUG("[%s] GH_HAL_SAMPLE_TOP_START!\r\n", __FUNCTION__);
            gh_hal_top_ctrl(1);
        }
        break;

        case GH_HAL_SAMPLE_CHN_EN:
        {
            APP_LOG_DEBUG("[%s] GH_HAL_SAMPLE_CHN_EN, %u\r\n", __FUNCTION__, p_sample_ctrl->ch_map.channel);
            gh_hal_channel_en(&p_sample_ctrl->ch_map);
        }
        break;

        default:
        break;
    }
#endif

    return 0;
}

uint32_t gh_demo_data_publish(gh_func_frame_t *p_frame)
{
#if (1 == GH_PROTOCOL_EN)
    gh_protocol_process(p_frame);
#endif

    /* Example1: get algorithm result */
#if 0
    if (NULL == p_frame->p_algo_res)
    {
        return 0;
    }

    switch (p_frame->id)
    {
        case GH_FUNC_FIX_IDX_ADT:
        {
            gh_algo_adt_result_t *p_adt_res = (gh_algo_adt_result_t *)p_frame->p_algo_res;
            APP_LOG_DEBUG("[ADT] wear_status:%d, det_status:%d, ctr:%d\r\n", p_adt_res->wear_evt,
                          p_adt_res->det_status, p_adt_res->ctr);
        }
        break;

        case GH_FUNC_FIX_IDX_HR:
        {
            gh_algo_hr_result_t *p_hr_res = (gh_algo_hr_result_t *)p_frame->p_algo_res;
            APP_LOG_DEBUG("[HR] flag:%d, out:%d, snr:%d, level:%d, score:%d, acc:%d, scence:%d\r\n",
                          p_hr_res->hba_out_flag, p_hr_res->hba_out, p_hr_res->hba_snr,
                          p_hr_res->valid_level, p_hr_res->valid_score, p_hr_res->hba_acc_info,
                          p_hr_res->hba_acc_scence);
        }
        break;

        case GH_FUNC_FIX_IDX_SPO2:
        {
            gh_algo_spo2_result_t *p_spo2_res = (gh_algo_spo2_result_t *)p_frame->p_algo_res;
            APP_LOG_DEBUG("[SPO2] final:%d, r:%d, confi:%d, level:%d, hb:%d, flag:%d\r\n",
                          p_spo2_res->final_spo2, p_spo2_res->r_val, p_spo2_res->confi_coeff,
                          p_spo2_res->valid_level, p_spo2_res->hb_mean, p_spo2_res->invalid_flag);
        }
        break;

        case GH_FUNC_FIX_IDX_HRV:
        {
            gh_algo_hrv_result_t *p_hrv_res = (gh_algo_hrv_result_t *)p_frame->p_algo_res;
            APP_LOG_DEBUG("[HRV] rri0:%d, rri1:%d, rri2:%d, rri3:%d, confi:%d, num:%d\r\n",
                          p_hrv_res->rri[0], p_hrv_res->rri[1], p_hrv_res->rri[2],
                          p_hrv_res->rri[3], p_hrv_res->confidence, p_hrv_res->valid_num);
        }
        break;

        case GH_FUNC_FIX_IDX_GNADT:
        case GH_FUNC_FIX_IDX_IRNADT:
        {
            gh_algo_nadt_result_t *p_nadt_res = (gh_algo_nadt_result_t *)p_frame->p_algo_res;
            APP_LOG_DEBUG("[NADT] wear_off_detect:%d, live_body_conf:%d\r\n",
                          GH_ALGO_NADT_RES0_MASK(p_nadt_res->nadt_out), p_nadt_res->live_body_conf);
        }
        break;

        default:
        break;
    } // switch (p_frame->id)

#endif

    /* Example2: get rawdata or debug data */
#if 0
    APP_LOG_DEBUG("[ch_num]:%d\r\n", p_frame->ch_num);

    for (uint8_t cnt = 0; cnt < p_frame->ch_num; cnt++)
    {
        switch (p_frame->p_ch_map[cnt].data_type)
        {
            case GH_PPG_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d] cnt:%d, ch_num:%d, func:%s, data_type:%s, cfg%d, rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_ppg.slot_cfg_id,
                              p_frame->p_ch_map[0].channel_ppg.rx_id);
            }
            break;

            case GH_PPG_MIX_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d] cnt:%d, ch_num:%d, func:%s, data_type:%s, cfg%d, rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_ppg_mix.slot_cfg_id,
                              p_frame->p_ch_map[cnt].channel_ppg_mix.rx_id);
            }
            break;

            case GH_PPG_BG_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d] cnt:%d,ch_num:%d,func:%s,data_type:%s,cfg%d,rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_ppg_bg.slot_cfg_id,
                              p_frame->p_ch_map[cnt].channel_ppg_bg.rx_id);
            }
            break;

            case GH_CAP_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d]cnt:%d,ch_num:%d,func:%s,data_type:%s,cfg%d,rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_cap.slot_cfg_id,
                              0);
            }
            break;

            case GH_PPG_PARAM_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d]cnt:%d,ch_num:%d,func:%s,data_type:%s,cfg%d,rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_ppg_param.slot_cfg_id,
                              p_frame->p_ch_map[cnt].channel_ppg_param.rx_id);
            }
            break;

            case GH_PPG_DRE_DATA:
            {
                APP_LOG_DEBUG("[DATA][%d]cnt:%d,ch_num:%d,func:%s,data_type:%s,cfg%d,rx%d\r\n",
                              p_frame->p_data[cnt].rawdata,
                              p_frame->frame_cnt,
                              cnt,
                              g_function_name[p_frame->id],
                              g_data_type_name[p_frame->p_ch_map[cnt].data_type],
                              p_frame->p_ch_map[cnt].channel_ppg_dre.slot_cfg_id,
                              p_frame->p_ch_map[cnt].channel_ppg_dre.rx_id);
            }
            break;

            default:
            break;
        }
    } // for (uint8_t cnt = 0; cnt < p_frame->ch_num; cnt++)
#endif

#if (1 == GH3036_DEMO)
    if (NULL == p_frame->p_algo_res)
    {
        return 0;
    }

    // APP_LOG_DEBUG("[gh_demo_data_publish]\r\n");

    switch (p_frame->id)
    {
        case GH_FUNC_FIX_IDX_HR:
        {
            gh_algo_hr_result_t *p_hr_res = (gh_algo_hr_result_t *)p_frame->p_algo_res;

            if (p_hr_res->hba_out_flag)
            {
                set_ui_hr_value(p_hr_res->hba_out);
            }
        }
        break;

        case GH_FUNC_FIX_IDX_SPO2:
        {
            gh_algo_spo2_result_t *p_spo2_res = (gh_algo_spo2_result_t *)p_frame->p_algo_res;

            if (p_spo2_res->update)
            {
                set_ui_spo2_value(p_spo2_res->final_spo2);
            }
        }
        break;

        case GH_FUNC_FIX_IDX_HRV:
        {
            gh_algo_hrv_result_t *p_hrv_res = (gh_algo_hrv_result_t *)p_frame->p_algo_res;

            if (p_hrv_res->update)
            {
                set_ui_hrv_value(p_hrv_res->rri[0]);
            }
        }
        break;

        case GH_FUNC_FIX_IDX_GNADT:
        case GH_FUNC_FIX_IDX_IRNADT:
        {
        }
        break;

        default:
        break;
    }
#endif

    return 0;
}

#if (GH_MTSS_EN)
uint32_t gh_demo_mtss_event_publish(gh_action_event_e evt, uint64_t timestamp)
{
#if 1
    switch (evt)
    {
        case GH_ACTION_WEAR_ON:
        {
            APP_LOG_DEBUG("[EVENT] wear on!\r\n");

            // example: add customer handle after wear on
            if (0 == g_direct_sample)
            {
#if (1 == GH3036_DEMO)
                UI_ReportWearState(1);
#endif
                gh_app_sample_ctrl_t func;
                gh_common_msg_t msg;

                gh_demo_function_get(&g_func_en);
                func.func_bits = g_func_en.bits;

                msg.src_id = GH_APP_TASK_ID;
                msg.dst_id = GH_APP_TASK_ID;
                msg.payload = pvPortMalloc(sizeof(gh_app_sample_ctrl_t));
                memcpy((void *)msg.payload, (void *)&func, sizeof(gh_app_sample_ctrl_t));
                msg.size = sizeof(gh_app_sample_ctrl_t);
                msg.msg_id = GH_APP_MSG_SAMPLE_START;
                GH_MSG_SEND_AND_FREE(&msg, 0);

                if (0 == g_adt_working_state)
                {
                    g_adt_working_state = 1;
                }
            }
        }
        break;

        case GH_ACTION_WEAR_OFF:
        {
            APP_LOG_DEBUG("[EVENT] wear off!\r\n");

            // example: add customer handle after wear off
            if (0 == g_direct_sample)
            {
#if (1 == GH3036_DEMO)
                UI_ReportWearState(0);
#endif
                gh_app_sample_ctrl_t func;
                gh_common_msg_t msg;
                g_func_en.func.function_adt_en = 0;
                g_func_en.func.function_gnadt_en = 0;
                g_func_en.func.function_hr_en = 0;
                g_func_en.func.function_spo2_en = 0;
                g_func_en.func.function_hrv_en = 0;
                func.func_bits = g_func_en.bits;

                msg.src_id = GH_APP_TASK_ID;
                msg.dst_id = GH_APP_TASK_ID;
                msg.payload = pvPortMalloc(sizeof(gh_app_sample_ctrl_t));
                memcpy((void *)msg.payload, (void *)&func, sizeof(gh_app_sample_ctrl_t));
                msg.size = sizeof(gh_app_sample_ctrl_t);
                msg.msg_id = GH_APP_MSG_SAMPLE_STOP;
                GH_MSG_SEND_AND_FREE(&msg, 0);

                g_func_en.func.function_adt_en = 1;
                g_func_en.func.function_gnadt_en = 0;
                g_func_en.func.function_hr_en = 0;
                g_func_en.func.function_spo2_en = 0;
                g_func_en.func.function_hrv_en = 0;
                func.func_bits = g_func_en.bits;

                msg.src_id = GH_APP_TASK_ID;
                msg.dst_id = GH_APP_TASK_ID;
                msg.payload = pvPortMalloc(sizeof(gh_app_sample_ctrl_t));
                memcpy((void *)msg.payload, (void *)&func, sizeof(gh_app_sample_ctrl_t));
                msg.size = sizeof(gh_app_sample_ctrl_t);
                msg.msg_id = GH_APP_MSG_SAMPLE_START;
                GH_MSG_SEND_AND_FREE(&msg, 0);

                if (0 == g_adt_working_state)
                {
                    g_adt_working_state = 1;
                }
            }
        }
        break;

        case GH_ACTION_MOVE:
        {
            APP_LOG_DEBUG("[EVENT] move detected!\r\n");

            // example: gsensor detects motion, add code about start Ghealth ADT sample
            if (0 == g_adt_working_state && 0 == g_direct_sample)
            {
                g_adt_working_state = 1;

                gh_app_sample_ctrl_t func;
                gh_common_msg_t msg;

                g_func_en.func.function_adt_en = 1;
                g_func_en.func.function_gnadt_en = 0;
                g_func_en.func.function_hr_en = 0;
                g_func_en.func.function_spo2_en = 0;
                g_func_en.func.function_hrv_en = 0;
                func.func_bits = g_func_en.bits;

                msg.src_id = GH_APP_TASK_ID;
                msg.dst_id = GH_APP_TASK_ID;
                msg.payload = pvPortMalloc(sizeof(gh_app_sample_ctrl_t));
                memcpy((void *)msg.payload, (void *)&func, sizeof(gh_app_sample_ctrl_t));
                msg.size = sizeof(gh_app_sample_ctrl_t);
                msg.msg_id = GH_APP_MSG_SAMPLE_START;
                GH_MSG_SEND_AND_FREE(&msg, 0);
#if ((GH_FUSION_MODE_SEL) == (GH_FUSION_MODE_SYNC))
                gh_move_det_timer_stop();
#endif
            }
        }
        break;

        case GH_ACTION_MOVE_TIME_OUT:
        {
            APP_LOG_DEBUG("[EVENT] move detect timeout!\r\n");

            // example: gsensor detects static, add code about stop Ghealth ADT sample
            if (1 == g_adt_working_state && 0 == g_direct_sample)
            {
                g_adt_working_state = 0;

                gh_app_sample_ctrl_t func;
                gh_common_msg_t msg;

                g_func_en.func.function_adt_en = 0;
                g_func_en.func.function_gnadt_en = 0;
                g_func_en.func.function_hr_en = 0;
                g_func_en.func.function_spo2_en = 0;
                g_func_en.func.function_hrv_en = 0;
                func.func_bits = g_func_en.bits;

                msg.src_id = GH_APP_TASK_ID;
                msg.dst_id = GH_APP_TASK_ID;
                msg.payload = pvPortMalloc(sizeof(gh_app_sample_ctrl_t));
                memcpy((void *)msg.payload, (void *)&func, sizeof(gh_app_sample_ctrl_t));
                msg.size = sizeof(gh_app_sample_ctrl_t);
                msg.msg_id = GH_APP_MSG_SAMPLE_STOP;
                GH_MSG_SEND_AND_FREE(&msg, 0);
#if ((GH_FUSION_MODE_SEL) == (GH_FUSION_MODE_SYNC))
                gh_move_det_timer_start();
#endif
            }
        }
        break;

        default:
        break;
    }
#endif

    return 0;
}
#else

uint32_t gh_demo_action_event_publish(gh_action_event_e evt, uint64_t timestamp)
{
    switch (evt)
    {
        case GH_ACTION_ADT_WEAR_ON:
        {
            // example: add customer handle after ADT wear on
        }
        break;

        case GH_ACTION_ADT_WEAR_OFF:
        {
            // example: add customer handle after ADT wear off
        }
        break;

        case GH_ACTION_NADT_WEAR_ON:
        {
            // example: add customer handle after NADT wear on
        }
        break;

        case GH_ACTION_NADT_WEAR_OFF:
        {
            // example: add customer handle after NADT wear off(non-living)
        }
        break;

        case GH_ACTION_MOVE:
        {
            // example: gsensor detects motion, add code about start Ghealth ADT sample
#if ((GH_FUSION_MODE_SEL) == (GH_FUSION_MODE_SYNC))
            gh_move_det_timer_stop();
#endif
        }
        break;

        case GH_ACTION_MOVE_TIME_OUT:
        {
            // example: gsensor detects static, add code about stop Ghealth ADT sample
#if ((GH_FUSION_MODE_SEL) == (GH_FUSION_MODE_SYNC))
            gh_move_det_timer_start();
#endif
        }
        break;

        default:
        break;
    }

    return 0;
}
#endif

#if (1 == GH_APP_MIPS_STA_EN)
uint64_t gh_demo_app_mips_ts_get(void)
{
    return bsp_timestamp_get();
}
#endif

#if ((GH_FUSION_MODE_SEL) == (GH_FUSION_MODE_SYNC))
uint32_t gh_move_det_timer_start(void)
{
    gs_timer_start();
    return 0;
}

uint32_t gh_move_det_timer_stop(void)
{
    gs_timer_stop();
    return 0;
}
#endif
#endif // #if (GOODIX_PLATFORM == 1)
