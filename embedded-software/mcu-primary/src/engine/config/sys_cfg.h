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
 * @file    sys_cfg.h
 * @author  foxBMS Team
 * @date    21.09.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  SYS
 *
 * @brief   Sys driver configuration header
 */

#ifndef SYS_CFG_H_
#define SYS_CFG_H_

/*================== Includes =============================================*/
#include "general.h"

/*================== Macros and Definitions ===============================*/

/**
 * This define MUST represent the cycle time of the task in which context the
 * functions run, e.g., if the SYS_Trigger() is running in the 10 ms task
 * then the define must be set to 10.
 *
 * This define also sets the minimum time.
 */

#define SYS_TASK_CYCLE_CONTEXT_MS (10)

/**
 * SYS statemachine short time definition in ms
 */

#define SYS_STATEMACH_SHORTTIME_MS      (SYS_TASK_CYCLE_CONTEXT_MS)

/**
 * SYS statemachine short time definition in ms
 */

#define SYS_STATEMACH_MEDIUMTIME_MS     (SYS_TASK_CYCLE_CONTEXT_MS)

/**
 * SYS statemachine long time definition in ms
 */

#define SYS_STATEMACH_LONGTIME_MS       ((10) * (SYS_TASK_CYCLE_CONTEXT_MS))


/*================== Function Prototypes ==================================*/

/**
 * Function to send out boot message with SW version
 */
extern void SYS_SendBootMessage(uint8_t directTransmission);

/*================== Function Implementations =============================*/

#endif /* SYS_CFG_H_ */
