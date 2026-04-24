/**
 ****************************************************************************************
 *
 * @file    gh_app.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of public api.
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

/** @addtogroup GH API
  * @{
  */

/** @defgroup API
  * @brief Public API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_APP_H__
#define __GH_APP_H__

#include <stdint.h>
#include "gh_global_config.h"
#include "gh_hal_log.h"
#include "gh_hal_service.h"
#include "gh_data_common.h"
#include "gh_app_common.h"
#include "gh_public_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    GH_FUNC_START = 0,
    GH_FUNC_STOP = 1,
} gh_app_func_op_e;

typedef enum
{
    GH_APP_FUNC_OK = 0,
    GH_APP_FUNC_NULL = 1,
    GH_APP_FUNC_OP_FAIL = 2,
    GH_APP_FUNC_MISMATCH = 3,
} gh_app_func_ret_e;

/**
  * @brief Global config.
  */
typedef struct
{
    gh_demo_data_publish_t data_publish_func;
    gh_demo_sample_ctrl_t sample_ctrl_func;
    gh_demo_config_ctrl_t config_ctrl_func;
    gh_demo_action_evt_publish_t evt_publish_func;
    gh_demo_time_stamp_get_t ts_get_func;
} gh_global_config_t;

/**
 * @brief SDK goodix health data set
 * @param p_data: register address
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_ghealth_data_set(gh_data_t *p_data);

/**
 * @brief SDK data set
 * @param p_data: register address
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_gsensor_data_set(gh_gsensor_ts_and_data_t *p_data);

/**
 * @brief SDK data set
 * @param p_data: register address
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_gsensor_data_sync_set(gh_gsensor_ts_and_data_t *p_data, uint16_t size);

/**
 * @brief Get SDK data publish funcion
 * @return gh_demo_data_publish_t
 *
 * @note None
 */
extern gh_demo_data_publish_t gh_demo_data_publish_func_get(void);

/**
 * @brief Get SDK sample ctrl funcion
 * @return gh_demo_sample_ctrl_t
 *
 * @note None
 */
extern gh_demo_sample_ctrl_t gh_demo_sample_ctrl_func_get(void);

/**
 * @brief Get SDK config ctrl funcion
 * @return gh_demo_config_ctrl_t
 *
 * @note None
 */
extern gh_demo_config_ctrl_t gh_demo_config_ctrl_func_get(void);

/**
 * @brief Get SDK event publish function
 * @return gh_demo_config_ctrl_t
 *
 * @note None
 */
extern gh_demo_action_evt_publish_t gh_demo_evt_publish_func_get(void);

/**
 * @brief Get SDK timestamp function
 * @return gh_demo_time_stamp_get_t
 *
 * @note None
 */
extern gh_demo_time_stamp_get_t gh_demo_ts_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __GH_APP_H__ */

/** @} */

/** @} */


