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
#include "gh_data_fusion.h"

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

// #define GH_32BIT_TIMESTAMP_A_IS_SMALLER(a, b)        (((((b) - (a)) < 0x80000000)) && ((a) != (b)))
// #define GH_32BIT_TIMESTAMP_A_IS_NOT_BIGGER(a, b)     (((((b) - (a)) < 0x80000000)) || ((a) == (b)))
#define GH_24BIT_TO_32BIT(X)                         (((X) & 0x800000) ? ((X) | 0xFF000000) : (X))
#define GH_ABS_U64(x, y)        ((x) > (y) ? ((x) - (y)) : ((y) - (x)))

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static void gh_collector_msg_process(gh_data_collector_t *this,
                                     gh_sensor_buffer_msg_e msg,
                                     gh_sensor_buffer_t *sensor_buf);
static void gh_collector_ghealth_data_pro(gh_data_collector_t *this,
                                          gh_sensor_buffer_msg_e msg,
                                          gh_sensor_buffer_t *sensor_buf);

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
uint32_t gh_sensor_buffer_init(gh_sensor_buffer_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }
    gh_memset((void *)this, 0, sizeof(gh_sensor_buffer_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_sensor_buffer_deinit(gh_sensor_buffer_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }
    gh_memset((void *)this, 0, sizeof(gh_sensor_buffer_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

void gh_sens_buf_cllct_list_register(gh_sensor_buffer_t *this, gh_data_collector_t **list)
{
    this->collector_list = list;
}

void gh_sensor_buffer_msg_distribute(gh_sensor_buffer_t *this, gh_sensor_buffer_msg_e msg)
{
    uint32_t func_cnt;

    for (func_cnt = 0; func_cnt < (uint32_t)GH_FUNC_FIX_IDX_MAX; func_cnt++)
    {
        if (this->collector_list[func_cnt])
        {
            gh_collector_msg_process(this->collector_list[func_cnt], msg, this);
        }
    }
}

#if  GH_GSENSOR_FUSION_EN
static void gh_sens_buf_gsensor_data_push(gh_sensor_buffer_t *this, gh_gsensor_ts_and_data_t *gsensor_data)
{
    gh_sensor_buffer_msg_e msg = GH_MSG_NEW_GSENOR_DATA;

    /* queue is full, it must dequeue firstly */
    if (this->gsensor_buffer.len >= GH_ASYNC_GSENSOR_QUEUE_DEPTH)
    {
        this->gsensor_buffer.head_index++;
        if (this->gsensor_buffer.head_index >= GH_ASYNC_GSENSOR_QUEUE_DEPTH)
        {
            this->gsensor_buffer.head_index = 0;
        }
        this->gsensor_buffer.len--;
        msg = GH_MSG_NEW_GSENOR_DATA_OLD_DEQ;
    }

    gh_memcpy((void*)&(this->gsensor_buffer.queue_data[this->gsensor_buffer.tail_index]),
              (void*)gsensor_data, sizeof(gh_gsensor_ts_and_data_t));

    this->gsensor_buffer.tail_index++;

    if (this->gsensor_buffer.tail_index >= GH_ASYNC_GSENSOR_QUEUE_DEPTH)
    {
        this->gsensor_buffer.tail_index = 0;
    }

    this->gsensor_buffer.len++;

    /* send messange (sync process) firstly */
    gh_sensor_buffer_msg_distribute(this, msg);
}

static void gh_sens_buf_gs_queue_len_get(gh_sensor_buffer_t *this, uint16_t *len)
{
    len[0] = this->gsensor_buffer.len;
}

/// @brief get one sample gsensor data from gsensor buffer
/// @param this sensor buffer module
/// @param head_offset queue offset which relate head;
/// head_offset = 0 means this data is oldest data in queue
/// head_offset = this->gsensor_buffer.len - 1 means this data is newest data in queue
/// @param gsensor_data output point of gsensor data
/// @return error code
static uint32_t gh_sens_buf_gsensor_data_read(gh_sensor_buffer_t *this,
                                              uint16_t head_offset,
                                              gh_gsensor_ts_and_data_t *gsensor_data)
{
    /* get head index firstly */
    uint16_t temp_head_index = this->gsensor_buffer.head_index;

    if (head_offset >= this->gsensor_buffer.len)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_READ_DATA_ERR);
    }

    temp_head_index += head_offset;
    if (temp_head_index >= GH_ASYNC_GSENSOR_QUEUE_DEPTH)
    {
        temp_head_index -= GH_ASYNC_GSENSOR_QUEUE_DEPTH;
    }

    gh_memcpy((void *)gsensor_data,
              (void *)&(this->gsensor_buffer.queue_data[temp_head_index]),
              sizeof(gh_gsensor_ts_and_data_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}
#endif

static void gh_sens_buf_ghealth_buf_reset(gh_sensor_buffer_t *this)
{
    this->ghealth_buffer.sample_cnt = 0;
    this->ghealth_buffer.last_sample_is_recieved = 0;

    for (uint8_t cfg_cnt = 0; cfg_cnt < GH_PPG_CFG_NUM; cfg_cnt++)
    {
        this->ghealth_buffer.ppg_tx_param[cfg_cnt].debug_pack_update_flag = 0;
    }

    for (uint8_t chnl_cnt = 0; chnl_cnt < GH_PPG_CHNL_NUM; chnl_cnt++)
    {
        this->ghealth_buffer.ppg_rx_param[chnl_cnt].debug_pack_update_flag = 0;
    }
}

uint32_t gh_sens_buf_ghealth_data_push(gh_sensor_buffer_t *this, gh_data_t *ghealth_data)
{
    uint8_t fifo_end = ghealth_data[0].common_data.fifo_end;
    gh_hal_data_channel_t chnl_map = ghealth_data[0].data_channel;

    if ((GH_NULL_PTR == this) || (GH_NULL_PTR == ghealth_data))
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    /**********************parse ghealth_data************************************/

    /* data of new fifo read period will be pushed */
    if (this->ghealth_buffer.last_sample_is_recieved)
    {
        DEBUG_LOG("[ghealth_data_push] send msg: GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR\r\n");

        /* send messange (sync process) firstly */
        gh_sensor_buffer_msg_distribute(this, GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR);
        gh_sens_buf_ghealth_buf_reset(this);
    }

    /* fill tx and rx param */
#if 0
    if (GH_PPG_PARAM_DATA  == ghealth_data[0].data_channel.data_type)
    {
        uint8_t cfg_index = ghealth_data[0].data_channel.channel_ppg_param.slot_cfg_id;
        uint8_t chnl_index = cfg_index* GH_PPG_RX_NUM + ghealth_data[0].data_channel.channel_ppg_param.rx_id;
        this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.bg_cancel_range
             = ghealth_data[0].ppg_param_data.bg_cancel_code;
        this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.dc_cancel_range
             = ghealth_data[0].ppg_param_data.dc_cancel_code;
        this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.tia_gain
             = ghealth_data[0].ppg_param_data.gain_code;
        this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.bg_cancel_range
             = ghealth_data[0].ppg_param_data.bg_cancel_range;
        this->ghealth_buffer.ppg_rx_param->debug_pack_update_flag = 1;
    }
#endif

    /* fill phy_info and tx&rx param */
    if ((GH_PPG_DATA == ghealth_data[0].data_channel.data_type)
#if (1 == GH_RAWDATA_CAP_PRO_EN)
        || (GH_CAP_DATA == ghealth_data[0].data_channel.data_type)
#endif
#if (1 == GH_CHIP_PPG_DATA1)
        || (GH_PPG_MIX_DATA == ghealth_data[0].data_channel.data_type)
#endif
#if ((1 == GH_CHIP_PPG_DEBUG1_BGDATA0) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA1) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA2))
        || (GH_PPG_BG_DATA == ghealth_data[0].data_channel.data_type)
#endif
#if (1 == GH_CHIP_PPG_DEBUG2_DRE)
        || (GH_PPG_DRE_DATA == ghealth_data[0].data_channel.data_type)
#endif
#if (1 == GH_CHIP_PPG_DEBUG0)
        || (GH_PPG_PARAM_DATA == ghealth_data[0].data_channel.data_type)
#endif
#if (1 == GH_CHIP_PPG_DC_INFO)
        || (GH_PPG_DRE_DC_INFO == ghealth_data[0].data_channel.data_type)
#endif
        )
    {
        uint32_t adc_code = 0;
        uint32_t pysical_data = 0;

        /* fatal! data is too more !!!! */
        if ((this->ghealth_buffer.sample_cnt + 1) > GH_FIFO_READ_MAX_NUM)
        {
            gh_sens_buf_ghealth_buf_reset(this);
        }

        if (GH_PPG_DATA == ghealth_data[0].data_channel.data_type
#if (1 == GH_CHIP_PPG_DC_INFO)
            || GH_PPG_DRE_DC_INFO == ghealth_data[0].data_channel.data_type
#endif
            )
        {
            uint8_t cfg_index = ghealth_data[0].data_channel.channel_ppg.slot_cfg_id;
            uint8_t chnl_index = cfg_index * GH_PPG_RX_NUM + ghealth_data[0].data_channel.channel_ppg.rx_id;

            /* adc code and pysical value */
            adc_code = ghealth_data[0].ppg_data.rawdata;
            pysical_data = ghealth_data[0].ppg_data.ipd_pa;

            /* some flag */
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].led_adj_flag =
                        ghealth_data[0].ppg_data.led_adj_flag;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].sa_flag =
                        ghealth_data[0].ppg_data.sa_flag;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].param_change_flag =
                        ghealth_data[0].ppg_data.param_change_flag;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].dre_update =
                        ghealth_data[0].ppg_data.dre_update;

            /* some sample params */
            this->ghealth_buffer.ppg_tx_param[cfg_index].tx_param.led_drv[0] =
                        ghealth_data[0].ppg_data.led_drv[0];
            this->ghealth_buffer.ppg_tx_param[cfg_index].tx_param.led_drv[1] =
                        ghealth_data[0].ppg_data.led_drv[1];
            this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.bg_cancel_range =
                        ghealth_data[0].ppg_data.bg_cancel_range;
            this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.dc_cancel_range =
                        ghealth_data[0].ppg_data.dc_cancel_range;
            this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.tia_gain =
                        ghealth_data[0].ppg_data.gain_code;
            this->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.dc_cancel_code =
                        ghealth_data[0].ppg_data.dc_cancel_code;

            /* led drv fs */
            for (uint8_t drv_cnt = 0; drv_cnt < GH_LED_DRV_NUM; drv_cnt++)
            {
                this->ghealth_buffer.led_drv_fs[drv_cnt] =
                        ghealth_data[0].ppg_data.led_drv_fs[drv_cnt];
            }
        }  // if (GH_PPG_DATA  == ghealth_data[0].data_channel.data_type)...
#if (1 == GH_CHIP_PPG_DATA1)
        if (GH_PPG_MIX_DATA == ghealth_data[0].data_channel.data_type)
        {
            adc_code = ghealth_data[0].ppg_mixdata.rawdata;
            pysical_data = ghealth_data[0].ppg_mixdata.ipd_pa;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].sa_flag =
                        ghealth_data[0].ppg_mixdata.sa_flag;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].param_change_flag =
                        ghealth_data[0].ppg_mixdata.param_change_flag;
        }
#endif

#if ((1 == GH_CHIP_PPG_DEBUG1_BGDATA0) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA1) || (1 == GH_CHIP_PPG_DEBUG1_BGDATA2))
        if (GH_PPG_BG_DATA == ghealth_data[0].data_channel.data_type)
        {
            adc_code = ghealth_data[0].ppg_bg_data.rawdata;
            pysical_data = ghealth_data[0].ppg_bg_data.ipd_pa;
        }
#endif
#if  GH_RAWDATA_CAP_PRO_EN
        if (GH_CAP_DATA == ghealth_data[0].data_channel.data_type)
        {
            adc_code = ghealth_data[0].cap_data.rawdata;
        }
#endif
#if (1 == GH_CHIP_PPG_DEBUG2_DRE)
        if (GH_PPG_DRE_DATA == ghealth_data[0].data_channel.data_type)
        {
            adc_code = ghealth_data[0].ppg_dre_data.rawdata;
            this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].dre_update
                        = ghealth_data[0].ppg_dre_data.dre_update;
        }
#endif
#if (1 == GH_CHIP_PPG_DEBUG0)
        if (GH_PPG_PARAM_DATA  == ghealth_data[0].data_channel.data_type)
        {
            adc_code = ghealth_data[0].ppg_param_data.param_rawdata.rawdata;
        }
#endif
        this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].adc_code = adc_code;
        this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].physical_value = pysical_data;
        this->ghealth_buffer.phy_info[this->ghealth_buffer.sample_cnt].channel_map = chnl_map;
        this->ghealth_buffer.sample_cnt++;
    }   // if ((GH_PPG_DATA  == ghealth_data[0].data_channel.data_type)...

    /* fill timestamp and send messange */
    if (fifo_end)
    {
        this->ghealth_buffer.last_sample_is_recieved = 1;
        this->ghealth_buffer.timestamp_begin = ghealth_data[0].ppg_data.timestamp_begin;
        this->ghealth_buffer.timestamp_end = ghealth_data[0].ppg_data.timestamp_end;
        DEBUG_LOG("[ghealth_data_push] send msg: GH_MSG_NEW_GHEALTH_DATA\r\n");

        /* send messange (sync process) firstly */
        gh_sensor_buffer_msg_distribute(this, GH_MSG_NEW_GHEALTH_DATA);
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_collector_init(gh_data_collector_t *this,
                           gh_func_frame_t *frame,
                           gh_frame_publish_t pulish_func,
                           void *parent_node)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_data_collector_t));
    this->frame = frame;
    this->frame_publish = pulish_func;
    this->p_parent_node = parent_node;

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_collector_deinit(gh_data_collector_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_data_collector_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

static void gh_collector_ctrl(gh_data_collector_t *this, gh_data_collector_ctrl_e ctrl_option)
{
    if (GH_DATA_COLLECTOR_CTRL_DISABLE == ctrl_option)
    {
        gh_func_frame_t *frame_bak = this->frame;
        gh_frame_publish_t frame_publish_bak = this->frame_publish;
        void *parent_node = this->p_parent_node;

        gh_memset((void *)this, 0, sizeof(gh_data_collector_t));
        this->frame = frame_bak;
        this->frame_publish = frame_publish_bak;
        this->p_parent_node = parent_node;

        if (GH_NULL_PTR != this->frame)
        {
            this->frame->frame_cnt = 0;
        }
        this->state = GH_DATA_COLLECTOR_STATE_DISABLE;
    }
    else if (GH_DATA_COLLECTOR_CTRL_ENABLE == ctrl_option)
    {
        if (GH_DATA_COLLECTOR_STATE_DISABLE == this->state)
        {
            this->state = GH_DATA_COLLECTOR_STATE_WATT_NEW_FIFO_MSG;
        }
    }
}

static uint8_t gh_collector_chnl_index_find(gh_hal_data_channel_t chnl_map_of_this_sample,
                                            gh_hal_data_channel_t *function_chnl_map,
                                            uint16_t chnl_num)
{
    uint8_t search_cnt;
    uint8_t search_result = GH_INVALID_CHNL_INDEX;

    for (search_cnt = 0; search_cnt < chnl_num; search_cnt++)
    {
        if (function_chnl_map[search_cnt].channel == chnl_map_of_this_sample.channel)
        {
            search_result = search_cnt;
            break;
        }
    }

    return search_result;
}

static uint16_t gh_collector_chnl0_spl_num_get(gh_hal_data_channel_t chnl0,
                                               gh_chnl_phy_info_t *chnl_phy_info,
                                               uint16_t sample_cnt)
{
    uint16_t search_cnt;
    uint16_t search_result = 0;

    for (search_cnt = 0; search_cnt < sample_cnt; search_cnt++)
    {
        if (chnl_phy_info[search_cnt].channel_map.channel == chnl0.channel)
        {
            search_result++;
        }
    }

    return search_result;
}

#if  GH_GSENSOR_FUSION_EN
static int16_t linear_interpolate(uint32_t x0, uint32_t x1, int16_t y0, int16_t y1, uint32_t x_target)
{
    int64_t x_diff;
    int64_t x_diff_target;
    int32_t y_diff;

    if (x1 <= x_target)
    {
        return y1;
    }
    else if (x0 >= x_target)
    {
        return y0;
    }

    x_diff = (int64_t)x1 - (int64_t)x0;
    y_diff = (int32_t)y1 - (int32_t)y0;
    x_diff_target = (int64_t)x_target - (int64_t)x0;

#if 0
    DEBUG_LOG("y0 = %d, y1 = %d, x_diff = %d, y_diff = %d, x_diff_target = %d.\r\n",
              (int32_t)y0,
              (int32_t)y1,
              (int32_t)x_diff,
              (int32_t)y_diff,
              (int32_t)x_diff_target);
#endif

    return (int16_t)(y0 + ((int64_t)(y_diff * x_diff_target) / x_diff));
}

static void gsensor_data_linear_interpolate(gh_gsensor_ts_and_data_t *left_gsensor_data,
                                            uint64_t right_timestamp,
                                            gh_func_frame_t *frame)
{
    uint32_t x1 = 0;
    uint32_t x_target = 0;
    int16_t y0 = 0;
    int16_t y1 = 0;

    /* use relative timestamp to prevent 32bit timestamp overflow */
    x1 = (uint32_t)(right_timestamp - left_gsensor_data->timestamp);
    x_target = (uint32_t)(frame->timestamp - left_gsensor_data->timestamp);

    for (uint8_t axis_cnt = 0; axis_cnt < GH_ACC_AXIS_NUM; axis_cnt++)
    {
        y0 = left_gsensor_data->data.acc[axis_cnt];
        y1 = frame->gsensor_data.acc[axis_cnt];
        frame->gsensor_data.acc[axis_cnt] = linear_interpolate(0, x1, y0, y1, x_target);
    }

#if GH_GYRO_EN
    for (uint8_t axis_cnt = 0; axis_cnt < GH_GYRO_AXIS_NUM; axis_cnt++)
    {
        y0 = left_gsensor_data->data.gyro[axis_cnt];
        y1 = frame->gsensor_data.gyro[axis_cnt];
        frame->gsensor_data.gyro[axis_cnt] = linear_interpolate(0, x1, y0, y1, x_target);
    }
#endif
}

static gh_collector_gs_sync_ret_e gh_collector_gsensor_sync(gh_data_collector_t *this,
                                                            gh_gsensor_ts_and_data_t *gsensor_data,
                                                            uint16_t gsensor_buf_offset,
                                                            gh_collector_gs_sync_type_e sync_type)
{
    gh_collector_gs_sync_ret_e sync_result = GH_COLLECTOR_GS_SYNC_FAIL;
    uint64_t ghealth_timestamp = this->frame->timestamp;
    uint64_t right_timestamp = 0;

//    DEBUG_LOG("[gsensor_async] gesnor ts = %llu. offset = %d, sync_type = %d.\r\n",
//              gsensor_data->timestamp,
//              (int32_t)gsensor_buf_offset,
//              (int32_t)sync_type);
    if (0 == this->gsensor_left_point_valid_flag)
    {
//        DEBUG_LOG("[gsensor_async] set left point data firstly.\r\n");
        gh_memcpy((void*)(&(this->gsensor_left_point_data)), (void *)gsensor_data, sizeof(gh_gsensor_ts_and_data_t));
        this->gsensor_left_point_valid_flag = 1;
        this->gsensor_search_offset = gsensor_buf_offset;
    }

    if (GH_DATA_COLLECTOR_STATE_WAIT_GS_DATA != this->state)
    {
//        DEBUG_LOG("[gsensor_sync] this gsensor sample is not needed currently.\r\n");
        return sync_result;
    }

    /* check it is better left gsensor point or not */
    if (gsensor_data->timestamp < ghealth_timestamp)
    {
        if ((this->gsensor_left_point_data.timestamp < gsensor_data->timestamp)
            ||(GH_ABS_U64(gsensor_data->timestamp, ghealth_timestamp)
            < GH_ABS_U64(this->gsensor_left_point_data.timestamp, ghealth_timestamp)))
        {
//            DEBUG_LOG("[gsensor_async] found better left point.\r\n");
            gh_memcpy((void *)(&(this->gsensor_left_point_data)),
                      (void *)gsensor_data,
                      sizeof(gh_gsensor_ts_and_data_t));
            this->gsensor_search_offset = gsensor_buf_offset;
        }
    }

    /* check it is right gsensor point or not */
    if (ghealth_timestamp <= gsensor_data->timestamp)
    {
        gh_memcpy((void *)(&(this->frame->gsensor_data)), (void *)(&(gsensor_data->data)), sizeof(gh_gsensor_data_t));
        right_timestamp = gsensor_data->timestamp;
        this->gsensor_search_offset = gsensor_buf_offset;
        sync_result = GH_COLLECTOR_GS_SYNC_OK;
    }
    else if (GH_COLLECTOR_GS_SYNC_FORCE == sync_type) // update newest gsensor data
    {
        gh_memcpy((void*)(&(this->frame->gsensor_data)),
                  (void*)(&(this->gsensor_left_point_data.data)),
                  sizeof(gh_gsensor_data_t));
        right_timestamp = this->gsensor_left_point_data.timestamp;
        sync_result = GH_COLLECTOR_GS_SYNC_OK;
    }

    if (GH_COLLECTOR_GS_SYNC_OK == sync_result)
    {
        /* store gsensor left timestamp and right timestamp for debug */
#if GH_GSENSOR_DEBUG_EN
        this->frame->gs_left_ts = this->gsensor_left_point_data.timestamp;
        this->frame->gs_right_ts = right_timestamp;
#endif

        /* linear interpolate and fill gsensor dater into frame */
        gsensor_data_linear_interpolate(&(this->gsensor_left_point_data), right_timestamp, this->frame);
    }

#if 0
    if (GH_COLLECTOR_GS_SYNC_OK == sync_result)
    {
        DEBUG_LOG("[gsensor_sync] sync ok, ghealth ts = %llu, gs left ts = %llu, gs right ts = %llu\r\n",
                  ghealth_timestamp,
                  this->gsensor_left_point_data.timestamp,
                  right_timestamp);
    }
    else
    {
        DEBUG_LOG("[gsensor_sync] sync fail, ghealth ts = %llu, gs left ts = %llu, cur_gs_ts = %llu\r\n",
                  ghealth_timestamp,
                  this->gsensor_left_point_data.timestamp,
                  gsensor_data->timestamp);
    }
#endif

    return sync_result;
}

static gh_collector_gs_sync_ret_e gh_collector_gsensor_data_scan(gh_data_collector_t *this,
                                                                 gh_sensor_buffer_t *sensor_buf,
                                                                 gh_collector_gs_sync_type_e sync_type)
{
    uint16_t gsensor_buf_sample_num;
    gh_gsensor_ts_and_data_t gsensor_data;
    gh_collector_gs_sync_ret_e sync_result;

    gh_sens_buf_gs_queue_len_get(sensor_buf, &gsensor_buf_sample_num);
    if ((GH_COLLECTOR_GS_SYNC_FORCE == sync_type) && (0 == gsensor_buf_sample_num))
    {
        gh_memset((void *)&gsensor_data, 0, sizeof(gh_gsensor_ts_and_data_t));
        sync_result = gh_collector_gsensor_sync(this,
                                                &gsensor_data,
                                                this->gsensor_search_offset,
                                                GH_COLLECTOR_GS_SYNC_FORCE);
        return GH_COLLECTOR_GS_SYNC_OK;
    }
    else
    {
        for (uint16_t gsensor_read_cnt = this->gsensor_search_offset;
            gsensor_read_cnt < gsensor_buf_sample_num;
            gsensor_read_cnt++)
        {
            gh_sens_buf_gsensor_data_read(sensor_buf, gsensor_read_cnt, &gsensor_data);
            if ((gsensor_read_cnt == (gsensor_buf_sample_num - 1)) && (GH_COLLECTOR_GS_SYNC_FORCE == sync_type))
            {
                sync_result = gh_collector_gsensor_sync(this,
                                                        &gsensor_data,
                                                        gsensor_read_cnt,
                                                        GH_COLLECTOR_GS_SYNC_FORCE);
            }
            else
            {
                sync_result = gh_collector_gsensor_sync(this,
                                                        &gsensor_data,
                                                        gsensor_read_cnt,
                                                        GH_COLLECTOR_GS_SYNC_NORMAL);
            }

            /* frame is read, include gsensor data */
            if (GH_COLLECTOR_GS_SYNC_OK == sync_result)
            {
                return GH_COLLECTOR_GS_SYNC_OK;
            }
        }
    }  //else there is some ghealth data in buffer
    return GH_COLLECTOR_GS_SYNC_FAIL;
}
#endif

static void gh_cllct_frame_out_and_post_pro(gh_data_collector_t *this)
{
#if (1 == GH_STACK_INFO_EN)
    uint32_t gh_app_manager_fusion_callback(void *p_parent_node, gh_func_frame_t *p_frame);
    gh_app_manager_fusion_callback(this->p_parent_node, this->frame);
#else
    this->frame_publish(this->p_parent_node, this->frame);
#endif
    this->frame->frame_cnt++;
    this->chnl_collect_flag = 0;
    this->frame->fifo_end_flag = 0;
}

#if  GH_GSENSOR_FUSION_EN
static void gh_collector_gsensor_data_proc(gh_data_collector_t *this,
                                           gh_sensor_buffer_msg_e msg,
                                           gh_sensor_buffer_t *sensor_buf)
{
    uint16_t gsensor_buf_sample_num;
    gh_gsensor_ts_and_data_t gsensor_data;
    gh_collector_gs_sync_ret_e sync_result;

    if (GH_DATA_COLLECTOR_STATE_DISABLE == this->state)
    {
        return;
    }

    if (this->frame->gsensor_en)
    {

        /* update offset because of oldest data dequeue */
        if (GH_MSG_NEW_GSENOR_DATA_OLD_DEQ == msg)
        {
            if (this->gsensor_search_offset > 0)
            {
                this->gsensor_search_offset--;
            }
        }
        gh_sens_buf_gs_queue_len_get(sensor_buf, &gsensor_buf_sample_num);
        gh_sens_buf_gsensor_data_read(sensor_buf, gsensor_buf_sample_num - 1, &gsensor_data);
        sync_result = gh_collector_gsensor_sync(this,
                                                &gsensor_data,
                                                gsensor_buf_sample_num - 1,
                                                GH_COLLECTOR_GS_SYNC_NORMAL);

        /* frame is read, include gsensor data */
        if (GH_COLLECTOR_GS_SYNC_OK == sync_result)
        {
            gh_cllct_frame_out_and_post_pro(this);
            this->state = GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA;
            gh_collector_ghealth_data_pro(this, msg, sensor_buf);
        }
    }
}
#endif

static void gh_collector_msg_process(gh_data_collector_t *this,
                                     gh_sensor_buffer_msg_e msg,
                                     gh_sensor_buffer_t *sensor_buf)
{
    if ((GH_MSG_NEW_GHEALTH_DATA == msg) || (GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR == msg))
    {
        gh_collector_ghealth_data_pro(this, msg, sensor_buf);
    }
#if  GH_GSENSOR_FUSION_EN
    else if ((GH_MSG_NEW_GSENOR_DATA == msg) || (GH_MSG_NEW_GSENOR_DATA_OLD_DEQ == msg))
    {
        gh_collector_gsensor_data_proc(this, msg, sensor_buf);
    }
#endif
}

static void gh_cllct_new_ghealth_fifo_pre(gh_data_collector_t *this)
{
    this->chnl0_sample_cnt = 0;
    this->ghealth_buf_read_index = 0;
}

static void gh_first_sample_pro(gh_data_collector_t *this,
                                gh_hal_data_channel_t *function_chnl_map,
                                uint8_t function_chnl_num,
                                gh_sensor_buffer_t *sensor_buf)
{
    /* get number of channel0 samples of this fifo read period */
    uint16_t chnl0_num;

//                DEBUG_LOG("[test] 0x%p, 0x%p, 0x%p, %d\r\n", sensor_buf, function_chnl_map[0],
//                    sensor_buf->ghealth_buffer.phy_info, sensor_buf->ghealth_buffer.sample_cnt);

    for (uint8_t chnl_cnt = 0; chnl_cnt < function_chnl_num; chnl_cnt++)
    {
            DEBUG_LOG("[test] chnl_cnt:%d, chnl_map: %d\r\n", chnl_cnt, function_chnl_map[chnl_cnt].channel);
    }
    chnl0_num = gh_collector_chnl0_spl_num_get(function_chnl_map[0],
                                               sensor_buf->ghealth_buffer.phy_info,
                                               sensor_buf->ghealth_buffer.sample_cnt);

    /* calculate timestamp_step via channel0 sample number */
    if (0 != chnl0_num)
    {
        this->timestamp_step = (sensor_buf->ghealth_buffer.timestamp_end
                                - sensor_buf->ghealth_buffer.timestamp_begin)
                                * GH_TIMESTAMPE_STEP_UNIT_RADIO / chnl0_num;
    }
}

static void gh_frame_data_fill(gh_data_collector_t *this,
                               gh_hal_data_channel_t cur_point_chnl_map,
                               gh_hal_data_channel_t *function_chnl_map,
                               uint16_t current_ghealth_read_index,
                               uint8_t search_result,
                               gh_sensor_buffer_t *sensor_buf)
{
    uint8_t cfg_index;
    uint8_t chnl_index;

    /* fill this sample data into framedata */
    this->frame->p_data[search_result].rawdata =
        GH_24BIT_TO_32BIT(sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].adc_code);

    this->frame->p_data[search_result].ipd_pa =
        sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].physical_value;

    if (GH_PPG_DATA == cur_point_chnl_map.data_type
#if (1 == GH_CHIP_PPG_DC_INFO)
        || GH_PPG_DRE_DC_INFO == cur_point_chnl_map.data_type
#endif
        )
    {
        cfg_index = cur_point_chnl_map.channel_ppg.slot_cfg_id;
        chnl_index = cfg_index * GH_PPG_RX_NUM + cur_point_chnl_map.channel_ppg.rx_id;

        //some flag
        this->frame->p_data[search_result].flag.led_adj_flag =
                    sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].led_adj_flag;
        this->frame->p_data[search_result].flag.sa_flag =
                    sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].sa_flag;
        this->frame->p_data[search_result].flag.param_change_flag =
                    sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].param_change_flag;
        this->frame->p_data[search_result].flag.dre_update =
                    sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].dre_update;

        //tx parm
        this->frame->p_data[search_result].agc_info.led_drv0 =
                    sensor_buf->ghealth_buffer.ppg_tx_param[cfg_index].tx_param.led_drv[0];
        this->frame->p_data[search_result].agc_info.led_drv1 =
                    sensor_buf->ghealth_buffer.ppg_tx_param[cfg_index].tx_param.led_drv[1];
        for (uint8_t drv_cnt = 0; drv_cnt < GH_LED_DRV_NUM; drv_cnt++)
        {
            this->frame->led_drv_fs[drv_cnt] = sensor_buf->ghealth_buffer.led_drv_fs[drv_cnt];
        }

        //rx param
        this->frame->p_data[search_result].agc_info.gain_code =
                    sensor_buf->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.tia_gain;
        this->frame->p_data[search_result].agc_info.dc_cancel_code =
                    sensor_buf->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.dc_cancel_code;
        this->frame->p_data[search_result].agc_info.dc_cancel_range =
                    sensor_buf->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.dc_cancel_range;
        this->frame->p_data[search_result].agc_info.bg_cancel_range =
                    sensor_buf->ghealth_buffer.ppg_rx_param[chnl_index].rx_param.bg_cancel_range;
    }  //if (GH_PPG_DATA == cur_point_chnl_map.data_type)...
#if  GH_CHIP_PPG_DEBUG2_DRE
    if (GH_PPG_DRE_DATA == cur_point_chnl_map.data_type)
    {
        this->frame->p_data[search_result].flag.dre_update =
            sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].dre_update;
    }
#endif
#if (1 == GH_CHIP_PPG_DATA1)
    if (GH_PPG_MIX_DATA == cur_point_chnl_map.data_type)
    {
        this->frame->p_data[search_result].flag.sa_flag =
            sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].sa_flag;
        this->frame->p_data[search_result].flag.param_change_flag =
            sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].param_change_flag;
    }
#endif
}

static void gh_collector_ghealth_data_pro(gh_data_collector_t *this,
                                          gh_sensor_buffer_msg_e msg,
                                          gh_sensor_buffer_t *sensor_buf)
{
    uint16_t ghealth_sample_read_num = 0;
    gh_hal_data_channel_t *function_chnl_map;
    uint8_t function_chnl_num;
    uint8_t search_result;
#if  GH_GSENSOR_FUSION_EN
    gh_collector_gs_sync_type_e sync_type = GH_COLLECTOR_GS_SYNC_NORMAL;
#endif
    function_chnl_map = this->frame->p_ch_map;
    function_chnl_num = this->frame->ch_num;

    if (GH_DATA_COLLECTOR_STATE_WATT_NEW_FIFO_MSG == this->state)
    {
        if (GH_MSG_NEW_GHEALTH_DATA == msg)
        {
            this->state = GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA;
        }
    }

    /* no need to process ghealth data */
    if (this->state < GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA)
    {
        return;
    }
#if  GH_GSENSOR_FUSION_EN
    if (GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR == msg)
    {
        sync_type = GH_COLLECTOR_GS_SYNC_FORCE;

        //we must publish frame data immediately when sync_type = force
        //although state is GH_DATA_COLLECTOR_STATE_WAIT_GS_DATA
        if (GH_DATA_COLLECTOR_STATE_WAIT_GS_DATA == this->state)
        {
            gh_cllct_frame_out_and_post_pro(this);
            this->state = GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA;
        }
    }
#endif
    for (uint16_t current_ghealth_read_index = this->ghealth_buf_read_index;
        current_ghealth_read_index < sensor_buf->ghealth_buffer.sample_cnt;
        current_ghealth_read_index++)
    {
        if (GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA == this->state)
        {
            gh_hal_data_channel_t cur_point_chnl_map =
                        sensor_buf->ghealth_buffer.phy_info[current_ghealth_read_index].channel_map;

            /* fisrt sample of this fifo read period */
            if (0 == current_ghealth_read_index)
            {
                gh_first_sample_pro(this,
                                    function_chnl_map,
                                    function_chnl_num,
                                    sensor_buf);
            }

            /* last sample of this fifo read period */
            if (current_ghealth_read_index == (sensor_buf->ghealth_buffer.sample_cnt - 1))
            {
                this->frame->fifo_end_flag = 1;
            }

            // DEBUG_LOG("[gh_collector_ghealth_data_pro] current_ghealth_read_index = %d\r\n",
            //           current_ghealth_read_index);
            search_result = gh_collector_chnl_index_find(cur_point_chnl_map,
                                                         function_chnl_map,
                                                         function_chnl_num);

            if ((GH_INVALID_CHNL_INDEX != search_result) && (search_result < this->frame->ch_num))
            {
                this->chnl_collect_flag |= (1 << search_result);

                if (0 == search_result)
                {
                    /* calculate timestamp and update to frame data */
                    this->frame->timestamp = sensor_buf->ghealth_buffer.timestamp_begin
                                           + (this->timestamp_step * this->chnl0_sample_cnt)
                                           / GH_TIMESTAMPE_STEP_UNIT_RADIO;
                    this->chnl0_sample_cnt++;
                }
                gh_frame_data_fill(this,
                                   cur_point_chnl_map,
                                   function_chnl_map,
                                   current_ghealth_read_index,
                                   search_result,
                                   sensor_buf);

                /* ghealth data collection ok */
                DEBUG_LOG("[ghealth_data_pro] chnl_collect_flag = 0x%x, function_chnl_num:0x%llx\r\n",
                          this->chnl_collect_flag, ((uint64_t)1 << (function_chnl_num)) - 1);

                if (this->chnl_collect_flag == (((uint64_t)1 << (function_chnl_num)) - 1))
                {
#if  GH_GSENSOR_FUSION_EN
                    if (this->frame->gsensor_en)
                    {
                        this->state = GH_DATA_COLLECTOR_STATE_WAIT_GS_DATA;

                        /* scan gsensor buffer */
                        if (GH_COLLECTOR_GS_SYNC_OK == gh_collector_gsensor_data_scan(this, sensor_buf, sync_type))
                        {
                            gh_cllct_frame_out_and_post_pro(this);
                            this->state = GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA;
                        }
                    }
                    else
#endif
                    {
                        gh_cllct_frame_out_and_post_pro(this);
                    }
                }
            }  // if ((GH_INVALID_CHNL_INDEX != search_result)&&(search_result < this->frame->ch_num))...

            ghealth_sample_read_num++;
        }   // if (GH_DATA_COLLECTOR_STATE_WAIT_GH_DATA == this->state)...
        else // we do not want ghealth data currently
        {
            break;
        }
    }  // for (uint16_t current_ghealth_read_index = this->ghealth_buf_read_index;...

    this->ghealth_buf_read_index += ghealth_sample_read_num;

    /* last sample is handled */
    if ((this->ghealth_buf_read_index >= sensor_buf->ghealth_buffer.sample_cnt)
        && (GH_MSG_GHRALTH_DATA_WILL_BE_CLEAR == msg))
    {
        gh_cllct_new_ghealth_fifo_pre(this);
    }

    return;
}

uint32_t gh_fusion_init(gh_fusion_t *this,
                        gh_data_collector_t **collector_list,
                        gh_func_frame_t **frame_list,
                        gh_frame_publish_t frame_data_publish,
                        void *parent_node)
{
    if ((GH_NULL_PTR == this) || (GH_NULL_PTR == collector_list) || (GH_NULL_PTR == frame_list))
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    uint8_t func_cnt;

    gh_sensor_buffer_init(&(this->sensor_buffer));

    this->collector_list = collector_list;
    gh_sens_buf_cllct_list_register(&(this->sensor_buffer), collector_list);

    for (func_cnt = 0; func_cnt < (uint8_t)GH_FUNC_FIX_IDX_MAX; func_cnt++)
    {
        if ((GH_NULL_PTR != this->collector_list[func_cnt]) && (GH_NULL_PTR != frame_list))
        {
            gh_collector_init(this->collector_list[func_cnt],
                              frame_list[func_cnt],
                              frame_data_publish,
                              parent_node);
        }
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_fusion_deinit(gh_fusion_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    uint8_t func_cnt;

    gh_sensor_buffer_deinit(&(this->sensor_buffer));

    for (func_cnt = 0; func_cnt < (uint8_t)GH_FUNC_FIX_IDX_MAX; func_cnt++)
    {
        if (this->collector_list[func_cnt])
        {
            gh_collector_deinit(this->collector_list[func_cnt]);
        }
    }
    this->collector_list = 0;

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_fusion_ctrl(gh_fusion_t *this, uint32_t function_group)
{
    uint8_t func_cnt;

    for (func_cnt = 0; func_cnt < (uint8_t)GH_FUNC_FIX_IDX_MAX; func_cnt++)
    {
        if (0 != ((((uint32_t)1) << func_cnt) & (function_group)))
        {
            gh_collector_ctrl(this->collector_list[func_cnt], GH_DATA_COLLECTOR_CTRL_ENABLE);
        }
        else
        {
            gh_collector_ctrl(this->collector_list[func_cnt], GH_DATA_COLLECTOR_CTRL_DISABLE);
        }
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

uint32_t gh_fusion_ghealth_data_push(gh_fusion_t *this, gh_data_t *gh_data)
{
    if ((GH_NULL_PTR == this) || (GH_NULL_PTR == gh_data))
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

#if 0
    DEBUG_LOG("[PUSH] ch:%d\r\n", gh_data->data_channel.channel);
    switch (gh_data->data_channel.data_type)
    {
        case GH_PPG_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_ppg.slot_cfg_id,
                      gh_data->data_channel.channel_ppg.rx_id);
        }
        break;

        case GH_PPG_MIX_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_ppg_mix.slot_cfg_id,
                      gh_data->data_channel.channel_ppg_mix.rx_id);
        }
        break;

        case GH_PPG_BG_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_ppg_bg.slot_cfg_id,
                      gh_data->data_channel.channel_ppg_bg.rx_id);
        }
        break;

        case GH_CAP_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_cap.slot_cfg_id,
                      0);
        }
        break;

        case GH_PPG_PARAM_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_ppg_param.slot_cfg_id,
                      gh_data->data_channel.channel_ppg_param.rx_id);
        }
        break;

        case GH_PPG_DRE_DATA:
        {
            DEBUG_LOG("[PUSH]type:%s, cfg%d, rx%d\r\n",
                      g_data_type_name[gh_data->data_channel.data_type],
                      gh_data->data_channel.channel_ppg_dre.slot_cfg_id,
                      gh_data->data_channel.channel_ppg_dre.rx_id);
        }
        break;

        default:
        break;
    }
#endif

    gh_sens_buf_ghealth_data_push(&(this->sensor_buffer), gh_data);

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}

#if  GH_GSENSOR_FUSION_EN
uint32_t gh_fusion_gsensor_data_push(gh_fusion_t *this, gh_gsensor_ts_and_data_t *gsensor_data)
{
    if ((GH_NULL_PTR == this) || (GH_NULL_PTR == gsensor_data))
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_PTR_NULL);
    }

    gh_sens_buf_gsensor_data_push(&(this->sensor_buffer), gsensor_data);

    return RETURN_VALUE_ASSEMBLY(0, GH_FUSION_OK);
}
#endif
