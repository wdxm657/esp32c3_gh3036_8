#ifndef __GOODIX_SPO2_INTERFACE_H
#define __GOODIX_SPO2_INTERFACE_H

#include <stdint.h>
#include "goodix_type.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define SPO2_INTERFACE_VERSION                  "bas_pv_v1.1.0"
#define SPO2_INTERFACE_VERSION_LEN_MAX          (20)

/**
 * @brief SPO2 algorithm function returns status value
 */
typedef enum
{
    GX_ALGO_SPO2_SUCCESS = 0x00000000,              /**< Success */
    GX_ALGO_SPO2_RWONG_INPUT = 0x00000001,          /**< Illegal input data format */
    GX_ALGO_SPO2_NO_MEMORY = 0x00000002,            /**< Insufficient memory space */
    GX_ALGO_SPO2_FRAME_UNCOMPLETE = 0x00000003,     /**< In the process of downsampling and averaging */
    GX_ALGO_SPO2_WIN_UNCOMPLETE = 0x00000004,       /**< Not reach the complete sliding window frame position (integer s) */
    GX_ALGO_SPO2_UNEXPECTED = 0x00000005,           /**< Exception in design logic */
    GX_ALGO_SPO2_CFG_ERROR = 0x00000006,            /**< Configuration parameter verification failed */
    GX_ALGO_SPO2_INIT_FAILURE = 0x00000007,         /**< Initialization failed */
} goodix_spo2_ret;

/**
 * @brief SPO2 algorithm configuration struct
 */
typedef struct
{
    // raw configuration information
    uint32_t valid_chl_num;                         // valid channel number
    uint32_t raw_fs;                                // raw sampling rate
    int32_t cali_coef_a4;                           // calibration parameter 4th order
    int32_t cali_coef_a3;                           // calibration parameter 3rd order
    int32_t cali_coef_a2;                           // calibration parameter 2nd order
    int32_t cali_coef_a1;                           // calibration parameter 1st order
    int32_t cali_coef_a0;                           // calibration parameter constant

    // hb enable flag
    uint32_t hb_en_flg;                             // hb enable flag

    // wearing status
    uint32_t wear_mode;                             // wearing status 0: bracelet 1: finger 2: earphone

    // acc move thr
    uint32_t acc_thr_max;
    uint32_t acc_thr_min;
    uint32_t acc_thr_scale;
    uint32_t acc_thr_num;
    uint32_t acc_thr_angle;

    // CTR settings
    uint32_t ctr_en_flg;
    uint32_t ctr_red_thr;

    // Signal quality assessment threshold
    uint8_t ppg_jitter_thr;
    uint8_t ppg_noise_thr;
    uint8_t ppg_coeff_thr;
    uint8_t quality_module_key;

    // Output strategy
    uint8_t low_spo2_thr;
    uint8_t fast_out_time;
    uint8_t slow_out_time;
    uint8_t min_stable_time_high;
    uint8_t min_stable_time_low;
    uint8_t max_spo2_variation_high;
    uint8_t max_spo2_variation_low;
    uint8_t ouput_module_key;

    //second calibration
    uint32_t spo2_cali_left;
    uint32_t spo2_cali_right;
    uint32_t spo2_cali_factor;

}goodix_spo2_config;

/**
 * @brief SPO2 algorithm input data struct
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
} goodix_spo2_input_rawdata;

/**
 * @brief SPO2 algorithm result struct
 */
typedef struct
{
    /* Four channels output final value */
    int32_t final_spo2;
    int32_t final_valid_level;
    int32_t final_confi_coeff;
    int32_t final_WeightR;
    int32_t final_hb_mean;
    int32_t final_hb_confi_lvl;
    int32_t final_invalidFlg;
    int32_t final_piIR;
    int32_t final_piRed;
    int32_t final_snrIR;
    int32_t final_snrRed;
    int32_t final_r_val;
    int32_t final_calc_flg;
    int32_t reserved1;
    int32_t reserved2;
    int32_t reserved3;
} goodix_spo2_result;

/**
 * @brief SPO2 processing algorithm version number
 * @param[in] version External storage version number space, 20 bytes allocated
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_spo2_version(uint8_t version[150]);

/**
 * @brief SPO2 processing algorithm initialization
 * @param[in] cfg_instance Externally passed configuration parameter structure
 * @param[in] cfg_size Configuration structure size, used to verify whether the external interface file is aligned
 * @param[in] interface_ver Interface version number, used to verify whether the external interface file is aligned
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_spo2_init(const void *cfg_instance, uint32_t cfg_size, const int8_t *interface_ver);

/**
 * @brief SPO2 blood oxygen calculation
 * @param[in] input_data: raw input raw data structure, refer to goodix_spo2_input_rawdata
 * @param[out] output_result: res output result structure, refer to goodix_spo2_result
 * @return function execution status
 */
DRVDLL_API int32_t goodix_spo2_calc(goodix_spo2_input_rawdata *input_data, goodix_spo2_result *output_result);

/**
 * @brief Destroy the SPO2 algorithm, release the internal memory space, used when calling other module algorithms
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_spo2_deinit(void);

/**
 * @brief SPO2 configuration parameter acquisition, called before initialization,
 *        imports the memory storing the configuration structure content
 * @param[out] None
 * @return pointer to the configuration structure instance
 */
DRVDLL_API const void *goodix_spo2_config_get_instance(void);

/**
 * @brief SPO2 Get the memory size of the configuration parameter structure, called before initialization,
 *        used for internal verification of the algorithm
 * @param[out] None
 * @return The memory size occupied by the configuration parameters
 */
DRVDLL_API uint32_t goodix_spo2_config_get_size(void);

/**
 * @brief SPO2 Get the version of the interface file, called before initialization,
 *        used for internal verification of the algorithm
 *        (to prevent the interface file definition from being misaligned during external compilation)
 * @param[out] None
 * @return None
 */
DRVDLL_API void goodix_spo2_config_get_version(int8_t *ver, uint8_t ver_len);

/**
 * @brief is used by customers to debug alignment and print some key variable information
 *        after the algorithm starts running (called after goodix_spo2_calc)
 * @param[in] raw input raw data structure, when NULL==rawdata, no data information is printed
 * @param[out] None
 * @return None
 */
DRVDLL_API void goodix_spo2_essential_info_print(goodix_spo2_input_rawdata *raw);

#ifdef __cplusplus
}
#endif

#endif // __GOODIX_SPO2_H__
