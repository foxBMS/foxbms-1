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
 * @file    epcos_b57251v5103j060.c
 * @author  foxBMS Team
 * @date    30.10.2018 (date of creation)
 * @ingroup TSENSORS
 * @prefix  B57251V5103J060
 *
 * @brief   Resistive divider used for measuring temperature
 *
 */

/*================== Includes ===============================================*/
#include "epcos_b57251v5103j060.h"

#include <float.h>
#include "foxmath.h"

/*================== Macros and Definitions =================================*/

/**
 * temperature-resistance LUT for Vishay NTCALUG01A103G NTC
 */
typedef struct {
    int16_t temperature_C;
    float resistance_Ohm;
} B57251V5103J060_LUT_s;

/*================== Static Constant and Variable Definitions ===============*/

/**
 * LUT filled from higher resistance to lower resistance
 */
static const B57251V5103J060_LUT_s b57251v5103j060_LUT[] = {
    { -55, 961580.00 },
    { -50, 668920.00 },
    { -45, 471270.00 },
    { -40, 336060.00 },
    { -35, 242430.00 },
    { -30, 176810.00 },
    { -25, 130320.00 },
    { -20, 97020.00 },
    { -15, 72923.00 },
    { -10, 55314.00 },
    {  -5, 42325.00 },
    {   0, 32657.00 },
    {   5, 25400.00 },
    {  10, 19907.00 },
    {  15, 15716.00 },
    {  20, 12494.00 },
    {  25, 10000.00 },
    {  30, 8055.20 },
    {  35, 6528.80 },
    {  40, 5322.90 },
    {  45, 4364.50 },
    {  50, 3598.10 },
    {  55, 2981.90 },
    {  60, 2483.70 },
    {  65, 2078.70 },
    {  70, 1747.90 },
    {  75, 1476.30 },
    {  80, 1252.30 },
    {  85, 1066.70 },
    {  90, 912.27 },
    {  95, 783.19 },
    { 100, 674.88 },
    { 105, 583.63 },
    { 110, 506.47 },
    { 115, 440.98 },
    { 120, 385.20 },
    { 125, 337.52 },
    { 130, 296.63 },
    { 135, 261.46 },
    { 140, 231.11 },
    { 145, 204.84 },
    { 150, 182.03 }
};

static uint16_t sizeLUT = sizeof(b57251v5103j060_LUT)/sizeof(B57251V5103J060_LUT_s);


/*================== Extern Constant and Variable Definitions ===============*/
/* Defines for calculating the ADC voltage on the ends of the operating range.
 * The ADC voltage is calculated with the following formula:
 *
 * Vadc = ((Vsupply * Rntc) / (R + Rntc))
 *
 * Depending on the position of the NTC in the voltage resistor (R1/R2),
 * different Rntc values are used for the calculation.
 */
#if B57251V5103J060_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE
#define ADC_VOLTAGE_VMAX_V    (float)((B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * b57251v5103j060_LUT[sizeLUT-1].resistance_Ohm) / (b57251v5103j060_LUT[sizeLUT-1].resistance_Ohm+B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMIN_V    (float)((B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * b57251v5103j060_LUT[0].resistance_Ohm) / (b57251v5103j060_LUT[0].resistance_Ohm+B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#else /* B57251V5103J060_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */
#define ADC_VOLTAGE_VMIN_V    (float)((B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * b57251v5103j060_LUT[sizeLUT-1].resistance_Ohm) / (b57251v5103j060_LUT[sizeLUT-1].resistance_Ohm+B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMAX_V    (float)((B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * b57251v5103j060_LUT[0].resistance_Ohm) / (b57251v5103j060_LUT[0].resistance_Ohm+B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#endif
/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/

extern float B57251V5103J060_GetTempFromLUT(uint16_t vadc_mV) {
    float temperature = 0.0;
    float resistance_Ohm = 0.0;
    float adcVoltage_V = vadc_mV/1000.0;    /* Convert mV to V */

    /* Check for valid ADC measurements to prevent undefined behavior */
    if (adcVoltage_V > ADC_VOLTAGE_VMAX_V) {
        /* Invalid measured ADC voltage -> sensor out of operating range or disconnected/shorted */
        temperature = -FLT_MAX;
    } else if (adcVoltage_V < ADC_VOLTAGE_VMIN_V) {
        /* Invalid measured ADC voltage -> sensor out of operating range or shorted/disconnected */
        temperature = FLT_MAX;
    } else {
        /* Calculate NTC resistance based on measured ADC voltage */
#if B57251V5103J060_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE

        /* R1 = R2*((Vsupply/Vadc)-1) */
        resistance_Ohm = B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                ((B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V/adcVoltage_V) - 1);
#else /* B57251V5103J060_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */

        /* R2 = R1*(V2/(Vsupply-Vadc)) */
        resistance_Ohm = B57251V5103J060_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                (adcVoltage_V/(B57251V5103J060_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V - adcVoltage_V));
#endif /* B57251V5103J060_POSITION_IN_RESISTOR_DIVIDER_IS_R1 */

        /* Variables for interpolating LUT value */
        uint16_t between_high = 0;
        uint16_t between_low = 0;
        for (uint16_t i = 1; i < sizeLUT; i++) {
            if (resistance_Ohm < b57251v5103j060_LUT[i].resistance_Ohm) {
                between_low = i+1;
                between_high = i;
            }
        }

        /* Interpolate between LUT vales, but do not extrapolate LUT! */
        if (!((between_high == 0 && between_low == 0) ||  /* measured resistance > maximum LUT resistance */
                 (between_low > sizeLUT))) {              /* measured resistance < minimum LUT resistance */
            temperature = MATH_linearInterpolation(b57251v5103j060_LUT[between_low].resistance_Ohm,
                    b57251v5103j060_LUT[between_low].temperature_C,
                    b57251v5103j060_LUT[between_high].resistance_Ohm,
                    b57251v5103j060_LUT[between_high].temperature_C,
                              resistance_Ohm);
        }
    }

    /* Return temperature based on measured NTC resistance */
    return temperature;
}


extern float B57251V5103J060_GetTempFromPolynom(uint16_t vadc_mV) {
    float temperature = 0.0;
    float vadc_V = vadc_mV/1000.0;
    float vadc2 = vadc_V * vadc_V;
    float vadc3 = vadc2 * vadc_V;
    float vadc4 = vadc3 * vadc_V;
    float vadc5 = vadc4 * vadc_V;
    float vadc6 = vadc5 * vadc_V;

    temperature = 6.8405f*vadc6 - 74.815f*vadc5 + 317.48f*vadc4 - 669.16f*vadc3 +
            740.82f*vadc2 - 444.97f*vadc_V + 166.48f;

    return temperature;
}
