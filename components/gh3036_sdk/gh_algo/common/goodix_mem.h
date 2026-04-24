/**
  ****************************************************************************************
  * @file    goodix_mem.h
  * @author  GHealth Driver Team
  * @brief   goodix algorithm memory interface
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

#ifndef __GOODIX_MEM_H__
#define __GOODIX_MEM_H__

#include <stdint.h>
#include "goodix_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief memory return error code.
 */
typedef enum
{
    MEMPOOL_IS_OK_E                 = 0,  /*< MEM POOL is ok >*/
    ERR_MEMPOOL_NOT_EXIST_E         = -1, /*< mempool not exist >*/
    ERR_PARAMS_MEMPOOL_SIZE_E       = -2, /*< Input parameter is invalid: mempool size is invalid >*/
    ERR_PARAMS_MALLOC_SIZE_E        = -3, /*< Input parameter is invalid: malloc size is invalid >*/
    ERR_MEMPOOL_NOT_ENOUGH_MEM_E    = -4, /*< Memory size does not meet the required memory >*/
    ERR_PARAMS_ADDR_WRONG_E         = -5, /*< Input parameter is invalid: free addr is invalid >*/
    ERR_OP_CHUNK_TREAD_E            = -6, /*< Occur memory tread >*/
    ERR_OP_CHUNK_LEAKY_E            = -7, /*< Occur memory leaky >*/
} goodix_mem_ret;

// The interface for insufficient memory only supports calls when memory is allocated,
// not when memory exceeds the bounds
#if !(_WIN32 || _WIN64)
extern void Gh3x2xPoolIsNotEnough(void);
#endif

/**
 * @brief Memory pool initialization operation
 * @param[in] mem_addr Memory address
 *            size Memory size
 * @param[out] None
 * @return Initialization status 0 < Operation success.
                                -1 < Input parameter is invalid
                                -2 < Memory size does not meet the minimum required memory
                                -3 < Memory manager has been initialized.
*/
DRVDLL_API int32_t goodix_mem_init(void* mem_addr, int32_t size);

/**
 * @brief Memory pool destruction operation
 * @param[in] None
 * @param[out] None
 * @return None
 */
DRVDLL_API void goodix_mem_deinit(void);

/**
 * @brief Memory request operation
 * @param[in] Requested memory size (bytes)
 * @param[out] None
 * @return The first address of the requested memory segment (do type conversion outside the function call)
 */
DRVDLL_API void* goodix_mem_malloc(int32_t size);

/**
 * @brief Release memory operation
 * @param[in] The first address of the memory segment to be released
 * @param[out] None
 * @return None
 */
DRVDLL_API void goodix_mem_free(void* mem_addr);

/**
 * @brief Get the current free memory capacity of the memory pool
 * @param[in] None
 * @param[out] None
 * @return The current free capacity of the memory pool (byte)
 */
DRVDLL_API uint32_t goodix_mem_get_free_size(void);

#ifdef __cplusplus
}
#endif

#endif // __GOODIX_MEM_H__

