/**
 ****************************************************************************************
 *
 * @file    gh_mtss_evt_manager.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing multi-sensor event manager
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_MTSS_EVT_MANAGER_H__
#define __GH_MTSS_EVT_MANAGER_H__

#include <stdint.h>
#include "gh_data_common.h"
#include "gh_wear_detector.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief mutil sensor event offset
 */
typedef enum
{
    GH_MTSS_EVT_OFST_GHEALTH_ON     = 0,
    GH_MTSS_EVT_OFST_GHEALTH_OFF    = 1,
    GH_MTSS_EVT_OFST_MOVE           = 2,
    GH_MTSS_EVT_OFST_MOVE_TIME_OUT  = 3,
    GH_MTSS_EVT_OFST_MAX
} gh_mtss_evt_offset_e;

/**
 * @brief mutil sensor event id
 */
typedef enum
{
    GH_MTSS_EVT_ID_GHEALTH_ON       = 1 << GH_MTSS_EVT_OFST_GHEALTH_ON,
    GH_MTSS_EVT_ID_GHEALTH_OFF      = 1 << GH_MTSS_EVT_OFST_GHEALTH_OFF,
    GH_MTSS_EVT_ID_MOVE             = 1 << GH_MTSS_EVT_OFST_MOVE,
    GH_MTSS_EVT_ID_MOVE_TIME_OUT    = 1 << GH_MTSS_EVT_OFST_MOVE_TIME_OUT,
    GH_MTSS_EVT_ID_MAX
} gh_mtss_evt_id_e;

/**
 * @brief move det module error code
 */
typedef enum
{
    GH_MTSS_EVT_MNG_OK              = 0,
    GH_MTSS_EVT_MNG_PTR_NULL        = 1,
    GH_MTSS_EVT_MNG_INIT_ERR        = 2
} gh_mtss_evt_mng_ret_e;

/**
 * @brief multi-sensor event manager control option
 */
typedef enum
{
    GH_MTSS_CTRL_DIS                = 0,
    GH_MTSS_CTRL_EN                 = 1
} gh_mtss_ctrl_e;

/**
 * @brief multi-sensor event node struct
 */
typedef struct
{
    uint64_t timestamp;
    uint8_t next;
} gh_mtss_evt_node_t;

/**
 * @brief multi-sensor event manager struct
 */
typedef struct gh_mtss_evt_manager_struct gh_mtss_evt_manager_t;
typedef void (*gh_mtss_new_evt_hook_t)(gh_mtss_evt_id_e event_id,
                                           uint64_t timestamp);
struct gh_mtss_evt_manager_struct
{
    uint8_t init_flag;
    uint8_t head;
    gh_mtss_ctrl_e state;
    gh_mtss_new_evt_hook_t new_evt_hook;
    gh_mtss_evt_node_t evt_linked_list[GH_MTSS_EVT_OFST_MAX];
};

/// @brief multi-sensor event manager initialization
/// @param this multi-sensor event manager module
/// @param evt_hook hook which will be called when new event is pushed
/// @return error code
uint32_t gh_mtss_evt_manager_init(gh_mtss_evt_manager_t *this,
                                  gh_mtss_new_evt_hook_t evt_hook);

/// @brief multi-sensor event manager deinit
/// @param this multi-sensor event manager module
/// @return error code
uint32_t gh_mtss_evt_manager_deinit(gh_mtss_evt_manager_t *this);

/// @brief multi-sensor event manager control
/// @param this multi-sensor event manager module
/// @param option control option
/// @return error code
uint32_t gh_mtss_evt_manager_ctrl(gh_mtss_evt_manager_t *this,
                                  gh_mtss_ctrl_e option);

/// @brief Get multi-sensor event manager state
/// @param this multi-sensor event manager module
/// @param state multi-sensor state
/// @return error code
uint32_t gh_mtss_evt_manager_state_get(gh_mtss_evt_manager_t *this,
                                       gh_mtss_ctrl_e *state);

/// @brief multi-sensor event manager event push
/// @param this multi-sensor event manager module
/// @param event_id event id
/// @param timestamp timestamp of this event
/// @return error code
uint32_t gh_mtss_evt_manager_evt_push(gh_mtss_evt_manager_t *this,
                                      gh_mtss_evt_id_e event_id,
                                      uint64_t timestamp);

/// @brief multi-sensor new event user hook, code in this function can be modify by user
/// @param this multi-sensor event manager module
/// @param event_id event id
/// @param timestamp timestamp of this event
/// @return error code
uint32_t gh_mtss_new_evt_user_hook(gh_mtss_evt_manager_t *this,
                                   gh_mtss_evt_id_e event_id,
                                   uint64_t timestamp);

/// @brief check est_new_event is new than est_old_event or not
/// @param this multi-sensor event manager module
/// @param est_new_event new event we hope for
/// @param est_old_event old event we hope for
/// @return 0: no or there is no valid new_event   1: yes or there is only valid new event
uint8_t gh_mtss_evt_is_new_chk(gh_mtss_evt_manager_t *this,
                               gh_mtss_evt_id_e est_new_event,
                               gh_mtss_evt_id_e est_old_event);

/// @brief check event combination is in this time window or not
/// @param this multi-sensor event manager module
/// @param event_comb event combination
/// @param window   time window (unit: ms)
/// @param cur_time_need_in_win  current timestamp is include or not  (0: is include  1: is NOT include)
/// @return 0: no  1: yes
uint8_t gh_mtss_evt_in_wnd_chk(gh_mtss_evt_manager_t *this,
                               uint32_t event_comb,
                               uint32_t window,
                               uint8_t cur_time_need_in_win);

#ifdef __cplusplus
}
#endif

#endif  // __GH_MTSS_EVT_MANAGER_H__

