/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "gh_demo_api.h"

static const char TAG[] = "main";

// uint32_t function_adt_en : 0;
// uint32_t function_hr_en : 1;
// uint32_t function_spo2_en: 2;
// uint32_t function_hrv_en: 3;
// uint32_t function_gnadt_en : 1;
// uint32_t function_irnadt_en : 1;
// uint32_t function_test1_en : 1;
// uint32_t function_test2_en : 1;
// uint32_t function_slot_en : 1;

#define EN_SLOT GH_FUNC_SLOT_EN << 8
#define EN_ADT GH_FUNC_ADT_EN << 0 | EN_SLOT
#define EN_HR GH_FUNC_HR_EN << 1 | EN_SLOT
#define EN_SPO2 GH_FUNC_SPO2_EN << 2 | EN_SLOT
#define EN_HRV GH_FUNC_HRV_EN << 3 | EN_SLOT
#define EN_GNADT GH_FUNC_GNADT_EN << 4 | EN_SLOT
#define EN_IRNADT GH_FUNC_IRNADT_EN << 5 | EN_SLOT

void app_main(void)
{

    ESP_LOGI(TAG, "GH3036 SDK Demo Start");
    gh_app_demo_init();
    gh_app_demo_start(EN_HR | EN_HRV | EN_ADT);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5));
        gh_app_demo_int_process();
    }
}
