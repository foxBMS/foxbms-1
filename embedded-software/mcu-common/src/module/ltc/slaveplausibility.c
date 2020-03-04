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
 * @file    slaveplausibility.c
 * @author  foxBMS Team
 * @date    24.01.2019 (date of creation)
 * @ingroup MODULES
 * @prefix  SPL
 *
 * @brief   slave plausibility checks for cell voltage and cell temperatures
 *
 */

/*================== Includes ===============================================*/
#include "slaveplausibility.h"

#include "batterysystem_cfg.h"
#include "diag.h"
#include "foxmath.h"

/*================== Macros and Definitions =================================*/

/*================== Static Constant and Variable Definitions ===============*/

/*================== Extern Constant and Variable Definitions ===============*/

/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/
extern STD_RETURN_TYPE_e PL_CheckVoltageMinMax(DATA_BLOCK_CELLVOLTAGE_s *ptrCellvoltage) {
    STD_RETURN_TYPE_e retval = E_OK;

    /* Pointer validity check */
    if (ptrCellvoltage != NULL_PTR) {
        /* Iterate over all modules */
        for (uint16_t mod = 0; mod < BS_NR_OF_MODULES; mod++) {
            /* Iterate over all cells */
            for (uint16_t cell = 0; cell < BS_NR_OF_BAT_CELLS_PER_MODULE; cell++) {
                /* Cell voltage above measurement range */
                if (ptrCellvoltage->voltage[(mod*BS_NR_OF_BAT_CELLS_PER_MODULE) + cell] > SPL_MAX_CELL_VOLTAGE_LIMIT_mV) {
                    /* Cell temperature below range -> set invalid flag */
                    /* Deviation too large -> set invalid flag */
                    ptrCellvoltage->valid_volt[mod] |= (0x01 << cell);
                    retval = E_NOT_OK;
                }
            }
        }
    } else {
        /* Invalid pointer -> TODO: error handling */
    }
    return retval;
}


extern STD_RETURN_TYPE_e PL_CheckVoltageSpread(DATA_BLOCK_CELLVOLTAGE_s *ptrCellvoltage, uint16_t averageCellVolt_mV) {
    STD_RETURN_TYPE_e retval = E_OK;
    int16_t diff = 0;

    /* Pointer validity check */
    if (ptrCellvoltage != NULL_PTR) {
        /* Iterate over all modules */
        for (uint16_t mod = 0; mod < BS_NR_OF_MODULES; mod++) {
            /* Iterate over all cells */
            for (uint16_t cell = 0; cell < BS_NR_OF_BAT_CELLS_PER_MODULE; cell++) {
                /* Cell voltage deviation from average cell voltage too large */
                diff = (averageCellVolt_mV - ptrCellvoltage->voltage[(mod*BS_NR_OF_BAT_CELLS_PER_MODULE) + cell]);
                if (abs(diff) > SPL_CELL_VOLTAGE_AVG_TOLERANCE_mV) {
                    /* Deviation too large -> set invalid flag */
                    ptrCellvoltage->valid_volt[mod] |= (0x01 << cell);
                    retval = E_NOT_OK;
                }
            }
        }
    } else {
        /* Invalid pointer -> TODO: error handling */
    }
    return retval;
}

extern STD_RETURN_TYPE_e PL_CheckTempMinMax(DATA_BLOCK_CELLTEMPERATURE_s *ptrCelltemperature) {
    STD_RETURN_TYPE_e retval = E_OK;

    /* Pointer validity check */
    if (ptrCelltemperature != NULL_PTR) {
        /* Iterate over all modules */
        for (uint16_t mod = 0; mod < BS_NR_OF_MODULES; mod++) {
            /* Iterate over all cells */
            for (uint16_t sensor = 0; sensor < BS_NR_OF_TEMP_SENSORS_PER_MODULE; sensor++) {
                /* Cell temperature below measurement range */
                if (ptrCelltemperature->temperature[(mod*BS_NR_OF_TEMP_SENSORS_PER_MODULE) + sensor] < SPL_MINIMUM_TEMP_MEASUREMENT_RANGE) {
                    /* Cell temperature below range -> set invalid flag */
                    /* Deviation too large -> set invalid flag */
                    ptrCelltemperature->valid_temperature[mod] |= (0x01 << sensor);
                    retval = E_NOT_OK;
                }

                /* Cell temperature above measurement range */
                if (ptrCelltemperature->temperature[(mod*BS_NR_OF_TEMP_SENSORS_PER_MODULE) + sensor] > SPL_MAXIMUM_TEMP_MEASUREMENT_RANGE) {
                    /* Cell temperature above range -> set invalid flag */
                    /* Deviation too large -> set invalid flag */
                    ptrCelltemperature->valid_temperature[mod] |= (0x01 << sensor);
                    retval = E_NOT_OK;
                }
            }
        }
    } else {
        /* Invalid pointer -> TODO: error handling */
    }
    return retval;
}
