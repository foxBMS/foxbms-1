/**
 *
 * @copyright &copy; 2010 - 2021, Fraunhofer-Gesellschaft zur Foerderung der
 *  angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer
 * to foxBMS in your hardware, software, documentation or advertising
 * materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    adc_cfg.h
 * @author  foxBMS Team
 * @date    02.10.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  ADC
 *
 * @brief   Headers for the configuration for the analog to digital converter
 *
 */

#ifndef ADC_CFG_H_
#define ADC_CFG_H_

/*================== Includes =============================================*/
#include "general.h"

#include "cpu_cfg.h"
#include "batterysystem_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
extern ADC_HandleTypeDef adc_devices[];
extern const uint8_t adc_number_of_used_devices;
extern uint32_t adc_ChannelList[BS_NR_OF_VOLTAGES_FROM_MCU_ADC];
extern uint16_t adc_values[BS_NR_OF_VOLTAGES_FROM_MCU_ADC];


/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* ADC_CFG_H_ */
