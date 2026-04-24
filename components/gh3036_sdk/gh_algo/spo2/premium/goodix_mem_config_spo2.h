/**
  ****************************************************************************************
  * @file    goodix_mem_config_spo2.h
  * @author  GHealth Driver Team
  * @brief   goodix spo2 algorithm memory configuration
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

#ifndef __GOODIX_MEM_CONFIG_SPO2_H__
#define __GOODIX_MEM_CONFIG_SPO2_H__

//The unit of memory is Byte
#define SPO2_MEM_CONFIG
#define SPO2_VERSION    "GH_SPO2_pre_pv_v2.1.10.0"

#define GOODIX_SPO2_ALGO_PRE_MEM_PEAK_1CHNL               (8416)
#define GOODIX_SPO2_ALGO_PRE_MEM_PEAK_2CHNL               (8416)
#define GOODIX_SPO2_ALGO_PRE_MEM_PEAK_3CHNL               (8416)
#define GOODIX_SPO2_ALGO_PRE_MEM_PEAK_4CHNL               (8416)

#define GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_1CHNL           (5440)
#define GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_2CHNL           (5440)
#define GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_3CHNL           (5440)
#define GOODIX_SPO2_ALGO_PRE_MEM_RESIDENT_4CHNL           (5440)

#define GOODIX_SPO2_ALGO_PRE_STACK_1CHNL                  (1536)
#define GOODIX_SPO2_ALGO_PRE_STACK_2CHNL                  (1536)
#define GOODIX_SPO2_ALGO_PRE_STACK_3CHNL                  (1536)
#define GOODIX_SPO2_ALGO_PRE_STACK_4CHNL                  (1536)

#endif // __GOODIX_MEM_CONFIG_SPO2_H__

