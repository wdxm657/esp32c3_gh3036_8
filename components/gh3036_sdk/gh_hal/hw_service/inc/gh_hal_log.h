#ifndef __GH_HAL_LOG_H__
#define __GH_HAL_LOG_H__


#include "gh_hal_config.h"
#include "gh_hal_utils.h"

#if (GH_USE_STD_SNPRINTF == 1)
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if GH_LOG_DEBUG_ENABLE
/* Define logging enables only if not already defined to avoid redefinition */
#ifndef GH_APP_LOG_EN
#define GH_APP_LOG_EN                1
#endif

#ifndef GH_APP_MIPS_LOG_EN
#define GH_APP_MIPS_LOG_EN           0
#endif

#ifndef GH_APP_MANAGER_LOG_EN
#define GH_APP_MANAGER_LOG_EN        0
#endif

#ifndef GH_APP_ALGO_LOG_EN
#define GH_APP_ALGO_LOG_EN           0
#endif

#ifndef GH_APP_FUSION_LOG_EN
#define GH_APP_FUSION_LOG_EN         0
#endif

#ifndef GH_APP_MOVE_DET_LOG_EN
#define GH_APP_MOVE_DET_LOG_EN       0
#endif

#ifndef GH_APP_MTSS_LOG_EN
#define GH_APP_MTSS_LOG_EN           0
#endif

#ifndef GH_MODULE_FIFO_LOG_EN
#define GH_MODULE_FIFO_LOG_EN        0
#endif

#ifndef GH_MODULE_ISR_LOG_EN
#define GH_MODULE_ISR_LOG_EN         0
#endif

#ifndef GH_HAL_SERVICE_LOG_EN
#define GH_HAL_SERVICE_LOG_EN        0
#endif

#ifndef GH_HAL_CONFIG_LOG_EN
#define GH_HAL_CONFIG_LOG_EN         0
#endif

#ifndef GH_MODULE_PROTOCOL_LOG_EN
#define GH_MODULE_PROTOCOL_LOG_EN    0
#endif

#ifndef GH_HAL_STD_LOG_EN
#define GH_HAL_STD_LOG_EN            0
#endif

#ifndef GH_HAL_SETTINGS_LOG_EN
#define GH_HAL_SETTINGS_LOG_EN       0
#endif

#ifndef GH_HAL_CONTROL_LOG_EN
#define GH_HAL_CONTROL_LOG_EN        0
#endif

#ifndef GH_AGC_LOG_EN
#define GH_AGC_LOG_EN                0
#endif
#else
#define GH_APP_LOG_EN                0
#define GH_APP_MANAGER_LOG_EN        0
#define GH_APP_ALGO_LOG_EN           0
#define GH_APP_FUSION_LOG_EN         0
#define GH_APP_MOVE_DET_LOG_EN       0
#define GH_APP_MTSS_LOG_EN           0
#define GH_MODULE_FIFO_LOG_EN        0
#define GH_MODULE_ISR_LOG_EN         0
#define GH_HAL_SERVICE_LOG_EN        0
#define GH_HAL_CONFIG_LOG_EN         0
#define GH_MODULE_PROTOCOL_LOG_EN    0
#define GH_HAL_STD_LOG_EN            0
#define GH_HAL_SETTINGS_LOG_EN       0
#define GH_HAL_CONTROL_LOG_EN        0
#define GH_AGC_LOG_EN                0
#endif

#define GH_LOG_MAX_LEN               256
#define GH_LOG_LEVEL_NONE            0
#define GH_LOG_LEVEL_DEBUG           1
#define GH_LOG_LEVEL_WARNING         2
#define GH_LOG_LEVEL_ERROR           3
#define GH_LOG_DEBUG_TAG             "[GH][D]"
#define GH_LOG_WARNING_TAG           "[GH][W]"
#define GH_LOG_ERROR_TAG             "[GH][E]"


#if (GH_LOG_DEBUG_ENABLE)
    #ifdef CONFIG_GH_LOG_LEVEL_NONE
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_NONE
    #elif defined(CONFIG_GH_LOG_LEVEL_DEBUG)
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_DEBUG
    #elif defined(CONFIG_GH_LOG_LEVEL_WARNING)
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_WARNING
    #elif defined(CONFIG_GH_LOG_LEVEL_ERROR)
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_ERROR
    #else
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_ERROR
    #endif
#else
    #undef GH_LOG_LEVEL
    #define GH_LOG_LEVEL    GH_LOG_LEVEL_NONE
#endif

#if (GH_LOG_LEVEL == GH_LOG_LEVEL_NONE)
#define GH_LOG_LVL_DEBUG(...)
#define GH_LOG_LVL_WARNING(...)
#define GH_LOG_LVL_ERROR(...)
#else

int gh_hal_log_user(char *p_str);

#if (GH_USE_STD_SNPRINTF == 0)
// #define GH_SNPRINTF(buffer, size, format, ...)  gh_hal_snprintf_user(buffer, size, format, __VA_ARGS__)
#define GH_SNPRINTF(buffer, size, format, ...)    gh_hal_snprintf_user((buffer), (size), (format), __VA_ARGS__)
#else
#define GH_SNPRINTF                               snprintf
#endif

#if (GH_LOG_LEVEL >= GH_LOG_LEVEL_DEBUG)
#define GH_LOG_LVL_DEBUG(...)    do {\
                                        char gh_log[GH_LOG_MAX_LEN] = GH_LOG_DEBUG_TAG;\
                                        GH_SNPRINTF(&gh_log[gh_strlen(GH_LOG_DEBUG_TAG)], \
                                                    GH_LOG_MAX_LEN - gh_strlen(GH_LOG_DEBUG_TAG), \
                                                    __VA_ARGS__);\
                                        gh_hal_log_user((char *)gh_log);\
                                    } while (0)
#endif

#if (GH_LOG_LEVEL >= GH_LOG_LEVEL_WARNING)
#define GH_LOG_LVL_WARNING(...)    do {\
                                        char gh_log[GH_LOG_MAX_LEN] = GH_LOG_WARNING_TAG;\
                                        GH_SNPRINTF(&gh_log[gh_strlen(GH_LOG_WARNING_TAG)], \
                                                    GH_LOG_MAX_LEN - gh_strlen(GH_LOG_WARNING_TAG), \
                                                    __VA_ARGS__);\
                                        gh_hal_log_user((char *)gh_log);\
                                      } while (0)
#endif

#if (GH_LOG_LEVEL >= GH_LOG_LEVEL_ERROR)
#define GH_LOG_LVL_ERROR(...)    do {\
                                        char gh_log[GH_LOG_MAX_LEN] = GH_LOG_ERROR_TAG;\
                                        GH_SNPRINTF(&gh_log[gh_strlen(GH_LOG_ERROR_TAG)], \
                                                    GH_LOG_MAX_LEN - gh_strlen(GH_LOG_ERROR_TAG), \
                                                    __VA_ARGS__);\
                                        gh_hal_log_user((char *)gh_log);\
                                    } while (0)

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif  // __GH_HAL_LOG_H__
