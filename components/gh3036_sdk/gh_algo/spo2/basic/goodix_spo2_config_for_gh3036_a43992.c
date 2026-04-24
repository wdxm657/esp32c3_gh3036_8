/**
  ****************************************************************************************
  * @file    goodix_spo2_config.c
  * @author  GHealth Driver Team
  * @brief   goodix SPO2 algorithm configuration
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
#include "goodix_spo2.h"

/**
 * @brief SPO2 algorithm configuration
 */
const goodix_spo2_config g_gh_spo2_cfg =
{
    // raw configuration information
    .valid_chl_num = 1,           // Number of valid channels
    .raw_fs = 25,                 // Raw sampling rate
    // calibration parameters
    .cali_coef_a4 = 0,            // Calibration parameter 4th order term
    .cali_coef_a3 = 0,            // Calibration parameter 3rd order term
    .cali_coef_a2 = 0,            // Calibration parameter 2nd order term
    .cali_coef_a1 = -207909,      // Calibration parameter 1st order term
    .cali_coef_a0 = 1081401,      // Calibration parameter constant term
    // hb enable flag
    .hb_en_flg = 1,               // hb enable flag
    // wearing status
    .wear_mode = 0,               // Wearing status 0: wristband 1: finger 2: earphone
    // acc move threshold
    .acc_thr_max = 50,
    .acc_thr_min = 25,
    .acc_thr_scale = 3,
    .acc_thr_num = 3,
    .acc_thr_angle = 90,
    // CTR settings
    .ctr_en_flg = 0,
    .ctr_red_thr = 160,
    // signal quality evaluation related thresholds
    .ppg_jitter_thr = 35,
    .ppg_noise_thr = 15,
    .ppg_coeff_thr = 80,
    .quality_module_key = 253,
    // output strategy
    .low_spo2_thr = 95,
    .fast_out_time = 10,
    .slow_out_time = 30,
    .min_stable_time_high = 2,
    .min_stable_time_low = 6,
    .max_spo2_variation_high = 10,
    .max_spo2_variation_low = 5,
    .ouput_module_key = 3,
    .spo2_cali_left = 90,
    .spo2_cali_right = 95,
    .spo2_cali_factor = 0
};

const void *goodix_spo2_config_get_instance(void)
{
    return (const void *)&g_gh_spo2_cfg;
}

uint32_t goodix_spo2_config_get_size(void)
{
    return sizeof(goodix_spo2_config);
}

void goodix_spo2_config_get_version(int8_t *ver, uint8_t ver_len)
{
    uint32_t copy_num = ver_len < (strlen(SPO2_INTERFACE_VERSION) + 1)
                        ? ver_len : (strlen(SPO2_INTERFACE_VERSION) + 1);

    memcpy(ver, SPO2_INTERFACE_VERSION, copy_num);
    ver[copy_num - 1] = '\0'; // If the length of NET_VERSION is greater than size, add a terminator
}

#endif // Goodix_DSP_EXPORTS

