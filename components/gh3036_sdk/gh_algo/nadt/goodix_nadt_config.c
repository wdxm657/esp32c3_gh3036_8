/**
  ****************************************************************************************
  * @file    goodix_nadt_config.c
  * @author  GHealth Driver Team
  * @brief   goodix NADT algorithm configuration
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
#include "goodix_nadt.h"

/**
 * @brief NADT algorithm configuration
 */
const goodix_nadt_config g_gh_nadt_cfg =
{
    .sample_rate = 25,                 // sampling rate
    .unwear_detect_level = 3,          // wearing level

    .check_timeout_thr = 1200,         // live detection timeout
    .unwear_check_cnt_thr = 4,         // fall-off confirmation time
    .wear_check_cnt_thr = 2,           // live confirmation time

    // ADT judgment
    .adt_wear_on_thr = 8949996,        // ADT wear on threshold
    .adt_wear_off_thr = 8793588,       // ADT wear off threshold

    // motion judgment
    .acc_std_thr = 25,                 // ACC standard deviation

    // live detection
    .live_detect_enable = 1,           // live detection enable
    .raw_pv_len_thr = 300,             // high harmonic threshold
    .heart_beat_thr_low = 50,          // heart rate lower limit
    .heart_beat_thr_high = 140,        // heart rate upper limit
    .dif_rr_inter_thr = 8,             // heart rate change threshold
    .wear_acf_score_thr = 50,          // autocorrelation score threshold
    .base_line_pos_ratio_thr = 30,     // baseline monotonicity rate threshold
    .base_line_diff_thr = 15,          // baseline flatness threshold
    .sig_scale_thr = 1000,

    // high frequency background light detection
    .bg_jump_detect_enable = 1,        // background light mutation detection enable
    .jump_bg_diff_thr = 100,           // background light change threshold
    .bg_peak_valley_num_thr = 12,      // background light peak valley number threshold

    // background light period detection
    .bg_period_detect_enable = 1,      // background light period detection enable
    .period_bg_diff_thr = 10,          // Background light change threshold
    .bg_period_thr = 10,               // Background light period threshold
    .bg_period_diff_thr = 5            // Background light period difference threshold
};

const void *goodix_nadt_config_get_arr(void)
{
    return (const void *)&g_gh_nadt_cfg;
}

uint32_t goodix_nadt_config_get_size(void)
{
    return sizeof(goodix_nadt_config);
}

void goodix_nadt_config_get_version(int8_t *ver, uint8_t ver_len)
{
    uint32_t copy_num = ver_len < (strlen(NADT_INTERFACE_VERSION) + 1)
                        ? ver_len : (strlen(NADT_INTERFACE_VERSION) + 1);

    memcpy(ver, NADT_INTERFACE_VERSION, copy_num);
    ver[copy_num - 1] = '\0'; // If the length of NET_VERSION is greater than size, you need to add a terminator
}
