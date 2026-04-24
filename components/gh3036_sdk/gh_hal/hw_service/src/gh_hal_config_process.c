#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_chip.h"
#include "gh_hal_fifo_parser.h"
#include "gh_hal_control.h"
#include "gh_hal_interface.h"
#include "gh_hal_isr.h"
#include "gh_hal_config_process.h"
#include "gh_hal_std.h"
#include "gh_hal_agc.h"
#include "gh_hal_settings.h"
#include "gh_hal_config.h"
#include "gh_hal_log.h"

#define GH_VIRTURAL_REG_BEGIN_ADDR                  (0x1000)

#if GH_HAL_CONFIG_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define GH_HAL_CONFIG_DEBUG_EN                      (1)

/**
 * @fn     static uint32_t gh_hal_reg_config(uint16_t addr, uint16_t val)
 *
 * @brief   Config register.
 *
 * @attention   None
 *
 * @param[in]   addr         config register address.
 * @param[in]   val          config register value.
 * @param[out]  None
 *
 * @return  error code
 */
static uint32_t gh_hal_reg_config(uint16_t addr, uint16_t val)
{
    uint32_t ret = 0;
    gh_hal_settings_t *p_settings = gh_hal_get_settings();
    gh_fifo_parser_t* p_fifo_parser = gh_hal_get_fifo_parser();
    gh_hal_control_t* p_hal_control = gh_hal_get_controller();
    gh_hal_std_parser_t* p_std_parser = gh_hal_get_std_parser();
#if GH_SUPPORT_SOFT_AGC
    gh_hal_agc_t* p_agc = gh_hal_get_agc();
#endif

    ret |= gh_hal_settings_config(p_settings, addr, val);
    ret |= gh_fifo_parse_config(p_fifo_parser, addr, val);
    ret |= gh_hal_control_config(p_hal_control, addr, val);
    ret |= gh_hal_std_parse_config(p_std_parser, addr, val);
#if GH_SUPPORT_SOFT_AGC
    ret |= gh_agc_config(p_agc, addr, val);
#endif
    return ret;
}

uint32_t gh_hal_config_download(uint16_t addr, uint16_t val)
{
    uint32_t ret = 0;

    ret |= gh_hal_reg_config(addr, val);
    if (addr < GH_VIRTURAL_REG_BEGIN_ADDR)
    {
        ret |= gh_hal_reg_write(addr, val);
#if GH_HAL_CONFIG_DEBUG_EN
        uint16_t read_val;
        ret |= gh_hal_reg_read(addr, &read_val);
        if (read_val != val)
        {
            DEBUG_LOG("confg write reg fail, addr = 0x%x, val = 0x%x, read val = 0x%x",
                      addr, val, read_val);
        }
#endif
    }

    return ret;
}
