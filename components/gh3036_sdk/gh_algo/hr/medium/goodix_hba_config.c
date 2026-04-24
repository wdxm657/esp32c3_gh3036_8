/**
  ****************************************************************************************
  * @file    goodix_hba_config.c
  * @author  GHealth Driver Team
  * @brief   goodix HR algorithm configuration
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
#include "goodix_hba.h"

/**
 * @brief hba configuration
 */
const goodix_hba_config g_gh_hr_cfg =
{
    .mode = HBA_TEST_DYNAMIC,
    .fs = 25,                           // Original sampling
    .valid_ch_num = 4,                  // Number of valid channels
    .hba_earliest_output_time = 9,
    .hba_latest_output_time = 20,

    .raw_ppg_scale = 202,
};

const void *goodix_hba_config_get_arr(void)
{
    return (const void *)&g_gh_hr_cfg;
}

uint32_t goodix_hba_config_get_size(void)
{
    return sizeof(goodix_hba_config);
}

void goodix_hba_config_get_version(int8_t *ver, uint8_t ver_len)
{
    uint32_t copy_num = ver_len < (strlen(HBA_INTERFACE_VERSION) + 1)
                        ? ver_len : (strlen(HBA_INTERFACE_VERSION) + 1);

    memcpy(ver, HBA_INTERFACE_VERSION, copy_num);
    ver[copy_num - 1] = '\0';  // If the length of NET_VERSION is greater than size, you need to add a terminator
}

#endif //Goodix_DSP_EXPORTS

