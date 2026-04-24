#include <stdint.h>
#include "gh_hal_service.h"
#include "gh_hal_chip.h"
#include "gh_hal_fifo_parser.h"
#include "gh_hal_control.h"
#include "gh_hal_isr.h"
#include "gh_hal_utils.h"
#include "gh_hal_user.h"
#include "gh_hal_log.h"

#if GH_HAL_CONTROL_LOG_EN
#define DEBUG_LOG(...)                              GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)                            GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)                              GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

#define MILLISECOND                                 (1000)
#define BYTE_BITS                                   (8)

static uint32_t gh_hal_fifo_watermark_cal(gh_hal_control_t* p_hal_control, uint16_t* p_fifo_watermark);

uint32_t gh_hal_control_init(gh_hal_control_t* p_hal_control, gh_slot_disable_callback_t p_slot_disable_callback)
{
    uint32_t ret =  0;
    ret |= gh_hal_control_reset(p_hal_control);
    if (GH_NULL_PTR != p_slot_disable_callback)
    {
        p_hal_control->p_slot_disable_callback = p_slot_disable_callback;
    }
    return ret;
}

uint32_t gh_hal_control_reset(gh_hal_control_t* p_hal_control)
{
    for (uint8_t i = 0; i < sizeof(p_hal_control->slot_index) / sizeof(p_hal_control->slot_index[0]); i++)
    {
        p_hal_control->slot_index[i].slot_cfg_id = GH_CFG_CONTINUE;
    }
    p_hal_control->fifo_watermark_time = 0;
    return 0;
}


uint32_t gh_control_top_ctrl(gh_hal_control_t* p_hal_control, uint8_t start)
{
    uint32_t ret = 0;
    uint8_t slot_idx[SLOT_CFG_MAX_NUM];
    uint8_t slot_cnt = 0;
    uint8_t ppg_cfg_en_bits[PPG_SLOT_CFG_MAX_BYTE_NUM];
    if (0 == start)
    {
        for (uint8_t i = 0; i < sizeof(p_hal_control->slot_index) / sizeof(p_hal_control->slot_index[0]); i++)
        {
            p_hal_control->slot_index[i].slot_cfg_id = GH_CFG_CONTINUE;
        }
    }
    else
    {
        slot_cnt = 0;
        gh_memset(slot_idx, GH_CFG_CONTINUE, SLOT_CFG_MAX_NUM);
        gh_memset(ppg_cfg_en_bits, 0, sizeof(ppg_cfg_en_bits));
        for (uint8_t i = 0; i < SLOT_CFG_MAX_NUM; i++)
        {
            if (p_hal_control->slot_index[i].slot_cfg_id <= GH_PPG_CFG7
                || (p_hal_control->slot_index[i].slot_cfg_id == GH_CAP_CFG))
            {
                slot_idx[slot_cnt] = p_hal_control->slot_index[i].slot_cfg_id;
                slot_cnt++;

                //record which ppg cfg is enabled
                if ((uint8_t)p_hal_control->slot_index[i].slot_cfg_id < sizeof(ppg_cfg_en_bits) * BYTE_BITS
                    && p_hal_control->slot_index[i].slot_cfg_id <= GH_PPG_CFG7)
                {
                    ppg_cfg_en_bits[p_hal_control->slot_index[i].slot_cfg_id / BYTE_BITS] |=
                        (1 << (p_hal_control->slot_index[i].slot_cfg_id % BYTE_BITS));
                }
            }
        }

        gh_exit_lowpower_mode();
        ret |= gh3036_top_start(0);

        //calculate which ppg cfg is disabled
        for (uint8_t i = 0; i < sizeof(ppg_cfg_en_bits) * BYTE_BITS; i++)
        {
            if ((0 == (ppg_cfg_en_bits[i / BYTE_BITS] & (1 << (i % BYTE_BITS))))
                && (0 != (p_hal_control->p_settings->ppg_cfg_en_bits[i / BYTE_BITS] & (1 << (i % BYTE_BITS))))
                && (i < PPG_SLOT_CFG_MAX_NUM))
            {
                if (GH_NULL_PTR != p_hal_control->p_slot_disable_callback)
                {
                    p_hal_control->p_slot_disable_callback(i);
                }
            }
        }

        gh_memcpy(p_hal_control->p_settings->ppg_cfg_en_bits, ppg_cfg_en_bits, sizeof(ppg_cfg_en_bits));

        ret |= gh3036_set_slot_index(slot_idx, SLOT_CFG_MAX_NUM);
        gh3036_slot_en_t slot_en;
        slot_en.slot_en = (1 << slot_cnt) - 1;
        ret |= gh3036_set_slot_en(&slot_en);

        if (0 != slot_cnt && 0 != p_hal_control->fifo_watermark_time)
        {
            uint16_t fifo_watermark;
            ret |= gh_hal_fifo_watermark_cal(p_hal_control, &fifo_watermark);
            if (0 != fifo_watermark)
            {
                ret |= gh3036_set_fifo_watermark(fifo_watermark);
            }
        }

        if (0 != slot_en.slot_en)
        {
            ret |= gh3036_top_start(start);
        }
        gh_enter_lowpower_mode();
    }//else
    return ret;
}

uint32_t gh_control_channel_en(gh_hal_control_t* p_hal_control, gh_hal_data_channel_t* p_data_channel)
{
    uint32_t ret = 0;
    gh3036_slot_cfg_id_e cfg_id = (gh3036_slot_cfg_id_e)p_data_channel->channel_ppg.slot_cfg_id;

    for (uint8_t i = 0 ; i < SLOT_CFG_MAX_NUM; i++)
    {
        //1. check the channel is already in array
        if (p_hal_control->slot_index[i].slot_cfg_id == p_data_channel->channel_ppg.slot_cfg_id)
        {
            return ret;
        }
        if (0 == i && p_hal_control->slot_index[i].slot_cfg_id > GH_PPG_CFG7
            && p_hal_control->slot_index[i].slot_cfg_id != GH_CAP_CFG)
        {
            p_hal_control->slot_index[i].slot_cfg_id = cfg_id;
            return ret;
        }
    }

    // find place to insert the channel in array.
    uint8_t insert_idx = 0;
    for (uint8_t i = 0 ; i < SLOT_CFG_MAX_NUM; i++)
    {
        if ((p_hal_control->slot_index[i].slot_cfg_id > GH_PPG_CFG7
            && p_hal_control->slot_index[i].slot_cfg_id != GH_CAP_CFG)
            || i == SLOT_CFG_MAX_NUM - 1) //invalid slot num
        {
            insert_idx = i;
            break;
        }
        else
        {
            uint16_t slot_multiplier = (cfg_id >= SLOT_CFG_MAX_NUM) ?
                (p_hal_control->p_settings->global_cfg_param.cap_cfg_multiplier)
                : (p_hal_control->p_settings->ppg_cfg_param[cfg_id].multiplier);

            uint16_t this_multiplier = (p_hal_control->slot_index[i].slot_cfg_id >= SLOT_CFG_MAX_NUM) ?
                (p_hal_control->p_settings->global_cfg_param.cap_cfg_multiplier)
                : (p_hal_control->p_settings->ppg_cfg_param[p_hal_control->slot_index[i].slot_cfg_id].multiplier);

            if (slot_multiplier < this_multiplier)
            {
                insert_idx = i;
                break;
            }
        }
    }

    for (uint8_t i = SLOT_CFG_MAX_NUM - 1; i > insert_idx; i--)
    {
        p_hal_control->slot_index[i] = p_hal_control->slot_index[i -1];
    }
    p_hal_control->slot_index[insert_idx].slot_cfg_id = cfg_id;

    return ret;
}

static uint32_t gh_hal_fifo_watermark_cal(gh_hal_control_t* p_hal_control, uint16_t* p_fifo_watermark)
{
    // the slot idx has already been sorted by multiplier,
    // so the first slot is the one with the highest multiplier.
    // 1. calculate the period of the first slot
    uint16_t fastest_slot_period = MILLISECOND
        * ((p_hal_control->p_settings->global_cfg_param.fastest_sample_rate + 1)
        * (p_hal_control->p_settings->ppg_cfg_param[p_hal_control->slot_index[0].slot_cfg_id].multiplier + 1))
        / GH_SENSOR_MAIN_FREQ;

    uint16_t data_cnt = 0;
    uint16_t data_cnt_common = 0;
    uint16_t sample_time = 0;
    uint16_t frame_cnt = 0;
    uint16_t data_cnt_of_frame = 0;

    // 2. calculate the sample data count
    while (sample_time < p_hal_control->fifo_watermark_time)
    {
        /* code */
        data_cnt_of_frame = 0;
        uint8_t is_common = 1;
        for (uint8_t i = 0; i < SLOT_CFG_MAX_NUM; i++)
        {
            if (p_hal_control->slot_index[i].slot_cfg_id > GH_PPG_CFG7
                && GH_CAP_CFG != p_hal_control->slot_index[i].slot_cfg_id)
            {
                break;
            }

            uint16_t slot_multiplier = (p_hal_control->slot_index[i].slot_cfg_id >= SLOT_CFG_MAX_NUM) ?
                (p_hal_control->p_settings->global_cfg_param.cap_cfg_multiplier)
                : (p_hal_control->p_settings->ppg_cfg_param[p_hal_control->slot_index[i].slot_cfg_id].multiplier);

            uint16_t multi = (slot_multiplier + 1)
                /(p_hal_control->p_settings->ppg_cfg_param[p_hal_control->slot_index[0].slot_cfg_id].multiplier + 1);

            if ((frame_cnt + 1) % multi != 0)
            {
                is_common = 0;
            }
            if (frame_cnt % multi != 0)
            {
                continue;
            }
            gh_hal_cal_slot_data_cnt(p_hal_control->p_settings,
                                     p_hal_control->slot_index[i].slot_cfg_id,
                                     &multi);
            data_cnt_of_frame += multi;//use val multi as slot_data_cnt to save memory
        }//for (uint8_t i = 0; i < SLOT_CFG_MAX_NUM; i++)
        frame_cnt++;
        sample_time += fastest_slot_period;
        data_cnt += data_cnt_of_frame;

        if (1 == is_common)
        {
            data_cnt_common = data_cnt;

            //actual_time = sample_time;
        }
        if (data_cnt >= GH3036_FIFO_MAX_LEN)
        {
            break;
        }
    }//while (sample_time < p_hal_control->fifo_watermark_time)

    *p_fifo_watermark = data_cnt_common != 0 ? data_cnt_common : data_cnt;
    *p_fifo_watermark = *p_fifo_watermark >= GH3036_FIFO_MAX_LEN ?
                         (*p_fifo_watermark - data_cnt_of_frame)
                         : *p_fifo_watermark;

    //actual_time = data_cnt_common != 0 ? actual_time : sample_time;
    DEBUG_LOG("fifo watermark time: %d, fifo_watermark: %d, frame_cnt = %d, actual time = \n",
              p_hal_control->fifo_watermark_time, *p_fifo_watermark, frame_cnt);
    return 0;
}

uint32_t gh_hal_control_config(gh_hal_control_t* p_hal_control,  uint16_t reg_addr, uint16_t reg_val)
{
    uint32_t ret = 0;
    if (GH_CTRL_REG_FIFO_FULL_TIME_ADRS == reg_addr)
    {
        p_hal_control->fifo_watermark_time = reg_val;
    }

    return ret;
}
