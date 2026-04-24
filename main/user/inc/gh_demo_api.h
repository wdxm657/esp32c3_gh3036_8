#ifndef __GH_DEMO_API_H__
#define __GH_DEMO_API_H__

#include "gh_hal_interface.h"
#include "gh_global_config.h"
#include "gh_public_api.h"
#include "gh_hal_log.h"

#include "gh_protocol_user.h"
#include "gh_hal_user.h"
#include "gh_app_user.h"

#include "gh_demo_reg_lists.h"

void gh_app_demo_init(void);
void gh_app_demo_start(uint32_t func);
void gh_app_demo_stop(uint32_t func);   
void gh_app_demo_int_process(void);
void gh_app_demo_config_switch(uint8_t index);



#endif // !__GH_DEMO_API_H__
