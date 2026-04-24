/**
  ****************************************************************************************
  * @file    gh_module_isr.h
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

#ifndef __GH_DATA_FUSION_H__
#define __GH_DATA_FUSION_H__

#include <stdint.h>
#include "gh_global_config.h"
#include "gh_app_common.h"
#include "gh_data_common.h"
#include "gh_hal_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef GH_FIFO_READ_MAX_NUM
#define GH_FIFO_READ_MAX_NUM              (256)
#endif

#ifndef GH_ASYNC_GSENSOR_QUEUE_DEPTH
#define GH_ASYNC_GSENSOR_QUEUE_DEPTH      ((uint32_t)(1.5 * 100))
#endif


#define GH_PPG_CFG_NUM                    (8)
#define GH_PPG_CHNL_NUM                   (GH_PPG_CFG_NUM * GH_PPG_RX_NUM)
#define GH_INVALID_CHNL_INDEX             (255)
#define GH_TIMESTAMPE_STEP_UNIT_RADIO     (256) // 1 LSB = (1 / 256) ms

/**
 * @brief GH fusion return code
 */
typedef enum
{
    GH_FUSION_OK                          = 0,
    GH_FUSION_PTR_NULL                    = 1,
    GH_FUSION_INIT_ERR                    = 2,
    GH_FUSION_LEN_ERR                     = 3,
    GH_FUSION_PARSE_ERR                   = 4,
    GH_FUSION_CONFIG_ERR                  = 5,
    GH_FUSION_RESET_ERR                   = 6,
    GH_FUSION_READ_DATA_ERR               = 7,
    GH_FUSION_RET_END
} gh_fusion_ret_e;

/**
 * @brief GH fusion collector gsensor sync return code
 */
typedef enum
{
    GH_COLLECTOR_GS_SYNC_OK               = 0,
    GH_COLLECTOR_GS_SYNC_FAIL
} gh_collector_gs_sync_ret_e;

/**
 * @brief GH fusion collector gsensor sync type code
 */
typedef enum
{
    GH_COLLECTOR_GS_SYNC_NORMAL           = 0,
    GH_COLLECTOR_GS_SYNC_FORCE
} gh_collector_gs_sync_type_e;

/**
 * @brief GH fusion sensor buffer message
 */
typedef enum
{
    GH_MSG_NEW_GHEALTH_DATA               = 0,
    GH_MSG_NEW_GSENOR_DATA,          //there is new gsensor data, but no old data dequeue
    GH_MSG_NEW_GSENOR_DATA_OLD_DEQ,  //there is new gsensor data and one oldest data has been dequeue
    GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR,
} gh_sensor_buffer_msg_e;

/**
 * @brief GH fusion ppg tx parameter struct
 */
typedef struct
{
    uint8_t led_drv[GH_LED_DRV_NUM];
} gh_ppg_tx_param_t;

/**
 * @brief GH fusion ppg rx parameter struct
 */
typedef struct
{
    uint32_t tia_gain           : 4;
    uint32_t dc_cancel_range    : 2;
    uint32_t bg_cancel_range    : 2;
    uint32_t dc_cancel_code     : 8;
    uint32_t reserved           : 16;
} __attribute__((packed)) gh_ppg_rx_param_t;

/**
 * @brief GH fusion ppg tx debug parameter struct
 */
typedef struct
{
    gh_ppg_tx_param_t tx_param;
    uint8_t debug_pack_update_flag;
} gh_ppg_tx_param_debug_t;

/**
 * @brief GH fusion ppg rx debug parameter struct
 */
typedef struct
{
    gh_ppg_rx_param_t rx_param;
    uint8_t debug_pack_update_flag;
} __attribute__((packed)) gh_ppg_rx_param_debug_t;

/**
 * @brief GH fusion channel physics information struct
 */
typedef struct
{
    gh_hal_data_channel_t  channel_map;
    uint32_t led_adj_flag         :1;
    uint32_t sa_flag              :1;
    uint32_t param_change_flag    :1;
    uint32_t dre_update           :1;
    uint32_t res                  :4;
    uint32_t adc_code             :24;
    uint32_t physical_value;
} __attribute__((packed)) gh_chnl_phy_info_t;

/**
 * @brief GH fusion ghealth buffer
 */
typedef struct
{
    gh_ppg_tx_param_debug_t ppg_tx_param[GH_PPG_CFG_NUM];
    gh_ppg_rx_param_debug_t ppg_rx_param[GH_PPG_CHNL_NUM];
    gh_chnl_phy_info_t phy_info[GH_FIFO_READ_MAX_NUM];
    uint64_t timestamp_begin;           // timestamp of this fifo data sample beginning postion
    uint64_t timestamp_end;             // timestamp of this fifo data sample end postion
    uint16_t sample_cnt;
    uint8_t led_drv_fs[GH_LED_DRV_NUM];
    uint8_t last_sample_is_recieved;    //if last sample of this fifo read period is recieved, this flag will be set
} gh_ghealth_buffer_t;

/**
 * @brief GH fusion cgsensor buffer
 */
typedef struct
{
    gh_gsensor_ts_and_data_t queue_data[GH_ASYNC_GSENSOR_QUEUE_DEPTH];
    uint16_t tail_index;
    uint16_t head_index;
    uint16_t len;
} gh_gsensor_buffer_t;

/**
 * @brief GH fusion sensor buffer
 */
struct gh_sensor_buffer_struct
{
    gh_ghealth_buffer_t ghealth_buffer;
#if  GH_GSENSOR_FUSION_EN
    gh_gsensor_buffer_t gsensor_buffer;
#endif
    gh_data_collector_t **collector_list;
};
typedef struct gh_sensor_buffer_struct gh_sensor_buffer_t;

/**
 * @brief GH fusion instance
 */
typedef struct
{
    gh_sensor_buffer_t sensor_buffer;
    gh_data_collector_t **collector_list;
} gh_fusion_t;

/// @brief fusion module init
/// @param this this module
/// @param collector_list list of data collector struct point
/// @param frame_list  list of frame struct point
/// @param frame_data_publish  function point of frame publish
/// @return error code
uint32_t gh_fusion_init(gh_fusion_t *this,
                        gh_data_collector_t **collector_list,
                        gh_func_frame_t **frame_list,
                        gh_frame_publish_t frame_data_publish,
                        void *parent_node);

/// @brief fusion module deinit
/// @param this this module
/// @return error code
uint32_t gh_fusion_deinit(gh_fusion_t *this);

/// @brief fusion module control function, it can open or close fusion corresponding function
/// @param this this fusion
/// @param function_group function group, each bit represents a function, 0 means close, 1 means open
/// @return error code
uint32_t gh_fusion_ctrl(gh_fusion_t *this, uint32_t function_group);

/// @brief push ghealth data into fusion module by this function
/// @param this fusion module
/// @param gh_data one sample ghealth data
/// @return error code
uint32_t gh_fusion_ghealth_data_push(gh_fusion_t *this, gh_data_t *gh_data);

#if  GH_GSENSOR_FUSION_EN
/// @brief push gsensor data into fusion module by this function
/// @param this fusion module
/// @param gsensor_data one sample gsenosr data
/// @return error code
uint32_t gh_fusion_gsensor_data_push(gh_fusion_t *this, gh_gsensor_ts_and_data_t *gsensor_data);
#endif

#ifdef __cplusplus
}
#endif

#endif // __GH_DATA_FUSION_H__
