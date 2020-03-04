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
 * @file    enginetask.c
 * @author  foxBMS Team
 * @date    27.08.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  ENG
 *
 * @brief   Calls of functions within engine task
 *
 */

/*================== Includes =============================================*/
#include "enginetask.h"

#include "database.h"
#include "diag.h"
#include "nvramhandler.h"
#include "nvram_cfg.h"
#include "runtime_stats_light.h"

/*================== Macros and Definitions ===============================*/
/**
 * @brief Stack size of engine task
 */
#define ENG_TSK_ENGINE_STACKSIZE        (1024u/4u)

/*================== Constant and Variable Definitions ====================*/
static OS_Task_Definition_s eng_tskdef_engine  = {0, 1, OS_PRIORITY_REALTIME, ENG_TSK_ENGINE_STACKSIZE};

/**
 * Definition of task handle of the engine task
 */
static TaskHandle_t eng_handle_engine;

/**
 * @brief Task Struct for #eng_handle_engine.
 */
StaticTask_t xEng_engine_TaskStruct;

/**
 * Stack of #eng_handle_engine.
 */
StackType_t xEng_engine_Stack[ ENG_TSK_ENGINE_STACKSIZE ];

/**
 * Definition of task handle 1 millisecond task
 */
static TaskHandle_t eng_handle_tsk_1ms;

/**
 * @brief Task Struct for #eng_handle_tsk_1ms.
 */
StaticTask_t xEng_1ms_TaskStruct;

/**
 * Stack of #eng_handle_tsk_1ms.
 */
StackType_t xEng_1ms_Stack[ ENG_TSK_C_1MS_STACKSIZE ];

/**
 * Definition of task handle 10 milliseconds task
 */
static TaskHandle_t eng_handle_tsk_10ms;

/**
 * @brief Task Struct for #eng_handle_tsk_10ms.
 */
StaticTask_t xEng_10ms_TaskStruct;

/**
 * Stack of #eng_handle_tsk_10ms.
 */
StackType_t xEng_10ms_Stack[ ENG_TSK_C_10MS_STACKSIZE ];

/**
 * Definition of task handle 100 milliseconds task
 */
static TaskHandle_t eng_handle_tsk_100ms;

/**
 * @brief Task Struct for #eng_handle_tsk_100ms.
 */
StaticTask_t xEng_100ms_TaskStruct;

/**
 * Stack of #eng_handle_tsk_100ms.
 */
StackType_t xEng_100ms_Stack[ ENG_TSK_C_100MS_STACKSIZE ];

/**
 * Definition of task handle Diagnosis task
 */
static TaskHandle_t eng_handle_tsk_diagnosis;

/**
 * @brief Task Struct for #eng_handle_tsk_diagnosis.
 */
StaticTask_t xEng_diagnosis_TaskStruct;

/**
 * Stack of #eng_handle_tsk_diagnosis.
 */
StackType_t xEng_diagnosis_Stack[ ENG_TSK_DIAGNOSIS_STACKSIZE ];

/**
 * Definition of task handle EventHandler
 */
static TaskHandle_t eng_handle_tsk_eventhandler;

/**
 * @brief Task Struct for #eng_handle_eventhandler.
 */
StaticTask_t xEng_eventhandler_TaskStruct;

/**
 * Stack of #eng_handle_tsk_eventhandler.
 */
StackType_t xEng_eventhandler_Stack[ ENG_TSK_EVENTHANDLER_STACKSIZE ];

#if BUILD_DIAG_ENABLE_TASK_STATISTICS
static TASK_METRICS_s eng_metric_engine = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s eng_metric_tsk_1ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s eng_metric_tsk_10ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s eng_metric_tsk_100ms = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s eng_metric_tsk_diagnosis = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};

static TASK_METRICS_s eng_metric_tsk_eventhandler = {
        .call_period = 0,
        .jitter = 0,
        .lastCalltime = 0,
};
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

void ENG_CreateTask(void) {
    /* Database Task */
    eng_handle_engine = xTaskCreateStatic((TaskFunction_t) ENG_TSK_Engine,
            (const portCHAR *) "TSK_Engine", eng_tskdef_engine.Stacksize, NULL,
            eng_tskdef_engine.Priority, xEng_engine_Stack, &xEng_engine_TaskStruct);

    /* Cyclic Task 1ms */
    eng_handle_tsk_1ms = xTaskCreateStatic((TaskFunction_t) ENG_TSK_Cyclic_1ms,
            (const portCHAR *) "TSK_Cyclic_1ms",
            eng_tskdef_cyclic_1ms.Stacksize, NULL,
            eng_tskdef_cyclic_1ms.Priority, xEng_1ms_Stack,
            &xEng_1ms_TaskStruct);

    /* Cyclic Task 10ms */
    eng_handle_tsk_10ms = xTaskCreateStatic(
            (TaskFunction_t) ENG_TSK_Cyclic_10ms,
            (const portCHAR *) "TSK_Cyclic_10ms",
            eng_tskdef_cyclic_10ms.Stacksize, NULL,
            eng_tskdef_cyclic_10ms.Priority, xEng_10ms_Stack,
            &xEng_10ms_TaskStruct);

    /* Cyclic Task 100ms */
    eng_handle_tsk_100ms = xTaskCreateStatic(
            (TaskFunction_t) ENG_TSK_Cyclic_100ms,
            (const portCHAR *) "TSK_Cyclic_100ms",
            eng_tskdef_cyclic_100ms.Stacksize, NULL,
            eng_tskdef_cyclic_100ms.Priority, xEng_100ms_Stack,
            &xEng_100ms_TaskStruct);

    /* EventHandler Task */
    eng_handle_tsk_eventhandler = xTaskCreateStatic(
            (TaskFunction_t) ENG_TSK_EventHandler,
            (const portCHAR *) "TSK_EventHandler",
            eng_tskdef_eventhandler.Stacksize, NULL,
            eng_tskdef_eventhandler.Priority, xEng_eventhandler_Stack,
            &xEng_eventhandler_TaskStruct);

    /* Diagnosis Task */
    eng_handle_tsk_diagnosis = xTaskCreateStatic(
            (TaskFunction_t) ENG_TSK_Diagnosis,
            (const portCHAR *) "TSK_Diagnosis", eng_tskdef_diagnosis.Stacksize,
            NULL, eng_tskdef_diagnosis.Priority, xEng_diagnosis_Stack,
            &xEng_diagnosis_TaskStruct);
}

void ENG_CreateMutex(void) {
}

void ENG_CreateEvent(void) {
}

void ENG_CreateQueues(void) {
}

void ENG_TSK_Engine(void) {
    DATA_Init();
    ENG_PostOSInit();

    os_boot = OS_SYSTEM_RUNNING;

    for (;;) {
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_engine, 0, OS_getOSSysTick());
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        DATA_Task();    /* Call database manager */
        DIAG_SysMon();  /* Call Overall System Monitoring */
    }
}


void ENG_TSK_Cyclic_1ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    if (eng_init == FALSE) {
        ENG_Init();
        eng_init = TRUE;
    }

    OS_taskDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_1ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        OS_TimerTrigger(&os_timer);    /* Increment system timer os_timer */
        NVM_setOperatingHours(&bkpsram_op_hours);
        DIAG_updateFlags();
        ENG_Cyclic_1ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, eng_tskdef_cyclic_1ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_tsk_1ms, eng_tskdef_cyclic_1ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}


void ENG_TSK_Cyclic_10ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_10ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        ENG_Cyclic_10ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, eng_tskdef_cyclic_10ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_tsk_10ms, eng_tskdef_cyclic_10ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}

void ENG_TSK_Cyclic_100ms(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_100ms.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        ENG_Cyclic_100ms();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, eng_tskdef_cyclic_100ms.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_tsk_100ms, eng_tskdef_cyclic_100ms.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}


void ENG_TSK_EventHandler(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, eng_tskdef_eventhandler.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        ENG_EventHandler();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, eng_tskdef_eventhandler.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_tsk_eventhandler, eng_tskdef_eventhandler.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}

void ENG_TSK_Diagnosis(void) {
    while (os_boot != OS_SYSTEM_RUNNING) {
    }

    OS_taskDelayUntil(&os_schedulerstarttime, eng_tskdef_diagnosis.Phase);

    while (1) {
        uint32_t currentTime = OS_getOSSysTick();
        ENG_Diagnosis();
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        uint32_t time_entry_into_wait = OS_getOSSysTick();
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
        OS_taskDelayUntil(&currentTime, eng_tskdef_diagnosis.CycleTime);
#if BUILD_DIAG_ENABLE_TASK_STATISTICS
        diag_calc_runtime_stats(&eng_metric_tsk_diagnosis, eng_tskdef_diagnosis.CycleTime, time_entry_into_wait);
#endif /* BUILD_DIAG_ENABLE_TASK_STATISTICS */
    }
}
