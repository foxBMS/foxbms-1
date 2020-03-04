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
 * @file    vishay_ntcalug01a103g.c
 * @author  foxBMS Team
 * @date    30.10.2018 (date of creation)
 * @ingroup TSENSORS
 * @prefix  NTCALUG01A103G
 *
 * @brief   Resistive divider used for measuring temperature
 *
 */

/*================== Includes ===============================================*/
#include "vishay_ntcalug01a103g.h"

#include <float.h>
#include "foxmath.h"

/*================== Macros and Definitions =================================*/

/**
 * temperature-resistance LUT for Vishay NTCALUG01A103G NTC
 */
typedef struct {
    int16_t temperature_C;
    float resistance_Ohm;
} NTCALUG01A103G_LUT_s;

/*================== Static Constant and Variable Definitions ===============*/

/**
 * LUT filled from higher resistance to lower resistance
 */
static const NTCALUG01A103G_LUT_s ntcalug01a103g_LUT[] = {
        { -40.0, 334274.4},
        { -39.0, 312904.4},
        { -38.0, 293033.6},
        { -37.0, 274548.0},
        { -36.0, 257343.1},
        { -35.0, 241322.9},
        { -34.0, 226398.8},
        { -33.0, 212489.7},
        { -32.0, 199520.6},
        { -31.0, 187422.7},
        { -30.0, 176132.5},
        { -29.0, 165591.5},
        { -28.0, 155745.6},
        { -27.0, 146545.1},
        { -26.0, 137944.1},
        { -25.0, 129900.0},
        { -24.0, 122373.7},
        { -23.0, 115329.0},
        { -22.0, 108732.2},
        { -21.0, 102552.5},
        { -20.0, 96761.1 },
        { -19.0, 91331.5 },
        { -18.0, 86239.0 },
        { -17.0, 81460.9 },
        { -16.0, 76976.0 },
        { -15.0, 72764.6 },
        { -14.0, 68808.6 },
        { -13.0, 65091.1 },
        { -12.0, 61596.4 },
        { -11.0, 58309.9 },
        { -10.0, 55218.1 },
        {  -9.0, 52308.4 },
        {  -8.0, 49569.0 },
        {  -7.0, 46989.1 },
        {  -6.0, 44558.56},
        {  -5.0, 42267.85},
        {  -4.0, 40108.20},
        {  -3.0, 38071.41},
        {  -2.0, 36149.83},
        {  -1.0, 34336.32},
        {   0.0, 32624.23},
        {   1.0, 31007.34},
        {   2.0, 29479.85},
        {   3.0, 28036.35},
        {   4.0, 26671.76},
        {   5.0, 25381.36},
        {   6.0, 24160.73},
        {   7.0, 23005.71},
        {   8.0, 21912.45},
        {   9.0, 20877.31},
        {  10.0, 19896.90},
        {  11.0, 18968.04},
        {  12.0, 18087.75},
        {  13.0, 17253.25},
        {  14.0, 16461.90},
        {  15.0, 15711.26},
        {  16.0, 14999.01},
        {  17.0, 14323.01},
        {  18.0, 13681.22},
        {  19.0, 13071.73},
        {  20.0, 12492.75},
        {  21.0, 11942.59},
        {  22.0, 11419.69},
        {  23.0, 10922.54},
        {  24.0, 10449.75},
        {  25.0, 10000.00},
        {  26.0, 9572.05 },
        {  27.0, 9164.74 },
        {  28.0, 8776.97 },
        {  29.0, 8407.70 },
        {  30.0, 8055.96 },
        {  31.0, 7720.82 },
        {  32.0, 7401.43 },
        {  33.0, 7096.96 },
        {  34.0, 6806.64 },
        {  35.0, 6529.74 },
        {  36.0, 6265.58 },
        {  37.0, 6013.51 },
        {  38.0, 5772.92 },
        {  39.0, 5543.22 },
        {  40.0, 5323.88 },
        {  41.0, 5114.37 },
        {  42.0, 4914.20 },
        {  43.0, 4722.92 },
        {  44.0, 4540.08 },
        {  45.0, 4365.27 },
        {  46.0, 4198.11 },
        {  47.0, 4038.21 },
        {  48.0, 3885.23 },
        {  49.0, 3738.84 },
        {  50.0, 3598.72 },
        {  51.0, 3464.58 },
        {  52.0, 3336.12 },
        {  53.0, 3213.08 },
        {  54.0, 3095.22 },
        {  55.0, 2982.27 },
        {  56.0, 2874.02 },
        {  57.0, 2770.26 },
        {  58.0, 2670.76 },
        {  59.0, 2575.34 },
        {  60.0, 2483.82 },
        {  61.0, 2396.00 },
        {  62.0, 2311.74 },
        {  63.0, 2230.85 },
        {  64.0, 2153.21 },
        {  65.0, 2078.65 },
        {  66.0, 2007.05 },
        {  67.0, 1938.27 },
        {  68.0, 1872.19 },
        {  69.0, 1808.69 },
        {  70.0, 1747.65 },
        {  71.0, 1688.98 },
        {  72.0, 1632.56 },
        {  73.0, 1578.31 },
        {  74.0, 1526.13 },
        {  75.0, 1475.92 },
        {  76.0, 1427.62 },
        {  77.0, 1381.12 },
        {  78.0, 1336.37 },
        {  79.0, 1293.29 },
        {  80.0, 1251.80 },
        {  81.0, 1211.85 },
        {  82.0, 1173.36 },
        {  83.0, 1136.28 },
        {  84.0, 1100.55 },
        {  85.0, 1066.11 },
        {  86.0, 1032.91 },
        {  87.0, 1000.91 },
        {  88.0, 970.05 },
        {  89.0, 940.29 },
        {  90.0, 911.59 },
        {  91.0, 883.89 },
        {  92.0, 857.17 },
        {  93.0, 831.38 },
        {  94.0, 806.49 },
        {  95.0, 782.46 },
        {  96.0, 759.26 },
        {  97.0, 736.85 },
        {  98.0, 715.21 },
        {  99.0, 694.31 },
        { 100.0, 674.11 },
        { 101.0, 654.60 },
        { 102.0, 635.74 },
        { 103.0, 617.51 },
        { 104.0, 599.88 },
        { 105.0, 582.84 }
};

static const uint16_t sizeLUT = sizeof(ntcalug01a103g_LUT)/sizeof(NTCALUG01A103G_LUT_s);


/*================== Extern Constant and Variable Definitions ===============*/
/* Defines for calculating the ADC voltage on the ends of the operating range.
 * The ADC voltage is calculated with the following formula:
 *
 * Vadc = ((Vsupply * Rntc) / (R + Rntc))
 *
 * Depending on the position of the NTC in the voltage resistor (R1/R2),
 * different Rntc values are used for the calculation.
 */
#if NTCALUG01A103G_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE
#define ADC_VOLTAGE_VMAX_V    (float)((NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * ntcalug01a103g_LUT[sizeLUT-1].resistance_Ohm) / (ntcalug01a103g_LUT[sizeLUT-1].resistance_Ohm+NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMIN_V    (float)((NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * ntcalug01a103g_LUT[0].resistance_Ohm) / (ntcalug01a103g_LUT[0].resistance_Ohm+NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#else /*NTCALUG01A103G_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */
#define ADC_VOLTAGE_VMIN_V    (float)((NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * ntcalug01a103g_LUT[sizeLUT-1].resistance_Ohm) / (ntcalug01a103g_LUT[sizeLUT-1].resistance_Ohm+NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#define ADC_VOLTAGE_VMAX_V    (float)((NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V * ntcalug01a103g_LUT[0].resistance_Ohm) / (ntcalug01a103g_LUT[0].resistance_Ohm+NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm))
#endif
/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/

extern float NTCALUG01A103G_GetTempFromLUT(uint16_t vadc_mV) {
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
#if NTCALUG01A103G_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == TRUE

        /* R1 = R2*((Vsupply/Vadc)-1) */
        resistance_Ohm = NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                ((NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V/adcVoltage_V) - 1);
#else /* NTCALUG01A103G_POSITION_IN_RESISTOR_DIVIDER_IS_R1 == FALSE */

        /* R2 = R1*(V2/(Vsupply-Vadc)) */
        resistance_Ohm = NTCALUG01A103G_RESISTOR_DIVIDER_RESISTANCE_R1_R2_Ohm *
                (adcVoltage_V/(NTCALUG01A103G_RESISTOR_DIVIDER_SUPPLY_VOLTAGE_V - adcVoltage_V));
#endif /* NTCALUG01A103G_POSITION_IN_RESISTOR_DIVIDER_IS_R1 */

        /* Variables for interpolating LUT value */
        uint16_t between_high = 0;
        uint16_t between_low = 0;
        for (uint16_t i = 1; i < sizeLUT; i++) {
            if (resistance_Ohm < ntcalug01a103g_LUT[i].resistance_Ohm) {
                between_low = i+1;
                between_high = i;
            }
        }

        /* Interpolate between LUT vales, but do not extrapolate LUT! */
        if (!((between_high == 0 && between_low == 0) ||  /* measured resistance > maximum LUT resistance */
                 (between_low > sizeLUT))) {              /* measured resistance < minimum LUT resistance */
                     temperature = MATH_linearInterpolation(ntcalug01a103g_LUT[between_low].resistance_Ohm,
                                       ntcalug01a103g_LUT[between_low].temperature_C,
                                       ntcalug01a103g_LUT[between_high].resistance_Ohm,
                                       ntcalug01a103g_LUT[between_high].temperature_C,
                                       resistance_Ohm);
        }
    }

    /* Return temperature based on measured NTC resistance */
    return temperature;
}


extern float NTCALUG01A103G_GetTempFromPolynom(uint16_t vadc_mV) {
    float temperature = 0.0;
    /* TODO */
    return temperature;
}
