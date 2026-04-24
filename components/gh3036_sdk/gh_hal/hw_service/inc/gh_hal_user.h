#ifndef __GH_HAL_USER_H__
#define __GH_HAL_USER_H__

#include <stdio.h>
#include "gh_hal_chip.h"
#include "gh_hal_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn    gh_hal_spi_cs_ctrl(uint8_t level);
 *
 * @brief  control cs pin
 *
 * @attention   None
 *
 * @param[in]   level                1:high, 0:low
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_spi_cs_ctrl(uint8_t level);

/**
 * @fn    uint32_t gh_hal_delay_us(uint16_t ms);
 *
 * @brief  delay us
 *
 * @attention   None
 *
 * @param[in]   ms                  delay time in us
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_delay_us(uint16_t us);

/**
 * @fn    uint32_t gh_hal_delay_ms(uint16_t ms);
 *
 * @brief  delay ms
 *
 * @attention   None
 *
 * @param[in]   ms                  delay time in ms
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_delay_ms(uint16_t ms);

#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS || GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)

/**
 * @fn    uint32_t gh_hal_spi_init(void);
 *
 * @brief  initialize spi bus.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_spi_init(void);

/**
 * @fn    uint32_t gh_hal_spi_write(uint8_t* p_buffer, uint16_t len);
 *
 * @brief   write data to spi bus.
 *
 * @attention   None
 *
 * @param[in]   p_buffer      pointer to tx data
 * @param[in]   len              data length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_spi_write(uint8_t* p_buffer, uint16_t len);
#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_SW_CS)

/**
 * @fn    uint32_t gh_hal_spi_read(uint8_t* p_buffer, uint16_t len);
 *
 * @brief     read data from spi bus.
 *
 * @attention   None
 *
 * @param[in]   p_buffer      pointer to rx data
 * @param[in]   len              data length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_spi_read(uint8_t* p_buffer, uint16_t len);
#endif
#if (GH_INTERFACE_TYPE == GH_INTERFACE_SPI_HW_CS)

/**
 * @fn    uint32_t gh_hal_spi_write_read(uint8_t* p_tx_buffer, uint8_t* p_rx_buffer, uint16_t len);
 *
 * @brief    SPI read and write function
 *
 * @attention   None
 *
 * @param[in]   p_tx_buffer      pointer to tx data
 * @param[in]   p_rx_buffer      pointer to rx data
 * @param[in]   len              data length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_spi_write_read(uint8_t* p_tx_buffer, uint8_t* p_rx_buffer, uint16_t len);
#endif

#elif (GH_INTERFACE_TYPE == GH_INTERFACE_I2C)

/**
 * @fn    uint32_t gh_hal_i2c_init(void);
 *
 * @brief  initialize i2c bus.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_i2c_init(void);

/**
 * @fn    uint32_t gh_hal_i2c_write(uint8_t* p_buffer, uint16_t len);
 *
 * @brief    I2C write function
 *
 * @attention   None
 *
 * @param[in]   i2c_slaver_id      i2c slaver id
 * @param[in]   p_buffer           pointer to tx data
 * @param[in]   len                data length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_i2c_write(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len);

/**
 * @fn    uint32_t gh_hal_i2c_read(uint8_t* p_buffer, uint16_t len);
 *
 * @brief    I2C write function
 *
 * @attention   None
 *
 * @param[in]   i2c_slaver_id      i2c slaver id
 * @param[in]   p_buffer           pointer to rx data
 * @param[in]   len                data length
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_i2c_read(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len);

#endif

#if (GH_USE_STD_SNPRINTF == 0)
/**
 * @fn     int gh_hal_snprintf_user(char *p_str, size_t size, const char *p_format, ...);
 *
 * @brief    snprintf user function
 *
 * @attention   None
 *
 * @param[in]   p_str         data to print
 * @param[in]   size          data to print length
 * @param[in]   p_format      print format string
 * @param[out]  None
 *
 * @return  error code
 */
int gh_hal_snprintf_user(char *p_str, size_t size, const char *p_format, ...);
#endif

/**
 * @fn     int gh_hal_log_user(char *p_str);
 *
 * @brief     log user entity.
 *
 * @attention   None
 *
 * @param[in]   p_str        data to print
 * @param[out]  None
 *
 * @return  error code
 */
int gh_hal_log_user(char *p_str);

/**
 * @fn     uint32_t gh_hal_int_pin_init(void);
 *
 * @brief     interrupt pin init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_int_pin_init(void);

/**
 * @fn     uint64_t gh_hal_get_timestamp(void);
 *
 * @brief   get timestamp in ms, return current time stamp if not set by user
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   timestamp in ms
 */
uint64_t gh_hal_get_timestamp(void);

#if GH_SUPPORT_HARD_RESET
/**
 * @fn     uint32_t gh_hal_reset_pin_init(void);
 *
 * @brief   init reset pin
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   error code
 */
uint32_t gh_hal_reset_pin_init(void);

/**
 * @fn     uint32_t gh_hal_reset_pin_ctrl(void);
 *
 * @brief  control reset pin
 *
 * @attention   None
 *
 * @param[in]   level                1:high, 0:low
 * @param[out]  None
 *
 * @return   error code
 */
uint32_t gh_hal_reset_pin_ctrl(uint8_t level);
#endif

/**
 * @fn     uint32_t gh_hal_isr_event_publish(void);
 *
 * @brief   publish isr event
 *
 * @attention   None
 *
 * @param[in]   p_event
 * @param[out]  None
 *
 * @return   None
 */
uint32_t gh_hal_isr_event_publish(gh_hal_isr_status_t *p_event);

/**
 * @fn       uint32_t gh_hal_fifo_data_publish(uint8_t *p_data, uint16_t size);
 *
 * @brief    publish fifo data
 *
 * @attention   None
 *
 * @param[in]   p_data                  pointer to data
 * @param[in]   size                    data size
 * @param[out]  None
 *
 * @return   None
 */
uint32_t gh_hal_fifo_data_publish(uint8_t *p_data, uint16_t size);


#ifdef __cplusplus
}
#endif

#endif  /* __GH_HAL_USER_H__ */
