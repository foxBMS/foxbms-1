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
 * @file    batterysystem_cfg.h
 * @author  foxBMS Team
 * @date    20.02.2017 (date of creation)
 * @ingroup BATTER_SYSTEM_CONF
 * @prefix  BS
 *
 * @brief   Configuration of the battery system (e.g., number of battery modules, battery cells, temperature sensors)
 *
 * This files contains basic macros of the battery system in order to derive needed inputs
 * in other parts of the software. These macros are all depended on the hardware.
 *
 */

#ifndef BATTERYSYSTEM_CFG_H_
#define BATTERYSYSTEM_CFG_H_

/*================== Includes =============================================*/
#include "general.h"

/*================== Macros and Definitions ===============================*/

/**
 * Define if discharge current are positive negative, default is positive
 */
#define POSITIVE_DISCHARGE_CURRENT          TRUE

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of modules in battery pack
 * \par Type:
 * int
 * \par Default:
 * 2
*/
#define BS_NR_OF_MODULES                           1

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of battery cells per battery module (parallel cells are counted as one)
 * \par Type:
 * int
 * \par Default:
 * 12
*/
#define BS_NR_OF_BAT_CELLS_PER_MODULE               12

#if BS_NR_OF_BAT_CELLS_PER_MODULE <= 12
    #define BS_MAX_SUPPORTED_CELLS         12
#elif BS_NR_OF_BAT_CELLS_PER_MODULE <= 15
    #define BS_MAX_SUPPORTED_CELLS         15
#elif BS_NR_OF_BAT_CELLS_PER_MODULE <= 18
    #define BS_MAX_SUPPORTED_CELLS         18
#else
    #error "Unsupported number of cells per module, higher than 18"
#endif

#define BS_BALANCING_RESISTANCE_OHM             100.0
/* Number of GPIOs on the LTC IC
 * 5 for 12 cell version
 * 9 for 18 cell version
 */
#if BS_MAX_SUPPORTED_CELLS == 12
    #define BS_NR_OF_GPIOS_PER_MODULE               5
#else
    #define BS_NR_OF_GPIOS_PER_MODULE               9
#endif

/**
 * Number of user multiplexer used per LTC-IC
 * The other type is temperature multiplexer
 */
#define BS_N_USER_MUX_PER_LTC               2

/**
 * Number of channels per multiplexer
 */
#define BS_N_MUX_CHANNELS_PER_MUX      8

/*
 * specify the type of LTC connected to the battery module
 * @var LTC 6804 used
 * @type select(2)
 * @default  0
 * @level    user
 * @group    GENERAL
 */
/* #define BS_LTC6804_USED      TRUE */
#define BS_LTC6804_USED      FALSE

/*
 * specify the type of LTC connected to the battery module
 * @var LTC 6810 used
 * @type select(2)
 * @default  0
 * @level    user
 * @group    GENERAL
 */
/* #define BS_LTC6810_USED      TRUE */
#define BS_LTC6810_USED      FALSE

/*
 * specify the type of LTC connected to the battery module
 * @var LTC 6811 used
 * @type select(2)
 * @default  1
 * @level    user
 * @group    GENERAL
 */
#define BS_LTC6811_USED      TRUE
/* #define BS_LTC6811_USED      FALSE */

/*
 * specify the type of LTC connected to the battery module
 * @var LTC 6812 used
 * @type select(2)
 * @default  0
 * @level    user
 * @group    GENERAL
 */
/* #define BS_LTC6812_USED      TRUE */
#define BS_LTC6812_USED      FALSE

/*
 * specify the type of LTC connected to the battery module
 * @var LTC 6813 used
 * @type select(2)
 * @default  0
 * @level    user
 * @group    GENERAL
 */
/* #define BS_LTC6813_USED      TRUE */
#define BS_LTC6813_USED      FALSE

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of temperature sensors per battery module
 * \par Type:
 * int
 * \par Default:
 * 6
*/
#define BS_NR_OF_TEMP_SENSORS_PER_MODULE            8


#define BS_NR_OF_BAT_CELLS                         (BS_NR_OF_MODULES * BS_NR_OF_BAT_CELLS_PER_MODULE)
#define BS_NR_OF_TEMP_SENSORS                      (BS_NR_OF_MODULES * BS_NR_OF_TEMP_SENSORS_PER_MODULE)


/**
 * If set to FALSE, foxBMS does not check for the presence of a current sensor.
 * If set to TRUE, foxBMS checks for the presence of a current sensor. If sensor stops responding during runtime, an error is raised
 *
*/
#define CURRENT_SENSOR_PRESENT               TRUE

#if CURRENT_SENSOR_PRESENT == TRUE
/**
 * defines if the Isabellenhuette current sensor is used in cyclic or triggered mode
*/
#define CURRENT_SENSOR_ISABELLENHUETTE_CYCLIC
/* #define CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED */

/**
 * Delay in ms after which it is considered the current sensor is not responding anymore.
 */
#define CURRENT_SENSOR_RESPONSE_TIMEOUT_MS         100

#endif /* CURRENT_SENSOR_PRESENT == TRUE */

/**
 * @brief Maximum safety limit of current on powerline 0 in charge
 * direction in mA. When maximum safety limit (MSL) is violated, error state
 * is requested and contactors will open. When recommended safety limit (RSL)
 * or maximum operating limit (MOL) is violated, the respective flag will be
 * set.
 */
#define BS_CURRENTMAX_CHARGE_PL0_MSL_mA        (180000u)
#define BS_CURRENTMAX_CHARGE_PL0_RSL_mA        (170000u)
#define BS_CURRENTMAX_CHARGE_PL0_MOL_mA        (160000u)

/**
 * @brief Maximum operation limit of current on the powerline 0 in
 * discharge direction in mA. When maximum safety limit (MSL) is violated, error state
 * is requested and contactors will open. When recommended safety limit (RSL)
 * or maximum operating limit (MOL) is violated, the respective flag will be
 * set.
 */
#define BS_CURRENTMAX_DISCHARGE_PL0_MSL_mA     (BS_CURRENTMAX_CHARGE_PL0_MSL_mA)
#define BS_CURRENTMAX_DISCHARGE_PL0_RSL_mA     (BS_CURRENTMAX_CHARGE_PL0_RSL_mA)
#define BS_CURRENTMAX_DISCHARGE_PL0_MOL_mA     (BS_CURRENTMAX_CHARGE_PL0_MOL_mA)

/**
 * @brief Maximum operation limit of current of powerline 1 in
 * charge direction in mA. When maximum safety limit (MSL) is violated, error state
 * is requested and contactors will open. When recommended safety limit (RSL)
 * or maximum operating limit (MOL) is violated, the respective flag will be
 * set.
 */
#define BS_CURRENTMAX_CHARGE_PL1_MSL_mA        (180000u)
#define BS_CURRENTMAX_CHARGE_PL1_RSL_mA        (170000u)
#define BS_CURRENTMAX_CHARGE_PL1_MOL_mA        (160000u)

/**
 * @brief Maximum operation limit of current of powerline 1 in
 * discharge direction in mA. When maximum safety limit (MSL) is violated, error state
 * is requested and contactors will open. When recommended safety limit (RSL)
 * or maximum operating limit (MOL) is violated, the respective flag will be
 * set.
 */
#define BS_CURRENTMAX_DISCHARGE_PL1_MSL_mA     (BS_CURRENTMAX_CHARGE_PL1_MSL_mA)
#define BS_CURRENTMAX_DISCHARGE_PL1_RSL_mA     (BS_CURRENTMAX_CHARGE_PL1_RSL_mA)
#define BS_CURRENTMAX_DISCHARGE_PL1_MOL_mA     (BS_CURRENTMAX_CHARGE_PL1_MOL_mA)


/**
 * If set to FALSE, foxBMS does not check CAN timing.
 * If set to TRUE, foxBMS checks CAN timing. A valid request must come every 100ms, within the 95-150ms window.
 *
*/
#define CHECK_CAN_TIMING                     TRUE

/**
 * If set to TRUE, balancing is deactivated completely.
 * If set to FALSE, foxBMS checks when balancing must be done and activates it accordingly.
 *
*/
#define BALANCING_DEFAULT_INACTIVE           TRUE

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * checking if current is in SOF limits of cells.
 *
 * If set to TRUE the current is checked against the SOF limits defined in
 * SOX_cfg.h.
 *
 * If set to FALSE the current is checked against the constant values for
 * charging and discharging (defined in batterycell_cfg.h):
 * - BC_CURRENTMAX_DISCHARGE_MSL
 * - BC_CURRENTMAX_DISCHARGE_RSL
 * - BC_CURRENTMAX_DISCHARGE_MOL
 *
 * - BC_CURRENTMAX_CHARGE_MSL
 * - BC_CURRENTMAX_CHARGE_RSL
 * - BC_CURRENTMAX_CHARGE_MOL
 * \par Type:
 * select(2)
 * \par Default:
 * 1
*/
#define BMS_TEST_CELL_SOF_LIMITS                TRUE


/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * Defines behaviour if an insulation error is detected
 *
 * If set to TRUE: contactors will be opened
 * If set to FALSE: contactors will NOT be opened
 */
#define BMS_OPEN_CONTACTORS_ON_INSULATION_ERROR     FALSE

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of pack voltage inputs measured by current sensors (like IVT-MOD)
 * \par Type:
 * int
 * \par Default:
 * 3
*/
#define BS_NR_OF_VOLTAGES_FROM_CURRENT_SENSOR      3

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of voltages measured by MCU internal ADC
 * \par Type:
 * int
 * \par Default:
 * 3
*/
#define BS_NR_OF_VOLTAGES_FROM_MCU_ADC      2

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * number of contactors
 * On the foxBMS Basic Board 6 contactors are supported. On the foxBMS
 * Basic Extension board one can use 3 more contactors.
 * \par Type:
 * int
 * \par Default:
 * 3
*/
#define BS_NR_OF_CONTACTORS                        6

/**
 * @ingroup CONFIG_BATTERYSYSTEM
 * separation of charge and discharge power line
 * \par Type:
 * select(2)
 * \par Default:
 * 1
*/
/* #define BS_SEPARATE_POWERLINES 0 */
#define BS_SEPARATE_POWERLINES 1

#if BS_NR_OF_CONTACTORS > 3 && BS_SEPARATE_POWERLINES == 0
#error "Configuration mismatch: Can't use only one powerline with more than 3 contactors"
#endif  /*  */

#if BS_NR_OF_CONTACTORS < 4 && BS_SEPARATE_POWERLINES == 1
#error "Configuration mismatch: Can't use seperate powerlines with less than 4 contactors"
#endif  /*  */

/**
 * current threshold for determing rest state of battery. If absolute current is below
 * this limit value the battery is resting.
 */
#define BS_REST_CURRENT_mA                          (200)


/**
 * Wait time in milliseconds before battery system is at rest. Balancing
 * for example only starts if battery system is at rest.
 */
#define BS_RELAXATION_PERIOD_MS                   (600000u)


/**
 * current sensor threshold for 0 current in mA as the sensor has a jitter.
 */
#define BS_CS_THRESHOLD_NO_CURRENT_mA               (200u)

/**
 * maximum voltage drop over fuse. If the measured voltage difference
 * between battery voltage measured voltage after fuse is larger than this
 * value. It can be concluded, that the fuse has tripped. The voltage
 * difference can be estimated by the maximum current and the resistance.
 *
 * For a Cooper Bussmann 1000A fuse the voltage drop can be estimated to:
 * Imax =  1000A, Ploss = 206W: -> voltage drop at 1000A roughly 206mV
 * -> select 500mV because of measurement inaccuracies
 */
#define BS_MAX_VOLTAGE_DROP_OVER_FUSE_mV            500

/**
 * Set to TRUE if fuse in NORMAL path should be checked. This can only be done
 * if one dedicated HV measurement is used to monitor the voltage directly after
 * the fuse. Then a voltage difference between Vbat and Vfuse indicates a tripped
 * fuse.
 *
 *    Vbat   ------      Vfuse       Precharge/Main+ contactor
 * -----+---| FUSE |-----+------------/   -----------------
 *           ------
 */
#define BS_CHECK_FUSE_PLACED_IN_NORMAL_PATH          TRUE

/**
 * Set to TRUE if fuse in CHARGE path should be checked. This can only be done
 * if one dedicated HV measurement is used to monitor the voltage directly after
 * the fuse. Then a voltage difference between Vbat and Vfuse indicates a tripped
 * fuse.
 *
 *    Vbat   ------      Vfuse       Precharge/Charge+ contactor
 * -----+---| FUSE |-----+------------/   -----------------
 *           ------
 */
#define BS_CHECK_FUSE_PLACED_IN_CHARGE_PATH          FALSE

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* BATTERYSYSTEM_CFG_H_ */
