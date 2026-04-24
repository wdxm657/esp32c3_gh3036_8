/**
  ****************************************************************************************
  * @file    gh_hal_common.c
  * @author  GHealth Driver Team
  * @brief   gh hal std file
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
#include <stdint.h>
#include "gh_hal_config.h"
#include "gh_hal_common.h"
#if GH_HAL_COMLIB_LOG_EN
#include "gh_hal_log.h"
#endif


#if (GH_USE_HAL_COMMON_LIB == 0)
/*
 * DEFINES
 *****************************************************************************************
 */

#if GH_HAL_STD_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define ADC_FULLSCALE_MAX               (8388608)
#define VREF_HALF_MV                    (900)
#define ROUND                           (10)
#define HALFROUND                       (5)
#define KOM2OM                          (1000)
#define MV2UV                           (1000)
#define UA2PA                           (1000000)

#define MAX31BIT                        (0x7FFFFF)
#define MAX2BIT                         (0x3)
#define TWO                             (2)

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */



int32_t gh_hal_common_ipd_cal(int64_t rawdata, int32_t gain, int32_t idc_cancel)
{
    int64_t ipd_pa = 0;

    if (gain)
    {
        if (rawdata > 0)
        {
            ipd_pa = ((int64_t)(rawdata) * (int64_t)VREF_HALF_MV
                     * (int64_t)UA2PA / KOM2OM * ROUND / (int64_t)ADC_FULLSCALE_MAX
                     * MV2UV * KOM2OM / (int64_t)gain + HALFROUND) / ROUND;
        }
        else
        {
            ipd_pa = ((int64_t)(rawdata) * (int64_t)VREF_HALF_MV
                     * (int64_t)UA2PA / KOM2OM * ROUND / (int64_t)ADC_FULLSCALE_MAX
                     * MV2UV * KOM2OM / (int64_t)gain - HALFROUND) / ROUND;
        }
    }
    DEBUG_LOG("[%s] rawdata= %lld, gain = %d, idc_cancel = %d, ipd(no cancel) = %d \r\n",
              __FUNCTION__, rawdata, gain,
              idc_cancel,   (int32_t)ipd_pa);
    ipd_pa = ipd_pa + idc_cancel;

    DEBUG_LOG("[%s] rawdata= %lld, gain = %d, idc_cancel = %d, ipd(with cancel) = %d \r\n",
              __FUNCTION__, rawdata, gain,
              idc_cancel,   (int32_t)ipd_pa);
    return (int32_t)ipd_pa;
}

#endif

