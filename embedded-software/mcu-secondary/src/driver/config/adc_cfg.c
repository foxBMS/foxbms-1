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
 * @file    adc_cfg.c
 * @author  foxBMS Team
 * @date    02.10.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  ADC
 *
 * @brief   Configuration for the analog to digital converter
 *
 */

/*================== Includes =============================================*/
#include "adc_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
ADC_HandleTypeDef adc_devices[] = {
    {
            .Instance = ADC1,

            .Init.Resolution = ADC_RESOLUTION_12B,
            .Init.DataAlign = ADC_DATAALIGN_RIGHT,
            .Init.ScanConvMode = DISABLE,
            .Init.ContinuousConvMode = DISABLE,
            .Init.DiscontinuousConvMode = DISABLE,
            .Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
            .Init.ExternalTrigConv = ADC_SOFTWARE_START,
            .Init.DMAContinuousRequests = DISABLE,

            .Init.NbrOfDiscConversion = 1,
            .Init.NbrOfConversion = 1,

            .Init.EOCSelection = ADC_EOC_SINGLE_CONV,
            .Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2,
    }
  };

  /* TODO: change to enum */
  uint32_t adc_ChannelList[BS_NR_OF_VOLTAGES_FROM_MCU_ADC] = {ADC_CHANNEL_VBAT, ADC_CHANNEL_TEMPSENSOR};
  uint16_t adc_values[BS_NR_OF_VOLTAGES_FROM_MCU_ADC];


  const uint8_t adc_number_of_used_devices = sizeof(adc_devices)/sizeof(ADC_HandleTypeDef);

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/
