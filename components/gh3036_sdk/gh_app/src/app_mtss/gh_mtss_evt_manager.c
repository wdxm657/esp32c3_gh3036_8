/**
  ****************************************************************************************
  * @file    gh_mtss_evt_manager.c
  * @author  GHealth Driver Team
  * @brief   multi-sensor event manager module
  ****************************************************************************************
  * @attention
  #####Copyright (c) 2024 GOODIX
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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gh_hal_log.h"
#include "gh_hal_utils.h"
#include "gh_app_common.h"
#include "gh_mtss_evt_manager.h"

#if (GH_MTSS_EN)

/*
 * DEFINES
 *****************************************************************************************
 */
#define RETURN_VALUE_ASSEMBLY(internal_err, interface_err) \
            (GH_APP_MTSS_ID << 24 | (internal_err) << 8 | (interface_err))

#if (1 == GH_APP_MTSS_LOG_EN)
#define DEBUG_LOG(...)          GH_LOG_LVL_DEBUG(__VA_ARGS__)
#define WARNING_LOG(...)        GH_LOG_LVL_WARNING(__VA_ARGS__)
#define ERROR_LOG(...)          GH_LOG_LVL_ERROR(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#define WARNING_LOG(...)
#define ERROR_LOG(...)
#endif

// Invalid event
#define GH_MTSS_EVENT_LL_INDEX_INVALID        255

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static uint8_t *gh_mtss_next_evt_get(gh_mtss_evt_manager_t *this, uint8_t *cur_evt_ptr);
static uint8_t *gh_mtss_previous_evt_get(gh_mtss_evt_manager_t *this, uint8_t evt_offset);
static uint8_t *gh_mtss_tail_evt_get(gh_mtss_evt_manager_t *this);
static void gh_mtss_evt_print(gh_mtss_evt_manager_t *this);
static void gh_mtss_evt_mng_range_evt_get(gh_mtss_evt_manager_t *this,
                                          uint32_t event_comb,
                                          uint64_t* oldest_ts,
                                          uint64_t* newest_ts,
                                          uint32_t* oldest_evt,
                                          uint32_t* newest_evt);


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/// @brief get the pointer of next member
/// @param[in] this multi-sensor event manager module
/// @param[in] cur_evt_ptr this pointer points current event member's uchNext
/// @return the pointer of next member, this point points next event member's uchNext
static uint8_t *gh_mtss_next_evt_get(gh_mtss_evt_manager_t *this, uint8_t *cur_evt_ptr)
{
    if (0 != cur_evt_ptr)
    {
        return &(this->evt_linked_list[*cur_evt_ptr].next);
    }
    else
    {
        return 0;
    }
}

/// @brief get the pointer of previous member of event evt_offset
/// @param[in] this multi-sensor event manager module
/// @param[in] evt_offset offset of event
/// @return the pointer of previous member of event evt_offset,
/// this point points uchHead or previous member's uchNext. 0: can not find this member
static uint8_t *gh_mtss_previous_evt_get(gh_mtss_evt_manager_t *this, uint8_t evt_offset)
{
    uint8_t *temp_ptr = 0;
    uint8_t *next = &(this->head);

    do
    {
        if (GH_MTSS_EVENT_LL_INDEX_INVALID == *next)
        {
            break;
        }

        if (evt_offset == (*next))
        {
            temp_ptr = next;
            break;
        }

        next = gh_mtss_next_evt_get(this, next);
    } while (1); // break when get tail node

    return temp_ptr;
}

/// @brief get the pointer of tail in linker list
/// @param this multi-sensor event manager module
/// @return the pointer of tail in linker list, the poiniter points to this->head or evt_linked_list[N].uchNext
static uint8_t *gh_mtss_tail_evt_get(gh_mtss_evt_manager_t *this)
{
    uint8_t *temp_ptr = &(this->head);

    do
    {
        if (GH_MTSS_EVENT_LL_INDEX_INVALID == *temp_ptr)
        {
            break;
        }

        temp_ptr = gh_mtss_next_evt_get(this, temp_ptr);
    } while (1);    //break when get tail node

    return temp_ptr;
}

/// @brief print all events in linked list
/// @param this multi-sensor event manager module
static void gh_mtss_evt_print(gh_mtss_evt_manager_t *this)
{
    uint8_t* temp_ptr = &(this->head);
    uint8_t event_num = 0;

    DEBUG_LOG("[MTSS] print events start...\r\n");

    do
    {
        if (GH_MTSS_EVENT_LL_INDEX_INVALID == *temp_ptr)
        {
            break;
        }

        DEBUG_LOG("[MTSS] EvtPosi = %d, Evt = 0x%X, Ts = %llu.\r\n",
                  (int32_t)event_num,
                  (((uint32_t)1) << (*temp_ptr)),
                  this->evt_linked_list[*temp_ptr].timestamp);

        event_num++;
        temp_ptr = gh_mtss_next_evt_get(this, temp_ptr);
    } while (1); // break when get tail node

    DEBUG_LOG("[MTSS] print events end, event_num = %d.\r\n", (int32_t)event_num);
}

/// @brief get oldest and newest event(in range of event_comb) information
/// @param this multi-sensor event manager module
/// @param event_comb Event combination you need check
/// @param[out] oldest_ts pointer of oldest event timestamp
/// @param[out] newest_ts pointer of newest event timestamp
/// @param[out] oldest_evt pointer of oldest event,  *oldest_evt = 0 means this event cannot find
/// @param[out] newest_evt pointer of newest event, *newest_evt = 0 means this event cannot find
/// @return
static void gh_mtss_evt_mng_range_evt_get(gh_mtss_evt_manager_t *this,
                                          uint32_t event_comb,
                                          uint64_t* oldest_ts,
                                          uint64_t* newest_ts,
                                          uint32_t* oldest_evt,
                                          uint32_t* newest_evt)
{
    uint32_t cur_evt;
    uint8_t* temp_ptr = &(this->head);

    (*oldest_evt) = 0;
    (*newest_evt) = 0;

    do
    {
        if (GH_MTSS_EVENT_LL_INDEX_INVALID == *temp_ptr)
        {
            break;
        }

        cur_evt = ((uint32_t)1 << (*temp_ptr));
        if (0 != (event_comb & cur_evt))
        {
            if (0 == (*newest_evt)) // init newest_ts, oldest_ts, newest_evt, oldest_evt
            {
                (*newest_ts) = this->evt_linked_list[*temp_ptr].timestamp;
                (*oldest_ts) = this->evt_linked_list[*temp_ptr].timestamp;
                (*newest_evt) = cur_evt;
                (*oldest_evt) = cur_evt;
            }
            else // update newest_ts, oldest_ts, newest_evt, oldest_evt
            {
                if ((*oldest_ts) < this->evt_linked_list[*temp_ptr].timestamp)
                {
                    (*oldest_ts) = this->evt_linked_list[*temp_ptr].timestamp;
                    (*oldest_evt) = cur_evt;
                }
                if ((*newest_ts) >= this->evt_linked_list[*temp_ptr].timestamp)
                {
                    (*newest_ts) = this->evt_linked_list[*temp_ptr].timestamp;
                    (*newest_evt) = cur_evt;
                }
            }
        }

        temp_ptr = gh_mtss_next_evt_get(this, temp_ptr);
    } while (1); // break when get tail node
}

uint32_t gh_mtss_evt_manager_init(gh_mtss_evt_manager_t *this,
                                  gh_mtss_new_evt_hook_t evt_hook)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_mtss_evt_manager_t));
    this->head = GH_MTSS_EVENT_LL_INDEX_INVALID; // point to NULL

    //clear linked list members
    for (uint8_t event_cnt = 0; event_cnt < GH_MTSS_EVT_OFST_MAX; event_cnt++)
    {
        this->evt_linked_list[event_cnt].next = GH_MTSS_EVENT_LL_INDEX_INVALID;
    }

    this->new_evt_hook = evt_hook;
    this->init_flag = 1;

    return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
}

uint32_t gh_mtss_evt_manager_deinit(gh_mtss_evt_manager_t *this)
{
    if (GH_NULL_PTR == this)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_PTR_NULL);
    }

    gh_memset((void *)this, 0, sizeof(gh_mtss_evt_manager_t));

    return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
}

uint32_t gh_mtss_evt_manager_ctrl(gh_mtss_evt_manager_t *this,
                                  gh_mtss_ctrl_e option)
{
    if (GH_NULL_PTR == this || 0 == this->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_PTR_NULL);
    }

    if (GH_MTSS_CTRL_DIS == option)
    {
        this->head = GH_MTSS_EVENT_LL_INDEX_INVALID; // point to NULL

        // clear linked list members
        for (uint8_t event_cnt = 0; event_cnt < GH_MTSS_EVT_OFST_MAX; event_cnt++)
        {
            this->evt_linked_list[event_cnt].next = GH_MTSS_EVENT_LL_INDEX_INVALID;
        }
    }

    this->state = option;

    return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
}

uint32_t gh_mtss_evt_manager_state_get(gh_mtss_evt_manager_t *this,
                                       gh_mtss_ctrl_e *state)
{
    if (GH_NULL_PTR == this || 0 == this->init_flag || GH_NULL_PTR == state)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_PTR_NULL);
    }

    *state = this->state;

    return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
}

uint32_t gh_mtss_evt_manager_evt_push(gh_mtss_evt_manager_t *this,
                                      gh_mtss_evt_id_e event_id,
                                      uint64_t timestamp)
{
    uint8_t *pre_member_ptr;
    uint8_t *tail_ptr;
    uint8_t  new_evt_offset = GH_MTSS_EVENT_LL_INDEX_INVALID;

    if (GH_NULL_PTR == this || 0 == this->init_flag)
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_PTR_NULL);
    }

    // get offset of this new event
    for (uint8_t evt_cnt = 0; evt_cnt < GH_MTSS_EVT_OFST_MAX; evt_cnt++)
    {
        if (((uint32_t)1 << evt_cnt) == (uint32_t)event_id)
        {
            new_evt_offset = evt_cnt;
            break;
        }
    }

    // invalid event or event manager is disable
    if ((GH_MTSS_EVENT_LL_INDEX_INVALID == new_evt_offset)
        || (GH_MTSS_CTRL_DIS == this->state))
    {
        return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
    }

    // remove outdated event member in linked list
    pre_member_ptr = gh_mtss_previous_evt_get(this, new_evt_offset);
    if (pre_member_ptr)
    {
        (*pre_member_ptr) = this->evt_linked_list[new_evt_offset].next;
    }

    // add new event to link tail
    tail_ptr = gh_mtss_tail_evt_get(this);
    (*tail_ptr) = new_evt_offset;

    // fill timestamp and next for this new event
    this->evt_linked_list[new_evt_offset].timestamp = timestamp;
    this->evt_linked_list[new_evt_offset].next = GH_MTSS_EVENT_LL_INDEX_INVALID;

    // print new event
    DEBUG_LOG("[MTSS] new event received, Evt = 0x%X.\r\n", event_id);

    // print all event
    gh_mtss_evt_print(this);

    // call event hook
    if (this->new_evt_hook)
    {
        this->new_evt_hook(event_id, timestamp);
    }

    return RETURN_VALUE_ASSEMBLY(0, GH_MTSS_EVT_MNG_OK);
}

uint8_t gh_mtss_evt_is_new_chk(gh_mtss_evt_manager_t *this,
                               gh_mtss_evt_id_e est_new_event,
                               gh_mtss_evt_id_e est_old_event)
{
    uint64_t timestamp;
    uint32_t oldest_evt;
    uint32_t newest_evt;

    if (GH_NULL_PTR == this || 0 == this->init_flag)
    {
        return 0;
    }

    gh_mtss_evt_mng_range_evt_get(this,
                                  ((uint32_t)est_new_event) | ((uint32_t)est_old_event),
                                  &timestamp,
                                  &timestamp,
                                  &oldest_evt,
                                  &newest_evt);

    if (newest_evt == (uint32_t)est_new_event)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/// @brief get tail node timestamp in mtss event manager
/// @param this multi-sensor event manager module
/// @return timestamp
uint64_t gh_mtss_evt_tail_ts_get(gh_mtss_evt_manager_t *this)
{
    uint64_t timestamp = 0;
    uint8_t *temp_ptr = &(this->head);

    if (GH_NULL_PTR == this || 0 == this->init_flag)
    {
        return 0;
    }

    do
    {
        if (GH_MTSS_EVENT_LL_INDEX_INVALID == *temp_ptr)
        {
            break;
        }

        timestamp = this->evt_linked_list[*temp_ptr].timestamp;
        temp_ptr = gh_mtss_next_evt_get(this, temp_ptr);
    } while (1); // break when get tail node

    return timestamp;
}

uint8_t gh_mtss_evt_in_wnd_chk(gh_mtss_evt_manager_t *this,
                               uint32_t event_comb,
                               uint32_t window,
                               uint8_t cur_time_need_in_win)
{
    uint64_t oldest_ts;
    uint64_t newest_ts;
    uint32_t oldest_evt;
    uint32_t newest_evt;

    if (GH_NULL_PTR == this || 0 == this->init_flag)
    {
        return 0;
    }

    gh_mtss_evt_mng_range_evt_get(this,
                                  event_comb,
                                  &oldest_ts,
                                  &newest_ts,
                                  &oldest_evt,
                                  &newest_evt);

    if ((0 == oldest_evt) || (0 == newest_evt)) // cannot find any event in event_comb
    {
        return 0;
    }

    if (1 == cur_time_need_in_win)
    {
        newest_ts = gh_mtss_evt_tail_ts_get(this);
    }

    if ((newest_ts - oldest_ts) < window)
    {
        return 1;
    }

    return 0;
}

#endif

