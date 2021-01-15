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
 * @file    epcos_b57861s0103f045.h
 * @author  foxBMS Team
 * @date    30.10.2018 (date of creation)
 * @ingroup TSENSORS
 * @prefix  B57861S0103F045
 *
 * @brief   Resistive divider used for measuring temperature
 *
 *          Vsupply
 *             |
 *            ----
 *            |  | R1
 *            |  |
 *            ----
 *              |
 *              ------- Vadc
 *              |
 *            ----
 *            |   | R2
 *            |   |
 *            ----
 *              |
 *             ---
 *             GND
 *
 */

#ifndef EPCOS_B57861S0103F045_H_
#define EPCOS_B57861S0103F045_H_

/*================== Includes ===============================================*/
#include "general.h"

/*================== Macros and Definitions =================================*/
/**
 * Position of the NTC in the voltage resistor
 * TRUE: NTC is positioned above the voltage tap for the ADC voltage.
 * This equals resistor R1 in the above circuit diagram
 *
 * FALSE: NTC is positioned below the voltage tap for the ADC voltage.
 * This equals resistor R2 in the above circuit diagram
 */
#define B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1        (FALSE)

/**
 * Resistor divider supply voltage in volt
 */
#define B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V         (3.0f)

/**
 * Resistance value of the other resistor (not the NTC) in the resistor
 * divider in kOhm.
 */
#define B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm       (10000.0f)

/*================== Extern Constant and Variable Declarations ==============*/
/*
 * @brief   returns temperature based on measured ADC voltage
 *
 * @param   adc voltage in mV
 *
 * @return  corresponding temperature in &deg;C or FLT_MAX/FLT_MIN if NTC is
 *          shorted or got disconnected. The caller of this functions needs to
 *          check for these return values to prevent invalid data.
 */
extern float B57861S0103F045_GetTempFromLUT(uint16_t vadc_mV);

/*
 * @brief   returns temperature based on measured ADC voltage
 *
 * @param   adc voltage in mV
 *
 * @return  corresponding temperature in &deg;C
 */
extern float B57861S0103F045_GetTempFromPolynom(uint16_t vadc_mV);

/*================== Extern Function Prototypes =============================*/

#endif /* EPCOS_B57861S0103F045_H_ */
