#include "gh_demo_api.h"

extern int8_t *gh_sdk_version_get(void);
#if (1 == GH_ALGO_EN)
extern int32_t goodix_hba_version(uint8_t version[150]);
extern int32_t goodix_spo2_version(uint8_t version[150]);
extern int32_t goodix_nadt_version(uint8_t version[150]);
extern int32_t goodix_hrv_version(uint8_t version[150]);
static uint8_t goodix_algo_version[150];
#endif


static gh_function_en_union_t s_func_en; 
static uint8_t current_cfg_index = 0xFF;

void gh_app_demo_init(void)
{
    if(GH_API_OK == gh_demo_init())
    {
        GH_LOG_LVL_DEBUG("gh demo init ok!\r\n");
    }
    
    if(GH_API_OK == gh_hal_service_init())
    {
      GH_LOG_LVL_DEBUG("gh hal service init ok!\r\n");  
    }
#if (1 == GH_PROTOCOL_EN)
    gh_protocol_init();
    GH_LOG_LVL_DEBUG("gh protocol init ok!\r\n");  
#endif

    GH_LOG_LVL_DEBUG("SDK version: %s\r\n", gh_sdk_version_get());

#if (1 == GH_ALGO_EN)
    goodix_hba_version(goodix_algo_version);
    GH_LOG_LVL_DEBUG("hba version: %s\r\n", goodix_algo_version);
    goodix_spo2_version(goodix_algo_version);
    GH_LOG_LVL_DEBUG("spo2 version: %s\r\n", goodix_algo_version);
    goodix_nadt_version(goodix_algo_version);
    GH_LOG_LVL_DEBUG("nadt version: %s\r\n", goodix_algo_version);
    goodix_hrv_version(goodix_algo_version);
    GH_LOG_LVL_DEBUG("hrv version: %s\r\n", goodix_algo_version);
#endif

    gh_hal_delay_ms(10);
    gh_app_demo_config_switch(0);
    gh_hal_delay_ms(10);
}

void gh_app_demo_start(uint32_t func)
{
    s_func_en.bits |= func;
    gh_assist_en_union_t assist;
    assist.bits = 0;
    assist.assist.assist_gsensor_en = 1;
    gh_demo_assist_config(&s_func_en, &assist);
    gh_demo_function_ctrl(&s_func_en);
}

void gh_app_demo_stop(uint32_t func)
{
    s_func_en.bits &= (~func);
    gh_demo_function_ctrl(&s_func_en);
}

void gh_app_demo_int_process(void)
{
    gh_demo_int_process();
}

void gh_app_demo_config_switch(uint8_t index)
{
    if ((index != current_cfg_index) && (index < g_stGhCfgListNumber)) {
        current_cfg_index = index;
        gh_demo_config_write ((gh_config_reg_t *) g_stGhCfgListArr[index].reg, g_stGhCfgListArr[index].reg_size);
    }
#if (GH_ISR_MODE == POLLING_MODE)
    gh_hal_reg_write(0x0500, 0x0003);
    gh_hal_reg_write(0x0020, 0x2905);
#endif
}
