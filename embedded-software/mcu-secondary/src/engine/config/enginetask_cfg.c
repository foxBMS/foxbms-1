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
 * @file    enginetask_cfg.c
 * @author  foxBMS Team
 * @date    26.08.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  ENG
 *
 * @brief   Engine task configuration
 *
 */

/*================== Includes =============================================*/
#include "enginetask_cfg.h"

#include "adc.h"
#include "database.h"
#include "interlock.h"
#include "meas.h"
#include "ltc.h"
#include "sys.h"
#include "vic.h"
#include "wdg.h"
#include "hwinfo.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
OS_Task_Definition_s eng_tskdef_cyclic_1ms   = { 0,      1,  OS_PRIORITY_ABOVE_HIGH,        ENG_TSK_C_1MS_STACKSIZE };
OS_Task_Definition_s eng_tskdef_cyclic_10ms  = { 2,     10,  OS_PRIORITY_HIGH,              ENG_TSK_C_10MS_STACKSIZE };
OS_Task_Definition_s eng_tskdef_cyclic_100ms = { 56,    100,  OS_PRIORITY_ABOVE_NORMAL,     ENG_TSK_C_100MS_STACKSIZE };
OS_Task_Definition_s eng_tskdef_eventhandler = { 0,      1,  OS_PRIORITY_VERY_HIGH,         ENG_TSK_EVENTHANDLER_STACKSIZE };
OS_Task_Definition_s eng_tskdef_diagnosis    = { 0,      1,  OS_PRIORITY_BELOW_REALTIME,    ENG_TSK_DIAGNOSIS_STACKSIZE };

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

void ENG_PostOSInit(void) {
    uint32_t retErrorCode = 0;

    os_boot = OS_RUNNING;

    retErrorCode = VIC_PostOsInterruptInit();
    if (retErrorCode != 0) {
        DIAG_Handler(DIAG_CH_VIC_INIT_FAILURE, DIAG_EVENT_NOK, retErrorCode);  /* error event in vic init */
    }
    retErrorCode = 0;

    os_boot = OS_BMS_INIT;
}


void ENG_Init(void) {
    SYS_RETURN_TYPE_e sys_retVal = SYS_ILLEGAL_REQUEST;

    /*  Init Sys */
    sys_retVal = SYS_SetStateRequest(SYS_STATE_INIT_REQUEST);

    /* This function operates under the assumption that it is called when
     * the operating system is not yet running.
     * In this state the return value of SYS_SetStateRequest() should
     * always be #SYS_OK. Therefore we trap otherwise.
     */
    if (sys_retVal != SYS_OK) {
        configASSERT(0);
    }
}

void ENG_Cyclic_1ms(void) {
    SYS_Trigger();
    ILCK_Trigger();
    LTC_Trigger();
}

void ENG_Cyclic_10ms(void) {
#if BUILD_MODULE_ENABLE_WATCHDOG
    WDG_IWDG_Refresh();
#endif
}

void ENG_Cyclic_100ms(void) {
    static uint8_t counter = 0;

    ADC_Ctrl();
    HW_update();

    counter++;
}

void ENG_IdleTask(void) {
}

void ENG_EventHandler(void) {
}

void ENG_Diagnosis(void) {
}
