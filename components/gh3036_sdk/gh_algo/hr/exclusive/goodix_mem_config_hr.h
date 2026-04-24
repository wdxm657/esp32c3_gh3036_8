/**
  ****************************************************************************************
  * @file    goodix_mem_config_hr.h
  * @author  GHealth Driver Team
  * @brief   goodix hr algorithm memory configuration
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

#ifndef __GOODIX_MEM_CONFIG_HR_H__
#define __GOODIX_MEM_CONFIG_HR_H__

//The unit of memory is Byte
#define HR_MEM_CONFIG
#define HR_VERSION    "GH_HR_exc_pv_v2.0.2.0"

#define GOODIX_HR_ALGO_EXC_MEM_PEAK_1CHNL               (19788)
#define GOODIX_HR_ALGO_EXC_MEM_PEAK_2CHNL               (19788)
#define GOODIX_HR_ALGO_EXC_MEM_PEAK_3CHNL               (19788)
#define GOODIX_HR_ALGO_EXC_MEM_PEAK_4CHNL               (19788)

#define GOODIX_HR_ALGO_EXC_MEM_RESIDENT_1CHNL           (7688)
#define GOODIX_HR_ALGO_EXC_MEM_RESIDENT_2CHNL           (7688)
#define GOODIX_HR_ALGO_EXC_MEM_RESIDENT_3CHNL           (7688)
#define GOODIX_HR_ALGO_EXC_MEM_RESIDENT_4CHNL           (7688)

#define GOODIX_HR_ALGO_EXC_STACK_1CHNL                  (1756)
#define GOODIX_HR_ALGO_EXC_STACK_2CHNL                  (1724)
#define GOODIX_HR_ALGO_EXC_STACK_3CHNL                  (1740)
#define GOODIX_HR_ALGO_EXC_STACK_4CHNL                  (1708)

#endif // __GOODIX_MEM_CONFIG_HR_H__

