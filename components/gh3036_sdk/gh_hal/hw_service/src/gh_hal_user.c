/**
  ****************************************************************************************
  * @file    gh_hal_user.c
  * @author  GHealth Driver Team
  * @brief   goodix hal user
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
#include <stdint.h>
#include "gh_hal_log.h"
#include "gh_hal_config.h"
#include "gh_hal_service.h"
#if (1 == GH_USE_SDK_APP)
#include "gh_app.h"
#endif
#include "gh_hal_user.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/* isr event name */
#if GH_LOG_DEBUG_ENABLE
static const char* g_isr_event_name[GH_HAL_ISR_MAX] =
{
    "CHIP_RESET",
    "FIFO_UP",
    "FIFO_DOWN",
    "FIFO_WATER",
    "TIMER",
    "USER",
    "FRAME_DONE",
    "SAMPLE_ERROR",
    "CAP_CANCEL",
    "LDO_OC",
    "SYNC_SAMPLE_ERR"
};
#endif

#if (0 == GH_USE_SDK_APP)
/* data type */
static const char* g_data_type[] =
{
    "GH_PPG_DATA",
    "GH_PPG_MIX_DATA",
    "GH_PPG_BG_DATA",
    "GH_PPG_BG_CANCEL",
    "GH_PPG_LED_DRV",
    "GH_ECG_DATA",
    "GH_BIA_DATA",
    "GH_GSR_DATA",
    "GH_PRESSURE_DATA",
    "GH_TEMPERATURE_DATA",
    "GH_CAP_DATA",
    "GH_PPG_PARAM_DATA",
    "GH_PPG_DRE_DATA",
};
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
__attribute__((weak)) uint64_t gh_hal_get_timestamp(void)
{
    // return bsp_timestamp_get();
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_delay_ms(uint16_t ms)
{
    // bsp_delay_ms(ms);
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_delay_us(uint16_t us)
{
    // bsp_delay_us(us);
    return 0;
}

__attribute__((weak)) int gh_hal_log_user(char *str)
{
    // bsp_log_out(str);
    return 0;
}

#if (GH_USE_STD_SNPRINTF == 0)
__attribute__((weak)) int gh_hal_snprintf_user(char *p_str, size_t size, const char *p_format, ...)
{
    // return bsp_snprintf(p_str, size, p_format);
    return 0;
}
#endif

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS || GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)
__attribute__((weak)) uint32_t gh_hal_spi_init(void)
{
    // bsp_spi_init();
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_spi_write(uint8_t* buffer, uint16_t len)
{
    // bsp_spi_write(buffer, len);
    return 0;
}

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS)
__attribute__((weak)) uint32_t gh_hal_spi_read(uint8_t* buffer, uint16_t len)
{
    // bsp_spi_read(buffer, len);
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_spi_cs_ctrl(uint8_t level)
{
    // bsp_spi_cs_pin_ctrl(level);
    return 0;
}
#endif

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)
__attribute__((weak)) uint32_t gh_hal_spi_write_read(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t len)
{
    // bsp_spi_write_read(tx_buffer, rx_buffer, len);
    return 0;
}
#endif

#elif (GH_INTERFACE_TYPE == GH_INTERFACE_I2C)
__attribute__((weak)) uint32_t gh_hal_i2c_init(void)
{
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_i2c_write(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len)
{
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_i2c_read(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len)
{
    return 0;
}
#endif

#if (GH_ISR_MODE == INTERRUPT_MODE)
__attribute__((weak)) uint32_t gh_hal_int_pin_init(void)
{
    // bsp_sensor_int_pin_init();
    return 0;
}
#endif

#if (GH_SUPPORT_HARD_RESET)
__attribute__((weak)) uint32_t gh_hal_reset_pin_init(void)
{
    // bsp_sensor_reset_pin_init();
    return 0;
}

__attribute__((weak)) uint32_t gh_hal_reset_pin_ctrl(uint8_t level)
{
    // bsp_sensor_reset_pin_ctrl(level);
    return 0;
}
#endif

__attribute__((weak)) uint32_t gh_hal_isr_event_publish(gh_hal_isr_status_t *p_event)
{
    /* Get interrupt event from here */
    uint16_t *p_payload = (uint16_t *)p_event;

    for (uint8_t i = 0; i < GH_HAL_ISR_MAX; i++)
    {
        if (0 != ((*p_payload >> i) & 0x1))
        {
            /* Just for debug */
            GH_LOG_LVL_DEBUG("[%s] %s \r\n", __FUNCTION__, g_isr_event_name[i]);
        }
    }

    return 0;
}

__attribute__((weak)) uint32_t gh_hal_fifo_data_publish(uint8_t *p_data, uint16_t size)
{
    /* Get fifo rawadata from here, */
    /* If the customer selects the gsensor synchronization mode, start reading gsensor data */
#if (1 == GH_USE_SDK_APP)
    #if (GH_FUSION_MODE_SEL == GH_FUSION_MODE_SYNC)

    /* step1: get gsensor data when using gsensor sync mode */


    /* step2: call gh_demo_gsensor_data_sync_set to set gsensor data */
    // gh_demo_gsensor_data_sync_set(gh_gsensor_data_t *p_data, uint16_t size);
    #endif
#endif
    return 0;
}

__attribute__((weak)) void gh_data_get_callback(gh_data_t *p_gh_data, uint16_t len)
{
#if (0 == GH_USE_SDK_APP)

    GH_LOG_LVL_DEBUG("DATA LEN = %d", len);

    for (uint16_t i = 0; i < len; i++)
    {
        gh_data_type_e data_type = p_gh_data[i].data_channel.data_type;
        switch (data_type)
        {
        case GH_PPG_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rx:%d, rawdata:%d, ipd_pA:%d",
                             g_data_type[data_type],
                             p_gh_data[i].ppg_data.data_channel.channel_ppg.slot_cfg_id,
                             p_gh_data[i].ppg_data.data_channel.channel_ppg.rx_id,
                             p_gh_data[i].ppg_data.rawdata,
                             p_gh_data[i].ppg_data.ipd_pa);
            break;

#if GH_SUPPORT_FIFO_CTRL_DEBUG1
        case GH_PPG_MIX_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rx:%d, rawdata:%d, ipd_pA:%d, mix_id:%d",
                             g_data_type[data_type],
                             p_gh_data[i].ppg_mixdata.data_channel.channel_ppg_mix.slot_cfg_id,
                             p_gh_data[i].ppg_mixdata.data_channel.channel_ppg_mix.rx_id,
                             p_gh_data[i].ppg_mixdata.rawdata,
                             p_gh_data[i].ppg_mixdata.ipd_pa,
                             p_gh_data[i].ppg_mixdata.data_channel.channel_ppg_mix.mix_id);
            break;

        case GH_PPG_BG_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rx:%d, rawdata:%d, ipd_pA:%d, bg_id:%d",
                             g_data_type[data_type],
                             p_gh_data[i].ppg_bg_data.data_channel.channel_ppg_bg.slot_cfg_id,
                             p_gh_data[i].ppg_bg_data.data_channel.channel_ppg_bg.rx_id,
                             p_gh_data[i].ppg_bg_data.rawdata,
                             p_gh_data[i].ppg_bg_data.ipd_pa,
                             p_gh_data[i].ppg_bg_data.data_channel.channel_ppg_bg.bg_id);
            break;

        case GH_PPG_DRE_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rx:%d, rawdata:%d, dre_update:%d",
                             g_data_type[data_type],
                             p_gh_data[i].ppg_dre_data.data_channel.channel_ppg_dre.slot_cfg_id,
                             p_gh_data[i].ppg_dre_data.data_channel.channel_ppg_dre.rx_id,
                             p_gh_data[i].ppg_dre_data.rawdata,
                             p_gh_data[i].ppg_dre_data.dre_update);
            break;
#endif

#if GH_SUPPORT_FIFO_CTRL_DEBUG0
        case GH_PPG_PARAM_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rx:%d, dc range:%d, bg range:%d, gain: %d",
                             g_data_type[data_type],
                             p_gh_data[i].ppg_param_data.data_channel.channel_ppg_param.slot_cfg_id,
                             p_gh_data[i].ppg_param_data.data_channel.channel_ppg_param.rx_id,
                             p_gh_data[i].ppg_param_data.param_rawdata.param.dc_cancel_range,
                             p_gh_data[i].ppg_param_data.param_rawdata.param.bg_cancel_range,
                             p_gh_data[i].ppg_param_data.param_rawdata.param.gain_code);
            GH_LOG_LVL_DEBUG("skip:%d, dc_cancel: %d, bg_cancel: %d",
                             p_gh_data[i].ppg_param_data.param_rawdata.param.skip_ok_flag,
                             p_gh_data[i].ppg_param_data.param_rawdata.param.dc_cancel_code,
                             p_gh_data[i].ppg_param_data.param_rawdata.param.bg_cancel_code);

            break;
#endif
        case GH_CAP_DATA:
            GH_LOG_LVL_DEBUG("%s, slot:%d, rawdata:%d",
                             g_data_type[data_type],
                             p_gh_data[i].cap_data.data_channel.channel_cap.slot_cfg_id,
                             p_gh_data[i].cap_data.rawdata);
            break;

        default:
            break;
        }//switch(data_type)

    }//for (uint16_t i = 0; i < len; i++)
#endif

#if (1 == GH_USE_SDK_APP)
    /* Get rawadata or debug data from here */
    for (uint16_t i = 0; i < len; i++)
    {
        /* If the customer needs fusion module, call gh_demo_ghealth_data_set and get data from gh_demo_data_publish */
        gh_demo_ghealth_data_set(p_gh_data + i);
    }
#endif
}

