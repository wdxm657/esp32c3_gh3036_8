#ifndef GH_AUTO_CONFIG_H
#define GH_AUTO_CONFIG_H

#if defined(CONFIG_AUTO_CONFIG_H)
	#include CONFIG_AUTO_CONFIG_H
#else
    #include "auto_config.h"
#endif

/*
 * Consolidated auto configuration header
 * This file centralizes macro configuration extracted from:
 *  1) gh_hal_config.h
 *  2) gh_hal_log.h
 *  3) gh_global_config.h
 *
 * Each macro is defined with an #ifndef guard so users/build can override
 * the values by defining them earlier (e.g. in build system or project-level
 * config). Keep grouped sections in the same order as the source files.
 */

/* ------------------------------------------------------------------ */
/* 1) From gh_hal_config.h (interface / HAL related defaults)           */
/* ------------------------------------------------------------------ */

#if defined(CONFIG_GH_USE_SDK_APP)
#define GH_USE_SDK_APP                 (CONFIG_GH_USE_SDK_APP)
#else
#define GH_USE_SDK_APP                 (0)
#endif

#if defined(CONFIG_GH_ISR_MODE)
#define GH_ISR_MODE                    (CONFIG_GH_ISR_MODE)
#else
#define GH_ISR_MODE                    (0)  /* Default to INTERRUPT_MODE */
#endif

#if defined(CONFIG_GH_INTERFACE_TYPE)
#define GH_INTERFACE_TYPE              (CONFIG_GH_INTERFACE_TYPE)
#else
#define GH_INTERFACE_TYPE              (0)
#endif

/* FIFO buffer extra len depends on hardware CS SPI */
#if defined(GH_INTERFACE_TYPE) && (GH_INTERFACE_TYPE == 2)
	#if defined(CONFIG_GH_FIFO_BUFFER_EXTRA_LEN)
		#define GH_FIFO_BUFFER_EXTRA_LEN       (CONFIG_GH_FIFO_BUFFER_EXTRA_LEN)
	#else
		#define GH_FIFO_BUFFER_EXTRA_LEN       (1)
	#endif
#else
	#if defined(CONFIG_GH_FIFO_BUFFER_EXTRA_LEN)
		#define GH_FIFO_BUFFER_EXTRA_LEN       (CONFIG_GH_FIFO_BUFFER_EXTRA_LEN)
	#else
		#define GH_FIFO_BUFFER_EXTRA_LEN       (0)
	#endif
#endif

#if defined(GH_INTERFACE_TYPE) && (GH_INTERFACE_TYPE == 0)
	#if defined(CONFIG_GH_I2C_DEVICE_ID_SEL)
		#define GH_I2C_DEVICE_ID_SEL           (CONFIG_GH_I2C_DEVICE_ID_SEL) /* GH_I2C_ID_SEL_1L0L */
	#else
		#define GH_I2C_DEVICE_ID_SEL           (0) /* GH_I2C_ID_SEL_1L0L */
	#endif
#endif

#if defined(CONFIG_GH_FIFO_READ_BUFFER_SIZE)
#define GH_FIFO_READ_BUFFER_SIZE       (CONFIG_GH_FIFO_READ_BUFFER_SIZE * 4 + GH_FIFO_BUFFER_EXTRA_LEN)
#else
#define GH_FIFO_READ_BUFFER_SIZE       (0)
#endif

#if defined(CONFIG_GH_FIFO_DATA_BULK_LEN)
#define GH_FIFO_DATA_BULK_LEN          (CONFIG_GH_FIFO_DATA_BULK_LEN)
#else
#define GH_FIFO_DATA_BULK_LEN          (0)
#endif

#if defined(CONFIG_GH_USE_HAL_COMMON_LIB)
#define GH_USE_HAL_COMMON_LIB          (CONFIG_GH_USE_HAL_COMMON_LIB)
#else
#define GH_USE_HAL_COMMON_LIB          (0)
#endif

/* Select whether to use standard snprintf or SDK-provided one */
#if defined(CONFIG_GH_USE_STD_SNPRINTF)
#define GH_USE_STD_SNPRINTF            (CONFIG_GH_USE_STD_SNPRINTF)
#else
#define GH_USE_STD_SNPRINTF            (0)
#endif

#if defined(CONFIG_GH_SUPPORT_HARD_RESET)
#define GH_SUPPORT_HARD_RESET          (CONFIG_GH_SUPPORT_HARD_RESET)
#else
#define GH_SUPPORT_HARD_RESET          (0)
#endif

#if defined(CONFIG_GH_SUPPORT_SOFT_AGC)
#define GH_SUPPORT_SOFT_AGC            (CONFIG_GH_SUPPORT_SOFT_AGC)
#else
#define GH_SUPPORT_SOFT_AGC            (0)
#endif

#if defined(GH_SUPPORT_SOFT_AGC) && (GH_SUPPORT_SOFT_AGC)
	#if defined(CONFIG_GH_HAL_AGC_DRE_EN)
		#define GH_HAL_AGC_DRE_EN                   (CONFIG_GH_HAL_AGC_DRE_EN)
	#else
		#define GH_HAL_AGC_DRE_EN                   (0)
	#endif
#endif

#if defined(CONFIG_GH_SUPPORT_FIFO_CTRL_CAP)
#define GH_SUPPORT_FIFO_CTRL_CAP                (CONFIG_GH_SUPPORT_FIFO_CTRL_CAP)
#else
#define GH_SUPPORT_FIFO_CTRL_CAP                (0)
#endif
#if defined(CONFIG_GH_SUPPORT_FIFO_CTRL_DEBUG0)
#define GH_SUPPORT_FIFO_CTRL_DEBUG0             (CONFIG_GH_SUPPORT_FIFO_CTRL_DEBUG0)
#else
#define GH_SUPPORT_FIFO_CTRL_DEBUG0             (0)
#endif
#if defined(CONFIG_GH_SUPPORT_FIFO_CTRL_DEBUG1)
#define GH_SUPPORT_FIFO_CTRL_DEBUG1             (CONFIG_GH_SUPPORT_FIFO_CTRL_DEBUG1)
#else
#define GH_SUPPORT_FIFO_CTRL_DEBUG1             (0)
#endif
#if defined(CONFIG_GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO)
#define GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO        (CONFIG_GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO)
#else
#define GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO        (0)
#endif

/* HAL standard calibration: default to disabled unless set elsewhere */
#if defined(CONFIG_GH_HAL_STD_CALI_EN)
#define GH_HAL_STD_CALI_EN                    (CONFIG_GH_HAL_STD_CALI_EN)
#else
#define GH_HAL_STD_CALI_EN                    (0)
#endif

#if (GH_HAL_STD_CALI_EN)
		#if defined(CONFIG_GH_HAL_STD_CALI_DRV_EN)
			#define GH_HAL_STD_CALI_DRV_EN        (CONFIG_GH_HAL_STD_CALI_DRV_EN)
		#else
			#define GH_HAL_STD_CALI_DRV_EN        (1)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_DC_CANCEL_EN)
			#define GH_HAL_STD_CALI_DC_CANCEL_EN  (CONFIG_GH_HAL_STD_CALI_DC_CANCEL_EN)
		#else
			#define GH_HAL_STD_CALI_DC_CANCEL_EN  (1)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_BG_CANCEL_EN)
			#define GH_HAL_STD_CALI_BG_CANCEL_EN  (CONFIG_GH_HAL_STD_CALI_BG_CANCEL_EN)
		#else
			#define GH_HAL_STD_CALI_BG_CANCEL_EN  (1)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_RX_OFFSET_EN)
			#define GH_HAL_STD_CALI_RX_OFFSET_EN  (CONFIG_GH_HAL_STD_CALI_RX_OFFSET_EN)
		#else
			#define GH_HAL_STD_CALI_RX_OFFSET_EN  (1)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_GAIN_EN)
			#define GH_HAL_STD_CALI_GAIN_EN       (CONFIG_GH_HAL_STD_CALI_GAIN_EN)
		#else
			#define GH_HAL_STD_CALI_GAIN_EN       (1)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_RES_GAIN_EN)
			#define GH_HAL_STD_CALI_RES_GAIN_EN   (CONFIG_GH_HAL_STD_CALI_RES_GAIN_EN)
		#else
			#define GH_HAL_STD_CALI_RES_GAIN_EN   (1)
		#endif
#else
		#if defined(CONFIG_GH_HAL_STD_CALI_DRV_EN)
			#define GH_HAL_STD_CALI_DRV_EN        (CONFIG_GH_HAL_STD_CALI_DRV_EN)
		#else
			#define GH_HAL_STD_CALI_DRV_EN        (0)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_DC_CANCEL_EN)
			#define GH_HAL_STD_CALI_DC_CANCEL_EN  (CONFIG_GH_HAL_STD_CALI_DC_CANCEL_EN)
		#else
			#define GH_HAL_STD_CALI_DC_CANCEL_EN  (0)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_BG_CANCEL_EN)
			#define GH_HAL_STD_CALI_BG_CANCEL_EN  (CONFIG_GH_HAL_STD_CALI_BG_CANCEL_EN)
		#else
			#define GH_HAL_STD_CALI_BG_CANCEL_EN  (0)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_RX_OFFSET_EN)
			#define GH_HAL_STD_CALI_RX_OFFSET_EN  (CONFIG_GH_HAL_STD_CALI_RX_OFFSET_EN)
		#else
			#define GH_HAL_STD_CALI_RX_OFFSET_EN  (0)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_GAIN_EN)
			#define GH_HAL_STD_CALI_GAIN_EN       (CONFIG_GH_HAL_STD_CALI_GAIN_EN)
		#else
			#define GH_HAL_STD_CALI_GAIN_EN       (0)
		#endif
		#if defined(CONFIG_GH_HAL_STD_CALI_RES_GAIN_EN)
			#define GH_HAL_STD_CALI_RES_GAIN_EN   (CONFIG_GH_HAL_STD_CALI_RES_GAIN_EN)
		#else
			#define GH_HAL_STD_CALI_RES_GAIN_EN   (0)
		#endif
#endif

#if defined(CONFIG_GH_FIFO_USE_WATERMARK_LIMIT)
#define GH_FIFO_USE_WATERMARK_LIMIT   (CONFIG_GH_FIFO_USE_WATERMARK_LIMIT)
#else
#define GH_FIFO_USE_WATERMARK_LIMIT   (0)
#endif

#if defined(CONFIG_GH_PARAM_BACKUP_EN)
#define GH_PARAM_BACKUP_EN            (CONFIG_GH_PARAM_BACKUP_EN)
#else
#define GH_PARAM_BACKUP_EN            (0)
#endif

#if defined(CONFIG_GH_PARAM_SYNC_UPDATE_EN)
#define GH_PARAM_SYNC_UPDATE_EN       (CONFIG_GH_PARAM_SYNC_UPDATE_EN)
#else
#define GH_PARAM_SYNC_UPDATE_EN       (0)
#endif

/* ------------------------------------------------------------------ */
/* 2) From gh_hal_log.h (log related defaults)                         */
/* ------------------------------------------------------------------ */

/* Centralize log debug enable: other headers expect GH_LOG_DEBUG_ENABLE */
#ifndef GH_LOG_DEBUG_ENABLE
	#if defined(CONFIG_GH_LOG_DEBUG_ENABLE)
		#define GH_LOG_DEBUG_ENABLE           (CONFIG_GH_LOG_DEBUG_ENABLE)
	#else
		#define GH_LOG_DEBUG_ENABLE           (0)
	#endif
#endif

/* Per-module log enables (set to 0 by default; when GH_LOG_DEBUG_ENABLE==1 you may want to enable some) */
#if defined(GH_LOG_DEBUG_ENABLE) && (GH_LOG_DEBUG_ENABLE)
#ifndef GH_APP_LOG_EN
	#if defined(CONFIG_GH_APP_LOG_EN)
		#define GH_APP_LOG_EN                (CONFIG_GH_APP_LOG_EN)
	#else
		#define GH_APP_LOG_EN                (0)
	#endif
#endif
#ifndef GH_APP_MIPS_LOG_EN
	#if defined(CONFIG_GH_APP_MIPS_LOG_EN)
		#define GH_APP_MIPS_LOG_EN           (CONFIG_GH_APP_MIPS_LOG_EN)
	#else
		#define GH_APP_MIPS_LOG_EN           (0)
	#endif
#endif
#ifndef GH_APP_MANAGER_LOG_EN
	#if defined(CONFIG_GH_APP_MANAGER_LOG_EN)
		#define GH_APP_MANAGER_LOG_EN        (CONFIG_GH_APP_MANAGER_LOG_EN)
	#else
		#define GH_APP_MANAGER_LOG_EN        (0)
	#endif
#endif
#ifndef GH_APP_ALGO_LOG_EN
	#if defined(CONFIG_GH_APP_ALGO_LOG_EN)
		#define GH_APP_ALGO_LOG_EN           (CONFIG_GH_APP_ALGO_LOG_EN)
	#else
		#define GH_APP_ALGO_LOG_EN           (0)
	#endif
#endif
#ifndef GH_APP_FUSION_LOG_EN
	#if defined(CONFIG_GH_APP_FUSION_LOG_EN)
		#define GH_APP_FUSION_LOG_EN         (CONFIG_GH_APP_FUSION_LOG_EN)
	#else
		#define GH_APP_FUSION_LOG_EN         (0)
	#endif
#endif
#ifndef GH_APP_MOVE_DET_LOG_EN
	#if defined(CONFIG_GH_APP_MOVE_DET_LOG_EN)
		#define GH_APP_MOVE_DET_LOG_EN       (CONFIG_GH_APP_MOVE_DET_LOG_EN)
	#else
		#define GH_APP_MOVE_DET_LOG_EN       (0)
	#endif
#endif
#ifndef GH_APP_MTSS_LOG_EN
	#if defined(CONFIG_GH_APP_MTSS_LOG_EN)
		#define GH_APP_MTSS_LOG_EN           (CONFIG_GH_APP_MTSS_LOG_EN)
	#else
		#define GH_APP_MTSS_LOG_EN           (0)
	#endif
#endif
#ifndef GH_MODULE_FIFO_LOG_EN
	#if defined(CONFIG_GH_MODULE_FIFO_LOG_EN)
		#define GH_MODULE_FIFO_LOG_EN        (CONFIG_GH_MODULE_FIFO_LOG_EN)
	#else
		#define GH_MODULE_FIFO_LOG_EN        (0)
	#endif
#endif
#ifndef GH_MODULE_ISR_LOG_EN
	#if defined(CONFIG_GH_MODULE_ISR_LOG_EN)
		#define GH_MODULE_ISR_LOG_EN         (CONFIG_GH_MODULE_ISR_LOG_EN)
	#else
		#define GH_MODULE_ISR_LOG_EN         (0)
	#endif
#endif
#ifndef GH_HAL_SERVICE_LOG_EN
	#if defined(CONFIG_GH_HAL_SERVICE_LOG_EN)
		#define GH_HAL_SERVICE_LOG_EN        (CONFIG_GH_HAL_SERVICE_LOG_EN)
	#else
		#define GH_HAL_SERVICE_LOG_EN        (0)
	#endif
#endif
#ifndef GH_HAL_CONFIG_LOG_EN
	#if defined(CONFIG_GH_HAL_CONFIG_LOG_EN)
		#define GH_HAL_CONFIG_LOG_EN         (CONFIG_GH_HAL_CONFIG_LOG_EN)
	#else
		#define GH_HAL_CONFIG_LOG_EN         (0)
	#endif
#endif
#ifndef GH_MODULE_PROTOCOL_LOG_EN
	#if defined(CONFIG_GH_MODULE_PROTOCOL_LOG_EN)
		#define GH_MODULE_PROTOCOL_LOG_EN    (CONFIG_GH_MODULE_PROTOCOL_LOG_EN)
	#else
		#define GH_MODULE_PROTOCOL_LOG_EN    (0)
	#endif
#endif
#ifndef GH_HAL_STD_LOG_EN
	#if defined(CONFIG_GH_HAL_STD_LOG_EN)
		#define GH_HAL_STD_LOG_EN            (CONFIG_GH_HAL_STD_LOG_EN)
	#else
		#define GH_HAL_STD_LOG_EN            (0)
	#endif
#endif
#ifndef GH_HAL_SETTINGS_LOG_EN
	#if defined(CONFIG_GH_HAL_SETTINGS_LOG_EN)
		#define GH_HAL_SETTINGS_LOG_EN       (CONFIG_GH_HAL_SETTINGS_LOG_EN)
	#else
		#define GH_HAL_SETTINGS_LOG_EN       (0)
	#endif
#endif
#ifndef GH_HAL_CONTROL_LOG_EN
	#if defined(CONFIG_GH_HAL_CONTROL_LOG_EN)
		#define GH_HAL_CONTROL_LOG_EN        (CONFIG_GH_HAL_CONTROL_LOG_EN)
	#else
		#define GH_HAL_CONTROL_LOG_EN        (0)
	#endif
#endif
#ifndef GH_AGC_LOG_EN
	#if defined(CONFIG_GH_AGC_LOG_EN)
		#define GH_AGC_LOG_EN                (CONFIG_GH_AGC_LOG_EN)
	#else
		#define GH_AGC_LOG_EN                (0)
	#endif
#endif
#endif
/* ------------------------------------------------------------------ */
/* 3) From gh_global_config.h (app / algorithm related defaults)       */
/* ------------------------------------------------------------------ */

/* Chip selection: default to 3036 if not set by project */
#ifndef GH_CHIP_TYPE
	#if defined(CONFIG_GH_CHIP_TYPE)
		#define GH_CHIP_TYPE                                (CONFIG_GH_CHIP_TYPE)
	#else
		#define GH_CHIP_TYPE                                (0)
	#endif
#endif

/* Function enables (grouped defaults) */
#ifndef GH_FUNC_ADT_EN
	#if defined(CONFIG_GH_FUNC_ADT_EN)
		#define GH_FUNC_ADT_EN                                (CONFIG_GH_FUNC_ADT_EN)
	#else
		#define GH_FUNC_ADT_EN                                (0)
	#endif
#endif
#ifndef GH_FUNC_HR_EN
	#if defined(CONFIG_GH_FUNC_HR_EN)
		#define GH_FUNC_HR_EN                                 (CONFIG_GH_FUNC_HR_EN)
	#else
		#define GH_FUNC_HR_EN                                 (0)
	#endif
#endif
#ifndef GH_FUNC_SPO2_EN
	#if defined(CONFIG_GH_FUNC_SPO2_EN)
		#define GH_FUNC_SPO2_EN                               (CONFIG_GH_FUNC_SPO2_EN)
	#else
		#define GH_FUNC_SPO2_EN                               (0)
	#endif
#endif
#ifndef GH_FUNC_HRV_EN
	#if defined(CONFIG_GH_FUNC_HRV_EN)
		#define GH_FUNC_HRV_EN                                (CONFIG_GH_FUNC_HRV_EN)
	#else
		#define GH_FUNC_HRV_EN                                (0)
	#endif
#endif
#ifndef GH_FUNC_GNADT_EN
	#if defined(CONFIG_GH_FUNC_GNADT_EN)
		#define GH_FUNC_GNADT_EN                              (CONFIG_GH_FUNC_GNADT_EN)
	#else
		#define GH_FUNC_GNADT_EN                              (0)
	#endif
#endif
#ifndef GH_FUNC_IRNADT_EN
	#if defined(CONFIG_GH_FUNC_IRNADT_EN)
		#define GH_FUNC_IRNADT_EN                              (CONFIG_GH_FUNC_IRNADT_EN)
	#else
		#define GH_FUNC_IRNADT_EN                              (0)
	#endif
#endif

/* Algorithm adapter/enables */
#ifndef GH_ALGO_ADAPTER_EN
	#if defined(CONFIG_GH_ALGO_ADAPTER_EN)
		#define GH_ALGO_ADAPTER_EN                            (CONFIG_GH_ALGO_ADAPTER_EN)
	#else
		#define GH_ALGO_ADAPTER_EN                            (0)
	#endif
#endif
#ifndef GH_ALGO_EN
	#if defined(CONFIG_GH_ALGO_EN)
		#define GH_ALGO_EN                                    (CONFIG_GH_ALGO_EN)
	#else
		#define GH_ALGO_EN                                    (0)
	#endif
#endif

#ifndef GH_HR_ALGO_SUPPORT_CHNL_MAX
	#if defined(CONFIG_GH_HR_ALGO_SUPPORT_CHNL_MAX)
		#define GH_HR_ALGO_SUPPORT_CHNL_MAX                   (CONFIG_GH_HR_ALGO_SUPPORT_CHNL_MAX)
	#else
		#define GH_HR_ALGO_SUPPORT_CHNL_MAX                   (0)
	#endif
#endif
#ifndef GH_SPO2_ALGO_SUPPORT_CHNL_MAX
	#if defined(CONFIG_GH_SPO2_ALGO_SUPPORT_CHNL_MAX)
		#define GH_SPO2_ALGO_SUPPORT_CHNL_MAX                 (CONFIG_GH_SPO2_ALGO_SUPPORT_CHNL_MAX)
	#else
		#define GH_SPO2_ALGO_SUPPORT_CHNL_MAX                 (0)
	#endif
#endif

#ifndef GOODIX_HR_ALGO_VERISON
	#if defined(CONFIG_GOODIX_HR_ALGO_VERISON)
		#define GOODIX_HR_ALGO_VERISON                        (CONFIG_GOODIX_HR_ALGO_VERISON)
	#else
		#define GOODIX_HR_ALGO_VERISON                        (0)
	#endif
#endif
#ifndef GOODIX_SPO2_ALGO_VERISON
	#if defined(CONFIG_GOODIX_SPO2_ALGO_VERISON)
		#define GOODIX_SPO2_ALGO_VERISON                      (CONFIG_GOODIX_SPO2_ALGO_VERISON)
	#else
		#define GOODIX_SPO2_ALGO_VERISON                      (0)
	#endif
#endif

/* Fusion mode default */
#ifndef GH_FUSION_MODE_SEL
	#if defined(CONFIG_GH_FUSION_MODE_SEL)
		#define GH_FUSION_MODE_SEL                            (CONFIG_GH_FUSION_MODE_SEL)
	#else
		#define GH_FUSION_MODE_SEL                            (0)
	#endif
#endif

/* MTSS (multi-sensor) */
#ifndef GH_MTSS_EN
	#if defined(CONFIG_GH_MTSS_EN)
		#define GH_MTSS_EN                                    (CONFIG_GH_MTSS_EN)
	#else
		#define GH_MTSS_EN                                    (0)
	#endif
#endif

/* G-sensor move detect default */
#ifndef GH_GS_MOVE_DET_EN
	#if defined(CONFIG_GH_GS_MOVE_DET_EN)
		#define GH_GS_MOVE_DET_EN                             (CONFIG_GH_GS_MOVE_DET_EN)
	#else
		#define GH_GS_MOVE_DET_EN                             (0)
	#endif
#endif

/* FIFO read max */
#ifndef GH_FIFO_READ_MAX_NUM
	#if defined(CONFIG_GH_FIFO_READ_MAX_NUM)
		#define GH_FIFO_READ_MAX_NUM                          (CONFIG_GH_FIFO_READ_MAX_NUM)
	#else
		#define GH_FIFO_READ_MAX_NUM                          (0)
	#endif
#endif

/* Default demo logging */
#ifndef GH_DEMO_DATA_LOG_EN
	#if defined(CONFIG_GH_DEMO_DATA_LOG_EN)
		#define GH_DEMO_DATA_LOG_EN                           (CONFIG_GH_DEMO_DATA_LOG_EN)
	#else
		#define GH_DEMO_DATA_LOG_EN                           (0)
	#endif
#endif

/* By default disable app mips stats */
#ifndef GH_APP_MIPS_STA_EN
	#if defined(CONFIG_GH_APP_MIPS_STA_EN)
		#define GH_APP_MIPS_STA_EN                            (CONFIG_GH_APP_MIPS_STA_EN)
	#else
		#define GH_APP_MIPS_STA_EN                            (0)
	#endif
#endif

/* Keep a default for protocol enabling */
#ifndef GH_PROTOCOL_EN
	#if defined(CONFIG_GH_PROTOCOL_EN)
		#define GH_PROTOCOL_EN                                (CONFIG_GH_PROTOCOL_EN)
	#else
		#define GH_PROTOCOL_EN                                (0)
	#endif
#endif

/* End of consolidated macros */

#endif /* GH_AUTO_CONFIG_H */
