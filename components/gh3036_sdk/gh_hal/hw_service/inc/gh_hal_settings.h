#ifndef __GH_HAL_SETTINGS_H__
#define __GH_HAL_SETTINGS_H__

#include <stdint.h>
#include "gh_hal_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_SENSOR_MAIN_FREQ           (32000)
#define PPG_SLOT_CFG_MAX_NUM          (8)
#define PPG_SLOT_CFG_MAX_BYTE_NUM     ((PPG_SLOT_CFG_MAX_NUM + 7) / 8)

/**
  * @brief errot code
  */
typedef enum
{
    GH_SETTINGS_OK = 0,
    GH_SETTINGS_PTR_NULL = 1,
} gh_hal_settings_ret_e;

/**
  * @brief settings module structure.
  */
typedef struct gh_hal_settings
{
    gh3036_global_cfg_param_t   global_cfg_param;
    gh3036_ppg_cfg_param_t      ppg_cfg_param[PPG_SLOT_CFG_MAX_NUM];
    uint8_t                     ppg_cfg_en_bits[PPG_SLOT_CFG_MAX_BYTE_NUM];
} gh_hal_settings_t;

/**
  * @fn     uint32_t gh_hal_settings_init(gh_hal_settings_t* p_settings);
  *
  * @brief  settings module init.
  *
  * @attention   None
  *
  * @param[in]   p_settings      pointer of settings module structure.
  * @param[out]  None
  *
  * @return  error code
  */
uint32_t gh_hal_settings_init(gh_hal_settings_t* p_settings);

/**
  * @fn     uint32_t gh_hal_settings_reset(gh_hal_settings_t* p_settings);
  *
  * @brief  settings module reset.
  *
  * @attention   None
  *
  * @param[in]   p_settings      pointer of settings module structure.
  * @param[out]  None
  *
  * @return  error code
  */
uint32_t gh_hal_settings_reset(gh_hal_settings_t* p_settings);

/**
  * @fn     uint32_t gh_hal_settings_config(gh_hal_settings_t* p_settings, uint16_t reg_addr, uint16_t reg_val);
  *
  * @brief  settings module config.
  *
  * @attention   None
  *
  * @param[in]   p_settings      pointer of settings module structure.
  * @param[in]   reg_addr        register address.
  * @param[in]   reg_val         register value.
  *
  * @return  error code
  */
uint32_t gh_hal_settings_config(gh_hal_settings_t* p_settings, uint16_t reg_addr, uint16_t reg_val);

/**
  * @fn     gh_hal_settings_t* gh_hal_get_settings(void);
  *
  * @brief  get settings module structure.
  *
  * @attention   None
  *
  * @param       None.
  *
  * @return  pointer of settings module structure.
  */
gh_hal_settings_t* gh_hal_get_settings(void);

/**
  * @fn     uint32_t gh_hal_cal_slot_data_cnt(gh_hal_settings_t* p_settings, gh3036_slot_cfg_id_e slot_cfg_id, uint16_t* p_data_cnt);
  *
  * @brief  calculate the data count of slot.
  *
  * @attention   None
  *
  * @param[in]   p_settings      pointer of settings module structure.
  * @param[in]   slot_cfg_id     slot config id.
  * @param[out]  p_data_cnt      pointer of data count.
  *
  * @return  error code
  */
uint32_t gh_hal_cal_slot_data_cnt(gh_hal_settings_t* p_settings,
                                  gh3036_slot_cfg_id_e slot_cfg_id,
                                  uint16_t* p_data_cnt);
#ifdef __cplusplus
}
#endif

#endif  /* __GH_HAL_SETTINGS_H__ */
