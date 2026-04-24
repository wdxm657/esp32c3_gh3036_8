#ifndef __GOODIX_NADT_H__
#define __GOODIX_NADT_H__


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "goodix_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NADT_INTERFACE_VERSION                  "pv_v1.0.0"
#define NADT_INTERFACE_VERSION_LEN_MAX          (20)

/**
 * @brief NADT algorithm function return status value
 */
typedef enum
{
    GX_ALGO_NADT_SUCCESS = 0x00000000,          /**< Success */
    GX_ALGO_NADT_NO_MEMORY = 0x00000001,        /**< Insufficient memory space */
    GX_ALGO_NADT_UNINIT = 0x00000002,           /**< Uninitialized */
    GX_ALGO_NADT_RWONG_INPUT = 0x00000003,      /**< Invalid input data format */
} goodix_nadt_ret;

/**
 * @brief NADT algorithm configuration struct
 */
typedef struct
{
    uint16_t sample_rate;                       // sampling rate
    uint8_t unwear_detect_level;                // wearing level
    uint16_t check_timeout_thr;                 // live detection timeout
    uint8_t unwear_check_cnt_thr;               // fall-off confirmation time
    uint8_t wear_check_cnt_thr;                 // live confirmation time

    // ADT judgment
    int32_t adt_wear_on_thr;                    // ADT wear on threshold
    int32_t adt_wear_off_thr;                   // ADT wear off threshold

    // motion judgment
    int32_t acc_std_thr;                        // ACC standard deviation

    // live detection
    uint8_t live_detect_enable;                 // live detection enable
    int32_t raw_pv_len_thr;                     // high harmonic threshold
    uint16_t heart_beat_thr_low;                // heart rate lower limit
    uint16_t heart_beat_thr_high;               // heart rate upper limit
    int32_t dif_rr_inter_thr;                   // heart rate change threshold
    uint8_t wear_acf_score_thr;                 // autocorrelation score threshold
    uint16_t base_line_pos_ratio_thr;           // baseline monotonicity rate threshold
    int32_t base_line_diff_thr;                 // baseline flatness threshold
    int32_t sig_scale_thr;

    // high-frequency background light detection
    uint8_t bg_jump_detect_enable;              // background light mutation detection enable
    int16_t jump_bg_diff_thr;                   // background light change threshold
    uint8_t bg_peak_valley_num_thr;             // background light peak valley number threshold

    // background light period detection
    uint8_t bg_period_detect_enable;            // background light period detection enable
    int16_t period_bg_diff_thr;                 // background light change threshold
    uint16_t bg_period_thr;                     // background light period threshold
    uint16_t bg_period_diff_thr;                // Background light period difference threshold
} goodix_nadt_config;

/**
 * @brief NADT algorithm input data struct
 */
typedef struct
{
    uint32_t frame_id;            // Frame number
    uint8_t total_ch_num;         // The total number of channels fed into the algorithm
    uint8_t *enable_flg;          // 1. Indicates the validity of each ppg signal in ppg_rawdata.
                                  //    From high to low, each bit represents one ppg signal.
                                  // 2. Total_chl_num*3 bits need to be set,
                                  //    occupying ceil(total_chl_num*3/8) bytes in total
    int32_t *ppg_rawdata;         // PPG raw data is grouped according to total_chl_num.
                                  // The storage order is green[ch1-total_chl_num],
                                  // ir[ch1-total_chl_num], red[ch1-total_chl_num]
    uint8_t *ch_agc_gain;         // Gain value, the order is consistent with ppg
    uint16_t *ch_agc_drv;         // Drive current, the order is consistent with ppg
    int32_t acc_x;                // Accelerometer x-axis
    int32_t acc_y;                // Accelerometer y-axis
    int32_t acc_z;                // Accelerometer z-axis
    int32_t groy_x;               // Gyroscope x-axis
    int32_t groy_y;               // Gyroscope y-axis
    int32_t groy_z;               // Gyroscope z-axis
    uint8_t sleep_flg;            // Sleep flag
    uint8_t bit_num;              // Number of data bits
    uint8_t chip_type;            // Chip Type
    uint8_t data_type;            // Data Types
    void *pst_module_unique;      // Heart rate specific input
} goodix_nadt_input_rawdata;

/**
 * @brief NADT algorithm input unique data struct
 */
typedef struct
{
    uint32_t ppg_colour_flg;      // PPG color, 0-green, 1-infrared
    uint32_t pre_status;          // Wearing status at the last moment, 0 fall off (adt wear on),
                                  // 1 live, 2 timeout 30s, but the timeout state does not exceed 2 times,
                                  // 3 timeout 30s, and the timeout state exceeds 2 times
    uint32_t cap_channel_num;     // Total number of capacitor channels (PPG Only defaults to 0)
    int32_t *cap_rawdata;         // Raw data of capacitors, capacitor 12-ref12 in sequence (PPG Only defaults to 0)
    int32_t *cap_enable_flg;      // Capacitor channel enable flag (PPG Only defaults to 0)
    int32_t *ch_ipd;              // Chip current, the order is consistent with ppg_rawdata
} goodix_nadt_input_unique;

/**
 * @brief NADT algorithm result struct
 */
typedef struct
{
    int32_t nadt_out_flag;
    int32_t nadt_out;             // bit0-bit1: wearing status, 0-default, 1-wearing, 2-falling off, 3-non-living;
                                  // bit2: suspected falling off mark, 0-normal, 1-suspected falling off
    int32_t nadt_confi;           // confidence level
    int32_t reserved1;
    int32_t reserved2;
    int32_t reserved3;
} goodix_nadt_result;

/**
 * @brief NADT processing algorithm version number
 * @param[in] version External storage version number space, 20 bytes allocated
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_nadt_version(uint8_t version[150]);

/**
 * @brief NADT processing algorithm initialization
 * @param[in] cfg_instance Externally passed configuration parameter structure
 * @param[in] cfg_size Configuration structure size, used to verify whether the external interface file is aligned
 * @param[in] interface_ver Interface version number, used to verify whether the external interface file is aligned
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_nadt_init(const void *cfg_instance, uint32_t cfg_size, const int8_t *interface_ver);

/**
 * @brief Destroy the NADT algorithm, release the internal memory space, used when calling other module algorithms
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_nadt_deinit(void);

/**
 * @brief NADT calculation
 * @param[in] input_data: raw input raw data structure, refer to goodix_nadt_input_rawdata
 * @param[out] output_result: res output result structure, refer to goodix_nadt_result
 * @return function execution status
 */
DRVDLL_API int32_t goodix_nadt_calc(goodix_nadt_input_rawdata *input_data, goodix_nadt_result *output_result);

/**
 * @brief NADT CAP calculation
 * @param[in] raw input raw data structure
 * @param[out] res output result structure
 * @return function execution status
 */
DRVDLL_API int32_t goodix_nadt_calc_by_cap(goodix_nadt_input_rawdata *raw, goodix_nadt_result *res);

/**
 * @brief Get the default configuration parameters of NADT
 * @return the default configuration parameters of NADT
 */
DRVDLL_API const void *goodix_nadt_config_get_arr(void);

/**
 * @brief Get NADT configuration size
 * @return NADT configuration size
 */
DRVDLL_API uint32_t goodix_nadt_config_get_size(void);

/**
 * @brief Get the version number of the NADT configuration file
 * @param[in/out] ver Version number storage array
 * @param[in] ver_len Version number length, it is recommended to use NADT_INTERFACE_VERSION_LEN_MAX
 * @return None
 */
DRVDLL_API void goodix_nadt_config_get_version(int8_t *ver, uint8_t ver_len);

/**
 * @brief The user calls to print NADT important information,
 *        and the printing function needs to be initialized in advance
 * @param[in] raw heart rate input data, when NULL == raw, no data is printed
 * @return None
 */
DRVDLL_API void goodix_nadt_essential_info_print(goodix_nadt_input_rawdata *raw);

#ifdef __cplusplus
}
#endif

#endif // __GOODIX_NADT_H__

