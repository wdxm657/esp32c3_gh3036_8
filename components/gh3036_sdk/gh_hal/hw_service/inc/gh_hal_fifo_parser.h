#ifndef __GH_HAL_FIFO_PARSER_H__
#define __GH_HAL_FIFO_PARSER_H__

#include <stdint.h>
#include "gh_hal_config.h"
#include "gh_hal_settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PPG_SLOT_CFG_MAX_NUM      (8)

#define GH_GET_SLOT_CFG_ID(x)     ((uint8_t)((((uint32_t)(x)) >> 28) & 0x0F))

/**
  * @brief get fifo data callback function type
  */
typedef void (*gh_data_get_callback_t)(gh_data_t* gh_data, uint16_t len);

/**
  * @brief get fifo data callback function type
  */
typedef enum
{
#if GH_SUPPORT_FIFO_CTRL_DEBUG0
    PPG_FIFO_PARSER_STATUS_RX0_DEBUG0,
    PPG_FIFO_PARSER_STATUS_RX1_DEBUG0,
#endif
#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
    PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG0,
    PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG0,
#endif
    PPG_FIFO_PARSER_STATUS_RX0_RAWDATA0,
    PPG_FIFO_PARSER_STATUS_RX1_RAWDATA0,
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    PPG_FIFO_PARSER_STATUS_RX0_DC_INFO,
    PPG_FIFO_PARSER_STATUS_RX1_DC_INFO,
#endif
#if  GH_SUPPORT_FIFO_CTRL_DEBUG1
    PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG1,
    PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG1,
    PPG_FIFO_PARSER_STATUS_RX0_RAWDATA1,
    PPG_FIFO_PARSER_STATUS_RX1_RAWDATA1,
    PPG_FIFO_PARSER_STATUS_RX0_DEBUG1_BG2,
    PPG_FIFO_PARSER_STATUS_RX1_DEBUG1_BG2,
    PPG_FIFO_PARSER_STATUS_RX0_DEBUG2_DRE,
    PPG_FIFO_PARSER_STATUS_RX1_DEBUG2_DRE,
#endif
    PPG_FIFO_PARSER_STATUS_MAX,
} gh3036_parser_status_e;

/**
  * @brief fifo data id
  */
typedef enum
{
#if GH_SUPPORT_FIFO_CTRL_DEBUG0
    GH_FIFO_PPG_PARAM_RX0,
    GH_FIFO_PPG_PARAM_RX1,
#endif
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    GH_FIFO_PPG_BG0_RX0,
    GH_FIFO_PPG_BG0_RX1,
#endif
    GH_FIFO_PPG_MIX0_RX0,
    GH_FIFO_PPG_MIX0_RX1,
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    GH_FIFO_PPG_DRE_DC_INFO_RX0,
    GH_FIFO_PPG_DRE_DC_INFO_RX1,
#endif
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    GH_FIFO_PPG_BG1_RX0,
    GH_FIFO_PPG_BG1_RX1,
    GH_FIFO_PPG_MIX1_RX0,
    GH_FIFO_PPG_MIX1_RX1,
    GH_FIFO_PPG_BG2_RX0,
    GH_FIFO_PPG_BG2_RX1,
    GH_FIFO_PPG_DRE_RX0,
    GH_FIFO_PPG_DRE_RX1,
#endif
    GH_FIFO_PPG_DATA_MAX,
} gh3036_fifo_data_id_e;

/**
  * @brief ppg raw data structure.
  */
typedef struct
{
    int32_t rawdata : 24;
    uint32_t dre_update : 1;
    uint32_t param_change_flag : 1;
    uint32_t sa_flag : 1;
    uint32_t rx_id : 1;
    uint32_t config_id : 4;
} __attribute__ ((packed)) gh3036_ppg_rawdata_t;

/**
  * @brief ppg raw data structure.
  */
typedef struct
{
    uint32_t dc_cancel_code : 8;
    int32_t rawdata_dc : 19;
    uint32_t rx_id : 1;
    uint32_t config_id : 4;
} __attribute__ ((packed)) gh3036_ppg_dc_info_t;

/**
  * @brief ppg param structure.
  */
typedef struct
{
    uint32_t bg_cancel_code : 8;
    uint32_t dc_cancel_code : 8;
    uint32_t bg_cancel_range : 2;
    uint32_t tia_gain : 3;
    uint32_t dc_cancel_range : 2;
    uint32_t res3 : 1;
    uint32_t res2 : 1;
    uint32_t skip_ok_flag : 1;
    uint32_t res1 : 1;
    uint32_t rx_id : 1;
    uint32_t config_id : 4;

} __attribute__ ((packed)) gh3036_ppg_param_t;

/**
  * @brief ppg bg data structure.
  */
typedef struct
{
    int32_t  rawdata : 24;
    uint32_t res : 3;
    uint32_t rx_id : 1;
    uint32_t config_id : 4;
} __attribute__ ((packed)) gh3036_ppg_bgdata_t;

/**
  * @brief ppg dre data structure.
  */
typedef struct
{
    int32_t  rawdata : 24;
    uint32_t dre_update : 1;
    uint32_t res : 2;
    uint32_t rx_id : 1;
    uint32_t config_id : 4;
} __attribute__ ((packed)) gh3036_ppg_dredata_t;

/**
  * @brief cap data structure.
  */
typedef struct
{
    int32_t  rawdata : 24;
    uint32_t res : 4;
    uint32_t config_id : 4;
} __attribute__ ((packed)) gh3036_capdata_t;

/**
  * @brief fifo data structure.
  */
typedef union
{
    uint32_t             fifo_data;
    gh3036_ppg_rawdata_t ppg_rawdata;
#if GH_SUPPORT_FIFO_CTRL_DEBUG0
    gh3036_ppg_param_t   ppg_param;
#endif
#if GH_SUPPORT_FIFO_CTRL_DEBUG1
    gh3036_ppg_rawdata_t ppg_mixdata;
    gh3036_ppg_bgdata_t  ppg_bgdata;
    gh3036_ppg_dredata_t ppg_dredata;
#endif
#if GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
    gh3036_ppg_dc_info_t ppg_dc_info;
#endif
    gh3036_capdata_t     capdata;
} __attribute__ ((packed)) gh3036_fifo_data_t;

/**
  * @brief rawdata flags record
  */
typedef struct
{
    uint8_t     sa_flag : 1;
    uint8_t     param_change_flag : 1;
    uint8_t     dre_update : 1;
} __attribute__ ((packed)) gh3036_rawdata_flag_t;

/**
  * @brief fifo parser structure.
  */
typedef struct
{
    uint8_t*                     p_fifo_buffer;
    gh_data_t*                   p_data_buffer;
    uint16_t                     fifo_buffer_index;
    uint16_t                     fifo_buffer_size;
    uint16_t                     bulk_len;
    gh_hal_settings_t*           p_settings;
    gh3036_parser_status_e       parser_status;
    gh3036_slot_cfg_id_e         current_cfg;
    gh_data_get_callback_t       p_ghdata_get_callback;
    gh_data_get_callback_t       p_ghdata_cali_callback;
    gh_data_get_callback_t       p_ghdata_agc_callback;
    uint64_t                     time_stamp_begin;
    uint64_t                     time_stamp_end;
    uint8_t                      ppg_data_cnt[PPG_SLOT_CFG_MAX_NUM][GH_FIFO_PPG_DATA_MAX];
    uint8_t                      cap_data_cnt;
    gh3036_rawdata_flag_t        rawdata_flag[GH_PPG_RX_NUM];
#if GH_PARAM_SYNC_UPDATE_EN
    uint16_t                     ppg_param_change_flag;//bit0:slot rx0; bit1:slot 0 rx1; bit2:slot 1 rx0...
#endif
} gh_fifo_parser_t;

/**
 * @fn     uint32_t gh_hal_fifo_init(gh_fifo_parser_t* p_fifo_parser,
 *                                   uint8_t* p_fifo_buffer,
 *                                   uint16_t fifo_buffer_size)
 *
 * @brief  fifo parse init
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser        pointer to fifo parser structure
 * @param[in]   p_fifo_buffer        pointer to data buffer
 * @param[in]   fifo_buffer_size     buffer size in bytes
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_fifo_init(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_fifo_buffer, uint16_t fifo_buffer_size,
                          gh_data_t *p_data_buffer, uint16_t bulk_len);

/**
 * @fn     uint32_t gh_fifo_call_back_register(gh_fifo_parser_t* p_fifo_parser,
 *                                             gh_data_get_callback_t p_ghdata_get_callback,
 *                                             gh_data_get_callback_t p_ghdata_cali_callback,
 *                                             gh_data_get_callback_t p_ghdata_agc_callback);
 *
 * @brief  register for callback function call back
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser                pointer to fifo parser structure
 * @param[in]   ghdata_get_callback        pointer to callback function pointer
 * @param[in]   ghdata_cali_callback       pointer to callback function pointer
 * @param[in]   ghdata_agc_callback        pointer to callback function pointer
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_fifo_call_back_register(gh_fifo_parser_t* p_fifo_parser,
                                    gh_data_get_callback_t ghdata_get_callback,
                                    gh_data_get_callback_t ghdata_cali_callback,
                                    gh_data_get_callback_t ghdata_agc_callback);

/**
 * @fn     uint32_t gh_fifo_parse(gh_fifo_parser_t* p_fifo_parser,
 *                                uint8_t* p_buffer,
 *                                uint16_t buffer_len);
 *
 * @brief  parse process for data get from fifo buffer.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser                pointer to fifo parser structure
 * @param[in]   p_buffer                     pointer to buffer for data get from fifo.
 * @param[in]   buffer_len                   length of data get from fifo.
 *
 * @return  error code
 */
uint32_t gh_fifo_parse(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_buffer, uint16_t buffer_len);

/**
 * @fn     uint32_t gh_fifo_parse_for_agc(gh_fifo_parser_t* p_fifo_parser,
 *                                        uint8_t* p_buffer,
 *                                        uint16_t buffer_len);
 *
 * @brief  parse process for data get from fifo buffer.
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser                pointer to fifo parser structure
 * @param[in]   p_buffer                     pointer to buffer for data get from fifo.
 * @param[in]   buffer_len                   length of data get from fifo.
 *
 * @return  error code
 */
uint32_t gh_fifo_parse_for_agc(gh_fifo_parser_t* p_fifo_parser, uint8_t* p_buffer, uint16_t buffer_len);

/**
 * @fn     uint32_t gh_fifo_parse_reset(gh_fifo_parser_t* p_fifo_parser);
 *
 * @brief  reset fifo parser structure
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser                pointer to fifo parser structure
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_fifo_parse_reset(gh_fifo_parser_t* p_fifo_parser);

/**
 * @fn     uint32_t gh_fifo_parse_config(gh_fifo_parser_t* p_fifo_parser,
 *                                       uint16_t reg_addr, uint16_t reg_val);
 *
 * @brief  config register for fifo parser
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser              pointer to fifo parser structure
 * @param[in]   reg_addr                   register address for config.
 * @param[in]   reg_val                    value to write in the register.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_fifo_parse_config(gh_fifo_parser_t* p_fifo_parser, uint16_t reg_addr, uint16_t reg_val);

/**
 * @fn     uint32_t gh_fifo_timestamp_update(gh_fifo_parser_t* p_fifo_parser, uint32_t time_stamp);
 *
 * @brief  update timestamp in the fifo parser
 *
 * @attention   None
 *
 * @param[in]   p_fifo_parser              pointer to fifo parser structure
 * @param[in]   time_stamp                 new timestamp for fifo parser structure
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_fifo_timestamp_update(gh_fifo_parser_t* p_fifo_parser, uint64_t time_stamp);

/**
 * @fn     gh_fifo_parser_t* gh_hal_get_fifo_parser(void);
 *
 * @brief  get the FIFO parser instance.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  pointer to fifo parser structure
 */
gh_fifo_parser_t* gh_hal_get_fifo_parser(void);

/**
 * @fn    void gh_data_get_callback(gh_data_t* p_gh_data);
 *
 * @brief    get data from gh_data structure
 *
 * @attention   None
 *
 * @param[in]   p_gh_data      pointer to data structure
 * @param[out]  None
 *
 * @return  error code
 */
void gh_data_get_callback(gh_data_t* p_gh_data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __GH_HAL_FIFO_PARSER_H__ */
