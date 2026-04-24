#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_user.h"
#include "gh_hal_chip.h"
#include "gh_hal_settings.h"
#include "gh_hal_fifo_parser.h"
#include "gh_hal_agc.h"
#include "gh_hal_control.h"
#include "gh_hal_isr.h"
#include "gh_hal_config.h"
#include "gh_hal_utils.h"
#include "gh_hal_log.h"

#if GH_MODULE_ISR_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#if (GH_INTERFACE_SPI_HW_CS == GH_INTERFACE_TYPE)
#define FIFO_START_ADDR                             (1)
#else
#define FIFO_START_ADDR                             (0)
#endif

/**
 * @fn     uint32_t gh_update_agc_info(void);
 *
 * @brief  update AGC info
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
#if GH_SUPPORT_SOFT_AGC
static uint32_t gh_update_agc_info(void)
{
    gh_hal_settings_t* p_hal_settings = gh_hal_get_settings();
    gh_hal_agc_t* p_agc = gh_hal_get_agc();

    for (uint16_t i = 0; i < sizeof(p_hal_settings->ppg_cfg_param) / sizeof(p_hal_settings->ppg_cfg_param[0]); i++)
    {
#if GH_PARAM_SYNC_UPDATE_EN
        gh_memcpy(p_hal_settings->ppg_cfg_param[i].rx_param_pre,
                  p_hal_settings->ppg_cfg_param[i].rx_param,
                  sizeof(p_hal_settings->ppg_cfg_param[i].rx_param));
#endif
        p_hal_settings->ppg_cfg_param[i].rx_param[0].gain_code = p_agc->agc_chip_info.slotcfg_agc_param[i].gain0_code;
        p_hal_settings->ppg_cfg_param[i].rx_param[1].gain_code = p_agc->agc_chip_info.slotcfg_agc_param[i].gain1_code;
        p_hal_settings->ppg_cfg_param[i].led_drv_code[0] = p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv0_code;
        p_hal_settings->ppg_cfg_param[i].led_drv_code[1] = p_agc->agc_chip_info.slotcfg_agc_param[i].led_drv1_code;
    }

    return 0;
}
#endif

uint32_t gh_hal_isr(void)
{
    uint32_t ret = 0;
    gh_hal_isr_status_t isr_status;
    uint16_t fifo_used = 0;
    uint8_t fifo_continue = 0;
    uint32_t fifo_use_byte_len = 0;
    gh_fifo_parser_t* p_fifo_parser = gh_hal_get_fifo_parser();
    uint8_t is_wakeup = 0;

    ret |= gh_exit_lowpower_mode();
    is_wakeup = 1;
    ret |= gh_get_irq(&isr_status);

    gh_hal_isr_event_publish(&isr_status);

    if (isr_status.fifo_up_overflow == 1
         || isr_status.fifo_waterline == 1
         || isr_status.cap_cancel_done == 1)
    {
        ret |= gh_get_fifo_use(&fifo_used);
#if GH_FIFO_USE_WATERMARK_LIMIT
        uint16_t fifo_watermark = 0;
        ret |= gh_get_fifo_watermark(&fifo_watermark);
        if ((fifo_used > fifo_watermark) && (1 != isr_status.cap_cancel_done))
        {
            fifo_used -= (fifo_used % fifo_watermark);
        }
#endif
        fifo_use_byte_len = (uint32_t)fifo_used * GH3036_FIFO_WIDTH;
        DEBUG_LOG("fifo_used = %d, fifo_use_byte_len = %d", fifo_used, fifo_use_byte_len);

        do
        {
            ret |= gh_fifo_read(p_fifo_parser->p_fifo_buffer, &(p_fifo_parser->fifo_buffer_index),
                                p_fifo_parser->fifo_buffer_size - FIFO_START_ADDR,
                                &fifo_use_byte_len, &fifo_continue);

            gh_fifo_timestamp_update(p_fifo_parser, gh_hal_get_timestamp());
            DEBUG_LOG("fifo_buffer_index = %d, fifo_use_byte_len = %d, time: %lld - %lld, diff: %lld",
                      p_fifo_parser->fifo_buffer_index, fifo_use_byte_len,
                      p_fifo_parser->time_stamp_begin,  p_fifo_parser->time_stamp_end,
                      p_fifo_parser->time_stamp_end - p_fifo_parser->time_stamp_begin);
#if GH_SUPPORT_SOFT_AGC
            gh_update_agc_info();
#endif
            gh_fifo_parse_for_agc(p_fifo_parser, p_fifo_parser->p_fifo_buffer + FIFO_START_ADDR,
                                  p_fifo_parser->fifo_buffer_index);

            if (0 == fifo_continue)
            {
                ret |= gh_enter_lowpower_mode();
                is_wakeup = 0;
            }

            if (0 < p_fifo_parser->fifo_buffer_index)
            {
                gh_hal_fifo_data_publish(p_fifo_parser->p_fifo_buffer + FIFO_START_ADDR,
                                         p_fifo_parser->fifo_buffer_index);
            }

            ret |= gh_fifo_parse(p_fifo_parser, p_fifo_parser->p_fifo_buffer + FIFO_START_ADDR,
                                 p_fifo_parser->fifo_buffer_index);

        } while (fifo_continue);
    }//if (isr_status.fifo_up_overflow == 1 || isr_status.fifo_waterline == 1 || isr_status.cap_cancel_done == 1)

    if (1 == is_wakeup)
    {
        ret |= gh_enter_lowpower_mode();
    }

    return ret;
}
