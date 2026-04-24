#ifndef __GH_HAL_CONTROL_H__
#define __GH_HAL_CONTROL_H__

#include "gh_hal_settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_CTRL_REG_FIFO_FULL_TIME_ADRS    (0x1120)

typedef void (*gh_slot_disable_callback_t)(uint8_t slot_cfg_id);

/**
  * @brief slot cfg id of each slot
  */
typedef struct
{
    gh3036_slot_cfg_id_e slot_cfg_id;
} gh_hal_slot_index_t;

/**
  * @brief gh3036 control module structure
  */
typedef struct
{
    gh_hal_slot_index_t           slot_index[SLOT_CFG_MAX_NUM];
    gh_hal_settings_t*            p_settings;
    uint16_t                      fifo_watermark_time;
    gh_slot_disable_callback_t    p_slot_disable_callback;
} gh_hal_control_t;

/**
 * @fn     uint32_t gh_hal_control_init(gh_hal_control_t* p_hal_control);
 *
 * @brief  init control module.
 *
 * @attention   None
 *
 * @param[in]   p_hal_control                 pointer to control register structure.
 * @param[in]   p_slot_disable_callback       slot disable callback.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_control_init(gh_hal_control_t* p_hal_control, gh_slot_disable_callback_t p_slot_disable_callback);

/**
 * @fn     uint32_t gh_hal_control_reset(gh_hal_control_t* p_hal_control);
 *
 * @brief  reset control module.
 *
 * @attention   None
 *
 * @param[in]   p_hal_control        pointer to control register structure.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_control_reset(gh_hal_control_t* p_hal_control);

/**
 * @fn     uint32_t gh_control_top_ctrl(gh_hal_control_t* p_hal_control, uint8_t start);
 *
 * @brief  start sampling.
 *
 * @attention   None
 *
 * @param[in]   p_hal_control        pointer to control register structure.
 * @param[in]   start                0: stop sampling; 1: start sampling.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_control_top_ctrl(gh_hal_control_t* p_hal_control, uint8_t start);

/**
 * @fn     uint32_t gh_control_channel_en(gh_hal_control_t* p_hal_control,
 *                                        gh_hal_data_channel_t* p_data_channel);
 *
 * @brief  enable the specified channel.
 *
 * @attention   None
 *
 * @param[in]   p_hal_control        pointer to control register structure.
 * @param[in]   data_channel         the specified channel.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_control_channel_en(gh_hal_control_t* p_hal_control, gh_hal_data_channel_t* p_data_channel);

/**
 * @fn     uint32_t gh_hal_control_config(gh_hal_control_t* p_hal_control,
 *                                        uint16_t reg_addr, uint16_t reg_val);
 *
 * @brief  config the control register value.
 *
 * @attention   None
 *
 * @param[in]   p_hal_control        pointer to control register structure.
 * @param[in]   reg_addr             register address.
 * @param[in]   reg_val              register value.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_control_config(gh_hal_control_t* p_hal_control, uint16_t reg_addr, uint16_t reg_val);

/**
 * @fn     gh_hal_control_t* gh_hal_get_controller(void);
 *
 * @brief  get the control instance.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  pointer to control structure
 */
gh_hal_control_t* gh_hal_get_controller(void);

#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_CONTROL_H__*/

