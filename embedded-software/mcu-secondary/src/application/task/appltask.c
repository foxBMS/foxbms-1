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
 * @file    appltask.c
 * @author  foxBMS Team
 * @date    27.08.2015 (date of creation)
 * @ingroup APPLICATION
 * @prefix  APPL
 *
 * @brief   Function calls within application tasks
 */

/*================== Includes =============================================*/
#include "appltask.h"

#include "runtime_stats_light.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/**
 *  Definition of task handle 1 milliseconds task
 */
static TaskHandle_t appl_handle_tsk_1ms;

/**
 * @brief Task Struct for #appl_handle_tsk_1ms.
 */
StaticTask_t xAppl_1ms_TaskStruct;

/**
 * Stack of #appl_handle_tsk_1ms.
 */
StackType_t xAppl_1ms_Stack[ APPL_TSK_C_1MS_STACKSIZE ];

/**
 *  Definition of task handle 10 milliseconds task
 */
static TaskHandle_t appl_handle_tsk_10ms;

/**
 * @brief Task Struct for #appl_handle_tsk_10ms.
 */
StaticTask_t xAppl_10ms_TaskStruct;

/**
 * Stack of #appl_handle_tsk_10ms.
 */
StackType_t xAppl_10ms_Stack[ APPL_TSK_C_10MS_STACKSIZE ];

/**
 *  Definition of task handle 100 milliseconds task
 */
static TaskHandle_t appl_handle_tsk_100ms;

/**
 * @brief Task Struct for #appl_handle_tsk_100ms.
 */
StaticTask_t xAppl_100ms_TaskStruct;

/**
 * Stack of #appl_handle_tsk_100ms.
 */
StackType_t xAppl_100ms_Stack[ APPL_TSK_C_100MS_STACKSIZE ];

/**
 *  Definition of task handle aperiodic
 */
static TaskHandle_t appl_handle_tsk_aperiodic;

/**
 * @brief Task Struct for #appl_handle_tsk_aperiodic.
 */
StaticTask_t xAppl_aperiodic_TaskStruct;

/**
 * Stack of #appl_handle_tsk_aperiodic.
 */
StackType_t xAppl_aperiodic_Stack[ APPL_TSK_APERIODIC_STACKSIZE ];


#if BUILD_DIAG_ENABLE_TASK_STATISTICS
static TASK_METRICS_s appl_metric_tsk_1ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s appl_metric_tsk_10ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s appl_metric_tsk_100ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s appl_metric_tsk_aperiodic = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

void APPL_CreateTask(void) {
    /* Cyclic Task 1ms */
    appl_handle_tsk_1ms = xTaskCreateStatic(
            (TaskFunction_t) APPL_TSK_Cyclic_1ms,
            (const portCHAR *) "APPL_TSK_Cyclic_1ms",
            appl_tskdef_cyclic_1ms.Stacksize, NULL,
            appl_tskdef_cyclic_1ms.Priority, xAppl_1ms_Stack,
            &xAppl_1ms_TaskStruct);

    /* Cyclic Task 10ms */
    appl_handle_tsk_10ms = xTaskCreateStatic(
            (TaskFunction_t) APPL_TSK_Cyclic_10ms,
            (const portCHAR *) "APPL_TSK_Cyclic_10ms",
            appl_tskdef_cyclic_10ms.Stacksize, NULL,
            appl_tskdef_cyclic_10ms.Priority, xAppl_10ms_Stack,
            &xAppl_10ms_TaskStruct);

    /* Cyclic Task 100ms */
    appl_handle_tsk_100ms = xTaskCreateStatic(
            (TaskFunction_t) APPL_TSK_Cyclic_100ms,
            (const portCHAR *) "APPL_TSK_Cyclic_100ms",
            appl_tskdef_cyclic_100ms.Stacksize, NULL,
            appl_tskdef_cyclic_100ms.Priority, xAppl_100ms_Stack,
            &xAppl_100ms_TaskStruct);

    /* Aperiodic Task */
    appl_handle_tsk_aperiodic = xTaskCreateStatic(
            (TaskFunction_t) APPL_TSK_Aperiodic,
            (const portCHAR *) "APPL_TSK_Aperiodic",
            appl_tskdef_aperiodic.Stacksize, NULL,
            appl_tskdef_aperiodic.Priority, xAppl_aperiodic_Stack,
            &xAppl_aperiodic_TaskStruct);
}

void APPL_CreateMutex(void) {
}

void APPL_CreateEvent(void) {
}

void APPL_CreateQueues(void) {
}

void APPL_TSK_Cyclic_1ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, appl_tskdef_cyclic_1ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        APPL_Cyclic_1ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, appl_tskdef_cyclic_1ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&appl_metric_tsk_1ms, appl_tskdef_cyclic_1ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}

void APPL_TSK_Cyclic_10ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, appl_tskdef_cyclic_10ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        APPL_Cyclic_10ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, appl_tskdef_cyclic_10ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&appl_metric_tsk_10ms, appl_tskdef_cyclic_10ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}

void APPL_TSK_Cyclic_100ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, appl_tskdef_cyclic_100ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        APPL_Cyclic_100ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, appl_tskdef_cyclic_100ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&appl_metric_tsk_100ms, appl_tskdef_cyclic_100ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}

void APPL_TSK_Aperiodic(void) {
  while (os_boot != OS_SYSTEM_RUNNING) {
  }

  OS_taskDelayUntil(&os_schedulerstarttime, appl_tskdef_aperiodic.Phase);

  while (1) {
      uint32_t currentTime = OS_getOSSysTick();
      APPL_Aperiodic();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
      OS_taskDelayUntil(&currentTime, appl_tskdef_aperiodic.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
      diag_calc_runtime_stats(&appl_metric_tsk_aperiodic, appl_tskdef_aperiodic.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
  }
}
