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
 * @file    ltc_cfg.c
 * @author  foxBMS Team
 * @date    18.02.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  LTC
 *
 * @brief   Configuration for the LTC monitoring chip
 *
 */

/*================== Includes =============================================*/
#include "ltc_cfg.h"

#include "epcos_b57861s0103f045.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/**
 * Default multiplexer measurement sequence
 * Must be adapted to the application
 */
LTC_MUX_CH_CFG_s ltc_mux_seq_main_ch1[] = {
    /*  multiplexer 0 measurement */
/*         {*/
/*             .muxID    = 0, */
/*             .muxCh    = 0xFF, */
/*         }, */
    {
        .muxID    = 0,
        .muxCh    = 0,
    },
    {
        .muxID    = 0,
        .muxCh    = 1,
    },
    {
        .muxID    = 0,
        .muxCh    = 2,
    },
    {
        .muxID    = 0,
        .muxCh    = 3,
    },
    {
        .muxID    = 0,
        .muxCh    = 4,
    },
    {
        .muxID    = 0,
        .muxCh    = 5,
    },
    {
        .muxID    = 0,
        .muxCh    = 6,
    },
    {
        .muxID    = 0,
        .muxCh    = 7,
    },
/*     ,
      multiplexer 2 and 3 measurement
    {
        .muxID    = 0,
        .muxCh    = 0xFF,    disable enabled mux
    },
    {
        .muxID    = 1,
        .muxCh    = 0,
    },
    {
        .muxID    = 1,
        .muxCh    = 1,
    },
    {
        .muxID    = 1,
        .muxCh    = 2,
    },
    {
        .muxID    = 1,
        .muxCh    = 3,
    },
    {
        .muxID    = 1,
        .muxCh    = 4,
    },
    {
        .muxID    = 1,
        .muxCh    = 5,
    },
    {
        .muxID    = 1,
        .muxCh    = 6,
    },
    {
        .muxID    = 1,
        .muxCh    = 7,
    },
    {
        .muxID    = 1,
        .muxCh    = 0xFF,         disable enabled mux
    },

    {
        .muxID    = 2,
        .muxCh    = 0,
    },
    {
        .muxID    = 2,
        .muxCh    = 1,
    },
    {
        .muxID    = 2,
        .muxCh    = 2,
    },
    {
        .muxID    = 2,
        .muxCh    = 3,
    },
    {
        .muxID    = 2,
        .muxCh    = 4,
    },
    {
        .muxID    = 2,
        .muxCh    = 5,
    },
    {
        .muxID    = 2,
        .muxCh    = 6,
    },
    {
        .muxID    = 2,
        .muxCh    = 7,
    }*/
};


LTC_MUX_SEQUENZ_s ltc_mux_seq = {
    .seqptr         =  &ltc_mux_seq_main_ch1[0],
    .nr_of_steps    =  (sizeof(ltc_mux_seq_main_ch1)/sizeof(LTC_MUX_CH_CFG_s))
};


const uint8_t ltc_muxsensortemperatur_cfg[BS_NR_OF_TEMP_SENSORS_PER_MODULE] = {
    1-1 ,       /*!< index 0 = mux 0, ch 0 */
    2-1 ,       /*!< index 1 = mux 0, ch 1 */
    3-1 ,       /*!< index 2 = mux 0, ch 2 */
    4-1 ,       /*!< index 3 = mux 0, ch 3 */
    5-1 ,       /*!< index 4 = mux 0, ch 4 */
    6-1 ,       /*!< index 5 = mux 0, ch 5 */
    7-1 ,       /*!< index 6 = mux 0, ch 6 */
    8-1 ,       /*!< index 7 = mux 0, ch 7 */
    /* 9-1 ,      !< index 8 = mux 1, ch 0 */
    /* 10-1 ,     !< index 9 = mux 1, ch 1 */
    /* 11-1 ,     !< index 10 = mux 1, ch 2 */
    /* 12-1 ,     !< index 11 = mux 1, ch 3 */
    /* 13-1 ,     !< index 12 = mux 1, ch 4 */
    /* 14-1 ,     !< index 13 = mux 1, ch 5 */
    /* 15-1 ,     !< index 14 = mux 1, ch 6 */
    /* 16-1       !< index 15 = mux 1, ch 7 */
};


const uint8_t ltc_voltage_input_used[BS_MAX_SUPPORTED_CELLS] = {
#if BS_MAX_SUPPORTED_CELLS == 12 || BS_MAX_SUPPORTED_CELLS == 15 || BS_MAX_SUPPORTED_CELLS == 18
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
    1 ,
#endif
#if BS_MAX_SUPPORTED_CELLS == 15 || BS_MAX_SUPPORTED_CELLS == 18
    1 ,
    1 ,
    1 ,
#endif
#if BS_MAX_SUPPORTED_CELLS == 18
    1 ,
    1 ,
    1 ,
#endif
};


/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

float LTC_Convert_MuxVoltages_to_Temperatures(float v_adc) {
    float temperature = 0.0;

    /* Example: 5th grade polynomial for EPCOS B57861S0103F045 NTC-Thermistor, 10 kOhm, Series B57861S, Vref = 3V, R in series 10k */
    /* temperature = B57861S0103F045_GetTempFromPolynom(v_adc*1000); */

    /* Dummy function, must be adapted to the application */
    temperature = 10 * v_adc;

    return temperature;
}
