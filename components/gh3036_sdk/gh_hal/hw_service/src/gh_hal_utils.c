#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_utils.h"




/*
 * DEFINES
 *****************************************************************************************
 */
#define GH_MEM_ALIGH_4_CHECK_OFFSET    30
#define GH_MEM_WORD_COPY_LEN           4
#define GH_MAKEUP_DWORD(byte3, byte2, byte1, byte0)     (((((uint32_t)(byte3)) << 24) & 0xFF000000U)| \
                                                          ((((uint32_t)(byte2)) << 16) & 0x00FF0000U) | \
                                                          ((((uint32_t)(byte1)) << 8) & 0x0000FF00U) | \
                                                           (((uint32_t)(byte0)) & 0x000000FFU))

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

void *gh_memcpy(void* p_dest, const void* p_src, uint32_t byte_size)
{
    if ((GH_NULL_PTR == p_dest) || (GH_NULL_PTR == p_src) || (0 == byte_size))
    {
        return GH_NULL_PTR;
    }

    uint8_t* p_src_op = (uint8_t *)p_src;
    uint8_t* p_dest_op = (uint8_t *)p_dest;
    uint32_t align = ((uint32_t)p_dest_op | (uint32_t)p_src_op) << GH_MEM_ALIGH_4_CHECK_OFFSET;

    if (0 == align) // if align 4
    {
        while (byte_size >= GH_MEM_WORD_COPY_LEN)
        {
            *(uint32_t *)p_dest_op = *(uint32_t *)p_src_op;
            p_src_op += GH_MEM_WORD_COPY_LEN;
            p_dest_op += GH_MEM_WORD_COPY_LEN;
            byte_size -= GH_MEM_WORD_COPY_LEN;
        }
    }

    while (byte_size) // bytes
    {
        *(uint8_t *)p_dest_op = *(uint8_t *)p_src_op;
        p_src_op++;
        p_dest_op++;
        byte_size--;
    }

    return p_dest;
}

void *gh_memset(void* p_dest, uint8_t val, uint32_t byte_size)
{
    if (GH_NULL_PTR == p_dest || 0 == byte_size)
    {
        return GH_NULL_PTR;
    }

    uint8_t* p_dest_op = (uint8_t *)p_dest;
    uint32_t align = ((uint32_t)p_dest_op) << GH_MEM_ALIGH_4_CHECK_OFFSET;
    uint32_t word_val = GH_MAKEUP_DWORD(val, val, val, val);

    if (0 == align) // if align 4
    {
        while (byte_size >= GH_MEM_WORD_COPY_LEN)
        {
            *(uint32_t *)p_dest_op = word_val;
            p_dest_op += GH_MEM_WORD_COPY_LEN;
            byte_size -= GH_MEM_WORD_COPY_LEN;
        }
    }

    while (byte_size) // bytes
    {
        *(uint8_t *)p_dest_op = (uint8_t)val;
        p_dest_op++;
        byte_size--;
    }

    return p_dest;
}

void *gh_memmove(void* p_dest, const void* p_src, uint32_t count)
{
    void *p_ret = p_dest;
    uint8_t *p_dest_op = (uint8_t *)p_dest;
    uint8_t *p_src_op = (uint8_t *)p_src;

    if (p_src < p_dest)
    {
        while (count)
        {
            *((uint8_t *)p_dest_op + count) = *((uint8_t *)p_src_op + count);
            count--;
        }
    }
    else
    {
        while (count)
        {
            *((uint8_t *)p_dest_op) = *((uint8_t *)p_src_op);
            p_dest_op++;
            p_src_op++;
            count--;
        }
    }

    return p_ret;
}

uint32_t gh_strlen(const char *sz_src)
{
    uint32_t cnt = 0;

    if (GH_NULL_PTR != sz_src)
    {
        while (0 != sz_src[cnt])
        {
            cnt++;
        }
    }

    return cnt;
}

int32_t gh_memcmp(const void *s1, const void *s2, uint32_t len)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (uint32_t i = 0; i < len; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

char *gh_strcpy(char *p_dest, const char *p_src)
{
    if ((GH_NULL_PTR == p_dest) || (GH_NULL_PTR == p_src))
    {
        return GH_NULL_PTR;
    }

    char *p_dest_op = p_dest;

    while (*p_src != '\0')
    {
        *p_dest_op = *p_src;
        p_dest_op++;
        p_src++;
    }
    *p_dest_op = '\0';

    return p_dest;
}

uint8_t gh_has_nonzero(const uint8_t *p_addr, uint32_t len)
{
    if (GH_NULL_PTR == p_addr || 0 == len)
    {
        return 0; // Return 0 if invalid parameters
    }

    const uint8_t *p_op = p_addr;
    uint32_t align = ((uint32_t)p_op) << GH_MEM_ALIGH_4_CHECK_OFFSET;

    // Check word-aligned if possible for better performance
    if (0 == align) // if align 4
    {
        while (len >= GH_MEM_WORD_COPY_LEN)
        {
            if (*(uint32_t *)p_op != 0)
            {
                return 1; // Found non-zero value
            }
            p_op += GH_MEM_WORD_COPY_LEN;
            len -= GH_MEM_WORD_COPY_LEN;
        }
    }

    // Check remaining bytes
    while (len)
    {
        if (*p_op != 0)
        {
            return 1; // Found non-zero value
        }
        p_op++;
        len--;
    }

    return 0; // All bytes are zero
}
