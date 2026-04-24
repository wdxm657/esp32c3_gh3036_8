/**
  ****************************************************************************************
  * @file    gh_global_config.c
  * @author  GHealth Driver Team
  * @brief   gh application global configuration file
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
#include "gh_public_api.h"
#include "gh_app.h"
#include "gh_app_user.h"
#include "gh_global_config.h"

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


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gh_global_config_t g_global_config =
{
    .data_publish_func = gh_demo_data_publish,
    .sample_ctrl_func = gh_demo_sample_ctrl,
    .config_ctrl_func = gh_demo_config_ctrl,
#if (1 == GH_MTSS_EN)
    .evt_publish_func = gh_demo_mtss_event_publish,
#else
    .evt_publish_func = gh_demo_action_event_publish,
#endif
#if (1 == GH_APP_MIPS_STA_EN)
    .ts_get_func = gh_demo_app_mips_ts_get,
#else
    .ts_get_func = GH_NULL_PTR,
#endif
};


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

/**
 * @fn gh_demo_data_publish_t gh_demo_data_publish_func_get(void)
 * @brief Get customer data hook
 * @param[in] None
 * @param[out] None
 * @return gh_std_data_publish_t
 *
 * @note Get bsp function from global config
 */
gh_demo_data_publish_t gh_demo_data_publish_func_get(void)
{
    return g_global_config.data_publish_func;
}

/**
 * @fn gh_demo_sample_ctrl_t gh_demo_sample_ctrl_func_get(void)
 * @brief Get sample ctrl hook
 * @param[in] None
 * @param[out] None
 * @return gh_demo_sample_ctrl_t
 *
 * @note Get bsp function from global config
 */
gh_demo_sample_ctrl_t gh_demo_sample_ctrl_func_get(void)
{
    return g_global_config.sample_ctrl_func;
}

/**
 * @fn gh_demo_config_ctrl_t gh_demo_config_ctrl_func_get(void)
 * @brief Get config ctrl hook
 * @param[in] None
 * @param[out] None
 * @return gh_demo_config_ctrl_t
 *
 * @note Get bsp function from global config
 */
gh_demo_config_ctrl_t gh_demo_config_ctrl_func_get(void)
{
    return g_global_config.config_ctrl_func;
}

/**
 * @fn gh_demo_action_evt_publish_t gh_demo_evt_publish_func_get(void)
 * @brief Get action event publish function
 * @param[in] None
 * @param[out] None
 * @return gh_demo_action_evt_publish_t
 *
 * @note Get bsp function from global config
 */
gh_demo_action_evt_publish_t gh_demo_evt_publish_func_get(void)
{
    return g_global_config.evt_publish_func;
}

/**
 * @fn gh_demo_time_stamp_get_t gh_demo_ts_get(void)
 * @brief Get timestamp
 * @param[in] None
 * @param[out] None
 * @return gh_demo_action_evt_publish_t
 *
 * @note Get bsp function from global config
 */
gh_demo_time_stamp_get_t gh_demo_ts_get(void)
{
    return g_global_config.ts_get_func;
}

