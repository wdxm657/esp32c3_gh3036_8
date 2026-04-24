/**
  ****************************************************************************************
  * @file    gh_app_mips.c
  * @author  GHealth Driver Team
  * @brief   gh application mips user file
  ****************************************************************************************
  * @attention
  #####Copyright (c) 2024 GOODIX
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "gh_global_config.h"
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_app_common.h"
#include "gh_app_mips.h"

#if (1 == GH_APP_MIPS_STA_EN)
/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_MIPS_LOG_EN
#define DEBUG_LOG(...)                      GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                    GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                      GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    (GH_APP_MIPS_ID << 24 | (internal_err) << 8 | (interface_err))

#define GH_MIPS_US                          (1000000)
#define GH_MIPS_ACCURACY                    (1)

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static gh_app_mips_t g_mips_inst;

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
gh_app_mips_t *gh_app_mips_inst_get(void)
{
    return &g_mips_inst;
}

uint32_t gh_app_mips_freq_set(gh_app_mips_t *this, uint32_t id, uint16_t hz)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_PTR_NULL);
    }

    this->mips_op[id].stat_num = hz;

    return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_OK);
}

uint32_t gh_app_mips_cal_init(gh_app_mips_t *this, uint32_t mcu_freq_mhz,
                              uint32_t unit, gh_app_mips_ts_t ts_get_cb)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_PTR_NULL);
    }

    this->mcu_freq_mhz = mcu_freq_mhz;
    this->ts_unit_us = unit;
    this->ts_get_cb = ts_get_cb;
    gh_memset(this->mips_op, 0, sizeof(this->mips_op));

    return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_OK);
}

uint32_t gh_app_mips_algo_start(gh_app_mips_t *this, uint32_t id)
{
    if (this->ts_get_cb)
    {
        this->mips_op[id].ts_start = this->ts_get_cb();
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_OK);
}

uint32_t gh_app_mips_algo_stop(gh_app_mips_t *this, uint32_t id)
{
    if (this->ts_get_cb)
    {
        this->mips_op[id].ts_stop = this->ts_get_cb();
    }

    uint32_t diff = this->mips_op[id].ts_stop - this->mips_op[id].ts_start;
    uint32_t mips = diff * this->ts_unit_us * GH_MIPS_ACCURACY * this->mcu_freq_mhz / GH_MIPS_US;

//    DEBUG_LOG("[MIPS][id:%d] %lld -> %lld, diff %d\r\n", id,
//              this->mips_op[id].ts_start, this->mips_op[id].ts_stop, diff);

    if (this->mips_op[id].peak_mips < mips)
    {
        this->mips_op[id].peak_mips = mips;
    }

    this->mips_op[id].ts_cost_total = this->mips_op[id].ts_cost_total + diff;
    this->mips_op[id].stat_cnt++;
    if (this->mips_op[id].stat_cnt == this->mips_op[id].stat_num)
    {
        this->mips_op[id].average_mips = this->mips_op[id].ts_cost_total * this->ts_unit_us
                                         * GH_MIPS_ACCURACY * this->mcu_freq_mhz / GH_MIPS_US;
        DEBUG_LOG("[MIPS][id:%d] average:%d, peak:%d\r\n", id,
                  this->mips_op[id].average_mips, this->mips_op[id].peak_mips);

        this->mips_op[id].peak_mips = 0;
        this->mips_op[id].ts_cost_total = 0;
        this->mips_op[id].stat_cnt = 0;
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MIPS_OK);
}
#endif

