#ifndef __GOODIX_HRV_H__
#define __GOODIX_HRV_H__

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define RRI_NUM                          4
#define ACC_THR_NUM                      4
#define HRV_INTERFACE_VERSION            "pv_v1.1.0"
#define HRV_INTERFACE_VERSION_LEN_MAX    (20)

/**
 * @brief return error code.
 */
typedef enum
{
    GX_HRV_ALGO_OK              = 0x00000000, // Returns successfully, or no output value is refreshed
    GX_HRV_ALGO_FAILED          = 0x10000001, // Failure return
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // The pointer passed in from outside is null
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // Invalid parameter range
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // Memory allocation failed
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // Release Failed
    GX_HRV_ALGO_UPDATE          = 0x10000006, // There is a value refresh
} goodix_hrv_ret;

/**
 * @brief hrv configuration
 */
typedef struct
{
    int32_t need_ipl;                   // Determine whether to interpolate to 1KHz
    uint32_t fs;                        // Sampling rate
    int32_t acc_thr[ACC_THR_NUM];       // acc related threshold, default value: 20/10/3/3
} goodix_hrv_config;

/**
 * @brief hrv unique input struct
 */
typedef struct
{
    uint8_t hr;                     // External heart rate input
} goodix_hrv_input_unique;

/**
 * @brief hrv configuration struct
 */
typedef struct
{
    uint32_t frame_id;              // Frame number
    uint8_t total_ch_num;           // The total number of channels fed into the algorithm
    uint8_t *enable_flg;            // 1. Indicates the validity of each ppg signal in ppg_rawdata.
                                    //    From high to low, each bit represents one ppg signal.
                                    // 2. Total_chl_num*3 bits need to be set,
                                    //    occupying ceil(total_chl_num*3/8) bytes in total
    int32_t *ppg_rawdata;           // PPG raw data is grouped according to total_chl_num.
                                    // The storage order is green[ch1-total_chl_num],
                                    // ir[ch1-total_chl_num], red[ch1-total_chl_num]
    uint8_t *ch_agc_gain;           // Gain value, the order is consistent with ppg
    uint16_t *ch_agc_drv;           // Drive current, the order is consistent with ppg
    int32_t acc_x;                  // Accelerometer x-axis
    int32_t acc_y;                  // Accelerometer y-axis
    int32_t acc_z;                  // Accelerometer z-axis
    int32_t groy_x;                 // Gyroscope x-axis
    int32_t groy_y;                 // Gyroscope y-axis
    int32_t groy_z;                 // Gyroscope z-axis
    uint8_t sleep_flg;              // Sleep flag
    uint8_t bit_num;                // Number of data bits
    uint8_t chip_type;              // Chip Type
    uint8_t data_type;              // Data Types
    void *pst_module_unique;        // Heart rate specific input
} goodix_hrv_input_rawdata;

/**
 * @brief hrv result struct
 */
typedef struct
{
    int32_t rri[RRI_NUM];           // RRI result array
    int32_t rri_confidence;         // Result confidence
    int32_t rri_valid_num;          // Number of RRI valid results
    int32_t reserved1;              // Three spare interfaces
    int32_t reserved2;
    int32_t reserved3;
} goodix_hrv_result;

/**
 * @brief Get goodix hrv algo version
 *
 * @param version: algo version
 * @return refer to goodix_hrv_ret
 *
 * @note None
 */
DRVDLL_API int32_t goodix_hrv_version(uint8_t version[150]);

/**
 * @brief Goodix hrv algo initialization
 *
 * @param cfg_instance: configuration instance
 * @param cfg_size: configuration size
 * @param interface_ver: configuration version
 * @return refer to goodix_hrv_ret
 *
 * @note None
 */
DRVDLL_API int32_t goodix_hrv_init(const void *cfg_instance, uint32_t cfg_size, const int8_t *interface_ver);

/**
 * @brief Goodix hrv algo calculate
 *
 * @param input_data: input rawdata, refer to goodix_hrv_input_rawdata
 * @param output_result: output result, refer to goodix_hrv_result
 * @return refer to goodix_hrv_ret
 *
 * @note None
 */
DRVDLL_API int32_t goodix_hrv_calc(goodix_hrv_input_rawdata *input_data, goodix_hrv_result *output_result);

/**
 * @brief Goodix hrv algo deinitialization
 *
 * @return refer to goodix_hrv_ret
 *
 * @note None
 */
DRVDLL_API int32_t goodix_hrv_deinit(void);

/**
 * @brief Get the default configuration parameters of hrv
 *
 * @return Default configuration parameters for hrv
 *
 * @note None
 */
DRVDLL_API const void *goodix_hrv_config_get_arr(void);

/**
 * @brief Get the configuration size of hrv
 *
 * @return Configuration size of hrv
 *
 * @note None
 */
DRVDLL_API uint32_t  goodix_hrv_config_get_size(void);

/**
 * @brief Get the version number of the HRV configuration file
 *
 * @param ver: Version number storage array
 * @param ver_len: Version number length. It is recommended to use HRV_INTERFACE_VERSION_LEN_MAX
 * @return None
 *
 * @note None
 */
DRVDLL_API void goodix_hrv_config_get_version(int8_t *ver, uint8_t ver_len);

/**
 * @brief When the user calls to print important HRV information, the printing function needs to be initialized in advance
 *
 * @param raw: Heart rate input data. When NULL == raw, no data is printed.
 * @return None
 *
 * @note None
 */
DRVDLL_API void goodix_hrv_essential_info_print(goodix_hrv_input_rawdata *raw);

#ifdef __cplusplus
}
#endif

#endif // __GOODIX_HRV_H__

