/**
 ****************************************************************************************
 *
 * @file    gh_hal_agc_dre.h
 * @author  GOODIX GH Driver Team
 * @brief   Header file containing functions prototypes of public api.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */

/** @addtogroup GH HAL AGC DRE
  * @{
  */

/** @defgroup DRE
  * @brief Public DRE.
  * @{
  */
#ifndef __GH_HAL_AGC_DRE_H__
#define __GH_HAL_AGC_DRE_H__

#include <stdint.h>
#include "gh_hal_agc.h"
#include "gh_hal_service.h"
#include "gh_hal_settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if GH_HAL_AGC_DRE_EN
/*
 * DEFINES
 *****************************************************************************************
 */
#define GH_AGC_DRE_DRV_ADJ_EN                (1)  /* drv adj enable */
#define GH_AGC_DRE_GAIN_ADJ_EN               (1)  /* gain adj enable */

#define MAX(a, b)                            ((a) > (b) ? (a) : (b))
#define MIN(a, b)                            ((a) > (b) ? (b) : (a))

#define GH_DC_CANCEL_RANGE_BASE              (32)  /* dc cancel range base. */
#define GH_8BIT_MAX                          (255) /* 8 bit max. */
#define GH_AGC_DRE_FRE_SEC                   (2)   /* dre fre check tiem(sec). */
#define GH_AGC_DRE_FRE_CHECK_FATOR           (2)   /* dre fre check factor (1<<2:1/4) */
#define GH_AGC_SAT_CHECK_FATOR               (3)   /* sat check factor (1<<3:1/8) */
#define GH_AGC_EXTREMUM_FATOR                (3)   /* MAX or MIN *3 */
#define GH_AGC_THD_FATOR                     (4)   /* high or low thres *4 */
#define GH_AGC_DRE_CHNL_NUM                  ((GH_AGC_SLOT_NUM)*(GH_RX_NUM))
#define GH_AGC_RX_BIT                        (3)   /* rx bit of slot num in slot cfg */
#define GH_AGC_DRV_CHECK_SEC                 (15)  /* check drv upper thes period */
#define GH_AGC_GAIN_CHECK_SEC                (5)   /* check gain upper thes period */
#define GH_AGC_GAIN_CHANGE_GEAR_MAX          (2)   /* gain change no more than 2 at one time */
#define GH_AGC_LAST_SAT_BIT                  (0)   /* bit of sat flag in last mean rawdata */
#define GH_AGC_LAST_DRE_BIT                  (1)   /* bit of dre flag in last mean rawdata */
#define GH_AGC_LAST_RAW_MASK                 (0xfffffffc)   /* last rawdata mask */
#define GH_AGC_UA_TO_NA                      (1000) /* ua to na */
#define GH_AGC_IDC_FACTOR                    (10)   /* dc rawdata factor */

/**
  * @brief agc chnl type
  */
typedef enum
{
    GH_PPG_AGC_MAIN      = 0, // main agc chnl
    GH_PPG_AGC_SUB       = 1, // sub agc chnl
} gh_agc_chnl_type_e;

/**
  * @brief agc sat type
  */
typedef enum
{
    GH_PPG_NO_SAT           = 0, // normal data
    GH_PPG_SAT              = 1, // sat
    GH_PPG_SERIOUS_SAT      = 2, // serious sat
    GH_PPG_DRE_FRE          = 3, // frequent adjustment of dre
    GH_PPG_SERIOUS_DRE_FRE  = 4, // serious frequent adjustment of dre
} gh_agc_sat_type_e;

/**
  * @brief agc flag type
  */
typedef enum
{
    GH_PPG_NO_AGC           = 0, // no agc
    GH_PPG_GAIN_ADJ         = 1, // change gain
    GH_PPG_DRV_ADJ          = 2, // change drv
    GH_PPG_DC_CANCEL_ADJ    = 3, // change dc cancel
} gh_agc_flag_type_e;

/**
  * @brief agc thres
  */
typedef struct
{
    int32_t high_thd;
    int32_t low_thd;
    int32_t high_ipd_thd;
    int32_t low_ipd_thd;
    int32_t ideal_rawdata;
} gh_agc_thres_t;

/**
  * @brief agc thres
  */
typedef struct
{
    gh_agc_thres_t  agc_thres;
    int32_t         mean_ipd_ary[GH_SLOT_NUM];
    uint8_t         init_flag[GH_AGC_DRE_CHNL_NUM];
    uint8_t         agc_flag[GH_AGC_DRE_CHNL_NUM];
    uint8_t         gain[GH_AGC_DRE_CHNL_NUM];
    uint8_t         drv_code[GH_AGC_DRE_CHNL_NUM];
    uint8_t         drv_en[GH_AGC_DRE_CHNL_NUM];
} gh_agc_common_info_t;

/**
 * @brief agc data information
 */
typedef struct
{
    int64_t* rawdata_sum;           /* rawdata sum for cacluate drv current and gain*/
    int32_t* mean_rawdata;          /* current rawdata mean*/
    int32_t* last_mean_rawdata;     /* last rawdata mean*/
    int32_t* last_rawdata;          /* last rawdata*/
    int32_t* last2_rawdata;         /* last last rawdata*/
    int32_t* max_rawdata;           /* max rawdata*/
    int32_t* min_rawdata;           /* min rawdata*/
    uint8_t* last_dc_cancel_code;   /* last dc cancel code*/
    uint8_t* last_dre_update;       /* last dre update*/
    int32_t  dc_cancel_code_rawdata;/* dc cancel code rawdata*/
} gh_agc_store_data_t;

/**
 * @brief agc param information
 */
typedef struct
{
    uint8_t* current_drv;         /* current drv*/
    uint8_t* current_gain;        /* current gain*/
    uint8_t* dc_cancel_code;      /* current dc cancel code*/
} gh_agc_current_param_t;

/**
 * @brief agc param information
 */
typedef struct
{
    uint8_t  new_drv;                 /* current drv*/
    uint8_t  new_gain;                /* current gain*/
    uint8_t  new_dc_cancel_code;      /* current dc cancel code*/
} gh_agc_new_param_t;

/**
 * @brief agc configure param information
 */
typedef struct
{
    uint8_t  dre_en;               /* dre enable*/
    uint8_t  agc_en;               /* agc enable*/
    uint8_t  agc_drv_en;           /* agc drv enable*/
    uint8_t  agc_src;              /* agc src enable*/
    uint8_t  slotcfg_rx;           /* agc slot cfg rx*/
    uint16_t sample_rate;          /* agc slot sample*/
    uint8_t  min_led_drv;          /* agc slot min drv thres*/
    uint8_t  max_led_drv;          /* agc slot max drv thres*/
    uint8_t  agc_gain_limit;       /* agc slot max gain thres*/
} gh_agc_configure_t;

/**
 * @brief agc counters
 */
typedef struct
{
    uint8_t*  window_cnt;          /* window cnt*/
    uint16_t* analysis_cnt;        /* analysis cnt*/
    uint16_t* sat_cnt;             /* sat num of each channel*/
    uint16_t* sat_chceked_cnt;     /* num of sat checked data*/
    uint16_t* dre_fre_cnt;         /* dre update num of each channel*/
    uint16_t* dre_fre_chceked_cnt; /* num of dre checked data*/
    uint16_t* ipd_tcnt;            /* ipd time cnt*/
    uint16_t* gain_tcnt;           /* gain time cnt*/
    uint16_t* fluc_tcnt;           /* data fluctuation time cnt*/
} gh_agc_cnt_t;

/**
 * @brief agc flag
 */
typedef struct
{
    uint8_t  sat_flag;              /* sat flag*/
    uint8_t  dre_update_flag;       /* dre update flag*/
    uint8_t  reg_backed_flag;
    uint8_t* init_flag;             /*init flag*/
} gh_agc_flag_t;

/**
 * @brief agc flag
 */
typedef struct
{
    uint8_t sat_flow;             /* 0: return, 1: start sat adj process*/
    uint8_t gain_drv_flow;        /* 0: return, 1: start gain adj process 2: start drv adj process */
    uint8_t adj_full_data_flow;   /* 0: return, 1: data is ready, start adj process*/
    uint8_t param_update_flow;    /* 0: return, 1: data is end, start update param*/
} gh_agc_flow_t;

typedef struct
{
    gh_agc_store_data_t    agc_store_data;
    gh_agc_current_param_t agc_current_param;
    gh_agc_configure_t     agc_configure;
    gh_agc_cnt_t           agc_cnt;
    gh_agc_flag_t          agc_flag;
    gh_agc_flow_t          agc_flow;
    int32_t                rawdata;              /* rawdata*/
    uint16_t               data_index;           /*fifo data index*/
    uint8_t                agc_main_sub_type;    /*agc data type: main or sub*/
    uint8_t                agc_slot_idex;        /*agc slot cfg chn index*/
    uint8_t                main_idex;            /*agc main info index*/
    uint8_t                slot_idex;            /*data slot index*/
    uint8_t                agc_common_index;     /*agc common index*/
    const uint16_t*        p_gain_value;         /*< gain value(0.1 Kohm),starting from gain code 0. */
    uint8_t                drop_len;             /*drop data len for first init*/
} gh_dre_agc_info_t;

/**
 * @brief dre agc process
 * @param p_agc: pointer of g_agc, p_ppg_data: pointer of ppg_data, data_len: rawdata length
 * @return error word
 *
 * @note dre agc process
 */
int32_t gh_agc_dre_process(gh_hal_agc_t* p_agc, gh_data_t *p_gh_data, uint16_t data_len);
#endif

/**
 * @brief dre agc reset
 * @param none
 * @return error word
 *
 * @note
 */
void gh_agc_dre_common_reset(void);

#ifdef __cplusplus
}
#endif

#endif // __GH_HAL_AGC_DRE_H__
