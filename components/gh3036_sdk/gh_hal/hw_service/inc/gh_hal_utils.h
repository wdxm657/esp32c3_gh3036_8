#ifndef __GH_HAL_UTILS_H__
#define __GH_HAL_UTILS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GH_NULL_PTR              ((void *) 0)

/**
 * @fn     void *gh_memcpy(void *dest, const void *src, uint32_t byte_size)
 *
 * @brief  memcpy() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   src        pointer to source buffer
 * @param[in]   byte_size  source buffer byte size
 * @param[out]  dest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *gh_memcpy(void *p_dest, const void *p_src, uint32_t byte_size);

/**
 * @fn     void *gh_memset(void *dest, uint8_t val, uint32_t byte_size)
 *
 * @brief  memset() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   val         char val for set
 * @param[in]   byte_size   source buffer len
 * @param[out]  p_dest      pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *gh_memset(void* p_dest, uint8_t val, uint32_t byte_size);

/**
 * @fn     void *gh_memmove(void* p_dest, const void* p_src, uint32_t count)
 *
 * @brief  memmove() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   p_src       source buffer address
 * @param[in]   count       source buffer len
 * @param[out]  dest        pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *gh_memmove(void* p_dest, const void* p_src, uint32_t count);

/**
 * @fn     uint32_t gh_strlen(const char *sz_src);
 *
 * @brief  strlen() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   sz_src      source string address
 *
 * @return  len of sz_src
 */
uint32_t gh_strlen(const char *sz_src);

/**
 * @fn     int32_t gh_memcmp(const void *s1, const void *s2, uint32_t n)
 *
 * @brief  memcmp() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   s1      pointer to buffer 1
 * @param[in]   s2      pointer to buffer 2
 * @param[in]   len     buffer len
 *
 * @return  0: s1 == s2; 1: s1 > s2; -1: s1 < s2
 */
int32_t gh_memcmp(const void *s1, const void *s2, uint32_t len);

/**
 * @fn     char *gh_strcpy(char *p_dest, const char *p_src)
 *
 * @brief  strcpy() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   p_src       source string address
 * @param[out]  p_dest      pointer to destination string buffer
 *
 * @return  pointer to destination string buffer
 */
char *gh_strcpy(char *p_dest, const char *p_src);

/**
 * @fn     uint8_t gh_has_nonzero(const uint8_t *p_addr, uint32_t len)
 *
 * @brief  Check if memory block has non-zero byte
 *
 * @attention   None
 *
 * @param[in]   p_addr      memory block address
 * @param[in]   len         memory block length in bytes
 *
 * @return  1: has non-zero byte; 0: all bytes are zero or invalid parameters
 */
uint8_t gh_has_nonzero(const uint8_t *p_addr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __GH_HAL_UTILS_H__ */
