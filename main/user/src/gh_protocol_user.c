/**
  ****************************************************************************************
  * @file    gh_protocol_user.c
  * @author  GHealth Driver Team
  * @brief   gh protocol user file
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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "gh_rpccore.h"

/*
 * DEFINES
 *****************************************************************************************
 */


/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static const char *TAG = "gh_protocol_user";
static SemaphoreHandle_t s_protocol_mutex = NULL;

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void gh_protocal_lock(void)
{
    if (s_protocol_mutex != NULL)
    {
        (void)xSemaphoreTake(s_protocol_mutex, portMAX_DELAY);
    }
}

void gh_protocal_unlock(void)
{
    if (s_protocol_mutex != NULL)
    {
        (void)xSemaphoreGive(s_protocol_mutex);
    }
}

void gh_protocol_delay()
{
    vTaskDelay(pdMS_TO_TICKS(5));
}

void gh_protocol_data_send(void *data, int size)
{
    if (data == NULL || size <= 0)
    {
        return;
    }

    // TODO: Replace with actual BLE/UART/Wi-Fi transport send function.
    ESP_LOGI(TAG, "protocol tx pending transport, len=%d", size);
}

void gh_protocol_init(void)
{
    GhRPCInitialInfo info;

    if (s_protocol_mutex == NULL)
    {
        s_protocol_mutex = xSemaphoreCreateMutex();
    }

    memset(&info, 0, sizeof(GhRPCInitialInfo));
    info.lock = gh_protocal_lock;
    info.unlock = gh_protocal_unlock;
    info.delay = gh_protocol_delay;
    info.sendFunction = gh_protocol_data_send;
    GHRPC_init(info);
}

void gh_protocol_data_recevice(uint8_t *p_rx_buffer, uint8_t rx_len)
{
    GHRPC_process(p_rx_buffer, rx_len, 0);
}

