#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_log.h"
#include "gh_hal_settings.h"
#include "gh_hal_chip.h"
#include "gh_hal_fifo_parser.h"
#include "gh_hal_control.h"
#include "gh_hal_config.h"
#include "gh_hal_config_process.h"
#include "gh_hal_interface.h"
#include "gh_hal_isr.h"
#include "gh_hal_user.h"
#include "gh_hal_std.h"
#include "gh_hal_agc.h"
#include "gh_hal_utils.h"
#include "gh3036_reg.h"

#if GH_HAL_SERVICE_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif


#define SHIFT_BITS                       (16)
#define ROUNDING_OFFSET                  (1 << (SHIFT_BITS - 1))
#define SCALE_FACTOR                     (255)

/**
  * @brief FIFO buffer
  */
static uint8_t g_fifo_buffer[GH_FIFO_READ_BUFFER_SIZE];

/**
  * @brief gh data Data buffer
  */
static gh_data_t g_data_buffer[GH_FIFO_DATA_BULK_LEN];

/**
  * @brief FIFO parser
  */
static gh_fifo_parser_t g_fifo_parser;

/**
  * @brief Controller
  */
static gh_hal_control_t g_hal_control;

/**
  * @brief std parser
  */
static gh_hal_std_parser_t g_std_parser;

/**
  * @brief hal settings
  */
static gh_hal_settings_t g_hal_settings;


#if GH_SUPPORT_SOFT_AGC

/**
* @brief  tia gain value unit: 0.1kohm
 */
static const uint16_t g_tia_gain_0p1_ohm[] =
{
    100,
    250,
    500,
    1000,
    2500,
    5000,
    10000,
};

/**
  * @brief agc module
  */
static gh_hal_agc_t g_agc = {0};

gh_hal_agc_t* gh_hal_get_agc(void)
{
    return &g_agc;
}
#endif

gh_fifo_parser_t* gh_hal_get_fifo_parser(void)
{
    return &g_fifo_parser;
}

gh_hal_control_t* gh_hal_get_controller(void)
{
    return &g_hal_control;
}

gh_hal_std_parser_t* gh_hal_get_std_parser(void)
{
    return &g_std_parser;
}

gh_hal_settings_t* gh_hal_get_settings(void)
{
    return &g_hal_settings;
}

void gh_data_cali_callback(gh_data_t* p_gh_data, uint16_t len)
{
    gh_hal_std_parser_t* p_std_parser = gh_hal_get_std_parser();
    for (uint16_t i = 0; i < len; i++)
    {
        gh_hal_std_process(p_std_parser, p_gh_data);
        p_gh_data++;
    }
}

void ghdata_agc_callback(gh_data_t* p_gh_data, uint16_t len)
{
#if GH_SUPPORT_SOFT_AGC
    gh_hal_agc_t* p_agc = gh_hal_get_agc();
    gh_agc_process(p_agc, p_gh_data, len);
#endif
}

void gh_slot_disable_callback(uint8_t slot_cfg_id)
{
#if (GH_PARAM_BACKUP_EN && GH_SUPPORT_SOFT_AGC)
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();
    gh_hal_agc_t* p_agc = gh_hal_get_agc();
    uint8_t tia_gain[GH_PPG_RX_NUM];

    for (uint8_t i = 0; i < sizeof(p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code_backup)
        / sizeof(p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code_backup[0]); i++)
    {
        gh_hal_led_drv_write(slot_cfg_id, i, p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code_backup[i]);
        p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code[i] =
            p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code_backup[i];

    }

    for (uint8_t i = 0; i < sizeof(p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param_backup)
        / sizeof(p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param_backup[0]); i++)
    {
        gh_hal_tia_gain_write(slot_cfg_id, i, p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param_backup[i].gain_code);
        p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param[i].gain_code =
            p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param_backup[i].gain_code;
        tia_gain[i] = p_hal_settings->ppg_cfg_param[slot_cfg_id].rx_param_backup[i].gain_code;

    }
    gh_agc_slot_cfg_restore(p_agc, slot_cfg_id,
                            p_hal_settings->ppg_cfg_param[slot_cfg_id].led_drv_code,
                            tia_gain);

#endif
}

uint32_t gh_hal_service_init(void)
{
    uint32_t ret = 0;
    gh_fifo_parser_t* p_fifo_parser = gh_hal_get_fifo_parser();
    gh_hal_control_t* p_hal_control = gh_hal_get_controller();
    gh_hal_std_parser_t* p_std_parser = gh_hal_get_std_parser();
#if GH_SUPPORT_SOFT_AGC
    gh_hal_agc_t* p_agc = gh_hal_get_agc();
#endif
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();

    ret = gh_hal_interface_init();

    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_INTF_INIT_ERROR;
        ERROR_LOG("gh_hal_interface_init error\n");
        return ret;
    }

    ret = gh_hal_settings_init(p_hal_settings);
    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_SETTINGS_INIT_ERROR;
        ERROR_LOG("gh_hal_settings_init error\n");
        return ret;
    }

    p_hal_control->p_settings = p_hal_settings;
    ret = gh_hal_control_init(p_hal_control, gh_slot_disable_callback);
    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_CONTROL_INIT_ERROR;
        ERROR_LOG("gh_hal_control_init error\n");
        return ret;
    }

    p_fifo_parser->p_settings = p_hal_settings;
    ret = gh_hal_fifo_init(p_fifo_parser, (uint8_t*)g_fifo_buffer, GH_FIFO_READ_BUFFER_SIZE,
                           g_data_buffer, GH_FIFO_DATA_BULK_LEN);
    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_FIFO_PARSER_INIT_ERROR;
        ERROR_LOG("gh_hal_fifo_init error\n");
        return ret;
    }

    ret = gh_fifo_call_back_register(p_fifo_parser,
                                     gh_data_get_callback,
                                     gh_data_cali_callback,
                                     ghdata_agc_callback);

    p_std_parser->p_settings = p_hal_settings;
    ret = gh_hal_std_init(p_std_parser);
    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_STD_INIT_ERROR;
        ERROR_LOG("gh_hal_std_init error\n");
        return ret;
    }

#if GH_SUPPORT_SOFT_AGC
    p_agc->p_settings = p_hal_settings;
    p_agc->led_drv_write = gh_hal_led_drv_write;
    p_agc->tia_gain_write = gh_hal_tia_gain_write;
    p_agc->agc_chip_info.p_gain_value = g_tia_gain_0p1_ohm;
    #if GH_HAL_AGC_DRE_EN
    p_agc->param_update_end = gh_hal_param_update_end;
    p_agc->param_update_start = gh_hal_param_update_start;
    #endif
    ret = gh_agc_init(p_agc);
    if (0 != GH_RET_HAL_ERR_GET(ret))
    {
        ret = GH_SERVICE_AGC_INIT_ERROR;
        ERROR_LOG("gh_agc_init error\n");
        return ret;
    }
#endif

    return ret;
}

uint32_t gh_hal_channel_en(gh_hal_data_channel_t* p_data_channel)
{
    uint32_t ret = 0;
    gh_hal_control_t* p_hal_control = gh_hal_get_controller();

    ret |= gh_control_channel_en((gh_hal_control_t*)p_hal_control, p_data_channel);

    return ret;
}

uint32_t gh_hal_top_ctrl(uint8_t start)
{
    uint32_t ret = 0;
    gh_hal_control_t* p_hal_control = gh_hal_get_controller();

    if (start)
    {
        gh_fifo_parser_t* p_fifo_parser = gh_hal_get_fifo_parser();
        gh_fifo_timestamp_update(p_fifo_parser, gh_hal_get_timestamp());
    }

    ret |= gh_control_top_ctrl((gh_hal_control_t*)p_hal_control, start);

    return ret;
}

uint32_t gh_hal_service_reset(void)
{
    uint32_t ret = 0;
    gh_fifo_parser_t* p_fifo_parser = gh_hal_get_fifo_parser();
    gh_hal_control_t* p_hal_control = gh_hal_get_controller();
    gh_hal_std_parser_t* p_std_parser = gh_hal_get_std_parser();
#if GH_SUPPORT_SOFT_AGC
    gh_hal_agc_t* p_agc = gh_hal_get_agc();
#endif
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();

    ret |= gh_hal_chip_reset();
    ret |= gh_hal_settings_reset(p_hal_settings);
    ret |= gh_hal_control_reset(p_hal_control);
    ret |= gh_fifo_parse_reset(p_fifo_parser);
    ret |= gh_hal_std_parse_reset(p_std_parser);
#if GH_SUPPORT_SOFT_AGC
    ret |= gh_agc_reset(p_agc);
#endif
    return ret;
}

uint32_t gh_hal_service_cfg_download(gh_reg_t* p_reg, uint16_t len)
{
    uint32_t ret = 0;

    gh_hal_service_reset();
    for (uint16_t i = 0; i < len; i++)
    {
        gh_hal_config_download(p_reg[i].addr, p_reg[i].val);
    }

    return ret;
}

uint32_t gh_service_reg_field_write(uint16_t reg_addr, uint8_t lsb, uint8_t msb, uint16_t value)
{
    uint16_t mask_data = ((((uint16_t)0x0001) << (msb - lsb + 1)) - 1) << lsb;
    uint16_t reg_data = 0;
    uint32_t ret = gh_hal_reg_read(reg_addr, &reg_data);
    GH_VAL_CLEAR_BIT(reg_data, mask_data);
    GH_VAL_SET_BIT(reg_data, (value << lsb) & mask_data);
    ret |= gh_hal_config_download(reg_addr, reg_data);
    return ret;
}

#if GH_SUPPORT_SOFT_AGC
uint32_t gh_service_agc_disable(void)
{
    uint32_t ret = 0;
    gh_hal_agc_t *p_agc = gh_hal_get_agc();
    ret = gh_agc_disable(p_agc);
    return ret;
}
#endif

uint32_t gh_service_led_current_set(uint8_t slot_cfg_num, uint8_t drv_idx, uint32_t current_ma)
{
    uint32_t ret = 0;
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();
    uint8_t current_code = 0;

    uint32_t temp = (current_ma * SCALE_FACTOR << SHIFT_BITS)
        / (p_hal_settings->global_cfg_param.led_drv_fs[drv_idx]);
    current_code = (temp + ROUNDING_OFFSET) >> SHIFT_BITS;

    ret = gh_hal_led_drv_write(slot_cfg_num, drv_idx, current_code);

    uint16_t reg_addr = (RG_LED_DRIVER0_CODE_0_ADDR + GH3036_REG_WIDTH * slot_cfg_num);
    if (GH_LED_DRV0_IDX == drv_idx)
    {
        ret |= gh_service_reg_field_write(reg_addr,
                                          RG_LED_DRIVER0_CODE_0_LSB,
                                          RG_LED_DRIVER0_CODE_0_MSB,
                                          current_code);
    }
    else if (GH_LED_DRV1_IDX == drv_idx)
    {
        ret |= gh_service_reg_field_write(reg_addr,
                                          RG_LED_DRIVER1_CODE_0_LSB,
                                          RG_LED_DRIVER1_CODE_0_MSB,
                                          current_code);
    }

    return ret;
}

uint32_t gh_service_sample_rate_set(uint8_t slot_cfg_num, uint16_t sample_rate)
{
    uint32_t ret = 0;
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();
    uint16_t multiplier = 0;

    multiplier = GH_SENSOR_MAIN_FREQ / (p_hal_settings->global_cfg_param.fastest_sample_rate + 1) / sample_rate - 1;

    gh_service_reg_field_write(RG_SR_MULTIPLIER_0_ADDR + GH3036_REG_WIDTH * slot_cfg_num,
                               RG_SR_MULTIPLIER_0_LSB,
                               RG_SR_MULTIPLIER_0_MSB,
                               multiplier);

    return ret;
}

uint32_t gh_service_int_time_set(uint8_t slot_cfg_num, gh_hal_int_time_e int_time)
{
    gh_service_reg_field_write(RG_ADC_INT_0_ADDR + GH3036_REG_WIDTH * slot_cfg_num,
                               RG_ADC_INT_0_LSB,
                               RG_ADC_INT_0_MSB,
                               (uint16_t)int_time);
    return 0;
}

uint32_t gh_service_tia_gain_set(uint8_t slot_cfg_num, uint8_t rx_idx, uint8_t gain_code)
{
    uint32_t ret = 0;
    uint16_t reg_addr = (RG_RX0_TIA_GAIN_0_ADDR + GH3036_REG_WIDTH * slot_cfg_num);
    if (GH_TIA_GIAN0_IDX == rx_idx)
    {
        ret |= gh_service_reg_field_write(reg_addr, RG_RX0_TIA_GAIN_0_LSB, RG_RX0_TIA_GAIN_0_MSB, gain_code);
    }
    else if (GH_TIA_GIAN1_IDX == rx_idx)
    {
        ret |= gh_service_reg_field_write(reg_addr, RG_RX1_TIA_GAIN_0_LSB, RG_RX1_TIA_GAIN_0_MSB, gain_code);
    }

    return ret;
}

#if (0 == GH_USE_SDK_APP)
uint32_t gh_sevice_demo_ctrl(uint32_t running_cfg_id_bitmap)
{
    uint32_t ret = 0;
    ret = gh_hal_top_ctrl(0);
    gh_hal_data_channel_t data_channel;
    gh_memset(&data_channel, 0 , sizeof(gh_hal_data_channel_t));
    data_channel.channel_ppg.data_type = GH_PPG_DATA;

    for (uint8_t cfg_id = 0; cfg_id < sizeof(running_cfg_id_bitmap); cfg_id++)
    {
        if (0 != (running_cfg_id_bitmap & (1 << cfg_id)))
        {
            if (cfg_id <= GH_PPG_CFG7)
            {
                data_channel.channel_ppg.data_type = GH_PPG_DATA;
                data_channel.channel_ppg.slot_cfg_id = cfg_id;
                ret |= gh_hal_channel_en(&data_channel);
            }
            else if (GH_CAP_CFG == cfg_id)
            {
                data_channel.channel_ppg.data_type = GH_CAP_DATA;
                data_channel.channel_ppg.slot_cfg_id = cfg_id;
                ret |= gh_hal_channel_en(&data_channel);
            }
        }
    }

    ret = gh_hal_top_ctrl(1);
    return ret;
}

#endif
