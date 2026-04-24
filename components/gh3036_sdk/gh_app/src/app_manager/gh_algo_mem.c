/**
  ****************************************************************************************
  * @file    gh_algo_mem.c
  * @author  GHealth Driver Team
  * @brief   gh algorithm memory
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
#include "gh_hal_log.h"
#include "gh_global_config.h"
#include "gh_data_common.h"
#include "gh_algo_adapter_common.h"
#include "gh_algo_mem.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_ALGO_LOG_EN
#define DEBUG_LOG(...)                      GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                    GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                      GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
#if (1 == GH_USE_DYNAMIC_ALGO_MEM)
uint8_t *gh_algo_mem_free(void *p_mem_addr)
{
    if (GH_NULL_PTR == p_mem_addr)
    {
        return GH_NULL_PTR;
    }

    // add customer malloc function
    // return malloc(p_mem_addr);

    return GH_NULL_PTR;
}

uint8_t *gh_algo_mem_malloc(uint32_t mem_size)
{
    DEBUG_LOG("[%s] %d\r\n", __FUNCTION__, mem_size);

    // add customer free function
    // return free(p_mem_addr);

    return GH_NULL_PTR;
}
#endif

void Gh3x2xPoolIsNotEnough(void)
{
    ERROR_LOG("[%s] error!\r\n", __FUNCTION__);

    return;
}
