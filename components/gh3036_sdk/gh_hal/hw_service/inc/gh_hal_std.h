/**
 ****************************************************************************************
 *
 * @file    gh_hal_std.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of public api.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
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

/** @addtogroup GH HAL STD
  * @{
  */

/** @defgroup STD
  * @brief Public STD.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GH_HAL_STD_H__
#define __GH_HAL_STD_H__

#include <stdint.h>
#include "gh_hal_chip.h"
#include "gh_hal_settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_HAL_STD_INTERNEL_ID          (0)
#define PPG_SLOT_CFG_MAX_NUM            (8)
#define ADC_FULLSCALE_MAX               (8388608)
#define VREF_HALF_MV                    (900)
#define UA2PA                           (1000000)
#define UA2NA                           (1000)
#define KOM2OM                          (1000)
#define MV2UV                           (1000)
#define ROUND                           (10)
#define HALFROUND                       (5)
#define EIGHT_BIT_MAX                   (255)

#define LED_CALI_Y0                     (100000) //times 1000000
#define LED_CALI_Y1                     (10000)  //times 1000000
#define LED_CALI_DRV                    (160000) //unit uv
#define LED_CALI_FS_CODE_MAX            (3)
#define LED_CALI_EXTEND                 (1000000)
#define LED_CALI_HALF_EXTEND            (500000)
#define GAIN_CALI_MAX                   (512)
#define GAIN_CALI_EXTEND                (1000000)
#define GAIN_CALI_HALF_EXTEND           (500000)
#define DC_CANCEL_CALI_A_MAX            (256)
#define DC_CANCEL_CALI_B_MAX            (64)
#define DC_CANCEL_CALI_EXTEND           (1000000)
#define DC_CANCEL_CALI_HALF_EXTEND      (500000)
#define RX_OFFSET_CALI_MAX              (10)
#define RX_OFFSET_HALF_CALI_MAX         (5)

/**
  * @brief hal std error code
  */
typedef enum
{
    GH_HAL_STD_OK = 0,
    GH_HAL_STD_NULL = 1,
    GH_HAL_STD_FAIL = 2,
} gh_hal_std_ret_e;

/**
  * @brief slot param structure.
  */
typedef struct
{

    uint8_t dc_cancel_range_0 : 2;
    uint8_t dc_cancel_range_1 : 2;
    uint8_t reserved : 4;
    uint8_t dc_cancel_code_0;
    uint8_t dc_cancel_code_1;
} __attribute__ ((packed)) gh_std_slot_cfg_param_t;

#if GH_HAL_STD_CALI_EN
/**
  * @brief hal sample ctrl operation
  */
typedef enum
{
    GH_HAL_STD_CALI_LED = 0,
    GH_HAL_STD_CALI_DC_CANCEL = 1,
    GH_HAL_STD_CALI_BG_CANCEL = 2,
    GH_HAL_STD_CALI_RX_OFFSET = 3,
    GH_HAL_STD_CALI_GAIN = 4,
} gh_hal_std_cali_op_e;

/**
  * @brief drv data structure.
  */
typedef struct
{
    uint8_t drv_num;
    uint8_t drv_fs;
    uint8_t drv_code;
} __attribute__ ((packed)) gh_std_cali_drv_data_t;

/**
  * @brief dc cancel data structure.
  */
typedef struct
{
    uint8_t dc_cancel_code;
    uint8_t dc_cancel_fs;
} __attribute__ ((packed)) gh_std_cali_dc_data_t;

/**
  * @brief bg cancel data structure.
  */
typedef struct
{
    uint8_t bg_cancel_code;
    uint8_t bg_cancel_fs;
} __attribute__ ((packed)) gh_std_cali_bg_data_t;

/**
  * @brief rx offset data structure.
  */
typedef struct
{
    uint8_t rx_num;
} __attribute__ ((packed)) gh_std_cali_rx_data_t;

/**
  * @brief gain data structure.
  */
typedef struct
{
    uint8_t rx_num : 4;
    uint8_t gain_code : 4;
} __attribute__ ((packed)) gh_std_cali_gain_data_t;

/**
  * @brief cali data structure.
  */
typedef struct
{
#if GH_HAL_STD_CALI_DRV_EN
    gh_std_cali_drv_data_t  drv_data;
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
    gh_std_cali_dc_data_t   dc_cancel_data;
#endif

#if GH_HAL_STD_CALI_BG_CANCEL_EN
    gh_std_cali_bg_data_t   bg_cancel_data;
#endif

#if GH_HAL_STD_CALI_RX_OFFSET_EN
    gh_std_cali_rx_data_t   rx_offset_data;
#endif

#if GH_HAL_STD_CALI_GAIN_EN
    gh_std_cali_gain_data_t gain_data;
#endif
} __attribute__ ((packed)) gh_std_cali_data_t;
#endif

/**
  * @brief std parser structure.
  */
typedef struct
{
    gh_hal_settings_t*          p_settings;
    gh_std_slot_cfg_param_t     slot_cfg_param[PPG_SLOT_CFG_MAX_NUM];
#if GH_HAL_STD_CALI_EN
    gh_std_cali_data_t          cali_data;
    gh_hal_std_cali_param_t     cali_param;
#endif
} gh_hal_std_parser_t;

/**
 * @fn     uint32_t gh_hal_std_parse_config(gh_hal_std_parser_t* p_std_parser,
 *                                       uint16_t reg_addr, uint16_t reg_val);
 *
 * @brief  config register for std parser
 *
 * @attention   None
 *
 * @param[in]   p_std_parser               pointer to std parser structure
 * @param[in]   reg_addr                   register address for config.
 * @param[in]   reg_val                    value to write in the register.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_std_parse_config(gh_hal_std_parser_t* p_std_parser, uint16_t reg_addr, uint16_t reg_val);

/**
 * @fn uint32_t gh_hal_std_parse_reset(gh_hal_std_parser_t* p_std_parser)
 * @brief reset gh_hal_std
 * @param[in]   p_std_parser               pointer to std parser structure
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_hal_std_parse_reset(gh_hal_std_parser_t* p_std_parser);

/**
 * @fn uint32_t gh_hal_std_init(gh_hal_std_parser_t* p_std_parser)
 * @brief init gh_hal_std
 * @param[in]   p_std_parser               pointer to std parser structure
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_hal_std_init(gh_hal_std_parser_t* p_std_parser);

/**
 * @fn uint32_t gh_hal_std_process(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data)
 * @brief std process
 * @param[in]   p_std_parser               pointer to std parser structure
 * @param[in]   p_gh_data                  pointer to gh data
 * @param[out] None
 * @return error code
 *
 * @note None
 */
uint32_t gh_hal_std_process(gh_hal_std_parser_t* p_std_parser, gh_data_t* p_gh_data);

#if GH_HAL_STD_CALI_DRV_EN
/**
 * @fn uint8_t gh_hal_std_set_drv_code(gh_std_cali_led_param_t led_cali_param, uint32_t drv_ua, uint8_t drv_fs,
                                uint8_t drv_num, uint8_t slot_cfg_num);
 * @brief get drv code with calibration
 * @param[in]   led_cali_param             led cali param
 * @param[in]   drv_ua                     led drv(ua)
 * @param[in]   drv_fs                     led drv fullscale
 * @param[in]   drv_num                    led drv num
 * @param[in]   slot_cfg_num               slot cfg num
 * @param[out] None
 * @return code
 *
 * @note None
 */
uint8_t gh_hal_std_set_drv_code(gh_std_cali_led_param_t led_cali_param, uint32_t drv_ua, uint8_t drv_fs,
                                uint8_t drv_num, uint8_t slot_cfg_num);
#else

/**
 * @fn uint8_t gh_hal_std_set_drv_code(uint32_t drv_ua, uint8_t drv_fs, uint8_t drv_num, uint8_t slot_cfg_num);
 * @brief get drv code
 * @param[in]   drv_ua                     led drv(ua)
 * @param[in]   drv_fs                     led drv fullscale
 * @param[in]   drv_num                    led drv num
 * @param[in]   slot_cfg_num               slot cfg num
 * @param[out] None
 * @return code
 *
 * @note None
 */
uint8_t gh_hal_std_set_drv_code(uint32_t drv_ua, uint8_t drv_fs, uint8_t drv_num, uint8_t slot_cfg_num);
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
/**
 * @fn gh_hal_std_set_dc_cancel_code(gh_std_cali_dc_param_t dc_cancel_param, uint32_t dc_na, uint16_t dc_fs,
                                      uint8_t dc_num, uint8_t slot_cfg_num);
 * @brief get dc cancel code with calibration
 * @param[in]   dc_cancel_param            dc cancel cali param
 * @param[in]   dc_na                      dc cancel(na)
 * @param[in]   dc_fs                      dc cancel fullscale
 * @param[in]   dc_num                     dc cancel drv num
 * @param[in]   slot_cfg_num               slot cfg num
 * @param[out]  None
 * @return code
 *
 * @note None
 */
uint8_t gh_hal_std_set_dc_cancel_code(gh_std_cali_dc_param_t dc_cancel_param, uint32_t dc_na, uint16_t dc_fs,
                                      uint8_t dc_num, uint8_t slot_cfg_num);

#else
/**
 * @fn gh_hal_std_set_dc_cancel_code(uint32_t dc_na, uint16_t dc_fs, uint8_t dc_num, uint8_t slot_num);
 * @brief get dc cancel code
 * @param[in]   dc_na                      dc cancel(na)
 * @param[in]   dc_fs                      dc cancel fullscale
 * @param[in]   dc_num                     dc cancel drv num
 * @param[in]   slot_cfg_num               slot cfg num
 * @param[out]  None
 * @return code
 *
 * @note None
 */
uint8_t gh_hal_std_set_dc_cancel_code(uint32_t dc_na, uint16_t dc_fs, uint8_t dc_num, uint8_t slot_cfg_num);
#endif

/**
 * @fn     gh_hal_std_parser_t* gh_hal_get_std_parser(void);
 *
 * @brief  get the std parser instance.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  pointer to control structure
 */
gh_hal_std_parser_t* gh_hal_get_std_parser(void);


#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_STD_H__ */

/** @} */

/** @} */

