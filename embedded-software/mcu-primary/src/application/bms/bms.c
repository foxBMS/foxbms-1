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
 * @file    bms.c
 * @author  foxBMS Team
 * @date    21.09.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  BMS
 *
 * @brief   bms driver implementation
 */


/*================== Includes =============================================*/
#include "bms.h"

#include "bal.h"
#include "batterycell_cfg.h"
#include "batterysystem_cfg.h"
#include "database.h"
#include "diag.h"
#include "interlock.h"
#include "ltc_cfg.h"
#include "meas.h"
#include "os.h"
#include "plausibility.h"


/*================== Macros and Definitions ===============================*/

/**
 * Saves the last state and the last substate
 */
#define BMS_SAVELASTSTATES()    bms_state.laststate = bms_state.state; \
                                bms_state.lastsubstate = bms_state.substate;

/*================== Constant and Variable Definitions ====================*/

/**
 * contains the state of the contactor state machine
 */
static BMS_STATE_s bms_state = {
    .timer             = 0,
    .statereq          = BMS_STATE_NO_REQUEST,
    .state             = BMS_STATEMACH_UNINITIALIZED,
    .substate          = BMS_ENTRY,
    .currentFlowState  = BMS_RELAXATION,
    .laststate         = BMS_STATEMACH_UNINITIALIZED,
    .lastsubstate      = 0,
    .triggerentry      = 0,
    .ErrRequestCounter = 0,
    .initFinished      = E_NOT_OK,
    .restTimer_ms      = BS_RELAXATION_PERIOD_MS,
    .counter           = 0,
};

static DATA_BLOCK_CELLVOLTAGE_s bms_tab_cellvolt;
static DATA_BLOCK_CURRENT_SENSOR_s bms_tab_cur_sensor;
static DATA_BLOCK_MINMAX_s bms_tab_minmax;
static DATA_BLOCK_OPENWIRE_s bms_ow_tab;
static DATA_BLOCK_SOF_s bms_tab_sof;


/*================== Function Prototypes ==================================*/

static BMS_RETURN_TYPE_e BMS_CheckStateRequest(BMS_STATE_REQUEST_e statereq);
static BMS_STATE_REQUEST_e BMS_GetStateRequest(void);
static BMS_STATE_REQUEST_e BMS_TransferStateRequest(void);
static uint8_t BMS_CheckReEntrance(void);
static uint8_t BMS_CheckCANRequests(void);
static STD_RETURN_TYPE_e BMS_CheckAnyErrorFlagSet(void);
static void BMS_UpdateBatsysState(DATA_BLOCK_CURRENT_SENSOR_s *curSensor);
static void BMS_GetMeasurementValues(void);
static void BMS_CheckVoltages(void);
static void BMS_CheckTemperatures(void);
static void BMS_CheckCurrent(void);
static void BMS_CheckSlaveTemperatures(void);
static void BMS_CheckOpenSenseWire(void);

/*================== Function Implementations =============================*/

/**
 * @brief   re-entrance check of SYS state machine trigger function
 *
 * @details This function is not re-entrant and should only be called time- or event-triggered. It
 *          increments the triggerentry counter from the state variable ltc_state. It should never
 *          be called by two different processes, so if it is the case, triggerentry should never
 *          be higher than 0 when this function is called.
 *
 * @return  retval  0 if no further instance of the function is active, 0xff else
 */
static uint8_t BMS_CheckReEntrance(void) {
    uint8_t retval = 0;
    OS_TaskEnter_Critical();
    if (!bms_state.triggerentry) {
        bms_state.triggerentry++;
    } else {
        retval = 0xFF;  /* multiple calls of function */
    }
    OS_TaskExit_Critical();
    return (retval);
}

/**
 * @brief   gets the current state request.
 *
 * @details This function is used in the functioning of the SYS state machine.
 *
 * @return  current state request, taken from BMS_STATE_REQUEST_e
 */
static BMS_STATE_REQUEST_e BMS_GetStateRequest(void) {
    BMS_STATE_REQUEST_e retval = BMS_STATE_NO_REQUEST;

    OS_TaskEnter_Critical();
    retval    = bms_state.statereq;
    OS_TaskExit_Critical();

    return (retval);
}


BMS_STATEMACH_e BMS_GetState(void) {
    return (bms_state.state);
}


STD_RETURN_TYPE_e BMS_GetInitializationState(void) {
    return (bms_state.initFinished);
}


/**
 * @brief   transfers the current state request to the state machine.
 *
 * @details This function takes the current state request from cont_state and transfers it to th
 *          state machine. It resets the value from cont_state to BMS_STATE_NO_REQUEST
 *
 * @return  retVal          current state request, taken from BMS_STATE_REQUEST_e
 */
static BMS_STATE_REQUEST_e BMS_TransferStateRequest(void) {
    BMS_STATE_REQUEST_e retval = BMS_STATE_NO_REQUEST;

    OS_TaskEnter_Critical();
    retval    = bms_state.statereq;
    bms_state.statereq = BMS_STATE_NO_REQUEST;
    OS_TaskExit_Critical();
    return (retval);
}



BMS_RETURN_TYPE_e BMS_SetStateRequest(BMS_STATE_REQUEST_e statereq) {
    BMS_RETURN_TYPE_e retVal = BMS_STATE_NO_REQUEST;

    OS_TaskEnter_Critical();
    retVal = BMS_CheckStateRequest(statereq);

    if (retVal == BMS_OK) {
            bms_state.statereq = statereq;
    }
    OS_TaskExit_Critical();

    return (retVal);
}

/**
 * @brief   checks the state requests that are made.
 *
 * @details This function checks the validity of the state requests. The results of the checked is
 *          returned immediately.
 *
 * @param   statereq    state request to be checked
 *
 * @return  result of the state request that was made, taken from BMS_RETURN_TYPE_e
 */
static BMS_RETURN_TYPE_e BMS_CheckStateRequest(BMS_STATE_REQUEST_e statereq) {
    if (statereq == BMS_STATE_ERROR_REQUEST) {
        return BMS_OK;
    }

    if (bms_state.statereq == BMS_STATE_NO_REQUEST) {
        /* init only allowed from the uninitialized state */
        if (statereq == BMS_STATE_INIT_REQUEST) {
            if (bms_state.state == BMS_STATEMACH_UNINITIALIZED) {
                return BMS_OK;
            } else {
                return BMS_ALREADY_INITIALIZED;
            }
        } else {
            return BMS_ILLEGAL_REQUEST;
        }
    } else {
        return BMS_REQUEST_PENDING;
    }
}

void BMS_Trigger(void) {
    BMS_STATE_REQUEST_e statereq = BMS_STATE_NO_REQUEST;
    CONT_STATEMACH_e contstate = CONT_STATEMACH_UNDEFINED;
    DATA_BLOCK_SYSTEMSTATE_s systemstate = {0};
    uint32_t timestamp = OS_getOSSysTick();
    static uint32_t nextOpenWireCheck = 0;

    DIAG_SysMonNotify(DIAG_SYSMON_BMS_ID, 0);  /* task is running, state = ok */

    if (bms_state.state != BMS_STATEMACH_UNINITIALIZED) {
        BMS_GetMeasurementValues();
        BMS_UpdateBatsysState(&bms_tab_cur_sensor);
        BMS_CheckVoltages();
        BMS_CheckTemperatures();
        BMS_CheckCurrent();
        BMS_CheckSlaveTemperatures();
        BMS_CheckOpenSenseWire();

        /* Plausibility check */
        PL_CheckPackvoltage(&bms_tab_cellvolt, &bms_tab_cur_sensor);
    }
    /* Check re-entrance of function */
    if (BMS_CheckReEntrance()) {
        return;
    }

    if (bms_state.timer) {
        if (--bms_state.timer) {
            bms_state.triggerentry--;
            return;    /* handle state machine only if timer has elapsed */
        }
    }

    /****Happens every time the state machine is triggered**************/
    switch (bms_state.state) {
        /****************************UNINITIALIZED***********************************/
        case BMS_STATEMACH_UNINITIALIZED:
            /* waiting for Initialization Request */
            statereq = BMS_TransferStateRequest();
            if (statereq == BMS_STATE_INIT_REQUEST) {
                BMS_SAVELASTSTATES();
                bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                bms_state.state = BMS_STATEMACH_INITIALIZATION;
                bms_state.substate = BMS_ENTRY;
            } else if (statereq == BMS_STATE_NO_REQUEST) {
                /* no actual request pending */
            } else {
                bms_state.ErrRequestCounter++;  /* illegal request pending */
            }
            break;


        /****************************INITIALIZATION**********************************/
        case BMS_STATEMACH_INITIALIZATION:
            BMS_SAVELASTSTATES();

            bms_state.timer = BMS_STATEMACH_LONGTIME_MS;
            bms_state.state = BMS_STATEMACH_INITIALIZED;
            bms_state.substate = BMS_ENTRY;

            break;

        /****************************INITIALIZED*************************************/
        case BMS_STATEMACH_INITIALIZED:
            BMS_SAVELASTSTATES();
            bms_state.initFinished = E_OK;
            bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
            bms_state.state = BMS_STATEMACH_IDLE;
            bms_state.substate = BMS_ENTRY;
            break;

        /****************************IDLE*************************************/
        case BMS_STATEMACH_IDLE:
            BMS_SAVELASTSTATES();

            if (bms_state.substate == BMS_ENTRY) {
                DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                systemstate.bms_state = BMS_STATEMACH_IDLE;
                DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                break;
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                    break;
                }
            } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_STANDBY;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                }
            }
            break;


        /****************************STANDBY*************************************/
        case BMS_STATEMACH_STANDBY:
            BMS_SAVELASTSTATES();

            if (bms_state.substate == BMS_ENTRY) {
                BAL_SetStateRequest(BAL_STATE_ALLOWBALANCING_REQUEST);
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                CONT_SetStateRequest(CONT_STATE_STANDBY_REQUEST);
#endif /* BUILD_MODULE_ENABLE_CONTACTOR == 1 */
#if BUILD_MODULE_ENABLE_ILCK == 1
                ILCK_SetStateRequest(ILCK_STATE_CLOSE_REQUEST);
#endif /* BUILD_MODULE_ENABLE_ILCK == 1 */
#if LTC_STANDBY_PERIODIC_OPEN_WIRE_CHECK == TRUE
                nextOpenWireCheck = timestamp + LTC_STANDBY_OPEN_WIRE_PERIOD_ms;
#endif /* LTC_STANDBY_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                bms_state.timer = BMS_STATEMACH_MEDIUMTIME_MS;
                bms_state.substate = BMS_CHECK_ERROR_FLAGS_INTERLOCK;
                DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                systemstate.bms_state = BMS_STATEMACH_STANDBY;
                DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                break;
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS_INTERLOCK) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_INTERLOCK_CHECKED;
                    break;
                }
            } else if (bms_state.substate == BMS_INTERLOCK_CHECKED) {
                bms_state.timer = BMS_STATEMACH_VERYLONGTIME_MS;
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                break;
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                    break;
                }
            } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                if (BMS_CheckCANRequests() == BMS_REQ_ID_NORMAL) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_PRECHARGE;
                    bms_state.substate = BMS_ENTRY;
                    break;
                }
                if (BMS_CheckCANRequests() == BMS_REQ_ID_CHARGE) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_CHARGE_PRECHARGE;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
#if LTC_STANDBY_PERIODIC_OPEN_WIRE_CHECK == TRUE
                    if (nextOpenWireCheck <= timestamp) {
                        MEAS_Request_OpenWireCheck();
                        nextOpenWireCheck = timestamp + LTC_STANDBY_OPEN_WIRE_PERIOD_ms;
                    }
#endif /* LTC_STANDBY_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                }
            }
            break;

        /****************************PRECHARGE*************************************/
        case BMS_STATEMACH_PRECHARGE:
            BMS_SAVELASTSTATES();

            if (bms_state.substate == BMS_ENTRY) {
                BAL_SetStateRequest(BAL_STATE_NOBALANCING_REQUEST);
                DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                systemstate.bms_state = BMS_STATEMACH_PRECHARGE;
                DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                CONT_SetStateRequest(CONT_STATE_NORMAL_REQUEST);
#endif
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                break;
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                    break;
                }
            } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_STANDBY;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                    bms_state.substate = BMS_CHECK_CONTACTOR_NORMAL_STATE;
#else
                    bms_state.state = BMS_STATEMACH_NORMAL;
                    bms_state.substate = BMS_ENTRY;
#endif
                    break;
                }
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
            } else if (bms_state.substate == BMS_CHECK_CONTACTOR_NORMAL_STATE) {
                contstate = CONT_GetState();
                if (contstate == CONT_STATEMACH_NORMAL) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_NORMAL;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else if (contstate == CONT_STATEMACH_ERROR) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                }
#endif
            }
            break;

        /****************************NORMAL*************************************/
        case BMS_STATEMACH_NORMAL:
            BMS_SAVELASTSTATES();

            if (bms_state.substate == BMS_ENTRY) {
#if LTC_NORMAL_PERIODIC_OPEN_WIRE_CHECK == TRUE
                nextOpenWireCheck = timestamp + LTC_NORMAL_OPEN_WIRE_PERIOD_ms;
#endif /* LTC_NORMAL_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                systemstate.bms_state = BMS_STATEMACH_NORMAL;
                DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                break;
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_ERROR;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                    break;
                }
            } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_STANDBY;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
#if LTC_NORMAL_PERIODIC_OPEN_WIRE_CHECK == TRUE
                    if (nextOpenWireCheck <= timestamp) {
                        MEAS_Request_OpenWireCheck();
                        nextOpenWireCheck = timestamp + LTC_NORMAL_OPEN_WIRE_PERIOD_ms;
                    }
#endif /* LTC_NORMAL_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                }
            }
            break;

            /****************************CHARGE_PRECHARGE*************************************/
            case BMS_STATEMACH_CHARGE_PRECHARGE:
                BMS_SAVELASTSTATES();

                if (bms_state.substate == BMS_ENTRY) {
                    BAL_SetStateRequest(BAL_STATE_NOBALANCING_REQUEST);
                    DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                    systemstate.bms_state = BMS_STATEMACH_CHARGE_PRECHARGE;
                    DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                    CONT_SetStateRequest(CONT_STATE_CHARGE_REQUEST);
#endif
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    break;
                } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                    if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_ERROR;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                        break;
                    }
                } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                    if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_STANDBY;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                        bms_state.substate = BMS_CHECK_CONTACTOR_CHARGE_STATE;
#else
                        bms_state.state = BMS_STATEMACH_CHARGE;
                        bms_state.substate = BMS_ENTRY;
#endif
                        break;
                    }
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                } else if (bms_state.substate == BMS_CHECK_CONTACTOR_CHARGE_STATE) {
                    contstate = CONT_GetState();
                    if (contstate == CONT_STATEMACH_CHARGE) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_CHARGE;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else if (contstate == CONT_STATEMACH_ERROR) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_ERROR;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    }
#endif
                }
                break;




            /****************************CHARGE*************************************/
            case BMS_STATEMACH_CHARGE:
                BMS_SAVELASTSTATES();

                if (bms_state.substate == BMS_ENTRY) {
#if LTC_CHARGE_PERIODIC_OPEN_WIRE_CHECK == TRUE
                    nextOpenWireCheck = timestamp + LTC_CHARGE_OPEN_WIRE_PERIOD_ms;
#endif /* LTC_CHARGE_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                    DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                    systemstate.bms_state = BMS_STATEMACH_CHARGE;
                    DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                    if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_ERROR;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                        break;
                    }
                } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                    if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.state = BMS_STATEMACH_STANDBY;
                        bms_state.substate = BMS_ENTRY;
                        break;
                    } else {
#if LTC_CHARGE_PERIODIC_OPEN_WIRE_CHECK == TRUE
                    if (nextOpenWireCheck <= timestamp) {
                        MEAS_Request_OpenWireCheck();
                        nextOpenWireCheck = timestamp + LTC_CHARGE_OPEN_WIRE_PERIOD_ms;
                    }
#endif /* LTC_CHARGE_PERIODIC_OPEN_WIRE_CHECK == TRUE */
                        bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                        bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                        break;
                    }
                }
                break;

        /****************************ERROR*************************************/
        case BMS_STATEMACH_ERROR:
            BMS_SAVELASTSTATES();

            if (bms_state.substate == BMS_ENTRY) {
                BAL_SetStateRequest(BAL_STATE_NOBALANCING_REQUEST);
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
                CONT_SetStateRequest(CONT_STATE_ERROR_REQUEST);
#endif
                bms_state.timer = BMS_STATEMACH_VERYLONGTIME_MS;
#if BUILD_MODULE_ENABLE_ILCK == 1
                bms_state.substate = BMS_OPEN_INTERLOCK;
#else
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
#endif
                DB_ReadBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                systemstate.bms_state = BMS_STATEMACH_ERROR;
                DB_WriteBlock(&systemstate, DATA_BLOCK_ID_SYSTEMSTATE);
                break;
#if BUILD_MODULE_ENABLE_ILCK == 1
            } else if (bms_state.substate == BMS_OPEN_INTERLOCK) {
                ILCK_SetStateRequest(ILCK_STATE_OPEN_REQUEST);
                nextOpenWireCheck = timestamp + LTC_ERROR_OPEN_WIRE_PERIOD_ms;
                bms_state.timer = BMS_STATEMACH_VERYLONGTIME_MS;
                bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                break;
#endif
            } else if (bms_state.substate == BMS_CHECK_ERROR_FLAGS) {
                if (BMS_CheckAnyErrorFlagSet() == E_NOT_OK) {
                    /* we stay already in requested state */
                    if (nextOpenWireCheck <= timestamp) {
                        /* Perform open-wire check periodically */
                        MEAS_Request_OpenWireCheck();
                        nextOpenWireCheck = timestamp + LTC_ERROR_OPEN_WIRE_PERIOD_ms;
                    }
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_STATE_REQUESTS;
                    break;
                }
            } else if (bms_state.substate == BMS_CHECK_STATE_REQUESTS) {
                if (BMS_CheckCANRequests() == BMS_REQ_ID_STANDBY) {
#if BUILD_MODULE_ENABLE_ILCK == 1
                    ILCK_SetStateRequest(ILCK_STATE_CLOSE_REQUEST);
                    bms_state.substate = BMS_CHECK_INTERLOCK_CLOSE_AFTER_ERROR;
#else
                    bms_state.state = BMS_STATEMACH_STANDBY;
                    bms_state.substate = BMS_ENTRY;
#endif
                    bms_state.timer = BMS_STATEMACH_MEDIUMTIME_MS;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                }
#if BUILD_MODULE_ENABLE_ILCK == 1
            } else if (bms_state.substate == BMS_CHECK_INTERLOCK_CLOSE_AFTER_ERROR) {
                if (ILCK_GetInterlockFeedback() == ILCK_SWITCH_ON) {
                    /* TODO: check */
                    BAL_SetStateRequest(BAL_STATE_ALLOWBALANCING_REQUEST);
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.state = BMS_STATEMACH_STANDBY;
                    bms_state.substate = BMS_ENTRY;
                    break;
                } else {
                    bms_state.timer = BMS_STATEMACH_SHORTTIME_MS;
                    bms_state.substate = BMS_CHECK_ERROR_FLAGS;
                    break;
                }
#endif
            }
            break;
        default:
            break;
    }  /* end switch (bms_state.state) */

    bms_state.triggerentry--;
    bms_state.counter++;
}

/*================== Static functions =====================================*/
/*
 * @brief   Get latest database entries for static module variables
 */
static void BMS_GetMeasurementValues(void) {
    DB_ReadBlock(&bms_tab_cellvolt, DATA_BLOCK_ID_CELLVOLTAGE);
    DB_ReadBlock(&bms_tab_cur_sensor, DATA_BLOCK_ID_CURRENT_SENSOR);
    DB_ReadBlock(&bms_ow_tab, DATA_BLOCK_ID_OPEN_WIRE);
    DB_ReadBlock(&bms_tab_minmax, DATA_BLOCK_ID_MINMAX);
#if MEAS_TEST_CELL_SOF_LIMITS == TRUE
    /* Database entry only needed if current is checked against SOF values */
    DB_ReadBlock(&bms_tab_sof, DATA_BLOCK_ID_SOF);
#endif /* MEAS_TEST_CELL_SOF_LIMITS == TRUE */
}

/*
 * @brief   Checks the state requests made to the BMS state machine
 *
 * @details Checks of the state request in the database and sets this value as return value
 *
 * @return  requested state
 */
static uint8_t BMS_CheckCANRequests(void) {
    uint8_t retVal = BMS_REQ_ID_NOREQ;
    DATA_BLOCK_STATEREQUEST_s request;

    DB_ReadBlock(&request, DATA_BLOCK_ID_STATEREQUEST);

    if (request.state_request == BMS_REQ_ID_STANDBY) {
        retVal = BMS_REQ_ID_STANDBY;
    } else if (request.state_request == BMS_REQ_ID_NORMAL) {
        retVal = BMS_REQ_ID_NORMAL;
    }

#if BS_SEPARATE_POWERLINES == 1
    else if (request.state_request == BMS_REQ_ID_CHARGE) { /* NOLINT(readability/braces) */
        retVal = BMS_REQ_ID_CHARGE;
    }
#endif /*  BS_SEPARATE_POWERLINES == 1 */

    return retVal;
}

/**
 * @brief   checks the abidance by the safe operating area
 *
 * @details verify for cell voltage measurements (U), if minimum and maximum values are out of range
 */
static void BMS_CheckVoltages(void) {
    uint16_t vol_max = bms_tab_minmax.voltage_max;
    uint16_t vol_min = bms_tab_minmax.voltage_min;
    DIAG_RETURNTYPE_e retvalUndervoltMSL = DIAG_HANDLER_RETURN_ERR_OCCURRED;

    if (vol_max >= BC_VOLTMAX_MOL) {
        /* Over voltage maximum operating limit violated */
        DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MOL, DIAG_EVENT_NOK, 0);
        if (vol_max >= BC_VOLTMAX_RSL) {
            /* Over voltage recommended safety limit violated */
            DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_RSL, DIAG_EVENT_NOK, 0);
            if (vol_max >= BC_VOLTMAX_MSL) {
                /* Over voltage maximum safety limit violated */
                DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL, DIAG_EVENT_NOK, 0);
            }
        }
    }
    if (vol_max < BC_VOLTMAX_MSL) {
        /* over voltage maximum safety limit NOT violated */
        DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL, DIAG_EVENT_OK, 0);
        if (vol_max < BC_VOLTMAX_RSL) {
            /* over voltage recommended safety limit NOT violated */
            DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_RSL, DIAG_EVENT_OK, 0);
            if (vol_max < BC_VOLTMAX_MOL) {
                /* over voltage maximum operating limit NOT violated */
                DIAG_Handler(DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MOL, DIAG_EVENT_OK, 0);
            }
        }
    }

    if (vol_min <= BC_VOLTMIN_MOL) {
        /* Under voltage maximum operating limit violated */
        DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MOL, DIAG_EVENT_NOK, 0);
        if (vol_min <= BC_VOLTMIN_RSL) {
            /* Under voltage recommended safety limit violated */
            DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_RSL, DIAG_EVENT_NOK, 0);
            if (vol_min <= BC_VOLTMIN_MSL) {
                /* Under voltage maximum safety limit violated */
                retvalUndervoltMSL = DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL, DIAG_EVENT_NOK, 0);

                /* If under voltage flag is set and deep-discharge voltage is violated */
                if ((retvalUndervoltMSL == DIAG_HANDLER_RETURN_ERR_OCCURRED) &&
                        (vol_min <= BC_VOLT_DEEP_DISCHARGE)) {
                    DIAG_Handler(DIAG_CH_DEEP_DISCHARGE_DETECTED, DIAG_EVENT_NOK, 0);
                }
            }
        }
    }
    if (vol_min > BC_VOLTMIN_MSL) {
        /* under voltage maximum safety limit NOT violated */
        DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL, DIAG_EVENT_OK, 0);
        if (vol_min > BC_VOLTMIN_RSL) {
            /* under voltage recommended safety limit NOT violated */
            DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_RSL, DIAG_EVENT_OK, 0);
            if (vol_min > BC_VOLTMIN_MOL) {
                /* under voltage maximum operating limit NOT violated */
                DIAG_Handler(DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MOL, DIAG_EVENT_OK, 0);
            }
        }
    }
}


/**
 * @brief   checks the abidance by the safe operating area
 *
 * @details verify for cell temperature measurements (T), if minimum and maximum values are out of range
 */
static void BMS_CheckTemperatures(void) {
    int16_t temp_min = bms_tab_minmax.temperature_min;
    int16_t temp_max = bms_tab_minmax.temperature_max;

    /* Over temperature check */
    if (BMS_GetBatterySystemState() == BMS_DISCHARGING) {
        /* Discharge */
        if (temp_max >= BC_TEMPMAX_DISCHARGE_MOL) {
            /* Over temperature maximum operating limit violated*/
            DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MOL, DIAG_EVENT_NOK, 0);
            if (temp_max >= BC_TEMPMAX_DISCHARGE_RSL) {
                /* Over temperature recommended safety limit violated*/
                DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_RSL, DIAG_EVENT_NOK, 0);
                if (temp_max >= BC_TEMPMAX_DISCHARGE_MSL) {
                    /* Over temperature maximum safety limit violated */
                    DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (temp_max < BC_TEMPMAX_DISCHARGE_MSL) {
            /* over temperature maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL, DIAG_EVENT_OK, 0);
            if (temp_max < BC_TEMPMAX_DISCHARGE_RSL) {
                /* over temperature recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_RSL, DIAG_EVENT_OK, 0);
                if (temp_max < BC_TEMPMAX_DISCHARGE_MOL) {
                    /* over temperature maximum operating limit NOT violated */
                    DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }

    } else {
        /* Charge/Relaxation/At rest */
        if (temp_max >= BC_TEMPMAX_CHARGE_MOL) {
            /* Over temperature maximum operating limit violated */
            DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MOL, DIAG_EVENT_NOK, 0);
            if (temp_max >= BC_TEMPMAX_CHARGE_RSL) {
                /* Over temperature recommended safety limit violated */
                DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_RSL, DIAG_EVENT_NOK, 0);
                /* Over temperature maximum safety limit violated */
                if (temp_max >= BC_TEMPMAX_CHARGE_MSL) {
                    DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (temp_max < BC_TEMPMAX_CHARGE_MSL) {
            /* over temperature maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL, DIAG_EVENT_OK, 0);
            if (temp_max < BC_TEMPMAX_CHARGE_RSL) {
                /* over temperature recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_RSL, DIAG_EVENT_OK, 0);
                if (temp_max < BC_TEMPMAX_CHARGE_MOL) {
                    /* over temperature maximum operating limit NOT violated*/
                    DIAG_Handler(DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    }

    /* Under temperature check */
    if (BMS_GetBatterySystemState() == BMS_DISCHARGING) {
        /* Discharge */
        if (temp_min <= BC_TEMPMIN_DISCHARGE_MOL) {
            /* Under temperature maximum operating limit violated */
            DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MOL, DIAG_EVENT_NOK, 0);
            if (temp_min <= BC_TEMPMIN_DISCHARGE_RSL) {
                /* Under temperature recommended safety limit violated*/
                DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_RSL, DIAG_EVENT_NOK, 0);
                if (temp_min <= BC_TEMPMIN_DISCHARGE_MSL) {
                    /* Under temperature maximum safety limit violated */
                    DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (temp_min > BC_TEMPMIN_DISCHARGE_MSL) {
            /* under temperature maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL, DIAG_EVENT_OK, 0);
            if (temp_min > BC_TEMPMIN_DISCHARGE_RSL) {
                /* under temperature recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_RSL, DIAG_EVENT_OK, 0);
                if (temp_min > BC_TEMPMIN_DISCHARGE_MOL) {
                    /* under temperature maximum operating limit NOT violated*/
                    DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else {
        /* Charge/Relaxation/At rest */
        if (temp_min <= BC_TEMPMIN_CHARGE_MOL) {
            /* Under temperature maximum operating limit violated */
            DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MOL, DIAG_EVENT_NOK, 0);
            if (temp_min <= BC_TEMPMIN_CHARGE_RSL) {
                /* Under temperature recommended safety limit violated */
                DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_RSL, DIAG_EVENT_NOK, 0);
                if (temp_min <= BC_TEMPMIN_CHARGE_MSL) {
                    /* Under temperature maximum safety limit violated */
                    DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (temp_min > BC_TEMPMIN_CHARGE_MSL) {
            /* under temperature maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL, DIAG_EVENT_OK, 0);
            if (temp_min > BC_TEMPMIN_CHARGE_RSL) {
                /* under temperature recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_RSL, DIAG_EVENT_OK, 0);
                if (temp_min > BC_TEMPMIN_CHARGE_MOL) {
                    /* under temperature maximum operating limit NOT violated*/
                    DIAG_Handler(DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    }
}



/**
 * @brief   checks the abidance by the safe operating area
 *
 * @details verify for cell current measurements (I), if minimum and maximum values are out of range
 */
static void BMS_CheckCurrent(void) {
    int32_t i_current = bms_tab_cur_sensor.current;
    uint32_t i_current_abs = 0;
    BMS_CURRENT_FLOW_STATE_e i_dir = BMS_GetBatterySystemState();
    if (i_current < 0) {
        i_current_abs = - i_current;
    } else {
        i_current_abs = i_current;
    }

    /* initialize variables with default values */
    uint32_t batsys_charge_limit_msl = 0;
    DIAG_CH_ID_e batsys_charge_limit_diag_msl = DIAG_CH_OVERCURRENT_CHARGE_PL0_MSL;
    uint32_t batsys_discharge_limit_msl = 0;
    DIAG_CH_ID_e batsys_discharge_limit_diag_msl = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MSL;
    uint32_t batsys_charge_limit_rsl = 0;
    DIAG_CH_ID_e batsys_charge_limit_diag_rsl = DIAG_CH_OVERCURRENT_CHARGE_PL0_RSL;
    uint32_t batsys_discharge_limit_rsl = 0;
    DIAG_CH_ID_e batsys_discharge_limit_diag_rsl = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_RSL;
    uint32_t batsys_charge_limit_mol = 0;
    DIAG_CH_ID_e batsys_charge_limit_diag_mol = DIAG_CH_OVERCURRENT_CHARGE_PL0_MOL;
    uint32_t batsys_discharge_limit_mol = 0;
    DIAG_CH_ID_e batsys_discharge_limit_diag_mol = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MOL;

    /* get active power line */
    CONT_POWER_LINE_e powerline = CONT_GetActivePowerLine();

    /* set limits for batterysystem according to current power line */
    if (powerline == CONT_POWER_LINE_0) {
        batsys_charge_limit_msl = BS_CURRENTMAX_CHARGE_PL0_MSL_mA;
        batsys_charge_limit_diag_msl = DIAG_CH_OVERCURRENT_CHARGE_PL0_MSL;
        batsys_discharge_limit_msl = BS_CURRENTMAX_DISCHARGE_PL0_MSL_mA;
        batsys_discharge_limit_diag_msl = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MSL;

        batsys_charge_limit_rsl = BS_CURRENTMAX_CHARGE_PL0_RSL_mA;
        batsys_charge_limit_diag_rsl = DIAG_CH_OVERCURRENT_CHARGE_PL0_RSL;
        batsys_discharge_limit_rsl = BS_CURRENTMAX_DISCHARGE_PL0_RSL_mA;
        batsys_discharge_limit_diag_rsl = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_RSL;

        batsys_charge_limit_mol = BS_CURRENTMAX_CHARGE_PL0_MOL_mA;
        batsys_charge_limit_diag_mol = DIAG_CH_OVERCURRENT_CHARGE_PL0_MOL;
        batsys_discharge_limit_mol = BS_CURRENTMAX_DISCHARGE_PL0_MOL_mA;
        batsys_discharge_limit_diag_mol = DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MOL;
#if BS_SEPARATE_POWERLINES == 1
    } else if (powerline == CONT_POWER_LINE_1) {
        batsys_charge_limit_msl = BS_CURRENTMAX_CHARGE_PL1_MSL_mA;
        batsys_charge_limit_diag_msl = DIAG_CH_OVERCURRENT_CHARGE_PL1_MSL;
        batsys_discharge_limit_msl = BS_CURRENTMAX_DISCHARGE_PL1_MSL_mA;
        batsys_discharge_limit_diag_msl = DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MSL;

        batsys_charge_limit_rsl = BS_CURRENTMAX_CHARGE_PL1_RSL_mA;
        batsys_charge_limit_diag_rsl = DIAG_CH_OVERCURRENT_CHARGE_PL1_RSL;
        batsys_discharge_limit_rsl = BS_CURRENTMAX_DISCHARGE_PL1_RSL_mA;
        batsys_discharge_limit_diag_rsl = DIAG_CH_OVERCURRENT_DISCHARGE_PL1_RSL;

        batsys_charge_limit_mol = BS_CURRENTMAX_CHARGE_PL1_MOL_mA;
        batsys_charge_limit_diag_mol = DIAG_CH_OVERCURRENT_CHARGE_PL1_MOL;
        batsys_discharge_limit_mol = BS_CURRENTMAX_DISCHARGE_PL1_MOL_mA;
        batsys_discharge_limit_diag_mol = DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MOL;
#endif
    } else {
        /* this is a configuration error, assume safe default */
        batsys_charge_limit_msl = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_charge_limit_diag_msl = DIAG_CH_OVERCURRENT_PL_NONE;
        batsys_discharge_limit_msl = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_discharge_limit_diag_msl = DIAG_CH_OVERCURRENT_PL_NONE;

        batsys_charge_limit_rsl = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_charge_limit_diag_rsl = DIAG_CH_OVERCURRENT_PL_NONE;
        batsys_discharge_limit_rsl = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_discharge_limit_diag_rsl = DIAG_CH_OVERCURRENT_PL_NONE;

        batsys_charge_limit_mol = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_charge_limit_diag_mol = DIAG_CH_OVERCURRENT_PL_NONE;
        batsys_discharge_limit_mol = BS_CS_THRESHOLD_NO_CURRENT_mA;
        batsys_discharge_limit_diag_mol = DIAG_CH_OVERCURRENT_PL_NONE;
    }

    /* check limits of battery system */
    if (i_dir == BMS_CHARGING) {
        /* Charge */
        if (i_current_abs >= batsys_charge_limit_mol) {
            /* Over current maximum operating limit of batsys violated */
            DIAG_Handler(batsys_charge_limit_diag_mol, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= batsys_charge_limit_rsl) {
                /* Over current recommended safety limit of batsys violated */
                DIAG_Handler(batsys_charge_limit_diag_rsl, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= batsys_charge_limit_msl) {
                    /* Over current maximum safety limit of batsys violated */
                    DIAG_Handler(batsys_charge_limit_diag_msl, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (i_current_abs < batsys_charge_limit_msl) {
            /* Over current maximum safety limit of batsys NOT violated */
            DIAG_Handler(batsys_charge_limit_diag_msl, DIAG_EVENT_OK, 0);
            if (i_current_abs < batsys_charge_limit_rsl) {
                /* Over current recommended safety limit of batsys NOT violated */
                DIAG_Handler(batsys_charge_limit_diag_rsl, DIAG_EVENT_OK, 0);
                if (i_current_abs < batsys_charge_limit_mol) {
                    /* Over current maximum operating limit of batsys NOT violated */
                    DIAG_Handler(batsys_charge_limit_diag_mol, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else if (i_dir == BMS_DISCHARGING) {
        /* Discharge */
        if (i_current_abs >= batsys_discharge_limit_mol) {
            /* Over current maximum operating limit of batsys violated */
            DIAG_Handler(batsys_discharge_limit_diag_mol, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= batsys_discharge_limit_rsl) {
                /* Over current recommended safety limit of batsys violated */
                DIAG_Handler(batsys_discharge_limit_diag_rsl, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= batsys_discharge_limit_msl) {
                    /* Over current maximum safety limit of batsys violated */
                    DIAG_Handler(batsys_discharge_limit_diag_msl, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (i_current_abs < batsys_discharge_limit_msl) {
            /* Over current maximum safety limit of batsys NOT violated */
            DIAG_Handler(batsys_discharge_limit_diag_msl, DIAG_EVENT_OK, 0);
            if (i_current_abs < batsys_discharge_limit_rsl) {
                /* Over current recommended safety limit of batsys NOT violated */
                DIAG_Handler(batsys_discharge_limit_diag_rsl, DIAG_EVENT_OK, 0);
                if (i_current_abs < batsys_discharge_limit_mol) {
                    /* Over current maximum operating limit of batsys NOT violated */
                    DIAG_Handler(batsys_discharge_limit_diag_mol, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else {
        /* BS_CURRENT_NO_CURRENT -> no violations */
        DIAG_Handler(DIAG_CH_OVERCURRENT_PL_NONE, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL0_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL0_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL0_MOL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL1_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL1_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_PL1_MOL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL0_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MOL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL1_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MOL, DIAG_EVENT_OK, 0);
    }

    /* check limits of cells */
#if MEAS_TEST_CELL_SOF_LIMITS == TRUE
    if (i_dir == BMS_CHARGING) {
        /* Charge */
        if (i_current_abs >= bms_tab_sof.continuous_charge_MOL) {
            /* Over current maximum operating limit violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= bms_tab_sof.continuous_charge_RSL) {
                /* Over current recommended safety limit violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= bms_tab_sof.continuous_charge_MSL) {
                    /* Over current maximum safety limit violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (i_current_abs < bms_tab_sof.continuous_charge_MSL) {
            /* over current maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
            if (i_current_abs < bms_tab_sof.continuous_charge_RSL) {
                /* over current recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
                if (i_current_abs < bms_tab_sof.continuous_charge_MOL) {
                    /* over current maximum operating limit NOT violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else if (i_dir == BMS_DISCHARGING) {
        /* Discharge */
        if (i_current_abs >= bms_tab_sof.continuous_discharge_MOL) {
            /* Over current maximum operating limit violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= bms_tab_sof.continuous_discharge_RSL) {
                /* Over current recommended safety limit violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= bms_tab_sof.continuous_discharge_MSL) {
                    /* Over current error */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }

        if (i_current_abs < bms_tab_sof.continuous_discharge_MSL) {
            /* over current maximum safety limit NOT violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
            if (i_current_abs < bms_tab_sof.continuous_discharge_RSL) {
                /* over current recommended safety limit NOT violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
                if (i_current_abs < bms_tab_sof.continuous_discharge_MOL) {
                    /* over current maximum operating limit NOT violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else {
        /* BS_CURRENT_NO_CURRENT -> no check needed if no current is floating */
    }
#else /* MEAS_TEST_CELL_SOF_LIMITS == FALSE */
    if (i_dir == BMS_CHARGING) {
        /* Charge */
        if (i_current_abs >= BC_CURRENTMAX_CHARGE_MOL) {
            /* Over current maximum operating limit of cells violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= BC_CURRENTMAX_CHARGE_RSL) {
                /* Over current recommended safety limit of cells violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= BC_CURRENTMAX_CHARGE_MSL) {
                    /* Over current maximum safety limit of cells violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (i_current_abs < BC_CURRENTMAX_CHARGE_MSL) {
            /* Over current maximum safety limit of cells NOT violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
            if (i_current_abs < BC_CURRENTMAX_CHARGE_RSL) {
                /* Over current recommended safety limit of cells NOT violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
                if (i_current_abs < BC_CURRENTMAX_CHARGE_MOL) {
                    /* Over current maximum operating limit of cells NOT violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else if (i_dir == BMS_DISCHARGING) {
        /* Discharge */
        if (i_current_abs >= BC_CURRENTMAX_DISCHARGE_MOL) {
            /* Over current maximum operating limit of cells violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL, DIAG_EVENT_NOK, 0);
            if (i_current_abs >= BC_CURRENTMAX_DISCHARGE_RSL) {
                /* Over current recommended safety limit of cells violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL, DIAG_EVENT_NOK, 0);
                if (i_current_abs >= BC_CURRENTMAX_DISCHARGE_MSL) {
                    /* Over current maximum safety limit of cells violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL, DIAG_EVENT_NOK, 0);
                }
            }
        }
        if (i_current_abs < BC_CURRENTMAX_DISCHARGE_MSL) {
            /* Over current maximum safety limit of cells NOT violated */
            DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
            if (i_current_abs < BC_CURRENTMAX_DISCHARGE_RSL) {
                /* Over current recommended safety limit of cells NOT violated */
                DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
                if (i_current_abs < BC_CURRENTMAX_DISCHARGE_MOL) {
                    /* Over current maximum operating limit of cells NOT violated */
                    DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
                }
            }
        }
    } else {
        /* BS_CURRENT_NO_CURRENT -> no violations */
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL, DIAG_EVENT_OK, 0);
        DIAG_Handler(DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL, DIAG_EVENT_OK, 0);
    }
#endif /* MEAS_TEST_CELL_SOF_LIMITS == TRUE */
}

/**
 * @brief   FOR FUTURE COMPATIBILITY; DUMMY FUNCTION; DO NOT USE
 *
 * @details FOR FUTURE COMPATIBILITY; DUMMY FUNCTION; DO NOT USE
 */
static void BMS_CheckSlaveTemperatures(void) {
    /* TODO: to be implemented */
}


/**
 * @brief   Check for any open voltage sense wire
 */
static void BMS_CheckOpenSenseWire(void) {
    uint8_t openWireDetected = 0;

    /* Iterate over all modules */
    for (uint8_t m = 0; m < BS_NR_OF_MODULES; m++) {
        /* Iterate over all voltage sense wires: cells per module + 1 */
        for (uint8_t wire = 0; wire < (BS_NR_OF_BAT_CELLS_PER_MODULE + 1); wire++) {
            /* open wire detected */
            if (bms_ow_tab.openwire[wire + m*(BS_NR_OF_BAT_CELLS_PER_MODULE + 1) == 1]) {
                openWireDetected++;

                /* Add additional error handling here */
            }
        }
    }
    /* Set error if open wire detected */
    if (openWireDetected == 0u) {
        DIAG_Handler(DIAG_CH_OPEN_WIRE, DIAG_EVENT_OK, 0);
    } else {
        DIAG_Handler(DIAG_CH_OPEN_WIRE, DIAG_EVENT_NOK, 0);
    }
}


/**
 * @brief   Checks the error flags
 *
 * @details Checks all the error flags from the database and returns an error if at least one is set.
 *
 * @return  E_OK if no error flag is set, otherwise E_NOT_OK
 */
static STD_RETURN_TYPE_e BMS_CheckAnyErrorFlagSet(void) {
    STD_RETURN_TYPE_e retVal = E_OK;  /* is set to E_NOT_OK if error detected */
    DATA_BLOCK_ERRORSTATE_s error_flags;
    DATA_BLOCK_MSL_FLAG_s msl_flags;

    DB_ReadBlock(&error_flags, DATA_BLOCK_ID_ERRORSTATE);
    DB_ReadBlock(&msl_flags, DATA_BLOCK_ID_MSL);

    /* Check maximum safety limit flags */
    if (msl_flags.over_current_charge_cell    == 1 ||
        msl_flags.over_current_charge_pl0     == 1 ||
        msl_flags.over_current_charge_pl1     == 1 ||
        msl_flags.over_current_discharge_cell == 1 ||
        msl_flags.over_current_discharge_pl0  == 1 ||
        msl_flags.over_current_discharge_pl1  == 1 ||
        msl_flags.over_voltage                == 1 ||
        msl_flags.under_voltage               == 1 ||
        msl_flags.over_temperature_charge     == 1 ||
        msl_flags.over_temperature_discharge  == 1 ||
        msl_flags.under_temperature_charge    == 1 ||
        msl_flags.under_temperature_discharge == 1) {
        /* error detected */
        retVal = E_NOT_OK;
    }

    /* Check system error flags */
    if (error_flags.currentOnOpenPowerline    == 1 ||
        error_flags.deepDischargeDetected     == 1 ||
        error_flags.main_plus                 == 1 ||
        error_flags.main_minus                == 1 ||
        error_flags.precharge                 == 1 ||
        error_flags.charge_main_plus          == 1 ||
        error_flags.charge_main_minus         == 1 ||
        error_flags.charge_precharge          == 1 ||
        error_flags.fuse_state_normal         == 1 ||
        error_flags.fuse_state_charge         == 1 ||
        error_flags.interlock                 == 1 ||
        error_flags.crc_error                 == 1 ||
        error_flags.mux_error                 == 1 ||
        error_flags.spi_error                 == 1 ||
        error_flags.ltc_config_error          == 1 ||
        error_flags.currentsensorresponding   == 1 ||
        error_flags.open_wire                 == 1 ||
#if BMS_OPEN_CONTACTORS_ON_INSULATION_ERROR == TRUE
        error_flags.insulation_error          == 1 ||
#endif /* BMS_OPEN_CONTACTORS_ON_INSULATION_ERROR */
        error_flags.can_timing_cc             == 1 ||
        error_flags.can_timing                == 1) {
        /* error detected */
        retVal = E_NOT_OK;
    }

    return retVal;
}


/**
 * @brief   Updates battery system state variable depending on measured/recent
 *          current values
 *
 * @param   curSensor   recent measured values from current sensor
 */
static void BMS_UpdateBatsysState(DATA_BLOCK_CURRENT_SENSOR_s *curSensor) {
    if (POSITIVE_DISCHARGE_CURRENT == TRUE) {
        /* Positive current values equal a discharge of the battery system */
        if (curSensor->current >= BS_REST_CURRENT_mA) {
            bms_state.currentFlowState = BMS_DISCHARGING;
            bms_state.restTimer_ms = BS_RELAXATION_PERIOD_MS;
        } else if (curSensor->current <= -BS_REST_CURRENT_mA) {
            bms_state.currentFlowState = BMS_CHARGING;
            bms_state.restTimer_ms = BS_RELAXATION_PERIOD_MS;
        } else {
            /* Current below rest current: either battery system is at rest
             * or the relaxation process is still ongoing */
            if (bms_state.restTimer_ms == 0) {
                /* Rest timer elapsed -> battery system at rest */
                bms_state.currentFlowState = BMS_AT_REST;
            } else {
                bms_state.restTimer_ms--;
                bms_state.currentFlowState = BMS_RELAXATION;
            }
        }
    } else {
        /* Negative current values equal a discharge of the battery system */
        if (curSensor->current <= -BS_REST_CURRENT_mA) {
            bms_state.currentFlowState = BMS_DISCHARGING;
            bms_state.restTimer_ms = BS_RELAXATION_PERIOD_MS;
        } else if (curSensor->current >= BS_REST_CURRENT_mA) {
            bms_state.currentFlowState = BMS_CHARGING;
            bms_state.restTimer_ms = BS_RELAXATION_PERIOD_MS;
        } else {
            /* Current below rest current: either battery system is at rest
             * or the relaxation process is still ongoing */
            if (bms_state.restTimer_ms == 0) {
                /* Rest timer elapsed -> battery system at rest */
                bms_state.currentFlowState = BMS_AT_REST;
            } else {
                bms_state.restTimer_ms--;
                bms_state.currentFlowState = BMS_RELAXATION;
            }
        }
    }
}


BMS_CURRENT_FLOW_STATE_e BMS_GetBatterySystemState(void) {
    return bms_state.currentFlowState;
}
