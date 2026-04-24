#ifndef __GH_HAL_INTERFACE_H__
#define __GH_HAL_INTERFACE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GH3036_REG_WIDTH                                (2)

#define GH_CMD_RESET                                    (0xC2)
#define GH_CMD_WAKE_UP                                  (0xC3)
#define GH_CMD_SLEEP                                    (0xC4)

#define GH_HARD_RESET_PULSE_WIDTH                       (2)
#define GH_CMD_RESET_WAIT_TIME                          (7)
#define GH_CMD_WAKE_UP_WAIT_TIME                        (200)

/// macro of val clear bits
#define GH_VAL_CLEAR_BIT(x, b)                          ((x) &= (~(b)))

/// macro of val set bits
#define GH_VAL_SET_BIT(x, b)                            ((x) |= (b))

/// macro of val get bits
#define GH_VAL_GET_BIT(x, b)                            ((x) &= (b))

#define GH_GET_HIGH_BYTE_FROM_WORD(value)                ((uint8_t)(((uint16_t)(value)) >> 8))
#define GH_GET_LOW_BYTE_FROM_WORD(value)                 ((uint8_t)(((uint16_t)(value)) & 0xFF))

/**
  * @brief hal return value.
  */
typedef enum
{
    GH_HAL_OK           = 0,
    GH_HAL_PTR_NULL     = 1,
    GH_HAL_INIT_FAIL    = 2,
    GH_HAL_UNREG_ERR    = 3,
    GH_HAL_OP_ERR       = 4,
} gh_hal_ret_e;

/**
  * @brief DRV IO control value.
  */
typedef enum
{
    GH_IO_SET_LOW       = 0,
    GH_IO_SET_HIGH      = 1,
} gh_hal_io_state_e;

/**
  * @brief sleep status
  */
typedef enum
{
    GH_STATUS_SLEPP        = 0,
    GH_STATUS_WAKE_UP      = 1,
} gh_hal_sleep_state_e;

/**
 * @fn     uint32_t gh_hal_interface_init(void);
 *
 * @brief  initialize the interface.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_interface_init(void);

/**
 * @fn     gh_hal_reg_read(uint16_t reg_addr, uint16_t* p_reg_val);
 *
 * @brief  register read function
 *
 * @attention   None
 *
 * @param[in]   reg_addr          register address
 * @param[in]   p_reg_values      pointer of the array containing value for reading.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_reg_read(uint16_t reg_addr, uint16_t* p_reg_val);

/**
 * @fn     gh_hal_regs_read(uint16_t reg_addr, uint16_t* p_reg_buffer, uint16_t regs_num);
 *
 * @brief  register read function
 *
 * @attention   if (GH_INTERFACE_SPI_HW_CS == GH_INTERFACE_TYPE),
 *              regs_num should be less than 20
 *
 * @param[in]   reg_addr          register address
 * @param[in]   p_reg_values      pointer of the array containing values for reading.
 * @param[in]   regs_num          number of registers for reading.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_regs_read(uint16_t reg_addr, uint16_t* p_reg_buffer, uint16_t regs_num);

/**
 * @fn     uint32_t gh_hal_regs_write(uint16_t reg_addr, uint16_t* p_reg_values, uint16_t regs_num);
 *
 * @brief  register burst write function
 *
 * @attention   None
 *
 * @param[in]   reg_addr          register address
 * @param[in]   p_reg_values      pointer of the array containing values for writing.
 * @param[in]   regs_num          number of registers for writing.
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_regs_write(uint16_t reg_addr, uint16_t* p_reg_values, uint16_t regs_num);

/**
 * @fn     uint32_t gh_hal_reg_write(uint16_t reg_addr, uint16_t reg_value);
 *
 * @brief  write register value to device
 *
 * @attention   None
 *
 * @param[in]   reg_addr          register address
 * @param[in]   reg_value         write value
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_reg_write(uint16_t reg_addr, uint16_t reg_value);

/**
 * @fn     uint32_t gh_hal_reg_bit_field_write(uint16_t reg_addr, uint8_t lsb, uint8_t msb, uint16_t value);
 *
 * @brief  write bit field to register.
 *
 * @attention   None
 *
 * @param[in]   reg_addr          register address
 * @param[in]   lsb               lsb to read
 * @param[in]   msb               msb to read
 * @param[in]   value             write value
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_reg_bit_field_write(uint16_t reg_addr, uint8_t lsb, uint8_t msb, uint16_t value);

/**
 * @fn     uint32_t gh_hal_get_reg_field(uint16_t reg_val, uint8_t lsb, uint8_t msb, uint16_t* p_value);
 *
 * @brief  get reg field from register val and lsb,msb.
 *
 * @attention   None
 *
 * @param[in]   reg_val           register value
 * @param[in]   lsb               lsb to read
 * @param[in]   msb               msb to read
 * @param[out]  p_value           pointer to value
 *
 * @return  error code
 */
uint32_t gh_hal_get_reg_field(uint16_t reg_val, uint8_t lsb, uint8_t msb, uint16_t* p_value);

/**
 * @fn     uint32_t gh_hal_reg_bit_field_read(uint16_t reg_addr, uint8_t lsb,
 *                                            uint8_t msb, uint16_t* p_value);
 *
 * @brief  Reads a bit field from the specified register.
 *
 * @attention   None
 *
 * @param[in]   reg_addr          reg addr
 * @param[in]   lsb               lsb to read
 * @param[in]   msb               msb to read
 * @param[out]  p_value           pointer to value
 *
 * @return  error code
 */
uint32_t gh_hal_reg_bit_field_read(uint16_t reg_addr, uint8_t lsb, uint8_t msb, uint16_t* p_value);


/**
 * @fn     uint32_t gh_hal_cmd_write(uint8_t cmd);
 *
 * @brief  Write command.
 *
 * @attention   None
 *
 * @param[in]   cmd              command to write
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_cmd_write(uint8_t cmd);

/**
 * @fn     uint32_t gh_hal_fifo_read(uint8_t* p_buffer, uint16_t len);
 *
 * @brief  read data from fifo
 *
 * @attention   if (GH_INTERFACE_SPI_HW_CS == GH_INTERFACE_TYPE),
 *              the buffer size should be 1+len, and valid start address is p_buffer + 1
 *
 * @param[in]   p_buffer          pointer to buffer for read data
 * @param[in]   len               length of data to read
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_fifo_read(uint8_t* p_buffer, uint16_t len);

/**
 * @fn     uint32_t gh_enter_lowpower_mode(void);
 *
 * @brief  Enter low power mode.
 *
 * @attention   None
 *
 * @param[in]   None              register address
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_enter_lowpower_mode(void);

/**
 * @fn     uint32_t gh_exit_lowpower_mode(void);
 *
 * @brief  Exit low power mode.
 *
 * @attention   None
 *
 * @param[in]   None              register address
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_exit_lowpower_mode(void);

/**
 * @fn     uint32_t gh_soft_reset(void);
 *
 * @brief  Soft reset the chip
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_soft_reset(void);

/**
 * @fn     uint32_t gh_hard_reset(void);
 *
 * @brief  Hardware reset the chip
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hard_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __GH_HAL_INTERFACE_H__*/
