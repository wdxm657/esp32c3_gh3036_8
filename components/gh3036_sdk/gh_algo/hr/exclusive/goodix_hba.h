#ifndef __GOODIX_HBA_H__
#define __GOODIX_HBA_H__

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define HBA_INTERFACE_VERSION                   "pv_v1.1.0"
#define HBA_INTERFACE_VERSION_LEN_MAX           (20)

/**
 * @brief HR algorithm function return status value
 */
typedef enum
{
    GX_ALGO_HBA_SUCCESS = 0x00000000,           /**< success */
    GX_ALGO_HBA_RWONG_INPUT = 0x00000001,       /**< The input data format is invalid */
    GX_ALGO_HBA_NO_MEMORY = 0x00008000,         /**< Insufficient memory space */
} goodix_hba_ret;

/**
 * @brief Heart rate scenes
 */
typedef enum
{
    HBA_SCENES_DEFAULT = 0,                     // Default: Identified and processed by the algorithm internally
    HBA_SCENES_DAILY_LIFE = 1,                  // Daily life
    HBA_SCENES_RUNNING_INSIDE = 2,              // Indoor treadmill
    HBA_SCENES_WALKING_INSIDE = 3,              // Indoor walking
    HBA_SCENES_STAIRS = 4,                      // Going up and down stairs
    HBA_SCENES_RUNNING_OUTSIDE = 5,             // Outdoor running
    HBA_SCENES_WALKING_OUTSIDE = 6,             // Outdoor walking
    HBA_SCENES_STILL_REST = 7,                  // Resting
    HBA_SCENES_REST = 8,                        // Rest
    HBA_SCENES_STILLRADON = 9,                  // Breath holding
    HBA_SCENES_BIKING_INSIDE = 10,              // Indoor cycling
    HBA_SCENES_BIKING_OUTSIDE = 11,             // Outdoor cycling
    HBA_SCENES_BIKING_MOUNTAIN = 12,            // Outdoor mountain biking
    HBA_SCENES_RUNNING_HIGH_HR = 13,            // High heart rate running
    HBA_SCENES_RUNNING_TREADMILL_CCOMBINE = 14,     // Treadmill combination running
    HBA_SCENES_HIGH_INTENSITY_COMBINE = 15,         // High-intensity exercise combination
    HBA_SCENES_TRADITIONAL_STRENGTH_COMBINE = 16,   // Traditional strength training combination
    HBA_SCENES_STEP_TEST = 17,                      // Step test
    HBA_SCENES_BALL_SPORTS = 18,                // Ball Games
    HBA_SCENES_AEROBICS = 19,                   // Aerobics
    HBA_SCENES_SLEEP = 20,                      // Sleeping scene
    HBA_SCENES_JUMP = 21,                       // Wrist skipping rope (LS ROMA)
    HBA_SCENES_CORDLESS_JUMP = 22,              // Wanji cordless skipping rope test
    HBA_SCENES_SWIMMING = 23,                   // Swimming scene
    HBA_SCENES_SIZE = 24,                       // Number of scenes
} hba_scenes_e;

/**
 * @brief Heart rate test mode
 */
typedef enum
{
    HBA_TEST_DYNAMIC = 0,                       // Default: Dynamic test
    HBA_TEST_DOT = 1,                           // Point test
    HBA_TEST_SENSELESS = 2,                     // Non-sensing mode
} hba_test_mode;

/**
 * @brief Heart rate config
 */
typedef struct
{
    uint32_t mode;                              // Test Mode
    uint32_t fs;                                // Sampling rate
    int32_t valid_ch_num;                       // Number of effective channels

    /* Parameters for external control of algorithm bias */
    int32_t hba_earliest_output_time;           // The earliest output result time
    int32_t hba_latest_output_time;             // The latest output result time
    uint32_t sigma;                             // used to control the standard deviation of random noise
    uint32_t raw_ppg_scale;                     // Rawdata filter compensation factor
    uint32_t delay_time;                        // delay time after reset
    uint32_t valid_score_scale;                 // Confidence score magnification
} goodix_hba_config;

/**
 * @brief Heart rate config
 */
typedef struct
{
    uint8_t scence;               // Scene ID
} goodix_hba_input_unique;

/**
 * @brief HR algorithm input data struct
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
} goodix_hba_input_rawdata;

/**
 * @brief HR algorithm result struct
 */
typedef struct
{
    uint32_t hba_out_flag;        // Output value flag: 1 is valid
    uint32_t hba_out;             // Heart rate value
    uint32_t hba_snr;             // Signal-to-noise ratio: After filtering to remove interference,
                                  // the main peak energy of the spectrum is at the total energy
    uint32_t valid_level;         // Confidence level: 0 -> 1, the larger the more reliable
    uint32_t valid_score;         // Confidence score: 0->100, the larger the value, the more reliable it is
    uint32_t hba_acc_info;        // Movement status: 0-resting, small movement;
                                  //                  1-walking-medium movement;
                                  //                  2-running-large movement;
    uint32_t hba_reg_scence;      // Motion scenes: refer to hba_scenes_e enumeration type
    int32_t reserved1;            // Three alternative parameters
    int32_t reserved2;
    int32_t reserved3;
} goodix_hba_result;

/**
 * @brief HBA processing algorithm version number
 * @param[in] version External storage version number space, 20 bytes allocated
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t goodix_hba_version(uint8_t version[150]);

/**
 * @brief HR processing algorithm initialization
 * @param[in] cfg_instance Externally passed configuration parameter structure
 * @param[in] cfg_size Configuration structure size, used to verify whether the external interface file is aligned
 * @param[in] interface_ver Interface version number, used to verify whether the external interface file is aligned
 * @param[out] None
 * @return Function execution status
 */
DRVDLL_API int32_t  goodix_hba_init(const void *cfg_instance, uint32_t cfg_size, const int8_t *interface_ver);

/**
 * @brief HBA algorithm update
 * @param[in] input_data: input raw data structure, refer to goodix_hba_input_rawdata
 * @param[out] output_result: result output result structure, refer to goodix_hba_result
 * @return function execution status
 */
DRVDLL_API int32_t goodix_hba_calc(goodix_hba_input_rawdata *input_data, goodix_hba_result *output_result);

/**
 * @brief HBA algorithm destruction, release internal memory space, used when calling other module algorithms
 * @param[out] None
 * @return function execution status
 */
DRVDLL_API int32_t goodix_hba_deinit(void);

/**
 * @brief Get the default configuration parameters of hba
 * @return the default configuration parameters of hba
 */
DRVDLL_API const void *goodix_hba_config_get_arr(void);

/**
 * @brief Get hba configuration size
 * @return hba configuration size
 */
DRVDLL_API uint32_t goodix_hba_config_get_size(void);

/**
 * @brief Get the version number of the HBA configuration file
 * @param[in/out] ver Version number storage array
 * @param[in] ver_len Version number length, it is recommended to use HBA_INTERFACE_VERSION_LEN_MAX
 * @return None
 */
DRVDLL_API void goodix_hba_config_get_version(int8_t *ver, uint8_t ver_len);

/**
 * @brief The user calls to print HBA important information, and the printing function needs to be initialized in advance
 * @param[in] raw heart rate input data, when NULL == raw, no data is printed
 * @return None
 */
DRVDLL_API void goodix_hba_essential_info_print(goodix_hba_input_rawdata *raw);

#ifdef __cplusplus
}
#endif

#endif // __GOODIX_HBA_H__

