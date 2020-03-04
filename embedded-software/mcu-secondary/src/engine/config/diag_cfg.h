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
 * @file    diag_cfg.h
 * @author  foxBMS Team
 * @date    09.11.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  DIAG
 *
 * @brief   Diagnostic module configuration header
 *
 * In this header filer are the different diagnosis channel
 * defines assigned to different diagnosis IDs.
 *
 * Furthermore are the diagnosis error log settings be configured here..
 */

#ifndef DIAG_CFG_H_
#define DIAG_CFG_H_

/*================== Includes ===============================================*/
#include "general.h"

#include "batterysystem_cfg.h"

/*================== Macros and Definitions =================================*/
#define DIAG_ERROR_SENSITIVITY_HIGH         (0)    /* logging at first event */
#define DIAG_ERROR_SENSITIVITY_MID          (5)    /* logging at fifth event */
#define DIAG_ERROR_SENSITIVITY_LOW          (10)   /* logging at tenth event */

#define DIAG_ERROR_VOLTAGE_SENSITIVITY_MSL      (500)   /*!< MSL level for event occurrence if over/under voltage event   */
#define DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL  (500)   /*!< MSL level for event occurrence if over/under temperature event    */
#define DIAG_ERROR_CURRENT_SENSITIVITY_MSL      (500)   /*!< MSL level for event occurrence if over/under current event        */

#define DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MSL   (500)   /*!< MSL level for event occurrence if slave PCB temperature event     */

#define DIAG_ERROR_LTC_PEC_SENSITIVITY          (5)
#define DIAG_ERROR_LTC_MUX_SENSITIVITY          (5)
#define DIAG_ERROR_LTC_SPI_SENSITIVITY          (5)

#define DIAG_ERROR_CAN_TIMING_SENSITIVITY       (100)
#define DIAG_ERROR_CAN_TIMING_CC_SENSITIVITY    (100)
#define DIAG_ERROR_CAN_SENSOR_SENSITIVITY       (100)

#define DIAG_ERROR_MAIN_PLUS_SENSITIVITY        (50)
#define DIAG_ERROR_MAIN_MINUS_SENSITIVITY       (50)
#define DIAG_ERROR_PRECHARGE_SENSITIVITY        (50)

#define DIAG_ERROR_INTERLOCK_SENSITIVITY        (10)

/** Number of errors that can be logged */
#define DIAG_FAIL_ENTRY_LENGTH              (50)

/** Maximum number of the same errors that are logged */
#define DIAG_MAX_ENTRIES_OF_ERROR           (5)

/** Number of contactor errors that are logged */
#define DIAG_FAIL_ENTRY_CONTACTOR_LENGTH    (50)


typedef enum {
    DIAG_CH_FLASHCHECKSUM,                          /*  */
    DIAG_CH_BKPDIAG_FAILURE,                        /*  */
    DIAG_CH_WATCHDOGRESET_FAILURE,                  /*  */
    DIAG_CH_POSTOSINIT_FAILURE,                     /*  */
    DIAG_CH_CALIB_EEPR_FAILURE,                     /*  */
    DIAG_CH_CAN_INIT_FAILURE,                       /*  */
    DIAG_CH_VIC_INIT_FAILURE,
    /* HW-/SW-Runtime events: 16-31 */
    DIAG_CH_DIV_BY_ZERO_FAILURE,                    /*  */
    DIAG_CH_UNDEF_INSTRUCTION_FAILURE,              /*  */
    DIAG_CH_DATA_BUS_FAILURE,                       /*  */
    DIAG_CH_INSTRUCTION_BUS_FAILURE,                /*  */
    DIAG_CH_HARDFAULT_NOTHANDLED,                   /*  */
    DIAG_CH_RUNTIME_ERROR_RESERVED_1,               /*  reserved for future needs */
    DIAG_CH_RUNTIME_ERROR_RESERVED_2,               /*  reserved for future needs */
    DIAG_CH_RUNTIME_ERROR_RESERVED_3,               /*  reserved for future needs */
    DIAG_CH_CONFIGASSERT,                           /*  */
    DIAG_CH_SYSTEMMONITORING_TIMEOUT,               /*  */
    /* Measurement events: 32-47 */
    DIAG_CH_CANS_MAX_VALUE_VIOLATE,
    DIAG_CH_CANS_MIN_VALUE_VIOLATE,
    DIAG_CH_CANS_CAN_MOD_FAILURE,
    DIAG_CH_ISOMETER_TIM_ERROR,                     /* Measured frequency too low or no new value captured during last cycle */
    DIAG_CH_ISOMETER_GROUNDERROR,                   /* Ground error detected */
    DIAG_CH_ISOMETER_ERROR,                         /* Device error, invalid measurement result */
    DIAG_CH_ISOMETER_MEAS_INVALID,                  /* Measurement trustworthy or not, hysteresis to ground error flag */
    DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL,            /* Cell voltage limits violated */
    DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL,           /* Cell voltage limits violated */
    DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL,        /* Temperature limits violated */
    DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL,     /* Temperature limits violated */
    DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL,       /* Temperature limits violated */
    DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL,    /* Temperature limits violated */
    DIAG_CH_LTC_SPI,                                /* LTC */
    DIAG_CH_LTC_PEC,                                /* LTC */
    DIAG_CH_LTC_MUX,                                /* LTC */
    DIAG_CH_LTC_CONFIG,                             /* LTC */

/* Contactor events: 64-79 */
    DIAG_CH_CONTACTOR_DAMAGED, /* Opening contactor at over current */
    DIAG_CH_CONTACTOR_OPENING, /* counter for contactor opening */
    DIAG_CH_CONTACTOR_CLOSING, /* counter for contactor closing */
    DIAG_CH_INTERLOCK_FEEDBACK, /* Interlock feedback error */
    DIAG_CH_SLAVE_PCB_UNDERTEMPERATURE_MSL,
    DIAG_CH_SLAVE_PCB_OVERTEMPERATURE_MSL,
    DIAG_CH_ERROR_MCU_DIE_TEMPERATURE, /* MCU die temperature */
    DIAG_CH_LOW_COIN_CELL_VOLTAGE, /* coin cell voltage */
    DIAG_CH_CRIT_LOW_COIN_CELL_VOLTAGE, /* coin cell voltage */
    DIAG_CH_PLAUSIBILITY_CELL_VOLTAGE, /* plausibility checks */
    DIAG_CH_PLAUSIBILITY_CELL_TEMP, /* plausibility checks */
    DIAG_ID_MAX, /* MAX indicator - do not change */
} DIAG_CH_ID_e;

/** diagnosis check result (event) */
typedef enum {
    DIAG_EVENT_OK, /*!< diag channel event OK */
    DIAG_EVENT_NOK, /*!< diag channel event NOK */
    DIAG_EVENT_RESET, /*!< reset diag channel eventcounter to 0 */
} DIAG_EVENT_e;

/**
 * enable state of diagnosis entry
 */
typedef enum {
    DIAG_ENABLED,
    DIAG_DISABLED,
} DIAG_ENABLE_STATE_e;


/**
 * diagnosis recording activation
 */
typedef enum {
    DIAG_RECORDING_ENABLED,    /*!< enable diagnosis event recording   */
    DIAG_RECORDING_DISABLED,    /*!< disable diagnosis event recording  */
} DIAG_TYPE_RECORDING_e;

/*  FIXME some enums are typedefed with DIAG...TYPE_e, some with DIAG_TYPE..._e! Reconsider this */
/**
 * diagnosis types for system monitoring
 */
typedef enum {
    DIAG_SYSMON_CYCLICTASK, /*!< */
    DIAG_SYSMON_RESERVED,   /*!< */
} DIAG_SYSMON_TYPE_e;

/**
 * diagnosis handling type for system monitoring
 */
typedef enum {
    DIAG_SYSMON_HANDLING_DONOTHING,             /*!< */
    DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR,    /*!< */
} DIAG_SYSMON_HANDLING_TYPE_e;


/**
 * @brief listing of system-relevant tasks or functions which are checked by system monitoring
 *
 * diag_sysmon_ch_cfg[]=
 */
typedef enum {
    DIAG_SYSMON_DATABASE_ID,        /*!< diag entry for database                */
    DIAG_SYSMON_SYS_ID,             /*!< diag entry for sys              */
    DIAG_SYSMON_BMS_ID,             /*!< diag entry for bms              */
    DIAG_SYSMON_ILCK_ID,            /*!< diag entry for bms              */
    DIAG_SYSMON_LTC_ID,             /*!< diag entry for ltc                     */
    DIAG_SYSMON_APPL_CYCLIC_1ms,    /*!< diag entry for application 10ms task   */
    DIAG_SYSMON_APPL_CYCLIC_10ms,   /*!< diag entry for application 10ms task   */
    DIAG_SYSMON_APPL_CYCLIC_100ms,  /*!< diag entry for application 100ms task  */
    DIAG_SYSMON_MODULE_ID_MAX,      /*!< end marker do not delete               */
} DIAG_SYSMON_MODULE_ID_e;

/*  FIXME doxygen comment */
/*  FIXME is DIAG_CODE_s an appropriate name for this? */
typedef struct {
    uint32_t GENERALmsk;
    uint32_t CELLMONmsk;
    uint32_t COMmsk;
    uint32_t ADCmsk;
} DIAG_CODE_s;


/**
 * Channel configuration of one diag channel
*/
typedef struct {
    DIAG_CH_ID_e id;                        /*!< diagnosis event id diag_id */
    uint8_t description[40];
    uint16_t thresholds;                     /*!< threshold for number of events which will be tolerated before generating a notification in both direction (OK or NOT OK)
                                             *   threshold = 0: reports the value at first occurence, threshold = 1:reports the value at second occurence*/
    DIAG_TYPE_RECORDING_e enablerecording;  /*!< if enabled recording in diag_memory will be activated */
    DIAG_ENABLE_STATE_e state;              /*!< if enabled diagnosis event will be evaluated */
    void (*callbackfunc)(DIAG_CH_ID_e, DIAG_EVENT_e);     /*!< will be called if number of events exceeds threshold (in both direction) with parameter DIAG_EVENT_e */
} DIAG_CH_CFG_s;


/**
 * struct for device Configuration of diag module
 */
typedef struct {
    uint8_t nr_of_ch;       /*!< number of entries in DIAG_CH_CFG_s */
    DIAG_CH_CFG_s *ch_cfg;  /*!< pointer to diag channel config struct */
} DIAG_DEV_s;

/**
 * state (in summary) used for task or function notification
 */
typedef struct {
    uint32_t state;     /*!< state              */
    uint32_t timestamp; /*!< timestamp of state */
} DIAG_SYSMON_NOTIFICATION_s;


/**
 * Channel configuration of one system monitoring channel
 */
typedef struct {
    DIAG_SYSMON_MODULE_ID_e id;                     /*!< the diag type by its symbolic name            */
    DIAG_SYSMON_TYPE_e type;                        /*!< system monitoring types: cyclic or special    */
    uint16_t threshold;                             /*!< max. delay time in ms                         */
    DIAG_TYPE_RECORDING_e enablerecording;          /*!< enabled if set to DIAG_RECORDING_ENABLED      */
    DIAG_SYSMON_HANDLING_TYPE_e handlingtype;       /*!< type of handling of system monitoring errors  */
    DIAG_ENABLE_STATE_e state;                      /*!< enable or disable system monitoring           */
    void (*callbackfunc)(DIAG_SYSMON_MODULE_ID_e);  /*!< */
} DIAG_SYSMON_CH_CFG_s;

/*================== Extern Constant and Variable Declarations ==============*/
/**
 * diag device configuration struct
 */
extern DIAG_DEV_s diag_dev;

/**
 * diag system monitoring struct
 */
extern DIAG_SYSMON_CH_CFG_s diag_sysmon_ch_cfg[];
extern DIAG_CH_CFG_s  diag_ch_cfg[];

/*  FIXME why is it in header at all? and why is it in code at all? not used */
extern DIAG_CODE_s diag_mask;

/*================== Extern Function Prototypes =============================*/
/**
 * @brief update function for diagnosis flags (errors, MOL/RSL/MSL violations)
 */
extern void DIAG_updateFlags(void);

#endif /* DIAG_CFG_H_ */
