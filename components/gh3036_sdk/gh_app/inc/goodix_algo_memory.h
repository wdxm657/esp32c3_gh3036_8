/**
 ****************************************************************************************
 *
 * @file    goodix_algo_memory.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing goodix algorithm memory.
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

/** @addtogroup GH GOODIX ALGO MEMORY
  * @{
  */

/** @defgroup API
  * @brief MEMORY ALGO API.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __GOODIX_ALGO_MEMORY_H__
#define __GOODIX_ALGO_MEMORY_H__

#include <stdint.h>
#include "gh_global_config.h"
#if (GH_USE_GOODIX_HR_ALGO)
#include "goodix_mem_config_hr.h"
#endif
#if (GH_USE_GOODIX_HRV_ALGO)
#include "goodix_mem_config_hrv.h"
#endif
#if (GH_USE_GOODIX_SPO2_ALGO)
#include "goodix_mem_config_spo2.h"
#endif
#if (GH_USE_GOODIX_NADT_ALGO)
#include "goodix_mem_config_nadt.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if (GH_GOODIX_ALGO_ENABLE_MODE)

#if ((GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_BASIC) || (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_MEDIUM))
#if ((GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_PREMIUM) || (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_EXCLUSIVE))
#error "HR algo BASIC\MEDIUM version should adapt to SPO2 BASIC\MEDIUM version"
#endif
#endif

#if ((GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_PREMIUM) || (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_EXCLUSIVE))
#if ((GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_BASIC) || (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_MEDIUM))
#error "HR algo PREMIUM\EXCLUSIVE version should adapt to SPO2 PREMIUM\EXCLUSIVE version"
#endif
#endif

/* algorithm mem size in byte */
#if (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_BASIC)

#define GH_HR_ALGO_MEM_PEAK_1CHNL            (GOODIX_HR_ALGO_BAS_MEM_PEAK_1CHNL)
#define GH_HR_ALGO_MEM_PEAK_2CHNL            (GOODIX_HR_ALGO_BAS_MEM_PEAK_2CHNL)
#define GH_HR_ALGO_MEM_PEAK_3CHNL            (GOODIX_HR_ALGO_BAS_MEM_PEAK_3CHNL)
#define GH_HR_ALGO_MEM_PEAK_4CHNL            (GOODIX_HR_ALGO_BAS_MEM_PEAK_4CHNL)
#define GH_HR_ALGO_MEM_RES_1CHNL             (GOODIX_HR_ALGO_BAS_MEM_RESIDENT_1CHNL)
#define GH_HR_ALGO_MEM_RES_2CHNL             (GOODIX_HR_ALGO_BAS_MEM_RESIDENT_2CHNL)
#define GH_HR_ALGO_MEM_RES_3CHNL             (GOODIX_HR_ALGO_BAS_MEM_RESIDENT_3CHNL)
#define GH_HR_ALGO_MEM_RES_4CHNL             (GOODIX_HR_ALGO_BAS_MEM_RESIDENT_4CHNL)
#define GH_HR_ALGO_MEM_REDUNDANCY            (100)
#define GH_HR_ALGO_ROM                       (20212)
#define GH_HR_ALGO_STACK                     (GOODIX_HR_ALGO_BAS_STACK_1CHNL)
#define GH_HR_ALGO_MIPS_MS                   (2.61)

#elif (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_MEDIUM)

#define GH_HR_ALGO_MEM_PEAK_1CHNL            (GOODIX_HR_ALGO_MED_MEM_PEAK_1CHNL)
#define GH_HR_ALGO_MEM_PEAK_2CHNL            (GOODIX_HR_ALGO_MED_MEM_PEAK_2CHNL)
#define GH_HR_ALGO_MEM_PEAK_3CHNL            (GOODIX_HR_ALGO_MED_MEM_PEAK_3CHNL)
#define GH_HR_ALGO_MEM_PEAK_4CHNL            (GOODIX_HR_ALGO_MED_MEM_PEAK_4CHNL)
#define GH_HR_ALGO_MEM_RES_1CHNL             (GOODIX_HR_ALGO_MED_MEM_RESIDENT_1CHNL)
#define GH_HR_ALGO_MEM_RES_2CHNL             (GOODIX_HR_ALGO_MED_MEM_RESIDENT_2CHNL)
#define GH_HR_ALGO_MEM_RES_3CHNL             (GOODIX_HR_ALGO_MED_MEM_RESIDENT_3CHNL)
#define GH_HR_ALGO_MEM_RES_4CHNL             (GOODIX_HR_ALGO_MED_MEM_RESIDENT_4CHNL)
#define GH_HR_ALGO_MEM_REDUNDANCY            (100)
#define GH_HR_ALGO_ROM                       (25988)
#define GH_HR_ALGO_STACK                     (GOODIX_HR_ALGO_MED_STACK_1CHNL)
#define GH_HR_ALGO_MIPS_MS                   (3.25)

#elif (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_PREMIUM)

#define GH_HR_ALGO_MEM_PEAK_1CHNL            (GOODIX_HR_ALGO_PRE_MEM_PEAK_1CHNL)
#define GH_HR_ALGO_MEM_PEAK_2CHNL            (GOODIX_HR_ALGO_PRE_MEM_PEAK_2CHNL)
#define GH_HR_ALGO_MEM_PEAK_3CHNL            (GOODIX_HR_ALGO_PRE_MEM_PEAK_3CHNL)
#define GH_HR_ALGO_MEM_PEAK_4CHNL            (GOODIX_HR_ALGO_PRE_MEM_PEAK_4CHNL)
#define GH_HR_ALGO_MEM_RES_1CHNL             (GOODIX_HR_ALGO_PRE_MEM_RESIDENT_1CHNL)
#define GH_HR_ALGO_MEM_RES_2CHNL             (GOODIX_HR_ALGO_PRE_MEM_RESIDENT_2CHNL)
#define GH_HR_ALGO_MEM_RES_3CHNL             (GOODIX_HR_ALGO_PRE_MEM_RESIDENT_3CHNL)
#define GH_HR_ALGO_MEM_RES_4CHNL             (GOODIX_HR_ALGO_PRE_MEM_RESIDENT_4CHNL)
#define GH_HR_ALGO_MEM_REDUNDANCY            (100)
#define GH_HR_ALGO_ROM                       (76424)
#define GH_HR_ALGO_STACK                     (GOODIX_HR_ALGO_PRE_STACK_1CHNL)
#define GH_HR_ALGO_MIPS_MS                   (21)

#elif (GOODIX_HR_ALGO_VERISON == GOODIX_ALGO_EXCLUSIVE)

#define GH_HR_ALGO_MEM_PEAK_1CHNL            (GOODIX_HR_ALGO_EXC_MEM_PEAK_1CHNL)
#define GH_HR_ALGO_MEM_PEAK_2CHNL            (GOODIX_HR_ALGO_EXC_MEM_PEAK_2CHNL)
#define GH_HR_ALGO_MEM_PEAK_3CHNL            (GOODIX_HR_ALGO_EXC_MEM_PEAK_3CHNL)
#define GH_HR_ALGO_MEM_PEAK_4CHNL            (GOODIX_HR_ALGO_EXC_MEM_PEAK_4CHNL)
#define GH_HR_ALGO_MEM_RES_1CHNL             (GOODIX_HR_ALGO_EXC_MEM_RESIDENT_1CHNL)
#define GH_HR_ALGO_MEM_RES_2CHNL             (GOODIX_HR_ALGO_EXC_MEM_RESIDENT_2CHNL)
#define GH_HR_ALGO_MEM_RES_3CHNL             (GOODIX_HR_ALGO_EXC_MEM_RESIDENT_3CHNL)
#define GH_HR_ALGO_MEM_RES_4CHNL             (GOODIX_HR_ALGO_EXC_MEM_RESIDENT_4CHNL)
#define GH_HR_ALGO_MEM_REDUNDANCY            (100)
#define GH_HR_ALGO_ROM                       (80044)
#define GH_HR_ALGO_STACK                     (GOODIX_HR_ALGO_EXC_STACK_1CHNL)
#define GH_HR_ALGO_MIPS_MS                   (68)

#else
#error "please define GOODIX_HR_ALGO_VERISON in gh_global_config.h !"
#endif

#if (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_BASIC) || (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_MEDIUM)

#define GH_SPO2_ALGO_MEM_PEAK_1CHNL          (GOODIX_SPO2_ALGO_BAS_MEM_PEAK_1CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_2CHNL          (GOODIX_SPO2_ALGO_BAS_MEM_PEAK_2CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_3CHNL          (GOODIX_SPO2_ALGO_BAS_MEM_PEAK_3CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_4CHNL          (GOODIX_SPO2_ALGO_BAS_MEM_PEAK_4CHNL)
#define GH_SPO2_ALGO_MEM_RES_1CHNL           (GOODIX_SPO2_ALGO_BAS_MEM_RESIDENT_1CHNL)
#define GH_SPO2_ALGO_MEM_RES_2CHNL           (GOODIX_SPO2_ALGO_BAS_MEM_RESIDENT_2CHNL)
#define GH_SPO2_ALGO_MEM_RES_3CHNL           (GOODIX_SPO2_ALGO_BAS_MEM_RESIDENT_3CHNL)
#define GH_SPO2_ALGO_MEM_RES_4CHNL           (GOODIX_SPO2_ALGO_BAS_MEM_RESIDENT_4CHNL)
#define GH_SPO2_ALGO_MEM_REDUNDANCY          (100)
#define GH_SPO2_ALGO_ROM                     (12596)
#define GH_SPO2_ALGO_STACK                   (GOODIX_SPO2_ALGO_BAS_STACK_1CHNL)
#define GH_SPO2_ALGO_MIPS_MS                 (\)

#elif (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_PREMIUM) || (GOODIX_SPO2_ALGO_VERISON == GOODIX_ALGO_EXCLUSIVE)

#define GH_SPO2_ALGO_MEM_PEAK_1CHNL          (GOODIX_SPO2_ALGO_PRE_MEM_PEAK_1CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_2CHNL          (GOODIX_SPO2_ALGO_PRE_MEM_PEAK_2CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_3CHNL          (GOODIX_SPO2_ALGO_PRE_MEM_PEAK_3CHNL)
#define GH_SPO2_ALGO_MEM_PEAK_4CHNL          (GOODIX_SPO2_ALGO_PRE_MEM_PEAK_4CHNL)
#define GH_SPO2_ALGO_MEM_RES_1CHNL           (GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_1CHNL)
#define GH_SPO2_ALGO_MEM_RES_2CHNL           (GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_2CHNL)
#define GH_SPO2_ALGO_MEM_RES_3CHNL           (GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_3CHNL)
#define GH_SPO2_ALGO_MEM_RES_4CHNL           (GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_4CHNL)
#define GH_SPO2_ALGO_MEM_REDUNDANCY          (100)
#define GH_SPO2_ALGO_ROM                     (49428)
#define GH_SPO2_ALGO_STACK                   (GOODIX_SPO2_ALGO_PRE_STACK_1CHNL)
#define GH_SPO2_ALGO_MIPS_MS                 (26)

#else
#error "please define GOODIX_SPO2_ALGO_VERISON in gh_global_config.h !"
#endif

#if (GOODIX_ECG_ALGO_CONFIG == GOODIX_ECG_ALGO_500FS_QRS_ENABLE)

#define GH_ECG_ALGO_MEM_PEAK                 (38360)       /**< ECG algorithm peak memory size in byte */
#define GH_ECG_ALGO_MEM_RES                  (37176)       /**< ECG algorithm resident memory size in byte */
#define GH_ECG_ALGO_MEM_REDUNDANCY           (100)
#define GH_ECG_ALGO_ROM                      (33784)
#define GH_ECG_ALGO_STACK                    (5128)
#define GH_ECG_ALGO_MIPS_MS                  (216)

#elif (GOODIX_ECG_ALGO_CONFIG == GOODIX_ECG_ALGO_500FS_QRS_DISABLE)

#define GH_ECG_ALGO_MEM_PEAK                 (32024)       /**< ECG algorithm peak memory size in byte */
#define GH_ECG_ALGO_MEM_RES                  (30640)       /**< ECG algorithm resident memory size in byte */
#define GH_ECG_ALGO_MEM_REDUNDANCY           (100)
#define GH_ECG_ALGO_ROM                      (33784)
#define GH_ECG_ALGO_STACK                    (5128)
#define GH_ECG_ALGO_MIPS_MS                  (161)

#elif (GOODIX_ECG_ALGO_CONFIG == GOODIX_ECG_ALGO_250FS_QRS_ENABLE)

#define GH_ECG_ALGO_MEM_PEAK                 (23476)       /**< ECG algorithm peak memory size in byte */
#define GH_ECG_ALGO_MEM_RES                  (22132)       /**< ECG algorithm resident memory size in byte */
#define GH_ECG_ALGO_MEM_REDUNDANCY           (100)
#define GH_ECG_ALGO_ROM                      (33784)
#define GH_ECG_ALGO_STACK                    (5128)
#define GH_ECG_ALGO_MIPS_MS                  (151)

#elif (GOODIX_ECG_ALGO_CONFIG == GOODIX_ECG_ALGO_250FS_QRS_DISABLE)

#define GH_ECG_ALGO_MEM_PEAK                 (19464)       /**< ECG algorithm peak memory size in byte */
#define GH_ECG_ALGO_MEM_RES                  (18120)       /**< ECG algorithm resident memory size in byte */
#define GH_ECG_ALGO_MEM_REDUNDANCY           (100)
#define GH_ECG_ALGO_ROM                      (33784)
#define GH_ECG_ALGO_STACK                    (5128)
#define GH_ECG_ALGO_MIPS_MS                  (126)

#else
#error "please define GOODIX_ECG_ALGO_CONFIG in gh_global_config.h !"
#endif

#define GH_HRV_ALGO_MEM_PEAK                 (GOODIX_HRV_ALGO_PRE_MEM_PEAK)
#define GH_HRV_ALGO_MEM_RES                  (GOODIX_HRV_ALGO_PRE_MEM_RESIDENT)
#define GH_HRV_ALGO_MEM_REDUNDANCY           (100)
#define GH_HRV_ALGO_ROM                      (13320)
#define GH_HRV_ALGO_STACK                    (GOODIX_HRV_ALGO_PRE_STACK)
#define GH_HRV_ALGO_MIPS_MS                  (\)


#define GH_NADT_ALGO_MEM_PEAK                (GOODIX_NADT_ALGO_PRE_MEM_PEAK)
#define GH_NADT_ALGO_MEM_RES                 (GOODIX_NADT_ALGO_PRE_MEM_RESIDENT)
#define GH_NADT_ALGO_MEM_REDUNDANCY          (100)
#define GH_NADT_ALGO_ROM                     (17637)
#define GH_NADT_ALGO_STACK                   (GOODIX_NADT_ALGO_PRE_STACK)
#define GH_NADT_ALGO_MIPS_MS                 (\)


#define GH_HSM_ALGO_MEM_PEAK                 (944)         /**< HSM algorithm memory peak size in byte */
#define GH_HSM_ALGO_MEM_RES                  (944)         /**< HSM algorithm memory resident size in byte */
#define GH_HSM_ALGO_MEM_REDUNDANCY           (100)


#define GH_BT_ALGO_MEM_PEAK                  (1600)        /**< BT algorithm memory peak size in byte */
#define GH_BT_ALGO_MEM_RES                   (1500)        /**< BT algorithm memory resident size in byte */
#define GH_BT_ALGO_MEM_REDUNDANCY            (200)


#define GH_RESP_ALGO_MEM_PEAK                (5435)        /**< RESP algorithm memory peak size in byte */
#define GH_RESP_ALGO_MEM_RES                 (2363)        /**< RESP algorithm memory resident size in byte */
#define GH_RESP_ALGO_MEM_REDUNDANCY          (500)


#define GH_AF_ALGO_MEM_PEAK                  (3262)        /**< AF algorithm memory peak size in byte */
#define GH_AF_ALGO_MEM_RES                   (1102)        /**< AF algorithm memory resident size in byte */
#define GH_AF_ALGO_MEM_REDUNDANCY            (300)


/* algorithm mem config */
#if (GH_GOODIX_ALGO_ENABLE_MODE)

#if (GH_USE_GOODIX_HR_ALGO)
#if (GH_HR_ALGO_SUPPORT_CHNL_MAX <= 1)
#define GH_ALGO_HR_MEM_PEAK                  GH_HR_ALGO_MEM_PEAK_1CHNL
#define GH_ALGO_HR_MEM_RES                   GH_HR_ALGO_MEM_RES_1CHNL
#elif (GH_HR_ALGO_SUPPORT_CHNL_MAX == 2)
#define GH_ALGO_HR_MEM_PEAK                  GH_HR_ALGO_MEM_PEAK_2CHNL
#define GH_ALGO_HR_MEM_RES                   GH_HR_ALGO_MEM_RES_2CHNL
#elif (GH_HR_ALGO_SUPPORT_CHNL_MAX == 3)
#define GH_ALGO_HR_MEM_PEAK                  GH_HR_ALGO_MEM_PEAK_3CHNL
#define GH_ALGO_HR_MEM_RES                   GH_HR_ALGO_MEM_RES_3CHNL
#elif (GH_HR_ALGO_SUPPORT_CHNL_MAX >= 4)
#define GH_ALGO_HR_MEM_PEAK                  GH_HR_ALGO_MEM_PEAK_4CHNL
#define GH_ALGO_HR_MEM_RES                   GH_HR_ALGO_MEM_RES_4CHNL
#endif
#define GH_ALGO_HR_MEM_PEAK_NO_RES           (GH_ALGO_HR_MEM_PEAK - GH_ALGO_HR_MEM_RES)
#define GH_ALGO_HR_MEM_REDUNDANCY            (GH_HR_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_HR_MEM_RES                   (0)
#define GH_ALGO_HR_MEM_PEAK_NO_RES           (0)
#define GH_ALGO_HR_MEM_REDUNDANCY            (0)
#endif

#if (GH_USE_GOODIX_HSM_ALGO)
#define GH_ALGO_HSM_MEM_RES                  (GH_HSM_ALGO_MEM_RES)
#define GH_ALGO_HSM_MEM_PEAK_NO_RES          (GH_HSM_ALGO_MEM_PEAK - GH_HSM_ALGO_MEM_RES)
#define GH_ALGO_HSM_MEM_REDUNDANCY           (GH_HSM_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_HSM_MEM_RES                  (0)
#define GH_ALGO_HSM_MEM_PEAK_NO_RES          (0)
#define GH_ALGO_HSM_MEM_REDUNDANCY           (0)
#endif

#if (GH_USE_GOODIX_SPO2_ALGO)
#if (GH_SPO2_ALGO_SUPPORT_CHNL_MAX <= 1)
#define GH_ALGO_SPO2_MEM_PEAK                GH_SPO2_ALGO_MEM_PEAK_1CHNL
#define GH_ALGO_SPO2_MEM_RES                 GH_SPO2_ALGO_MEM_RES_1CHNL
#elif (GH_SPO2_ALGO_SUPPORT_CHNL_MAX == 2)
#define GH_ALGO_SPO2_MEM_PEAK                GH_SPO2_ALGO_MEM_PEAK_2CHNL
#define GH_ALGO_SPO2_MEM_RES                 GH_SPO2_ALGO_MEM_RES_2CHNL
#elif (GH_SPO2_ALGO_SUPPORT_CHNL_MAX == 3)
#define GH_ALGO_SPO2_MEM_PEAK                GH_SPO2_ALGO_MEM_PEAK_3CHNL
#define GH_ALGO_SPO2_MEM_RES                 GH_SPO2_ALGO_MEM_RES_3CHNL
#elif (GH_SPO2_ALGO_SUPPORT_CHNL_MAX >= 4)
#define GH_ALGO_SPO2_MEM_PEAK                GH_SPO2_ALGO_MEM_PEAK_4CHNL
#define GH_ALGO_SPO2_MEM_RES                 GH_SPO2_ALGO_MEM_RES_4CHNL
#endif
#define GH_ALGO_SPO2_MEM_PEAK_NO_RES         (GH_ALGO_SPO2_MEM_PEAK - GH_ALGO_SPO2_MEM_RES)
#define GH_ALGO_SPO2_MEM_REDUNDANCY          (GH_SPO2_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_SPO2_MEM_RES                 (0)
#define GH_ALGO_SPO2_MEM_PEAK_NO_RES         (0)
#define GH_ALGO_SPO2_MEM_REDUNDANCY          (0)

#endif

#if (GH_USE_GOODIX_ECG_ALGO)
#define GH_ALGO_ECG_MEM_RES                  (GH_ECG_ALGO_MEM_RES)
#define GH_ALGO_ECG_MEM_PEAK_NO_RES          (GH_ECG_ALGO_MEM_PEAK - GH_ECG_ALGO_MEM_RES)
#define GH_ALGO_ECG_MEM_REDUNDANCY           (GH_ECG_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_ECG_MEM_RES                  (0)
#define GH_ALGO_ECG_MEM_PEAK_NO_RES          (0)
#define GH_ALGO_ECG_MEM_REDUNDANCY           (0)
#endif

#if (GH_USE_GOODIX_HRV_ALGO)
#define GH_ALGO_HRV_MEM_RES                  (GH_HRV_ALGO_MEM_RES)
#define GH_ALGO_HRV_MEM_PEAK_NO_RES          (GH_HRV_ALGO_MEM_PEAK - GH_HRV_ALGO_MEM_RES)
#define GH_ALGO_HRV_MEM_REDUNDANCY           (GH_HRV_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_HRV_MEM_RES                  (0)
#define GH_ALGO_HRV_MEM_PEAK_NO_RES          (0)
#define GH_ALGO_HRV_MEM_REDUNDANCY           (0)
#endif

#if (GH_USE_GOODIX_BT_ALGO)
#define GH_ALGO_BT_MEM_RES                   (GH_BT_ALGO_MEM_RES)
#define GH_ALGO_BT_MEM_PEAK_NO_RES           (GH_BT_ALGO_MEM_PEAK - GH_BT_ALGO_MEM_RES)
#define GH_ALGO_BT_MEM_REDUNDANCY            (GH_BT_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_BT_MEM_RES                   (0)
#define GH_ALGO_BT_MEM_PEAK_NO_RES           (0)
#define GH_ALGO_BT_MEM_REDUNDANCY            (0)
#endif

#if (GH_USE_GOODIX_RESP_ALGO)
#define GH_ALGO_RESP_MEM_RES                 (GH_RESP_ALGO_MEM_RES)
#define GH_ALGO_RESP_MEM_PEAK_NO_RES         (GH_RESP_ALGO_MEM_PEAK - GH_RESP_ALGO_MEM_RES)
#define GH_ALGO_RESP_MEM_REDUNDANCY          (GH_RESP_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_RESP_MEM_RES                 (0)
#define GH_ALGO_RESP_MEM_PEAK_NO_RES         (0)
#define GH_ALGO_RESP_MEM_REDUNDANCY          (0)
#endif

#if (GH_USE_GOODIX_AF_ALGO)
#define GH_ALGO_AF_MEM_RES                   (GH_AF_ALGO_MEM_RES)
#define GH_ALGO_AF_MEM_PEAK_NO_RES           (GH_AF_ALGO_MEM_PEAK - GH_AF_ALGO_MEM_RES)
#define GH_ALGO_AF_MEM_REDUNDANCY            (GH_AF_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_AF_MEM_RES                   (0)
#define GH_ALGO_AF_MEM_PEAK_NO_RES           (0)
#define GH_ALGO_AF_MEM_REDUNDANCY            (0)
#endif

#if (GH_USE_GOODIX_NADT_ALGO)
#define GH_ALGO_NADT_MEM_RES                 (GH_NADT_ALGO_MEM_RES)
#define GH_ALGO_NADT_MEM_PEAK_NO_RES         (GH_NADT_ALGO_MEM_PEAK - GH_NADT_ALGO_MEM_RES)
#define GH_ALGO_NADT_MEM_REDUNDANCY          (GH_NADT_ALGO_MEM_REDUNDANCY)
#else
#define GH_ALGO_NADT_MEM_RES                 (0)
#define GH_ALGO_NADT_MEM_PEAK_NO_RES         (0)
#define GH_ALGO_NADT_MEM_REDUNDANCY          (0)
#endif
#endif

#define MAX2(a0, a1)                                    (((a0) > (a1)) ? (a0) : (a1))
#define ADD2(a0, a1)                                    ((a0) + (a1))
#define MAX4(a0, a1, a2, a3)                            (MAX2(MAX2(a0, a1), MAX2(a2, a3)))
#define MAX5(a0, a1, a2, a3, a4)                        (MAX2(MAX4(a0, a1, a2, a3), a4))
#define MAX9(a0, a1, a2, a3, a4, a5, a6, a7, a8)        (MAX2(MAX5(a0, a1, a2, a3, a4), MAX4(a5, a6, a7, a8))))

#define GH_ALGOS_MEM_RES(x)     (\
                                ((0 != (x & GH_FUNCTION_HR)) * GH_USE_GOODIX_HR_ALGO * GH_ALGO_HR_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_SPO2)) * GH_USE_GOODIX_SPO2_ALGO * GH_ALGO_SPO2_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_ECG)) * GH_USE_GOODIX_ECG_ALGO * GH_ALGO_ECG_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_HRV)) * GH_USE_GOODIX_HRV_ALGO * GH_ALGO_HRV_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_HSM)) * GH_USE_GOODIX_HSM_ALGO * GH_ALGO_HSM_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_BT)) * GH_USE_GOODIX_BT_ALGO * GH_ALGO_BT_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_RESP)) * GH_USE_GOODIX_RESP_ALGO * GH_ALGO_RESP_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_AF)) * GH_USE_GOODIX_AF_ALGO * GH_ALGO_AF_MEM_RES) + \
                                ((0 != (x & GH_FUNCTION_NADT)) * GH_USE_GOODIX_NADT_ALGO * GH_ALGO_NADT_MEM_RES))

#define GH_ALGOS_MEM_PEAK_NO_RES(x)    (MAX9(\
            ADD2(((0 != (x & GH_FUNCTION_HR)) * GH_USE_GOODIX_HR_ALGO * GH_ALGO_HR_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_HR)) * GH_USE_GOODIX_HR_ALGO * GH_ALGO_HR_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_SPO2)) * GH_USE_GOODIX_SPO2_ALGO* GH_ALGO_SPO2_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_SPO2)) * GH_USE_GOODIX_SPO2_ALGO* GH_ALGO_SPO2_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_ECG)) * GH_USE_GOODIX_ECG_ALGO* GH_ALGO_ECG_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_ECG)) * GH_USE_GOODIX_ECG_ALGO* GH_ALGO_ECG_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_HRV)) * GH_USE_GOODIX_HRV_ALGO* GH_ALGO_HRV_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_HRV)) * GH_USE_GOODIX_HRV_ALGO* GH_ALGO_HRV_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_HSM)) * GH_USE_GOODIX_HSM_ALGO* GH_ALGO_HSM_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_HSM)) * GH_USE_GOODIX_HSM_ALGO* GH_ALGO_HSM_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_BT)) * GH_USE_GOODIX_BT_ALGO* GH_ALGO_BT_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_BT)) * GH_USE_GOODIX_BT_ALGO* GH_ALGO_BT_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_RESP)) * GH_USE_GOODIX_RESP_ALGO* GH_ALGO_RESP_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_RESP)) * GH_USE_GOODIX_RESP_ALGO* GH_ALGO_RESP_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_AF)) * GH_USE_GOODIX_AF_ALGO* GH_ALGO_AF_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_AF)) * GH_USE_GOODIX_AF_ALGO* GH_ALGO_AF_MEM_REDUNDANCY)), \
            ADD2(((0 != (x & GH_FUNCTION_NADT)) * GH_USE_GOODIX_NADT_ALGO * GH_ALGO_NADT_MEM_PEAK_NO_RES), \
            ((0 != (x & GH_FUNCTION_NADT)) * GH_USE_GOODIX_NADT_ALGO * GH_ALGO_NADT_MEM_REDUNDANCY)))

#define GH_ALGO_MEM(x)     (GH_ALGOS_MEM_RES(x) + GH_ALGOS_MEM_PEAK_NO_RES(x))
#define GH_ALGOS_MEM    (\
                             MAX5(\
                                 GH_ALGO_MEM(GH_ALGO_FUNCTION_GROUP_0), \
                                 GH_ALGO_MEM(GH_ALGO_FUNCTION_GROUP_1), \
                                 GH_ALGO_MEM(GH_ALGO_FUNCTION_GROUP_2), \
                                 GH_ALGO_MEM(GH_ALGO_FUNCTION_GROUP_3), \
                                 GH_ALGO_MEM(GH_ALGO_FUNCTION_GROUP_4)) \
                         )

//final size
#define GH_ALGOS_MEM_SIZE_FINAL    (((uint32_t)((GH_ALGOS_MEM + 3) / 4)) * 4 + 4)

#endif

#ifdef __cplusplus
}
#endif

#endif /* __GOODIX_ALGO_MEMORY_H__ */

/** @} */

/** @} */

