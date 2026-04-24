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
#include "gh_hal_config_process.h"
#include "gh_app_common.h"
#include "gh_app_manager.h"
#include "gh_app.h"
#include "gh_sample_ctrl.h"
#include "gh_config_process.h"

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
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

gh_config_ret_e gh_config_update(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_config_reg_t *p_cfg,
                                 uint16_t number)
{
    if (GH_NULL_PTR == p_cfg)
    {
        return GH_APP_CONFIG_NULL;
    }

    gh_hal_config_ctrl_t ctrl;

    /* step1: call reset function */
    gh_sample_ctrl_reset();

    // gh_data_fusion_reset();
    gh_app_manager_reset(p_manager);

    /* step2: call config function */
    gh_config_reg_t *p_reg = p_cfg;

    for (uint16_t idx = 0; idx < number; idx++)
    {
        gh_sample_ctrl_config(p_reg->addr, p_reg->value);
        gh_app_manager_config(p_manager, p_reg->addr, p_reg->value);
#if (1 == GH_ALGO_ADAPTER_EN)
        gh_algo_adapter_config(&p_manager->adapter_inst, p_reg->addr, p_reg->value);
#endif
        p_reg++;
    }

    if (p_inf->config_ctrl)
    {
        ctrl.op = GH_HAL_CONFIG_SET;
        ctrl.p_reg = (gh_reg_t *)p_cfg;
        ctrl.number = number;
        p_inf->config_ctrl(&ctrl);
    }

    /* step3: call update function */
    gh_app_manager_update(p_manager);

    return GH_APP_CONFIG_OK;
}

