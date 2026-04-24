/**
 ****************************************************************************************
 *
 * @file    gh_app_manager.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing application manager.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_APP_MANAGER_H__
#define __GH_APP_MANAGER_H__

#include <stdint.h>
#include "gh_global_config.h"
#include "gh_data_common.h"
#include "gh_algo_adapter.h"
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
#include "gh_data_fusion.h"
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
#include "gh_data_sync_fusion.h"
#endif
#include "gh_public_api.h"
#include "gh_hal_service.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_ADT_RAWDATA_MAX                  (GH_ADT_CONFIG_MAX * GH_ADT_CONFIG_RX_MAX * GH_ADT_DATA_TYPE)
#define GH_HR_RAWDATA_MAX                   (GH_HR_CONFIG_MAX * GH_HR_CONFIG_RX_MAX * GH_HR_DATA_TYPE)
#define GH_SPO2_RAWDATA_MAX                 (GH_SPO2_CONFIG_MAX * GH_SPO2_CONFIG_RX_MAX * GH_SPO2_DATA_TYPE)
#define GH_HRV_RAWDATA_MAX                  (GH_HRV_CONFIG_MAX * GH_HRV_CONFIG_RX_MAX * GH_HRV_DATA_TYPE)
#define GH_NADT_RAWDATA_MAX                 (GH_NADT_CONFIG_MAX * GH_NADT_CONFIG_RX_MAX * GH_NADT_DATA_TYPE)
#define GH_TEST_RAWDATA_MAX                 (GH_TEST_CONFIG_MAX * GH_TEST_CONFIG_RX_MAX * GH_TEST_DATA_TYPE)
#define GH_PPG_SLOT_DATA_MAX                (GH_PPG_SLOT_CONFIG_MAX * GH_PPG_SLOT_CONFIG_RX_MAX * GH_PPG_SLOT_DATA_TYPE)
#define GH_CAP_SLOT_DATA_MAX                (GH_CAP_SLOT_CONFIG_MAX * GH_CAP_SLOT_CONFIG_RX_MAX * GH_CAP_SLOT_DATA_TYPE)

/**
 * @brief MANAGER return value enum.
 */
typedef enum
{
    GH_MANAGER_OK                          = 0,
    GH_MANAGER_PTR_NULL                    = 1,
    GH_MANAGER_ADDR_ERR                    = 2,
    GH_MANAGER_CFG_ERR                     = 3,
    GH_MANAGER_DATA_TYPE_ERR               = 4,
    GH_MANAGER_FUNC_EMPTY_ERR              = 5,
    GH_MANAGER_FUNC_DISABLE_ERR            = 6,
    GH_MANAGER_FUNC_OUT_RANGE_ERR          = 7,
    GH_MANAGER_RET_END
} gh_app_manager_ret_e;

/**
 * @brief MANAGER operation
 */
typedef enum
{
    GH_MANAGER_START = 0,
    GH_MANAGER_STOP = 1
} gh_app_manager_op;

/**
 * @brief GH function id index
 */
typedef enum
{
#if (GH_FUNC_ADT_EN)
    GH_FUNC_ID_ADT_IDX,
#endif

#if (GH_FUNC_HR_EN)
    GH_FUNC_ID_HR_IDX,
#endif

#if (GH_FUNC_SPO2_EN)
    GH_FUNC_ID_SPO2_IDX,
#endif

#if (GH_FUNC_HRV_EN)
    GH_FUNC_ID_HRV_IDX,
#endif

#if (GH_FUNC_GNADT_EN)
    GH_FUNC_ID_GNADT_IDX,
#endif

#if (GH_FUNC_IRNADT_EN)
    GH_FUNC_ID_IRNADT_IDX,
#endif

#if (GH_FUNC_TEST1_EN)
    GH_FUNC_ID_TEST1_IDX,
#endif

#if (GH_FUNC_TEST2_EN)
    GH_FUNC_ID_TEST2_IDX,
#endif

#if (GH_FUNC_SLOT_EN)
    GH_FUNC_ID_PPG_CFG0_IDX,
    GH_FUNC_ID_PPG_CFG1_IDX,
    GH_FUNC_ID_PPG_CFG2_IDX,
    GH_FUNC_ID_PPG_CFG3_IDX,
    GH_FUNC_ID_PPG_CFG4_IDX,
    GH_FUNC_ID_PPG_CFG5_IDX,
    GH_FUNC_ID_PPG_CFG6_IDX,
    GH_FUNC_ID_PPG_CFG7_IDX,
    GH_FUNC_ID_CAP_CFG_IDX,
#endif
    GH_FUNC_ID_MAX
} gh_func_index_e;

/**
 * @brief GH function id
 */
typedef enum
{
#if (GH_FUNC_ADT_EN)
    GH_FUNC_ADT = 1 << GH_FUNC_ID_ADT_IDX,
#endif

#if (GH_FUNC_HR_EN)
    GH_FUNC_HR = 1 << GH_FUNC_ID_HR_IDX,
#endif

#if (GH_FUNC_SPO2_EN)
    GH_FUNC_SPO2 = 1 << GH_FUNC_ID_SPO2_IDX,
#endif

#if (GH_FUNC_HRV_EN)
    GH_FUNC_HRV = 1 << GH_FUNC_ID_HRV_IDX,
#endif

#if (GH_FUNC_GNADT_EN)
    GH_FUNC_GNADT = 1 << GH_FUNC_ID_GNADT_IDX,
#endif

#if (GH_FUNC_IRNADT_EN)
    GH_FUNC_IRNADT = 1 << GH_FUNC_ID_IRNADT_IDX,
#endif

#if (GH_FUNC_TEST1_EN)
    GH_FUNC_TEST1 = 1 << GH_FUNC_ID_TEST1_IDX,
#endif

#if (GH_FUNC_TEST2_EN)
    GH_FUNC_TEST2 = 1 << GH_FUNC_ID_TEST2_IDX,
#endif

#if (GH_FUNC_SLOT_EN)
    GH_FUNC_PPG_CFG0 = 1 << GH_FUNC_ID_PPG_CFG0_IDX,
    GH_FUNC_PPG_CFG1 = 1 << GH_FUNC_ID_PPG_CFG1_IDX,
    GH_FUNC_PPG_CFG2 = 1 << GH_FUNC_ID_PPG_CFG2_IDX,
    GH_FUNC_PPG_CFG3 = 1 << GH_FUNC_ID_PPG_CFG3_IDX,
    GH_FUNC_PPG_CFG4 = 1 << GH_FUNC_ID_PPG_CFG4_IDX,
    GH_FUNC_PPG_CFG5 = 1 << GH_FUNC_ID_PPG_CFG5_IDX,
    GH_FUNC_PPG_CFG6 = 1 << GH_FUNC_ID_PPG_CFG6_IDX,
    GH_FUNC_PPG_CFG7 = 1 << GH_FUNC_ID_PPG_CFG7_IDX,
    GH_FUNC_CAP_CFG = 1 << GH_FUNC_ID_CAP_CFG_IDX,
#endif
} gh_func_id_e;

/**
 * @brief MANAGER channel map
 */
typedef struct
{
    uint16_t champ_cfg_rx0_l;
    uint16_t champ_cfg_rx1_l;
    uint16_t champ_cfg_rx0_data;
    uint16_t champ_cfg_rx1_data;
    uint8_t ready;
} gh_manager_chamap_t;

/**
 * @brief MANAGER slot enable map
 */
typedef struct
{
    uint8_t ppg_cfg[GH_CHIP_PPG_CONFIG_NUM];
    uint8_t cap_cfg[GH_CHIP_CAP_CONFIG_RX_NUM];
} __attribute__((packed)) gh_slot_enable_t;

/**
 * @brief MANAGER frame buffer
 */
typedef struct
{
#if (GH_FUNC_ADT_EN)
    gh_hal_data_channel_t adt_ch_map[GH_ADT_RAWDATA_MAX];
    gh_frame_data_t adt_data[GH_ADT_RAWDATA_MAX];
#endif
#if (GH_FUNC_HR_EN)
    gh_hal_data_channel_t hr_ch_map[GH_HR_RAWDATA_MAX];
    gh_frame_data_t hr_data[GH_HR_RAWDATA_MAX];
#endif
#if (GH_FUNC_SPO2_EN)
    gh_hal_data_channel_t spo2_ch_map[GH_SPO2_RAWDATA_MAX];
    gh_frame_data_t spo2_data[GH_SPO2_RAWDATA_MAX];
#endif
#if (GH_FUNC_HRV_EN)
    gh_hal_data_channel_t hrv_ch_map[GH_HRV_RAWDATA_MAX];
    gh_frame_data_t hrv_data[GH_HRV_RAWDATA_MAX];
#endif
#if (GH_FUNC_GNADT_EN)
    gh_hal_data_channel_t gnadt_ch_map[GH_NADT_RAWDATA_MAX];
    gh_frame_data_t gnadt_data[GH_NADT_RAWDATA_MAX];
#endif
#if (GH_FUNC_IRNADT_EN)
    gh_hal_data_channel_t irnadt_ch_map[GH_NADT_RAWDATA_MAX];
    gh_frame_data_t irnadt_data[GH_NADT_RAWDATA_MAX];
#endif
#if (GH_FUNC_TEST1_EN)
    gh_hal_data_channel_t test1_ch_map[GH_TEST_RAWDATA_MAX];
    gh_frame_data_t test1_data[GH_TEST_RAWDATA_MAX];
#endif
#if (GH_FUNC_TEST2_EN)
    gh_hal_data_channel_t test2_ch_map[GH_TEST_RAWDATA_MAX];
    gh_frame_data_t test2_data[GH_TEST_RAWDATA_MAX];
#endif
#if (GH_FUNC_SLOT_EN)
    gh_hal_data_channel_t ppg_ch_map[GH_CHIP_PPG_CONFIG_NUM][GH_PPG_SLOT_DATA_MAX];
    gh_frame_data_t ppg_data[GH_CHIP_PPG_CONFIG_NUM][GH_PPG_SLOT_DATA_MAX];
    gh_hal_data_channel_t cap_ch_map[GH_CHIP_CAP_CONFIG_NUM][GH_CAP_SLOT_DATA_MAX];
    gh_frame_data_t cap_data[GH_CHIP_CAP_CONFIG_NUM][GH_CAP_SLOT_DATA_MAX];
#endif
} gh_manager_frame_buffer_t;

/**
 * @brief MANAGER frame struct
 */
typedef struct
{
    gh_func_frame_t func_frame[GH_FUNC_ID_MAX];
    gh_func_frame_t *p_func_frame[GH_FUNC_FIX_IDX_MAX];
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    gh_data_collector_t data_collector[GH_FUNC_ID_MAX];
    gh_data_collector_t *p_data_collector[GH_FUNC_FIX_IDX_MAX];
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    gh_data_sync_collector_t sync_collector[GH_FUNC_ID_MAX];
#endif
} gh_manager_frame_t;

/**
 * @brief MANAGER data callback
 */
typedef void (*gh_data_cb_t)(gh_func_frame_t *p_frame);

/**
 * @brief MANAGER instance struct
 */
typedef struct
{
    void *p_parent_node;
    gh_manager_chamap_t chamap_parser;
    gh_manager_frame_buffer_t frame_buf;
    gh_manager_frame_t frame_inst;
#if (1 == GH_ALGO_ADAPTER_EN)
    gh_func_algo_adapter_t adapter_inst;
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    gh_fusion_t data_fusion;
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    gh_data_sync_fusion_t data_sync_fusion;
    gh_gsensor_data_t gsensor_data[GH_SYNC_GSENSOR_DATA_MAX];
#endif
    gh_data_cb_t data_cb;
    uint32_t func_cur;
} gh_app_manager_t;

/**
 * @brief app manager init
 *
 * @param p_manage: manager instance
 * @param data_cb: data callback
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_init(gh_app_manager_t *p_manage, gh_data_cb_t data_cb);

/**
 * @brief app manager deinit
 *
 * @param p_manage: manager instance
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_deinit(gh_app_manager_t *p_manage);

/**
 * @brief app manager udpate
 *
 * @param p_manage: manager instance
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_update(gh_app_manager_t *p_manage);

/**
 * @brief app manager reset
 *
 * @param p_manage: manager instance
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_reset(gh_app_manager_t *p_manage);

/**
 * @brief app manager slot map check
 *
 * @param p_manage: manager instance
 * @param p_slot: slot configuration
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_slot_map_check(gh_app_manager_t *p_func, gh_slot_enable_t *p_slot);

/**
 * @brief app manager config
 *
 * @param p_manage: manager instance
 * @param addr: config address
 * @param val: config value
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_config(gh_app_manager_t *p_manage, uint16_t addr, uint16_t val);

/**
 * @brief app manager function config
 *
 * @param p_manage: manager instance
 * @param fix_id: fixed function id
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_func_set(gh_app_manager_t *p_manage, uint32_t fix_id);

/**
 * @brief set ghealth data to app manager
 *
 * @param p_manage: manager instance
 * @param p_data: ghealth data
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_ghealth_data_set(gh_app_manager_t *p_manage, gh_data_t *p_data);

/**
 * @brief set gsensor data to app manager
 *
 * @param p_manage: manager instance
 * @param p_data: gsensor data
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_gsensor_data_set(gh_app_manager_t *p_manage, gh_gsensor_ts_and_data_t *p_data);

/**
 * @brief set gsensor data to app manager
 *
 * @param p_manage: manager instance
 * @param p_data: gsensor data
 * @param size: gsensor data size
 * @return refer to gh_app_manager_ret_e
 *
 * @note None
 */
extern uint32_t gh_app_manager_gsensor_data_sync_set(gh_app_manager_t *p_manage,
                                                     gh_gsensor_ts_and_data_t *p_data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* __GH_APP_MANAGER_H__ */


