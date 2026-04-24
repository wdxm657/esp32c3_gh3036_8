#ifndef __GH_DEMO_REG_LISTS_H__
#define __GH_DEMO_REG_LISTS_H__

#include <stdint.h>
#include "gh_public_api.h"
#include "gh_public_api.h"

typedef struct gh_reg_list_t
{
    const gh_config_reg_t* reg;
    uint16_t reg_size;
}gh_reg_list_t;

extern const gh_reg_list_t g_stGhCfgListArr[];
extern const uint8_t g_stGhCfgListNumber;

#endif // !__GH_DEMO_REG_LISTS_H__
