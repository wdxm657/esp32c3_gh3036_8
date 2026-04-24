/**
  ****************************************************************************************
  * @file    gh_data_sync_fusion.h
  * @author  GHealth Driver Team
  * @brief   ghealth data frame data package and gsensor sync
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

#ifndef __GH_DATA_SYNC_FUSION_H__
#define __GH_DATA_SYNC_FUSION_H__

#include <stdint.h>
#include "gh_global_config.h"
#include "gh_app_common.h"
#include "gh_data_common.h"
#include "gh_hal_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_CHAN_BITS_NUN            (3)

/**
 * @brief GH fusion return code
 */
typedef enum
{
    GH_SYNC_FUSION_OK                          = 0,
    GH_SYNC_FUSION_PTR_NULL                    = 1,
    GH_SYNC_FUSION_INIT_ERR                    = 2,
    GH_SYNC_FUSION_NO_CHANNEL                  = 3,
    GH_SYNC_FUSION_NO_FUNC_MATCH               = 4,
    GH_SYNC_FUSION_STEP_ERR                    = 5,
} gh_sync_fusion_ret_e;

/**
 * @brief GH frame sync publish function
 */
typedef uint32_t (*gh_frame_sync_publish_t)(void *parent_node, gh_func_frame_t *frame);

/**
 * @brief GH data sync collector struct
 */
typedef struct
{
    gh_func_frame_t *p_frame;
    uint8_t         channel_bits[GH_CHAN_BITS_NUN];//record the channel collected
    uint16_t        frame_cnt;// current frame num in this fifo buffer
    uint16_t        frame_num;// frame num in this fifo buffer
} gh_data_sync_collector_t;

/**
 * @brief GH fusion data struct
 */
typedef struct
{
    gh_data_sync_collector_t     *p_sync_collector_list;//collector list, one collector for one function
    uint16_t                     sync_collector_num;
    gh_gsensor_data_t            *p_gsensor_data;
    uint16_t                     gsensor_data_num;
    uint16_t                     gsensor_data_max_num;
    gh_frame_sync_publish_t      frame_sync_publish;
    uint8_t                      init_flag;
    void                         *parent_node;
} gh_data_sync_fusion_t;

/**
 * @fn uint32_t gh_sync_fusion_init(gh_data_sync_fusion_t *p_sync_fusion,
                                  gh_data_sync_collector_t *p_sync_collector_list,
                                  uint16_t sync_collector_num,
                                  gh_frame_sync_publish_t frame_sync_publish,
                                  void *parent_node)
 * @brief Init sync fusion
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] p_sync_collector_list: pointer of sync collector list
 * @param[in] sync_collector_num: sync collector number
 * @param[in] p_gsensor_data: pointer of gsensor data
 * @param[in] gsensor_data_max_num: gsensor data max number
 * @param[in] frame_sync_publish: frame sync publish function
 * @param[in] parent_node: parent node
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_sync_fusion_init(gh_data_sync_fusion_t *p_sync_fusion,
                             gh_data_sync_collector_t *p_sync_collector_list,
                             uint16_t sync_collector_num,
                             gh_gsensor_data_t *p_gsensor_data,
                             uint16_t gsensor_data_max_num,
                             gh_frame_sync_publish_t frame_sync_publish,
                             void *parent_node);

/**
 * @fn uint32_t gh_sync_fusion_ctrl(gh_data_sync_fusion_t *p_sync_fusion, uint32_t function_group)
 * @brief Control sync fusion
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] function_group: function group
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_sync_fusion_ctrl(gh_data_sync_fusion_t *p_sync_fusion, uint32_t function_group);

/**
 * @fn uint32_t gh_sync_fusion_deinit(gh_data_sync_fusion_t *p_sync_fusion)
 * @brief Deinit sync fusion
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_sync_fusion_deinit(gh_data_sync_fusion_t *p_sync_fusion);

/**
 * @fn uint32_t gh_sync_fusion_ghealth_data_push(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data)
 * @brief Push ghealth data to sync fusion
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] p_gh_data: pointer of ghealth data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_sync_fusion_ghealth_data_push(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data);

/**
 * @fn uint32_t gh_sync_fusion_gsensor_data_push(gh_data_sync_fusion_t *p_sync_fusion,
 *                                               gh_gsensor_data_t *p_gsensor_data)
 * @brief Push gsensor data to sync fusion
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] p_gsensor_data: pointer of gsensor data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_sync_fusion_gsensor_data_push(gh_data_sync_fusion_t *p_sync_fusion, gh_gsensor_data_t *p_gsensor_data);
#ifdef __cplusplus
}
#endif

#endif // __GH_DATA_SYNC_FUSION_H__
