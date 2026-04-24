#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_chip.h"
#include "gh_hal_fifo_parser.h"
#include "gh_hal_control.h"
#include "gh_hal_isr.h"
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_hal_user.h"

#if GH_MODULE_FIFO_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define GH_RAWDATA_DC_TO_RAWDATA(x)                 ((x) << 5)

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
static uint32_t gh3036_fifo_ppg_parse_debug1(gh_fifo_parser_t* p_fifo_parser,
                                             uint32_t fifo_data, gh_data_t* p_gh_data,
                                             uint8_t fifo_end, uint8_t record);
#endif

static uint32_t  gh3036_parser_rawdata_status_update(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                                     gh3036_parser_status_e* p_parser_status,
                                                     gh3036_parser_status_e rx0_status,
                                                     uint8_t* p_status_reset);

uint32_t gh_hal_fifo_init(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_fifo_buffer, uint16_t fifo_buffer_size,
                          gh_data_t *p_data_buffer, uint16_t bulk_len)
{
    uint32_t ret = 0;

    p_fifo_parser->p_fifo_buffer = (uint8_t*)p_fifo_buffer;
    p_fifo_parser->p_data_buffer = p_data_buffer;
    p_fifo_parser->fifo_buffer_size = (uint16_t)fifo_buffer_size;
    p_fifo_parser->bulk_len = bulk_len;
    p_fifo_parser->fifo_buffer_index = 0;
    ret |= gh_fifo_parse_reset(p_fifo_parser);

    return ret;
}

uint32_t gh_fifo_call_back_register(gh_fifo_parser_t* p_fifo_parser,
                                    gh_data_get_callback_t ghdata_get_callback,
                                    gh_data_get_callback_t ghdata_cali_callback,
                                    gh_data_get_callback_t ghdata_agc_callback)
{
    p_fifo_parser->p_ghdata_get_callback = ghdata_get_callback;
    p_fifo_parser->p_ghdata_cali_callback = ghdata_cali_callback;
    p_fifo_parser->p_ghdata_agc_callback = ghdata_agc_callback;
    return 0;
}

#if GH_PARAM_SYNC_UPDATE_EN
static void gh_param_change_flag_set(gh_fifo_parser_t* p_fifo_parser, uint8_t slot_cfg, uint8_t rx_id,
                                     uint8_t param_change_flag)
{
    if (1 == param_change_flag)
    {
        p_fifo_parser->ppg_param_change_flag |= (1 << (slot_cfg * GH_PPG_RX_NUM + rx_id));
    }
}

static uint16_t gh_param_change_flag_get(gh_fifo_parser_t* p_fifo_parser, uint8_t slot_cfg, uint8_t rx_id)
{
    return (uint16_t)((p_fifo_parser->ppg_param_change_flag >> (slot_cfg * GH_PPG_RX_NUM + rx_id)) & 0x01);
}
#endif

#if (1 == GH_SUPPORT_FIFO_CTRL_CAP)
/**
 * @fn     static uint32_t gh_capdata_transform(gh3036_ppg_bgdata_t* p_gh_bgdata,
 *                                      gh_data_t* p_gh_data, uint8_t fifo_end)
 *
 * @brief   transform cap dre data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser             pointer of fifo parser.
 * @param[in]   p_gh_capdata              pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                  fifo end flag(1:end).
 * @param[in]   record                    record flag(1:record).
 * @param[out]  p_gh_data                 pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_capdata_transform(gh_fifo_parser_t* p_fifo_parser,
                                     gh3036_capdata_t* p_gh_capdata,
                                     gh_data_t* p_gh_data,
                                     uint8_t fifo_end,
                                     uint8_t record)
{
    p_gh_data->cap_data.data_channel.data_type = GH_CAP_DATA;
    p_gh_data->cap_data.data_channel.channel_cap.slot_cfg_id = GH_CAP_CFG;
    p_gh_data->cap_data.rawdata = p_gh_capdata->rawdata;
    p_gh_data->cap_data.fifo_end = fifo_end;

    if (record)
    {
        p_fifo_parser->cap_data_cnt++;
    }
    else
    {
        p_gh_data->common_data.data_cnt = p_fifo_parser->cap_data_cnt;
    }
    DEBUG_LOG("time:%lld - %lld, cap rawdata = %d", p_gh_data->common_data.timestamp_begin,
              p_gh_data->common_data.timestamp_end, p_gh_capdata->rawdata);

    return 0;
}
#endif

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
/**
 * @fn     static uint32_t gh_ppg_dredata_transform(gh3036_ppg_bgdata_t* p_gh_bgdata,
 *                                      gh_data_t* p_gh_data, uint8_t fifo_end)
 *
 * @brief   transform ppg dre data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_gh_dredata             pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_ppg_dredata_transform(gh_fifo_parser_t* p_fifo_parser, gh3036_ppg_dredata_t* p_gh_dredata,
                                         gh_data_t* p_gh_data, uint8_t fifo_end, uint8_t record)
{
    p_gh_data->ppg_dre_data.data_channel.data_type = GH_PPG_DRE_DATA;
    p_gh_data->ppg_dre_data.data_channel.channel_ppg_dre.rx_id = (uint32_t)p_gh_dredata->rx_id;
    p_gh_data->ppg_dre_data.data_channel.channel_ppg_dre.slot_cfg_id = (uint32_t)p_gh_dredata->config_id;
    p_gh_data->ppg_dre_data.dre_update = (uint32_t)p_gh_dredata->dre_update;
    p_gh_data->ppg_dre_data.rawdata = p_gh_dredata->rawdata;
    p_gh_data->ppg_dre_data.fifo_end = fifo_end;

    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_gh_dredata->config_id][GH_FIFO_PPG_DRE_RX0 + p_gh_dredata->rx_id]++;
    }
    else
    {
        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_gh_dredata->config_id][GH_FIFO_PPG_DRE_RX0 + p_gh_dredata->rx_id];
    }

    DEBUG_LOG("time:%lld - %lld,DRE rawdata = %d ,slot = %d, rx = %d",
              p_gh_data->common_data.timestamp_begin,
              p_gh_data->common_data.timestamp_end,
              p_gh_dredata->rawdata,
              p_gh_dredata->config_id,
              p_gh_dredata->rx_id);

    return 0;
}

/**
 * @fn     static uint32_t gh_ppg_bgdata_transform(gh_fifo_parser_t* p_fifo_parser,
 *                                       gh3036_ppg_bgdata_t* p_gh_bgdata,
 *                                       gh_data_t* p_gh_data,
 *                                       uint8_t bg_id,
 *                                       uint8_t fifo_end,
 *                                       uint8_t record)
 *
 * @brief   transform ppg bg data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_gh_bgdata              pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   bg_id                    the bg id.
 * @param[out]  record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_ppg_bgdata_transform(gh_fifo_parser_t* p_fifo_parser,
                                        gh3036_ppg_bgdata_t* p_gh_bgdata,
                                        gh_data_t* p_gh_data,
                                        uint8_t bg_id,
                                        uint8_t fifo_end,
                                        uint8_t record)
{
    p_gh_data->ppg_bg_data.data_channel.data_type = GH_PPG_BG_DATA;
    p_gh_data->ppg_bg_data.data_channel.channel_ppg_bg.slot_cfg_id = (uint32_t)p_gh_bgdata->config_id;
    p_gh_data->ppg_bg_data.data_channel.channel_ppg_bg.rx_id = (uint32_t)p_gh_bgdata->rx_id;
    p_gh_data->ppg_bg_data.data_channel.channel_ppg_bg.bg_id = bg_id;
    p_gh_data->ppg_bg_data.rawdata = p_gh_bgdata->rawdata;
    p_gh_data->ppg_bg_data.fifo_end = fifo_end;
#if GH_PARAM_SYNC_UPDATE_EN
    uint16_t param_change = gh_param_change_flag_get(p_fifo_parser, p_gh_bgdata->config_id, p_gh_bgdata->rx_id);
    gh3036_ppg_rx_param_t* p_rx_param =
        (1 == param_change) ? p_fifo_parser->p_settings->ppg_cfg_param[p_gh_bgdata->config_id].rx_param
        : p_fifo_parser->p_settings->ppg_cfg_param[p_gh_bgdata->config_id].rx_param_pre;
#else
    gh3036_ppg_rx_param_t* p_rx_param = p_fifo_parser->p_settings->ppg_cfg_param[p_gh_bgdata->config_id].rx_param;
#endif
    p_gh_data->ppg_bg_data.gain_code = p_rx_param[p_gh_bgdata->rx_id].gain_code;
    const gh3036_fifo_data_id_e data_id[] = {GH_FIFO_PPG_BG0_RX0, GH_FIFO_PPG_BG1_RX0, GH_FIFO_PPG_BG2_RX0};
    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_gh_bgdata->config_id][data_id[bg_id] + p_gh_bgdata->rx_id]++;
    }
    else
    {
        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_gh_bgdata->config_id][data_id[bg_id] + p_gh_bgdata->rx_id];
    }
    DEBUG_LOG("time:%lld - %lld,BG rawdata = %d ,slot = %d, rx = %d, bg_id = %d",
              p_gh_data->common_data.timestamp_begin, p_gh_data->common_data.timestamp_end,
              p_gh_bgdata->rawdata, p_gh_bgdata->config_id, p_gh_bgdata->rx_id, bg_id);

    return 0;
}
#endif

/**
 *@fn    static uint32_t gh_ppg_rawdata_transform(gh_fifo_parser_t* p_fifo_parser,
 *                                                gh3036_ppg_rawdata_t* p_gh_rawdata,
 *                                                gh_data_t* p_gh_data,
 *                                                uint8_t fifo_end,
 *                                                uint8_t record)
 *
 * @brief   transform ppg data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_gh_rawdata             pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_ppg_rawdata_transform(gh_fifo_parser_t* p_fifo_parser,
                                         gh3036_ppg_rawdata_t* p_gh_rawdata,
                                         gh_data_t* p_gh_data,
                                         uint8_t fifo_end,
                                         uint8_t record)
{
    p_gh_data->ppg_data.data_channel.data_type = GH_PPG_DATA;
    p_gh_data->ppg_data.data_channel.channel_ppg.slot_cfg_id = (uint32_t)p_gh_rawdata->config_id;
    p_gh_data->ppg_data.data_channel.channel_ppg.rx_id = (uint32_t)p_gh_rawdata->rx_id;
    p_gh_data->ppg_data.dre_update = (uint32_t)p_gh_rawdata->dre_update;
    p_gh_data->ppg_data.sa_flag = (uint32_t)p_gh_rawdata->sa_flag;
    p_gh_data->ppg_data.param_change_flag = (uint32_t)p_gh_rawdata->param_change_flag;
#if GH_PARAM_SYNC_UPDATE_EN
    gh_param_change_flag_set(p_fifo_parser, p_gh_rawdata->config_id,
                             p_gh_rawdata->rx_id,
                             p_gh_rawdata->param_change_flag);
    uint16_t param_change = gh_param_change_flag_get(p_fifo_parser, p_gh_rawdata->config_id, p_gh_rawdata->rx_id);
#endif
    uint8_t* p_led_drv_code = p_fifo_parser->p_settings->ppg_cfg_param[p_gh_rawdata->config_id].led_drv_code;
    gh3036_ppg_rx_param_t* p_rx_param =
#if GH_PARAM_SYNC_UPDATE_EN
        (1 == param_change) ? p_fifo_parser->p_settings->ppg_cfg_param[p_gh_rawdata->config_id].rx_param
        : p_fifo_parser->p_settings->ppg_cfg_param[p_gh_rawdata->config_id].rx_param_pre;
#else
        p_fifo_parser->p_settings->ppg_cfg_param[p_gh_rawdata->config_id].rx_param;
#endif

    p_gh_data->ppg_data.rawdata = p_gh_rawdata->rawdata;
    p_gh_data->ppg_data.fifo_end = fifo_end;
    p_gh_data->ppg_data.gain_code =
        p_fifo_parser->p_settings->ppg_cfg_param[p_gh_rawdata->config_id].
        rx_param[p_gh_rawdata->rx_id].gain_code;
    for (uint8_t i = 0; i < GH_LED_DRV_NUM; i++)
    {
        p_gh_data->ppg_data.led_drv[i] = p_led_drv_code[i];
        p_gh_data->ppg_data.led_drv_fs[i] = p_fifo_parser->p_settings->global_cfg_param.led_drv_fs[i];
    }

    p_gh_data->ppg_data.dc_cancel_code = p_rx_param[p_gh_rawdata->rx_id].dc_cancel_code;
    p_gh_data->ppg_data.dc_cancel_range = p_rx_param[p_gh_rawdata->rx_id].dc_cancel_range;
    p_gh_data->ppg_data.bg_cancel_range = p_rx_param[p_gh_rawdata->rx_id].bg_cancel_range;

    p_fifo_parser->rawdata_flag[p_gh_rawdata->rx_id].dre_update = (uint32_t)p_gh_rawdata->dre_update;
    p_fifo_parser->rawdata_flag[p_gh_rawdata->rx_id].sa_flag = (uint32_t)p_gh_rawdata->sa_flag;
    p_fifo_parser->rawdata_flag[p_gh_rawdata->rx_id].param_change_flag = (uint32_t)p_gh_rawdata->param_change_flag;

    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_gh_rawdata->config_id][GH_FIFO_PPG_MIX0_RX0 + p_gh_rawdata->rx_id]++;
    }
    else
    {
        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_gh_rawdata->config_id][GH_FIFO_PPG_MIX0_RX0 + p_gh_rawdata->rx_id];
    }

    DEBUG_LOG("time:%lld - %lld,PPG rawdata = %d , gain = %d, slot = %d, rx = %d, dre upd = %d, data cnt = %d",
              p_gh_data->common_data.timestamp_begin,
              p_gh_data->common_data.timestamp_end, p_gh_rawdata->rawdata,
              p_gh_data->ppg_data.gain_code,
              p_gh_rawdata->config_id, p_gh_rawdata->rx_id,
              p_gh_data->ppg_data.dre_update,
              p_fifo_parser->ppg_data_cnt[p_gh_rawdata->config_id][GH_FIFO_PPG_MIX0_RX0 + p_gh_rawdata->rx_id]);

    return 0;
}

#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO

/**
 *@fn    static uint32_t gh_dre_dc_info_transform(gh_fifo_parser_t* p_fifo_parser,
 *                                                gh3036_ppg_dc_info_t* p_ppg_dc_info,
 *                                                gh_data_t* p_gh_data,
 *                                                uint8_t fifo_end,
 *                                                uint8_t record)
 *
 * @brief   transform ppg data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_ppg_dc_info            pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_dre_dc_info_transform(gh_fifo_parser_t* p_fifo_parser,
                                         gh3036_ppg_dc_info_t* p_ppg_dc_info,
                                         gh_data_t* p_gh_data,
                                         uint8_t fifo_end,
                                         uint8_t record)
{
    p_gh_data->ppg_dre_dc_info.data_channel.data_type = GH_PPG_DRE_DC_INFO;
    p_gh_data->ppg_dre_dc_info.data_channel.channel_ppg.slot_cfg_id = (uint32_t)p_ppg_dc_info->config_id;
    p_gh_data->ppg_dre_dc_info.data_channel.channel_ppg.rx_id = (uint32_t)p_ppg_dc_info->rx_id;
    p_gh_data->ppg_dre_dc_info.dre_update = (uint32_t)p_fifo_parser->rawdata_flag[p_ppg_dc_info->rx_id].dre_update;
    p_gh_data->ppg_dre_dc_info.sa_flag = (uint32_t)p_fifo_parser->rawdata_flag[p_ppg_dc_info->rx_id].sa_flag;
    p_gh_data->ppg_dre_dc_info.param_change_flag =
        (uint32_t)p_fifo_parser->rawdata_flag[p_ppg_dc_info->rx_id].param_change_flag;
#if GH_PARAM_SYNC_UPDATE_EN
    gh_param_change_flag_set(p_fifo_parser, p_ppg_dc_info->config_id,
                             p_ppg_dc_info->rx_id,
                             p_gh_data->ppg_dre_dc_info.param_change_flag);
    uint16_t param_change = gh_param_change_flag_get(p_fifo_parser, p_ppg_dc_info->config_id, p_ppg_dc_info->rx_id);

#endif
    uint8_t* p_led_drv_code = p_fifo_parser->p_settings->ppg_cfg_param[p_ppg_dc_info->config_id].led_drv_code;
    gh3036_ppg_rx_param_t* p_rx_param =
#if GH_PARAM_SYNC_UPDATE_EN
        (1 == param_change) ? p_fifo_parser->p_settings->ppg_cfg_param[p_ppg_dc_info->config_id].rx_param
        : p_fifo_parser->p_settings->ppg_cfg_param[p_ppg_dc_info->config_id].rx_param_pre;
#else
        p_fifo_parser->p_settings->ppg_cfg_param[p_ppg_dc_info->config_id].rx_param;
#endif

    p_gh_data->ppg_dre_dc_info.rawdata = GH_RAWDATA_DC_TO_RAWDATA(p_ppg_dc_info->rawdata_dc);
    p_gh_data->ppg_dre_dc_info.fifo_end = fifo_end;
    p_gh_data->ppg_dre_dc_info.gain_code = p_rx_param[p_ppg_dc_info->rx_id].gain_code;

    for (uint8_t i = 0; i < GH_LED_DRV_NUM; i++)
    {
        p_gh_data->ppg_dre_dc_info.led_drv[i] = p_led_drv_code[i];
        p_gh_data->ppg_dre_dc_info.led_drv_fs[i] = p_fifo_parser->p_settings->global_cfg_param.led_drv_fs[i];
    }


    p_gh_data->ppg_dre_dc_info.dc_cancel_code = p_ppg_dc_info->dc_cancel_code;

    p_gh_data->ppg_dre_dc_info.dc_cancel_range = p_rx_param[p_ppg_dc_info->rx_id].dc_cancel_range;
    p_gh_data->ppg_dre_dc_info.bg_cancel_range = p_rx_param[p_ppg_dc_info->rx_id].bg_cancel_range;

    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_ppg_dc_info->config_id][GH_FIFO_PPG_DRE_DC_INFO_RX0 + p_ppg_dc_info->rx_id]++;
    }
    else
    {
        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_ppg_dc_info->config_id][GH_FIFO_PPG_DRE_DC_INFO_RX0 + p_ppg_dc_info->rx_id];
    }

    DEBUG_LOG("time:%lld - %lld,PPG dc_info = %d , gain = %d, slot = %d, rx = %d, dre upd = %d",
              p_gh_data->common_data.timestamp_begin,
              p_gh_data->common_data.timestamp_end, p_gh_data->ppg_dre_dc_info.rawdata,
              p_gh_data->ppg_dre_dc_info.gain_code,
              p_ppg_dc_info->config_id, p_ppg_dc_info->rx_id,
              p_gh_data->ppg_dre_dc_info.dre_update);

    return 0;
}
#endif


#if GH_SUPPORT_FIFO_CTRL_DEBUG1
/**
 * @fn     static uint32_t gh_ppg_mixdata_transform(gh_fifo_parser_t* p_fifo_parser,
 *                                       gh3036_ppg_rawdata_t* p_gh_mixdata,
 *                                       gh_data_t* p_gh_data,
 *                                       uint8_t mix_id,
 *                                       uint8_t fifo_end,
 *                                       uint8_t record)
 *
 * @brief   transform ppg mix data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_ppg_cfg_param          pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   mix_id                   the id of the current channel.
 * @param[in]   record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_ppg_mixdata_transform(gh_fifo_parser_t* p_fifo_parser,
                                         gh3036_ppg_rawdata_t* p_gh_mixdata,
                                         gh_data_t* p_gh_data,
                                         uint8_t mix_id,
                                         uint8_t fifo_end,
                                         uint8_t record)
{
    p_gh_data->ppg_mixdata.data_channel.data_type = GH_PPG_MIX_DATA;
    p_gh_data->ppg_mixdata.data_channel.channel_ppg_mix.slot_cfg_id = (uint32_t)p_gh_mixdata->config_id;
    p_gh_data->ppg_mixdata.data_channel.channel_ppg_mix.rx_id = (uint32_t)p_gh_mixdata->rx_id;
    p_gh_data->ppg_mixdata.data_channel.channel_ppg_mix.mix_id = mix_id;
    p_gh_data->ppg_mixdata.dre_update = (uint32_t)p_gh_mixdata->dre_update;
    p_gh_data->ppg_mixdata.sa_flag = (uint32_t)p_gh_mixdata->sa_flag;
    p_gh_data->ppg_mixdata.param_change_flag = (uint32_t)p_gh_mixdata->param_change_flag;
#if GH_PARAM_SYNC_UPDATE_EN
    gh_param_change_flag_set(p_fifo_parser, p_gh_mixdata->config_id,
                             p_gh_mixdata->rx_id,
                             p_gh_mixdata->param_change_flag);
    uint16_t param_change = gh_param_change_flag_get(p_fifo_parser, p_gh_mixdata->config_id, p_gh_mixdata->rx_id);
#endif
    uint8_t* p_led_drv_code = p_fifo_parser->p_settings->ppg_cfg_param[p_gh_mixdata->config_id].led_drv_code;
    gh3036_ppg_rx_param_t* p_rx_param =
#if GH_PARAM_SYNC_UPDATE_EN
        (1 == param_change) ? p_fifo_parser->p_settings->ppg_cfg_param[p_gh_mixdata->config_id].rx_param
        : p_fifo_parser->p_settings->ppg_cfg_param[p_gh_mixdata->config_id].rx_param_pre;
#else
        p_fifo_parser->p_settings->ppg_cfg_param[p_gh_mixdata->config_id].rx_param;
#endif

    p_gh_data->ppg_mixdata.rawdata = p_gh_mixdata->rawdata;
    p_gh_data->ppg_mixdata.fifo_end = fifo_end;
    p_gh_data->ppg_mixdata.gain_code = p_rx_param[p_gh_mixdata->rx_id].gain_code;

    for (uint8_t i = 0; i < GH_LED_DRV_NUM; i++)
    {
        p_gh_data->ppg_mixdata.led_drv[i] = p_led_drv_code[i];
        p_gh_data->ppg_mixdata.led_drv_fs[i] = p_fifo_parser->p_settings->global_cfg_param.led_drv_fs[i];
    }

    p_gh_data->ppg_mixdata.dc_cancel_code = p_rx_param[p_gh_mixdata->rx_id].dc_cancel_code;
    p_gh_data->ppg_mixdata.dc_cancel_range = p_rx_param[p_gh_mixdata->rx_id].dc_cancel_range;
    p_gh_data->ppg_mixdata.bg_cancel_range = p_rx_param[p_gh_mixdata->rx_id].bg_cancel_range;
    const gh3036_fifo_data_id_e data_id[] = {GH_FIFO_PPG_MIX0_RX0, GH_FIFO_PPG_MIX1_RX0};
    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_gh_mixdata->config_id][data_id[mix_id] + p_gh_mixdata->rx_id]++;
    }
    else
    {
        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_gh_mixdata->config_id][data_id[mix_id] + p_gh_mixdata->rx_id];
    }

    DEBUG_LOG("time:%lld - %lld,MIX rawdata = %d ,slot = %d, rx = %d, mix_id = %d",
              p_gh_data->common_data.timestamp_begin, p_gh_data->common_data.timestamp_end,
              p_gh_mixdata->rawdata, p_gh_mixdata->config_id, p_gh_mixdata->rx_id, mix_id);

    return 0;
}
#endif

#if GH_SUPPORT_FIFO_CTRL_DEBUG0
/**
 * @fn     static uint32_t gh_ppg_param_transform(gh_fifo_parser_t* p_fifo_parser,
 *                                     gh3036_ppg_param_t* p_ppg_cfg_param,
 *                                     gh_data_t* p_gh_data,
 *                                     uint8_t fifo_end,
 *                                     uint8_t record)
 *
 * @brief   transform ppg data to user data format
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser            pointer of fifo parser.
 * @param[in]   p_ppg_cfg_param          pointer of gh3036 ppg param structure.
 * @param[in]   fifo_end                 fifo end flag(1:end).
 * @param[in]   record                   record flag(1:record).
 * @param[out]  p_gh_data                pointer of gh3036 data structure(user).
 *
 * @return  error code
 */
static uint32_t gh_ppg_param_transform(gh_fifo_parser_t* p_fifo_parser,
                                       gh3036_ppg_param_t* p_ppg_cfg_param,
                                       gh_data_t* p_gh_data,
                                       uint8_t fifo_end,
                                       uint8_t record)
{
    p_gh_data->ppg_param_data.data_channel.data_type = GH_PPG_PARAM_DATA;
    p_gh_data->ppg_param_data.data_channel.channel_ppg_param.slot_cfg_id = (uint32_t)p_ppg_cfg_param->config_id;
    p_gh_data->ppg_param_data.data_channel.channel_ppg_param.rx_id = (uint32_t)p_ppg_cfg_param->rx_id;
    p_gh_data->ppg_param_data.param_rawdata.param.skip_ok_flag = (uint32_t)p_ppg_cfg_param->skip_ok_flag;
    p_gh_data->ppg_param_data.param_rawdata.param.dc_cancel_range = (uint32_t)p_ppg_cfg_param->dc_cancel_range;
    p_gh_data->ppg_param_data.param_rawdata.param.gain_code = (uint32_t)p_ppg_cfg_param->tia_gain;
    p_gh_data->ppg_param_data.param_rawdata.param.bg_cancel_range = (uint32_t)p_ppg_cfg_param->bg_cancel_range;
    p_gh_data->ppg_param_data.param_rawdata.param.dc_cancel_code = (uint32_t)p_ppg_cfg_param->dc_cancel_code;
    p_gh_data->ppg_param_data.param_rawdata.param.bg_cancel_code = (uint32_t)p_ppg_cfg_param->bg_cancel_code;
    p_gh_data->ppg_param_data.fifo_end = fifo_end;

    if (record)
    {
        p_fifo_parser->ppg_data_cnt[p_ppg_cfg_param->config_id][GH_FIFO_PPG_PARAM_RX0 + p_ppg_cfg_param->rx_id]++;
    }
    else
    {

        p_gh_data->common_data.data_cnt =
            p_fifo_parser->ppg_data_cnt[p_ppg_cfg_param->config_id][GH_FIFO_PPG_PARAM_RX0 + p_ppg_cfg_param->rx_id];
    }

    DEBUG_LOG("time:%lld - %lld,PPG param gain= %d , slot = %d, rx = %d",
              p_gh_data->common_data.timestamp_begin, p_gh_data->common_data.timestamp_end,
              p_ppg_cfg_param->tia_gain, p_ppg_cfg_param->config_id, p_ppg_cfg_param->rx_id);

    return 0;
}
#endif

/**
 * @fn     static uint32_t  gh3036_parser_status_reset(gh3036_slot_cfg_param_t* p_slot_cfg_param,
 *                                           gh3036_parser_status_e* p_parser_status)
 *
 * @brief  reset fifo parser status from gh3036 register value and slot configuration parameter.
 *
 * @attention   None
 *
 * @param[in]   p_slot_cfg_param       slot configuration parameter, including bg level and rx en status.
 * @param[out]  p_parser_status        fifo parser status.
 *
 * @return  error code
 */
static  uint32_t  gh3036_parser_status_reset(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                             gh3036_parser_status_e* p_parser_status)
{
    switch (p_ppg_cfg_param->fifo_ctrl)
    {
    case GH3036_FIFOCTRL_MODE0:
        *p_parser_status = (p_ppg_cfg_param->rx_en & 0x01) ?
                               PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0 : PPG_FIFO_PARSER_STATUS_RX1_RAWDATA0;
        break;

#if GH_SUPPORT_FIFO_CTRL_DEBUG0
    case GH3036_FIFOCTRL_MODE1:
    case GH3036_FIFOCTRL_MODE3:
        *p_parser_status = (p_ppg_cfg_param->rx_en & 0x01) ?
                               PPG_FIFO_PARSER_STATUS_RX0_DEBUG0 : PPG_FIFO_PARSER_STATUS_RX1_DEBUG0;
        break;
#endif

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    case GH3036_FIFOCTRL_MODE2:
        if (p_ppg_cfg_param->bg_level > GH3036_BG_LEVEL0)
        {
            *p_parser_status = (p_ppg_cfg_param->rx_en & 0x01) ?
                               PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0 : PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG0;
        }
        else
        {
            *p_parser_status = (p_ppg_cfg_param->rx_en & 0x01) ?
                               PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0 : PPG_FIFO_PARSER_STATUS_RX1_RAWDATA0;
        }
        break;
#endif

    default:
        break;
    }//switch (p_ppg_cfg_param->fifo_ctrl)
    return 0;
}

#if  GH_SUPPORT_FIFO_CTRL_DEBUG0
/**
 * @fn     gh3036_debug0_status_update
 *
 * @brief  update fifo parser status from gh3036 register value and slot configuration parameter.
 *
 * @attention   None
 *
 * @param[in]   p_slot_cfg_param       slot configuration parameter, including bg level and rx en status.
 * @param[out]  p_parser_status        fifo parser status.
 * @param[in]   rx0_status             rx0 status.
 * @param[out]  p_status_reset         status reset flag.
 *
 * @return  error code
 *
 * @note
 */
static uint32_t  gh3036_debug0_status_update(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                             gh3036_parser_status_e* p_parser_status,
                                             gh3036_parser_status_e rx0_status,
                                             uint8_t* p_status_reset)
{
    switch (rx0_status)
    {
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG0:
        if (GH3036_FIFOCTRL_MODE1 == p_ppg_cfg_param->fifo_ctrl
             || ((GH3036_FIFOCTRL_MODE3 == p_ppg_cfg_param->fifo_ctrl)
             && (p_ppg_cfg_param->bg_level == GH3036_BG_LEVEL0)))
        {
            *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0;
        }
#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
        else if ((GH3036_FIFOCTRL_MODE3 == p_ppg_cfg_param->fifo_ctrl)
                && (p_ppg_cfg_param->bg_level > GH3036_BG_LEVEL0))
        {
            *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0;
        }
#endif
        else
        {
            //error
        }
        break;
    default:
        break;
    }
    return 0;
}
#endif

#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
/**
 * @fn     gh3036_debug1_status_update
 *
 * @brief  update fifo parser status from gh3036 register value and slot configuration parameter.
 *
 * @attention   None
 *
 * @param[in]   p_slot_cfg_param       slot configuration parameter, including bg level and rx en status.
 * @param[out]  p_parser_status        fifo parser status.
 * @param[in]   rx0_status             rx0 status.
 * @param[out]  p_status_reset         status reset flag.
 *
 * @return  error code
 *
 * @note
 */
static uint32_t  gh3036_debug1_status_update(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                             gh3036_parser_status_e* p_parser_status,
                                             gh3036_parser_status_e rx0_status,
                                             uint8_t* p_status_reset)
{
    switch (rx0_status)
    {
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0:
        *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0;
        break;
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG1:
        if ((GH3036_FIFOCTRL_MODE3 == p_ppg_cfg_param->fifo_ctrl)
              || (GH3036_FIFOCTRL_MODE2 == p_ppg_cfg_param->fifo_ctrl))
        {
            if (p_ppg_cfg_param->bg_level > GH3036_BG_LEVEL2)
            {
                *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_RAWDATA1;
            }
            else if (1 == p_ppg_cfg_param->dre_en)
            {
                *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE;
            }
            else
            {
                *p_status_reset = 1;
            }
        }
        else
        {
            //error
        }
        break;

    case PPG_FIFO_PARSER_STATUS_RX0_RAWDATA1:
        *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG2;
        break;
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG2:
        if (1 == p_ppg_cfg_param->dre_en)
        {
            *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE;
        }
        else
        {
            *p_status_reset = 1;
        }
        break;

    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE:
        *p_status_reset = 1;
        break;
    default:
        break;
    }//switch (rx0_status)
    return 0;
}
#endif

/**
 * @fn     static uint32_t  gh3036_parser_status_update(gh3036_slot_cfg_param_t* p_slot_cfg_param,
 *                                           gh3036_parser_status_e* p_parser_status)
 *
 * @brief  update fifo parser status from gh3036 register value and slot configuration parameter.
 *
 * @attention   None
 *
 * @param[in]   p_slot_cfg_param       slot configuration parameter, including bg level and rx en status.
 * @param[out]  p_parser_status        fifo parser status.
 *
 * @return  error code
 */
static uint32_t  gh3036_parser_status_update(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                             gh3036_parser_status_e* p_parser_status)
{
    gh3036_parser_status_e current_status;
    current_status = (gh3036_parser_status_e)(*p_parser_status);
    gh3036_parser_status_e rx0_status = (gh3036_parser_status_e)((int32_t)current_status
                                                             - ((int32_t)current_status % GH_PPG_RX_NUM));

    uint8_t status_reset = 0;
    if ((0 == current_status % GH_PPG_RX_NUM) && current_status < PPG_FIFO_PARSER_STATUS_MAX)
    {
        if (0 != (0x01 & (p_ppg_cfg_param->rx_en >> 1)))// RX1 EN
        {
            *p_parser_status = (gh3036_parser_status_e)((int32_t)(*p_parser_status) + 1);
            return 0;
        }
    }

    switch (rx0_status)
    {
#if  GH_SUPPORT_FIFO_CTRL_DEBUG0
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG0:
        gh3036_debug0_status_update(p_ppg_cfg_param, p_parser_status, rx0_status, &status_reset);
        break;
#endif

    case PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0:
        gh3036_parser_rawdata_status_update(p_ppg_cfg_param, p_parser_status, rx0_status, &status_reset);
        break;
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    case PPG_FIFO_PARSER_STATUS_RX0_DC_INFO:
        status_reset = 1;
        break;
#endif
#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0:
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG1:
    case PPG_FIFO_PARSER_STATUS_RX0_RAWDATA1:
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG2:
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE:
        gh3036_debug1_status_update(p_ppg_cfg_param, p_parser_status, rx0_status, &status_reset);
        break;
#endif

    default:
        status_reset = 1;
        break;
    }//switch (rx0_status)

    if (1 == status_reset)
    {
        gh3036_parser_status_reset(p_ppg_cfg_param, p_parser_status);
    }
    else if (1 == (current_status % GH_PPG_RX_NUM))
    {
        if (0 == (0x01 & p_ppg_cfg_param->rx_en))//RX0 DISABLE
        {
            (*p_parser_status) = (gh3036_parser_status_e)((int32_t)(*p_parser_status) + 1);
        }
    }

    return 0;
};

/**
 * @fn     gh3036_parser_rawdata_status_update
 *
 * @brief  update fifo parser status from gh3036 register value and slot configuration parameter.
 *
 * @attention   None
 *
 * @param[in]   p_slot_cfg_param       slot configuration parameter, including bg level and rx en status.
 * @param[out]  p_parser_status        fifo parser status.
 * @param[in]   rx0_status             rx0 status.
 * @param[out]  p_status_reset         status reset flag.
 *
 * @return  error code
 *
 * @note
 */
static uint32_t  gh3036_parser_rawdata_status_update(gh3036_ppg_cfg_param_t* p_ppg_cfg_param,
                                                     gh3036_parser_status_e* p_parser_status,
                                                     gh3036_parser_status_e rx0_status,
                                                     uint8_t* p_status_reset)
{

#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
        if ((GH3036_FIFOCTRL_MODE3 == p_ppg_cfg_param->fifo_ctrl)
             || (GH3036_FIFOCTRL_MODE2 == p_ppg_cfg_param->fifo_ctrl))
        {
            if (p_ppg_cfg_param->bg_level > GH3036_BG_LEVEL1)
            {
                *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG1;
            }
            else if (1 == p_ppg_cfg_param->dre_en)
            {
                *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE;
            }
            else
            {
                *p_status_reset = 1;
            }
        }
        else
        {
#endif
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
            if (1 == p_ppg_cfg_param->dre_en && 1 == p_ppg_cfg_param->dre_fifo_output_mode)
            {
                *p_parser_status = PPG_FIFO_PARSER_STATUS_RX0_DC_INFO;
            }
            else
#endif
            {
                *p_status_reset = 1;
            }
#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
        }
#endif
    return 0;
}

#if (1 == GH_SUPPORT_FIFO_CTRL_CAP)
/**
 * @fn     static uint32_t gh3036_fifo_cap_parse(gh_fifo_parser_t* p_fifo_parser,
 *                                    uint32_t fifo_data, gh_data_t* p_gh_data,
 *                                    uint8_t fifo_end)
 *
 * @brief  parse cap from gh3036 data structure to user defined format.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser        pointer to fifo parser structure
 * @param[in]   fifo_data            fifo data
 * @param[in]   fifo_end             end of fifo data. (1: end; 0 : not)
 * @param[out]  p_gh_data            pointer to user defined data structure.
 *
 * @return  error code
 */
static uint32_t gh3036_fifo_cap_parse(gh_fifo_parser_t* p_fifo_parser,
                                      uint32_t fifo_data, gh_data_t* p_gh_data,
                                      uint8_t fifo_end, uint8_t record)
{
    uint32_t ret = 0;
    gh3036_fifo_data_t cap_data;
    gh3036_slot_cfg_id_e slot_cfg = (gh3036_slot_cfg_id_e)(GH_GET_SLOT_CFG_ID(fifo_data));

    cap_data.fifo_data = fifo_data;
    if (slot_cfg == GH_CAP_CFG)
    {
        ret |= gh_capdata_transform(p_fifo_parser, &(cap_data.capdata), p_gh_data, fifo_end, record);
    }

    return ret;
}
#endif

/**
 * @fn     static uint32_t gh3036_fifo_ppg_parse(gh_fifo_parser_t* p_fifo_parser,
 *                                    uint32_t fifo_data, gh_data_t* p_gh_data,
 *                                    uint8_t fifo_end)
 *
 * @brief  parse ppg from gh3036 data structure to user defined format.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser        pointer to fifo parser structure
 * @param[in]   fifo_data            fifo data
 * @param[in]   fifo_end             end of fifo data. (1: end; 0 : not)
 * @param[out]  p_gh_data            pointer to user defined data structure.
 *
 * @return  error code
 */
static uint32_t gh3036_fifo_ppg_parse(gh_fifo_parser_t* p_fifo_parser,
                                      uint32_t fifo_data, gh_data_t* p_gh_data,
                                      uint8_t fifo_end, uint8_t record)
{
    gh3036_parser_status_e current_status;
    uint32_t ret = 0;
    gh3036_fifo_data_t ppg_data;
    gh3036_slot_cfg_id_e slot_cfg = (gh3036_slot_cfg_id_e)(GH_GET_SLOT_CFG_ID(fifo_data));

    if (slot_cfg <= GH_PPG_CFG7)
    {
        p_fifo_parser->current_cfg = slot_cfg;
        gh3036_parser_status_reset(&(p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg]),
                                   &(p_fifo_parser->parser_status));
    }

    current_status = p_fifo_parser->parser_status;
    ppg_data.fifo_data = fifo_data;

    switch (current_status)
    {
#if GH_SUPPORT_FIFO_CTRL_DEBUG0
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG0:
    case PPG_FIFO_PARSER_STATUS_RX1_DEBUG0:
        ppg_data.ppg_param.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_param.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_ppg_param_transform(p_fifo_parser, &(ppg_data.ppg_param), p_gh_data, fifo_end, record);
        break;
#endif

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0:
    case PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG0:
        ppg_data.ppg_bgdata.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_bgdata.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_ppg_bgdata_transform(p_fifo_parser, &(ppg_data.ppg_bgdata),
                                      p_gh_data,     GH3036_BG_ID0,
                                      fifo_end,      record);
        break;
#endif

    case PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0:
    case PPG_FIFO_PARSER_STATUS_RX1_RAWDATA0:
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
        if (GH3036_FIFOCTRL_MODE2 == p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg].fifo_ctrl
             || GH3036_FIFOCTRL_MODE3 == p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg].fifo_ctrl)
        {
            ppg_data.ppg_mixdata.config_id = p_fifo_parser->current_cfg;
            ppg_data.ppg_mixdata.rx_id = current_status % GH_PPG_RX_NUM;
            ret = gh_ppg_mixdata_transform(p_fifo_parser, &(ppg_data.ppg_mixdata), p_gh_data, 0, fifo_end, record);
        }
        else
#endif
        {
            ppg_data.ppg_rawdata.config_id = p_fifo_parser->current_cfg;
            ppg_data.ppg_rawdata.rx_id = current_status % GH_PPG_RX_NUM;
            ret = gh_ppg_rawdata_transform(p_fifo_parser,
                                           &(ppg_data.ppg_rawdata),
                                           p_gh_data,
                                           fifo_end,
                                           record);
        }
        break;
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    case PPG_FIFO_PARSER_STATUS_RX0_DC_INFO:
    case PPG_FIFO_PARSER_STATUS_RX1_DC_INFO:
        ppg_data.ppg_dc_info.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_dc_info.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_dre_dc_info_transform(p_fifo_parser,
                                       &(ppg_data.ppg_dc_info),
                                       p_gh_data,
                                       fifo_end,
                                       record);
        break;
#endif

    default:
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
        gh3036_fifo_ppg_parse_debug1(p_fifo_parser,
                                     fifo_data, p_gh_data,
                                     fifo_end, record);
#endif
        break;
    }//switch (current_status)

    ret |= gh3036_parser_status_update(&p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg],
                                       &p_fifo_parser->parser_status);

    return ret;
}


#if GH_SUPPORT_FIFO_CTRL_DEBUG1
/**
 * @fn     static uint32_t gh3036_fifo_ppg_parse_debug1(gh_fifo_parser_t* p_fifo_parser,
 *                                               uint32_t fifo_data, gh_data_t* p_gh_data,
 *                                               uint8_t fifo_end)
 *
 * @brief  parse ppg from gh3036 data structure to user defined format.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser        pointer to fifo parser structure
 * @param[in]   fifo_data            fifo data
 * @param[in]   fifo_end             end of fifo data. (1: end; 0 : not)
 * @param[out]  p_gh_data            pointer to user defined data structure.
 *
 * @return  error code
 */
static uint32_t gh3036_fifo_ppg_parse_debug1(gh_fifo_parser_t* p_fifo_parser,
                                             uint32_t fifo_data, gh_data_t* p_gh_data,
                                             uint8_t fifo_end, uint8_t record)
{
    gh3036_parser_status_e current_status;
    uint32_t ret = 0;
    gh3036_fifo_data_t ppg_data;

    current_status = p_fifo_parser->parser_status;
    ppg_data.fifo_data = fifo_data;

    switch (current_status)
    {
    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG1:
    case PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG1:
        ppg_data.ppg_bgdata.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_bgdata.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_ppg_bgdata_transform(p_fifo_parser, &(ppg_data.ppg_bgdata),
                                      p_gh_data,     GH3036_BG_ID1,
                                      fifo_end,      record);
        break;

    case PPG_FIFO_PARSER_STATUS_RX0_RAWDATA1:
    case PPG_FIFO_PARSER_STATUS_RX1_RAWDATA1:
        if (GH3036_FIFOCTRL_MODE2 == p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg].fifo_ctrl
            || GH3036_FIFOCTRL_MODE3 == p_fifo_parser->p_settings->ppg_cfg_param[p_fifo_parser->current_cfg].fifo_ctrl)
        {
            ppg_data.ppg_mixdata.config_id = p_fifo_parser->current_cfg;
            ppg_data.ppg_mixdata.rx_id = current_status % GH_PPG_RX_NUM;
            ret = gh_ppg_mixdata_transform(p_fifo_parser, &(ppg_data.ppg_mixdata), p_gh_data, 1, fifo_end, record);
        }
        else
        {
            //error
            ret = 1;
        }
        break;

    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG2:
    case PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG2:
        ppg_data.ppg_bgdata.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_bgdata.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_ppg_bgdata_transform(p_fifo_parser, &(ppg_data.ppg_bgdata),
                                      p_gh_data,     GH3036_BG_ID2,
                                      fifo_end,      record);
        break;

    case PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE:
    case PPG_FIFO_PARSER_STATUS_RX1_DEBUG2_DRE:
        ppg_data.ppg_dredata.config_id = p_fifo_parser->current_cfg;
        ppg_data.ppg_dredata.rx_id = current_status % GH_PPG_RX_NUM;
        ret = gh_ppg_dredata_transform(p_fifo_parser, &(ppg_data.ppg_dredata), p_gh_data, fifo_end, record);
        break;
    default:
        break;
    }// switch (current_status)
    return ret;
}
#endif

/**
 * @fn     uint32_t gh_fifo_parse_process(gh_fifo_parser_t* p_fifo_parser,
 *                                uint8_t* p_buffer,
 *                                uint16_t buffer_len,
 *                                gh_data_get_callback_t* p_gh_data_get_callback,
 *                                uint16_t call_back_num);
 *
 * @brief  parse process for data get from fifo buffer.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser                pointer to fifo parser structure
 * @param[in]   p_buffer                     pointer to buffer for data get from fifo.
 * @param[in]   buffer_len                   length of data get from fifo.
 * @param[in]   p_gh_data_get_callback       pointer to buffer for call back.
 * @param[in]   call_back_num                length of buffer.
 *
 * @return  error code
 */
static uint32_t gh_fifo_parse_process(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_buffer, uint16_t buffer_len,
                                      gh_data_get_callback_t* p_gh_data_get_callback,
                                      uint16_t call_back_num,
                                      uint8_t record)
{
    uint32_t* p_fifo_data;
    gh3036_slot_cfg_id_e slot_cfg;
    uint32_t ret = 0;
    uint32_t buf_cnt = 0;
    gh_data_t gh_data;
#if GH_PARAM_SYNC_UPDATE_EN
    p_fifo_parser->ppg_param_change_flag = 0;
#endif
    gh_memset(p_fifo_parser->p_data_buffer, 0, sizeof(gh_data_t) * p_fifo_parser->bulk_len);

    for (uint16_t i = 0; i < buffer_len; i += GH3036_FIFO_WIDTH)
    {
        gh_memset(&gh_data, 0, sizeof(gh_data_t));
        gh_data.common_data.timestamp_begin = p_fifo_parser->time_stamp_begin;
        gh_data.common_data.timestamp_end = p_fifo_parser->time_stamp_end;
        p_fifo_data = (uint32_t*)((uint8_t*)p_buffer + i);
        slot_cfg = (gh3036_slot_cfg_id_e)(GH_GET_SLOT_CFG_ID(*p_fifo_data));

        if (slot_cfg == GH_CAP_CFG)
        {
#if (1 == GH_SUPPORT_FIFO_CTRL_CAP)
            ret |= gh3036_fifo_cap_parse(p_fifo_parser, *p_fifo_data, &gh_data,
                                         i + GH3036_FIFO_WIDTH >= buffer_len,
                                         record);
#endif
        }
        else
        {
            ret |= gh3036_fifo_ppg_parse(p_fifo_parser, *p_fifo_data, &gh_data,
                                         i + GH3036_FIFO_WIDTH >= buffer_len,
                                         record);
        }

        gh_memcpy((void *)&p_fifo_parser->p_data_buffer[buf_cnt], &gh_data, sizeof(gh_data_t));
        buf_cnt++;

        if (p_fifo_parser->bulk_len == buf_cnt || (i + GH3036_FIFO_WIDTH) == buffer_len)
        {
            for (uint16_t call_back_cnt = 0; call_back_cnt < call_back_num; call_back_cnt++)
            {

#if (1 == GH_STACK_INFO_EN)
                void gh_data_get_callback(gh_data_t *p_gh_data, uint16_t len);
                gh_data_get_callback(p_fifo_parser->p_data_buffer, buf_cnt);
#else
                if (0 != p_gh_data_get_callback[call_back_cnt])
                {
                    p_gh_data_get_callback[call_back_cnt](p_fifo_parser->p_data_buffer, buf_cnt);
                }
#endif
            }
            buf_cnt = 0;
        }
    }//for (uint16_t i = 0; i < buffer_len; i += GH3036_FIFO_WIDTH)

    return ret;
}

uint32_t gh_fifo_parse_for_agc(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_buffer, uint16_t buffer_len)
{
    gh_data_get_callback_t gh_data_get_callback[] = {p_fifo_parser->p_ghdata_agc_callback};
    gh3036_slot_cfg_id_e current_cfg = p_fifo_parser->current_cfg;
    gh3036_parser_status_e parser_status = p_fifo_parser->parser_status;

    gh_memset(p_fifo_parser->ppg_data_cnt, 0, sizeof(p_fifo_parser->ppg_data_cnt));
    p_fifo_parser->cap_data_cnt = 0;

    gh_fifo_parse_process(p_fifo_parser, p_buffer, buffer_len,
                          gh_data_get_callback,
                          sizeof(gh_data_get_callback)/sizeof(gh_data_get_callback[0]),
                          1);

    p_fifo_parser->parser_status = parser_status;
    p_fifo_parser->current_cfg = current_cfg;

    return 0;
}

uint32_t gh_fifo_parse(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_buffer, uint16_t buffer_len)
{
    gh_data_get_callback_t gh_data_get_callback[] =
    {
        p_fifo_parser->p_ghdata_cali_callback,
        p_fifo_parser->p_ghdata_get_callback
    };
    gh_fifo_parse_process(p_fifo_parser, p_buffer, buffer_len,
                          gh_data_get_callback,
                          sizeof(gh_data_get_callback)/sizeof(gh_data_get_callback[0]),
                          0);
    return 0;
}

uint32_t gh_fifo_parse_reset(gh_fifo_parser_t* p_fifo_parser)
{
    p_fifo_parser->fifo_buffer_index = 0;
    p_fifo_parser->time_stamp_begin = 0;
    p_fifo_parser->time_stamp_end = 0;
    p_fifo_parser->cap_data_cnt = 0;
    gh_memset(p_fifo_parser->ppg_data_cnt, 0, sizeof(p_fifo_parser->ppg_data_cnt));
    return 0;
}

uint32_t gh_fifo_parse_config(gh_fifo_parser_t* p_fifo_parser, uint16_t reg_addr, uint16_t reg_val)
{
    uint32_t ret = 0;

    return ret;
}

uint32_t gh_fifo_timestamp_update(gh_fifo_parser_t* p_fifo_parser, uint64_t time_stamp)
{
    p_fifo_parser->time_stamp_begin = p_fifo_parser->time_stamp_end;
    p_fifo_parser->time_stamp_end = time_stamp;

    return 0;
}
