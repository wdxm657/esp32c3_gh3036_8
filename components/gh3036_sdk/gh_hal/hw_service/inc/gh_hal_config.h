#ifndef __GH_HAL_CONFIG_H__
#define __GH_HAL_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_GH_AUTO_CONFIG_H
#include CONFIG_GH_AUTO_CONFIG_H
#endif

/**
 * @brief i2c addr (7bits) low two bit select enum
 */
typedef enum
{
    GH_I2C_ID_SEL_1L0L = 0,             /**< i2c ID0 & ID1 pin low */
    GH_I2C_ID_SEL_1L0H = 1,             /**< i2c ID0 pin high & ID1 pin low */
    GH_I2C_ID_SEL_1H0L = 2,             /**< i2c ID0 pin low & ID1 pin high */
    GH_I2C_ID_SEL_1H0H = 3,             /**< i2c ID0 & ID1 pin high */
    GH_I2C_ID_INVALID  = 4,             /**< invalid val */
} gh_i2c_id_sel_e;

#ifndef GH_USE_SDK_APP
#define GH_USE_SDK_APP                  (1)
#endif

#define GH_INTERFACE_I2C                (0)
#define GH_INTERFACE_SPI_SW_CS          (1)
#define GH_INTERFACE_SPI_HW_CS          (2)

#define INTERRUPT_MODE                  (0)
#define POLLING_MODE                    (1)

#ifndef GH_ISR_MODE
#define GH_ISR_MODE                      INTERRUPT_MODE
#endif

#ifndef GH_INTERFACE_TYPE
#define GH_INTERFACE_TYPE                GH_INTERFACE_SPI_SW_CS
#endif

#ifndef GH_FIFO_BUFFER_EXTRA_LEN
    #if defined(GH_INTERFACE_TYPE) && (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)
        #define GH_FIFO_BUFFER_EXTRA_LEN         (1)
    #else
        #define GH_FIFO_BUFFER_EXTRA_LEN         (0)
    #endif
#endif

#if defined(GH_INTERFACE_TYPE) && (GH_INTERFACE_TYPE == GH_INTERFACE_I2C)
    #ifndef GH_I2C_DEVICE_ID_SEL
    #define GH_I2C_DEVICE_ID_SEL             (GH_I2C_ID_SEL_1L0L)
    #endif
#endif

#ifndef GH_FIFO_READ_BUFFER_SIZE
#define GH_FIFO_READ_BUFFER_SIZE         (255 * 4 + GH_FIFO_BUFFER_EXTRA_LEN)
#endif

#ifndef GH_FIFO_DATA_BULK_LEN
#define GH_FIFO_DATA_BULK_LEN            (1)
#endif

#ifndef GH_USE_HAL_COMMON_LIB
#define GH_USE_HAL_COMMON_LIB           (1)
#endif

#ifndef GH_USE_STD_SNPRINTF
#define GH_USE_STD_SNPRINTF              (1)
#endif

#ifndef GH_SUPPORT_HARD_RESET
#define GH_SUPPORT_HARD_RESET            (1)
#endif

#ifndef GH_SUPPORT_SOFT_AGC
#define GH_SUPPORT_SOFT_AGC              (1)
#endif

#if GH_SUPPORT_SOFT_AGC
    #ifndef GH_HAL_AGC_DRE_EN
    #define GH_HAL_AGC_DRE_EN                (0)
    #endif
#endif

#ifndef GH_SUPPORT_FIFO_CTRL_CAP
#define GH_SUPPORT_FIFO_CTRL_CAP                    (0)
#endif
#ifndef GH_SUPPORT_FIFO_CTRL_DEBUG0
#define GH_SUPPORT_FIFO_CTRL_DEBUG0                 (0)
#endif
#ifndef GH_SUPPORT_FIFO_CTRL_DEBUG1
#define GH_SUPPORT_FIFO_CTRL_DEBUG1                 (0)
#endif
#ifndef GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO
#define GH_SUPPORT_FIFO_CTRL_DRE_DC_INFO            (0)
#endif

/* define log enable */
#ifndef GH_LOG_DEBUG_ENABLE
#define GH_LOG_DEBUG_ENABLE              (0)
#endif

#if GH_HAL_STD_CALI_EN
#ifndef GH_HAL_STD_CALI_DRV_EN
#define GH_HAL_STD_CALI_DRV_EN          (1)
#endif
#ifndef GH_HAL_STD_CALI_DC_CANCEL_EN
#define GH_HAL_STD_CALI_DC_CANCEL_EN    (1)
#endif
#ifndef GH_HAL_STD_CALI_BG_CANCEL_EN
#define GH_HAL_STD_CALI_BG_CANCEL_EN    (1)
#endif
#ifndef GH_HAL_STD_CALI_RX_OFFSET_EN
#define GH_HAL_STD_CALI_RX_OFFSET_EN    (1)
#endif
#ifndef GH_HAL_STD_CALI_GAIN_EN
#define GH_HAL_STD_CALI_GAIN_EN         (1)
#endif
#ifndef GH_HAL_STD_CALI_RES_GAIN_EN
#define GH_HAL_STD_CALI_RES_GAIN_EN     (1)
#endif
#else
#define GH_HAL_STD_CALI_DRV_EN          (0)
#define GH_HAL_STD_CALI_DC_CANCEL_EN    (0)
#define GH_HAL_STD_CALI_BG_CANCEL_EN    (0)
#define GH_HAL_STD_CALI_RX_OFFSET_EN    (0)
#define GH_HAL_STD_CALI_GAIN_EN         (0)
#define GH_HAL_STD_CALI_RES_GAIN_EN     (0)
#endif

#ifndef GH_FIFO_USE_WATERMARK_LIMIT
#define GH_FIFO_USE_WATERMARK_LIMIT     (0)
#endif

#ifndef GH_PARAM_BACKUP_EN
#define GH_PARAM_BACKUP_EN              (0)
#endif

#ifndef GH_PARAM_SYNC_UPDATE_EN
#define GH_PARAM_SYNC_UPDATE_EN         (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_CONFIG_H__ */
