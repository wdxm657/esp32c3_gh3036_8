#ifndef __GH_HAL_AGC_H__
#define __GH_HAL_AGC_H__

#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_settings.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DEFINES
 *****************************************************************************************
 */
#define  GH_SLOT_NUM                         (8u) /**< number of slotcfg. */
#define  GH_DRV_NUM                          (2) /**< number of led drv. */
#define  GH_RX_NUM                           (2) /**< number of Rx. */

#define GH_AGC_SLOT_NUM                      (8u) /**< number of agc slot. */
#define GH_AGC_SUB_CHNL_NUM                  ((GH_AGC_SLOT_NUM)*(GH_RX_NUM-1)) /**< . */

#define PPG_TIA_GAIN_DEFAULT                 (3)
#define PPG_LED_DRV_DEFAULT                  (0)
#define FASTEST_SAMPLE_RATE_DEFAULT          (0x13F)

#define GH_AGC_GENE_H_THD                    (uint16_t)(((int32_t)8388608 * 85 / 100)>>8) /**< . */
#define GH_AGC_GENE_L_THD                    (uint16_t)(((int32_t)8388608 * 3 / 10)>>8) /**< . */
#define GH_AGC_GENE_IDEAL_VALUE              (uint16_t)(((int32_t)8388608 * 56 / 100)>>8) /**< . */
#define GH_AGC_GENE_IDEAL_IPD                (200u) /**< . */
#define GH_AGC_GENE_H_IPD                    (306u) /**< . */
#define GH_AGC_GENE_L_IPD                    (100u) /**< . */
#if GH_HAL_AGC_DRE_EN
#define GH_AGC_DRE_GENE_H_THD                (uint16_t)(((int32_t)8388608 * 85 / 100)>>8) /**< . */
#define GH_AGC_DRE_GENE_L_THD                (uint16_t)(((int32_t)8388608 * 3 / 10)>>8) /**< . */
#define GH_AGC_DRE_GENE_IDEAL_VALUE          (uint16_t)(((int32_t)8388608 * 56 / 100)>>8) /**< . */
#endif
#define GH_AGC_SPO2_H_THD                    (uint16_t)(((int32_t)8388608 * 9 / 10)>>8) /**< . */
#define GH_AGC_SPO2_L_THD                    (uint16_t)(((int32_t)8388608 * 5 / 10)>>8) /**< . */
#define GH_AGC_SPO2_IDEAL_VALUE              (uint16_t)(((int32_t)8388608 * 7 / 10)>>8) /**< . */
#define GH_AGC_SPO2_IDEAL_IPD                (2000u) /**< . */
#define GH_AGC_SPO2_H_IPD                    (2600u) /**< . */
#define GH_AGC_SPO2_L_IPD                    (1500u) /**< . */
#define GH_AGC_CONVERGENCE_LEVEL             (3u) /**< The values that can be set are 2, 3 and 4, and the recommended value is 3. The larger the value, the smaller the convergence range of AGC. */

#define GH_AGC_SLOTCFG_MASK                  (0x1F)
#define GH_AGC_LOW8BIT_MASK                  (0x00FF)

#define GH_VOLT_REF_UA                       (900000)
#define GH_VOLT_REF_MA                       (900)
#define GH_AGC_RAWDATA_LEV0                  (8000000)
#define GH_AGC_RAWDATA_LEV1                  (8100000)

#define GH_AGC_IPD_NA_FACTOR                 (10)
#define GH_AGC_TIMES_FACTOR                  (128)
#define GH_AGC_THD24_FACTOR                  (256)

#define GH_AGC_ANALYSE_SEC                   (15)
#define GH_AGC_SAT_SEC                       (5)
#define GH_AGC_DROP_FACTOR                   (5)

#define GH_MAX_RAWDATA                       (8388608) /**< max rawdata. */
#define GH_MIN_RAWDATA                       (-8388608) /**< min rawdata. */
#define GH_DATA_BASE_LINE                    (0) /**< rawdata base line value. */

#define GH_SLOTCFG_SHIFT_NUM      (2)
#define GH_AGC_SUB_CHNNL_ANAYLSIS_SEC        (1) /**< . */

#define GH_SYS_SAMPLE_RATE                   (32000)

#define GH_VIRTURAL_REG_BEGIN_ADDR           (0x1000)
#define GH_VIRTURAL_REG_END_ADDR             (0xffff)
#define GH_REG_ADDR_SIZE                     (0x0002)

#define GH_AGC_REG_BASE_ADRS                 (0x3400)
#define AGC_REG_EN_LOW_OFFSET                (0x0000)
#define GH_AGC_REG_EN_LOW_ADRS               (GH_AGC_REG_BASE_ADRS + AGC_REG_EN_LOW_OFFSET)
#define AGC_REG_EN_HIGH_OFFSET               (0x0002)
#define GH_AGC_REG_EN_HIGH_ADRS              (GH_AGC_REG_BASE_ADRS + AGC_REG_EN_HIGH_OFFSET)
#define AGC_REG_GAIN_LIMIT_OFFSET            (0x0004)
#define GH_AGC_REG_GAIN_LIMIT_ADRS           (GH_AGC_REG_BASE_ADRS + AGC_REG_GAIN_LIMIT_OFFSET)
#define AGC_REG_SOP2_RED_LOW_OFFSET          (0x0006)
#define GH_AGC_REG_SOP2_RED_LOW_ADRS         (GH_AGC_REG_BASE_ADRS + AGC_REG_SOP2_RED_LOW_OFFSET)
#define AGC_REG_SOP2_RED_HIGH_OFFSET         (0x0008)
#define GH_AGC_REG_SOP2_RED_HIGH_ADRS        (GH_AGC_REG_BASE_ADRS + AGC_REG_SOP2_RED_HIGH_OFFSET)
#define AGC_REG_SOP2_IR_LOW_OFFSET           (0x000A)
#define GH_AGC_REG_SOP2_IR_LOW_ADRS          (GH_AGC_REG_BASE_ADRS + AGC_REG_SOP2_IR_LOW_OFFSET)
#define AGC_REG_SOP2_IR_HIGH_OFFSET          (0x000C)
#define GH_AGC_REG_SOP2_IR_HIGH_ADRS         (GH_AGC_REG_BASE_ADRS + AGC_REG_SOP2_IR_HIGH_OFFSET)

#define AGC_REG_GENE_HIGH_IPD_OFFSET         (0x000E)
#define GH_AGC_REG_GENE_HIGH_IPD_ADRS        (GH_AGC_REG_BASE_ADRS + AGC_REG_GENE_HIGH_IPD_OFFSET)
#define AGC_REG_GENE_LOW_IPD_OFFSET          (0x0010)
#define GH_AGC_REG_GENE_LOW_IPD_ADRS         (GH_AGC_REG_BASE_ADRS + AGC_REG_GENE_LOW_IPD_OFFSET)
#define AGC_REG_SPO2_HIGH_IPD_OFFSET         (0x0012)
#define GH_AGC_REG_SPO2_HIGH_IPD_ADRS        (GH_AGC_REG_BASE_ADRS + AGC_REG_SPO2_HIGH_IPD_OFFSET)
#define AGC_REG_SPO2_LOW_IPD_OFFSET          (0x0014)
#define GH_AGC_REG_SPO2_LOW_IPD_ADRS         (GH_AGC_REG_BASE_ADRS + AGC_REG_SPO2_LOW_IPD_OFFSET)

#define AGC_REG_GENE_HIGH_THLD_OFFSET        (0x0016)
#define GH_AGC_REG_GENE_HIGH_THLD_ADRS       (GH_AGC_REG_BASE_ADRS + AGC_REG_GENE_HIGH_THLD_OFFSET)
#define AGC_REG_GENE_LOW_THLD_OFFSET         (0x0018)
#define GH_AGC_REG_GENE_LOW_THLD_ADRS        (GH_AGC_REG_BASE_ADRS + AGC_REG_GENE_LOW_THLD_OFFSET)
#define AGC_REG_SPO2_HIGH_THLD_OFFSET        (0x001A)
#define GH_AGC_REG_SPO2_HIGH_THLD_ADRS       (GH_AGC_REG_BASE_ADRS + AGC_REG_SPO2_HIGH_THLD_OFFSET)
#define AGC_REG_SPO2_LOW_THLD_OFFSET         (0x001C)
#define GH_AGC_REG_SPO2_LOW_THLD_ADRS        (GH_AGC_REG_BASE_ADRS + AGC_REG_SPO2_LOW_THLD_OFFSET)

#define AGC_REG_SCR_DRV_SLOTCFG0_OFFSET      (0x001e)
#define GH_AGC_REG_SCR_DRV_SLOTCFG0_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG0_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG1_OFFSET      (0x0020)
#define GH_AGC_REG_SCR_DRV_SLOTCFG1_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG1_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG2_OFFSET      (0x0022)
#define GH_AGC_REG_SCR_DRV_SLOTCFG2_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG2_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG3_OFFSET      (0x0024)
#define GH_AGC_REG_SCR_DRV_SLOTCFG3_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG3_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG4_OFFSET      (0x0026)
#define GH_AGC_REG_SCR_DRV_SLOTCFG4_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG4_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG5_OFFSET      (0x0028)
#define GH_AGC_REG_SCR_DRV_SLOTCFG5_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG5_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG6_OFFSET      (0x002A)
#define GH_AGC_REG_SCR_DRV_SLOTCFG6_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG6_OFFSET)
#define AGC_REG_SCR_DRV_SLOTCFG7_OFFSET      (0x002C)
#define GH_AGC_REG_SCR_DRV_SLOTCFG7_ADRS     (GH_AGC_REG_BASE_ADRS + AGC_REG_SCR_DRV_SLOTCFG7_OFFSET)

#define AGC_REG_DRV_LIMIT_SLOTCFG0_OFFSET    (0x005C)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG0_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG0_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG1_OFFSET    (0x005E)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG1_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG1_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG2_OFFSET    (0x0060)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG2_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG2_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG3_OFFSET    (0x0062)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG3_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG3_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG4_OFFSET    (0x0064)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG4_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG4_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG5_OFFSET    (0x0066)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG5_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG5_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG6_OFFSET    (0x0068)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG6_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG6_OFFSET)
#define AGC_REG_DRV_LIMIT_SLOTCFG7_OFFSET    (0x006A)
#define GH_AGC_REG_DRV_LIMIT_SLOTCFG7_ADRS   (GH_AGC_REG_BASE_ADRS + AGC_REG_DRV_LIMIT_SLOTCFG7_OFFSET)

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */

 /**
 * @brief gain limit
 */
typedef struct
{
    uint8_t gain_limit_bg32ua  : 4;          /**< gain limit bg current */
    uint8_t gain_limit_bg64ua  : 4;
    uint8_t gain_limit_bg128ua : 4;
    uint8_t gain_limit_bg256ua : 4;
} __attribute__ ((packed))gh_agc_gain_limit_t;

 /**
 * @brief agc_scr_drv_slotcfg
 */
typedef struct
{
    uint16_t agc_src_cfg : 4; /**< agc source */
    uint16_t agc_drv_cfg : 4; /**< agc drv */
    uint16_t reserve : 8;
} __attribute__ ((packed))gh_agc_scr_drv_slotcfg_t;

 /**
 * @brief agc_drv_limit_slotcfg
 */
typedef struct
{
    uint16_t agc_drv_up_limit : 8;
    uint16_t agc_drv_down_limit : 8;
} __attribute__ ((packed))gh_agc_drv_limit_slotcfg_t;

 /**
  * @brief soft agc parameter
  */
typedef struct
{
    uint32_t agc_adjust_en;              /**< soft agc enable */
    gh_agc_gain_limit_t agc_gain_limit;  /**< soft gain limit */
    uint32_t agc_spo2_red_slotcfg;       /**< soft agc enable for spo2 red */
    uint32_t agc_spo2_ir_slotcfg;        /**< soft agc enable for spo2 ir */
    uint16_t agc_gene_high_ipd;          /**< high ipd trig threshold of soft agc */
    uint16_t agc_gene_low_ipd;           /**< low ipd trig threshold of soft agc */
    uint16_t agc_spo2_high_ipd;          /**< high ipd trig threshold of soft agc for spo2*/
    uint16_t agc_spo2_low_ipd;           /**< low ipd trig threshold of soft agc for spo2*/
    uint16_t agc_gene_high_thld;         /**< high raw trig threshold of soft agc */
    uint16_t agc_gene_low_thld;          /**< low raw trig threshold of soft agc */
    uint16_t agc_spo2_high_thld;         /**< high raw trig threshold of soft agc for spo2*/
    uint16_t agc_spo2_low_thld;          /**< low raw trig threshold of soft agc for spo2*/
    gh_agc_scr_drv_slotcfg_t  agc_scr_drv_slotcfg[GH_SLOT_NUM];
    gh_agc_drv_limit_slotcfg_t agc_drv_limit_slotcfg[GH_SLOT_NUM];
} __attribute__ ((packed))gh_agc_confg_t;

typedef struct
{
    uint8_t  current_drv;
    uint8_t  min_led_drv;
    uint8_t  max_led_drv;
    uint8_t  slotcfg_rx;
    uint8_t  reg_backed_flag;
    uint8_t  spo2_chnl_flag;      //0: is not spo2 channel   N(N > 0):  is spo2 channl
    uint8_t  agc_src;
    uint8_t  agc_gain_limit;
    uint8_t  agc_drv_en;
    uint8_t  current_gain;
#if GH_HAL_AGC_DRE_EN
    uint8_t dre_en;                /* dre enable*/
    uint8_t  dc_cancel_code;      /* dc cancel code*/
#endif
} gh_agc_mainchnl_config_t;

typedef struct
{
    uint8_t  inite_flag;// inite flag   0:yes   1:not
    uint8_t  window_cnt;
} gh_agc_ideal_adj_info_t;

typedef struct
{
    int64_t rawdata_sum;     //  cacluate drv current and gain
    int32_t last_rwdata;
    int32_t last2_rwdata;
    int32_t max_rawdata;
    int32_t min_rawdata;
    int32_t mean_rawdata;
    int32_t last_mean_rawdata;
    uint16_t sat_cnt;        /* sat num of each channel*/
    uint16_t sat_chceked_cnt;   /* num of _Check data*/
    uint16_t analysis_cnt;   // numble of nalysis data
    uint16_t sample_rate;
    uint8_t sat_flag;           /* sat flag*/
    uint8_t agc_trigger;    //0: no agc action at last fifo read period  1: have agc action at last fifo read preiod
    uint8_t drop_flag;
#if GH_HAL_AGC_DRE_EN
    uint16_t dre_fre_cnt;         /* dre update num of each channel*/
    uint16_t dre_fre_chceked_cnt; /* num of dre checked data*/
    uint8_t last_dc_cancel_code;   /* last dc cancel code*/
    uint16_t ipd_tcnt;             /* ipd time cnt*/
    uint16_t gain_tcnt;            /* gain time cnt*/
    uint16_t fluc_tcnt;            /* data fluctuation time cnt*/
#endif
} gh_agc_mainchnl_info_t;

typedef struct
{
    int32_t last_rwdata;
    int32_t last2_rwdata;
    int32_t max_rawdata;
    int32_t min_rawdata;
    uint16_t analysis_cnt;
    uint16_t sat_cnt;             /* sat num of each channel*/
    uint16_t sat_chceked_cnt;     /* num of _Check data*/
    uint8_t  slotcfg_rx;
    uint8_t  sat_flag;            /* sat flag*/
    uint8_t  window_cnt;
    uint8_t  agc_en;
    uint8_t  current_gain;
 #if GH_HAL_AGC_DRE_EN
    uint16_t dre_fre_cnt;         /* dre update num of each channel*/
    uint16_t dre_fre_chceked_cnt; /* num of dre checked data*/
    int64_t rawdata_sum;           /* cacluate drv current and gain*/
    int32_t mean_rawdata;
    int32_t last_mean_rawdata;
    uint8_t last_dc_cancel_code;   /* last dc cancel code*/
    uint16_t ipd_tcnt;             /* ipd time cnt*/
    uint16_t gain_tcnt;            /* gain time cnt*/
    uint16_t fluc_tcnt;            /* data fluctuation time cnt*/
#endif
}gh_agc_subchnl_info_t;

/**
 * @brief GH slotcfg led drv code write  function
 */
typedef uint32_t (*gh_led_drv_write_t)(uint8_t slotcfg_idx, uint8_t drv_idx, uint8_t drv_code);

/**
 * @brief GH slotcfg tia gain code write  function
 */
typedef uint32_t (*gh_tia_gain_write_t)(uint8_t slotcfg_idx, uint8_t rx_idx, uint8_t gain_code);

#if GH_HAL_AGC_DRE_EN
/**
 * @brief GH param update start function
 */
typedef uint32_t (*gh_param_update_start_t)();

/**
 * @brief GH param update end function
 */
typedef uint32_t (*gh_param_update_end_t)();
#endif

 /**
 * @brief agc param of slotcfg
 */
typedef struct
{
    uint8_t led_drv0_code; /**< led_drv0 code. */
    uint8_t led_drv1_code; /**< led_drv0 code. */
    uint8_t gain0_code : 4; /**< gain0 code. */
    uint8_t gain1_code : 4; /**< gain1 code. */
#if GH_HAL_AGC_DRE_EN
    uint8_t dc_cancel_code0; /**< gain0 code. */
    uint8_t dc_cancel_code1; /**< gain1 code. */
    uint8_t dre_en;          /**< dre enable. */
#endif

} __attribute__ ((packed)) gh_slotcfg_agc_param_t;

 /**
 * @brief agc infomation of chip
 */
typedef struct
{
    gh_slotcfg_agc_param_t slotcfg_agc_param[GH_SLOT_NUM];
    const uint16_t* p_gain_value; /**< gain value(0.1 Kohm),starting from gain code 0. */
} gh_agc_chip_info_t;

 /**
  * @brief soft agc structure
  */
typedef struct
{
    gh_agc_chip_info_t agc_chip_info; /**< agc infomation of chip. */
    gh_hal_settings_t* p_settings; /**< pointer of agc info. */ 
    gh_agc_confg_t agc_confg; /**< agc config. */
    gh_led_drv_write_t led_drv_write; /**< pointer of drv write function. */
    gh_tia_gain_write_t tia_gain_write; /**< pointer of gain write function. */
#if GH_HAL_AGC_DRE_EN
    gh_param_update_start_t param_update_start; /**< pointer param update start function. */
    gh_param_update_end_t param_update_end;     /**< pointer param update end function. */
#endif
    uint8_t agc_slot_num;
    uint8_t agc_subchnl_num;
    gh_agc_mainchnl_config_t agc_mainchnl_config[GH_AGC_SLOT_NUM];
    gh_agc_mainchnl_info_t agc_mainchnl_info[GH_AGC_SLOT_NUM];
    gh_agc_ideal_adj_info_t agc_ideal_adj_info[GH_AGC_SLOT_NUM];
    gh_agc_subchnl_info_t gh_agc_subchnl_info[GH_AGC_SUB_CHNL_NUM];

} gh_hal_agc_t;

/**
 * @brief GH agc return code
 */
typedef enum
{
    GH_AGC_OK                          = 0,
    GH_AGC_PTR_NULL                    = 1,
    GH_AGC_INIT_ERR                    = 2,
    GH_AGC_LEN_ERR                     = 3,
    GH_AGC_CONFIG_ERR                  = 4,
    GH_AGC_RESET_ERR                   = 5,
    GH_AGC_PROCESS_ERR                 = 7,
    GH_AGC_DATA_TYPE_ERR               = 8,
    GH_AGC_RET_END
} gh_agc_ret_e;

/**
 * @fn gh_hal_agc_t* gh_hal_get_agc(void)
 * @brief get the agc instance.
 * @param[in] None
 * @param[out] None
 * @return pointer to agc structure
 *
 * @note None
 */
gh_hal_agc_t* gh_hal_get_agc(void);

/**
 * @brief agc reste
 * @param p_agc: pointer of g_agc
 * @return error word
 *
 * @note reset value of g_agc
 */
uint32_t gh_agc_reset(gh_hal_agc_t* p_agc);

/**
 * @brief agc init
 * @param p_agc: pointer of g_agc
 * @return error word
 *
 * @note init value of g_agc by p_agc_info
 */
uint32_t gh_agc_init(gh_hal_agc_t* p_agc);

/**
 * @brief agc cofig
 * @param p_agc: pointer of g_agc, reg_addr:register address, reg_val: register value
 * @return error word
 *
 * @note init value of g_agc by virtual registers
 */
uint32_t gh_agc_config(gh_hal_agc_t* p_agc, uint16_t reg_addr, uint16_t reg_val);

/**
 * @brief agc process
 * @param p_agc: pointer of g_agc, p_ppg_data: pointer of ppg_data, data_len: rawdata length
 * @return error word
 *
 * @note agc process
 */
uint32_t gh_agc_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len);

/**
 * @brief agc disable
 * @param p_agc: pointer of g_agc
 * @return error word
 *
 * @note disable agc
 */
uint32_t gh_agc_disable(gh_hal_agc_t *p_agc);

/**
 * @brief gh_agc_slot_cfg_restore
 * @param p_agc:        pointer of g_agc
 * @param slot_cfg_id:  cfg id
 * @param led_drv:      led drv
 * @param tia_gain:     tia gain
 * @return error word
 *
 * @note none
 */
uint32_t gh_agc_slot_cfg_restore(gh_hal_agc_t *p_agc, uint8_t slot_cfg_id,
                                 uint8_t led_drv[GH_LED_DRV_NUM],
                                 uint8_t tia_gain[GH_PPG_RX_NUM]);

/**
 * @brief store led drv
 * @param p_agc:        pointer of g_agc
 * @param slotcfg_idx:  cfg id
 * @param drv_idx:      drv id
 * @param drv_code:     drv code
 * @return error word
 *
 * @note none
 */
void gh_agc_leddrv_store(gh_hal_agc_t* p_agc, uint8_t slotcfg_idx, uint8_t drv_idx, uint8_t drv_code);

/**
 * @brief store gain
 * @param p_agc:        pointer of g_agc
 * @param slotcfg_idx:  cfg id
 * @param rx_idx:       rx id
 * @param gain_code:    tia gain
 * @return error word
 *
 * @note none
 */
void gh_agc_gain_store(gh_hal_agc_t* p_agc, uint8_t slotcfg_idx, uint8_t rx_idx, uint8_t gain_code);

#ifdef __cplusplus
}
#endif

#endif
