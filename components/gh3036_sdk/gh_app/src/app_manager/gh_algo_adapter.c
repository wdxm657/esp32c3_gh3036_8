/**
  ****************************************************************************************
  * @file    gh_algo_adapter.c
  * @author  GHealth Driver Team
  * @brief   gh algorithm adapter
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
#include "gh_hal_utils.h"
#include "gh_global_config.h"
#include "gh_data_common.h"
#include "gh_algo_adapter_common.h"
#if (1 == GH_APP_MIPS_STA_EN)
#include "gh_app_mips.h"
#endif
#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
#include "goodix_log.h"
#endif
#include "gh_algo_adapter.h"

#if (1 == GH_ALGO_ADAPTER_EN)

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

#define GH_ALGO_HR_FS_ADDR                  0x1282
#define GH_ALGO_HRV_FS_ADDR                 0x1284
#define GH_ALGO_SPO2_FS_ADDR                0x128C
#define GH_ALGO_GNADT_FS_ADDR               0x1292
#define GH_ALGO_IRNADT_FS_ADDR               0x129E

#define GH_ALGO_8BIT_MASK                   0xFF
#define GH_ALGO_CH_OFFSET                   8
#define GH_ALGO_CHL_NUM_ADDR                0xC0
#define GH_ALGO_GREEN_CH01_ADDR             0xC4
#define GH_ALGO_GREEN_CH23_ADDR             0xC6
#define GH_ALGO_IR_CH01_ADDR                0xCC
#define GH_ALGO_IR_CH23_ADDR                0xCE
#define GH_ALGO_RED_CH01_ADDR               0xD4
#define GH_ALGO_RED_CH23_ADDR               0xD6

#define GH_ALGO_HR_BASE_ADDR                0x7500
#define GH_ALGO_HR_CHL_NUM_ADDR             0x75C0
#define GH_ALGO_HR_GREEN_CH01_ADDR          0x75C4
#define GH_ALGO_HR_GREEN_CH23_ADDR          0x75C6
#define GH_ALGO_HR_IR_CH01_ADDR             0x75CC
#define GH_ALGO_HR_IR_CH23_ADDR             0x75CE
#define GH_ALGO_HR_RED_CH01_ADDR            0x75D4
#define GH_ALGO_HR_RED_CH23_ADDR            0x75D6

#define GH_ALGO_HRV_BASE_ADDR               0x7800
#define GH_ALGO_HRV_CHL_NUM_ADDR            0x78C0
#define GH_ALGO_HRV_GREEN_CH01_ADDR         0x78C4
#define GH_ALGO_HRV_GREEN_CH23_ADDR         0x78C6
#define GH_ALGO_HRV_IR_CH01_ADDR            0x78CC
#define GH_ALGO_HRV_IR_CH23_ADDR            0x78CE
#define GH_ALGO_HRV_RED_CH01_ADDR           0x78D4
#define GH_ALGO_HRV_RED_CH23_ADDR           0x78D6

#define GH_ALGO_SPO2_BASE_ADDR              0x8400
#define GH_ALGO_SPO2_CHL_NUM_ADDR           0x84C0
#define GH_ALGO_SPO2_GREEN_CH01_ADDR        0x84C4
#define GH_ALGO_SPO2_GREEN_CH23_ADDR        0x84C6
#define GH_ALGO_SPO2_IR_CH01_ADDR           0x84CC
#define GH_ALGO_SPO2_IR_CH23_ADDR           0x84CE
#define GH_ALGO_SPO2_RED_CH01_ADDR          0x84D4
#define GH_ALGO_SPO2_RED_CH23_ADDR          0x84D6

#define GH_ALGO_GNADT_BASE_ADDR             0x8D00
#define GH_ALGO_GNADT_CHL_NUM_ADDR          0x8DC0
#define GH_ALGO_GNADT_GREEN_CH01_ADDR       0x8DC4
#define GH_ALGO_GNADT_GREEN_CH23_ADDR       0x8DC6
#define GH_ALGO_GNADT_IR_CH01_ADDR          0x8DCC
#define GH_ALGO_GNADT_IR_CH23_ADDR          0x8DCE
#define GH_ALGO_GNADT_RED_CH01_ADDR         0x8DD4
#define GH_ALGO_GNADT_RED_CH23_ADDR         0x8DD6

#define GH_ALGO_IRNADT_BASE_ADDR             0x9F00
#define GH_ALGO_IRNADT_CHL_NUM_ADDR          0x9FC0
#define GH_ALGO_IRNADT_GREEN_CH01_ADDR       0x9FC4
#define GH_ALGO_IRNADT_GREEN_CH23_ADDR       0x9FC6
#define GH_ALGO_IRNADT_IR_CH01_ADDR          0x9FCC
#define GH_ALGO_IRNADT_IR_CH23_ADDR          0x9FCE
#define GH_ALGO_IRNADT_RED_CH01_ADDR         0x9FD4
#define GH_ALGO_IRNADT_RED_CH23_ADDR         0x9FD6

#define GH_BASIC_IPD_SHIFT                   (2)

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gh_func_algo_inf_t g_algo_inf_map[GH_FUNC_FIX_IDX_ALGO_MAX] =
{
#if (1 == GH_USE_GOODIX_ADT_ALGO)
    {gh_algo_adt_init, gh_algo_adt_deinit, gh_algo_adt_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_ALGO_HR_EN) && (1 == GH_USE_GOODIX_HR_ALGO)
    {gh_goodix_algo_inf_init, gh_goodix_algo_inf_deinit, gh_goodix_algo_inf_process},
#elif (1 == GH_ALGO_HR_EN)
    {gh_algo_hr_init, gh_algo_hr_deinit, gh_algo_hr_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_ALGO_SPO2_EN) && (1 == GH_USE_GOODIX_SPO2_ALGO)
    {gh_goodix_algo_inf_init, gh_goodix_algo_inf_deinit, gh_goodix_algo_inf_process},
#elif (1 == GH_ALGO_SPO2_EN)
    {gh_algo_spo2_init, gh_algo_spo2_deinit, gh_algo_spo2_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_ALGO_HRV_EN) && (1 == GH_USE_GOODIX_HRV_ALGO)
    {gh_goodix_algo_inf_init, gh_goodix_algo_inf_deinit, gh_goodix_algo_inf_process},
#elif (1 == GH_ALGO_HRV_EN)
    {gh_algo_hrv_init, gh_algo_hrv_deinit, gh_algo_hrv_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_ALGO_GNADT_EN) && (1 == GH_USE_GOODIX_NADT_ALGO)
    {gh_goodix_algo_inf_init, gh_goodix_algo_inf_deinit, gh_goodix_algo_inf_process},
#elif (1 == GH_ALGO_GNADT_EN)
    {gh_algo_nadt_init, gh_algo_nadt_deinit, gh_algo_nadt_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_ALGO_IRNADT_EN) && (1 == GH_USE_GOODIX_NADT_ALGO)
    {gh_goodix_algo_inf_init, gh_goodix_algo_inf_deinit, gh_goodix_algo_inf_process},
#elif (1 == GH_ALGO_IRNADT_EN)
    {gh_algo_nadt_init, gh_algo_nadt_deinit, gh_algo_nadt_exe},
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif
};

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
const gh_goodix_algo_inf_t g_goodix_algo_inf_map[GH_FUNC_FIX_IDX_ALGO_MAX] =
{
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},

#if (1 == GH_USE_GOODIX_HR_ALGO)
    {
        goodix_hba_version, goodix_hba_config_get_arr, goodix_hba_config_get_version,
        goodix_hba_init, goodix_hba_deinit, GH_NULL_PTR
    },
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_USE_GOODIX_SPO2_ALGO)
    {
        goodix_spo2_version, goodix_spo2_config_get_instance, goodix_spo2_config_get_version,
        goodix_spo2_init, goodix_spo2_deinit, GH_NULL_PTR
    },
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_USE_GOODIX_HRV_ALGO)
    {
        goodix_hrv_version, goodix_hrv_config_get_arr, goodix_hrv_config_get_version,
        goodix_hrv_init, goodix_hrv_deinit, GH_NULL_PTR
    },
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_USE_GOODIX_NADT_ALGO)
    {
        goodix_nadt_version, goodix_nadt_config_get_arr, goodix_nadt_config_get_version,
        goodix_nadt_init, goodix_nadt_deinit, GH_NULL_PTR
    },
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif

#if (1 == GH_USE_GOODIX_NADT_ALGO)
    {
        goodix_nadt_version, goodix_nadt_config_get_arr, goodix_nadt_config_get_version,
        goodix_nadt_init, goodix_nadt_deinit, GH_NULL_PTR
    },
#else
    {GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR, GH_NULL_PTR},
#endif
};
#endif
#if (1 == GH_ALGO_GNADT_EN) || (1 == GH_ALGO_IRNADT_EN)
const int8_t g_nadt_result[4][15] = {"Normal", "Wear on", "Wear off", "Non-living"};
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
gh_adapter_ret_e gh_algo_adapter_init(gh_func_algo_adapter_t *p_adapter)
{
    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

    /* clear adapter instance */
    gh_memset((void *)p_adapter, 0, sizeof(gh_func_algo_adapter_t));

    /* set algorithm interface */
    gh_memcpy((void *)p_adapter->algo_inf, (void *)g_algo_inf_map, sizeof(p_adapter->algo_inf));

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
    gh_memcpy((void *)p_adapter->goodix_algo_inf, (void *)g_goodix_algo_inf_map, sizeof(p_adapter->goodix_algo_inf));
#endif

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
#if (1 == GH_USE_DYNAMIC_ALGO_MEM)
    p_adapter->p_algo_mem_buf = gh_algo_mem_malloc(GH_ALGOS_MEM_SIZE_FINAL);
    void *mem_addr = (void *)p_adapter->p_algo_mem_buf;
#else
    void *mem_addr = (void *)p_adapter->algo_mem_buf;
#endif

    int32_t ret = goodix_mem_init(mem_addr, GH_ALGOS_MEM_SIZE_FINAL);
    if (0 != ret)
    {
        ERROR_LOG("[%s] algo mem ini fail:%u\r\n", __FUNCTION__, ret);
#if (1 == GH_USE_DYNAMIC_ALGO_MEM)
        gh_algo_mem_free((void *)p_adapter->p_algo_mem_buf);
        p_adapter->p_algo_mem_buf = GH_NULL_PTR;
#endif
        return GH_ADAPTER_MEM_ERR;
    }
    DEBUG_LOG("[%s] algo mem init ok! size:%d\r\n", __FUNCTION__, GH_ALGOS_MEM_SIZE_FINAL);

//    goodix_set_sprintFunc(printf, GH_NULL_PTR, GH_NULL_PTR);
#endif

    /* initialize each function */
#if (1 == GH_ALGO_ADT_EN)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_ADT].p_res = (void *)&p_adapter->adt_res;
    p_adapter->algo_param[GH_FUNC_FIX_IDX_ADT].p_param = (void *)&p_adapter->adt_param;
    #if (1 == GH_USE_GOODIX_ADT_ALGO)
    p_adapter->adt_param.p_algo_inst = (void *)&p_adapter->adt_inst;
    #endif
    p_adapter->adt_param.p_inf = GH_NULL_PTR;
#endif

#if (1 == GH_ALGO_HR_EN)
    p_adapter->algo_param[GH_FUNC_FIX_IDX_HR].p_param = (void *)&p_adapter->hr_param;
    p_adapter->hr_param.p_algo_inst = (void *)&p_adapter->hr_inst;
#if (1 == GH_USE_GOODIX_HR_ALGO)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_HR].p_res = (void *)&p_adapter->hr_res;
    p_adapter->hr_param.p_inf = &p_adapter->goodix_algo_inf[GH_FUNC_FIX_IDX_HR];
#endif
#endif

#if (1 == GH_ALGO_SPO2_EN)
    p_adapter->algo_param[GH_FUNC_FIX_IDX_SPO2].p_param = (void *)&p_adapter->spo2_param;
    p_adapter->spo2_param.p_algo_inst = GH_NULL_PTR;
#if (1 == GH_USE_GOODIX_SPO2_ALGO)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_SPO2].p_res = (void *)&p_adapter->spo2_res;
    p_adapter->spo2_param.p_inf = &p_adapter->goodix_algo_inf[GH_FUNC_FIX_IDX_SPO2];
#endif
#endif

#if (1 == GH_ALGO_HRV_EN)
    p_adapter->algo_param[GH_FUNC_FIX_IDX_HRV].p_param = (void *)&p_adapter->hrv_param;
    p_adapter->hrv_param.p_algo_inst = (void *)&p_adapter->hrv_inst;
#if (1 == GH_USE_GOODIX_HRV_ALGO)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_HRV].p_res = (void *)&p_adapter->hrv_res;
    p_adapter->hrv_param.p_inf = &p_adapter->goodix_algo_inf[GH_FUNC_FIX_IDX_HRV];
#endif
#if ((1 == GH_ALGO_HR_EN) &&(1 == GH_USE_GOODIX_HR_ALGO))
    p_adapter->hrv_inst.p_hr = (int32_t*)(&p_adapter->hr_res.hba_out);
#else
    p_adapter->hrv_inst.p_hr = GH_NULL_PTR;
#endif
#endif

#if (1 == GH_ALGO_GNADT_EN)

    p_adapter->algo_param[GH_FUNC_FIX_IDX_GNADT].p_param = (void *)&p_adapter->gnadt_param;
    p_adapter->gnadt_param.p_algo_inst = GH_NULL_PTR;
#if (1 == GH_USE_GOODIX_NADT_ALGO)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_GNADT].p_res = (void *)&p_adapter->gnadt_res;
    p_adapter->gnadt_param.p_inf = &p_adapter->goodix_algo_inf[GH_FUNC_FIX_IDX_GNADT];
#endif
#endif

#if (1 == GH_ALGO_IRNADT_EN)
    p_adapter->algo_result[GH_FUNC_FIX_IDX_IRNADT].p_res = (void *)&p_adapter->irnadt_res;
    p_adapter->algo_param[GH_FUNC_FIX_IDX_IRNADT].p_param = (void *)&p_adapter->irnadt_param;
    p_adapter->irnadt_param.p_algo_inst = GH_NULL_PTR;
    #if (1 == GH_USE_GOODIX_NADT_ALGO)
    p_adapter->irnadt_param.p_inf = &p_adapter->goodix_algo_inf[GH_FUNC_FIX_IDX_IRNADT];
    #endif
#endif

    /* reset adapter */
    gh_algo_adapter_reset(p_adapter);

    return GH_ADAPTER_OK;
}

gh_adapter_ret_e gh_algo_adapter_reset(gh_func_algo_adapter_t *p_adapter)
{
    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

    /* reset channel information for each algorithm */
#if (1 == GH_ALGO_ADT_EN)
    gh_memset((void *)p_adapter->adt_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
    p_adapter->adt_res.wear_evt = GH_ADT_WEAR_IDLE;
    p_adapter->adt_res.det_status = GH_ADT_WEAR_DET_UNKONW;
#endif

#if (1 == GH_ALGO_HR_EN)
    gh_memset((void *)p_adapter->hr_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
#if (1 == GH_USE_GOODIX_HR_ALGO)
    gh_memset((void *)&p_adapter->hr_res, 0, sizeof(p_adapter->hr_res));
    p_adapter->hr_inst.scence = HBA_SCENES_DEFAULT;
#else
    p_adapter->hr_inst.scence = 0;
#endif
#endif

#if (1 == GH_ALGO_HRV_EN)
    gh_memset((void *)p_adapter->hrv_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
#if GH_USE_GOODIX_HRV_ALGO
    gh_memset((void *)&p_adapter->hrv_res, 0, sizeof(p_adapter->hrv_res));
#endif
#endif

#if (1 == GH_ALGO_SPO2_EN)
    gh_memset((void *)p_adapter->spo2_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
#if GH_USE_GOODIX_SPO2_ALGO
    gh_memset((void *)&p_adapter->spo2_res, 0, sizeof(p_adapter->spo2_res));
#endif
#endif

#if (1 == GH_ALGO_GNADT_EN)
    gh_memset((void *)p_adapter->gnadt_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
#if GH_USE_GOODIX_NADT_ALGO
    gh_memset((void *)&p_adapter->gnadt_res, 0, sizeof(p_adapter->gnadt_res));
#endif
#endif

#if (1 == GH_ALGO_IRNADT_EN)
    gh_memset((void *)p_adapter->irnadt_param.chl_map, GH_ALGO_CHL_MAP_DEFAULT, sizeof(uint8_t) * GH_ALGO_CHL_MAX);
    gh_memset((void *)&p_adapter->irnadt_res, 0, sizeof(p_adapter->irnadt_res));
#endif

    for (gh_func_fix_idx_e i = GH_FUNC_FIX_IDX_ADT; i < GH_FUNC_FIX_IDX_ALGO_MAX; i++)
    {
        p_adapter->algo_state[i] = GH_ALGO_IDLE;
    }

    return GH_ADAPTER_OK;
}

gh_adapter_ret_e gh_algo_adapter_deinit(gh_func_algo_adapter_t *p_adapter)
{
    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

#if (GH_GOODIX_ALGO_ENABLE_MODE)
    /* deinit algo memory */
    goodix_mem_deinit();
#endif

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
#if (1 == GH_USE_DYNAMIC_ALGO_MEM)
    gh_algo_mem_free((void *)p_adapter->p_algo_mem_buf);
#endif
#endif

    /* deinit algo adapter */
    gh_memset((void *)p_adapter->algo_inf, 0, sizeof(p_adapter->algo_inf));

    return GH_ADAPTER_OK;
}

gh_adapter_ret_e gh_algo_init(gh_func_algo_adapter_t *p_adapter, uint32_t func_mask)
{
    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

    // DEBUG_LOG("[%s] func_mask = 0x%x\r\n", __FUNCTION__, func_mask);

    uint32_t ret = 0;

    /* Call the initialization interface of each enabled algorithm */
    for (gh_func_fix_idx_e i = GH_FUNC_FIX_IDX_ADT; i < GH_FUNC_FIX_IDX_ALGO_MAX; i++)
    {
        if (((func_mask >> i) & 0x1) && p_adapter->algo_inf[i].init)
        {
            ret = p_adapter->algo_inf[i].init(i, (gh_func_algo_param_t *)p_adapter->algo_param[i].p_param);
            if (GH_ALGO_OK != GH_RET_INERFACE_ERR_GET(ret))
            {
                ERROR_LOG("[%s] %s init fail, 0x%x\r\n", __FUNCTION__, g_function_name[i],
                          GH_RET_INTERNAL_ERR_GET(ret));
            }
            p_adapter->algo_state[i] = GH_ALGO_INITED;
#if (1 == GH_APP_MIPS_STA_EN)
            gh_func_algo_param_t *p_algo_param = p_adapter->algo_param[i].p_param;
            gh_app_mips_freq_set(gh_app_mips_inst_get(), i, p_algo_param->fs);
#endif
        }
    }

    return GH_ADAPTER_OK;
}

gh_adapter_ret_e gh_algo_deinit(gh_func_algo_adapter_t *p_adapter, uint32_t func_mask)
{
    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

    uint32_t ret = 0;

    /* Call the deinitialization interface of each enabled algorithm */
    for (gh_func_fix_idx_e i = GH_FUNC_FIX_IDX_ADT; i < GH_FUNC_FIX_IDX_ALGO_MAX; i++)
    {
        if (((func_mask >> i) & 0x1) && p_adapter->algo_inf[i].deinit)
        {
            ret = p_adapter->algo_inf[i].deinit(i, (gh_func_algo_param_t *)p_adapter->algo_param[i].p_param);
            if (GH_ALGO_OK != GH_RET_INERFACE_ERR_GET(ret))
            {
                ERROR_LOG("[%s] %s deinit fail, 0x%x\r\n", __FUNCTION__, g_function_name[i],
                          GH_RET_INTERNAL_ERR_GET(ret));
            }
            p_adapter->algo_state[i] = GH_ALGO_DEINITED;
        }
    }

    return GH_ADAPTER_OK;
}

gh_adapter_ret_e gh_algo_process(gh_func_algo_adapter_t *p_adapter, gh_func_frame_t *p_frame)
{
    if (GH_NULL_PTR == p_adapter || GH_NULL_PTR == p_frame || GH_FUNC_FIX_IDX_ALGO_MAX <= p_frame->id)
    {
        return GH_ADAPTER_NULL;
    }

    uint32_t ret = 0;

    /* Get the algorithm result object for each frame */
    p_frame->p_algo_res = p_adapter->algo_result[p_frame->id].p_res;

    /* Call the process interface of each enabled algorithm */
    if (p_adapter->algo_inf[p_frame->id].process && GH_ALGO_INITED == p_adapter->algo_state[p_frame->id])
    {
        gh_func_algo_param_t *p_param = (gh_func_algo_param_t *)p_adapter->algo_param[p_frame->id].p_param;
#if (1 == GH_APP_MIPS_STA_EN)
        gh_app_mips_algo_start(gh_app_mips_inst_get(), p_frame->id);
#endif

#if (1 == GH_STACK_INFO_EN)
        gh_algo_ret_e gh_goodix_algo_inf_process(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param);
        gh_goodix_algo_inf_process(p_frame, p_param);
#else
        ret = p_adapter->algo_inf[p_frame->id].process(p_frame, p_param);
#endif
        if (GH_ALGO_OK != GH_RET_INERFACE_ERR_GET(ret))
        {
            ERROR_LOG("[%s] %s process fail, 0x%x\r\n", __FUNCTION__, g_function_name[p_frame->id],
                      GH_RET_INTERNAL_ERR_GET(ret));
        }
#if (1 == GH_APP_MIPS_STA_EN)
        gh_app_mips_algo_stop(gh_app_mips_inst_get(), p_frame->id);
#endif
    }

    return GH_ADAPTER_OK;
}

gh_algo_ret_e gh_goodix_algo_inf_init(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param)
{
    if (GH_NULL_PTR == p_param || GH_NULL_PTR == p_param->p_inf)
    {
        return GH_ALGO_NULL;
    }

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
    DEBUG_LOG("[%s] %s fs:%d, chl_num:%d\r\n", __FUNCTION__, g_function_name[id], p_param->fs, p_param->chl_num);

    int8_t version[150] = {0};
    int8_t inter_ver[GH_ALGO_INF_VERSION_LEN_MAX] = {0};
    const void *p_cfg_inst = GH_NULL_PTR;
    const int8_t *p_ver = GH_NULL_PTR;
    uint32_t cfg_size = 0;
    int32_t ret = 0;

    if (GH_NULL_PTR == p_param->p_inf->algo_cfg_ver_get || GH_NULL_PTR == p_param->p_inf->algo_init)
    {
        return GH_ALGO_INF_ERR;
    }

    if (p_param->p_inf->algo_ver)
    {
        ret = p_param->p_inf->algo_ver((uint8_t *)version);
        if (ret)
        {
            ERROR_LOG("[%s]%s init fail : 0x%x\r\n", __FUNCTION__, g_function_name[id], ret);
            return GH_ALGO_VER_ERR;
        }
        DEBUG_LOG("[%s]%s algorithm version : %s\r\n", __FUNCTION__, g_function_name[id], version);
    }

    if (p_param->p_inf->algo_cfg_get)
    {
        switch (id)
        {
    #if (1 == GH_USE_GOODIX_HR_ALGO)
            case GH_FUNC_FIX_IDX_HR:
            {
                goodix_hba_config algo_cfg;
                goodix_hba_config *p_cfg = (goodix_hba_config *)p_param->p_inf->algo_cfg_get();
                gh_memcpy((void *)&algo_cfg, p_cfg, sizeof(goodix_hba_config));
                algo_cfg.fs = p_param->fs;
                algo_cfg.valid_ch_num = p_param->chl_num;

                DEBUG_LOG("[%s]%s:params = %u,%u,%d,%d,%d,%u,%u,%u,%u\r\n", __FUNCTION__,
                          g_function_name[id],
                          algo_cfg.mode,
                          algo_cfg.fs,
                          algo_cfg.valid_ch_num,
                          algo_cfg.hba_earliest_output_time,
                          algo_cfg.hba_latest_output_time,
                          algo_cfg.sigma,
                          algo_cfg.raw_ppg_scale,
                          algo_cfg.delay_time,
                          algo_cfg.valid_score_scale);
                p_cfg_inst = (void *)&algo_cfg;
                cfg_size = sizeof(algo_cfg);
                p_param->p_inf->algo_cfg_ver_get(inter_ver, GH_ALGO_INF_VERSION_LEN_MAX);
                p_ver = (const int8_t *)inter_ver;
                ret = p_param->p_inf->algo_init(p_cfg_inst, cfg_size, p_ver);
                if (ret)
                {
                    ERROR_LOG("[%s] fail! ret= 0x%x\r\n", __FUNCTION__, ret);
                    return GH_ALGO_INIT_ERR;
                }
            }
            break;
    #endif
    #if (1 == GH_USE_GOODIX_SPO2_ALGO)
            case GH_FUNC_FIX_IDX_SPO2:
            {
                goodix_spo2_config algo_cfg;
                goodix_spo2_config *p_cfg = (goodix_spo2_config *)p_param->p_inf->algo_cfg_get();
                gh_memcpy((void *)&algo_cfg, p_cfg, sizeof(goodix_spo2_config));
                algo_cfg.raw_fs = p_param->fs;
                algo_cfg.valid_chl_num = p_param->chl_num;

                DEBUG_LOG("[%s]%s: params1 = %u,%u,%d,%d,%d,%d,%d,%u,%u,%u,%u,%u,%u,%u,%u,%u\r\n", __FUNCTION__,
                          g_function_name[id],
                          algo_cfg.valid_chl_num,
                          algo_cfg.raw_fs,
                          algo_cfg.cali_coef_a4,
                          algo_cfg.cali_coef_a3,
                          algo_cfg.cali_coef_a2,
                          algo_cfg.cali_coef_a1,
                          algo_cfg.cali_coef_a0,
                          algo_cfg.hb_en_flg,
                          algo_cfg.wear_mode,
                          algo_cfg.acc_thr_max,
                          algo_cfg.acc_thr_min,
                          algo_cfg.acc_thr_scale,
                          algo_cfg.acc_thr_num,
                          algo_cfg.acc_thr_angle,
                          algo_cfg.ctr_en_flg,
                          algo_cfg.ctr_red_thr);
                DEBUG_LOG("[%s]: params2 = %u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\r\n", __FUNCTION__,
                          algo_cfg.ppg_jitter_thr,
                          algo_cfg.ppg_noise_thr,
                          algo_cfg.ppg_coeff_thr,
                          algo_cfg.quality_module_key,
                          algo_cfg.low_spo2_thr,
                          algo_cfg.fast_out_time,
                          algo_cfg.slow_out_time,
                          algo_cfg.min_stable_time_high,
                          algo_cfg.min_stable_time_low,
                          algo_cfg.max_spo2_variation_high,
                          algo_cfg.max_spo2_variation_low,
                          algo_cfg.ouput_module_key);
                p_cfg_inst = (void *)&algo_cfg;
                cfg_size = sizeof(algo_cfg);
                p_param->p_inf->algo_cfg_ver_get(inter_ver, GH_ALGO_INF_VERSION_LEN_MAX);
                p_ver = (const int8_t *)inter_ver;
                ret = p_param->p_inf->algo_init(p_cfg_inst, cfg_size, p_ver);
                if (ret)
                {
                    ERROR_LOG("[%s] fail! ret= 0x%x\r\n", __FUNCTION__, ret);
                    return GH_ALGO_INIT_ERR;
                }
            }
            break;
    #endif
    #if (1 == GH_USE_GOODIX_HRV_ALGO)
            case GH_FUNC_FIX_IDX_HRV:
            {
                goodix_hrv_config algo_cfg;
                goodix_hrv_config *p_cfg = (goodix_hrv_config *)p_param->p_inf->algo_cfg_get();
                gh_memcpy((void *)&algo_cfg, p_cfg, sizeof(goodix_hrv_config));
                algo_cfg.fs = p_param->fs;

                DEBUG_LOG("[%s]%s:params = %d,%u,%d,%d,%d,%d\r\n", __FUNCTION__,
                          g_function_name[id],
                          algo_cfg.need_ipl,
                          algo_cfg.fs,
                          algo_cfg.acc_thr[0],
                          algo_cfg.acc_thr[1],
                          algo_cfg.acc_thr[2],
                          algo_cfg.acc_thr[3]);
                p_cfg_inst = (void *)&algo_cfg;
                cfg_size = sizeof(algo_cfg);
                p_param->p_inf->algo_cfg_ver_get(inter_ver, GH_ALGO_INF_VERSION_LEN_MAX);
                p_ver = (const int8_t *)inter_ver;
                ret = p_param->p_inf->algo_init(p_cfg_inst, cfg_size, p_ver);
                if (ret)
                {
                    ERROR_LOG("[%s] fail! ret= 0x%x\r\n", __FUNCTION__, ret);
                    return GH_ALGO_INIT_ERR;
                }
            }
            break;
    #endif
    #if (1 == GH_USE_GOODIX_NADT_ALGO)
            case GH_FUNC_FIX_IDX_GNADT:
            case GH_FUNC_FIX_IDX_IRNADT:
            {
                goodix_nadt_config algo_cfg;
                goodix_nadt_config *p_cfg = (goodix_nadt_config *)p_param->p_inf->algo_cfg_get();
                gh_memcpy((void *)&algo_cfg, p_cfg, sizeof(goodix_nadt_config));
                algo_cfg.sample_rate = p_param->fs;

                DEBUG_LOG("[%s]%s param0 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          g_function_name[id],
                          algo_cfg.sample_rate, algo_cfg.unwear_detect_level, algo_cfg.check_timeout_thr,
                          algo_cfg.unwear_check_cnt_thr, algo_cfg.wear_check_cnt_thr, algo_cfg.adt_wear_on_thr,
                          algo_cfg.adt_wear_off_thr, algo_cfg.acc_std_thr, algo_cfg.live_detect_enable,
                          algo_cfg.raw_pv_len_thr, algo_cfg.heart_beat_thr_low,
                          algo_cfg.heart_beat_thr_high, algo_cfg.dif_rr_inter_thr, algo_cfg.wear_acf_score_thr);
                DEBUG_LOG("[%s] param1 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          algo_cfg.base_line_pos_ratio_thr, algo_cfg.base_line_diff_thr, algo_cfg.sig_scale_thr,
                          algo_cfg.bg_jump_detect_enable, algo_cfg.jump_bg_diff_thr, algo_cfg.bg_peak_valley_num_thr,
                          algo_cfg.bg_period_detect_enable, algo_cfg.period_bg_diff_thr, algo_cfg.bg_period_thr,
                          algo_cfg.bg_period_diff_thr);
                p_cfg_inst = (void *)&algo_cfg;
                cfg_size = sizeof(algo_cfg);
                p_param->p_inf->algo_cfg_ver_get(inter_ver, GH_ALGO_INF_VERSION_LEN_MAX);
                p_ver = (const int8_t *)inter_ver;
                ret = p_param->p_inf->algo_init(p_cfg_inst, cfg_size, p_ver);
                if (ret)
                {
                    ERROR_LOG("[%s] fail! ret= 0x%x\r\n", __FUNCTION__, ret);
                    return GH_ALGO_INIT_ERR;
                }
            }
            break;
    #endif
            default:
            break;
        }
    }
#endif

    return GH_ALGO_OK;
}

gh_algo_ret_e gh_goodix_algo_inf_deinit(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param)
{
#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
    DEBUG_LOG("[%s] %s deinit\r\n", __FUNCTION__, g_function_name[id]);

    int32_t ret;

    if (p_param->p_inf->algo_deinit)
    {
        ret = p_param->p_inf->algo_deinit();
        if (ret)
        {
            ERROR_LOG("[%s] %s deinit fail! error code = 0x%x\r\n", __FUNCTION__, g_function_name[id], ret);
            return GH_ALGO_DEINIT_ERR;
        }
    }
#endif

    return GH_ALGO_OK;
}

gh_algo_ret_e gh_goodix_algo_inf_process(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param)
{
    if (GH_NULL_PTR == p_frame || GH_NULL_PTR == p_param)
    {
        return GH_ALGO_NULL;
    }

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
    int32_t ret = 0;
    int32_t rawdata[GH_ALGO_CHL_MAX] = {0};
    #if (1 == GH_USE_GOODIX_NADT_ALGO)
    int32_t ipd[GH_ALGO_CHL_MAX] = {0};
    #endif
    uint16_t drv_cur[GH_ALGO_CHL_MAX] = {0};
    uint8_t enable_flg[GH_ALGO_EN_FLAG_MAX] = {0};
    uint8_t gain_code[GH_ALGO_CHL_MAX] = {0};
    gh_frame_data_t *p_temp_data = p_frame->p_data;

    /* Poll and check the data of each channel */
    for (uint8_t chl = 0; chl < GH_ALGO_CHL_MAX; chl++)
    {
        if (p_param->chl_map[chl] != GH_ALGO_CHL_MAP_DEFAULT && p_param->chl_map[chl] < p_frame->ch_num
            && GH_PPG_DATA == p_frame->p_ch_map->data_type)
        {
            p_temp_data = p_frame->p_data;
            p_temp_data = p_temp_data + p_param->chl_map[chl];
            if (GH_FUNC_FIX_IDX_GNADT == p_frame->id || GH_FUNC_FIX_IDX_IRNADT == p_frame->id)
            {
    #if (1 == GH_USE_GOODIX_NADT_ALGO)
                rawdata[chl] = p_temp_data->rawdata;
                ipd[chl] = p_temp_data->ipd_pa;
    #endif
            }
            else
            {
#if GH_ALGO_DATA_TYPE == GH_ALGO_ADC_IPD_DATA
                rawdata[chl] = p_temp_data->ipd_pa;
    #if (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_BASIC || GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_MEDIUM)
                if (GH_FUNC_FIX_IDX_HR == p_frame->id)
                {
                    rawdata[chl] = rawdata[chl] >> GH_BASIC_IPD_SHIFT;
                }
    #endif
    #if (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_BASIC || GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_MEDIUM)
                if (GH_FUNC_FIX_IDX_SPO2 == p_frame->id)
                {
                    rawdata[chl] = rawdata[chl] >> GH_BASIC_IPD_SHIFT;
                }
    #endif
#else
                rawdata[chl] = p_temp_data->rawdata;
#endif
            }
            gain_code[chl] = p_temp_data->agc_info.gain_code;
            drv_cur[chl] = p_temp_data->agc_info.led_drv0 * p_frame->led_drv_fs[0] / GH_DRV_CODE_FS
                           + p_temp_data->agc_info.led_drv1 * p_frame->led_drv_fs[1] / GH_DRV_CODE_FS;
            enable_flg[chl / GH_ALGO_BYTE_BITS] = enable_flg[chl / GH_ALGO_BYTE_BITS]
                                                  | (1 << (GH_ALGO_EN_FLAG_OFFSET - (chl % GH_ALGO_BYTE_BITS)));

//            DEBUG_LOG("[%s] chl:%d, data_offset:%d, rawdata:%d, enable:%d, %d, gsensor:%d, %d, %d\r\n", __FUNCTION__,
//                      chl, p_param->chl_map[chl], rawdata[chl], enable_flg[0], enable_flg[0],
//                      p_frame->gsensor_data.acc[GH_GSENSOR_X_INDEX],
//                      p_frame->gsensor_data.acc[GH_GSENSOR_Y_INDEX],
//                      p_frame->gsensor_data.acc[GH_GSENSOR_Z_INDEX]);
        }
    }

    /* Preparing input data for the algorithm */
    gh_algo_common_input_t input_data = {0};
    input_data.frame_id = p_frame->frame_cnt;
    input_data.total_ch_num = GH_ALGO_PPG_CHL_NUM;
    input_data.enable_flg = enable_flg;
    input_data.ppg_rawdata = rawdata;
    input_data.ch_agc_gain = gain_code;
    input_data.ch_agc_drv = drv_cur;
    input_data.acc_x = p_frame->gsensor_data.acc[GH_GSENSOR_X_INDEX];
    input_data.acc_y = p_frame->gsensor_data.acc[GH_GSENSOR_Y_INDEX];
    input_data.acc_z = p_frame->gsensor_data.acc[GH_GSENSOR_Z_INDEX];
    input_data.groy_x = 0;
    input_data.groy_y = 0;
    input_data.groy_z = 0;
    input_data.sleep_flg = 0;
    input_data.bit_num = GH_ALGO_DATA_BIT_NUM;
    input_data.chip_type = GH_ALGO_CHIP_TYPE;
    input_data.data_type = GH_ALGO_DATA_TYPE;

//    DEBUG_LOG("[%s] total_ch_num:%d, enable:%d, %d, GH_ALGO_CHL_MAX:%d\r\n", __FUNCTION__,
//              input_data.total_ch_num, enable_flg[0], enable_flg[0], GH_ALGO_CHL_MAX);

    /* Choose different inputs and outputs according to different algorithms */
    switch (p_frame->id)
    {
#if (1 == GH_USE_GOODIX_HR_ALGO)
        case GH_FUNC_FIX_IDX_HR:
        {
            goodix_hba_input_rawdata *p_input = (goodix_hba_input_rawdata *)&input_data;
            goodix_hba_result output_result = {0};
            gh_algo_hr_result_t *p_res = (gh_algo_hr_result_t *)p_frame->p_algo_res;
            goodix_hba_input_unique in_unique = {0};
            gh_algo_hr_t *p_inst = (gh_algo_hr_t *)p_param->p_algo_inst;

            if (p_inst)
            {
                in_unique.scence = p_inst->scence;
            }
            else
            {
                in_unique.scence = HBA_SCENES_DEFAULT;
            }
            p_input->pst_module_unique = &in_unique;
            ret = goodix_hba_calc(p_input, &output_result);

            if (GX_ALGO_HBA_SUCCESS == ret)
            {
                /* Heart rate algorithm results */
                gh_memcpy((void *)p_res, (void *)&output_result, sizeof(goodix_hba_result));
                if (p_res->hba_out_flag)
                {
                    DEBUG_LOG("[HR] %d, %d, %d, %d\r\n", p_res->hba_out,
                              p_res->valid_score, p_res->hba_snr, p_res->valid_level);
                }
            }
            else
            {
                ERROR_LOG("[%s] hr error! error code = 0x%x\r\n", __FUNCTION__, ret);
            }
        }
        break;
#endif
#if (1 == GH_USE_GOODIX_SPO2_ALGO)
        case GH_FUNC_FIX_IDX_SPO2:
        {
            goodix_spo2_input_rawdata *p_input = (goodix_spo2_input_rawdata *)&input_data;
            goodix_spo2_result output_result = {0};
            gh_algo_spo2_result_t *p_res = (gh_algo_spo2_result_t *)p_frame->p_algo_res;

            p_input->pst_module_unique = GH_NULL_PTR;
            ret = goodix_spo2_calc(p_input, &output_result);

            if (GX_ALGO_SPO2_SUCCESS == ret || GX_ALGO_SPO2_WIN_UNCOMPLETE == ret ||
                GX_ALGO_SPO2_FRAME_UNCOMPLETE == ret)
            {
                gh_memcpy((void *)p_res, (void *)&output_result, sizeof(goodix_spo2_result));

                /* Blood oxygen algorithm results */
                if (0 != p_res->final_calc_flg)
                {
                    DEBUG_LOG("[SPO2] %d, %d, %d, %d, %d, %d\r\n", p_res->final_spo2 / GH_ALGO_SPO2_OUT_COEF,
                              p_res->final_r_val,
                              p_res->final_confi_coeff,
                              p_res->final_valid_level,
                              p_res->final_hb_mean,
                              p_res->final_invalidFlg);
                }
            }
            else
            {
                ERROR_LOG("[%s] spo2 error! error code = 0x%x\r\n", __FUNCTION__, ret);
            }
        }
        break;
#endif
#if (1 == GH_USE_GOODIX_HRV_ALGO)
        case GH_FUNC_FIX_IDX_HRV:
        {
            goodix_hrv_input_rawdata *p_input = (goodix_hrv_input_rawdata *)&input_data;
            goodix_hrv_result output_result = {0};
            gh_algo_hrv_result_t *p_res = (gh_algo_hrv_result_t *)p_frame->p_algo_res;
            gh_algo_hrv_t *p_inst = (gh_algo_hrv_t *)p_param->p_algo_inst;
            goodix_hrv_input_unique in_unique = {0};

            if (p_inst->p_hr)
            {
                in_unique.hr = *p_inst->p_hr;
            }
            p_input->pst_module_unique = &in_unique;
            ret = goodix_hrv_calc(p_input, &output_result);
            gh_memset((void *)p_res, 0, sizeof(goodix_hrv_result));
            if (GX_HRV_ALGO_OK == ret || GX_HRV_ALGO_UPDATE == ret)
            {
                /* Heart rate variability algorithm results */
                gh_memcpy((void *)p_res, (void *)&output_result, sizeof(goodix_hrv_result));

                if (GX_HRV_ALGO_UPDATE == ret)
                {
                    DEBUG_LOG("[HRV] %d, %d, %d, %d, %d, %d\r\n", p_res->rri[GH_ALGO_HRV_RRI_NUM0],
                              p_res->rri[GH_ALGO_HRV_RRI_NUM1], p_res->rri[GH_ALGO_HRV_RRI_NUM2],
                              p_res->rri[GH_ALGO_HRV_RRI_NUM3], p_res->rri_confidence,
                              p_res->rri_valid_num);
                }
            }
            else
            {
                ERROR_LOG("[%s] hrv error! error code = 0x%x\r\n", __FUNCTION__, ret);
            }
        }
        break;
#endif
#if (1 == GH_USE_GOODIX_NADT_ALGO)
        case GH_FUNC_FIX_IDX_GNADT:
        case GH_FUNC_FIX_IDX_IRNADT:
        {
            goodix_nadt_input_rawdata *p_input = (goodix_nadt_input_rawdata *)&input_data;
            goodix_nadt_result output_result = {0};
            gh_algo_nadt_result_t *p_res = (gh_algo_nadt_result_t *)p_frame->p_algo_res;
            goodix_nadt_input_unique in_unique = {0};

            in_unique.ppg_colour_flg = (GH_FUNC_FIX_IDX_GNADT == p_frame->id) ?
                                        GH_ALGO_NADT_GREEN_FLAG : GH_ALGO_NADT_IR_FLAG;
            in_unique.pre_status = 0;
            in_unique.ch_ipd = ipd;
            input_data.pst_module_unique = &in_unique;
            ret = goodix_nadt_calc(p_input, &output_result);

            if (GX_ALGO_NADT_SUCCESS == ret)
            {
                /* Liveness detection algorithm results */
                gh_memcpy((void *)p_res, (void *)&output_result, sizeof(goodix_nadt_result));

                if (output_result.nadt_out_flag)
                {
                    DEBUG_LOG("[NADT] %s, conf:%d\r\n",
                              g_nadt_result[p_res->nadt_out],
                              p_res->nadt_confi);
                }
            }
            else
            {
                ERROR_LOG("[%s] nadt error! error code = 0x%x\r\n", __FUNCTION__, ret);
            }
        }
        break;
#endif
        default:
        break;
    }
#endif

    return GH_ALGO_OK;
}


uint32_t gh_algo_adapter_config(gh_func_algo_adapter_t *p_adapter, uint16_t addr, uint16_t val)
{
    DEBUG_LOG("[%s] 0x%x:0x%x\r\n", __FUNCTION__, addr, val);

    if (GH_NULL_PTR == p_adapter)
    {
        return GH_ADAPTER_NULL;
    }

#if (1 == GH_ALGO_EN)
    gh_func_fix_idx_e id = GH_FUNC_FIX_IDX_ADT;
    gh_func_algo_param_t *p_param;
    uint16_t offset = 0;
#endif

    switch (addr)
    {
        /* hr fs config */
#if (GH_ALGO_HR_EN)
        case GH_ALGO_HR_FS_ADDR:
        {
            p_param = (gh_func_algo_param_t *)p_adapter->algo_param[GH_FUNC_FIX_IDX_HR].p_param;
            p_param->fs = val;
        }
        return GH_ADAPTER_OK;
#endif

        /* hrv fs config */
#if (GH_ALGO_HRV_EN)
        case GH_ALGO_HRV_FS_ADDR:
        {
            p_param = (gh_func_algo_param_t *)p_adapter->algo_param[GH_FUNC_FIX_IDX_HRV].p_param;
            p_param->fs = val;
        }
        return GH_ADAPTER_OK;
#endif

        /* spo2 fs config */
#if (GH_ALGO_SPO2_EN)
        case GH_ALGO_SPO2_FS_ADDR:
        {
            p_param = (gh_func_algo_param_t *)p_adapter->algo_param[GH_FUNC_FIX_IDX_SPO2].p_param;
            p_param->fs = val;
        }
        return GH_ADAPTER_OK;
#endif

        /* gnadt fs config */
#if (GH_ALGO_GNADT_EN)
        case GH_ALGO_GNADT_FS_ADDR:
        {
            p_param = (gh_func_algo_param_t *)p_adapter->algo_param[GH_FUNC_FIX_IDX_GNADT].p_param;
            p_param->fs = val;
        }
        return GH_ADAPTER_OK;
#endif

        /* irnadt fs config */
#if (GH_ALGO_IRNADT_EN)
        case GH_ALGO_IRNADT_FS_ADDR:
        {
            p_param = (gh_func_algo_param_t *)p_adapter->algo_param[GH_FUNC_FIX_IDX_IRNADT].p_param;
            p_param->fs = val;
        }
        return GH_ADAPTER_OK;
#endif
        default:
        {
            /* others are algo channel config */
            if (GH_ALGO_HR_CHL_NUM_ADDR <= addr && GH_ALGO_HR_RED_CH23_ADDR >= addr)
            {
#if (GH_ALGO_HR_EN)
                id = GH_FUNC_FIX_IDX_HR;
                offset = addr - GH_ALGO_HR_BASE_ADDR;
                break;
#else
                return GH_ADAPTER_ADDR_ERR;
#endif
            }
            else if (GH_ALGO_HRV_CHL_NUM_ADDR <= addr && GH_ALGO_HRV_RED_CH23_ADDR >= addr)
            {
#if (GH_ALGO_HRV_EN)
                id = GH_FUNC_FIX_IDX_HRV;
                offset = addr - GH_ALGO_HRV_BASE_ADDR;
                break;
#else
                return GH_ADAPTER_ADDR_ERR;
#endif
            }
            else if (GH_ALGO_SPO2_CHL_NUM_ADDR <= addr && GH_ALGO_SPO2_RED_CH23_ADDR >= addr)
            {
#if (GH_ALGO_SPO2_EN)
                id = GH_FUNC_FIX_IDX_SPO2;
                offset = addr - GH_ALGO_SPO2_BASE_ADDR;
                break;
#else
                return GH_ADAPTER_ADDR_ERR;
#endif
            }
            else if (GH_ALGO_GNADT_CHL_NUM_ADDR <= addr && GH_ALGO_GNADT_RED_CH23_ADDR >= addr)
            {
#if (GH_ALGO_GNADT_EN)
                id = GH_FUNC_FIX_IDX_GNADT;
                offset = addr - GH_ALGO_GNADT_BASE_ADDR;
                break;
#else
                return GH_ADAPTER_ADDR_ERR;
#endif
            }
            else if (GH_ALGO_IRNADT_CHL_NUM_ADDR <= addr && GH_ALGO_IRNADT_RED_CH23_ADDR >= addr)
            {
#if (GH_ALGO_IRNADT_EN)
                id = GH_FUNC_FIX_IDX_IRNADT;
                offset = addr - GH_ALGO_IRNADT_BASE_ADDR;
                break;
#else
                return GH_ADAPTER_ADDR_ERR;
#endif
            }
            else
            {
                return GH_ADAPTER_ADDR_ERR;
            }
        }
    } // switch (addr)

#if (1 == GH_ALGO_EN)
    /* Only the lower 8 bits of the address are used to resolve the data */
    p_param = (gh_func_algo_param_t *)p_adapter->algo_param[id].p_param;

    switch (offset)
    {
        case GH_ALGO_CHL_NUM_ADDR:
        {
            p_param->chl_num = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_num:%d\r\n", g_function_name[id], p_param->chl_num);
        }
        break;

        case GH_ALGO_GREEN_CH01_ADDR:
        {
            p_param->chl_map[GH_ALGO_GREEN_CH0] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_green0:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_GREEN_CH0]);
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL2)
            p_param->chl_map[GH_ALGO_GREEN_CH1] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_green1:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_GREEN_CH1]);
#endif
        }
        break;

        case GH_ALGO_GREEN_CH23_ADDR:
        {
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL3)
            p_param->chl_map[GH_ALGO_GREEN_CH2] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_green2:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_GREEN_CH2]);
#endif
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL4)
            p_param->chl_map[GH_ALGO_GREEN_CH3] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_green3:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_GREEN_CH3]);
#endif
        }
        break;

        case GH_ALGO_IR_CH01_ADDR:
        {

            p_param->chl_map[GH_ALGO_IR_CH0] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_ir0:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_IR_CH0]);
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL2)
            p_param->chl_map[GH_ALGO_IR_CH1] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_ir1:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_IR_CH1]);
#endif
        }
        break;

        case GH_ALGO_IR_CH23_ADDR:
        {
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL3)
            p_param->chl_map[GH_ALGO_IR_CH2] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_ir2:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_IR_CH2]);
#endif
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL4)
            p_param->chl_map[GH_ALGO_IR_CH3] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_ir3:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_IR_CH3]);
#endif
        }
        break;

        case GH_ALGO_RED_CH01_ADDR:
        {
            p_param->chl_map[GH_ALGO_RED_CH0] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_red0:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_RED_CH0]);
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL2)
            p_param->chl_map[GH_ALGO_RED_CH1] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_red1:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_RED_CH1]);
#endif
        }
        break;

        case GH_ALGO_RED_CH23_ADDR:
        {
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL3)
            p_param->chl_map[GH_ALGO_RED_CH2] = val & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_red2:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_RED_CH2]);
#endif
#if (GH_ALGO_PPG_CHL_NUM >= GH_ALGO_PPG_CHL4)
            p_param->chl_map[GH_ALGO_RED_CH3] = (val >> GH_ALGO_CH_OFFSET) & GH_ALGO_8BIT_MASK;
            DEBUG_LOG("[%s] chl_red3:%d\r\n", g_function_name[id],
                      p_param->chl_map[GH_ALGO_RED_CH3]);
#endif
        }
        break;

        default:
        break;
    } // switch (offset)

    return GH_ADAPTER_OK;
#endif
}
#endif
