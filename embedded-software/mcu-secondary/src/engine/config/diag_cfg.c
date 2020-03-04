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

/*================== Macros and Definitions =================================*/

/*================== Static Constant and Variable Definitions ===============*/
static DATA_BLOCK_ERRORSTATE_s error_flags = { 0 };
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

/* functions for system related events */
static void DIAG_error_ltc(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
static void DIAG_error_interlock(DIAG_CH_ID_e ch_id, DIAG_EVENT_e event);
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
#else
    {DIAG_CH_ISOMETER_TIM_ERROR,                        "ISOMETER_TIM_ERROR",                   DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_GROUNDERROR,                      "ISOMETER_GROUNDERROR",                 DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_ERROR,                            "ISOMETER_ERROR",                       DIAG_ERROR_SENSITIVITY_MID,               DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
    {DIAG_CH_ISOMETER_MEAS_INVALID,                     "ISOMETER_MEAS_INVALID",                DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_DISABLED, DIAG_DISABLED, dummyfu},
#endif

    /* Under and over temperature, voltage and current at cell level */
    {DIAG_CH_CELLVOLTAGE_OVERVOLTAGE_MSL,             "CELLVOLT_OVERVOLT_MSL",            DIAG_ERROR_VOLTAGE_SENSITIVITY_MSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overvoltage},
    {DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE_MSL,            "CELLVOLT_UNDERVOLT_MSL",           DIAG_ERROR_VOLTAGE_SENSITIVITY_MSL,          DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undervoltage},
    {DIAG_CH_TEMP_OVERTEMPERATURE_CHARGE_MSL,         "OVERTEMP_CHARGE_MSL",             DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_charge},
    {DIAG_CH_TEMP_OVERTEMPERATURE_DISCHARGE_MSL,      "OVERTEMP_DISCHARGE_MSL",          DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_overtemperature_discharge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_CHARGE_MSL,        "UNDERTEMP_CHARGE_MSL",            DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_charge},
    {DIAG_CH_TEMP_UNDERTEMPERATURE_DISCHARGE_MSL,     "UNDERTEMP_DISCHARGE_MSL",         DIAG_ERROR_TEMPERATURE_SENSITIVITY_MSL,       DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_undertemperature_discharge},

    {DIAG_CH_LTC_SPI,                                   "LTC_SPI",                              DIAG_ERROR_LTC_SPI_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_PEC,                                   "LTC_PEC",                              DIAG_ERROR_LTC_PEC_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_MUX,                                   "LTC_MUX",                              DIAG_ERROR_LTC_MUX_SENSITIVITY,           DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},
    {DIAG_CH_LTC_CONFIG,                                "LTC_CONFIG",                           DIAG_ERROR_SENSITIVITY_HIGH,              DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_ltc},

#if BUILD_MODULE_ENABLE_ILCK == 1
    /* Interlock Feedback Error */
    {DIAG_CH_INTERLOCK_FEEDBACK,                        "INTERLOCK_FEEDBACK",                    DIAG_ERROR_INTERLOCK_SENSITIVITY,       DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_error_interlock},
#else
    {DIAG_CH_INTERLOCK_FEEDBACK,                        "INTERLOCK_FEEDBACK",                    DIAG_ERROR_INTERLOCK_SENSITIVITY,       DIAG_RECORDING_DISABLED, DIAG_DISABLED, DIAG_error_interlock},
#endif

    /* Slave PCB temperature errors for under and over temperature */
    {DIAG_CH_SLAVE_PCB_UNDERTEMPERATURE_MSL,          "SLAVE_PCB_UNDERTEMP_MSL",        DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_SLAVE_PCB_OVERTEMPERATURE_MSL,           "SLAVE_PCB_OVERTEMP_MSL",         DIAG_ERROR_SLAVE_TEMP_SENSITIVITY_MSL,   DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ERROR_MCU_DIE_TEMPERATURE,     "MCU_DIE_TEMPERATURE",      DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_MCUdieTemperature},
    {DIAG_CH_LOW_COIN_CELL_VOLTAGE,         "COIN_CELL_VOLT_LOW",       DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_coinCellVoltage},
    {DIAG_CH_CRIT_LOW_COIN_CELL_VOLTAGE,    "COIN_CELL_VOLT_CRITICAL",  DIAG_ERROR_SENSITIVITY_LOW, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_coinCellVoltage},

    /* Plausibility checks */
    {DIAG_CH_PLAUSIBILITY_CELL_VOLTAGE,    "PL_CELL_VOLT",    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_plausibility_check},
    {DIAG_CH_PLAUSIBILITY_CELL_TEMP,       "PL_CELL_TEMP",    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, DIAG_error_plausibility_check},
};


DIAG_SYSMON_CH_CFG_s diag_sysmon_ch_cfg[] = {
    {DIAG_SYSMON_DATABASE_ID,       DIAG_SYSMON_CYCLICTASK,  10, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_SYS_ID,        DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_BMS_ID,        DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
#if BUILD_MODULE_ENABLE_ILCK == 1
    {DIAG_SYSMON_ILCK_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
#else
    {DIAG_SYSMON_ILCK_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_DISABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_DISABLED, dummyfu2},
#endif
    {DIAG_SYSMON_LTC_ID,            DIAG_SYSMON_CYCLICTASK,   5, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_1ms,   DIAG_SYSMON_CYCLICTASK,   2, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
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
    } else if (ch_id == DIAG_CH_LTC_CONFIG) {
        if (event == DIAG_EVENT_RESET) {
            error_flags.ltc_config_error = 0;
        }
        if (event == DIAG_EVENT_NOK) {
            error_flags.ltc_config_error = 1;
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
    }
}
/*================== Extern Function Implementations ========================*/
void DIAG_updateFlags(void) {
    DB_WriteBlock(&error_flags, DATA_BLOCK_ID_ERRORSTATE);
    DB_WriteBlock(&msl_flags, DATA_BLOCK_ID_MSL);
}
