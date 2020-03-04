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
 * @file    plausibility.c
 * @author  foxBMS Team
 * @date    24.01.2019 (date of creation)
 * @ingroup APPLICATION
 * @prefix  PL
 *
 * @brief   plausibility checks for cell voltage and cell temperatures
 *
 */

/*================== Includes ===============================================*/
#include "plausibility.h"

#include "batterysystem_cfg.h"
#include "diag.h"
#include "foxmath.h"

/*================== Macros and Definitions =================================*/

/*================== Static Constant and Variable Definitions ===============*/

/*================== Extern Constant and Variable Definitions ===============*/

/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/
extern void PL_CheckPackvoltage(DATA_BLOCK_CELLVOLTAGE_s* ptrCellvolt, DATA_BLOCK_CURRENT_SENSOR_s* ptrCurSensor) {
    int32_t diff = 0;
    STD_RETURN_TYPE_e result = E_NOT_OK;

    /* Pointer validity check */
    if (ptrCellvolt != NULL_PTR && ptrCurSensor != NULL_PTR) {
        /* Get deviation between these two measurements */
        diff = (uint32_t)(ptrCellvolt->packVoltage_mV - ptrCurSensor->voltage[0]);

        if (abs(diff) < PL_PACK_VOLTAGE_TOLERANCE_mV) {
            result = E_OK;
        }
        DIAG_checkEvent(result, DIAG_CH_PLAUSIBILITY_PACK_VOLTAGE, 0);
    } else {
        /* Invalid pointer -> TODO: error handling */
    }
}
