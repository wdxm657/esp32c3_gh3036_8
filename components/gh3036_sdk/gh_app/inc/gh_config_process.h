/**
 ****************************************************************************************
 *
 * @file    gh_config_process.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing config function.
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

/** @addtogroup GH CONFIG
  * @{
  */

/** @defgroup API
  * @brief CONFIG API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_CONFIG_PROCESS_H__
#define __GH_CONFIG_PROCESS_H__

#include <stdint.h>
#include "gh_app_manager.h"
#include "gh_public_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
  * @brief GH sample return code.
  */
typedef enum
{
    GH_APP_CONFIG_OK                = 0,
    GH_APP_CONFIG_NULL              = 1,
} gh_config_ret_e;

/**
 * @fn gh_config_ret_e gh_config_update(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_config_reg_t *p_cfg,
 *                               uint16_t number);
 * @brief Update config, send configuration to each module
 *
 * @param[in] p_manager: pointer of manager module
 * @param[in] p_inf: pointer of app interface
 * @param[in] p_cfg: pointer of register and value
 * @param[in] number: number of configuartions
 * @param[out] None
 * @return gh_config_ret_e
 *
 * @note None
 */
gh_config_ret_e gh_config_update(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf, gh_config_reg_t *p_cfg,
                                 uint16_t number);

#ifdef __cplusplus
}
#endif

#endif /* __GH_CONFIG_PROCESS_H__ */

/** @} */

/** @} */

