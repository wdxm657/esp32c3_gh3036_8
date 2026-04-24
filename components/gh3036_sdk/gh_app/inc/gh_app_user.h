/**
 ****************************************************************************************
 *
 * @file    gh_app_user.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of app user functions.
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

/** @addtogroup GH APP USER
  * @{
  */

/** @defgroup API
  * @brief APP USER API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_APP_USER_H__
#define __GH_APP_USER_H__

#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_data_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn uint32_t gh_demo_int_process(void)
 * @brief Process interrupt event, call hal services interface
 * @param[in] None
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_demo_int_process(void);

/**
 * @fn uint32_t gh_demo_config_ctrl(gh_hal_config_ctrl_t *p_config_ctrl)
 * @brief Send configuration
 * @param[in] p_config_ctrl: pointer of config ctrl
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_demo_config_ctrl(gh_hal_config_ctrl_t *p_config_ctrl);

/**
 * @fn uint32_t gh_demo_sample_ctrl(gh_hal_sample_ctrl_t *p_sample_ctrl)
 * @brief Ctrl sampling
 * @param[in] p_sample_ctrl: pointer of sample ctrl
 * @param[out] None
 * @return void
 *
 * @note None
*/
extern uint32_t gh_demo_sample_ctrl(gh_hal_sample_ctrl_t *p_sample_ctrl);

/**
 * @fn uint32_t gh_demo_data_publish(gh_func_frame_t *p_frame)
 * @brief Get data after fusion or algorithm or both
 * @param[in] p_frame: pointer of frame
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_demo_data_publish(gh_func_frame_t *p_frame);

#if (GH_MTSS_EN)
/**
 * @fn uint32_t gh_demo_mtss_event_publish(gh_action_event_e evt, uint64_t timestamp)
 * @brief Publish action event
 * @param[in] evt: refer to gh_action_event_e
 * @param[in] timestamp: time stamp
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_demo_mtss_event_publish(gh_action_event_e evt, uint64_t timestamp);
#else

/**
 * @fn uint32_t gh_demo_action_event_publish(gh_action_event_e evt, uint64_t timestamp)
 * @brief Publish action event
 * @param[in] evt: refer to gh_action_event_e
 * @param[in] timestamp: time stamp
 * @param[out] None
 * @return error code
 *
 * @note None
 */

extern uint32_t gh_demo_action_event_publish(gh_action_event_e evt, uint64_t timestamp);
#endif

/**
 * @fn uint64_t gh_demo_app_mips_ts_get(void)
 * @brief Get platform timestamp for app
 * @param[in] None
 * @param[in] None
 * @param[out] None
 * @return timestamp
 *
 * @note None
 */
extern uint64_t gh_demo_app_mips_ts_get(void);

/**
 * @fn uint32_t gh_move_det_timer_start(void)
 * @brief Start timer for move detection
 * @param[in] None
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_move_det_timer_start(void);

/**
 * @fn uint32_t gh_move_det_timer_stop(void)
 * @brief Stop timer for move detection
 * @param[in] None
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_move_det_timer_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __GH_APP_USER_H__ */

/** @} */

/** @} */


