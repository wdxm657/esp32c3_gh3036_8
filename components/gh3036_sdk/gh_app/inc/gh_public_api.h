/**
 ****************************************************************************************
 *
 * @file    gh_public_api.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of public api.
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
#ifndef __GH_PUBLIC_API_H__
#define __GH_PUBLIC_API_H__

#include "stdint.h"
#include "gh_data_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief GH public API return code.
 */
typedef enum
{
    GH_API_OK                                      = 0,
    GH_API_INIT_FAIL_ALREADY_INIT                  = 1,
    GH_API_DEINIT_FAIL_NEED_STOP                   = 2,
    GH_API_START_SAMPLING_PROCESS_FAIL             = 3,
    GH_API_START_SAMPLING_FAIL_ALREADY_START       = 4,
    GH_API_START_SAMPLING_FAIL_NEED_INIT           = 5,
    GH_API_START_SAMPLING_FAIL_CONFIG_MISMATCH     = 6,
    GH_API_START_SAMPLING_FAIL_FUNCTION_NULL       = 7,
    GH_API_STOP_SAMPLING_PROCESS_FAIL              = 8,
    GH_API_STOP_SAMPLING_FAIL_ALREADY_STOP         = 9,
    GH_API_STOP_SAMPLING_FAIL_NEED_START           = 10,
    GH_API_STOP_SAMPLING_FAIL_NEED_INIT            = 11,
    GH_API_NULL                                    = 12,
    GH_API_CONFIG_FAIL                             = 13,
    GH_API_GHEALTH_DATA_FAIL                       = 14,
    GH_API_GSENSOR_DATA_FAIL                       = 15,
    GH_API_ASSIST_CFG_FAIL                         = 16,
    GH_API_MOVE_DETECTOR_PRO_FAIL                  = 17,
    GH_API_NEED_INIT                               = 18,
    GH_API_GSENSOR_EN_FAIL                         = 19,
    GH_API_SAMPLING_CTRL_NEED_INIT                 = 20,
    GH_API_SAMPLING_CTRL_FAIL                      = 21,
} gh_public_ret_e;

/**
 * @brief GH action event enum.
 */
typedef enum
{
#if (GH_MTSS_EN)
    GH_ACTION_WEAR_ON                              = 0,
    GH_ACTION_WEAR_OFF                             = 1,
    GH_ACTION_MOVE                                 = 2,
    GH_ACTION_MOVE_TIME_OUT                        = 3,
#else
    GH_ACTION_ADT_WEAR_ON                          = 0,
    GH_ACTION_ADT_WEAR_OFF                         = 1,
    GH_ACTION_MOVE                                 = 2,
    GH_ACTION_MOVE_TIME_OUT                        = 3,
    GH_ACTION_NADT_WEAR_ON                         = 4,
    GH_ACTION_NADT_WEAR_OFF                        = 5,
#endif
    GH_ACTION_EVENT_MAX
} gh_action_event_e;

/**
 * @brief function enable struct
 */
typedef struct
{
    uint32_t function_adt_en : 1;
    uint32_t function_hr_en : 1;
    uint32_t function_spo2_en: 1;
    uint32_t function_hrv_en: 1;
    uint32_t function_gnadt_en : 1;
    uint32_t function_irnadt_en : 1;
    uint32_t function_test1_en : 1;
    uint32_t function_test2_en : 1;
    uint32_t function_slot_en : 1;
    uint32_t reserved : 23;
} __attribute__ ((packed)) gh_function_en_t;

/**
 * @brief function enable union struct
 */
typedef union
{
    uint32_t bits;
    gh_function_en_t func;
} gh_function_en_union_t;

/**
 * @brief assist enable struct
 */
typedef struct
{
    uint32_t assist_gsensor_en : 1;
    uint32_t reserved : 31;
} __attribute__ ((packed)) gh_assist_en_t;

/**
 * @brief assist enable union struct
 */
typedef union
{
    uint32_t bits;
    gh_assist_en_t assist;
} gh_assist_en_union_t;

/**
 * @brief config struct
 */
typedef  struct
{
    uint16_t addr;
    uint16_t value;
} gh_config_reg_t;

/**
 * @brief SDK module init
 *
 * @return gh_public_ret_e
 *
 * @note Init SDK module
 */
extern gh_public_ret_e gh_demo_init(void);

/**
 * @brief SDK module deinit
 *
 * @return gh_public_ret_e
 *
 * @note Deinit SDK module
 */
extern gh_public_ret_e gh_demo_deinit(void);

/**
 * @brief SDK module switches to control various functions
 * @param p_function_en: pointer of function, 0 means stop function, 1 means start function
 * @return gh_public_ret_e
 *
 * @note None
 */
extern gh_public_ret_e gh_demo_function_ctrl(gh_function_en_union_t *p_function_en);

/**
 * @brief SDK module get configured functions
 * @param p_function_en: pointer of function, 0 means no-configured, 1 means configured
 * @return gh_public_ret_e
 *
 * @note None
 */
extern gh_public_ret_e gh_demo_function_get(gh_function_en_union_t *p_function_en);

/**
 * @brief SDK config download
 * @param p_function_en: pointer of function
 * @param p_assist_en: pointer of assist status
 * @return gh_public_ret_e
 *
 * @note All functions require gsensor assistance by default.
 */
extern gh_public_ret_e gh_demo_assist_config(gh_function_en_union_t *p_function_en, gh_assist_en_union_t *p_assist_en);

/**
 * @brief SDK config download
 * @param p_cfg: configuration group
 * @param number: number of configuration group
 * @return gh_public_ret_e
 *
 * @note Write SDK/chip config.
 */
extern gh_public_ret_e gh_demo_config_write(gh_config_reg_t *p_cfg, uint16_t number);

/**
 * @brief SDK config upload
 * @param r_addr: register address
 * @param r_len: register number
 * @param p_r_val: register value buffer
 * @param p_r_len: the number of registers actually read
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_config_read(uint16_t r_addr, uint16_t r_len, uint16_t *p_r_val, uint16_t *p_r_len);

/**
 * @brief Get firmware version
 * @return version char pointer
 *
 * @note None
 */
extern int8_t *gh_firmware_version_get(void);

/**
 * @brief Get demo version
 * @return version char pointer
 *
 * @note None
 */
extern int8_t *gh_demo_version_get(void);

/**
 * @brief Get virtual register version
 * @return version char pointer
 *
 * @note None
 */
extern int8_t *gh_virtualreg_version_get(void);

/**
 * @brief SDK data set
 * @param p_data: register address
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_gsensor_data_set(gh_gsensor_ts_and_data_t *p_data);

/**
 * @brief SDK data set
 * @param p_data: register address
 * @return gh_public_ret_e
 *
 * @note Read SDK/chip config
 */
extern gh_public_ret_e gh_demo_gsensor_data_sync_set(gh_gsensor_ts_and_data_t *p_data, uint16_t size);

/**
 * @brief SDK enable motion detection
 * @param en: 0:disable, 1:enable
 * @return gh_public_ret_e
 *
 * @note Enable motion detection
 */
extern gh_public_ret_e gh_demo_move_det_enable(uint8_t en);

#ifdef __cplusplus
}
#endif

#endif /* __GH_PUBLIC_API_H__ */

/** @} */

/** @} */


