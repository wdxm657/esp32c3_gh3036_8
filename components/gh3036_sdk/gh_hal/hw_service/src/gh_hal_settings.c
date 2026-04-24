#include "gh_hal_config.h"
#include "gh_hal_chip.h"
#include "gh_hal_service.h"
#include "gh_hal_settings.h"
#include "gh_hal_utils.h"
#include "gh_hal_log.h"

#if GH_HAL_SETTINGS_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif


#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
    ((uint32_t)(GH_HAL_SETTINGS_ID << 24 | (internal_err) << 8 | (interface_err)))

uint32_t gh_hal_settings_init(gh_hal_settings_t* p_settings)
{
    if (GH_NULL_PTR == p_settings)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_PTR_NULL);
    }

    gh_hal_settings_reset(p_settings);
    return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
}

uint32_t gh_hal_settings_reset(gh_hal_settings_t* p_settings)
{
    if (GH_NULL_PTR == p_settings)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_PTR_NULL);
    }

    gh_memset(p_settings->ppg_cfg_en_bits, 0, sizeof(p_settings->ppg_cfg_en_bits));
    gh_memset(p_settings->ppg_cfg_param, 0, sizeof(p_settings->ppg_cfg_param));
    for (uint8_t i = 0; i < sizeof(p_settings->ppg_cfg_param) / sizeof(p_settings->ppg_cfg_param[0]); i++)
    {
        p_settings->ppg_cfg_param[i].rx_en = 1;
        p_settings->ppg_cfg_param[i].bg_level = 1;

        for (uint8_t j = 0;
             j < sizeof(p_settings->ppg_cfg_param[i].rx_param)
                 / sizeof(p_settings->ppg_cfg_param[i].rx_param[0]);
             j++)
        {
            p_settings->ppg_cfg_param[i].rx_param[j].gain_code = PPG_TIA_GAIN_DEFAULT;
        }
#if GH_PARAM_BACKUP_EN
        gh_memcpy(p_settings->ppg_cfg_param[i].led_drv_code_backup,
                  p_settings->ppg_cfg_param[i].led_drv_code,
                  sizeof(p_settings->ppg_cfg_param[i].led_drv_code));

        gh_memcpy(p_settings->ppg_cfg_param[i].rx_param_backup,
                  p_settings->ppg_cfg_param[i].rx_param,
                  sizeof(p_settings->ppg_cfg_param[i].rx_param));
#endif
    }

    gh_memset(&p_settings->global_cfg_param, 0, sizeof(p_settings->global_cfg_param));
    p_settings->global_cfg_param.fastest_sample_rate = FASTEST_SAMPLE_RET_DEFAULT;
    for (uint8_t i = 0;
         i < sizeof(p_settings->global_cfg_param.led_drv_fs)
             / sizeof(p_settings->global_cfg_param.led_drv_fs[0]);
         i++)
    {
        p_settings->global_cfg_param.led_drv_fs[i] = LED_DRV_FS_DEFAULT;
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
}

uint32_t gh_hal_settings_config(gh_hal_settings_t* p_settings, uint16_t reg_addr, uint16_t reg_val)
{
    if (GH_NULL_PTR == p_settings)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_PTR_NULL);
    }

    gh3036_global_cfg_parse(reg_addr, reg_val, &p_settings->global_cfg_param);

    for (uint8_t i = 0; i < sizeof(p_settings->ppg_cfg_param) / sizeof(p_settings->ppg_cfg_param[0]); i++)
    {
        gh3036_ppg_slot_config(reg_addr, reg_val, i,
                               p_settings->ppg_cfg_param + i);
#if GH_PARAM_BACKUP_EN
        gh_memcpy(p_settings->ppg_cfg_param[i].led_drv_code_backup,
                  p_settings->ppg_cfg_param[i].led_drv_code,
                  sizeof(p_settings->ppg_cfg_param[i].led_drv_code));

        gh_memcpy(p_settings->ppg_cfg_param[i].rx_param_backup,
                  p_settings->ppg_cfg_param[i].rx_param,
                  sizeof(p_settings->ppg_cfg_param[i].rx_param));
#endif
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
}

uint32_t gh_hal_cal_slot_data_cnt(gh_hal_settings_t* p_settings, gh3036_slot_cfg_id_e slot_cfg_id, uint16_t* p_data_cnt)
{
    if (GH_NULL_PTR == p_settings || GH_NULL_PTR == p_data_cnt)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_PTR_NULL);
    }
    if (slot_cfg_id > GH_PPG_CFG7
        && GH_CAP_CFG != slot_cfg_id)
    {
        *p_data_cnt = 0;
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
    }
    if (GH_CAP_CFG == slot_cfg_id)
    {
        *p_data_cnt = 1;
        return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
    }

    // ppg frame data count
    // calculate rx en bits num in slot_cfg
    uint16_t data_cnt_of_frame = 0;
    uint8_t rx_en_num = 0;
    uint8_t rx_en = p_settings->ppg_cfg_param[slot_cfg_id].rx_en;
    while (rx_en != 0)
    {
        rx_en &= (uint8_t)(rx_en - 1);
        rx_en_num++;
    }

    rx_en_num = (rx_en_num > GH_PPG_RX_NUM) ? GH_PPG_RX_NUM : rx_en_num;

    uint8_t fifo_ctrl = p_settings->ppg_cfg_param[slot_cfg_id].fifo_ctrl;
    uint8_t dre_en = p_settings->ppg_cfg_param[slot_cfg_id].dre_en;
    uint8_t bg_level = p_settings->ppg_cfg_param[slot_cfg_id].bg_level;
    data_cnt_of_frame = 1;//ppg rawdata
    if (fifo_ctrl == GH3036_FIFOCTRL_MODE1
        || fifo_ctrl == GH3036_FIFOCTRL_MODE3)
    {
        data_cnt_of_frame += 1;//ppg param data
    }
    if ((fifo_ctrl == GH3036_FIFOCTRL_MODE1
        || fifo_ctrl == GH3036_FIFOCTRL_MODE0)
        && 1 == p_settings->ppg_cfg_param[slot_cfg_id].dre_en
        && 1 == p_settings->ppg_cfg_param[slot_cfg_id].dre_fifo_output_mode)
    {
        data_cnt_of_frame += 1;//dre dc info
    }
    if (fifo_ctrl == GH3036_FIFOCTRL_MODE2
        || fifo_ctrl == GH3036_FIFOCTRL_MODE3)
    {
        if (bg_level > GH3036_BG_LEVEL0)
        {
            data_cnt_of_frame += 1;//ppg bg0
        }
        if (bg_level > GH3036_BG_LEVEL1)
        {
            data_cnt_of_frame += 1;//ppg bg1
        }
        if (bg_level > GH3036_BG_LEVEL2)
        {
            data_cnt_of_frame += 1;//ppg bg2
            data_cnt_of_frame += 1;//ppg mix2
        }
        if (dre_en)
        {
            data_cnt_of_frame += 1;//ppg dre
        }
    }
    *p_data_cnt = data_cnt_of_frame * rx_en_num;

    return RETURN_VALUE_ASSEMBLY(0, GH_SETTINGS_OK);
}
