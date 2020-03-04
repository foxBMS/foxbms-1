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
 * @file    slaveplausibility_cfg.h
 * @author  foxBMS Team
 * @date    24.01.2019 (date of creation)
 * @ingroup MODULES
 * @prefix  SPL
 *
 * @brief   plausibility checks for cell voltage and cell temperatures
 *
 */

#ifndef SLAVEPLAUSIBILITY_CFG_H_
#define SLAVEPLAUSIBILITY_CFG_H_

/*================== Includes ===============================================*/
#include "batterycell_cfg.h"

/*================== Macros and Definitions =================================*/
/**
 * @ingroup CONFIG_PLAUSIBILITY
 * Defines maximum accepted cell voltage as a this is the LTC operating limit
 * \par Type:
 * int
 */
#define SPL_MAX_CELL_VOLTAGE_LIMIT_mV            (5000)

/**
 * @ingroup CONFIG_PLAUSIBILITY
 * Defines the accepted tolerance between the average cell voltage and
 * individual cell voltage readings
 * \par Type:
 * int
 * \par Range:
 * [0, 2000]
 * \par Default:
 * 1000
*/
#define SPL_CELL_VOLTAGE_AVG_TOLERANCE_mV        (1000)

/**
 * @ingroup CONFIG_PLAUSIBILITY
 * Defines the minimum operating range temperature of cell temp measurements
 * \par Type:
 * int
 * \par Range:
 * [-150, 0]
 * \par Default:
 * -25
*/
#define SPL_MINIMUM_TEMP_MEASUREMENT_RANGE       (-50)

/**
 * @ingroup CONFIG_PLAUSIBILITY
 * Defines the maximum operating range temperature of cell temp measurements
 * \par Type:
 * int
 * \par Range:
 * [80, 250]
 * \par Default:
 * 125
*/
#define SPL_MAXIMUM_TEMP_MEASUREMENT_RANGE       (125)

/*================== Extern Constant and Variable Declarations ==============*/

/*================== Extern Function Prototypes =============================*/

#endif /* SLAVEPLAUSIBILITY_CFG_H_ */
