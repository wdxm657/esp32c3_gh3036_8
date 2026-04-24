/**
 ****************************************************************************************
 *
 * @file    gh_algo_hr.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing algorithm HR function.
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

/** @addtogroup GH ALGO HR
  * @{
  */

/** @defgroup API
  * @brief HR ALGO API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_ALGO_HR_H__
#define __GH_ALGO_HR_H__

#include <stdint.h>
#include "gh_algo_adapter_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief adt algo hr init
 *
 * @param p_param: algo param
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_algo_hr_init(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
 * @brief adt algo hr deinit
 *
 * @param p_param: algo param
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_algo_hr_deinit(gh_func_fix_idx_e id, gh_func_algo_param_t *p_param);

/**
 * @brief adt algo hr process
 *
 * @param p_frame: function frame
 * @param p_param: algo param
 * @return refer to gh_algo_ret_e
 *
 * @note None
 */
extern gh_algo_ret_e gh_algo_hr_exe(gh_func_frame_t *p_frame, gh_func_algo_param_t *p_param);

#ifdef __cplusplus
}
#endif

#endif /* __GH_ALGO_HR_H__ */

/** @} */

/** @} */

