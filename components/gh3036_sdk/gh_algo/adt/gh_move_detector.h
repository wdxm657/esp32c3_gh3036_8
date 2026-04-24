/**
 ****************************************************************************************
 *
 * @file    gh_move_detector.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing move detector module
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
#ifndef __GH_MOVE_DETECTOR_H__
#define __GH_MOVE_DETECTOR_H__

#include <stdint.h>
#include "gh_data_common.h"
#include "gh_wear_detector.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_MOVE_DET_DEBUG_EN        (1)

/**
 * @brief move det module error code
 */
typedef enum
{
    GH_MOVE_DET_OK                  = 0,
    GH_MOVE_DET_PTR_NULL            = 1,
    GH_MOVE_DET_INIT_ERR            = 2,
    GH_MOVE_DET_TS_ERR              = 3,
    GH_MOVE_DET_RET_END
} gh_move_det_ret_e;

/**
 * @brief move event offset
 */
typedef enum
{
    GH_MOVE_EVT_OFST_MOVE           = 0,
    GH_MOVE_EVT_OFST_MOVE_TIME_OUT  = 1,
    GH_MOVE_EVT_OFST_MAX
} gh_move_evt_offset_e;

/**
 * @brief move event id
 */
typedef enum
{
    GH_MOVE_EVT_ID_MOVE             = 1 << GH_MOVE_EVT_OFST_MOVE,
    GH_MOVE_EVT_ID_MOVE_TIME_OUT    = 1 << GH_MOVE_EVT_OFST_MOVE_TIME_OUT,
    GH_MOVE_EVT_ID_MAX
} gh_move_evt_id_e;

/**
 * @brief move det module control option
 */
typedef enum
{
    GH_MOVE_DET_CTRL_DIS         = 0,
    GH_MOVE_DET_CTRL_EN          = 1
} gh_move_det_ctrl_e;

/**
 * @brief Move detector callback
 */
typedef void (*gh_wear_evt_hook_t)(gh_move_evt_id_e evt_id, uint64_t timestamp);

/**
 * @brief move detector struct
 */
typedef struct
{
    int16_t last_acc[GH_ACC_AXIS_NUM];
    uint64_t ideal_timestamp;
    gh_wear_evt_hook_t evt_hook;
    uint8_t first_point_got_flag;
    uint8_t move_cnt;
    uint8_t not_move_cnt;
    gh_move_det_ctrl_e state;
#if GH_MOVE_DET_DEBUG_EN
    uint8_t downsample_flag;
    uint32_t gs_diff;
    gh_move_evt_id_e output_evt_id;
    uint64_t output_evt_ts;
#endif
} gh_move_detector_t;

/// @brief move detector module init
/// @param this move detector module
/// @param evt_hook wear event hook function pointer
/// @return error code
uint32_t gh_move_detector_init(gh_move_detector_t *this, gh_wear_evt_hook_t evt_hook);

/// @brief move detector module deinit
/// @param this move detector module
/// @return error code
uint32_t gh_move_detector_deinit(gh_move_detector_t *this);

/// @brief move detector module control
/// @param this move detector module
/// @param ctrl_option control param
/// @return error code
uint32_t gh_move_detector_ctrl(gh_move_detector_t *this, gh_move_det_ctrl_e ctrl_option);

/// @brief move detector module process
/// @param this move detector module
/// @param gs_data gsensor data
/// @return error code
uint32_t gh_move_detector_pro(gh_move_detector_t *this, gh_gsensor_ts_and_data_t *gs_data);

#ifdef __cplusplus
}
#endif

#endif  // __GH_MOVE_DETECTOR_H__

