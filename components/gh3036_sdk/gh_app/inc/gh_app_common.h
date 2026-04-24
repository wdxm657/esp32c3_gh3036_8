/**
 ****************************************************************************************
 *
 * @file    gh_app_common.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of app common struct.
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
#ifndef __GH_APP_COMMON_H__
#define __GH_APP_COMMON_H__

#include <stdint.h>
#include "gh_hal_log.h"
#include "gh_hal_service.h"
#include "gh_data_common.h"
#include "gh_public_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief sample ctrl interface.
 */
typedef uint32_t (*gh_demo_sample_ctrl_t)(gh_hal_sample_ctrl_t *p_ctrl);

/**
 * @brief config ctrl interface.
 */
typedef uint32_t (*gh_demo_config_ctrl_t)(gh_hal_config_ctrl_t *p_ctrl);

/**
 * @brief fifo data get interface.
 */
typedef uint32_t (*gh_demo_data_publish_t)(gh_func_frame_t *p_frame);

/**
 * @brief action event publish
 */
typedef uint32_t (*gh_demo_action_evt_publish_t)(gh_action_event_e evt, uint64_t timestamp);

/**
 * @brief timestamp get
 */
typedef uint64_t (*gh_demo_time_stamp_get_t)(void);

/**
 * @brief module id list.
 */
typedef enum
{
    GH_APP_ID            = 0,
    GH_APP_MANAGER_ID    = 1,
    GH_APP_FUSION_ID     = 2,
    GH_APP_MOVE_DET_ID   = 3,
    GH_APP_MTSS_ID       = 4,
    GH_APP_MIPS_ID       = 5,
} gh_app_id_list_e;

/**
 * @brief application interface
 */
typedef struct
{
    gh_demo_data_publish_t data_publish;
    gh_demo_sample_ctrl_t sample_ctrl;
    gh_demo_config_ctrl_t config_ctrl;
    gh_demo_action_evt_publish_t evt_publish;
    gh_demo_time_stamp_get_t ts_get;
} gh_app_inf_t;

#ifdef __cplusplus
}
#endif

#endif /* __GH_APP_COMMON_H__ */

/** @} */

/** @} */


