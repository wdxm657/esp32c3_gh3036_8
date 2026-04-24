#ifndef __GH_HAL_CHIP_H__
#define __GH_HAL_CHIP_H__

#include <stdint.h>
#include "gh_hal_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH3036_PPG_RX_NUM                       (2)
#define GH3036_LED_DRV_NUM                      (2)

#define GH_PPG_RX_NUM                           (GH3036_PPG_RX_NUM)
#define GH_LED_DRV_NUM                          (GH3036_LED_DRV_NUM)

#define GH3036_SLOT_INDEX_REG_NUM               (3)
#define GH3036_SLOT_INDEX_DEFAULT_VAL           (0xFF)
#define GH3036_CHIP_READY_VAL                   (0xAA55)
#define GH3036_CHIP_READY_CONFIRM_CNT           (5)

#define FASTEST_SAMPLE_RET_DEFAULT              (319)


#define PPG_TIA_GAIN_DEFAULT                    (3)
#define LED_DRV_FS_DEFAULT                      (40)
#define LED_DRV_FS_BASE                         (20)
#define LED_DRV_FS_MULTI                        (2)

#define GH3036_FIFO_WIDTH                       (4)
#define GH3036_FIFO_MAX_LEN                     (255)

#define RG_EFUSE_START                          (1)
#define RG_EFUSE_STOP                           (0)
#define RG_EFUSE_LED0_ER_ADDR                   (15)
#define RG_EFUSE_LED1_ER_ADDR                   (16)
#define RG_EFUSE_DC_CANCEL_ER1_ADDR             (17)
#define RG_EFUSE_DC_CANCEL_ER2_ADDR             (18)
#define RG_EFUSE_BG_CANCEL_ER1_ADDR             (19)
#define RG_EFUSE_BG_CANCEL_ER2_ADDR             (20)
#define RG_EFUSE_RX0_OFFSET_ER_ADDR             (21)
#define RG_EFUSE_RX1_OFFSET_ER_ADDR             (22)
#define RG_EFUSE_RX0_GAIN_ER_ADDR               (23)
#define RG_EFUSE_RX1_GAIN_ER_ADDR               (24)

#define GH3036_FIFOCTRL_MODE0                   (0x00)
#define GH3036_FIFOCTRL_MODE1                   (0x01)
#define GH3036_FIFOCTRL_MODE2                   (0x02)
#define GH3036_FIFOCTRL_MODE3                   (0x03)

#define GH3036_BG_ID0                           (0x00)
#define GH3036_BG_ID1                           (0x01)
#define GH3036_BG_ID2                           (0x02)

#define GH3036_BG_LEVEL0                        (0x00)
#define GH3036_BG_LEVEL1                        (0x01)
#define GH3036_BG_LEVEL2                        (0x02)

/**
 * @brief led drv idx enum
 */
typedef enum
{
    GH_LED_DRV0_IDX = 0,             /**< LED DRV0 IDX */
    GH_LED_DRV1_IDX = 1,             /**< LED DRV1 IDX */
    GH_LED_DRV2_IDX = 2,             /**< LED DRV2 IDX */
    GH_LED_DRV3_IDX = 3,             /**< LED DRV3 IDX */
} gh_led_drv_idx_e;


#if GH_HAL_AGC_DRE_EN
/**
 * @brief agc dc cancel idx enum
 */
typedef enum
{
    GH_AGC_DC0_IDX = 0,              /**< AGC DC CANCEL IDX */
    GH_AGC_DC1_IDX = 1,              /**< AGC DC CANCEL IDX */
} gh_agc_dc_cancel_idx_e;
#endif

/**
 * @brief tia gain idx enum
 */
typedef enum
{
    GH_TIA_GIAN0_IDX = 0,             /**< Tia gain 0 IDX */
    GH_TIA_GIAN1_IDX = 1,             /**< Tia gain 1 IDX */
    GH_TIA_GIAN2_IDX = 2,             /**< Tia gain 2 IDX */
    GH_TIA_GIAN3_IDX = 3,             /**< Tia gain 3 IDX */
} gh_tia_gain_idx_e;

/**
  * @brief GH3036 slot en structure.
  */
typedef struct
{
    uint16_t slot0_en : 1;
    uint16_t slot1_en : 1;
    uint16_t slot2_en : 1;
    uint16_t slot3_en : 1;
    uint16_t slot4_en : 1;
    uint16_t slot5_en : 1;
    uint16_t slot6_en : 1;
    uint16_t slot7_en : 1;
    uint16_t slot8_en : 1;
} __attribute__ ((packed)) slot_en_t;

/**
  * @brief GH3036 slot en union
  */
typedef union
{
    slot_en_t st_slot_en;
    uint16_t  slot_en;
} __attribute__ ((packed)) gh3036_slot_en_t;

/**
  * @brief GH3036 isr event enum.
  */
typedef enum
{
    GH_HAL_ISR_RESET = 0,
    GH_HAL_ISR_FIFO_UP = 1,
    GH_HAL_ISR_FIFO_DOWN = 2,
    GH_HAL_ISR_FIFO_WATER = 3,
    GH_HAL_ISR_TIMER = 4,
    GH_HAL_ISR_USER = 5,
    GH_HAL_ISR_FRAME_DONE = 6,
    GH_HAL_ISR_SAMPLE_ERROR = 7,
    GH_HAL_ISR_CAP_CANCEL = 8,
    GH_HAL_ISR_LDO_OC = 9,
    GH_HAL_ISR_SYNC_SAMPLE_ERROR = 10,
    GH_HAL_ISR_MAX
} gh_hal_isr_event_e;

typedef enum
{
    GH_HAL_INT_TIME_10_US = 0,
    GH_HAL_INT_TIME_20_US = 1,
    GH_HAL_INT_TIME_30_US = 2,
    GH_HAL_INT_TIME_39_US = 3,
    GH_HAL_INT_TIME_60_US = 4,
    GH_HAL_INT_TIME_79_US = 5,
    GH_HAL_INT_TIME_118_US = 6,
    GH_HAL_INT_TIME_158_US = 7,
    GH_HAL_INT_TIME_316_US = 8,
} gh_hal_int_time_e;

/**
  * @brief GH3036 chip init structure.
  */
typedef struct
{
    uint16_t chip_reset                     : 1;
    uint16_t fifo_up_overflow               : 1;
    uint16_t fifo_down_overflow             : 1;
    uint16_t fifo_waterline                 : 1;
    uint16_t timer_interrupt                : 1;
    uint16_t user_interrupt                 : 1;
    uint16_t frame_done                     : 1;
    uint16_t sample_rate_error              : 1;
    uint16_t cap_cancel_done                : 1;
    uint16_t ldo_oc                         : 1;
    uint16_t frame_sync_out_sample_rate_err : 1;
    uint16_t reserved                       : 5;
} __attribute__ ((packed)) gh_hal_isr_status_t;

/**
  * @brief PPG rx param structure.
  */
typedef struct
{
    uint8_t gain_code : 4;
    uint8_t dc_cancel_range : 2;
    uint8_t bg_cancel_range : 2;
    uint8_t dc_cancel_code;

} __attribute__ ((packed)) gh3036_ppg_rx_param_t;

/**
  * @brief GH3036 ppg cfg param structure.
  */
typedef struct
{
    uint8_t bg_level : 3;
    uint8_t dre_en : 1;
    uint8_t fifo_ctrl : 2;
    uint8_t rx_en : 2;
    uint8_t dc_cancel_en : 1;
    uint8_t dre_scale : 3;
    uint8_t dre_fifo_output_mode : 1;
    uint8_t reserved : 3;
    uint8_t led_drv_code[GH_LED_DRV_NUM];
#if GH_PARAM_BACKUP_EN
    uint8_t led_drv_code_backup[GH_LED_DRV_NUM];
#endif
    uint16_t multiplier;
    gh3036_ppg_rx_param_t rx_param[GH_PPG_RX_NUM];
#if GH_PARAM_BACKUP_EN
    gh3036_ppg_rx_param_t rx_param_backup[GH_PPG_RX_NUM];
#endif
#if GH_PARAM_SYNC_UPDATE_EN
    gh3036_ppg_rx_param_t rx_param_pre[GH_PPG_RX_NUM];
#endif

} __attribute__ ((packed)) gh3036_ppg_cfg_param_t;

/**
  * @brief GH3036 global cfg param structure.
  */
typedef struct
{
    uint8_t   led_drv_fs[GH_LED_DRV_NUM];
    uint16_t  cap_cfg_multiplier;
    uint16_t  fastest_sample_rate;

} __attribute__ ((packed)) gh3036_global_cfg_param_t;

#if GH_HAL_STD_CALI_EN
/**
  * @brief dc canccel cali param structure.
  */
typedef struct
{
    int8_t dc_cancel_a;
    int8_t dc_cancel_b;
} __attribute__ ((packed)) gh_std_cali_dc_param_t;

/**
  * @brief bg canccel cali param structure.
  */
typedef struct
{
    int8_t bg_cancel_a;
    int8_t bg_cancel_b;
} __attribute__ ((packed)) gh_std_cali_bg_param_t;

/**
  * @brief led cali param structure.
  */
typedef struct
{
    int8_t led0_er;
    int8_t led1_er;
} __attribute__ ((packed)) gh_std_cali_led_param_t;

/**
  * @brief rx offset param structure.
  */
typedef struct
{
    int8_t rx0_offset;
    int8_t rx1_offset;
} __attribute__ ((packed)) gh_std_cali_rx_param_t;

/**
  * @brief gain param structure.
  */
typedef struct
{
    int8_t gain_rx0_offset;
    int8_t gain_rx1_offset;
} __attribute__ ((packed)) gh_std_cali_gain_param_t;

/**
  * @brief cali param structure.
  */
typedef struct
{
#if GH_HAL_STD_CALI_DRV_EN
    gh_std_cali_led_param_t  led_param;
#endif

#if GH_HAL_STD_CALI_DC_CANCEL_EN
    gh_std_cali_dc_param_t   dc_cancel_param;
#endif

#if GH_HAL_STD_CALI_BG_CANCEL_EN
    gh_std_cali_bg_param_t   bg_cancel_param;
#endif

#if GH_HAL_STD_CALI_RX_OFFSET_EN
    gh_std_cali_rx_param_t   rx_param;
#endif

#if GH_HAL_STD_CALI_GAIN_EN
    gh_std_cali_gain_param_t gain_param;
#endif
} __attribute__ ((packed)) gh_hal_std_cali_param_t;
#endif

/**
 * @fn      uint32_t gh3036_set_slot_en(gh3036_slot_en_t* p_slot_en);
 *
 * @brief   set slot enable
 *
 * @attention   None
 *
 * @param[in]   p_slot_en                       slot enable structure pointer
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_set_slot_en(gh3036_slot_en_t* p_slot_en);

/**
 * @fn      uint32_t gh3036_set_fifo_watermark(uint16_t fifo_watermark);
 *
 * @brief   set fifo watermark
 *
 * @attention   None
 *
 * @param[in]   fifo_watermark                  fifo watermark
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_set_fifo_watermark(uint16_t fifo_watermark);

/**
 * @fn      uint32_t gh3036_set_slot_index(uint8_t* p_slot_index, uint16_t len);
 *
 * @brief   set slot index
 *
 * @attention   None
 *
 * @param[in]   p_slot_index                    slot index array pointer
 * @param[in]   len                             array length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_set_slot_index(uint8_t* p_slot_index, uint16_t len);

/**
 * @fn       uint32_t gh3036_top_start(uint8_t start);
 *
 *
 * @brief     top start
 *
 * @attention   None
 *
 * @param[in]   start                    1 : start 0 : stop
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_top_start(uint8_t start);

/**
 * @fn       uint32_t gh3036_global_cfg_parse(uint16_t reg_addr, uint16_t reg_val,
 *                                           gh3036_global_cfg_param_t* p_gh3036_global_cfg_param);
 *
 * @brief     get global config info from register value.
 *
 * @attention   None
 *
 * @param[in]   reg_addr                  register address.
 * @param[in]   reg_val                   register value.
 * @param[in]   p_gh3036_global_cfg_param pointer to global config structure.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_global_cfg_parse(uint16_t reg_addr, uint16_t reg_val,
                                 gh3036_global_cfg_param_t* p_gh3036_global_cfg_param);

/**
 * @fn       uint32_t gh3036_ppg_slot_config(uint16_t reg_addr, uint16_t reg_val, uint8_t slot_cfg_num,
                                gh3036_ppg_cfg_param_t* p_gh3036_ppg_cfg_param_t);
 *
 * @brief     get ppg slot config info from register value.
 *
 * @attention   None
 *
 * @param[in]   reg_addr                  register address.
 * @param[in]   reg_val                   register value.
 * @param[out]  p_gh3036_ppg_cfg_param_t  pointer to ppg config structure.
 *
 * @return  error code**/
uint32_t gh3036_ppg_slot_config(uint16_t reg_addr, uint16_t reg_val, uint8_t slot_cfg_num,
                                gh3036_ppg_cfg_param_t* p_gh3036_ppg_cfg_param_t);

/**
 * @fn       uint32_t gh3036_std_parse_config(uint16_t reg_addr, uint16_t reg_val, uint8_t slot_cfg_num,
                                uint8_t* dc_cancel_range_0, uint8_t* dc_cancel_range_1,
                                uint8_t* dc_cancel_code_0, uint8_t* dc_cancel_code_1);
 *
 * @brief     get fifo config info from register value.
 *
 * @attention   None
 *
 * @param[in]   reg_addr                 register address.
 * @param[in]   reg_val                  register value.
 * @param[in]   slot_cfg_num             slot config number.
 * @param[in]   dc_cancel_range_0        pointer to store the rx0 dc cancel range info.
 * @param[in]   dc_cancel_range_1        pointer to store the rx1 dc cancel range info.
 * @param[in]   dc_cancel_code_0         pointer to store the rx0 dc cancel code info.
 * @param[in]   dc_cancel_code_1         pointer to store the rx1 dc cancel code info.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_std_parse_config(uint16_t reg_addr,          uint16_t reg_val,     uint8_t slot_cfg_num,
                                 uint8_t* dc_cancel_range_0, uint8_t* dc_cancel_range_1,
                                 uint8_t* dc_cancel_code_0,  uint8_t* dc_cancel_code_1);

/**
 * @fn       uint32_t gh3036_agc_reg_parse(uint16_t reg_addr, uint16_t reg_val, uint8_t slot_cfg_num,
                                uint8_t* gain0_code, uint8_t* gain1_code,
                                uint8_t* led_drv0_code, uint8_t* led_drv1_code)
 *
 * @brief     get agc config info from register value.
 *
 * @attention   None
 *
 * @param[in]   reg_addr                 register address.
 * @param[in]   reg_val                  register value.
 * @param[in]   slot_cfg_num             slot config number.
 * @param[in]   gain0_code               pointer to store the gain0_code.
 * @param[in]   gain1_code               pointer to store the gain1_code.
 * @param[in]   led_drv0_code            pointer to store the led_drv0_code.
 * @param[in]   led_drv1_code            pointer to store the led_drv1_code.
 * @param[in]   dc_cancel_code0          pointer to store the dc_cancel_code0.
 * @param[in]   dc_cancel_code1          pointer to store the dc_cancel_code1.
 * @param[out]  None
 *
 * @return  error code
 */
#if GH_HAL_AGC_DRE_EN
uint32_t gh3036_agc_reg_parse(uint16_t reg_addr,   uint16_t reg_val, uint8_t slot_cfg_num,
                              uint8_t* gain0_code, uint8_t* gain1_code,
                              uint8_t* led_drv0_code, uint8_t* led_drv1_code,
                              uint8_t* dc_cancel_code0, uint8_t* dc_cancel_code1,
                              uint8_t* dre_enable);
#else
uint32_t gh3036_agc_reg_parse(uint16_t reg_addr,   uint16_t reg_val, uint8_t slot_cfg_num,
                              uint8_t* gain0_code, uint8_t* gain1_code,
                              uint8_t* led_drv0_code, uint8_t* led_drv1_code);
#endif

/**
 * @fn       uint32_t gh_hal_led_drv_write(uint8_t slotcfg_idx, uint8_t drv_idx, uint8_t drv_code);
 *
 * @brief   wirte led drv code
 *
 * @attention   None
 *
 * @param[in]   slotcfg_idx     slotcfg_idx: 0,1,2,3 ...7
 * @param[in]   drv_idx         led drv idx: 0, 1
 * @param[in]   drv_code        led drv code wirte to register
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_led_drv_write(uint8_t slotcfg_idx, uint8_t drv_idx, uint8_t drv_code);

/**
 * @fn       uint32_t gh_hal_tia_gain_write(uint8_t slotcfg_idx, uint8_t rx_idx, uint8_t gain_code);
 *
 * @brief   wirte led drv code
 *
 * @attention   None
 *
 * @param[in]   slotcfg_idx     slotcfg_idx: 0,1,2,3 ...7
 * @param[in]   rx_idx          Rx idx: 0, 1
 * @param[in]   gain_code       tia gain code wirte to register
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_tia_gain_write(uint8_t slotcfg_idx, uint8_t rx_idx, uint8_t gain_code);

/**
 * @fn       uint32_t gh_hal_param_update_start();
 *
 * @brief   param update start
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_param_update_start(void);

/**
 * @fn       uint32_t gh_hal_param_update_end();
 *
 * @brief   param update end
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_param_update_end(void);

/**
 * @fn       uint32_t gh_get_fifo_use(uint16_t* p_fifo_use);
 *
 * @brief   get fifo use
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  p_fifo_use               fifo use info
 *
 * @return  error code
 */
uint32_t gh_get_fifo_use(uint16_t* p_fifo_use);

/**
 * @fn       uint32_t gh_get_fifo_watermark(uint16_t* p_fifo_watermark);
 *
 * @brief   get fifo watermark
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  p_fifo_watermark         fifo watermark info
 *
 * @return  error code
 */
uint32_t gh_get_fifo_watermark(uint16_t* p_fifo_watermark);

/**
 * @fn     uint32_t gh_get_irq(gh_hal_isr_status_t* p_isr_status);
 *
 * @brief   get irq status
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  p_isr_status            irq status
 *
 * @return  error code
 */
uint32_t gh_get_irq(gh_hal_isr_status_t* p_isr_status);

/**
 * @fn     uint32_t gh_exit_lowpower_mode(void);
 *
 * @brief  enter low power mode
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_enter_lowpower_mode(void);

/**
 * @fn     uint32_t gh_exit_lowpower_mode(void);
 *
 * @brief  exit low power mode
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_exit_lowpower_mode(void);


/**
 * @fn     uint32_t gh_fifo_read(uint8_t* p_buffer, uint16_t* p_buffer_idx,
 *                    uint16_t buffer_size, uint32_t* p_fifo_use_byte_len,
 *                    uint8_t* p_continue);
 *
 * @brief  read data from FIFO, return the number of bytes read.
 *
 * @attention   None
 *
 * @param[in]   p_buffer              data read buffer
 * @param[in]   p_buffer_idx          data read led in the data read buffer.
 * @param[in]   buffer_size           data buffer max size.
 * @param[in]   p_fifo_use_byte_len   the number of bytes of fifo used.
 * @param[out]  p_continue            continue flag.
 *
 * @return  error code
 */
uint32_t gh_fifo_read(uint8_t* p_buffer, uint16_t* p_buffer_idx,
                      uint16_t buffer_size, uint32_t* p_fifo_use_byte_len,
                      uint8_t* p_continue);


/**
 * @fn     uint32_t gh_hal_chip_reset(void);
 *
 *
 * @brief   Reset the chip.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_chip_reset(void);

/**
 * @fn     uint32_t gh3036_ppg_multiplier_get(uint16_t reg_addr, uint16_t reg_val,
 *                                 uint16_t* p_multiplier, uint8_t multiplier_size);
 *
 * @brief  Get PPG multiplier.
 *
 * @attention   None
 *
 * @param[in]   reg_addr              register address
 * @param[in]   reg_val               register value
 * @param[in]   p_multiplier          pointer to store multiplier value
 * @param[in]   p_multiplier          pointer to store multiplier size
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_ppg_multiplier_get(uint16_t reg_addr, uint16_t reg_val,
                                   uint16_t* p_multiplier, uint8_t multiplier_size);

/**
 * @fn     uint32_t gh3036_cap_multiplier_get(uint16_t reg_addr, uint16_t reg_val, uint16_t* p_multiplier);
 *
 * @brief  Get cap multiplier.
 *
 * @attention   None
 *
 * @param[in]   reg_addr              register address
 * @param[in]   reg_val               register value
 * @param[in]   p_multiplier          pointer to store multiplier value
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_cap_multiplier_get(uint16_t reg_addr, uint16_t reg_val, uint16_t* p_multiplier);

/**
 * @fn     gh3036_set_led_code(uint8_t code, uint8_t drv_num, uint8_t slot_cfg_num);
 *
 * @brief  Set led code.
 *
 * @attention   None
 *
 * @param[in]   code              led code
 * @param[in]   drv_num           led driver number
 * @param[in]   slot_cfg_num      led driver slot cfg
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_set_led_code(uint8_t code, uint8_t drv_num, uint8_t slot_cfg_num);

/**
 * @fn     gh3036_set_dc_cancel_code(uint8_t code, uint8_t rx, uint8_t slot_cfg_num);
 *
 * @brief  Set led code.
 *
 * @attention   None
 *
 * @param[in]   code              dc cancel code
 * @param[in]   drv_num           dc cancel rx number
 * @param[in]   slot_cfg_num      dc cancel slot cfg
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh3036_set_dc_cancel_code(uint8_t code, uint8_t rx, uint8_t slot_cfg_num);

#if GH_HAL_STD_CALI_EN
/**
 * @fn     gh_hal_efuse_cali_param_get(gh_hal_std_cali_param_t* p_cali_param);
 *
 * @brief  Get efuse cali param.
 *
 * @attention   None
 *
 * @param[in]   p_cali_param      pointer of cali param
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_efuse_cali_param_get(gh_hal_std_cali_param_t* p_cali_param);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_CHIP_H__*/
