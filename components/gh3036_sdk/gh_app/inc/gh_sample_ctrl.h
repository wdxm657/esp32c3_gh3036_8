/**
 ****************************************************************************************
 *
 * @file    gh_sample_ctrl.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing gh task sample ctrl.
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

/** @addtogroup GH task common function
  * @{
  */

/** @defgroup API
  * @brief Common API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __GH_SAMPLE_CTRL_H__
#define __GH_SAMPLE_CTRL_H__

#include <stdint.h>
#include "gh_public_api.h"
#include "gh_app_common.h"
#include "gh_app_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
  * @brief GH sample return code.
  */
typedef enum
{
    GH_CHECK_OK                     = 0,
    GH_CHECK_FAIL                   = 1,
    GH_CHECK_ENABLE_FLAG_NULL       = 2,
    GH_CHECK_MISMATCH               = 3,
    GH_CHECK_FUNC_DISABLE           = 4,
    GH_CHECK_PTR_NULL               = 5,
    GH_CHECK_OP_FAIL                = 6,
} gh_sample_ret_e;

typedef enum
{
    GH_SAMPLE_START = 0,
    GH_SAMPLE_STOP = 1,
} gh_sample_ctrl_op_e;

/**
 * @brief Reset sample ctrl
 *
 * @return gh_sample_ret_e
 *
 * @note None
 */
extern gh_sample_ret_e gh_sample_ctrl_reset(void);

/**
 * @brief Set registers config
 *
 * @param addr: register address
 * @param val: register value
 * @return gh_sample_ret_e
 *
 * @note None
 */
extern gh_sample_ret_e gh_sample_ctrl_config(uint16_t addr, uint16_t val);

/**
 * @brief Ctrl sampling
 *
 * @param p_manager: pointer of manager module
 * @param p_inf: pointer of app interface
 * @param p_function_en: pointer of functions
 * @return gh_sample_ret_e
 *
 * @note None
 */
extern gh_sample_ret_e gh_sample_ctrl(gh_app_manager_t *p_manager, gh_app_inf_t *p_inf,
                                      gh_function_en_union_t *p_function_en);

#ifdef __cplusplus
}
#endif

#endif /* __GH_SAMPLE_CTRL_H__ */

/** @} */

/** @} */
