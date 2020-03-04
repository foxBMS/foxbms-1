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
 * @file    epcos_b57861s0103f045.c
 * @author  foxBMS Team
 * @date    30.10.2018 (date of creation)
 * @ingroup TSENSORS
 * @prefix  B57861S0103F045
 *
 * @brief   Resistive divider used for measuring temperature
 *
 */

/*================== Includes ===============================================*/
#include "epcos_b57861s0103f045.h"

#include <float.h>
#include "foxmath.h"

/*================== Macros and Definitions =================================*/

/**
 * temperature-resistance LUT for EPCOS B57861S0103F045
 */
typedef struct {
    int16_t temperature_C;
    float resistance_Ohm;
} B57861S0103F045_LUT_s;

/*================== Static Constant and Variable Definitions ===============*/

/**
 * LUT filled from higher resistance to lower resistance
 */
static const B57861S0103F045_LUT_s B57861s0103f045_LUT[] = {
    { -55, 963000.00 },
    { -50, 670100.00 },
    { -45, 471700.00 },
    { -40, 336500.00 },
    { -35, 242600.00 },
    { -30, 177000.00 },
    { -25, 130400.00 },
    { -20, 97070.00 },
    { -15, 72930.00 },
    { -10, 55330.00 },
    {  -5, 42320.00 },
    {   0, 32650.00 },
    {   5, 25390.00 },
    {  10, 19900.00 },
    {  15, 15710.00 },
    {  20, 12490.00 },
    {  25, 10000.00 },
    {  30, 8057.00 },
    {  35, 6531.00 },
    {  40, 5327.00 },
    {  45, 4369.00 },
    {  50, 3603.00 },
    {  55, 2986.00 },
    {  60, 2488.00 },
    {  65, 2083.00 },
    {  70, 1752.00 },
    {  75, 1481.00 },
    {  80, 1258.00 },
    {  85, 1072.00 },
    {  90, 917.70 },
    {  95, 788.50 },
    { 100, 680.00 },
    { 105, 588.60 },
    { 110, 511.20 },
    { 115, 445.40 },
    { 120, 389.30 },
    { 125, 341.70 },
    { 130, 300.90 },
    { 135, 265.40 },
    { 140, 234.80 },
    { 145, 208.30 },
    { 150, 185.30 },
    { 155, 165.30 }
};

static uint16_t sizeLUT = sizeof(B57861s0103f045_LUT)/sizeof(B57861S0103F045_LUT_s);


/*================== Extern Constant and Variable Definitions ===============*/
/* Defines for calculating the ADC voltage on the ends of the operating range.
 * The ADC voltage is calculated with the following formula:
 *
 * Vadc = ((Vsupply * Rntc) / (R + Rntc))
 *
 * Depending on the position of the NTC in the voltage resistor (R1/R2),
 * different Rntc values are used for the calculation.
 */
#if B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE
#define ADC_VOLTAGE_VMAX_V    (float)((B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * B57861s0103f045_LUT[sizeLUT-1].resistance_Ohm) / (B57861s0103f045_LUT[sizeLUT-1].resistance_Ohm+B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMIN_V    (float)((B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * B57861s0103f045_LUT[0].resistance_Ohm) / (B57861s0103f045_LUT[0].resistance_Ohm+B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#else /*B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */
#define ADC_VOLTAGE_VMIN_V    (float)((B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * B57861s0103f045_LUT[sizeLUT-1].resistance_Ohm) / (B57861s0103f045_LUT[sizeLUT-1].resistance_Ohm+B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMAX_V    (float)((B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * B57861s0103f045_LUT[0].resistance_Ohm) / (B57861s0103f045_LUT[0].resistance_Ohm+B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#endif
/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/

extern float B57861S0103F045_GetTempFromLUT(uint16_t vadc_mV) {
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
#if B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE

        /* R1 = R2*((Vsupply/Vadc)-1) */
        resistance_Ohm = B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                ((B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V/adcVoltage_V) - 1);
#else /* B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */

        /* R2 = R1*(V2/(Vsupply-Vadc)) */
        resistance_Ohm = B57861S0103F045_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                (adcVoltage_V/(B57861S0103F045_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V - adcVoltage_V));
#endif /* B57861S0103F045_POSITION_IN_RESISTOR_DIVIDER_IS_R1 */

        /* Variables for interpolating LUT value */
        uint16_t between_high = 0;
        uint16_t between_low = 0;
        for (uint16_t i = 1; i < sizeLUT; i++) {
            if (resistance_Ohm < B57861s0103f045_LUT[i].resistance_Ohm) {
                between_low = i+1;
                between_high = i;
            }
        }

        /* Interpolate between LUT vales, but do not extrapolate LUT! */
        if (!((between_high == 0 && between_low == 0) ||  /* measured resistance > maximum LUT resistance */
                 (between_low > sizeLUT))) {              /* measured resistance < minimum LUT resistance */
            temperature = MATH_linearInterpolation(B57861s0103f045_LUT[between_low].resistance_Ohm,
                    B57861s0103f045_LUT[between_low].temperature_C,
                    B57861s0103f045_LUT[between_high].resistance_Ohm,
                    B57861s0103f045_LUT[between_high].temperature_C,
                              resistance_Ohm);
        }
    }

    /* Return temperature based on measured NTC resistance */
    return temperature;
}


extern float B57861S0103F045_GetTempFromPolynom(uint16_t vadc_mV) {
    float temperature = 0.0;
    float vadc_V = vadc_mV/1000.0;
    float vadc2 = vadc_V * vadc_V;
    float vadc3 = vadc2 * vadc_V;
    float vadc4 = vadc3 * vadc_V;
    float vadc5 = vadc4 * vadc_V;

    /* 5th grade polynomial for EPCOS B57861S0103F045 NTC-Thermistor, 10 kOhm, Series B57861S, Vref = 3V, R in series 10k */
    temperature = -6.2765f*vadc5 + 49.0397f*vadc4 - 151.3602f*vadc3 + 233.2521f*vadc2 - 213.4588f*vadc_V + 130.5822f;

    return temperature;
}
