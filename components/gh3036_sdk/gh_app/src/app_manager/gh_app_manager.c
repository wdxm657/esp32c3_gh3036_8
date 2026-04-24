/**
  ****************************************************************************************
  * @file    gh_app_manager.c
  * @author  GHealth Driver Team
  * @brief   gh application manager file
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
#include "gh_hal_utils.h"
#include "gh_app.h"
#include "gh_app_manager.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if GH_APP_MANAGER_LOG_EN
#define DEBUG_LOG(...)                      GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                    GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                      GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    (GH_APP_MANAGER_ID << 24 | (internal_err) << 8 | (interface_err))

#define GH_CHANNEL_PPG_MAP_BASE_ADDR        (0x1300)
#define GH_CHANNEL_CAP_MAP_BASE_ADDR        (0x2d00)

#define GH_CHANNEL_MAP_SIZE                 (0x50)
#define GH_CHANNEL_ITEM_SIZE                (2)
#define GH_CHANNEL_PPG_CFG_NUM              (8)
#define GH_CHANNEL_CAP_CFG_NUM              (1)
#define GH_CHANNEL_PPG_END_ADDR             (GH_CHANNEL_PPG_MAP_BASE_ADDR \
                                            + GH_CHANNEL_MAP_SIZE * GH_CHANNEL_PPG_CFG_NUM)
#define GH_CHANNEL_CAP_END_ADDR             (GH_CHANNEL_CAP_MAP_BASE_ADDR \
                                            + GH_CHANNEL_MAP_SIZE * GH_CHANNEL_CAP_CFG_NUM)

#define GH_CHANNEL_RX0_L_OFFSET             (0)
#define GH_CHANNEL_RX0_H_OFFSET             (2)
#define GH_CHANNEL_RX1_L_OFFSET             (4)
#define GH_CHANNEL_RX1_H_OFFSET             (6)
#define GH_CHANNEL_RX2_L_OFFSET             (8)
#define GH_CHANNEL_RX2_H_OFFSET             (10)
#define GH_CHANNEL_RX3_L_OFFSET             (12)
#define GH_CHANNEL_RX3_H_OFFSET             (14)
#define GH_CHANNEL_RX0_DATA_OFFSET          (16)
#define GH_CHANNEL_RX1_DATA_OFFSET          (18)
#define GH_CHANNEL_RX2_DATA_OFFSET          (20)
#define GH_CHANNEL_RX3_DATA_OFFSET          (22)

#define GH_CHAMP_CFG0_RX0_L_BIT             (1 << 0)
#define GH_CHAMP_CFG0_RX1_L_BIT             (1 << 1)
#define GH_CHAMP_CFG0_RX0_DATA_BIT          (1 << 2)
#define GH_CHAMP_CFG0_RX1_DATA_BIT          (1 << 3)
#define GH_CHAMP_CFG0_RX0_DATA_OFFSET       (0)
#define GH_CHAMP_CFG0_RX1_DATA_OFFSET       (8)
#define GH_CHAMP_CFG0_RX_DATA_MASK          (0xff)
#define GH_CHAMP_CFG_RX0_READY_BIT          (GH_CHAMP_CFG0_RX0_L_BIT | GH_CHAMP_CFG0_RX0_DATA_BIT)
#define GH_CHAMP_CFG_RX1_READY_BIT          (GH_CHAMP_CFG0_RX1_L_BIT | GH_CHAMP_CFG0_RX1_DATA_BIT)







typedef enum
{
    GH_MANAGER_DATA_BG0         = 0,
    GH_MANAGER_DATA_BG1         = 1,
    GH_MANAGER_DATA_BG2         = 2,
} gh_manager_bg_id_e;

typedef enum
{
    GH_MANAGER_DATA_MIX0        = 0,
    GH_MANAGER_DATA_MIX1        = 1,
} gh_manager_mix_id_e;

typedef enum
{
    GH_CHANNEL_RX0_L_IDX        = 0,
    GH_CHANNEL_RX0_H_IDX        = 1,
    GH_CHANNEL_RX1_L_IDX        = 2,
    GH_CHANNEL_RX1_H_IDX        = 3,
    GH_CHANNEL_RX2_L_IDX        = 4,
    GH_CHANNEL_RX2_H_IDX        = 5,
    GH_CHANNEL_RX3_L_IDX        = 6,
    GH_CHANNEL_RX3_H_IDX        = 7,
    GH_CHANNEL_RX0_RX1_DATA_IDX = 8,
    GH_CHANNEL_RX2_RX3_DATA_IDX = 9,
} gh_manager_rx_e;

typedef enum
{
    GH_CHANNEL_FUNC_ADT,
    GH_CHANNEL_FUNC_HR,
    GH_CHANNEL_FUNC_SPO2,
    GH_CHANNEL_FUNC_HRV,
    GH_CHANNEL_FUNC_GNADT,
    GH_CHANNEL_FUNC_IRNADT,
    GH_CHANNEL_FUNC_TEST1,
    GH_CHANNEL_FUNC_TEST2,
    GH_CHANNEL_FUNC_SLOT,
    GH_CHANNEL_FUNC_MAX
} gh_manager_func_id_e;

typedef enum
{
    GH_APP_PPG_RX0          = 0,
    GH_APP_PPG_RX1          = 1,
    GH_APP_PPG_RX2          = 2,
    GH_APP_PPG_RX3          = 3,
    GH_APP_PPG_RX_MAX
} gh_manager_rx_id_e;

typedef enum
{
    GH_APP_PPG_CFG0         = 0,
    GH_APP_PPG_CFG1         = 1,
    GH_APP_PPG_CFG2         = 2,
    GH_APP_PPG_CFG3         = 3,
    GH_APP_PPG_CFG4         = 4,
    GH_APP_PPG_CFG5         = 5,
    GH_APP_PPG_CFG6         = 6,
    GH_APP_PPG_CFG7         = 7,
    GH_APP_CAP_CFG          = 13,
    GH_APP_CFG_MAX          = 9
} gh_manager_cfg_id_e;

typedef struct
{
    uint16_t func_adt       : 1;
    uint16_t func_hr        : 1;
    uint16_t func_hrv       : 1;
    uint16_t func_spo2      : 1;
    uint16_t func_gnadt     : 1;
    uint16_t func_irnadt     : 1;
    uint16_t func_cap       : 1;
    uint16_t func_test1     : 1;
    uint16_t func_test2     : 1;
    uint16_t func_slot      : 1;
    uint16_t resversed      : 6;
} __attribute__ ((packed)) gh_manager_func_t;

typedef union
{
    uint16_t bits;
    gh_manager_func_t func;
} gh_manager_func_union_t;

typedef struct
{
    uint8_t debugdata       : 1;
    uint8_t bgdata0         : 1;
    uint8_t mixdata0        : 1;
    uint8_t bgdata1         : 1;
    uint8_t mixdata1        : 1;
    uint8_t bgdata2         : 1;
    uint8_t dredata         : 1;
    uint8_t rawdata         : 1;
} __attribute__ ((packed)) gh_manager_data_t;

typedef enum
{
    GH_DATA_DEBUGDATA       = 0,
    GH_DATA_BGDATA0         = 1,
    GH_DATA_MIXDATA0        = 2,
    GH_DATA_BGDATA1         = 3,
    GH_DATA_MIXDATA1        = 4,
    GH_DATA_BGDATA2         = 5,
    GH_DATA_DREDATA         = 6,
    GH_DATA_RAWDATA         = 7,
    GH_DATA_DRE_DC          = 8,
    GH_DATA_TYPE_END,
} gh_manager_data_type_e;

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
/* functions name */
const int8_t g_function_name[GH_FUNC_FIX_IDX_MAX][GH_FUNC_NAME_LEN] =
{
    {"ADT"},
    {"HR"},
    {"SPO2"},
    {"HRV"},
    {"GNADT"},
    {"IRNADT"},
    {"TEST1"},
    {"TEST2"},
    {"PPG_CFG0"},
    {"PPG_CFG1"},
    {"PPG_CFG2"},
    {"PPG_CFG3"},
    {"PPG_CFG4"},
    {"PPG_CFG5"},
    {"PPG_CFG6"},
    {"PPG_CFG7"},
    {"CAP_CFG"}
};

/* data type name */
const int8_t g_data_type_name[GH_FUNC_FIX_IDX_MAX][GH_FUNC_NAME_LEN] =
{
    {"GH_PPG_DATA"},
    {"GH_PPG_MIX_DATA"},
    {"GH_PPG_BG_DATA"},
    {"GH_PPG_BG_CANCEL"},
    {"GH_PPG_LED_DRV"},
    {"GH_ECG_DATA"},
    {"GH_BIA_DATA"},
    {"GH_GSR_DATA"},
    {"GH_PRESSURE_DATA"},
    {"GH_TEMPERATURE_DATA"},
    {"GH_CAP_DATA"},
    {"GH_PPG_PARAM_DATA"},
    {"GH_PPG_DRE_DATA"},
    {"GH_HAL_DATA_TYPE_END"}
};

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC) || (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
#if 1 == GH_STACK_INFO_EN
uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame);
#else
static uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame);
#endif
#endif
static gh_app_manager_ret_e gh_app_manager_ch_config_set(gh_app_manager_t *p_manage, uint16_t func_val,
                                                         uint16_t data_val, gh_manager_cfg_id_e cfg_id,
                                                         gh_manager_rx_id_e rx_id, uint8_t cap_en);
static gh_app_manager_ret_e gh_app_manager_channel_set(gh_manager_data_type_e type, gh_manager_cfg_id_e cfg_id,
                                                       gh_manager_rx_id_e rx_id, gh_hal_data_channel_t *p_ch,
                                                       uint8_t cap_en);

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC) || (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
#if 1 == GH_STACK_INFO_EN
uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame)
#else
static uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame)
#endif
{
    if (GH_NULL_PTR == p_parent_node || GH_NULL_PTR == p_frame)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    gh_app_manager_t *p_manage = (gh_app_manager_t *)p_parent_node;

//    DEBUG_LOG("[fusion_callback] func:%s, type:%d\r\n", g_function_name[p_frame->id],
//              p_frame->p_ch_map->data_type);
#if (1 == GH_ALGO_ADAPTER_EN)
    /* step1: call algo */
    gh_algo_process(&p_manage->adapter_inst, p_frame);
#endif

    /* step2: call app callback to publish */
    if (p_manage->data_cb)
    {
        p_manage->data_cb(p_frame);
    }

    return 0;
}
#endif

uint32_t gh_app_manager_init(gh_app_manager_t *p_manage, gh_data_cb_t data_cb)
{
    if (GH_NULL_PTR == p_manage)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    /* clear struct */
    gh_memset((void *)p_manage, 0, sizeof(gh_app_manager_t));
    p_manage->data_cb = data_cb;

    /* init each function */
#if (GH_FUNC_ADT_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX].p_ch_map = p_manage->frame_buf.adt_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX].p_data = p_manage->frame_buf.adt_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX].ch_max = GH_ADT_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX].id = GH_FUNC_FIX_IDX_ADT;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_ADT_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_ADT] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_ADT_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_ADT_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_HR_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].p_ch_map = p_manage->frame_buf.hr_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].p_data = p_manage->frame_buf.hr_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].ch_max = GH_HR_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].id = GH_FUNC_FIX_IDX_HR;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_HR] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_HR_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_HR_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_SPO2_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX].p_ch_map = p_manage->frame_buf.spo2_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX].p_data = p_manage->frame_buf.spo2_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX].ch_max = GH_SPO2_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX].id = GH_FUNC_FIX_IDX_SPO2;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_SPO2_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_SPO2] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_SPO2_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_SPO2_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_HRV_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX].p_ch_map = p_manage->frame_buf.hrv_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX].p_data = p_manage->frame_buf.hrv_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX].ch_max = GH_HRV_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX].id = GH_FUNC_FIX_IDX_HRV;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HRV_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_HRV] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_HRV_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_HRV_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_GNADT_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX].p_ch_map = p_manage->frame_buf.gnadt_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX].p_data = p_manage->frame_buf.gnadt_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX].ch_max = GH_NADT_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX].id = GH_FUNC_FIX_IDX_GNADT;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_GNADT_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_GNADT] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_GNADT_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_GNADT_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_IRNADT_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX].p_ch_map = p_manage->frame_buf.irnadt_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX].p_data = p_manage->frame_buf.irnadt_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX].ch_max = GH_NADT_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX].id = GH_FUNC_FIX_IDX_IRNADT;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_IRNADT_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_IRNADT] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_IRNADT_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_IRNADT_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_TEST1_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX].p_ch_map = p_manage->frame_buf.test1_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX].p_data = p_manage->frame_buf.test1_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX].ch_max = GH_TEST_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX].id = GH_FUNC_FIX_IDX_TEST1;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST1_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_TEST1] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_TEST1_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_TEST1_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_TEST2_EN)
    p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2] = &p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX];
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX].p_ch_map = p_manage->frame_buf.test2_ch_map;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX].p_data = p_manage->frame_buf.test2_data;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX].ch_max = GH_TEST_RAWDATA_MAX;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX].id = GH_FUNC_FIX_IDX_TEST2;
    p_manage->frame_inst.func_frame[GH_FUNC_ID_TEST2_IDX].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    p_manage->frame_inst.p_data_collector[GH_FUNC_FIX_IDX_TEST2] =
        &p_manage->frame_inst.data_collector[GH_FUNC_ID_TEST2_IDX];
    p_manage->frame_inst.data_collector[GH_FUNC_ID_TEST2_IDX].p_parent_node = (void *)p_manage;
#endif
#endif
#if (GH_FUNC_SLOT_EN)
    gh_func_index_e idx = GH_FUNC_ID_PPG_CFG0_IDX;
    uint8_t ppg_idx = GH_CHIP_PPG_CONFIG0;

    for (gh_func_fix_idx_e id = GH_FUNC_FIX_IDX_PPG_CFG0; id < GH_FUNC_FIX_IDX_CAP_CFG; id++)
    {
        p_manage->frame_inst.p_func_frame[id] = &p_manage->frame_inst.func_frame[idx];
        p_manage->frame_inst.func_frame[idx].p_ch_map = p_manage->frame_buf.ppg_ch_map[ppg_idx];
        p_manage->frame_inst.func_frame[idx].p_data = p_manage->frame_buf.ppg_data[ppg_idx];
        p_manage->frame_inst.func_frame[idx].ch_max = GH_PPG_SLOT_DATA_MAX;
        p_manage->frame_inst.func_frame[idx].id = id;
        p_manage->frame_inst.func_frame[idx].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
        p_manage->frame_inst.p_data_collector[id] = &p_manage->frame_inst.data_collector[idx];
        p_manage->frame_inst.data_collector[idx].p_parent_node = (void *)p_manage;
#endif
        idx++;
        ppg_idx++;
    }

    idx = GH_FUNC_ID_CAP_CFG_IDX;
    uint8_t cap_idx = GH_CHIP_CAP_CONFIG0;

    for (gh_func_fix_idx_e id = GH_FUNC_FIX_IDX_CAP_CFG; id < GH_FUNC_FIX_IDX_MAX; id++)
    {
        p_manage->frame_inst.p_func_frame[id] = &p_manage->frame_inst.func_frame[idx];
        p_manage->frame_inst.func_frame[idx].p_ch_map = p_manage->frame_buf.cap_ch_map[cap_idx];
        p_manage->frame_inst.func_frame[idx].p_data = p_manage->frame_buf.cap_data[cap_idx];
        p_manage->frame_inst.func_frame[idx].ch_max = GH_CAP_SLOT_DATA_MAX;
        p_manage->frame_inst.func_frame[idx].id = id;
        p_manage->frame_inst.func_frame[idx].gsensor_en = 0;
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
        p_manage->frame_inst.p_data_collector[id] = &p_manage->frame_inst.data_collector[idx];
        p_manage->frame_inst.data_collector[idx].p_parent_node = (void *)p_manage;
#endif
        idx++;
        cap_idx++;
    }
#endif

#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    /* init fusion */
    gh_fusion_init(&p_manage->data_fusion, p_manage->frame_inst.p_data_collector,
                   p_manage->frame_inst.p_func_frame, (gh_frame_publish_t)gh_app_manager_fusion_callback,
                   (void *)p_manage);
#endif

#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    for (size_t i = 0; i < GH_FUNC_ID_MAX; i++)
    {
        p_manage->frame_inst.sync_collector[i].p_frame = &p_manage->frame_inst.func_frame[i];
    }
    gh_sync_fusion_init(&p_manage->data_sync_fusion, p_manage->frame_inst.sync_collector,
                        GH_FUNC_ID_MAX, p_manage->gsensor_data, GH_SYNC_GSENSOR_DATA_MAX,
                        gh_app_manager_fusion_callback, (void *)p_manage);
#endif
#if (1 == GH_ALGO_ADAPTER_EN)

    /* init algorithm adapter */
    gh_algo_adapter_init(&p_manage->adapter_inst);
#if GH_USE_GOODIX_HR_ALGO
    p_manage->frame_inst.func_frame[GH_FUNC_ID_HR_IDX].p_algo_input =
        (void *)p_manage->adapter_inst.algo_param[GH_FUNC_FIX_IDX_HR].p_param;
#endif
#endif
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}

uint32_t gh_app_manager_deinit(gh_app_manager_t *p_manage)
{
    if (GH_NULL_PTR == p_manage)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    gh_memset((void *)p_manage, 0, sizeof(gh_app_manager_t));

#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    gh_fusion_deinit(&p_manage->data_fusion);
#endif
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    gh_sync_fusion_deinit(&p_manage->data_sync_fusion);
#endif
#if (1 == GH_ALGO_ADAPTER_EN)
    gh_algo_adapter_deinit(&p_manage->adapter_inst);
#endif

    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}

uint32_t gh_app_manager_reset(gh_app_manager_t *p_manage)
{
    if (GH_NULL_PTR == p_manage)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    /* reset each function frame */
    for (gh_func_fix_idx_e idx = GH_FUNC_FIX_IDX_ADT; idx < GH_FUNC_FIX_IDX_MAX; idx++)
    {
        if (GH_NULL_PTR != p_manage->frame_inst.p_func_frame[idx])
        {
            p_manage->frame_inst.p_func_frame[idx]->ch_num = 0;
            p_manage->frame_inst.p_func_frame[idx]->frame_cnt = 0;
            p_manage->frame_inst.p_func_frame[idx]->timestamp = 0;
        }
    }
#if (1 == GH_ALGO_ADAPTER_EN)

    /* reset algorithm adapter channel map */
    gh_algo_adapter_reset(&p_manage->adapter_inst);
#endif
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}

static gh_app_manager_ret_e gh_app_manager_map_func_set(gh_app_manager_t *p_manage, gh_manager_func_id_e func_id,
                                                        gh_hal_data_channel_t *p_data_ch)
{
    if (func_id >= GH_CHANNEL_FUNC_MAX || GH_NULL_PTR == p_data_ch)
    {
        return GH_MANAGER_PTR_NULL;
    }

    uint8_t index = 0;
    gh_hal_data_channel_t *p_target = GH_NULL_PTR;
    gh_func_fix_idx_e func_fix_id  = GH_FUNC_FIX_IDX_ADT;

    /* Convert func_id to fixed id */
    if (func_id < GH_CHANNEL_FUNC_SLOT)
    {
        switch (func_id)
        {
            case GH_CHANNEL_FUNC_ADT:
#if (GH_FUNC_ADT_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_ADT;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_ADT_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_HR:
#if (GH_FUNC_HR_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_HR;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_HR_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_SPO2:
#if (GH_FUNC_SPO2_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_SPO2;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_SPO2_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_HRV:
#if (GH_FUNC_HRV_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_HRV;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_HRV_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_GNADT:
#if (GH_FUNC_GNADT_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_GNADT;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_GNADT_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_IRNADT:
#if (GH_FUNC_IRNADT_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_IRNADT;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_IRNADT_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_TEST1:
#if (GH_FUNC_TEST1_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_TEST1;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_TEST1_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            case GH_CHANNEL_FUNC_TEST2:
#if (GH_FUNC_TEST2_EN)
            {
                func_fix_id = GH_FUNC_FIX_IDX_TEST2;
            }
            break;
#else
                ERROR_LOG("[%s] GH_FUNC_TEST2_EN is disable!\r\n", __FUNCTION__);
                return GH_MANAGER_FUNC_DISABLE_ERR;
#endif

            default:
            {
                ERROR_LOG("[%s] wrong func_id:%d\r\n", __FUNCTION__, func_id);
                return GH_MANAGER_FUNC_DISABLE_ERR;
            }
        } // switch (func_id)
    } // if (func_id < GH_CHANNEL_FUNC_SLOT)
    else if (GH_CHANNEL_FUNC_SLOT == func_id)
    {
#if (GH_FUNC_SLOT_EN)
        func_fix_id = (gh_func_fix_idx_e)(GH_FUNC_FIX_IDX_PPG_CFG0 + p_data_ch->channel_ppg.slot_cfg_id);
        if (GH_CAP_CFG == p_data_ch->channel_ppg.slot_cfg_id)
        {
            func_fix_id = GH_FUNC_FIX_IDX_CAP_CFG;
        }
#else
        ERROR_LOG("[%s] GH_FUNC_SLOT_EN is disable!\r\n", __FUNCTION__);
        return GH_MANAGER_FUNC_DISABLE_ERR;
#endif
    }

    /* Determine whether the number of parsed channels exceeds the upper limit */
    if (p_manage->frame_inst.p_func_frame[func_fix_id]->ch_num
        >= p_manage->frame_inst.p_func_frame[func_fix_id]->ch_max)
    {
        ERROR_LOG("[%s] %s: out of range! max:%d\r\n", __FUNCTION__, g_function_name[func_fix_id],
                  p_manage->frame_inst.p_func_frame[func_fix_id]->ch_max);
        return GH_MANAGER_FUNC_OUT_RANGE_ERR;
    }

    index = p_manage->frame_inst.p_func_frame[func_fix_id]->ch_num;
    p_manage->frame_inst.p_func_frame[func_fix_id]->ch_num++;

    /* Write channel information */
    p_target = p_manage->frame_inst.p_func_frame[func_fix_id]->p_ch_map + index;
    p_target->channel = p_data_ch->channel;

    DEBUG_LOG("[%s] %s: %s, cfg%d, rx%d, chn:%d, num:%d \r\n", __FUNCTION__, g_function_name[func_fix_id],
              g_data_type_name[p_target->channel_ppg.data_type], p_target->channel_ppg.slot_cfg_id,
              p_target->channel_ppg.rx_id, p_target->channel,
              p_manage->frame_inst.p_func_frame[func_fix_id]->ch_num);

    return GH_MANAGER_OK;
}

static gh_app_manager_ret_e gh_app_manager_channel_set(gh_manager_data_type_e type, gh_manager_cfg_id_e cfg_id,
                                                       gh_manager_rx_id_e rx_id, gh_hal_data_channel_t *p_ch,
                                                       uint8_t cap_en)
{
    /* Assemble data_channel by data type */
    switch (type)
    {
        case GH_DATA_DEBUGDATA:
        {
            p_ch->data_type = GH_PPG_PARAM_DATA;
            p_ch->channel_ppg_param.rx_id = rx_id;
            p_ch->channel_ppg_param.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_BGDATA0:
        {
            p_ch->data_type = GH_PPG_BG_DATA;
            p_ch->channel_ppg_bg.bg_id = GH_MANAGER_DATA_BG0;
            p_ch->channel_ppg_bg.rx_id = rx_id;
            p_ch->channel_ppg_bg.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_MIXDATA0:
        {
            p_ch->data_type = GH_PPG_MIX_DATA;
            p_ch->channel_ppg_mix.mix_id = GH_MANAGER_DATA_MIX0;
            p_ch->channel_ppg_mix.rx_id = rx_id;
            p_ch->channel_ppg_mix.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_BGDATA1:
        {
            p_ch->data_type = GH_PPG_BG_DATA;
            p_ch->channel_ppg_bg.bg_id = GH_MANAGER_DATA_BG1;
            p_ch->channel_ppg_bg.rx_id = rx_id;
            p_ch->channel_ppg_bg.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_MIXDATA1:
        {
            p_ch->data_type = GH_PPG_MIX_DATA;
            p_ch->channel_ppg_mix.mix_id = GH_MANAGER_DATA_MIX1;
            p_ch->channel_ppg_mix.rx_id = rx_id;
            p_ch->channel_ppg_mix.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_BGDATA2:
        {
            p_ch->data_type = GH_PPG_BG_DATA;
            p_ch->channel_ppg_bg.bg_id = GH_MANAGER_DATA_BG2;
            p_ch->channel_ppg_bg.rx_id = rx_id;
            p_ch->channel_ppg_bg.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_DREDATA:
        {
            p_ch->data_type = GH_PPG_DRE_DATA;
            p_ch->channel_ppg_dre.rx_id = rx_id;
            p_ch->channel_ppg_dre.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_RAWDATA:
        {
            if (cap_en)
            {
                p_ch->data_type = GH_CAP_DATA;
            }
            else
            {
                p_ch->data_type = GH_PPG_DATA;
            }
            p_ch->channel_ppg.rx_id = rx_id;
            p_ch->channel_ppg.slot_cfg_id = cfg_id;
        }
        break;

        case GH_DATA_DRE_DC:
        {
            p_ch->data_type = GH_PPG_DRE_DC_INFO;
            p_ch->channel_ppg.rx_id = rx_id;
            p_ch->channel_ppg.slot_cfg_id = cfg_id;
        }
        break;
        default:
        {
            ERROR_LOG("[%s] wrong type:%d \r\n", __FUNCTION__, type);
            return GH_MANAGER_DATA_TYPE_ERR;
        }
    } // switch (type)

    return GH_MANAGER_OK;
}

static gh_app_manager_ret_e gh_app_manager_ch_config_set(gh_app_manager_t *p_manage, uint16_t func_val,
                                                         uint16_t data_val, gh_manager_cfg_id_e cfg_id,
                                                         gh_manager_rx_id_e rx_id, uint8_t cap_en)
{
    gh_hal_data_channel_t data_ch;

    /* The content is filled by the function gh_app_manager_channel_set */
    gh_memset((void *)&data_ch, 0, sizeof(gh_hal_data_channel_t));

    /* No function or data type is not required */
    if (0 == func_val || 0 == data_val)
    {
        return GH_MANAGER_FUNC_EMPTY_ERR;
    }

    /* Fill the required data_channel for each feature */
    for (gh_manager_func_id_e func_id = GH_CHANNEL_FUNC_ADT; func_id < GH_CHANNEL_FUNC_MAX; func_id++)
    {
        if (0 != ((func_val >> func_id) & 0x1))
        {
            for (gh_manager_data_type_e data_type = GH_DATA_DEBUGDATA; data_type < GH_DATA_TYPE_END; data_type++)
            {
                if (((data_val >> data_type) & 0x1)
                    && (GH_MANAGER_OK == gh_app_manager_channel_set(data_type, cfg_id, rx_id, &data_ch, cap_en)))
                {
                    gh_app_manager_map_func_set(p_manage, func_id, &data_ch);
                    data_ch.channel = 0;
                }
            }
        }
    }

    return GH_MANAGER_OK;
}

uint32_t gh_app_manager_config(gh_app_manager_t *p_manage, uint16_t addr, uint16_t val)
{
    if (addr < (uint16_t)GH_CHANNEL_PPG_MAP_BASE_ADDR || (addr >= (uint16_t)GH_CHANNEL_PPG_END_ADDR
        && addr < (uint16_t)GH_CHANNEL_CAP_MAP_BASE_ADDR) || addr >= (uint16_t)GH_CHANNEL_CAP_END_ADDR)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_ADDR_ERR);
    }

    if (GH_NULL_PTR == p_manage)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    gh_manager_cfg_id_e cfg_id = GH_APP_PPG_CFG0;
    uint8_t cfg_addr = 0;
    uint8_t cap_en = 0;

    /* The addresses configured for PPG and CAP are not in the same range and need to be processed separately */
    if (addr >= (uint16_t)GH_CHANNEL_PPG_MAP_BASE_ADDR && addr < (uint16_t)GH_CHANNEL_PPG_END_ADDR)
    {
        cfg_id = (gh_manager_cfg_id_e)((addr - GH_CHANNEL_PPG_MAP_BASE_ADDR) / GH_CHANNEL_MAP_SIZE);
        cfg_addr = addr - GH_CHANNEL_PPG_MAP_BASE_ADDR - (cfg_id * GH_CHANNEL_MAP_SIZE);
        if (cfg_id >= GH_CHANNEL_PPG_CFG_NUM)
        {
            return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_CFG_ERR);
        }
        cap_en = 0;
    }
    else
    {
        cfg_id = (gh_manager_cfg_id_e)((addr - GH_CHANNEL_CAP_MAP_BASE_ADDR) / GH_CHANNEL_MAP_SIZE);
        cfg_addr = addr - GH_CHANNEL_CAP_MAP_BASE_ADDR - (cfg_id * GH_CHANNEL_MAP_SIZE);
        if (cfg_id >= GH_CHANNEL_CAP_CFG_NUM)
        {
            return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_CFG_ERR);
        }
        cap_en = 1;
    }

    /* Record configuration assembly progress */
    switch (cfg_addr)
    {
        case GH_CHANNEL_RX0_L_OFFSET:
        {
            p_manage->chamap_parser.champ_cfg_rx0_l = val;
            p_manage->chamap_parser.ready |= GH_CHAMP_CFG0_RX0_L_BIT;
        }
        break;

        case GH_CHANNEL_RX1_L_OFFSET:
        {
            p_manage->chamap_parser.champ_cfg_rx1_l = val;
            p_manage->chamap_parser.ready |= GH_CHAMP_CFG0_RX1_L_BIT;
        }
        break;

        case GH_CHANNEL_RX0_DATA_OFFSET:
        {
            p_manage->chamap_parser.champ_cfg_rx0_data = val;
            p_manage->chamap_parser.ready |= GH_CHAMP_CFG0_RX0_DATA_BIT;
        }
        break;

        case GH_CHANNEL_RX1_DATA_OFFSET:
        {
            p_manage->chamap_parser.champ_cfg_rx1_data = val;
            p_manage->chamap_parser.ready |= GH_CHAMP_CFG0_RX1_DATA_BIT;
        }
        break;

        default:
        {
            ERROR_LOG("[%s] wrong address offset:%d, %d \r\n", __FUNCTION__, addr, cfg_addr);
        }
        break;
    } // switch (cfg_addr)

    /* After assembling one RX, record the configuration */
    if (GH_CHAMP_CFG_RX0_READY_BIT == (GH_CHAMP_CFG_RX0_READY_BIT & p_manage->chamap_parser.ready))
    {
        if (cap_en)
        {
            cfg_id = GH_APP_CAP_CFG;
        }
        gh_app_manager_ch_config_set(p_manage, p_manage->chamap_parser.champ_cfg_rx0_l,
                                     p_manage->chamap_parser.champ_cfg_rx0_data,
                                     cfg_id, GH_APP_PPG_RX0, cap_en);

        p_manage->chamap_parser.champ_cfg_rx0_l = 0;
        p_manage->chamap_parser.champ_cfg_rx0_data = 0;
        p_manage->chamap_parser.ready = p_manage->chamap_parser.ready & (~GH_CHAMP_CFG_RX0_READY_BIT);
    }

    if (GH_CHAMP_CFG_RX1_READY_BIT == (GH_CHAMP_CFG_RX1_READY_BIT & p_manage->chamap_parser.ready))
    {
        if (cap_en)
        {
            cfg_id = GH_APP_CAP_CFG;
        }

        gh_app_manager_ch_config_set(p_manage, p_manage->chamap_parser.champ_cfg_rx1_l,
                                     p_manage->chamap_parser.champ_cfg_rx1_data,
                                     cfg_id, GH_APP_PPG_RX1, cap_en);

        p_manage->chamap_parser.champ_cfg_rx1_l = 0;
        p_manage->chamap_parser.champ_cfg_rx1_data = 0;
        p_manage->chamap_parser.ready = p_manage->chamap_parser.ready & (~GH_CHAMP_CFG_RX1_READY_BIT);
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}

uint32_t gh_app_manager_update(gh_app_manager_t *p_manage)
{
#if 0
    DEBUG_LOG("[%s] ADT p_func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_ADT]->p_data);
    DEBUG_LOG("[%s] HR func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HR]->p_data);
    DEBUG_LOG("[%s] SPO2 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_SPO2]->p_data);
    DEBUG_LOG("[%s] HRV func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_HRV]->p_data);
    DEBUG_LOG("[%s] GNADT func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_GNADT]->p_data);
    DEBUG_LOG("[%s] IRNADT func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_IRNADT]->p_data);
    DEBUG_LOG("[%s] TEST1 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST1]->p_data);
    DEBUG_LOG("[%s] TEST2 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_TEST2]->p_data);
    DEBUG_LOG("[%s] CFG0 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG0],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG0]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG0]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG0]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG0]->p_data);
    DEBUG_LOG("[%s] CFG1 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG1],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG1]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG1]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG1]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG1]->p_data);
    DEBUG_LOG("[%s] CFG2 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG2],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG2]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG2]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG2]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG2]->p_data);
    DEBUG_LOG("[%s] CFG3 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG3],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG3]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG3]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG3]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG3]->p_data);
    DEBUG_LOG("[%s] CFG4 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG4],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG4]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG4]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG4]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG4]->p_data);
    DEBUG_LOG("[%s] CFG5 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG5],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG5]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG5]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG5]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG5]->p_data);
    DEBUG_LOG("[%s] CFG6 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG6],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG6]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG6]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG6]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG6]->p_data);
    DEBUG_LOG("[%s] CFG7 func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG7],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG7]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG7]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG7]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_PPG_CFG7]->p_data);
    DEBUG_LOG("[%s] CAP func_frame:0x%p, ch_num:%d, id:%d, p_ch_map:0x%p, p_data:0x%p \r\n", __FUNCTION__,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_CAP_CFG],
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_CAP_CFG]->ch_num,
              p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_CAP_CFG]->id,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_CAP_CFG]->p_ch_map,
              (void *)p_manage->frame_inst.p_func_frame[GH_FUNC_FIX_IDX_CAP_CFG]->p_data);
#endif

    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}

#if (GH_FUNC_SLOT_EN)
uint32_t gh_app_manager_slot_map_check(gh_app_manager_t *p_manage, gh_slot_enable_t *p_slot)
{
    if (GH_NULL_PTR == p_manage || GH_NULL_PTR == p_slot)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_PTR_NULL);
    }

    for (uint16_t ppg_idx = GH_FUNC_ID_PPG_CFG0_IDX; ppg_idx < GH_FUNC_ID_CAP_CFG_IDX; ppg_idx++)
    {
        if (0 != p_manage->frame_inst.func_frame[ppg_idx].ch_num)
        {
            p_slot->ppg_cfg[ppg_idx - GH_FUNC_ID_PPG_CFG0_IDX] = 1;
        }
        else
        {
            p_slot->ppg_cfg[ppg_idx - GH_FUNC_ID_PPG_CFG0_IDX] = 0;
        }
    }

    for (uint16_t cap_idx = GH_FUNC_ID_CAP_CFG_IDX; cap_idx < GH_FUNC_ID_MAX; cap_idx++)
    {
        if (0 != p_manage->frame_inst.func_frame[cap_idx].ch_num)
        {
            p_slot->cap_cfg[cap_idx - GH_FUNC_ID_CAP_CFG_IDX] = 1;
        }
        else
        {
            p_slot->cap_cfg[cap_idx - GH_FUNC_ID_CAP_CFG_IDX] = 0;
        }
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
}
#endif

uint32_t gh_app_manager_func_set(gh_app_manager_t *p_manage, uint32_t fix_id)
{
#if (1 == GH_ALGO_ADAPTER_EN)
    uint32_t func_id = 0;
    DEBUG_LOG("[%s]fix_id = 0x%x\r\n", __FUNCTION__, fix_id);

    /* Turning on or off an algorithm needs to be recorded separately */
    /* and the operation status of the current algorithm needs to be updated */
    func_id = (~p_manage->func_cur) & fix_id;
    if (0 != func_id)
    {
        gh_algo_init(&p_manage->adapter_inst, func_id);
    }

    func_id = p_manage->func_cur & (p_manage->func_cur ^ fix_id);
    if (0 != func_id)
    {
        gh_algo_deinit(&p_manage->adapter_inst, func_id);
    }
#endif

    p_manage->func_cur = fix_id;

#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    return gh_fusion_ctrl(&p_manage->data_fusion, fix_id);
#elif (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    return gh_sync_fusion_ctrl(&p_manage->data_sync_fusion, fix_id);
#else
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
#endif
}

uint32_t gh_app_manager_ghealth_data_set(gh_app_manager_t *p_manage, gh_data_t *p_data)
{
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC)
    // DEBUG_LOG("[%s] ENTER\r\n", __FUNCTION__);
    return gh_fusion_ghealth_data_push(&p_manage->data_fusion, p_data);
#elif (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    return gh_sync_fusion_ghealth_data_push(&p_manage->data_sync_fusion, p_data);
#else
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
#endif
}

uint32_t gh_app_manager_gsensor_data_set(gh_app_manager_t *p_manage, gh_gsensor_ts_and_data_t *p_data)
{
#if ((GH_FUSION_MODE_SEL == GH_FUSION_MODE_ASYNC) && (1 == GH_GSENSOR_FUSION_EN))
    // DEBUG_LOG("[%s] ENTER\r\n", __FUNCTION__);
    return gh_fusion_gsensor_data_push(&p_manage->data_fusion, p_data);
#else
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
#endif
}

uint32_t gh_app_manager_gsensor_data_sync_set(gh_app_manager_t *p_manage,
                                              gh_gsensor_ts_and_data_t *p_data, uint16_t size)
{
#if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)
    // DEBUG_LOG("[%s] ENTER\r\n", __FUNCTION__);
    uint32_t ret = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        ret |= gh_sync_fusion_gsensor_data_push(&p_manage->data_sync_fusion, &p_data[i].data);
    }

    return ret;
#else
    return RETURN_VALUE_ASSEMBLY(0, GH_MANAGER_OK);
#endif
}
