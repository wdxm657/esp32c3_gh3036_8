/**
 ****************************************************************************************
 *
 * @file    gh_app_mips.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing application mips function.
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

/** @addtogroup GH APP MIPS
  * @{
  */

/** @defgroup API
  * @brief APP MIPS API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_APP_MIPS_H__
#define __GH_APP_MIPS_H__

#include <stdint.h>
#include "gh_global_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if (1 == GH_APP_MIPS_STA_EN)

typedef enum
{
    GH_MIPS_ALGO_ADT = 0,
    GH_MIPS_ALGO_HR = 1,
    GH_MIPS_ALGO_SPO2 = 2,
    GH_MIPS_ALGO_HRV = 3,
    GH_MIPS_ALGO_GNADT = 4,
    GH_MIPS_ALGO_IRNADT = 5,
    GH_MIPS_FUSION = 6,
    GH_MIPS_ID_MAX
} gh_app_mips_id_e;

/**
 * @brief mips timestamp get interface.
 */
typedef uint64_t (*gh_app_mips_ts_t)(void);

/**
 * @brief MIPS return value enum.
 */
typedef enum
{
    GH_MIPS_OK                          = 0,
    GH_MIPS_PTR_NULL                    = 1,
} gh_app_mips_ret_e;

typedef enum
{
    GH_MIPS_TS_US = 1,
    GH_MIPS_TS_MS = 1000,
    GH_MIPS_TS_S = 1000000,
} gh_app_mips_ts_unit_e;

typedef struct
{
    uint64_t ts_start;
    uint64_t ts_stop;
    uint32_t ts_cost_total;
    uint16_t stat_cnt;
    uint16_t stat_num;
    uint32_t peak_mips;
    uint32_t average_mips;
} gh_app_mips_ob_t;

typedef struct
{
    uint32_t mcu_freq_mhz;
    uint32_t ts_unit_us;
    gh_app_mips_ts_t ts_get_cb;
    gh_app_mips_ob_t mips_op[GH_MIPS_ID_MAX];
} gh_app_mips_t;

/**
 * @fn gh_app_mips_t gh_app_mips_inst_get(void)
 * @brief Get mips module
 * @param[in] None
 * @param[out] None
 * @return pointer of mips module
 *
 * @note None
 */
extern gh_app_mips_t *gh_app_mips_inst_get(void);

/**
 * @fn uint32_t gh_app_mips_freq_set(gh_app_mips_t *this, uint32_t id, uint16_t hz)
 * @brief Set mips module function frequency
 * @param[in] this, mips instance
 * @param[in] id, function id
 * @param[in] hz, function frequency
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_app_mips_freq_set(gh_app_mips_t *this, uint32_t id, uint16_t hz);

/**
 * @fn uint32_t gh_app_mips_cal_init(gh_app_mips_t *this, uint32_t mcu_freq_mhz,
                                     uint32_t unit, gh_app_mips_ts_t ts_get_cb)
 * @brief Init mips module
 * @param[in] this, mips instance
 * @param[in] mcu_freq_mhz, mcu main frequency
 * @param[in] unit, refer to gh_app_mips_ts_unit_e
 * @param[in] ts_get_cb, API for obtaining timestamp
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_app_mips_cal_init(gh_app_mips_t *this, uint32_t mcu_freq_mhz,
                                     uint32_t unit, gh_app_mips_ts_t ts_get_cb);

/**
 * @fn uint32_t gh_app_mips_algo_start(gh_app_mips_t *this)
 * @brief Start recording timestamp
 * @param[in] this, mips instance
 * @param[in] id, refer to gh_app_mips_id_e
 * @param[out] None
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_app_mips_algo_start(gh_app_mips_t *this, uint32_t id);

/**
 * @fn uint32_t gh_app_mips_algo_stop(gh_app_mips_t *this, uint32_t *p_mips)
 * @brief End recording timestamps and calculating mips
 * @param[in] this, mips instance
 * @param[in] id, refer to gh_app_mips_id_e
 * @param[out] p_mips, Calculated mips
 * @return error code
 *
 * @note None
 */
extern uint32_t gh_app_mips_algo_stop(gh_app_mips_t *this, uint32_t id);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __GH_APP_MIPS_H__ */

/** @} */

/** @} */


