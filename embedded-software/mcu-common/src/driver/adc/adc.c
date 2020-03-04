/**
 *
 * @copyright &copy; 2010 - 2020, Fraunhofer-Gesellschaft zur Foerderung der
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
 * @file    adc.c
 * @author  foxBMS Team
 * @date    19.11.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  ADC
 *
 * @brief   Driver for the analog to digital converter
 *
 * This adc module provides support for analog/digital conversion.
 * It must be initialized during startup.
 */




/*================== Includes =============================================*/
#include "adc.h"


/*================== Macros and Definitions ===============================*/
#define ADC_CONVERT             0
#define ADC_WAITFORCONVERSION   1
#define ADC_STOREDATA           2

/* Macros for converting voltage -> temperature */
#define ADC_VBAT_VOLTAGE_DIVIDER    4.0f
#define ADC_FULL_RANGE              4095.0f        /* 12bit adc converter --> range [0, ..., 4095] */
#define ADC_VREF_EXT                2.5f
#define ADC_V25                     760
#define ADC_AVG_SLOPE               2.5f

/*================== Constant and Variable Definitions ====================*/
static ADC_ChannelConfTypeDef adc_cfg = {
    .Channel = ADC_CHANNEL_VBAT,
    .Rank = 1,
    .SamplingTime = ADC_SAMPLETIME_3CYCLES,
    .Offset = 0,
};

static uint8_t adc_conversion_state = ADC_CONVERT;
static uint16_t adc_vbat = 0;
static uint16_t adc_mcuTemp = 0;
static uint8_t adc_channel_num = 0;


/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

void ADC_Init(ADC_HandleTypeDef *AdcHandle) {
    uint8_t i = 0;

    if (AdcHandle != NULL) {
        for (i = 0; i < adc_number_of_used_devices; i++) {
            if (AdcHandle[i].Instance == ADC1) {
                __HAL_RCC_ADC1_CLK_ENABLE();
            }
            if (AdcHandle[i].Instance == ADC2) {
                __HAL_RCC_ADC2_CLK_ENABLE();
            }
            if (AdcHandle[i].Instance == ADC3) {
                __HAL_RCC_ADC3_CLK_ENABLE();
            }
            HAL_ADC_Init(&AdcHandle[i]);
        }
    }
}


void ADC_Convert(ADC_HandleTypeDef *AdcHandle) {
    /* Enables Interrupt and starts ADC conversion */
    HAL_ADC_Start_IT(AdcHandle);
}


void ADC_Ctrl(void) {
    adc_cfg.Rank = 1;
    adc_cfg.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    adc_cfg.Offset = 0;

    /* Alternate ADC conversion between inputs defined in ADC channel list */
    if (adc_conversion_state == ADC_CONVERT) {
        adc_cfg.Channel = adc_ChannelList[adc_channel_num];
        /* Configure ADC Channel */
#if defined(STM32F411xE) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
    if (adc_cfg.Channel == ADC_CHANNEL_TEMPSENSOR) {
        ADC->CCR &= ~(ADC_CCR_VBATE);
    }
#endif /* STM32F411xE || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F446xx || STM32F469xx || STM32F479xx */

        HAL_ADC_ConfigChannel(&adc_devices[0], &adc_cfg);
    }

    if (adc_conversion_state == ADC_CONVERT) {
        /* Enable conversion */
        adc_conversion_state = ADC_WAITFORCONVERSION;
        ADC_Convert(&adc_devices[0]);
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle) {
    uint32_t raw_voltage = 0;

    /* Disable interrupt and stop ADC conversion */
    HAL_ADC_Stop_IT(AdcHandle);
    raw_voltage = HAL_ADC_GetValue(AdcHandle);

    adc_values[adc_channel_num] = (raw_voltage & 0xFFF) * ADC_VREF_EXT * 1000 / ADC_FULL_RANGE;
    if (adc_cfg.Channel == ADC_CHANNEL_VBAT) {
      adc_vbat = adc_values[adc_channel_num];
    }
    if (adc_cfg.Channel == ADC_CHANNEL_TEMPSENSOR) {
      adc_mcuTemp = adc_values[adc_channel_num];
    }
    adc_channel_num++;
    if (adc_channel_num > BS_NR_OF_VOLTAGES_FROM_MCU_ADC - 1) {
      adc_channel_num = 0;
    }
    /* Start new conversion */
    adc_conversion_state = ADC_CONVERT;
}


/**
 * @brief get coin cell battery voltage
 */
extern float ADC_GetVBAT_mV(void) {
    float scaled_voltage = adc_vbat * ADC_VBAT_VOLTAGE_DIVIDER;
    return scaled_voltage;
}

/**
 * @brief get MCU temperature
 */
extern float ADC_GetMCUTemp_C(void) {
    float scaled_temperature = (adc_mcuTemp - ADC_V25)/(ADC_AVG_SLOPE) + 25.0f;
    return scaled_temperature;
}

/**
 * @brief get voltage in mV
 */
extern uint16_t ADC_GetValue(uint32_t value) {
    if (value < BS_NR_OF_VOLTAGES_FROM_MCU_ADC) {
      return adc_values[value];
    }
    return 0;
}
