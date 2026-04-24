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

void app_main(void)
{

    ESP_LOGI(TAG, "GH3036 SDK Demo Start");
    gh_app_demo_init();
    gh_app_demo_start(GH_FUNC_SLOT_EN);
    while (1) {
        // Add your main loop handling code here.
        vTaskDelay(1000);
        gh_app_demo_int_process();
    }
}
