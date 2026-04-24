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
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include <stdbool.h>

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
#ifndef GH_ESP_SPI_HOST
#define GH_ESP_SPI_HOST               SPI2_HOST
#endif
#ifndef GH_ESP_SPI_SCLK_GPIO
#define GH_ESP_SPI_SCLK_GPIO          11
#endif
#ifndef GH_ESP_SPI_MOSI_GPIO
#define GH_ESP_SPI_MOSI_GPIO          12
#endif
#ifndef GH_ESP_SPI_MISO_GPIO
#define GH_ESP_SPI_MISO_GPIO          13
#endif
#ifndef GH_ESP_SPI_CS_GPIO
#define GH_ESP_SPI_CS_GPIO            10
#endif
#ifndef GH_ESP_INT_GPIO
#define GH_ESP_INT_GPIO               9
#endif
#ifndef GH_ESP_RESET_GPIO
#define GH_ESP_RESET_GPIO             8
#endif
#ifndef GH_ESP_SPI_CLOCK_HZ
#define GH_ESP_SPI_CLOCK_HZ           (2 * 1000 * 1000)
#endif
#ifndef GH_ESP_SPI_MODE
#define GH_ESP_SPI_MODE               0
#endif
#ifndef GH_ESP_SPI_QUEUE_SIZE
#define GH_ESP_SPI_QUEUE_SIZE         1
#endif
#ifndef GH_ESP_SPI_MAX_TRANSFER_SZ
#define GH_ESP_SPI_MAX_TRANSFER_SZ    64
#endif

static const char *TAG = "gh_hal_user";
static spi_device_handle_t s_gh_spi_dev = NULL;
static bool s_gh_spi_inited = false;

static esp_err_t gh_spi_ensure_init(void)
{
    if (s_gh_spi_inited) {
        return ESP_OK;
    }

    spi_bus_config_t buscfg = {
        .mosi_io_num = GH_ESP_SPI_MOSI_GPIO,
        .miso_io_num = GH_ESP_SPI_MISO_GPIO,
        .sclk_io_num = GH_ESP_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = GH_ESP_SPI_MAX_TRANSFER_SZ,
    };

    esp_err_t ret = spi_bus_initialize(GH_ESP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %d", ret);
        return ret;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = GH_ESP_SPI_CLOCK_HZ,
        .mode = GH_ESP_SPI_MODE,
        .spics_io_num = -1,
        .queue_size = GH_ESP_SPI_QUEUE_SIZE,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };

    ret = spi_bus_add_device(GH_ESP_SPI_HOST, &devcfg, &s_gh_spi_dev);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "spi_bus_add_device failed: %d", ret);
        return ret;
    }

    gpio_config_t cs_io_cfg = {
        .pin_bit_mask = (1ULL << GH_ESP_SPI_CS_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cs_io_cfg);
    gpio_set_level(GH_ESP_SPI_CS_GPIO, 1);

    s_gh_spi_inited = true;
    return ESP_OK;
}

uint64_t gh_hal_get_timestamp(void)
{
    return (uint64_t)(esp_timer_get_time() / 1000);
}

uint32_t gh_hal_delay_ms(uint16_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
    return 0;
}

uint32_t gh_hal_delay_us(uint16_t us)
{
    esp_rom_delay_us(us);
    return 0;
}

int gh_hal_log_user(char *str)
{
    if (str != NULL)
    {
        ESP_LOGI(TAG, "%s", str);
    }
    return 0;
}

#if (GH_USE_STD_SNPRINTF == 0)
int gh_hal_snprintf_user(char *p_str, size_t size, const char *p_format, ...)
{
    (void)p_str;
    (void)size;
    (void)p_format;
    return 0;
}
#endif

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS || GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)
uint32_t gh_hal_spi_init(void)
{
    return (gh_spi_ensure_init() == ESP_OK) ? 0 : 1;
}

uint32_t gh_hal_spi_write(uint8_t* buffer, uint16_t len)
{
    if (gh_spi_ensure_init() != ESP_OK || buffer == NULL || len == 0) {
        return 1;
    }

    spi_transaction_t t = {
        .length = (size_t)len * 8,
        .tx_buffer = buffer,
        .rx_buffer = NULL,
    };

    return (spi_device_transmit(s_gh_spi_dev, &t) == ESP_OK) ? 0 : 1;
}

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS)
uint32_t gh_hal_spi_read(uint8_t* buffer, uint16_t len)
{
    if (gh_spi_ensure_init() != ESP_OK || buffer == NULL || len == 0) {
        return 1;
    }

    spi_transaction_t t = {
        .length = (size_t)len * 8,
        .rxlength = (size_t)len * 8,
        .tx_buffer = NULL,
        .rx_buffer = buffer,
    };

    return (spi_device_transmit(s_gh_spi_dev, &t) == ESP_OK) ? 0 : 1;
}

uint32_t gh_hal_spi_cs_ctrl(uint8_t level)
{
    gpio_set_level(GH_ESP_SPI_CS_GPIO, level ? 1 : 0);
    return 0;
}
#endif

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)
uint32_t gh_hal_spi_write_read(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t len)
{
    if (gh_spi_ensure_init() != ESP_OK || tx_buffer == NULL || rx_buffer == NULL || len == 0) {
        return 1;
    }

    spi_transaction_t t = {
        .length = (size_t)len * 8,
        .rxlength = (size_t)len * 8,
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
    };

    return (spi_device_transmit(s_gh_spi_dev, &t) == ESP_OK) ? 0 : 1;
}
#endif

#elif (GH_INTERFACE_TYPE == GH_INTERFACE_I2C)
uint32_t gh_hal_i2c_init(void)
{
    return 0;
}

uint32_t gh_hal_i2c_write(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len)
{
    (void)i2c_slaver_id;
    (void)p_buffer;
    (void)len;
    return 0;
}

uint32_t gh_hal_i2c_read(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len)
{
    (void)i2c_slaver_id;
    (void)p_buffer;
    (void)len;
    return 0;
}
#endif

#if (GH_ISR_MODE == INTERRUPT_MODE)
uint32_t gh_hal_int_pin_init(void)
{
    gpio_config_t int_io_cfg = {
        .pin_bit_mask = (1ULL << GH_ESP_INT_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return (gpio_config(&int_io_cfg) == ESP_OK) ? 0 : 1;
}
#endif

#if (GH_SUPPORT_HARD_RESET)
uint32_t gh_hal_reset_pin_init(void)
{
    gpio_config_t rst_io_cfg = {
        .pin_bit_mask = (1ULL << GH_ESP_RESET_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    if (gpio_config(&rst_io_cfg) != ESP_OK) {
        return 1;
    }

    gpio_set_level(GH_ESP_RESET_GPIO, 1);
    return 0;
}

uint32_t gh_hal_reset_pin_ctrl(uint8_t level)
{
    return (gpio_set_level(GH_ESP_RESET_GPIO, level ? 1 : 0) == ESP_OK) ? 0 : 1;
}
#endif

uint32_t gh_hal_isr_event_publish(gh_hal_isr_status_t *p_event)
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

uint32_t gh_hal_fifo_data_publish(uint8_t *p_data, uint16_t size)
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

void gh_data_get_callback(gh_data_t *p_gh_data, uint16_t len)
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

