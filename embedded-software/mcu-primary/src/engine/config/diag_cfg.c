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
 * @file    diag_cfg.c
 * @author  foxBMS Team
 * @date    09.11.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  DIAG
 *
 * @brief   Diagnostic module configuration
 *
 * The configuration of the different diagnosis events defined in diag_cfg.h is set in the array
 * diag_ch_cfg[], e.g. initialization errors or runtime errors.
 *
 * Every entry of the diag_ch_cfg[] array consists of
 *  - name of the diagnosis event (defined in diag_cfg.h)
 *  - type of diagnosis event
 *  - diagnosis sensitivity (after how many occurrences event is counted as error)
 *  - enabling of the recording for diagnosis event
 *  - enabling of the diagnosis event
 *  - callback function for diagnosis event if wished, otherwise dummyfu
 *
 * The system monitoring configuration defined in diag_cfg.h is set in the array
 * diag_sysmon_ch_cfg[]. The system monitoring is at the moment only used for
 * supervising the cyclic/periodic tasks.
 *
 * Every entry of the diag_sysmon_ch_cfg[] consists of
 *  - enum of monitored object
 *  - type of monitored object (at the moment only DIAG_SYSMON_CYCLICTASK is supported)
 *  - maximum delay in [ms] in which the object needs to call the DIAG_SysMonNotify function defined in diag.c
 *  - enabling of the recording for system monitoring
 *  - enabling of the system monitoring for the monitored object
 *  - callback function if system monitoring notices an error if wished, otherwise dummyfu2
 */

/*================== Includes ===============================================*/
#include "diag_cfg.h"

#include "database.h"
#include "rtc.h"

/*================== Macros and Definitions =================================*/

/*================== Static Constant and Variable Definitions ===============*/
static DATA_BLOCK_ERRORSTATE_s error_flags = { 0 };
static DATA_BLOCK_MOL_FLAG_s mol_flags = { 0 };
static DATA_BLOCK_RSL_FLAG_s rsl_flags = { 0 };
static DATA_BLOCK_MSL_FLAG_s msl_flags = { 0 };

/*================== Static Function Prototypes =============================*/
/* dummy functions */
static void dummyfu(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void dummyfu2(DIAG_SYSMON_MODULE_ID_e ch_id);

/* functions for SOA related events */
static void DIAG_overvoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_undervoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_overtemperature_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_overtemperature_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_undertemperature_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_undertemperature_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_overcurrent_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_overcurrent_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);

/* functions for system related events */
static void DIAG_error_cantiming(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_ltc(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_cancurrentsensor(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_cont_feedback(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_fuseState(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_interlock(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_insulation(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_openWire(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_deep_discharge_detected(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_MCUdieTemperature(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_coinCellVoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);

/* functions for plausibility related events */
static void DIAG_error_plausibility_check(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);

/*================== Extern Constant and Variable Definitions ===============*/
/**
 * Enable and Disable of Error Checks for Testing Purposes
 *
 * Each Bit enables or disables the diagnosis (checking of) the corresponding failure code
 * FIXME struct isn't used anywhere in the code at the moment.
 * FIXME delete if not needed
*/
DIAG_CODE_s diag_mask = {
        .GENERALmsk = 0xFFFFFFFF,
        .CELLMONmsk = 0xFFFFFFFF,
        .COMmsk = 0xFFFFFFFF,
        .ADCmsk = 0xFFFFFFFF,
};

DIAG_CH_CFG_s  diag_ch_cfg[] = {
    /* OS-Framework and startup events */
    {DIAG_CH_FLASHCHECKSUM,                             "FLASHCHECKSUM",                        DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_BKPDIAG_FAILURE,                           "BKPDIAG",                              DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_WATCHDOGRESET_FAILURE,                     "WATCHDOGRESET",                        DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_POSTOSINIT_FAILURE,                        "POSTOSINIT",                           DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CALIB_EEPR_FAILURE,                        "CALIB_EEPR",                           DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CAN_INIT_FAILURE,                          "CAN_INIT",                             DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_VIC_INIT_FAILURE,                          "VIC_INIT",                             DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    /* HW-/SW-Runtime events */
    {DIAG_CH_DIV_BY_ZERO_FAILURE,                       "DIV_BY_ZERO",                          DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_UNDEF_INSTRUCTION_FAILURE,                 "UNDEF_INSTRUCTION",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_DATA_BUS_FAILURE,                          "DATA_BUS_FAILURE",                     DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_INSTRUCTION_BUS_FAILURE,                   "INSTRUCTION_BUS",                      DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_HARDFAULT_NOTHANDLED,                      "HARDFAULT_NOTHANDLED",                 DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_CONFIGASSERT,                              "CONFIGASSERT",                         DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SYSTEMMONITORING_TIMEOUT,                  "SYSTEMMONITORING_TIMEOUT",             DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},


    /* Measurement events */
    {DIAG_CH_CANS_MAX_VALUE_VIOLATE,                    "CANS_MAX_VALUE_VIOLATE",               DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CANS_MIN_VALUE_VIOLATE,                    "CANS_MIN_VALUE_VIOLATE",               DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CANS_CAN_MOD_FAILURE,                      "CANS_CAN_MOD_FAILURE",                 DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

#if BUILD_MODULE_ENABLE_ISOGUARD == 1
    {DIAG_CH_ISOMETER_TIM_ERROR,                        "ISOMETER_TIM_ERROR",                   DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_GROUNDERROR,                      "ISOMETER_GROUNDERROR",                 DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_ERROR,                            "ISOMETER_ERROR",                       DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_MEAS_INVALID,                     "ISOMETER_MEAS_INVALID",                DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_INSULATION_ERROR,                          "INSULATION_ERROR",                     DIAG_ERROR_INSULATION_SENSITIVITY,        DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_insulation},
#else
    {DIAG_CH_ISOMETER_TIM_ERROR,                        "ISOMETER_TIM_ERROR",                   DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_GROUNDERROR,                      "ISOMETER_GROUNDERROR",                 DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_ERROR,                            "ISOMETER_ERROR",                       DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_MEAS_INVALID,                     "ISOMETER_MEAS_INVALID",                DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_INSULATION_ERROR,                          "INSULATION_ERROR",                     DIAG_ERROR_INSULATION_SENSITIVITY,        DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
#endif

    /* Under and over temperature, voltage and current at cell level */
    {DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL,             "CELLVOLT_OVERVOLT_MSL",            DIAG_ERROR_VOLTAGE_SENSITIVITY_MSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overvoltage},
    {DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_RSL,             "CELLVOLT_OVERVOLT_RSL",            DIAG_ERROR_VOLTAGE_SENSITIVITY_RSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overvoltage},
    {DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MOL,           "CELLVOLT_OVERVOLT_MOL",              DIAG_ERROR_VOLTAGE_SENSITIVITY_MOL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overvoltage},

    {DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL,            "CELLVOLT_UNDERVOLT_MSL",           DIAG_ERROR_VOLTAGE_SENSITIVITY_MSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undervoltage},
    {DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_RSL,            "CELLVOLT_UNDERVOLT_RSL",           DIAG_ERROR_VOLTAGE_SENSITIVITY_RSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undervoltage},
    {DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MOL,          "CELLVOLT_UNDERVOLT_MOL",             DIAG_ERROR_VOLTAGE_SENSITIVITY_MOL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undervoltage},

    {DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL,         "OVERTEMP_CHARGE_MSL",             DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_charge},
    {DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_RSL,         "OVERTEMP_CHARGE_RSL",             DIAG_ERROR_TEMPERATURE_SENSITIVITY_RSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_charge},
    {DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MOL,       "OVERTEMP_CHARGE_MOL",               DIAG_ERROR_TEMPERATURE_SENSITIVITY_MOL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_charge},

    {DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL,      "OVERTEMP_DISCHARGE_MSL",          DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_discharge},
    {DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_RSL,      "OVERTEMP_DISCHARGE_RSL",          DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_discharge},
    {DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MOL,    "OVERTEMP_DISCHARGE_MOL",            DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_discharge},

    {DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL,        "UNDERTEMP_CHARGE_MSL",            DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_charge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_RSL,        "UNDERTEMP_CHARGE_RSL",            DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_charge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MOL,      "UNDERTEMP_CHARGE_MOL",              DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_charge},

    {DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL,     "UNDERTEMP_DISCHARGE_MSL",         DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_discharge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_RSL,     "UNDERTEMP_DISCHARGE_RSL",         DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_discharge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MOL,   "UNDERTEMP_DISCHARGE_MOL",           DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_discharge},

    {DIAG_CH_OVERCURRENT_PL_NONE,             "OVERCUR_NO_POWERLINE",      DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},

    {DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL,     "OVERCUR_CHRG_CELL_MSL",     DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL,     "OVERCUR_CHRG_CELL_RSL",     DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL,     "OVERCUR_CHRG_CELL_MOL",     DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},

    {DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL,  "OVERCUR_DCHRG_CELL_MSL",    DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL,  "OVERCUR_DCHRG_CELL_RSL",    DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL,  "OVERCUR_DCHRG_CELL_MOL",    DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},

    {DIAG_CH_OVERCURRENT_CHARGE_PL0_MSL,       "OVERCUR_CHRG_PL0_MSL",     DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_PL0_RSL,       "OVERCUR_CHRG_PL0_RSL",     DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_PL0_MOL,       "OVERCUR_CHRG_PL0_MOL",     DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},

    {DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MSL,    "OVERCUR_DCHRG_PL0_MSL",    DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_PL0_RSL,    "OVERCUR_DCHRG_PL0_RSL",    DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MOL,    "OVERCUR_DCHRG_PL0_MOL",    DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},

    {DIAG_CH_OVERCURRENT_CHARGE_PL1_MSL,       "OVERCUR_CHRG_PL1_MSL",     DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_PL1_RSL,       "OVERCUR_CHRG_PL1_RSL",     DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},
    {DIAG_CH_OVERCURRENT_CHARGE_PL1_MOL,       "OVERCUR_CHRG_PL1_MOL",     DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_charge},

    {DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MSL,    "OVERCUR_DCHRG_PL1_MSL",    DIAG_ERROR_CURRENT_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_PL1_RSL,    "OVERCUR_DCHRG_PL1_RSL",    DIAG_ERROR_CURRENT_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},
    {DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MOL,    "OVERCUR_DCHRG_PL1_MOL",    DIAG_ERROR_CURRENT_SENSITIVITY_MOL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overcurrent_discharge},

    {DIAG_CH_LTC_SPI,                                   "LTC_SPI",                              DIAG_ERROR_LTC_SPI_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_PEC,                                   "LTC_PEC",                              DIAG_ERROR_LTC_PEC_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_MUX,                                   "LTC_MUX",                              DIAG_ERROR_LTC_MUX_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_CONFIG,                                "LTC_CONFIG",                           DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},

    /* Communication events */
    {DIAG_CH_CAN_TIMING,                                "CAN_TIMING",                           DIAG_ERROR_CAN_TIMING_SENSITIVITY,        DIAG_RECORDING_ENABLED, DIAG_CAN_TIMING, DIAG_error_cantiming},
    {DIAG_CH_CAN_CC_RESPONDING,                         "CAN_CC_RESPONDING",                    DIAG_ERROR_CAN_TIMING_CC_SENSITIVITY,     DIAG_RECORDING_ENABLED, DIAG_CAN_SENSOR_PRESENT, DIAG_error_cantiming},
    {DIAG_CH_CURRENT_SENSOR_RESPONDING,                 "CURRENT_SENSOR_RESPONDING",            DIAG_ERROR_CAN_SENSOR_SENSITIVITY,        DIAG_RECORDING_ENABLED, DIAG_CAN_SENSOR_PRESENT, DIAG_error_cancurrentsensor},

#if BUILD_MODULE_ENABLE_CONTACTOR == 1
    /* Contactor Damage Error */
    {DIAG_CH_CONTACTOR_DAMAGED,                         "CONTACTOR_DAMAGED",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CONTACTOR_OPENING,                         "CONTACTOR_OPENING",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CONTACTOR_CLOSING,                         "CONTACTOR_CLOSING",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    /* Contactor Feedback Error */
    {DIAG_CH_CONTACTOR_MAIN_PLUS_FEEDBACK,              "CONT_MAIN_PLUS_FEED",          DIAG_ERROR_MAIN_PLUS_SENSITIVITY,      DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },
    {DIAG_CH_CONTACTOR_MAIN_MINUS_FEEDBACK,             "CONT_MAIN_MINUS_FEED",         DIAG_ERROR_MAIN_MINUS_SENSITIVITY,     DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },
    {DIAG_CH_CONTACTOR_PRECHARGE_FEEDBACK,              "CONT_PRECHARGE_FEED",          DIAG_ERROR_PRECHARGE_SENSITIVITY,      DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },
    {DIAG_CH_CONTACTOR_CHARGE_MAIN_PLUS_FEEDBACK,       "CONT_CHRGE_MAIN_PLUS_FEED",    DIAG_ERROR_MAIN_PLUS_SENSITIVITY,      DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },
    {DIAG_CH_CONTACTOR_CHARGE_MAIN_MINUS_FEEDBACK,      "CONT_CHRGE_MAIN_MINUS_FEED",   DIAG_ERROR_MAIN_MINUS_SENSITIVITY,     DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },
    {DIAG_CH_CONTACTOR_CHARGE_PRECHARGE_FEEDBACK,       "CONT_CHRGE_PRECHARGE_FEED",    DIAG_ERROR_PRECHARGE_SENSITIVITY,      DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_cont_feedback },

    /* Fuse state */
    {DIAG_CH_FUSE_STATE_NORMAL,                         "FUSE_STATE_NORMAL",          DIAG_ERROR_SENSITIVITY_LOW,            DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_fuseState },
    {DIAG_CH_FUSE_STATE_CHARGE,                         "FUSE_STATE_CHARGE",          DIAG_ERROR_SENSITIVITY_LOW,            DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_fuseState },
#else
    /* Contactor Damage Error */
    {DIAG_CH_CONTACTOR_DAMAGED,                         "CONTACTOR_DAMAGED",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_CONTACTOR_OPENING,                         "CONTACTOR_OPENING",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_CONTACTOR_CLOSING,                         "CONTACTOR_CLOSING",                    DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},

    /* Contactor Feedback Error */
    {DIAG_CH_CONTACTOR_MAIN_PLUS_FEEDBACK,              "CONT_MAIN_PLUS_FEED",          DIAG_ERROR_MAIN_PLUS_SENSITIVITY,      DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},
    {DIAG_CH_CONTACTOR_MAIN_MINUS_FEEDBACK,             "CONT_MAIN_MINUS_FEED",         DIAG_ERROR_MAIN_MINUS_SENSITIVITY,     DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},
    {DIAG_CH_CONTACTOR_PRECHARGE_FEEDBACK,              "CONT_PRECHARGE_FEED",          DIAG_ERROR_PRECHARGE_SENSITIVITY,      DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},
    {DIAG_CH_CONTACTOR_CHARGE_MAIN_PLUS_FEEDBACK,       "CONT_CHRGE_MAIN_PLUS_FEED",   DIAG_ERROR_MAIN_PLUS_SENSITIVITY,      DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},
    {DIAG_CH_CONTACTOR_CHARGE_MAIN_MINUS_FEEDBACK,      "CONT_CHRGE_MAIN_MINUS_FEED",  DIAG_ERROR_MAIN_MINUS_SENSITIVITY,     DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},
    {DIAG_CH_CONTACTOR_CHARGE_PRECHARGE_FEEDBACK,       "CONT_CHRGE_PRECHARGE_FEED",   DIAG_ERROR_PRECHARGE_SENSITIVITY,      DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_cont_feedback},

    /* Fuse state */
    {DIAG_CH_FUSE_STATE_NORMAL,                         "FUSE_STATE_NORMAL",        DIAG_ERROR_SENSITIVITY_LOW,            DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_error_fuseState},
    {DIAG_CH_FUSE_STATE_CHARGE,                         "FUSE_STATE_CHARGE",        DIAG_ERROR_SENSITIVITY_LOW,            DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_error_fuseState},

#endif

#if BUILD_MODULE_ENABLE_ILCK == 1
    /* Interlock Feedback Error */
    {DIAG_CH_INTERLOCK_FEEDBACK,                        "INTERLOCK_FEEDBACK",                    DIAG_ERROR_INTERLOCK_SENSITIVITY,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_interlock},
#else
    {DIAG_CH_INTERLOCK_FEEDBACK,                        "INTERLOCK_FEEDBACK",                    DIAG_ERROR_INTERLOCK_SENSITIVITY,       DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_error_interlock},
#endif

    /* Slave PCB temperature errors for under and over temperature */
    {DIAG_CH_SLAVE_PCB_UNDERTEMPERATURE_MSL,          "SLAVE_PCB_UNDERTEMP_MSL",        DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SLAVE_PCB_UNDERTEMPERATURE_RSL,          "SLAVE_PCB_UNDERTEMP_RSL",        DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SLAVE_PCB_UNDERTEMPERATURE_MOL,        "SLAVE_PCB_UNDERTEMP_MOL",        DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MOL, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_SLAVE_PCB_OVERTEMPERATURE_MSL,           "SLAVE_PCB_OVERTEMP_MSL",         DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SLAVE_PCB_OVERTEMPERATURE_RSL,           "SLAVE_PCB_OVERTEMP_RSL",         DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_RSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SLAVE_PCB_OVERTEMPERATURE_MOL,         "SLAVE_PCB_OVERTEMP_MOL",         DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MOL, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_ERROR_MCU_DIE_TEMPERATURE,     "MCU_DIE_TEMPERATURE",      DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_MCUdieTemperature},
    {DIAG_CH_LOW_COIN_CELL_VOLTAGE,         "COIN_CELL_VOLT_LOW",       DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_coinCellVoltage},
    {DIAG_CH_CRIT_LOW_COIN_CELL_VOLTAGE,    "COIN_CELL_VOLT_CRITICAL",  DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_coinCellVoltage},

    {DIAG_CH_OPEN_WIRE,       "OPEN_WIRE",         DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_openWire},
    {DIAG_CH_DEEP_DISCHARGE_DETECTED,    "DEEP-DISCHARGE detected", DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_deep_discharge_detected},

    /* Plausibility checks */
    {DIAG_CH_PLAUSIBILITY_CELL_VOLTAGE,    "PL_CELL_VOLT",    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_plausibility_check},
    {DIAG_CH_PLAUSIBILITY_CELL_TEMP,       "PL_CELL_TEMP",    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_plausibility_check},
    {DIAG_CH_PLAUSIBILITY_PACK_VOLTAGE,    "PL_PACK_VOLT",    DIAG_ERROR_PLAUSIBILITY_PACK_SENSITIVITY, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_plausibility_check},
};


DIAG_SYSMON_CH_CFG_s diag_sysmon_ch_cfg[] = {
    {DIAG_SYSMON_DATABASE_ID,       DIAG_SYSMON_CYCLICTASK,  10, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_SYS_ID,            DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_BMS_ID,            DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},

#if BUILD_MODULE_ENABLE_CONTACTOR == 1
    {DIAG_SYSMON_CONT_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
#else
    {DIAG_SYSMON_CONT_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_DISABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_DISABLED, dummyfu2},
#endif

#if BUILD_MODULE_ENABLE_ILCK == 1
    {DIAG_SYSMON_ILCK_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
#else
    {DIAG_SYSMON_ILCK_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_DISABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_DISABLED, dummyfu2},
#endif
    {DIAG_SYSMON_LTC_ID,            DIAG_SYSMON_CYCLICTASK,   5, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},

#if BUILD_MODULE_ENABLE_ISOGUARD == 1
    {DIAG_SYSMON_ISOGUARD_ID,       DIAG_SYSMON_CYCLICTASK, 400, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
#else
    {DIAG_SYSMON_ISOGUARD_ID,       DIAG_SYSMON_CYCLICTASK, 400, DIAG_RECORDING_DISABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_DISABLED, dummyfu2},
#endif

    {DIAG_SYSMON_CANS_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_1ms,   DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_10ms,  DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_100ms, DIAG_SYSMON_CYCLICTASK, 200, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
};


DIAG_DEV_s  diag_dev = {
    .nr_of_ch   = sizeof(diag_ch_cfg)/sizeof(DIAG_CH_CFG_s),
    .ch_cfg     = &diag_ch_cfg[0],
};

/*================== Static Function Implementations ========================*/
/**
 * @brief  dummy callback function of diagnosis events
 */
void dummyfu(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    /* Dummy function -> empty */
}

/**
 * @brief  dummy callback function of system monitoring error events
 */
void dummyfu2(DIAG_SYSMON_MODULE_ID_e ch_id) {
    /* Dummy function -> empty */
}

/**
 * @brief  diagnosis callback function for overvoltage events
 */
static void DIAG_overvoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.over_voltage = 1;
        }
    } else if (ch_id == DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_voltage = 1;
        }
    } else if (ch_id == DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.over_voltage = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for undervoltage events
 */
static void DIAG_undervoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.under_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.under_voltage = 1;
        }
    } else if (ch_id == DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.under_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.under_voltage = 1;
        }
    } else if (ch_id == DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.under_voltage = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.under_voltage = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for overtemperature charge events
 */
static void DIAG_overtemperature_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.over_temperature_charge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_temperature_charge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.over_temperature_charge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for overtemperature discharge events
 */
static void DIAG_overtemperature_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.over_temperature_discharge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_temperature_discharge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.over_temperature_discharge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for undertemperature charge events
 */
static void DIAG_undertemperature_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.under_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.under_temperature_charge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.under_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.under_temperature_charge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.under_temperature_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.under_temperature_charge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for undertemperature discharge events
 */
static void DIAG_undertemperature_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL) {
        if (event == DIAG_EVENT_RESET) {
            msl_flags.under_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            msl_flags.under_temperature_discharge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_RSL) {
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.under_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            rsl_flags.under_temperature_discharge = 1;
        }
    } else if (ch_id == DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MOL) {
        if (event == DIAG_EVENT_RESET) {
            mol_flags.under_temperature_discharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            mol_flags.under_temperature_discharge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for overcurrent charge events
 */
static void DIAG_overcurrent_charge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    switch (ch_id) {
    case DIAG_CH_OVERCURRENT_CHARGE_CELL_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_charge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_charge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_CELL_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_charge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_charge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_CELL_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_charge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_charge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL0_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_charge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_charge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL0_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_charge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_charge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL0_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_charge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_charge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL1_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_charge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_charge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL1_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_charge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_charge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_CHARGE_PL1_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_charge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_charge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_PL_NONE:
        if (event == DIAG_EVENT_RESET) {
            error_flags.currentOnOpenPowerline = 0;
        } else if (event == DIAG_EVENT_NOK) {
            error_flags.currentOnOpenPowerline = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    default:
        /* no relevant channel, do nothing */
        break;
    }
}

/**
 * @brief  diagnosis callback function for overcurrent discharge events
 */
static void DIAG_overcurrent_discharge(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    switch (ch_id) {
    case DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_discharge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_discharge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_CELL_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_discharge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_discharge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_CELL_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_discharge_cell = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_discharge_cell = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_discharge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_discharge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL0_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_discharge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_discharge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL0_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_discharge_pl0 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_discharge_pl0 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MSL:
        if (event == DIAG_EVENT_RESET) {
            msl_flags.over_current_discharge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            msl_flags.over_current_discharge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL1_RSL:
        if (event == DIAG_EVENT_RESET) {
            rsl_flags.over_current_discharge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            rsl_flags.over_current_discharge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    case DIAG_CH_OVERCURRENT_DISCHARGE_PL1_MOL:
        if (event == DIAG_EVENT_RESET) {
            mol_flags.over_current_discharge_pl1 = 0;
        } else if (event == DIAG_EVENT_NOK) {
            mol_flags.over_current_discharge_pl1 = 1;
        } else {
            /* no relevant event, do nothing */
        }
        break;
    default:
        /* no relevant channel, do nothing */
        break;
    }
}

/**
 * @brief  diagnosis callback function for can related events
 */
void DIAG_error_cantiming(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_CAN_TIMING) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.can_timing = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.can_timing = 1;
        }
    } else if (ch_id == DIAG_CH_CAN_CC_RESPONDING) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.can_timing_cc = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.can_timing_cc = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for current sensor related events
 */
void DIAG_error_cancurrentsensor(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_CURRENT_SENSOR_RESPONDING) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.currentsensorresponding = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.currentsensorresponding = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for LTC module related events
 */
static void DIAG_error_ltc(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_LTC_SPI) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.spi_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.spi_error = 1;
        }
    } else if (ch_id == DIAG_CH_LTC_PEC) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.crc_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.crc_error = 1;
        }
    } else if (ch_id == DIAG_CH_LTC_MUX) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.mux_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.mux_error = 1;
        }
    }  else if (ch_id == DIAG_CH_LTC_CONFIG) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.ltc_config_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.ltc_config_error = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for contactor feedback events
 */
void DIAG_cont_feedback(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_CONTACTOR_MAIN_PLUS_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.main_plus = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.main_plus = 1;
        }
    } else if (ch_id == DIAG_CH_CONTACTOR_MAIN_MINUS_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.main_minus = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.main_minus = 1;
        }
    } else if (ch_id == DIAG_CH_CONTACTOR_PRECHARGE_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.precharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.precharge = 1;
        }
    } else if (ch_id == DIAG_CH_CONTACTOR_CHARGE_MAIN_PLUS_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.charge_main_plus = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.charge_main_plus = 1;
        }
    } else if (ch_id == DIAG_CH_CONTACTOR_CHARGE_MAIN_MINUS_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.charge_main_minus = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.charge_main_minus = 1;
        }
    } else if (ch_id == DIAG_CH_CONTACTOR_CHARGE_PRECHARGE_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.charge_precharge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.charge_precharge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for fuse related events
 */
void DIAG_error_fuseState(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_FUSE_STATE_NORMAL) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.fuse_state_normal = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.fuse_state_normal = 1;
        }
    } else if (ch_id == DIAG_CH_FUSE_STATE_CHARGE) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.fuse_state_charge = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.fuse_state_charge = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for interlock events
 */
void DIAG_error_interlock(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_INTERLOCK_FEEDBACK) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.interlock = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.interlock = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for insulation events
 */
void DIAG_error_insulation(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_INSULATION_ERROR) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.insulation_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.insulation_error = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for MCU die temperature events
 */
void DIAG_error_MCUdieTemperature(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_ERROR_MCU_DIE_TEMPERATURE) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.mcuDieTemperature = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.mcuDieTemperature = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for coin cell voltage events
 */
void DIAG_error_coinCellVoltage(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_LOW_COIN_CELL_VOLTAGE) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.coinCellVoltage &= 0xFE;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.coinCellVoltage |= 0x01;
        }
    } else if (ch_id == DIAG_CH_CRIT_LOW_COIN_CELL_VOLTAGE) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.coinCellVoltage &= 0xFD;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.coinCellVoltage |= 0x02;
        }
    }
}

/**
 * @brief  diagnosis callback function for plausibility events
 */
void DIAG_error_plausibility_check(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_PLAUSIBILITY_CELL_VOLTAGE) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.plausibilityCheck &= 0xFE;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.plausibilityCheck |= 0x01;
        }
    } else if (ch_id == DIAG_CH_PLAUSIBILITY_CELL_TEMP) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.plausibilityCheck &= 0xFD;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.plausibilityCheck |= 0x02;
        }
    } else if (ch_id == DIAG_CH_PLAUSIBILITY_PACK_VOLTAGE) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.plausibilityCheck &= 0xFB;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.plausibilityCheck |= 0x04;
        }
    }
}

/**
 * @brief  diagnosis callback function for open-wire events
 */
void DIAG_error_openWire(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_OPEN_WIRE) {
        if (event  ==  DIAG_EVENT_RESET) {
            error_flags.open_wire = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.open_wire = 1;
        }
    }
}

/**
 * @brief  diagnosis callback function for deep-discharge related events
 */
void DIAG_error_deep_discharge_detected(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event) {
    if (ch_id == DIAG_CH_DEEP_DISCHARGE_DETECTED) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.deepDischargeDetected = 0;
            RTC_DEEP_DISCHARGE_DETECTED = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.deepDischargeDetected = 1;
            RTC_DEEP_DISCHARGE_DETECTED = 1;
        }
    }
}

/*================== Extern Function Implementations ========================*/
void DIAG_updateFlags(void) {
    DB_WriteBlock(&error_flags, DATA_BLOCK_ID_ERRORSTATE);
    DB_WriteBlock(&msl_flags, DATA_BLOCK_ID_MSL);
    DB_WriteBlock(&rsl_flags, DATA_BLOCK_ID_RSL);
    DB_WriteBlock(&mol_flags, DATA_BLOCK_ID_MOL);
}
