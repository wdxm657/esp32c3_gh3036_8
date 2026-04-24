#ifndef __GH_HAL_CONFIG_PROCESS_H__
#define __GH_HAL_CONFIG_PROCESS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn     uint32_t gh_hal_config_download(uint16_t addr, uint16_t val);
 *
 * @brief   download config register value.
 *
 * @attention   None
 *
 * @param[in]   addr         config register address.
 * @param[in]   val          config register value.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_config_download(uint16_t addr, uint16_t val);

#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_CONFIG_PROCESS_H__ */
