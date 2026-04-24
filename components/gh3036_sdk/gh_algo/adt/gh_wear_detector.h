/**
 ****************************************************************************************
 *
 * @file    gh_wear_detector.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing wear detector module
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
#ifndef __GH_WEAR_DETECTOR_H__
#define __GH_WEAR_DETECTOR_H__

#include <stdint.h>
#include "gh_data_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief wear det module error code
 */
typedef enum
{
    GH_WEAR_DET_OK                  = 0,
    GH_WEAR_DET_PTR_NULL            = 1,
    GH_WEAR_DET_INIT_ERR            = 2,
    GH_WEAR_DET_RET_END
} gh_wear_det_ret_e;

/**
 * @brief wear event offset
 */
typedef enum
{
    GH_WEAR_EVT_OFST_GHEALTH_ON     = 0,
    GH_WEAR_EVT_OFST_GHEALTH_OFF    = 1,
    GH_WEAR_EVT_OFST_MAX
} gh_wear_evt_offset_e;

/**
 * @brief wear event id
 */
typedef enum
{
    GH_WEAR_EVT_ID_GHEALTH_ON       = 1 << GH_WEAR_EVT_OFST_GHEALTH_ON,
    GH_WEAR_EVT_ID_GHEALTH_OFF      = 1 << GH_WEAR_EVT_OFST_GHEALTH_OFF,
    GH_WEAR_EVT_ID_MAX
} gh_wear_evt_id_e;

/**
 * @brief ghealth wear detect state
 */
typedef enum
{
    GH_GHEALTH_WEAR_DET_ON          = 0, // wear on detect
    GH_GHEALTH_WEAR_DET_OFF         = 1, // wear off detect
} gh_ghealth_det_state_e;

/**
 * @brief ghealth wear detect result
 */
typedef struct
{
    uint8_t update;
    uint32_t evt_id;
    uint32_t det_state;
    uint32_t ctr;
} gh_adt_result_t;

/**
 * @brief wear detector struct
 */
typedef struct
{
    uint16_t ctr_sum;   // uint: 0.1nA/mA  or 100pA/mA
    uint8_t  mean_cnt;  //count for mean calculation
    gh_ghealth_det_state_e det_state;
} gh_wear_detector_t;

/// @brief ghealth wear detector init
/// @param this ghealth wear detector module
/// @return refer to gh_wear_det_ret_e
gh_wear_det_ret_e gh_wear_detector_init(gh_wear_detector_t *this);

/// @brief ghealth wear detector deinit
/// @param this ghealth wear detector module
/// @return refer to gh_wear_det_ret_e
gh_wear_det_ret_e gh_wear_detector_deinit(gh_wear_detector_t *this);

/// @brief ghealth wear detector process
/// @param this ghealth wear detector module
/// @param frame frame info for algo input and algo output
/// @param p_res result info for algo output
/// @return refer to gh_wear_det_ret_e
gh_wear_det_ret_e gh_wear_detector_pro(gh_wear_detector_t *this,
                                       gh_func_frame_t *p_frame,
                                       uint8_t index,
                                       gh_adt_result_t *p_res);

#ifdef __cplusplus
}
#endif

#endif  // __GH_WEAR_DETECTOR_H__

