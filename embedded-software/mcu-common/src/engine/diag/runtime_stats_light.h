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
 * @file    runtime_stats_light.h
 * @author  foxBMS Team
 * @date    18.03.2019 (date of creation)
 * @ingroup ENGINE
 * @prefix  DIAG
 *
 * @brief   Diagnosis for task runtime
 *
 * This module is an alternative to the FreeRTOS runtime stats
 * with a limited feature set.
 */

#ifndef RUNTIME_STATS_LIGHT_H_
#define RUNTIME_STATS_LIGHT_H_

/*================== Includes =============================================*/
#include "general.h"

/*================== Macros and Definitions ===============================*/

#ifndef BUILD_DIAG_ENABLE_TASK_STATISTICS
/**
 * @brief Enable task statistics
 *
 * If this define is set to 1, task statistics will be computed
 * during runtime with diag_calc_runtime_stats().
 */
#define BUILD_DIAG_ENABLE_TASK_STATISTICS 0
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */

/*================== Constant and Variable Definitions ====================*/

/**
 * @brief Struct for task metrics
 *
 * This struct stores the metrics for a task and should be passed as a
 * pointer to diag_calc_runtime_stats().
 */
typedef struct TASK_METRICS {
    uint32_t call_period;       /*!< time since the last call in ms */
    int32_t jitter;             /*!< jitter in reference to the expected call period in ms */
    uint32_t lastCalltime;      /*!< timestamp of the last call in ms */
    int32_t wait_ticks;        /*!< number of ticks that have been spent waiting */
} TASK_METRICS_s;

/*================== Function Prototypes ==================================*/
/**
 * @brief Update the runtime stats.
 *
 * This function updates a tracking struct for the call period and jitter
 * of task-calls.
 *
 * @param task_metric pointer static variable that stores the metric per task
 * @param expected_call_period time in ms that should be between each call
 * @param tick_entry_into_wait time at which the wait-function has been entered
 */
extern void diag_calc_runtime_stats(TASK_METRICS_s *task_metric, uint32_t expected_call_period, uint32_t tick_entry_into_wait);

/*================== Function Implementations =============================*/

#endif /* RUNTIME_STATS_LIGHT_H_ */
