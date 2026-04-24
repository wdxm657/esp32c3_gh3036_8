/**
 ****************************************************************************************
 *
 * @file    gh_algo_adapter.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing algorithm adapter function.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
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

/** @addtogroup GH ALGO ADAPTER
  * @{
  */

/** @defgroup API
  * @brief ALGO API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_ALGO_ADAPTER_H__
#define __GH_ALGO_ADAPTER_H__

#include <stdint.h>
#include "gh_global_config.h"
#if (1 == GH_ALGO_ADAPTER_EN)

#include "gh_algo_adapter_common.h"
#if (1 == GH_ALGO_ADT_EN)
#include "gh_algo_adt.h"
#endif
#if (1 == GH_ALGO_HR_EN)
#include "gh_algo_hr.h"
#endif
#if (1 == GH_ALGO_HRV_EN)
#include "gh_algo_hrv.h"
#endif
#if (1 == GH_ALGO_GNADT_EN) || (1 == GH_ALGO_IRNADT_EN)
#include "gh_algo_nadt.h"
#endif
#if (1 == GH_ALGO_SPO2_EN)
#include "gh_algo_spo2.h"
#endif
#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
#include "gh_algo_mem.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_ALGO_INF_VERSION_LEN_MAX       20

/**
  * @brief GH ALGO adapter return code.
  */
typedef enum
{
    GH_ADAPTER_OK                = 0,
    GH_ADAPTER_NULL              = 1,
    GH_ADAPTER_ADDR_ERR          = 2,
    GH_ADAPTER_MEM_ERR           = 3,
} gh_adapter_ret_e;

/**
  * @brief GH ALGO result pointer.
  */
typedef struct
{
    void *p_res;
} gh_func_algo_result_p_t;

/**
  * @brief GH ALGO parameter pointer.
  */
typedef struct
{
    void *p_param;
} gh_func_algo_param_p_t;

/**
  * @brief GH ALGO init function.
  */
typedef gh_algo_ret_e (*gh_algo_init_t)(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
  * @brief GH ALGO deinit function.
  */
typedef gh_algo_ret_e (*gh_algo_deinit_t)(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
  * @brief GH ALGO process function.
  */
typedef gh_algo_ret_e (*gh_algo_process_t)(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param);

/**
  * @brief GH ALGO interface.
  */
typedef struct
{
    gh_algo_init_t init;
    gh_algo_deinit_t deinit;
    gh_algo_process_t process;
} gh_func_algo_inf_t;

/**
  * @brief GH ALGO interface.
  */
typedef enum
{
    GH_ALGO_IDLE = 0,
    GH_ALGO_INITED = 1,
    GH_ALGO_DEINITED = 2,
} gh_algo_state_e;

/**
  * @brief GH ALGO adapter instance
  */
typedef struct
{
    gh_algo_state_e algo_state[GH_FUNC_FIX_IDX_ALGO_MAX];
    gh_func_algo_inf_t algo_inf[GH_FUNC_FIX_IDX_ALGO_MAX];
    gh_func_algo_param_p_t algo_param[GH_FUNC_FIX_IDX_ALGO_MAX];
    gh_func_algo_result_p_t algo_result[GH_FUNC_FIX_IDX_ALGO_MAX];

#if (1 == GH_ALGO_ADT_EN)
    gh_algo_adt_result_t adt_res;
    gh_func_algo_param_t adt_param;
#if (GH_USE_GOODIX_ADT_ALGO)
    gh_wear_detector_t adt_inst;
#endif
#endif
#if (1 == GH_ALGO_HR_EN)
#if GH_USE_GOODIX_HR_ALGO
    gh_algo_hr_result_t hr_res;
#endif
    gh_func_algo_param_t hr_param;
    gh_algo_hr_t hr_inst;
#endif
#if (1 == GH_ALGO_HRV_EN)
#if GH_USE_GOODIX_HRV_ALGO
    gh_algo_hrv_result_t hrv_res;
#endif
    gh_func_algo_param_t hrv_param;
    gh_algo_hrv_t hrv_inst;
#endif
#if (1 == GH_ALGO_GNADT_EN)
#if GH_USE_GOODIX_NADT_ALGO
    gh_algo_nadt_result_t gnadt_res;
#endif
    gh_func_algo_param_t gnadt_param;
#endif
#if (1 == GH_ALGO_IRNADT_EN)
#if GH_USE_GOODIX_NADT_ALGO
    gh_algo_nadt_result_t irnadt_res;
#endif
    gh_func_algo_param_t irnadt_param;
#endif
#if (1 == GH_ALGO_SPO2_EN)
#if GH_USE_GOODIX_SPO2_ALGO
    gh_algo_spo2_result_t spo2_res;
#endif
    gh_func_algo_param_t spo2_param;
#endif

#if (1 == GH_GOODIX_ALGO_ENABLE_MODE)
    gh_goodix_algo_inf_t goodix_algo_inf[GH_FUNC_FIX_IDX_ALGO_MAX];
#if (0 == GH_USE_DYNAMIC_ALGO_MEM)
    uint8_t algo_mem_buf[GH_ALGOS_MEM_SIZE_FINAL];
#else
    uint8_t *p_algo_mem_buf;
#endif
#endif
} gh_func_algo_adapter_t;

/**
 * @brief app adapter init
 *
 * @param p_adapter: adapter instance
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_adapter_init(gh_func_algo_adapter_t *p_adapter);

/**
 * @brief app adapter reset
 *
 * @param p_adapter: adapter instance
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_adapter_reset(gh_func_algo_adapter_t *p_adapter);

/**
 * @brief app adapter deinit
 *
 * @param p_adapter: adapter instance
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_adapter_deinit(gh_func_algo_adapter_t *p_adapter);

/**
 * @brief app algo init
 *
 * @param p_adapter: adapter instance
 * @param func_mask: function mask
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_init(gh_func_algo_adapter_t *p_adapter, uint32_t func_mask);

/**
 * @brief app algo deinit
 *
 * @param p_adapter: adapter instance
 * @param func_mask: function mask
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_deinit(gh_func_algo_adapter_t *p_adapter, uint32_t func_mask);

/**
 * @brief app algo process
 *
 * @param p_adapter: adapter instance
 * @param p_frame: function frame
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern gh_adapter_ret_e gh_algo_process(gh_func_algo_adapter_t *p_adapter, gh_func_frame_t *p_frame);

/**
 * @brief goodix algo init
 *
 * @param id: function id
 * @param p_param: algorithm parameter
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_goodix_algo_inf_init(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
 * @brief goodix algo deinit
 *
 * @param id: function id
 * @param p_param: algorithm parameter
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_goodix_algo_inf_deinit(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
 * @brief goodix algo process
 *
 * @param p_frame: data frame
 * @param p_param: algorithm parameter
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_goodix_algo_inf_process(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param);

/**
 * @brief goodix algo adapter config
 *
 * @param p_adapter: adapter instance
 * @param addr: config address
 * @param val: config value
 * @return refer to gh_adapter_ret_e
 *
 * @note None
 */
extern uint32_t gh_algo_adapter_config(gh_func_algo_adapter_t *p_adapter, uint16_t addr, uint16_t val);

#ifdef __cplusplus
}
#endif
#endif

#endif /* __GH_ALGO_ADAPTER_H__ */

/** @} */

/** @} */

