/**
  ****************************************************************************************
  * @file    gh_data_fusion.c
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */

#include "gh_hal_service.h"
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_data_sync_fusion.h"
#include "gh_global_config.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    (GH_APP_FUSION_ID << 24 | (internal_err) << 8 | (interface_err))

#if GH_APP_FUSION_LOG_EN
#define DEBUG_LOG(...)                               GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                             GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                               GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define GH_24BIT_TO_32BIT(X)                         (((X) & 0x800000) ? ((X) | 0xFF000000) : (X))
#define GH_BYTE_SIZE                                 (8)
#define GH_COUNT_SIZE(X)                             (sizeof((X)) / sizeof((X)[0]))

#define GH_FUSION_FIX_POINT                          (16)
#define GH_FUSION_FIX_POINT_MASK                     ((1 << GH_FUSION_FIX_POINT) - 1)
#define GH_NUM_TWO                                   (2)

static uint32_t gh_data_sync_fusion_reset(gh_data_sync_fusion_t *p_sync_fusion, gh_func_fix_idx_e id);
static uint32_t gh_sync_fusion_frame_handle(gh_data_sync_fusion_t *p_sync_fusion, uint8_t func_id,
                                            gh_data_t *p_gh_data);
static uint32_t gh_data_sync_fusion_pro(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data);

uint32_t gh_sync_fusion_init(gh_data_sync_fusion_t *p_sync_fusion,
                             gh_data_sync_collector_t *p_sync_collector_list,
                             uint16_t sync_collector_num,
                             gh_gsensor_data_t *p_gsensor_data,
                             uint16_t gsensor_data_max_num,
                             gh_frame_sync_publish_t frame_sync_publish,
                             void *parent_node)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    if (GH_NULL_PTR == p_sync_fusion || GH_NULL_PTR == p_sync_collector_list
        || GH_NULL_PTR == frame_sync_publish || GH_NULL_PTR == p_gsensor_data
        || 0 == gsensor_data_max_num || 0 == sync_collector_num)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_PTR_NULL);
    }

    p_sync_fusion->p_sync_collector_list = p_sync_collector_list;
    p_sync_fusion->sync_collector_num = sync_collector_num;
    p_sync_fusion->frame_sync_publish = frame_sync_publish;
    p_sync_fusion->parent_node = parent_node;
    p_sync_fusion->p_gsensor_data = p_gsensor_data;
    p_sync_fusion->gsensor_data_max_num = gsensor_data_max_num;
    p_sync_fusion->gsensor_data_num = 0;
    p_sync_fusion->init_flag = 1;

    for (uint8_t id = 0; id < p_sync_fusion->sync_collector_num; id++)
    {
        gh_data_sync_fusion_reset(p_sync_fusion, (gh_func_fix_idx_e)id);
    }

    return ret;
}

uint32_t gh_sync_fusion_deinit(gh_data_sync_fusion_t *p_sync_fusion)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    if (0 == p_sync_fusion->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_INIT_ERR);
    }

    for (uint8_t id = 0; id < p_sync_fusion->sync_collector_num; id++)
    {
        gh_data_sync_fusion_reset(p_sync_fusion, (gh_func_fix_idx_e)id);
    }

    p_sync_fusion->init_flag = 0;
    return ret;
}

uint32_t gh_sync_fusion_ctrl(gh_data_sync_fusion_t *p_sync_fusion, uint32_t function_group)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    uint8_t func_cnt;
    if (0 == p_sync_fusion->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_INIT_ERR);
    }

    for (func_cnt = 0; func_cnt < p_sync_fusion->sync_collector_num; func_cnt++)
    {
        if (0 == ((((uint32_t)1) << func_cnt) & (function_group)))
        {
            gh_data_sync_fusion_reset(p_sync_fusion, (gh_func_fix_idx_e)func_cnt);
        }
    }
    p_sync_fusion->gsensor_data_num = 0;

    return ret;
}

uint32_t gh_sync_fusion_ghealth_data_push(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    if (0 == p_sync_fusion->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_INIT_ERR);
    }
    ret = gh_data_sync_fusion_pro(p_sync_fusion, p_gh_data);

    return ret;
}

uint32_t gh_sync_fusion_gsensor_data_push(gh_data_sync_fusion_t *p_sync_fusion,
                                          gh_gsensor_data_t *p_gsensor_data)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    if (0 == p_sync_fusion->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_INIT_ERR);
    }

    if (p_sync_fusion->gsensor_data_num < p_sync_fusion->gsensor_data_max_num)
    {
        gh_memcpy(p_sync_fusion->p_gsensor_data + p_sync_fusion->gsensor_data_num,
                  p_gsensor_data, sizeof(gh_gsensor_data_t));
        p_sync_fusion->gsensor_data_num++;
    }
    else
    {
        WARNING_LOG("[gsensor_data_push] gsensor data buffer is full! size = %d\r\n", size);
    }

    return ret;
}

/**
 * uint32_t gh_data_sync_fusion_reset(gh_data_sync_fusion_t *p_sync_fusion, gh_func_fix_idx_e id)
 * @fn
 * @brief reset sync fusion
 * @param[in] p_sync_fusion : pointer of sync fusion
 * @param[in] id: function id
 * @param[out] None
 * @return error code
 *
 * @note None
 */
static uint32_t gh_data_sync_fusion_reset(gh_data_sync_fusion_t *p_sync_fusion, gh_func_fix_idx_e id)
{
    uint32_t ret = GH_SYNC_FUSION_OK;
    if (GH_NULL_PTR != p_sync_fusion->p_sync_collector_list
        && id < p_sync_fusion->sync_collector_num)
    {
        p_sync_fusion->p_sync_collector_list[id].frame_cnt = 0;
        p_sync_fusion->p_sync_collector_list[id].frame_num = 0;
        gh_memset(p_sync_fusion->p_sync_collector_list[id].channel_bits, 0,
                  sizeof(p_sync_fusion->p_sync_collector_list[id].channel_bits));
    }

    return ret;
}

/**
 * uint32_t gh_check_channel_map(gh_data_sync_collector_t *p_frame_collector,
 *                            gh_data_t *p_gh_data, uint8_t *p_channel_num)
 * @fn
 * @brief check channel map
 * @param[in] p_frame_collector: pointer of frame collector
 * @param[in] p_gh_data: pointer of gh data
 * @param[out] p_channel_num: pointer of channel number
 * @return error code
 *
 * @note None
 */
static uint32_t gh_check_channel_map(gh_data_sync_collector_t *p_frame_collector,
                                     gh_data_t *p_gh_data, uint8_t *p_channel_num)
{
    if (GH_NULL_PTR == p_frame_collector || GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_PTR_NULL);
    }
    gh_hal_data_channel_t *p_ch_map = p_frame_collector->p_frame->p_ch_map;
    uint8_t ch_num = p_frame_collector->p_frame->ch_num;
    uint8_t i = 0;

    for (i = 0; i < ch_num; i++)
    {
        if (p_ch_map[i].channel == p_gh_data->data_channel.channel)
        {
            *p_channel_num = i;
            return GH_SYNC_FUSION_OK;
        }
    }

    return GH_SYNC_FUSION_NO_CHANNEL;
}

/**
 * @fn uint32_t gh_pop_count(uint8_t *p_data, uint16_t size)
 * @brief count how many 1 in data
 * @param[in] p_data : pointer of data
 * @param[in] size: size of data
 * @param[out] None
 * @return pop count
 *
 * @note None
 */
static uint32_t gh_pop_count(uint8_t *p_data, uint16_t size)
{
    uint32_t cnt = 0;
    uint8_t offset = 1;

    for (uint16_t i = 0; i < size; i++)
    {
        uint8_t data = p_data[i];

        while (data != 0)
        {
            data = data & (data - offset);
            cnt++;
        }
    }

    return cnt;
}

/**
 * @fn uint32_t gh_update_frame_collector(gh_data_sync_collector_t *p_frame_collector, gh_data_t *p_gh_data, uint8_t channel_num)
 * @brief update frame collect
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] p_sync_collector: pointer of sync collector
 * @param[in] p_gh_data: pointer of gh data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
static uint32_t gh_update_frame_collector(gh_data_sync_collector_t *p_frame_collector, gh_data_t *p_gh_data,
                                          uint8_t channel_num)
{
    uint32_t ret = GH_SYNC_FUSION_OK;

    if (GH_NULL_PTR == p_frame_collector || GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_PTR_NULL);
    }

    // update rawdata
    switch (p_gh_data->data_channel.data_type)
    {
        case GH_PPG_DATA:
        case GH_PPG_MIX_DATA:
        case GH_PPG_DRE_DC_INFO:

            // rawdata
            p_frame_collector->p_frame->p_data[channel_num].rawdata = p_gh_data->ppg_data.rawdata;

            // agc_info
            p_frame_collector->p_frame->p_data[channel_num].agc_info.gain_code = p_gh_data->ppg_data.gain_code;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.bg_cancel_range =
                p_gh_data->ppg_data.bg_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.dc_cancel_range =
                p_gh_data->ppg_data.dc_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.dc_cancel_code =
                p_gh_data->ppg_data.dc_cancel_code;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.bg_cancel_code =
                p_gh_data->ppg_data.bg_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.led_drv0 = p_gh_data->ppg_data.led_drv[0];
            p_frame_collector->p_frame->p_data[channel_num].agc_info.led_drv1 = p_gh_data->ppg_data.led_drv[1];
            for (uint8_t i = 0; i < GH_LED_DRV_NUM; i++)
            {
                p_frame_collector->p_frame->led_drv_fs[i] = p_gh_data->ppg_data.led_drv_fs[i];
            }

            // flag
            p_frame_collector->p_frame->p_data[channel_num].flag.led_adj_flag = p_gh_data->ppg_data.led_adj_flag;
            p_frame_collector->p_frame->p_data[channel_num].flag.sa_flag = p_gh_data->ppg_data.sa_flag;
            p_frame_collector->p_frame->p_data[channel_num].flag.param_change_flag =
                p_gh_data->ppg_data.param_change_flag;
            p_frame_collector->p_frame->p_data[channel_num].flag.dre_update = p_gh_data->ppg_data.dre_update;

            // ipd
            p_frame_collector->p_frame->p_data[channel_num].ipd_pa = p_gh_data->ppg_data.ipd_pa;
            break;

#if ((1 == GH_CHIP_PPG_DEBUG1_BGDATA0) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA1) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA2))
        case GH_PPG_BG_DATA:

            // rawdata
            p_frame_collector->p_frame->p_data[channel_num].rawdata = p_gh_data->ppg_bg_data.rawdata;

            // agc_info
            p_frame_collector->p_frame->p_data[channel_num].agc_info.gain_code = p_gh_data->ppg_bg_data.gain_code;
            p_frame_collector->p_frame->p_data[channel_num].ipd_pa = p_gh_data->ppg_bg_data.ipd_pa;
            break;
#endif

#if (1 == GH_CHIP_PPG_DEBUG0)
        case GH_PPG_PARAM_DATA:

            // rawdata
            p_frame_collector->p_frame->p_data[channel_num].rawdata = p_gh_data->ppg_param_data.param_rawdata.rawdata;

            // agc_info
            p_frame_collector->p_frame->p_data[channel_num].agc_info.gain_code =
                p_gh_data->ppg_param_data.param_rawdata.param.gain_code;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.bg_cancel_range =
                p_gh_data->ppg_param_data.param_rawdata.param.bg_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.dc_cancel_range =
                p_gh_data->ppg_param_data.param_rawdata.param.dc_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.dc_cancel_code =
                p_gh_data->ppg_param_data.param_rawdata.param.dc_cancel_code;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.bg_cancel_code =
                p_gh_data->ppg_param_data.param_rawdata.param.bg_cancel_range;
            p_frame_collector->p_frame->p_data[channel_num].agc_info.tia_gain =
                p_gh_data->ppg_param_data.param_rawdata.param.gain_code;
            p_frame_collector->p_frame->p_data[channel_num].flag.skip_ok_flag =
                p_gh_data->ppg_param_data.param_rawdata.param.skip_ok_flag;
            break;
#endif

#if (1 == GH_CHIP_PPG_DEBUG2_DRE)
        case GH_PPG_DRE_DATA:

            // rawdata
            p_frame_collector->p_frame->p_data[channel_num].rawdata = p_gh_data->ppg_dre_data.rawdata;
            p_frame_collector->p_frame->p_data[channel_num].flag.dre_update = p_gh_data->ppg_dre_data.dre_update;
            break;
#endif

        case GH_CAP_DATA:

            // rawdata
            p_frame_collector->p_frame->p_data[channel_num].rawdata = p_gh_data->cap_data.rawdata;
            break;

        default:
            break;
    } // switch (p_gh_data->data_channel.data_type)

    uint32_t rawdata = p_frame_collector->p_frame->p_data[channel_num].rawdata;
    p_frame_collector->p_frame->p_data[channel_num].rawdata = GH_24BIT_TO_32BIT(rawdata);
    p_frame_collector->p_frame->fifo_end_flag = p_gh_data->common_data.fifo_end;

    //update frame num and channel bits
    if (0 == p_frame_collector->frame_cnt
        && (p_frame_collector->frame_num > p_gh_data->common_data.data_cnt
        || 0 == p_frame_collector->frame_num))
    {
        p_frame_collector->frame_num = p_gh_data->common_data.data_cnt;
    }
    if ((channel_num / GH_BYTE_SIZE) < GH_COUNT_SIZE(p_frame_collector->channel_bits))
    {
        p_frame_collector->channel_bits[channel_num / GH_BYTE_SIZE] |= (1 << (channel_num % GH_BYTE_SIZE));
    }

    return ret;
}

/**
 * @fn uint32_t gh_data_sync_fusion_pro(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data)
 * @brief sync fusion process
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] p_gh_data: pointer of gh data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
static uint32_t gh_data_sync_fusion_pro(gh_data_sync_fusion_t *p_sync_fusion, gh_data_t *p_gh_data)
{
    uint32_t ret = GH_SYNC_FUSION_OK;

    if (GH_NULL_PTR == p_sync_fusion || GH_NULL_PTR == p_gh_data)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SYNC_FUSION_PTR_NULL);
    }

    uint8_t match_cnt = 0;
    for (uint8_t id = 0; id < p_sync_fusion->sync_collector_num; id++)
    {
        gh_data_sync_collector_t *p_frame_collector = &p_sync_fusion->p_sync_collector_list[id];
        uint8_t channel_num = 0;
        uint32_t channel_ret = gh_check_channel_map(p_frame_collector, p_gh_data, &channel_num);

        if (GH_SYNC_FUSION_OK == channel_ret)
        {
            gh_update_frame_collector(p_frame_collector, p_gh_data, channel_num);
            if (p_frame_collector->p_frame->ch_num
                == gh_pop_count(p_frame_collector->channel_bits, GH_COUNT_SIZE(p_frame_collector->channel_bits)))
            {
                gh_sync_fusion_frame_handle(p_sync_fusion, id, p_gh_data);
            }
            match_cnt++;
        }
    }

    if (p_gh_data->common_data.fifo_end)
    {
        DEBUG_LOG("%s: fifo end\n", __FUNCTION__);
        for (uint8_t id = 0; id < p_sync_fusion->sync_collector_num; id++)
        {
            gh_data_sync_collector_t *p_frame_collector = &p_sync_fusion->p_sync_collector_list[id];
            p_frame_collector->frame_cnt = 0;
            p_frame_collector->frame_num = 0;
        }
        p_sync_fusion->gsensor_data_num = 0;
    }

    if (0 == match_cnt)
    {
        WARNING_LOG("%s: no match\n", __FUNCTION__);
    }

    return ret;
}

/**
 * @fn int16_t gh_gs_extrapolate(int16_t data0, int16_t data1, uint16_t last_idx, uint32_t idx)
 * @brief gsensor data extrapolation
 * @param[in] data0: second last data
 * @param[in] data1: last data
 * @param[in] last_idx: last index
 * @param[in] idx: current index
 * @param[out] None
 * @return extrapolated data
 *
 * @note None
 */
static int16_t gh_gs_extrapolate(int16_t data0, int16_t data1, uint16_t last_idx, uint32_t idx)
{
    int32_t diff = data1 - data0;
    int32_t extrapolation_factor = (int32_t)idx - ((int32_t)last_idx << GH_FUSION_FIX_POINT);
    return data1 + ((diff * extrapolation_factor) >> GH_FUSION_FIX_POINT);
}

/**
 * @fn uint32_t gh_sync_fusion_frame_handle(gh_data_sync_fusion_t *p_sync_fusion, uint8_t func_id, gh_data_t *p_gh_data)
 * @brief frame handle
 * @param[in] p_sync_fusion: pointer of sync fusion
 * @param[in] func_id: function id
 * @param[in] p_gh_data: pointer of gh data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
static uint32_t gh_sync_fusion_frame_handle(gh_data_sync_fusion_t *p_sync_fusion, uint8_t func_id,
                                            gh_data_t *p_gh_data)
{
    gh_data_sync_collector_t *p_frame_collector = &p_sync_fusion->p_sync_collector_list[func_id];
    gh_func_frame_t *p_frame = p_frame_collector->p_frame;

    // cal gsensor step
    uint16_t gh_frame_num;
    uint16_t gh_frame_cnt;
    uint32_t step;
    uint32_t idx;

    gh_frame_num = p_sync_fusion->p_sync_collector_list[func_id].frame_num;
    gh_frame_cnt = p_sync_fusion->p_sync_collector_list[func_id].frame_cnt;
    if (p_frame->gsensor_en)
    {
        step = (p_sync_fusion->gsensor_data_num << GH_FUSION_FIX_POINT) / gh_frame_num;
        idx = gh_frame_cnt * step;
        DEBUG_LOG("%s:ID = %d, frame_num = %d, frame_cnt = %d, idx = %d, step = %d, GS num = %d\n",
                  __FUNCTION__, func_id, gh_frame_num, gh_frame_cnt, idx >> GH_FUSION_FIX_POINT,
                  step >> GH_FUSION_FIX_POINT, p_sync_fusion->gsensor_data_num);
    }

    if (p_frame->gsensor_en && (p_sync_fusion->gsensor_data_num > ((idx >> GH_FUSION_FIX_POINT) + 1)))
    {
        for (uint8_t i = 0; i < GH_ACC_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.acc[i] =
                (((int32_t)p_sync_fusion->p_gsensor_data[idx >> GH_FUSION_FIX_POINT].acc[i]
                * ((1 << GH_FUSION_FIX_POINT) - (idx & GH_FUSION_FIX_POINT_MASK))
                + (int32_t)p_sync_fusion->p_gsensor_data[(idx >> GH_FUSION_FIX_POINT) + 1].acc[i]
                * (idx & GH_FUSION_FIX_POINT_MASK)) + (1 << (GH_FUSION_FIX_POINT - 1)))
                >> GH_FUSION_FIX_POINT;
        }
#if GH_GYRO_EN
        for (uint8_t i = 0; i < GH_GYRO_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.gyro[i] =
                (((int32_t)p_sync_fusion->p_gsensor_data[idx >> GH_FUSION_FIX_POINT].gyro[i]
                * ((1 << GH_FUSION_FIX_POINT) - (idx & GH_FUSION_FIX_POINT_MASK))
                + (int32_t)p_sync_fusion->p_gsensor_data[(idx >> GH_FUSION_FIX_POINT) + 1].gyro[i]
                * (idx & GH_FUSION_FIX_POINT_MASK)) + (1 << (GH_FUSION_FIX_POINT - 1)))
                >> GH_FUSION_FIX_POINT;
        }
#endif
    }
    else if (p_frame->gsensor_en
             && (p_sync_fusion->gsensor_data_num > 1))
    {
        // Extrapolation - use the last two data points for extrapolation
        for (uint8_t i = 0; i < GH_ACC_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.acc[i] =
                gh_gs_extrapolate(p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - GH_NUM_TWO].acc[i],
                                  p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - 1].acc[i],
                                  p_sync_fusion->gsensor_data_num - 1,
                                  idx);
        }
#if GH_GYRO_EN
        for (uint8_t i = 0; i < GH_GYRO_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.gyro[i] =
                gh_gs_extrapolate(p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - GH_NUM_TWO].gyro[i],
                                  p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - 1].gyro[i],
                                  p_sync_fusion->gsensor_data_num - 1,
                                  idx);
        }
#endif
    } //else if (p_frame->gsensor_en && (p_sync_fusion->gsensor_data_num > 1))
    else if (p_frame->gsensor_en && (p_sync_fusion->gsensor_data_num == 1))
    {
        for (uint8_t i = 0; i < GH_ACC_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.acc[i] = p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - 1].acc[i];
        }
#if GH_GYRO_EN
        for (uint8_t i = 0; i < GH_GYRO_AXIS_NUM; i++)
        {
            p_frame->gsensor_data.gyro[i] = p_sync_fusion->p_gsensor_data[p_sync_fusion->gsensor_data_num - 1].gyro[i];
        }
#endif
    } //else if (p_frame->gsensor_en && (p_sync_fusion->gsensor_data_num == 1))

    // update frame
    p_frame->frame_cnt++;

    uint64_t time_stamp_step = (((uint64_t)p_gh_data->common_data.timestamp_end
                               - (uint64_t)p_gh_data->common_data.timestamp_begin)
                               << GH_FUSION_FIX_POINT) / gh_frame_num;

    p_frame->timestamp = p_gh_data->common_data.timestamp_begin
                         + ((time_stamp_step * gh_frame_cnt) >> GH_FUSION_FIX_POINT);

    if (p_frame_collector->frame_cnt + 1 == p_frame_collector->frame_num)
    {
        p_frame->fifo_end_flag = 1;
    }


#if (1 == GH_STACK_INFO_EN)
    uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame);
    gh_app_manager_fusion_callback(p_sync_fusion->parent_node, p_frame);
#else
    if (GH_NULL_PTR != p_sync_fusion->frame_sync_publish)
    {
        p_sync_fusion->frame_sync_publish(p_sync_fusion->parent_node, p_frame);
    }
#endif

    p_frame_collector->frame_cnt++;
    gh_memset(p_frame_collector->channel_bits, 0, sizeof(p_frame_collector->channel_bits));
    return GH_SYNC_FUSION_OK;
}
