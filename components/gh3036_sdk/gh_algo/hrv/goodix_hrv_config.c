/**
  ****************************************************************************************
  * @file    goodix_hrv_config.c
  * @author  GHealth Driver Team
  * @brief   goodix HRV algorithm configuration
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
#ifndef Goodix_DSP_EXPORTS
#include "goodix_hrv.h"

/**
 * @brief HRV algorithm configuration
 */
const goodix_hrv_config g_gh_hrv_cfg =
{
    .need_ipl = 1,              // Determine whether to interpolate to 1KHz
    .fs = 100,                  // Sampling rate
    .acc_thr[0] = 200000,       // acc related threshold, default value: 20/10/3/3
    .acc_thr[1] = 100000,
    .acc_thr[2] = 30000,
    .acc_thr[3] = 30000,
};

const void *goodix_hrv_config_get_arr()
{
    return (const void *)&g_gh_hrv_cfg;
}

uint32_t goodix_hrv_config_get_size()
{
    return sizeof(goodix_hrv_config);
}

void goodix_hrv_config_get_version(int8_t* ver, uint8_t ver_len)
{
    uint32_t copy_num = ver_len < (strlen(HRV_INTERFACE_VERSION) + 1)
                        ? ver_len : (strlen(HRV_INTERFACE_VERSION) + 1);

    memcpy(ver, HRV_INTERFACE_VERSION, copy_num);
    ver[copy_num - 1] = '\0'; // If the length of NET_VERSION is greater than size, you need to add a terminator
}

#endif // Goodix_DSP_EXPORTS

