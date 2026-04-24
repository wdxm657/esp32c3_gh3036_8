#ifndef __GH_HAL_ISR_H__
#define __GH_HAL_ISR_H__

#include <stdio.h>
#include <gh_hal_chip.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn     uint32_t gh_hal_isr(void);
 *
 * @brief  interrupt service routine
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_isr(void);

/**
 * @fn     uint32_t gh_hal_isr_chip_opr(void);
 *
 * @brief  interrupt service routine,only for chip operation
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_isr_chip_opr(void);

/**
 * @fn     uint32_t gh_hal_isr_fifo_parse(void);
 *
 * @brief  interrupt service routine,only for fifo parse
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 */
uint32_t gh_hal_isr_fifo_parse(void);

/**
 * @fn     uint32_t gh_hal_isr_event_publish(gh_hal_isr_status_t *p_event);
 *
 * @brief  interrupt event publish
 *
 * @attention   None
 *
 * @param[in]   p_event isr event
 * @param[out]  None
 *
 * @return  error code
 */
extern uint32_t gh_hal_isr_event_publish(gh_hal_isr_status_t *p_event);

#ifdef __cplusplus
}
#endif

#endif  /* __GH_HAL_ISR_H__ */
