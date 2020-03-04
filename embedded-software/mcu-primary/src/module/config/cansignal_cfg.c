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
 * @file    cansignal_cfg.c
 * @author  foxBMS Team
 * @date    16.09.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  CANS
 *
 * @brief   Configuration of the messages and signal settings for the CAN driver
 *
 */

/*================== Includes ===============================================*/
#include "cansignal_cfg.h"

#include "bal.h"
#include "database.h"
#include "diag.h"
#include "sox.h"
#include "sys.h"

/*================== Macros and Definitions =================================*/
static DATA_BLOCK_CURRENT_SENSOR_s cans_current_tab;

#define CANS_MODULSIGNALS_VOLT      (CAN0_SIG_Mod0_temp_valid_0_2 - CAN0_SIG_Mod0_volt_valid_0_2)
#define CANS_MODULSIGNALS_TEMP      (CAN0_SIG_Mod1_volt_valid_0_2 - CAN0_SIG_Mod0_temp_valid_0_2)


/*================== Static Function Prototypes =============================*/

static float cans_checkLimits(float value, uint32_t sigIdx);

/* TX/Getter functions */
static uint32_t cans_getvolt(uint32_t, void *);
static uint32_t cans_gettempering(uint32_t, void *);
static uint32_t cans_getcanerr(uint32_t, void *);
static uint32_t cans_gettemp(uint32_t, void *);
static uint32_t cans_getsoc(uint32_t, void *);
static uint32_t cans_getRecommendedOperatingCurrent(uint32_t, void *);
static uint32_t cans_getMaxAllowedPower(uint32_t, void *);
static uint32_t cans_getpower(uint32_t, void *);
static uint32_t cans_getcurr(uint32_t, void *);
static uint32_t cans_getPackVoltage(uint32_t, void *);
static uint32_t cans_getminmaxvolt(uint32_t, void *);
static uint32_t cans_getminmaxtemp(uint32_t, void *);
static uint32_t cans_getisoguard(uint32_t, void *);


/* RX/Setter functions */
static uint32_t cans_setcurr(uint32_t, void *);
static uint32_t cans_setstaterequest(uint32_t, void *);
static uint32_t cans_setdebug(uint32_t, void *);
static uint32_t cans_setSWversion(uint32_t, void *);


#ifdef CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED
static uint32_t cans_gettriggercurrent(uint32_t sigIdx, void *value);
#endif /* CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED */



/*================== Static Constant and Variable Definitions ===============*/

/*================== Extern Constant and Variable Definitions ===============*/

const CANS_signal_s cans_CAN0_signals_tx[] = {
    { {CAN0_MSG_SystemState_0}, 0, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_general_error, */
    { {CAN0_MSG_SystemState_0}, 8, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_current_state, */
    { {CAN0_MSG_SystemState_0}, 16, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_overtemp_charge, */
    { {CAN0_MSG_SystemState_0}, 24, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_undertemp_charge, */
    { {CAN0_MSG_SystemState_0}, 32, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_overtemp_discharge, */
    { {CAN0_MSG_SystemState_0}, 40, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_undertemp_discharge, */
    { {CAN0_MSG_SystemState_0}, 48, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_overcurrent_charge, */
    { {CAN0_MSG_SystemState_0}, 56, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS0_error_overcurrent_discharge, */

    { {CAN0_MSG_SystemState_1}, 0, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_overvoltage, */
    { {CAN0_MSG_SystemState_1}, 8, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_undervoltage, */
    { {CAN0_MSG_SystemState_1}, 11, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_deep_discharge, */
    { {CAN0_MSG_SystemState_1}, 16, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_temperature_MCU0 */
    { {CAN0_MSG_SystemState_1}, 24, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_contactor */
    { {CAN0_MSG_SystemState_1}, 32, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_error_selftest, */
    { {CAN0_MSG_SystemState_1}, 40, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN_SIG_GS1_error_cantiming, */
    { {CAN0_MSG_SystemState_1}, 48, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_current_sensor, */
    { {CAN0_MSG_SystemState_1}, 56, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS1_balancing_active, */

    { {CAN0_MSG_SystemState_2}, 0, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS2_states_relays */
    { {CAN0_MSG_SystemState_2}, 16, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },   /*!< CAN0_SIG_GS2_error_insulation */
    { {CAN0_MSG_SystemState_2}, 24, 4, 0, 15, 1, 0, littleEndian, &cans_getcanerr },  /*!< CAN0_SIG_GS2_fuse_state */
    { {CAN0_MSG_SystemState_2}, 32, 2, 0, 3, 1, 0, littleEndian, &cans_getcanerr },   /*!< CAN0_SIG_GS2_lowCoinCellVolt */
    { {CAN0_MSG_SystemState_2}, 40, 1, 0, 1, 1, 0, littleEndian, &cans_getcanerr },   /*!< CAN0_SIG_GS2_error_openWire */
    { {CAN0_MSG_SystemState_2}, 48, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },   /*!< CAN0_SIG_GS2_daisyChain */
    { {CAN0_MSG_SystemState_2}, 56, 3, 0, 7, 1, 0, littleEndian, &cans_getcanerr },   /*!< CAN0_SIG_GS2_plausibilityCheck */

    { {CAN0_MSG_SlaveState_0}, 0, 64, 0, UINT64_MAX, 1, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SS0_states */
    { {CAN0_MSG_SlaveState_1}, 0, 64, 0, UINT64_MAX, 1, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SS0_states */

    { {CAN0_MSG_RecOperatingCurrent}, 0, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getRecommendedOperatingCurrent },  /*!< CAN0_SIG_MaxChargeCurrent */
    { {CAN0_MSG_RecOperatingCurrent}, 16, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getRecommendedOperatingCurrent },  /*!< CAN0_SIG_MaxChargeCurrent_Peak */
    { {CAN0_MSG_RecOperatingCurrent}, 32, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getRecommendedOperatingCurrent },  /*!< CAN0_SIG_MaxDischargeCurrent */
    { {CAN0_MSG_RecOperatingCurrent}, 48, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getRecommendedOperatingCurrent },  /*!< CAN0_SIG_MaxDischargeCurrent_Peak */

    { {CAN0_MSG_SOP}, 0, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getMaxAllowedPower },  /*!< CAN0_SIG_MaxChargePower */
    { {CAN0_MSG_SOP}, 16, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getMaxAllowedPower },  /*!< CAN0_SIG_MaxChargePower_Peak */
    { {CAN0_MSG_SOP}, 32, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getMaxAllowedPower },  /*!< CAN0_SIG_MaxDischargePower */
    { {CAN0_MSG_SOP}, 48, 16, 0, 6553.5, 10, 0, littleEndian, &cans_getMaxAllowedPower },  /*!< CAN0_SIG_MaxDischargePower_Peak */

    { {CAN0_MSG_SOC}, 0, 16, 0, 100, 100, 0, littleEndian, &cans_getsoc },  /*!< CAN0_SIG_SOC_mean */
    { {CAN0_MSG_SOC}, 16, 16, 0, 100, 100, 0, littleEndian, &cans_getsoc },  /*!< CAN0_SIG_SOC_min */
    { {CAN0_MSG_SOC}, 32, 16, 0, 100, 100, 0, littleEndian, &cans_getsoc },  /*!< CAN0_SIG_SOC_max */

    { {CAN0_MSG_SOH}, 0, 16, 0, 0, 100, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SOH_mean */
    { {CAN0_MSG_SOH}, 16, 16, 0, 0, 100, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SOH_min */
    { {CAN0_MSG_SOH}, 32, 16, 0, 0, 100, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SOH_max */

    { {CAN0_MSG_SOE}, 0, 16, 0, 0, 100, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SOE */
    { {CAN0_MSG_SOE}, 16, 32, 0, UINT32_MAX, 1, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_RemainingEnergy */

    { {CAN0_MSG_MinMaxCellVolt}, 0, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getminmaxvolt },  /*!< CAN0_SIG_Cellvolt_mean */
    { {CAN0_MSG_MinMaxCellVolt}, 16, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getminmaxvolt },  /*!< CAN0_SIG_Cellvolt_min */
    { {CAN0_MSG_MinMaxCellVolt}, 32, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getminmaxvolt },  /*!< CAN0_SIG_Cellvolt_max */
    { {CAN0_MSG_MinMaxCellVolt}, 48, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getminmaxvolt },  /*!< CAN0_SIG_ModNumber_min */
    { {CAN0_MSG_MinMaxCellVolt}, 56, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getminmaxvolt },  /*!< CAN0_SIG_ModNumber_max */

    { {CAN0_MSG_SOV}, 0, 16, 0, 100, 100, 0, littleEndian, NULL_PTR },  /*!< CAN0_SIG_SOV */

    { {CAN0_MSG_MinMaxCellTemp}, 0, 16, -128, 527.35, 100, 128, littleEndian, &cans_getminmaxtemp },  /*!< CAN0_SIG_Cellvolt_mean */
    { {CAN0_MSG_MinMaxCellTemp}, 16, 16, -128, 527.35, 100, 128, littleEndian, &cans_getminmaxtemp },  /*!< CAN0_SIG_Cellvolt_min */
    { {CAN0_MSG_MinMaxCellTemp}, 32, 16, -128, 527.35, 100, 128, littleEndian, &cans_getminmaxtemp },  /*!< CAN0_SIG_Cellvolt_max */
    { {CAN0_MSG_MinMaxCellTemp}, 48, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getminmaxtemp },  /*!< CAN0_SIG_ModNumber_min */
    { {CAN0_MSG_MinMaxCellTemp}, 56, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getminmaxtemp },  /*!< CAN0_SIG_ModNumber_max */

    { {CAN0_MSG_Tempering}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettempering },  /*!< CAN0_SIG_CoolingNeeded */
    { {CAN0_MSG_Tempering}, 8, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettempering },  /*!< CAN0_SIG_HeatingNeeded */
    { {CAN0_MSG_Tempering}, 16, 32, 0, UINT32_MAX, 1, 0, littleEndian, &cans_gettempering },  /*!< CAN0_SIG_TemperingDemand */

    { {CAN0_MSG_Insulation}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getisoguard },  /*!< CAN0_SIG_InsulationStatus */
    { {CAN0_MSG_Insulation}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getisoguard },  /*!< CAN0_SIG_InsulationValue */

    { {CAN0_MSG_Power_0}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_1s */
    { {CAN0_MSG_Power_0}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_5s */
    { {CAN0_MSG_Power_1}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_10s */
    { {CAN0_MSG_Power_1}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_30s */
    { {CAN0_MSG_Power_2}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_60s */
    { {CAN0_MSG_Power_2}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getpower },  /*!< CAN0_SIG_RunAverage_Power_config */

    { {CAN0_MSG_Current_0}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_1s */
    { {CAN0_MSG_Current_0}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_5s */
    { {CAN0_MSG_Current_1}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_10s */
    { {CAN0_MSG_Current_1}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_30s */
    { {CAN0_MSG_Current_2}, 0, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_60s */
    { {CAN0_MSG_Current_2}, 32, 32, -2500000, 4292467295, 1, 2500000, littleEndian, &cans_getcurr },  /*!< CAN0_SIG_RunAverage_Current_config */

    { {CAN0_MSG_PackVoltage}, 0, 32, 0, UINT32_MAX, 1, 0, littleEndian, &cans_getPackVoltage },  /*!< CAN0_SIG_PackVolt_Battery */
    { {CAN0_MSG_PackVoltage}, 32, 32, 0, UINT32_MAX, 1, 0, littleEndian, &cans_getPackVoltage },  /*!< CAN0_SIG_PackVolt_PowerNet */

    /* Module 0 cell voltages */
    { {CAN0_MSG_Mod0_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_0_2 */
    { {CAN0_MSG_Mod0_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_0 */
    { {CAN0_MSG_Mod0_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_1 */
    { {CAN0_MSG_Mod0_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_2 */
    { {CAN0_MSG_Mod0_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_3_5 */
    { {CAN0_MSG_Mod0_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_3 */
    { {CAN0_MSG_Mod0_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_4 */
    { {CAN0_MSG_Mod0_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_5 */
    { {CAN0_MSG_Mod0_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_6_8 */
    { {CAN0_MSG_Mod0_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_6 */
    { {CAN0_MSG_Mod0_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_7 */
    { {CAN0_MSG_Mod0_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_8 */
    { {CAN0_MSG_Mod0_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_9_11 */
    { {CAN0_MSG_Mod0_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_9 */
    { {CAN0_MSG_Mod0_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_10 */
    { {CAN0_MSG_Mod0_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_11 */
    { {CAN0_MSG_Mod0_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_12_14 */
    { {CAN0_MSG_Mod0_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_12 */
    { {CAN0_MSG_Mod0_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_13 */
    { {CAN0_MSG_Mod0_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_14 */
    { {CAN0_MSG_Mod0_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_valid_15_17 */
    { {CAN0_MSG_Mod0_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_15 */
    { {CAN0_MSG_Mod0_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_16 */
    { {CAN0_MSG_Mod0_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod0_volt_17 */

    /* Module 0 cell temperatures */
    { {CAN0_MSG_Mod0_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_volt_valid_0_2 */
    { {CAN0_MSG_Mod0_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_0 */
    { {CAN0_MSG_Mod0_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_1 */
    { {CAN0_MSG_Mod0_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_2 */
    { {CAN0_MSG_Mod0_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_volt_valid_3_5 */
    { {CAN0_MSG_Mod0_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_3 */
    { {CAN0_MSG_Mod0_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_4 */
    { {CAN0_MSG_Mod0_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_5 */
    { {CAN0_MSG_Mod0_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_volt_valid_6_8 */
    { {CAN0_MSG_Mod0_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_6 */
    { {CAN0_MSG_Mod0_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_7 */
    { {CAN0_MSG_Mod0_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_8 */
    { {CAN0_MSG_Mod0_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_volt_valid_9_11 */
    { {CAN0_MSG_Mod0_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_9 */
    { {CAN0_MSG_Mod0_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_10 */
    { {CAN0_MSG_Mod0_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod0_temp_11 */

    /* Module 1 cell voltages */
    { {CAN0_MSG_Mod1_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_0_2 */
    { {CAN0_MSG_Mod1_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_0 */
    { {CAN0_MSG_Mod1_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_1 */
    { {CAN0_MSG_Mod1_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_2 */
    { {CAN0_MSG_Mod1_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_3_5 */
    { {CAN0_MSG_Mod1_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_3 */
    { {CAN0_MSG_Mod1_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_4 */
    { {CAN0_MSG_Mod1_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_5 */
    { {CAN0_MSG_Mod1_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_6_8 */
    { {CAN0_MSG_Mod1_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_6 */
    { {CAN0_MSG_Mod1_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_7 */
    { {CAN0_MSG_Mod1_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_8 */
    { {CAN0_MSG_Mod1_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_9_11 */
    { {CAN0_MSG_Mod1_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_9 */
    { {CAN0_MSG_Mod1_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_10 */
    { {CAN0_MSG_Mod1_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_11 */
    { {CAN0_MSG_Mod1_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_12_14 */
    { {CAN0_MSG_Mod1_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_12 */
    { {CAN0_MSG_Mod1_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_13 */
    { {CAN0_MSG_Mod1_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_14 */
    { {CAN0_MSG_Mod1_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_valid_15_17 */
    { {CAN0_MSG_Mod1_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_15 */
    { {CAN0_MSG_Mod1_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_16 */
    { {CAN0_MSG_Mod1_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod1_volt_17 */

    /* Module 1 cell temperatures */
    { {CAN0_MSG_Mod1_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_volt_valid_0_2 */
    { {CAN0_MSG_Mod1_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_0 */
    { {CAN0_MSG_Mod1_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_1 */
    { {CAN0_MSG_Mod1_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_2 */
    { {CAN0_MSG_Mod1_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_volt_valid_3_5 */
    { {CAN0_MSG_Mod1_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_3 */
    { {CAN0_MSG_Mod1_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_4 */
    { {CAN0_MSG_Mod1_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_5 */
    { {CAN0_MSG_Mod1_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_volt_valid_6_8 */
    { {CAN0_MSG_Mod1_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_6 */
    { {CAN0_MSG_Mod1_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_7 */
    { {CAN0_MSG_Mod1_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_8 */
    { {CAN0_MSG_Mod1_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_volt_valid_9_11 */
    { {CAN0_MSG_Mod1_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_9 */
    { {CAN0_MSG_Mod1_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_10 */
    { {CAN0_MSG_Mod1_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod1_temp_11 */

    /* Module 2 cell voltages */
    { {CAN0_MSG_Mod2_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_0_2 */
    { {CAN0_MSG_Mod2_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_0 */
    { {CAN0_MSG_Mod2_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_1 */
    { {CAN0_MSG_Mod2_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_2 */
    { {CAN0_MSG_Mod2_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_3_5 */
    { {CAN0_MSG_Mod2_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_3 */
    { {CAN0_MSG_Mod2_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_4 */
    { {CAN0_MSG_Mod2_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_5 */
    { {CAN0_MSG_Mod2_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_6_8 */
    { {CAN0_MSG_Mod2_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_6 */
    { {CAN0_MSG_Mod2_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_7 */
    { {CAN0_MSG_Mod2_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_8 */
    { {CAN0_MSG_Mod2_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_9_11 */
    { {CAN0_MSG_Mod2_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_9 */
    { {CAN0_MSG_Mod2_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_10 */
    { {CAN0_MSG_Mod2_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_11 */
    { {CAN0_MSG_Mod2_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_12_14 */
    { {CAN0_MSG_Mod2_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_12 */
    { {CAN0_MSG_Mod2_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_13 */
    { {CAN0_MSG_Mod2_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_14 */
    { {CAN0_MSG_Mod2_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_valid_15_17 */
    { {CAN0_MSG_Mod2_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_15 */
    { {CAN0_MSG_Mod2_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_16 */
    { {CAN0_MSG_Mod2_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod2_volt_17 */

    /* Module 2 cell temperatures */
    { {CAN0_MSG_Mod2_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_volt_valid_0_2 */
    { {CAN0_MSG_Mod2_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_0 */
    { {CAN0_MSG_Mod2_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_1 */
    { {CAN0_MSG_Mod2_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_2 */
    { {CAN0_MSG_Mod2_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_volt_valid_3_5 */
    { {CAN0_MSG_Mod2_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_3 */
    { {CAN0_MSG_Mod2_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_4 */
    { {CAN0_MSG_Mod2_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_5 */
    { {CAN0_MSG_Mod2_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_volt_valid_6_8 */
    { {CAN0_MSG_Mod2_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_6 */
    { {CAN0_MSG_Mod2_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_7 */
    { {CAN0_MSG_Mod2_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_8 */
    { {CAN0_MSG_Mod2_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_volt_valid_9_11 */
    { {CAN0_MSG_Mod2_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_9 */
    { {CAN0_MSG_Mod2_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_10 */
    { {CAN0_MSG_Mod2_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod2_temp_11 */

    /* Module 3 cell voltages */
    { {CAN0_MSG_Mod3_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_0_2 */
    { {CAN0_MSG_Mod3_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_0 */
    { {CAN0_MSG_Mod3_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_1 */
    { {CAN0_MSG_Mod3_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_2 */
    { {CAN0_MSG_Mod3_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_3_5 */
    { {CAN0_MSG_Mod3_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_3 */
    { {CAN0_MSG_Mod3_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_4 */
    { {CAN0_MSG_Mod3_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_5 */
    { {CAN0_MSG_Mod3_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_6_8 */
    { {CAN0_MSG_Mod3_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_6 */
    { {CAN0_MSG_Mod3_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_7 */
    { {CAN0_MSG_Mod3_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_8 */
    { {CAN0_MSG_Mod3_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_9_11 */
    { {CAN0_MSG_Mod3_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_9 */
    { {CAN0_MSG_Mod3_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_10 */
    { {CAN0_MSG_Mod3_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_11 */
    { {CAN0_MSG_Mod3_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_12_14 */
    { {CAN0_MSG_Mod3_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_12 */
    { {CAN0_MSG_Mod3_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_13 */
    { {CAN0_MSG_Mod3_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_14 */
    { {CAN0_MSG_Mod3_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_valid_15_17 */
    { {CAN0_MSG_Mod3_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_15 */
    { {CAN0_MSG_Mod3_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_16 */
    { {CAN0_MSG_Mod3_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod3_volt_17 */

    /* Module 3 cell temperatures */
    { {CAN0_MSG_Mod3_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_volt_valid_0_2 */
    { {CAN0_MSG_Mod3_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_0 */
    { {CAN0_MSG_Mod3_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_1 */
    { {CAN0_MSG_Mod3_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_2 */
    { {CAN0_MSG_Mod3_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_volt_valid_3_5 */
    { {CAN0_MSG_Mod3_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_3 */
    { {CAN0_MSG_Mod3_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_4 */
    { {CAN0_MSG_Mod3_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_5 */
    { {CAN0_MSG_Mod3_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_volt_valid_6_8 */
    { {CAN0_MSG_Mod3_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_6 */
    { {CAN0_MSG_Mod3_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_7 */
    { {CAN0_MSG_Mod3_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_8 */
    { {CAN0_MSG_Mod3_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_volt_valid_9_11 */
    { {CAN0_MSG_Mod3_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_9 */
    { {CAN0_MSG_Mod3_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_10 */
    { {CAN0_MSG_Mod3_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod3_temp_11 */

    /* Module 4 cell voltages */
    { {CAN0_MSG_Mod4_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_valid_0_2 */
    { {CAN0_MSG_Mod4_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_0 */
    { {CAN0_MSG_Mod4_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_1 */
    { {CAN0_MSG_Mod4_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_2 */
    { {CAN0_MSG_Mod4_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_valid_3_5 */
    { {CAN0_MSG_Mod4_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_3 */
    { {CAN0_MSG_Mod4_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_4 */
    { {CAN0_MSG_Mod4_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_5 */
    { {CAN0_MSG_Mod4_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_valid_6_8 */
    { {CAN0_MSG_Mod4_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_6 */
    { {CAN0_MSG_Mod4_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_7 */
    { {CAN0_MSG_Mod4_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_8 */
    { {CAN0_MSG_Mod4_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_valid_9_11 */
    { {CAN0_MSG_Mod4_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_9 */
    { {CAN0_MSG_Mod4_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_10 */
    { {CAN0_MSG_Mod4_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod4_volt_11 */
    { {CAN0_MSG_Mod4_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_12_14 */
    { {CAN0_MSG_Mod4_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_12 */
    { {CAN0_MSG_Mod4_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_13 */
    { {CAN0_MSG_Mod4_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_14 */
    { {CAN0_MSG_Mod4_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_15_17 */
    { {CAN0_MSG_Mod4_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_15 */
    { {CAN0_MSG_Mod4_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_16 */
    { {CAN0_MSG_Mod4_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_17 */

    /* Module 4 cell temperatures */
    { {CAN0_MSG_Mod4_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_0_2 */
    { {CAN0_MSG_Mod4_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_0 */
    { {CAN0_MSG_Mod4_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_1 */
    { {CAN0_MSG_Mod4_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_2 */
    { {CAN0_MSG_Mod4_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_3_5 */
    { {CAN0_MSG_Mod4_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_3 */
    { {CAN0_MSG_Mod4_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_4 */
    { {CAN0_MSG_Mod4_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_5 */
    { {CAN0_MSG_Mod4_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_6_8 */
    { {CAN0_MSG_Mod4_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_6 */
    { {CAN0_MSG_Mod4_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_7 */
    { {CAN0_MSG_Mod4_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_8 */
    { {CAN0_MSG_Mod4_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_9_11 */
    { {CAN0_MSG_Mod4_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_9 */
    { {CAN0_MSG_Mod4_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_10 */
    { {CAN0_MSG_Mod4_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_11 */

    /* Module 5 cell voltages */
    { {CAN0_MSG_Mod5_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_0_2 */
    { {CAN0_MSG_Mod5_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_0 */
    { {CAN0_MSG_Mod5_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_1 */
    { {CAN0_MSG_Mod5_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_2 */
    { {CAN0_MSG_Mod5_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_3_5 */
    { {CAN0_MSG_Mod5_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_3 */
    { {CAN0_MSG_Mod5_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_4 */
    { {CAN0_MSG_Mod5_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_5 */
    { {CAN0_MSG_Mod5_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_6_8 */
    { {CAN0_MSG_Mod5_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_6 */
    { {CAN0_MSG_Mod5_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_7 */
    { {CAN0_MSG_Mod5_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_8 */
    { {CAN0_MSG_Mod5_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_9_11 */
    { {CAN0_MSG_Mod5_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_9 */
    { {CAN0_MSG_Mod5_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_10 */
    { {CAN0_MSG_Mod5_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_11 */
    { {CAN0_MSG_Mod5_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_12_14 */
    { {CAN0_MSG_Mod5_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_12 */
    { {CAN0_MSG_Mod5_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_13 */
    { {CAN0_MSG_Mod5_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_14 */
    { {CAN0_MSG_Mod5_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_valid_15_17 */
    { {CAN0_MSG_Mod5_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_15 */
    { {CAN0_MSG_Mod5_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_16 */
    { {CAN0_MSG_Mod5_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod5_volt_17 */

    /* Module 5 cell temperatures */
    { {CAN0_MSG_Mod5_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_volt_valid_0_2 */
    { {CAN0_MSG_Mod5_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_0 */
    { {CAN0_MSG_Mod5_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_1 */
    { {CAN0_MSG_Mod5_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_2 */
    { {CAN0_MSG_Mod5_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_volt_valid_3_5 */
    { {CAN0_MSG_Mod5_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_3 */
    { {CAN0_MSG_Mod5_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_4 */
    { {CAN0_MSG_Mod5_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_5 */
    { {CAN0_MSG_Mod5_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_volt_valid_6_8 */
    { {CAN0_MSG_Mod5_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_6 */
    { {CAN0_MSG_Mod5_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_7 */
    { {CAN0_MSG_Mod5_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_8 */
    { {CAN0_MSG_Mod5_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_volt_valid_9_11 */
    { {CAN0_MSG_Mod5_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_9 */
    { {CAN0_MSG_Mod5_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_10 */
    { {CAN0_MSG_Mod5_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod5_temp_11 */

    /* Module 6 cell voltages */
    { {CAN0_MSG_Mod6_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_0_2 */
    { {CAN0_MSG_Mod6_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_0 */
    { {CAN0_MSG_Mod6_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_1 */
    { {CAN0_MSG_Mod6_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_2 */
    { {CAN0_MSG_Mod6_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_3_5 */
    { {CAN0_MSG_Mod6_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_3 */
    { {CAN0_MSG_Mod6_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_4 */
    { {CAN0_MSG_Mod6_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_5 */
    { {CAN0_MSG_Mod6_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_6_8 */
    { {CAN0_MSG_Mod6_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_6 */
    { {CAN0_MSG_Mod6_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_7 */
    { {CAN0_MSG_Mod6_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_8 */
    { {CAN0_MSG_Mod6_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_9_11 */
    { {CAN0_MSG_Mod6_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_9 */
    { {CAN0_MSG_Mod6_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_10 */
    { {CAN0_MSG_Mod6_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_11 */
    { {CAN0_MSG_Mod6_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_12_14 */
    { {CAN0_MSG_Mod6_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_12 */
    { {CAN0_MSG_Mod6_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_13 */
    { {CAN0_MSG_Mod6_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_14 */
    { {CAN0_MSG_Mod6_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_valid_15_17 */
    { {CAN0_MSG_Mod6_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_15 */
    { {CAN0_MSG_Mod6_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_16 */
    { {CAN0_MSG_Mod6_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod6_volt_17 */

    /* Module 6 cell temperatures */
    { {CAN0_MSG_Mod6_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_volt_valid_0_2 */
    { {CAN0_MSG_Mod6_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_0 */
    { {CAN0_MSG_Mod6_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_1 */
    { {CAN0_MSG_Mod6_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_2 */
    { {CAN0_MSG_Mod6_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_volt_valid_3_5 */
    { {CAN0_MSG_Mod6_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_3 */
    { {CAN0_MSG_Mod6_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_4 */
    { {CAN0_MSG_Mod6_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_5 */
    { {CAN0_MSG_Mod6_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_volt_valid_6_8 */
    { {CAN0_MSG_Mod6_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_6 */
    { {CAN0_MSG_Mod6_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_7 */
    { {CAN0_MSG_Mod6_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_8 */
    { {CAN0_MSG_Mod6_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_volt_valid_9_11 */
    { {CAN0_MSG_Mod6_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_9 */
    { {CAN0_MSG_Mod6_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_10 */
    { {CAN0_MSG_Mod6_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod6_temp_11 */

    /* Module 7 cell voltages */
    { {CAN0_MSG_Mod7_Cellvolt_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_0_2 */
    { {CAN0_MSG_Mod7_Cellvolt_0}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_0 */
    { {CAN0_MSG_Mod7_Cellvolt_0}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_1 */
    { {CAN0_MSG_Mod7_Cellvolt_0}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_2 */
    { {CAN0_MSG_Mod7_Cellvolt_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_3_5 */
    { {CAN0_MSG_Mod7_Cellvolt_1}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_3 */
    { {CAN0_MSG_Mod7_Cellvolt_1}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_4 */
    { {CAN0_MSG_Mod7_Cellvolt_1}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_5 */
    { {CAN0_MSG_Mod7_Cellvolt_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_6_8 */
    { {CAN0_MSG_Mod7_Cellvolt_2}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_6 */
    { {CAN0_MSG_Mod7_Cellvolt_2}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_7 */
    { {CAN0_MSG_Mod7_Cellvolt_2}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_8 */
    { {CAN0_MSG_Mod7_Cellvolt_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_9_11 */
    { {CAN0_MSG_Mod7_Cellvolt_3}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_9 */
    { {CAN0_MSG_Mod7_Cellvolt_3}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_10 */
    { {CAN0_MSG_Mod7_Cellvolt_3}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_11 */
    { {CAN0_MSG_Mod7_Cellvolt_4}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_12_14 */
    { {CAN0_MSG_Mod7_Cellvolt_4}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_12 */
    { {CAN0_MSG_Mod7_Cellvolt_4}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_13 */
    { {CAN0_MSG_Mod7_Cellvolt_4}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_14 */
    { {CAN0_MSG_Mod7_Cellvolt_5}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_valid_15_17 */
    { {CAN0_MSG_Mod7_Cellvolt_5}, 8, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_15 */
    { {CAN0_MSG_Mod7_Cellvolt_5}, 24, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_16 */
    { {CAN0_MSG_Mod7_Cellvolt_5}, 40, 16, 0, UINT16_MAX, 1, 0, littleEndian, &cans_getvolt },  /*!< CAN0_SIG_Mod7_volt_17 */

    /* Module 7 cell temperatures */
    { {CAN0_MSG_Mod7_Celltemp_0}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_0_2 */
    { {CAN0_MSG_Mod7_Celltemp_0}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_0 */
    { {CAN0_MSG_Mod7_Celltemp_0}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_1 */
    { {CAN0_MSG_Mod7_Celltemp_0}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_2 */
    { {CAN0_MSG_Mod7_Celltemp_1}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_3_5 */
    { {CAN0_MSG_Mod7_Celltemp_1}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_3 */
    { {CAN0_MSG_Mod7_Celltemp_1}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_4 */
    { {CAN0_MSG_Mod7_Celltemp_1}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_5 */
    { {CAN0_MSG_Mod7_Celltemp_2}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_6_8 */
    { {CAN0_MSG_Mod7_Celltemp_2}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_6 */
    { {CAN0_MSG_Mod7_Celltemp_2}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_7 */
    { {CAN0_MSG_Mod7_Celltemp_2}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_8 */
    { {CAN0_MSG_Mod7_Celltemp_3}, 0, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_volt_valid_9_11 */
    { {CAN0_MSG_Mod7_Celltemp_3}, 8, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_9 */
    { {CAN0_MSG_Mod7_Celltemp_3}, 24, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_10 */
    { {CAN0_MSG_Mod7_Celltemp_3}, 40, 16, -128, 527.35, 100, 128, littleEndian, &cans_gettemp },  /*!< CAN0_SIG_Mod4_temp_11 */

#ifdef CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED
        {{CAN0_MSG_BMS_CurrentTrigger}, 0, 32, 0, 0, 1, 0, &cans_gettriggercurrent }  /*!< CAN0_SIG_ISA_Trigger */
#endif /* CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED */
};


const CANS_signal_s cans_CAN1_signals_tx[] = {
};


const CANS_signal_s cans_CAN0_signals_rx[] = {
    { {CAN0_MSG_StateRequest}, 8, 8, 0, UINT8_MAX, 1, 0, littleEndian, &cans_setstaterequest },
    { {CAN0_MSG_IVT_Current}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS0_I_MuxID */
    { {CAN0_MSG_IVT_Current}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS0_I_Status */
    { {CAN0_MSG_IVT_Current}, 16, 32, INT32_MIN, INT32_MAX, 1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS0_I_Measurement */
    { {CAN0_MSG_IVT_Voltage_1}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS1_U1_MuxID */
    { {CAN0_MSG_IVT_Voltage_1}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS1_U1_Status */
    { {CAN0_MSG_IVT_Voltage_1}, 16, 32, 0, INT32_MAX, 1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS1_U1_Measurement */
    { {CAN0_MSG_IVT_Voltage_2}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS2_U2_MuxID */
    { {CAN0_MSG_IVT_Voltage_2}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS2_U2_Status */
    { {CAN0_MSG_IVT_Voltage_2}, 16, 32, 0, INT32_MAX, 1, 0, bigEndian, &cans_setcurr, },  /* CAN0_SIG_ISENS2_U2_Measurement */
    { {CAN0_MSG_IVT_Voltage_3}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS3_U3_MuxID */
    { {CAN0_MSG_IVT_Voltage_3}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS3_U3_Status */
    { {CAN0_MSG_IVT_Voltage_3}, 16, 32, 0, INT32_MAX, 1, 0, bigEndian, &cans_setcurr, },  /* CAN0_SIG_ISENS3_U3_Measurement */
    { {CAN0_MSG_IVT_Temperature}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS4_T_MuxID */
    { {CAN0_MSG_IVT_Temperature}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS4_T_Status */
    { {CAN0_MSG_IVT_Temperature}, 16, 32, INT32_MIN, INT32_MAX, 0.1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS4_T_Measurement */
    { {CAN0_MSG_IVT_Power}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS5_P_MuxID */
    { {CAN0_MSG_IVT_Power}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS5_P_Status */
    { {CAN0_MSG_IVT_Power}, 16, 32, INT32_MIN, INT32_MAX, 1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS5_P_Measurement */
    { {CAN0_MSG_IVT_CoulombCount}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS6_CC_MuxID */
    { {CAN0_MSG_IVT_CoulombCount}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS6_CC_Status */
    { {CAN0_MSG_IVT_CoulombCount}, 16, 32, INT32_MIN, INT32_MAX, 1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS6_CC_Measurement */
    { {CAN0_MSG_IVT_EnergyCount}, 0, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS7_EC_MuxID */
    { {CAN0_MSG_IVT_EnergyCount}, 8, 8, 0, UINT8_MAX, 1, 0, bigEndian, NULL_PTR },  /* CAN0_SIG_ISENS7_EC_Status */
    { {CAN0_MSG_IVT_EnergyCount}, 16, 32, INT32_MIN, INT32_MAX, 1, 0, bigEndian, &cans_setcurr },  /* CAN0_SIG_ISENS7_EC_Measurement */
    { {CAN0_MSG_DEBUG}, 0, 64, 0, UINT64_MAX, 1, 0, littleEndian, &cans_setdebug },  /* CAN0_SIG_DEBUG_Data */
    { {CAN0_MSG_GetReleaseVersion}, 0, 64, 0, UINT64_MAX, 1, 0, littleEndian, &cans_setSWversion }  /* CAN0_SIG_DEBUG_Data */
};

const CANS_signal_s cans_CAN1_signals_rx[] = {
};


const uint16_t cans_CAN0_signals_tx_length = sizeof(cans_CAN0_signals_tx)/sizeof(cans_CAN0_signals_tx[0]);
const uint16_t cans_CAN1_signals_tx_length = sizeof(cans_CAN1_signals_tx)/sizeof(cans_CAN1_signals_tx[0]);

const uint16_t cans_CAN0_signals_rx_length = sizeof(cans_CAN0_signals_rx)/sizeof(cans_CAN0_signals_rx[0]);
const uint16_t cans_CAN1_signals_rx_length = sizeof(cans_CAN1_signals_rx)/sizeof(cans_CAN1_signals_rx[0]);

/*================== Static Function Implementations ========================*/

static uint32_t cans_getvolt(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_CELLVOLTAGE_s volt_tab;
    uint16_t modIdx = 0;
    uint32_t cellIdx = 0;
    uint32_t tmp = 0;
    uint32_t tmpVal = 0;
    float canData = 0;

    /* first signal to transmit cell voltages */
    if (sigIdx == CAN0_SIG_Mod0_volt_valid_0_2) {
        DB_ReadBlock(&volt_tab, DATA_BLOCK_ID_CELLVOLTAGE);
    }

    /* Determine module and cell number */
    if (sigIdx - CAN0_SIG_Mod0_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 0;
        cellIdx = sigIdx - CAN0_SIG_Mod0_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod1_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 1;
        cellIdx = sigIdx - CAN0_SIG_Mod1_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod2_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 2;
        cellIdx = sigIdx - CAN0_SIG_Mod2_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod3_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 3;
        cellIdx = sigIdx - CAN0_SIG_Mod3_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod4_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 4;
        cellIdx = sigIdx - CAN0_SIG_Mod4_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod5_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 5;
        cellIdx = sigIdx - CAN0_SIG_Mod5_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod6_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 6;
        cellIdx = sigIdx - CAN0_SIG_Mod6_volt_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod7_volt_valid_0_2 < CANS_MODULSIGNALS_VOLT) {
        modIdx = 7;
        cellIdx = sigIdx - CAN0_SIG_Mod7_volt_valid_0_2;
    }

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_Mod0_volt_valid_0_2:
            case CAN0_SIG_Mod1_volt_valid_0_2:
            case CAN0_SIG_Mod2_volt_valid_0_2:
            case CAN0_SIG_Mod3_volt_valid_0_2:
            case CAN0_SIG_Mod4_volt_valid_0_2:  /* Valid flags for cell voltages 0 - 2 */
            case CAN0_SIG_Mod5_volt_valid_0_2:
            case CAN0_SIG_Mod6_volt_valid_0_2:
            case CAN0_SIG_Mod7_volt_valid_0_2:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx];
                }
                *(uint32_t *)value = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_valid_3_5:
            case CAN0_SIG_Mod1_volt_valid_3_5:
            case CAN0_SIG_Mod2_volt_valid_3_5:
            case CAN0_SIG_Mod3_volt_valid_3_5:
            case CAN0_SIG_Mod4_volt_valid_3_5:  /* Valid flags for cell voltages 3 - 5 */
            case CAN0_SIG_Mod5_volt_valid_3_5:
            case CAN0_SIG_Mod6_volt_valid_3_5:
            case CAN0_SIG_Mod7_volt_valid_3_5:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx] >> 3;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_valid_6_8:
            case CAN0_SIG_Mod1_volt_valid_6_8:
            case CAN0_SIG_Mod2_volt_valid_6_8:
            case CAN0_SIG_Mod3_volt_valid_6_8:
            case CAN0_SIG_Mod4_volt_valid_6_8:  /* Valid flags for cell voltages 6 - 8 */
            case CAN0_SIG_Mod5_volt_valid_6_8:
            case CAN0_SIG_Mod6_volt_valid_6_8:
            case CAN0_SIG_Mod7_volt_valid_6_8:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx] >> 6;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_valid_9_11:
            case CAN0_SIG_Mod1_volt_valid_9_11:
            case CAN0_SIG_Mod2_volt_valid_9_11:
            case CAN0_SIG_Mod3_volt_valid_9_11:
            case CAN0_SIG_Mod4_volt_valid_9_11:  /* Valid flags for cell voltages 9 - 11 */
            case CAN0_SIG_Mod5_volt_valid_9_11:
            case CAN0_SIG_Mod6_volt_valid_9_11:
            case CAN0_SIG_Mod7_volt_valid_9_11:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx] >> 9;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_valid_12_14:
            case CAN0_SIG_Mod1_volt_valid_12_14:
            case CAN0_SIG_Mod2_volt_valid_12_14:
            case CAN0_SIG_Mod3_volt_valid_12_14:
            case CAN0_SIG_Mod4_volt_valid_12_14:  /* Valid flags for cell voltages 12 - 14 */
            case CAN0_SIG_Mod5_volt_valid_12_14:
            case CAN0_SIG_Mod6_volt_valid_12_14:
            case CAN0_SIG_Mod7_volt_valid_12_14:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx] >> 12;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_valid_15_17:
            case CAN0_SIG_Mod1_volt_valid_15_17:
            case CAN0_SIG_Mod2_volt_valid_15_17:
            case CAN0_SIG_Mod3_volt_valid_15_17:
            case CAN0_SIG_Mod4_volt_valid_15_17:  /* Valid flags for cell voltages 15 - 17 */
            case CAN0_SIG_Mod5_volt_valid_15_17:
            case CAN0_SIG_Mod6_volt_valid_15_17:
            case CAN0_SIG_Mod7_volt_valid_15_17:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = volt_tab.valid_volt[modIdx] >> 15;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_volt_0:
            case CAN0_SIG_Mod0_volt_1:
            case CAN0_SIG_Mod0_volt_2:
            case CAN0_SIG_Mod1_volt_0:
            case CAN0_SIG_Mod1_volt_1:
            case CAN0_SIG_Mod1_volt_2:
            case CAN0_SIG_Mod2_volt_0:
            case CAN0_SIG_Mod2_volt_1:
            case CAN0_SIG_Mod2_volt_2:
            case CAN0_SIG_Mod3_volt_0:
            case CAN0_SIG_Mod3_volt_1:
            case CAN0_SIG_Mod3_volt_2:
            case CAN0_SIG_Mod4_volt_0:
            case CAN0_SIG_Mod4_volt_1:
            case CAN0_SIG_Mod4_volt_2:
            case CAN0_SIG_Mod5_volt_0:
            case CAN0_SIG_Mod5_volt_1:
            case CAN0_SIG_Mod5_volt_2:
            case CAN0_SIG_Mod6_volt_0:
            case CAN0_SIG_Mod6_volt_1:
            case CAN0_SIG_Mod6_volt_2:
            case CAN0_SIG_Mod7_volt_0:
            case CAN0_SIG_Mod7_volt_1:
            case CAN0_SIG_Mod7_volt_2:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_volt_3:
            case CAN0_SIG_Mod0_volt_4:
            case CAN0_SIG_Mod0_volt_5:
            case CAN0_SIG_Mod1_volt_3:
            case CAN0_SIG_Mod1_volt_4:
            case CAN0_SIG_Mod1_volt_5:
            case CAN0_SIG_Mod2_volt_3:
            case CAN0_SIG_Mod2_volt_4:
            case CAN0_SIG_Mod2_volt_5:
            case CAN0_SIG_Mod3_volt_3:
            case CAN0_SIG_Mod3_volt_4:
            case CAN0_SIG_Mod3_volt_5:
            case CAN0_SIG_Mod4_volt_3:
            case CAN0_SIG_Mod4_volt_4:
            case CAN0_SIG_Mod4_volt_5:
            case CAN0_SIG_Mod5_volt_3:
            case CAN0_SIG_Mod5_volt_4:
            case CAN0_SIG_Mod5_volt_5:
            case CAN0_SIG_Mod6_volt_3:
            case CAN0_SIG_Mod6_volt_4:
            case CAN0_SIG_Mod6_volt_5:
            case CAN0_SIG_Mod7_volt_3:
            case CAN0_SIG_Mod7_volt_4:
            case CAN0_SIG_Mod7_volt_5:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_3_5 */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_volt_6:
            case CAN0_SIG_Mod0_volt_7:
            case CAN0_SIG_Mod0_volt_8:
            case CAN0_SIG_Mod1_volt_6:
            case CAN0_SIG_Mod1_volt_7:
            case CAN0_SIG_Mod1_volt_8:
            case CAN0_SIG_Mod2_volt_6:
            case CAN0_SIG_Mod2_volt_7:
            case CAN0_SIG_Mod2_volt_8:
            case CAN0_SIG_Mod3_volt_6:
            case CAN0_SIG_Mod3_volt_7:
            case CAN0_SIG_Mod3_volt_8:
            case CAN0_SIG_Mod4_volt_6:
            case CAN0_SIG_Mod4_volt_7:
            case CAN0_SIG_Mod4_volt_8:
            case CAN0_SIG_Mod5_volt_6:
            case CAN0_SIG_Mod5_volt_7:
            case CAN0_SIG_Mod5_volt_8:
            case CAN0_SIG_Mod6_volt_6:
            case CAN0_SIG_Mod6_volt_7:
            case CAN0_SIG_Mod6_volt_8:
            case CAN0_SIG_Mod7_volt_6:
            case CAN0_SIG_Mod7_volt_7:
            case CAN0_SIG_Mod7_volt_8:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_6_8 */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_volt_9:
            case CAN0_SIG_Mod0_volt_10:
            case CAN0_SIG_Mod0_volt_11:
            case CAN0_SIG_Mod1_volt_9:
            case CAN0_SIG_Mod1_volt_10:
            case CAN0_SIG_Mod1_volt_11:
            case CAN0_SIG_Mod2_volt_9:
            case CAN0_SIG_Mod2_volt_10:
            case CAN0_SIG_Mod2_volt_11:
            case CAN0_SIG_Mod3_volt_9:
            case CAN0_SIG_Mod3_volt_10:
            case CAN0_SIG_Mod3_volt_11:
            case CAN0_SIG_Mod4_volt_9:
            case CAN0_SIG_Mod4_volt_10:
            case CAN0_SIG_Mod4_volt_11:
            case CAN0_SIG_Mod5_volt_9:
            case CAN0_SIG_Mod5_volt_10:
            case CAN0_SIG_Mod5_volt_11:
            case CAN0_SIG_Mod6_volt_9:
            case CAN0_SIG_Mod6_volt_10:
            case CAN0_SIG_Mod6_volt_11:
            case CAN0_SIG_Mod7_volt_9:
            case CAN0_SIG_Mod7_volt_10:
            case CAN0_SIG_Mod7_volt_11:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_6_8 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_9_11 */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_volt_12:
            case CAN0_SIG_Mod0_volt_13:
            case CAN0_SIG_Mod0_volt_14:
            case CAN0_SIG_Mod1_volt_12:
            case CAN0_SIG_Mod1_volt_13:
            case CAN0_SIG_Mod1_volt_14:
            case CAN0_SIG_Mod2_volt_12:
            case CAN0_SIG_Mod2_volt_13:
            case CAN0_SIG_Mod2_volt_14:
            case CAN0_SIG_Mod3_volt_12:
            case CAN0_SIG_Mod3_volt_13:
            case CAN0_SIG_Mod3_volt_14:
            case CAN0_SIG_Mod4_volt_12:
            case CAN0_SIG_Mod4_volt_13:
            case CAN0_SIG_Mod4_volt_14:
            case CAN0_SIG_Mod5_volt_12:
            case CAN0_SIG_Mod5_volt_13:
            case CAN0_SIG_Mod5_volt_14:
            case CAN0_SIG_Mod6_volt_12:
            case CAN0_SIG_Mod6_volt_13:
            case CAN0_SIG_Mod6_volt_14:
            case CAN0_SIG_Mod7_volt_12:
            case CAN0_SIG_Mod7_volt_13:
            case CAN0_SIG_Mod7_volt_14:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_6_8 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_9_11 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_12_14 */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_volt_15:
            case CAN0_SIG_Mod0_volt_16:
            case CAN0_SIG_Mod0_volt_17:
            case CAN0_SIG_Mod1_volt_15:
            case CAN0_SIG_Mod1_volt_16:
            case CAN0_SIG_Mod1_volt_17:
            case CAN0_SIG_Mod2_volt_15:
            case CAN0_SIG_Mod2_volt_16:
            case CAN0_SIG_Mod2_volt_17:
            case CAN0_SIG_Mod3_volt_15:
            case CAN0_SIG_Mod3_volt_16:
            case CAN0_SIG_Mod3_volt_17:
            case CAN0_SIG_Mod4_volt_15:
            case CAN0_SIG_Mod4_volt_16:
            case CAN0_SIG_Mod4_volt_17:
            case CAN0_SIG_Mod5_volt_15:
            case CAN0_SIG_Mod5_volt_16:
            case CAN0_SIG_Mod5_volt_17:
            case CAN0_SIG_Mod6_volt_15:
            case CAN0_SIG_Mod6_volt_16:
            case CAN0_SIG_Mod6_volt_17:
            case CAN0_SIG_Mod7_volt_15:
            case CAN0_SIG_Mod7_volt_16:
            case CAN0_SIG_Mod7_volt_17:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_6_8 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_9_11 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_12_14 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_volt_valid_15_17 */
                if ((modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx >= BS_NR_OF_BAT_CELLS) {
                    tmpVal = CAN_DEFAULT_VOLTAGE;
                } else {
                    tmpVal = volt_tab.voltage[(modIdx * BS_NR_OF_BAT_CELLS_PER_MODULE) + cellIdx];
                }
                break;

            default:
                break;
        }
        /* Check limits */
        canData = cans_checkLimits((float)tmpVal, sigIdx);
        /* Apply offset and factor */
        *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
    }

    return 0;
}

uint32_t cans_gettemp(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_CELLTEMPERATURE_s temp_tab;
    uint16_t modIdx = 0;
    uint32_t cellIdx = 0;
    uint32_t tmp = 0;
    float tmpVal = 0;
    float canData = 0;

    /* first signal to transmit cell temperatures */
    if (sigIdx == CAN0_SIG_Mod0_temp_valid_0_2) {
        DB_ReadBlock(&temp_tab, DATA_BLOCK_ID_CELLTEMPERATURE);
    }

    /* Determine module and cell number */
    if (sigIdx - CAN0_SIG_Mod0_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 0;
        cellIdx = sigIdx - CAN0_SIG_Mod0_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod1_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 1;
        cellIdx = sigIdx - CAN0_SIG_Mod1_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod2_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 2;
        cellIdx = sigIdx - CAN0_SIG_Mod2_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod3_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 3;
        cellIdx = sigIdx - CAN0_SIG_Mod3_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod4_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 4;
        cellIdx = sigIdx - CAN0_SIG_Mod4_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod5_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 5;
        cellIdx = sigIdx - CAN0_SIG_Mod5_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod6_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 6;
        cellIdx = sigIdx - CAN0_SIG_Mod6_temp_valid_0_2;
    } else if (sigIdx - CAN0_SIG_Mod7_temp_valid_0_2 < CANS_MODULSIGNALS_TEMP) {
        modIdx = 7;
        cellIdx = sigIdx - CAN0_SIG_Mod7_temp_valid_0_2;
    }

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_Mod0_temp_valid_0_2:
            case CAN0_SIG_Mod1_temp_valid_0_2:
            case CAN0_SIG_Mod2_temp_valid_0_2:
            case CAN0_SIG_Mod3_temp_valid_0_2:
            case CAN0_SIG_Mod4_temp_valid_0_2:
            case CAN0_SIG_Mod5_temp_valid_0_2:
            case CAN0_SIG_Mod6_temp_valid_0_2:
            case CAN0_SIG_Mod7_temp_valid_0_2:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = temp_tab.valid_temperature[modIdx];
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_temp_valid_3_5:
            case CAN0_SIG_Mod1_temp_valid_3_5:
            case CAN0_SIG_Mod2_temp_valid_3_5:
            case CAN0_SIG_Mod3_temp_valid_3_5:
            case CAN0_SIG_Mod4_temp_valid_3_5:
            case CAN0_SIG_Mod5_temp_valid_3_5:
            case CAN0_SIG_Mod6_temp_valid_3_5:
            case CAN0_SIG_Mod7_temp_valid_3_5:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = temp_tab.valid_temperature[modIdx] >> 3;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_temp_valid_6_8:
            case CAN0_SIG_Mod1_temp_valid_6_8:
            case CAN0_SIG_Mod2_temp_valid_6_8:
            case CAN0_SIG_Mod3_temp_valid_6_8:
            case CAN0_SIG_Mod4_temp_valid_6_8:
            case CAN0_SIG_Mod5_temp_valid_6_8:
            case CAN0_SIG_Mod6_temp_valid_6_8:
            case CAN0_SIG_Mod7_temp_valid_6_8:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = temp_tab.valid_temperature[modIdx] >> 6;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_temp_valid_9_11:
            case CAN0_SIG_Mod1_temp_valid_9_11:
            case CAN0_SIG_Mod2_temp_valid_9_11:
            case CAN0_SIG_Mod3_temp_valid_9_11:
            case CAN0_SIG_Mod4_temp_valid_9_11:
            case CAN0_SIG_Mod5_temp_valid_9_11:
            case CAN0_SIG_Mod6_temp_valid_9_11:
            case CAN0_SIG_Mod7_temp_valid_9_11:
                if (modIdx >= BS_NR_OF_MODULES) {
                    tmpVal = CAN_DEFAULT_VALID_FLAG;
                } else {
                    tmp = temp_tab.valid_temperature[modIdx] >> 9;
                }
                tmpVal = 0x07 & tmp;
                break;

            case CAN0_SIG_Mod0_temp_0:
            case CAN0_SIG_Mod0_temp_1:
            case CAN0_SIG_Mod0_temp_2:
            case CAN0_SIG_Mod1_temp_0:
            case CAN0_SIG_Mod1_temp_1:
            case CAN0_SIG_Mod1_temp_2:
            case CAN0_SIG_Mod2_temp_0:
            case CAN0_SIG_Mod2_temp_1:
            case CAN0_SIG_Mod2_temp_2:
            case CAN0_SIG_Mod3_temp_0:
            case CAN0_SIG_Mod3_temp_1:
            case CAN0_SIG_Mod3_temp_2:
            case CAN0_SIG_Mod4_temp_0:
            case CAN0_SIG_Mod4_temp_1:
            case CAN0_SIG_Mod4_temp_2:
            case CAN0_SIG_Mod5_temp_0:
            case CAN0_SIG_Mod5_temp_1:
            case CAN0_SIG_Mod5_temp_2:
            case CAN0_SIG_Mod6_temp_0:
            case CAN0_SIG_Mod6_temp_1:
            case CAN0_SIG_Mod6_temp_2:
            case CAN0_SIG_Mod7_temp_0:
            case CAN0_SIG_Mod7_temp_1:
            case CAN0_SIG_Mod7_temp_2:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                if ((modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx >= BS_NR_OF_TEMP_SENSORS) {
                    tmpVal = CAN_DEFAULT_TEMPERATURE;
                } else {
                    tmpVal = temp_tab.temperature[(modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_temp_3:
            case CAN0_SIG_Mod0_temp_4:
            case CAN0_SIG_Mod0_temp_5:
            case CAN0_SIG_Mod1_temp_3:
            case CAN0_SIG_Mod1_temp_4:
            case CAN0_SIG_Mod1_temp_5:
            case CAN0_SIG_Mod2_temp_3:
            case CAN0_SIG_Mod2_temp_4:
            case CAN0_SIG_Mod2_temp_5:
            case CAN0_SIG_Mod3_temp_3:
            case CAN0_SIG_Mod3_temp_4:
            case CAN0_SIG_Mod3_temp_5:
            case CAN0_SIG_Mod4_temp_3:
            case CAN0_SIG_Mod4_temp_4:
            case CAN0_SIG_Mod4_temp_5:
            case CAN0_SIG_Mod5_temp_3:
            case CAN0_SIG_Mod5_temp_4:
            case CAN0_SIG_Mod5_temp_5:
            case CAN0_SIG_Mod6_temp_3:
            case CAN0_SIG_Mod6_temp_4:
            case CAN0_SIG_Mod6_temp_5:
            case CAN0_SIG_Mod7_temp_3:
            case CAN0_SIG_Mod7_temp_4:
            case CAN0_SIG_Mod7_temp_5:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_3_5 */
                if ((modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx >= BS_NR_OF_TEMP_SENSORS) {
                    tmpVal = CAN_DEFAULT_TEMPERATURE;
                } else {
                    tmpVal = temp_tab.temperature[(modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_temp_6:
            case CAN0_SIG_Mod0_temp_7:
            case CAN0_SIG_Mod0_temp_8:
            case CAN0_SIG_Mod1_temp_6:
            case CAN0_SIG_Mod1_temp_7:
            case CAN0_SIG_Mod1_temp_8:
            case CAN0_SIG_Mod2_temp_6:
            case CAN0_SIG_Mod2_temp_7:
            case CAN0_SIG_Mod2_temp_8:
            case CAN0_SIG_Mod3_temp_6:
            case CAN0_SIG_Mod3_temp_7:
            case CAN0_SIG_Mod3_temp_8:
            case CAN0_SIG_Mod4_temp_6:
            case CAN0_SIG_Mod4_temp_7:
            case CAN0_SIG_Mod4_temp_8:
            case CAN0_SIG_Mod5_temp_6:
            case CAN0_SIG_Mod5_temp_7:
            case CAN0_SIG_Mod5_temp_8:
            case CAN0_SIG_Mod6_temp_6:
            case CAN0_SIG_Mod6_temp_7:
            case CAN0_SIG_Mod6_temp_8:
            case CAN0_SIG_Mod7_temp_6:
            case CAN0_SIG_Mod7_temp_7:
            case CAN0_SIG_Mod7_temp_8:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_6_8 */
                if ((modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx >= BS_NR_OF_TEMP_SENSORS) {
                    tmpVal = CAN_DEFAULT_TEMPERATURE;
                } else {
                    tmpVal = temp_tab.temperature[(modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx];
                }
                break;

            case CAN0_SIG_Mod0_temp_9:
            case CAN0_SIG_Mod0_temp_10:
            case CAN0_SIG_Mod0_temp_11:
            case CAN0_SIG_Mod1_temp_9:
            case CAN0_SIG_Mod1_temp_10:
            case CAN0_SIG_Mod1_temp_11:
            case CAN0_SIG_Mod2_temp_9:
            case CAN0_SIG_Mod2_temp_10:
            case CAN0_SIG_Mod2_temp_11:
            case CAN0_SIG_Mod3_temp_9:
            case CAN0_SIG_Mod3_temp_10:
            case CAN0_SIG_Mod3_temp_11:
            case CAN0_SIG_Mod4_temp_9:
            case CAN0_SIG_Mod4_temp_10:
            case CAN0_SIG_Mod4_temp_11:
            case CAN0_SIG_Mod5_temp_9:
            case CAN0_SIG_Mod5_temp_10:
            case CAN0_SIG_Mod5_temp_11:
            case CAN0_SIG_Mod6_temp_9:
            case CAN0_SIG_Mod6_temp_10:
            case CAN0_SIG_Mod6_temp_11:
            case CAN0_SIG_Mod7_temp_9:
            case CAN0_SIG_Mod7_temp_10:
            case CAN0_SIG_Mod7_temp_11:
                cellIdx--;  /* Because cell 0 - valid flag = 1, decrement by one to get the right index */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_3_5 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_6_8 */
                cellIdx--;  /* Because of signal: CAN0_SIG_Modx_temp_valid_9_11 */
                if ((modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx >= BS_NR_OF_TEMP_SENSORS) {
                    tmpVal = CAN_DEFAULT_TEMPERATURE;
                } else {
                    tmpVal = temp_tab.temperature[(modIdx * BS_NR_OF_TEMP_SENSORS_PER_MODULE) + cellIdx];
                }
                break;

            default:
                break;
        }
        /* Check limits */
        canData = cans_checkLimits((float)tmpVal, sigIdx);
        /* Apply offset and factor */
        *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
    }

    return 0;
}


uint32_t cans_gettempering(uint32_t sigIdx, void *value) {
    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_CoolingNeeded:
                *(uint32_t *)value = 0;
                break;

            case CAN0_SIG_HeatingNeeded:
                *(uint32_t *)value = 0;
                break;

            case CAN0_SIG_TemperingDemand:
                *(uint32_t *)value = 0;
                break;

            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}


uint32_t cans_getcanerr(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_ERRORSTATE_s canerr_tab;
    static DATA_BLOCK_MSL_FLAG_s canMSL_tab;
    static DATA_BLOCK_RSL_FLAG_s canRSL_tab;
    static DATA_BLOCK_MOL_FLAG_s canMOL_tab;
    static DATA_BLOCK_CONTFEEDBACK_s cancontfeedback_tab;
    static DATA_BLOCK_ILCKFEEDBACK_s canilckfeedback_tab;
    static DATA_BLOCK_BALANCING_CONTROL_s balancing_tab;
    static DATA_BLOCK_SYSTEMSTATE_s systemstate_tab;

    static uint8_t tmp = 0;

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_GS0_general_error:

                /* First signal in CAN_MSG_GeneralState messages -> get database entry */
                DB_ReadBlock(&canerr_tab, DATA_BLOCK_ID_ERRORSTATE);
                DB_ReadBlock(&canMSL_tab, DATA_BLOCK_ID_MSL);
                DB_ReadBlock(&canRSL_tab, DATA_BLOCK_ID_RSL);
                DB_ReadBlock(&canMOL_tab, DATA_BLOCK_ID_MOL);
                tmp = 0;

                /* Check maximum safety limit flags */
                if (canMSL_tab.over_current_charge_cell    == 1 ||
                    canMSL_tab.over_current_discharge_cell == 1 ||
                    canMSL_tab.over_current_charge_pl0     == 1 ||
                    canMSL_tab.over_current_discharge_pl0  == 1 ||
                    canMSL_tab.over_current_charge_pl1     == 1 ||
                    canMSL_tab.over_current_discharge_pl1  == 1 ||
                    canMSL_tab.over_voltage                == 1 ||
                    canMSL_tab.under_voltage               == 1 ||
                    canMSL_tab.over_temperature_charge     == 1 ||
                    canMSL_tab.over_temperature_discharge  == 1 ||
                    canMSL_tab.under_temperature_charge    == 1 ||
                    canMSL_tab.under_temperature_discharge == 1 ||
                /* Check system error flags */
                    canerr_tab.deepDischargeDetected     == 1 ||
                    canerr_tab.main_plus                 == 1 ||
                    canerr_tab.main_minus                == 1 ||
                    canerr_tab.precharge                 == 1 ||
                    canerr_tab.charge_main_plus          == 1 ||
                    canerr_tab.charge_main_minus         == 1 ||
                    canerr_tab.charge_precharge          == 1 ||
                    canerr_tab.fuse_state_normal         == 1 ||
                    canerr_tab.fuse_state_charge         == 1 ||
                    canerr_tab.interlock                 == 1 ||
                    canerr_tab.crc_error                 == 1 ||
                    canerr_tab.mux_error                 == 1 ||
                    canerr_tab.spi_error                 == 1 ||
                    canerr_tab.currentsensorresponding   == 1 ||
                    canerr_tab.open_wire                 == 1 ||
            #if BMS_OPEN_CONTACTORS_ON_INSULATION_ERROR == TRUE
                    canerr_tab.insulation_error          == 1 ||
            #endif /* BMS_OPEN_CONTACTORS_ON_INSULATION_ERROR */
                    canerr_tab.can_timing_cc             == 1 ||
                    canerr_tab.can_timing                == 1) {
                    /* set flag if error detected */
                    tmp |= 0x01 << 0;
                }
                /* Check recommended safety limit flags */
                if (canRSL_tab.over_current_charge_cell    == 1 ||
                    canRSL_tab.over_current_discharge_cell == 1 ||
                    canRSL_tab.over_current_charge_pl0     == 1 ||
                    canRSL_tab.over_current_discharge_pl0  == 1 ||
                    canRSL_tab.over_current_charge_pl1     == 1 ||
                    canRSL_tab.over_current_discharge_pl1  == 1 ||
                    canRSL_tab.over_voltage                == 1 ||
                    canRSL_tab.under_voltage               == 1 ||
                    canRSL_tab.over_temperature_charge     == 1 ||
                    canRSL_tab.over_temperature_discharge  == 1 ||
                    canRSL_tab.under_temperature_charge    == 1 ||
                    canRSL_tab.under_temperature_discharge == 1) {
                    /* set flag if error detected */
                    tmp |= 0x01 << 1;
                }
                /* Check maximum operating limit flags */
                if (canMOL_tab.over_current_charge_cell    == 1 ||
                    canMOL_tab.over_current_discharge_cell == 1 ||
                    canMOL_tab.over_current_charge_pl0     == 1 ||
                    canMOL_tab.over_current_discharge_pl0  == 1 ||
                    canMOL_tab.over_current_charge_pl1     == 1 ||
                    canMOL_tab.over_current_discharge_pl1  == 1 ||
                    canMOL_tab.over_voltage                == 1 ||
                    canMOL_tab.under_voltage               == 1 ||
                    canMOL_tab.over_temperature_charge     == 1 ||
                    canMOL_tab.over_temperature_discharge  == 1 ||
                    canMOL_tab.under_temperature_charge    == 1 ||
                    canMOL_tab.under_temperature_discharge == 1) {
                    /* set flag if error detected */
                    tmp |= 0x01 << 2;
                }
                *(uint32_t *)value = tmp;
                break;

            case CAN0_SIG_GS0_current_state:
                DB_ReadBlock(&systemstate_tab, DATA_BLOCK_ID_SYSTEMSTATE);
                *(uint32_t *)value = systemstate_tab.bms_state;
                break;
            case CAN0_SIG_GS0_error_overtemp_charge:
                tmp = 0;
                tmp |= canMOL_tab.over_temperature_charge << 2;
                tmp |= canRSL_tab.over_temperature_charge << 1;
                tmp |= canMSL_tab.over_temperature_charge;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS0_error_undertemp_charge:
                tmp = 0;
                tmp |= canMOL_tab.under_temperature_charge << 2;
                tmp |= canRSL_tab.under_temperature_charge << 1;
                tmp |= canMSL_tab.under_temperature_charge;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS0_error_overtemp_discharge:
                tmp = 0;
                tmp |= canMOL_tab.over_temperature_discharge << 2;
                tmp |= canRSL_tab.over_temperature_discharge << 1;
                tmp |= canMSL_tab.over_temperature_discharge;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS0_error_undertemp_discharge:
                tmp = 0;
                tmp |= canMOL_tab.under_temperature_discharge << 2;
                tmp |= canRSL_tab.under_temperature_discharge << 1;
                tmp |= canMSL_tab.under_temperature_discharge;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS0_error_overcurrent_charge:
                tmp = 0;
                tmp |= canMOL_tab.over_current_charge_cell << 2;
                tmp |= canMOL_tab.over_current_charge_pl0 << 2;
                tmp |= canMOL_tab.over_current_charge_pl1 << 2;
                tmp |= canRSL_tab.over_current_charge_cell << 1;
                tmp |= canRSL_tab.over_current_charge_pl0 << 1;
                tmp |= canRSL_tab.over_current_charge_pl1 << 1;
                tmp |= canMSL_tab.over_current_charge_cell;
                tmp |= canMSL_tab.over_current_charge_pl0;
                tmp |= canMSL_tab.over_current_charge_pl1;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS0_error_overcurrent_discharge:
                tmp = 0;
                tmp |= canMOL_tab.over_current_discharge_cell << 2;
                tmp |= canMOL_tab.over_current_discharge_pl0 << 2;
                tmp |= canMOL_tab.over_current_discharge_pl1 << 2;
                tmp |= canRSL_tab.over_current_discharge_cell << 1;
                tmp |= canRSL_tab.over_current_discharge_pl0 << 1;
                tmp |= canRSL_tab.over_current_discharge_pl1 << 1;
                tmp |= canMSL_tab.over_current_discharge_cell;
                tmp |= canMSL_tab.over_current_discharge_pl0;
                tmp |= canMSL_tab.over_current_discharge_pl1;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS1_error_overvoltage:
                tmp = 0;
                tmp |= canMOL_tab.over_voltage << 2;
                tmp |= canRSL_tab.over_voltage << 1;
                tmp |= canMSL_tab.over_voltage;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS1_error_undervoltage:
                tmp = 0;
                tmp |= canMOL_tab.under_voltage << 2;
                tmp |= canRSL_tab.under_voltage << 1;
                tmp |= canMSL_tab.under_voltage;
                *(uint32_t *)value = tmp;
                break;
            case CAN0_SIG_GS1_error_deep_discharge:
                *(uint32_t *)value = canerr_tab.deepDischargeDetected;
                break;
            case CAN0_SIG_GS1_error_temperature_MCU0:
                *(uint32_t *)value = canerr_tab.mcuDieTemperature;
                break;
            case CAN0_SIG_GS1_error_contactor:
                *(uint32_t *)value = canerr_tab.main_plus | canerr_tab.main_minus | canerr_tab.precharge | canerr_tab.charge_main_plus | canerr_tab.charge_main_minus | canerr_tab.charge_precharge;
                break;
            case CAN0_SIG_GS1_error_selftest:
                *(uint32_t *)value = 0;
                break;
            case CAN0_SIG_GS1_error_cantiming:
                *(uint32_t *)value = canerr_tab.can_timing;
                break;
            case CAN0_SIG_GS1_current_sensor:
                *(uint32_t *)value = canerr_tab.currentsensorresponding | canerr_tab.can_timing_cc;
                break;
            case CAN0_SIG_GS1_balancing_active:

                /* only signal to use the balancing database entry */
                DB_ReadBlock(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
                *(uint32_t *)value = balancing_tab.enable_balancing;
                break;

            case CAN0_SIG_GS2_state_cont_interlock:
                DB_ReadBlock(&cancontfeedback_tab, DATA_BLOCK_ID_CONTFEEDBACK);
                DB_ReadBlock(&canilckfeedback_tab, DATA_BLOCK_ID_ILCKFEEDBACK);
                cancontfeedback_tab.contactor_feedback &= ~(1 << 9);
                cancontfeedback_tab.contactor_feedback |= canilckfeedback_tab.interlock_feedback << 9;
                *(uint32_t *)value = cancontfeedback_tab.contactor_feedback;
                break;

            case CAN0_SIG_GS2_error_insulation:
                *(uint32_t *)value = canerr_tab.insulation_error;
                break;

            case CAN0_SIG_GS2_fuse_state:
                tmp = 0;
                if (canerr_tab.fuse_state_normal != 0) {
#if BS_CHECK_FUSE_PLACED_IN_NORMAL_PATH == TRUE
                    tmp |= 0x01;
#else /* BS_CHECK_FUSE_PLACED_IN_NORMAL_PATH == FALSE */
                    tmp |= 0x02;
#endif
                }
                if (canerr_tab.fuse_state_charge != 0) {
#if BS_CHECK_FUSE_PLACED_IN_CHARGE_PATH == TRUE
                    tmp |= 0x04;
#else /* BS_CHECK_FUSE_PLACED_IN_CHARGE_PATH == FALSE */
                    tmp |= 0x08;
#endif
                }
                *(uint32_t *)value = tmp;
                break;

            case CAN0_SIG_GS2_lowCoinCellVolt:
                *(uint32_t *)value = canerr_tab.coinCellVoltage;
                break;

            case CAN0_SIG_GS2_error_openWire:
                *(uint32_t *)value = canerr_tab.open_wire;
                break;

            case CAN0_SIG_GS2_daisyChain:
                tmp = 0;
                tmp |= canerr_tab.spi_error;
                tmp |= canerr_tab.crc_error << 1;
                tmp |= canerr_tab.mux_error << 2;
                tmp |= canerr_tab.ltc_config_error << 3;
                *(uint32_t *)value = tmp;
                break;

            case CAN0_SIG_GS2_plausibilityCheck:
                *(uint32_t *)value = canerr_tab.plausibilityCheck;
                break;

            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}


uint32_t cans_getsoc(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_SOX_s sox_tab;
    DB_ReadBlock(&sox_tab, DATA_BLOCK_ID_SOX);
    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_SOC_mean:
                /* CAN signal resolution 0.01%, --> factor 100 */
                *(uint32_t *)value = (uint32_t)(sox_tab.soc_mean * cans_CAN0_signals_tx[sigIdx].factor);
                break;
            case CAN0_SIG_SOC_min:
                /* CAN signal resolution 0.01%, --> factor 100 */
                *(uint32_t *)value = (uint32_t)(sox_tab.soc_min * cans_CAN0_signals_tx[sigIdx].factor);
                break;
            case CAN0_SIG_SOC_max:
                /* CAN signal resolution 0.01%, --> factor 100 */
                *(uint32_t *)value = (uint32_t)(sox_tab.soc_max * cans_CAN0_signals_tx[sigIdx].factor);
                break;
            default:
                *(uint32_t *)value = 50.0;
                break;
        }
    }
    return 0;
}


static uint32_t cans_getRecommendedOperatingCurrent(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_SOF_s sof_tab;
    float canData = 0;

    if (value != NULL_PTR) {
        /* values transmitted in resolution of 10mA (16bit means 0A-655.35A) */
        switch (sigIdx) {
            case CAN0_SIG_RecChargeCurrent:
                /* first signal */
                DB_ReadBlock(&sof_tab, DATA_BLOCK_ID_SOF);

                /* Check limits */
                canData = cans_checkLimits((float)sof_tab.recommended_continuous_charge, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_RecChargeCurrent_Peak:
                /* Check limits */
                canData = cans_checkLimits((float)sof_tab.recommended_peak_charge, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_RecDischargeCurrent:
                /* Check limits */
                canData = cans_checkLimits((float)sof_tab.recommended_continuous_discharge, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_RecDischargeCurrent_Peak:
                /* Check limits */
                canData = cans_checkLimits((float)sof_tab.recommended_peak_discharge, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            default:
                break;
        }
    }
    return 0;
}


static uint32_t cans_getMaxAllowedPower(uint32_t sigIdx, void *value) {
    if (value != NULL_PTR) {
        switch (sigIdx) {
            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}


static uint32_t cans_getminmaxvolt(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_MINMAX_s minmax_volt_tab;
    float canData = 0;

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_Cellvolt_mean:
                /* First signal that is called */
                DB_ReadBlock(&minmax_volt_tab, DATA_BLOCK_ID_MINMAX);

                /* Check limits */
                canData = cans_checkLimits((float)minmax_volt_tab.voltage_mean, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_Cellvolt_min:
                /* Check limits */
                canData = cans_checkLimits((float)minmax_volt_tab.voltage_min, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_Cellvolt_max:
                /* Check limits */
                canData = cans_checkLimits((float)minmax_volt_tab.voltage_max, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_ModNumber_volt_min:
                /* Check limits */
                canData = cans_checkLimits((float)minmax_volt_tab.voltage_module_number_min, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            case CAN0_SIG_ModNumber_volt_max:
                /* Check limits */
                canData = cans_checkLimits((float)minmax_volt_tab.voltage_module_number_max, sigIdx);
                /* Apply offset and factor */
                *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
                break;

            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}

uint32_t cans_getminmaxtemp(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_MINMAX_s minmax_temp_tab;
    float canData = 0;

    if (value != NULL_PTR) {
        switch (sigIdx) {
        case CAN0_SIG_Celltemp_mean:
            /* First signal that is called */
            DB_ReadBlock(&minmax_temp_tab, DATA_BLOCK_ID_MINMAX);

            /*  Check limits */
            canData = cans_checkLimits((float)minmax_temp_tab.temperature_mean, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

        case CAN0_SIG_Celltemp_min:
            /* Check limits */
            canData = cans_checkLimits((float)minmax_temp_tab.temperature_min, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

        case CAN0_SIG_Celltemp_max:
            /* Check limits */
            canData = cans_checkLimits((float)minmax_temp_tab.temperature_max, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

        case CAN0_SIG_ModNumber_temp_min:
            /* Check limits */
            canData = cans_checkLimits((float)minmax_temp_tab.temperature_module_number_min, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

        case CAN0_SIG_ModNumber_temp_max:
            /* Check limits */
            canData = cans_checkLimits((float)minmax_temp_tab.temperature_module_number_max, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

        default:
            *(uint32_t *)value = 0;
            break;
        }
    }
    return 0;
}


#ifdef CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED
uint32_t cans_gettriggercurrent(uint32_t sigIdx, void *value) {
    *(uint32_t *)value = 0x00FFFF31;
    return 0;
}
#endif /* CURRENT_SENSOR_ISABELLENHUETTE_TRIGGERED */


static uint32_t cans_getpower(uint32_t sigIdx, void *value) {
    uint32_t retVal = 0;
    float canData = 0;
    static DATA_BLOCK_MOVING_AVERAGE_s powMovMean_tab;

    if (value != NULL_PTR) {
       switch (sigIdx) {
           case CAN0_SIG_MovAverage_Power_1s:
               /* first signal to call function */
               DB_ReadBlock(&powMovMean_tab, DATA_BLOCK_ID_MOV_AVERAGE);
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_1s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Power_5s:
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_5s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Power_10s:
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_10s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Power_30s:
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_30s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Power_60s:
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_60s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Power_config:
               /* Check limits */
               canData = cans_checkLimits((float)powMovMean_tab.movAverage_power_config, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           default:
               *(uint32_t *)value = 0;
               break;
       }
    }
    return retVal;
}

static uint32_t cans_getcurr(uint32_t sigIdx, void *value) {
    uint32_t retVal = 0;
    float canData = 0;
    static DATA_BLOCK_MOVING_AVERAGE_s curMovMean_tab;

    if (value != NULL_PTR) {
       switch (sigIdx) {
           case CAN0_SIG_MovAverage_Current_1s:
               /* first signal to call function */
               DB_ReadBlock(&curMovMean_tab, DATA_BLOCK_ID_MOV_AVERAGE);
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_1s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Current_5s:
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_5s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Current_10s:
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_10s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Current_30s:
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_30s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Current_60s:
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_60s, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_MovAverage_Current_config:
               /* Check limits */
               canData = cans_checkLimits((float)curMovMean_tab.movAverage_current_config, sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           default:
                   *(uint32_t *)value = 0;
                   break;
       }
    }
    return retVal;
}


static uint32_t cans_getPackVoltage(uint32_t sigIdx, void *value) {
    uint32_t retVal = 0;
    float canData = 0;
    static DATA_BLOCK_CURRENT_SENSOR_s packVolt_tab;

    if (value != NULL_PTR) {
       switch (sigIdx) {
           case CAN0_SIG_PackVolt_Battery:
               /* first signal to call function */
               DB_ReadBlock(&packVolt_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
               /* Check limits */
               canData = cans_checkLimits((float)packVolt_tab.voltage[0], sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           case CAN0_SIG_PackVolt_PowerNet:
               /* first signal to call function */
               DB_ReadBlock(&packVolt_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
               /* Check limits */
               canData = cans_checkLimits((float)packVolt_tab.voltage[2], sigIdx);
               /* Apply offset and factor */
               *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
               break;

           default:
               break;
       }
    }
    return retVal;
}


static uint32_t cans_setcurr(uint32_t sigIdx, void *value) {
    int32_t currentValue;
    int32_t temperatureValue;
    int32_t powerValue;
    int32_t currentcounterValue;
    int32_t energycounterValue;
    int32_t voltageValue[3];
    uint32_t idx = 0;
    uint8_t dummy = 0;

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_IVT_Current_Status:
            case CAN0_SIG_IVT_Voltage_1_Status:
            case CAN0_SIG_IVT_Voltage_2_Status:
            case CAN0_SIG_IVT_Voltage_3_Status:
            case CAN0_SIG_IVT_Temperature_Status:
            case CAN0_SIG_IVT_Power_Status:
            case CAN0_SIG_IVT_CC_Status:
            case CAN0_SIG_IVT_EC_Status:
                dummy = *(uint32_t *)value & 0x000000FF;
                dummy &= 0xF0;   /* only high nibble contains diag info */
                if ((dummy & 0x10) == TRUE) {
                    /* Overcurrent detected. This feature is currently not supported. */
                }
                if ((dummy & 0x20) == TRUE) {
                    if (sigIdx == CAN0_SIG_IVT_Current_Status) {
                        cans_current_tab.state_current = 1;
                    } else if (sigIdx == CAN0_SIG_IVT_Voltage_1_Status || sigIdx == CAN0_SIG_IVT_Voltage_2_Status || sigIdx == CAN0_SIG_IVT_Voltage_3_Status) {
                        cans_current_tab.state_voltage = 1;
                    } else if (sigIdx == CAN0_SIG_IVT_Temperature_Status) {
                        cans_current_tab.state_temperature = 1;
                    } else if (sigIdx == CAN0_SIG_IVT_Power_Status) {
                        cans_current_tab.state_power = 1;
                    } else if (sigIdx == CAN0_SIG_IVT_CC_Status) {
                        cans_current_tab.state_cc = 1;
                    } else {
                        cans_current_tab.state_ec = 1;
                    }
                } else {
                    cans_current_tab.state_current = 0;
                    cans_current_tab.state_voltage = 0;
                    cans_current_tab.state_temperature = 0;
                    cans_current_tab.state_power = 0;
                    cans_current_tab.state_cc = 0;
                    cans_current_tab.state_ec = 0;
                }
                if ((dummy & 0x40) == TRUE || (dummy & 0x80) == TRUE) {
                    cans_current_tab.state_current = 1;
                    cans_current_tab.state_voltage = 1;
                    cans_current_tab.state_temperature = 1;
                    cans_current_tab.state_power = 1;
                    cans_current_tab.state_cc = 1;
                    cans_current_tab.state_ec = 1;
                }

                break;

                case CAN0_SIG_IVT_Current_Measurement:
                /* case CAN1_SIG_ISENS0_I_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    currentValue = *(int32_t*)value;
                    cans_current_tab.current = (currentValue);
                    cans_current_tab.newCurrent++;
                    cans_current_tab.previous_timestamp_cur = cans_current_tab.timestamp_cur;
                    cans_current_tab.timestamp_cur = OS_getOSSysTick();
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_Voltage_1_Measurement:
                /* case CAN1_SIG_ISENS1_U1_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    idx = 0;
                    voltageValue[idx] = *(int32_t*)value;
                    cans_current_tab.voltage[idx] = (float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_Voltage_2_Measurement:
                /* case CAN1_SIG_ISENS2_U2_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    idx = 1;
                    voltageValue[idx] = *(int32_t*)value;
                    cans_current_tab.voltage[idx] = (float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_Voltage_3_Measurement:
                /* case CAN1_SIG_ISENS3_U3_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    idx = 2;
                    voltageValue[idx] = *(int32_t*)value;
                    cans_current_tab.voltage[idx]=(float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_Temperature_Measurement:
                /* case CAN1_SIG_ISENS4_T_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    temperatureValue = *(int32_t*)value;
                    cans_current_tab.temperature = (float)(temperatureValue)*cans_CAN0_signals_rx[sigIdx].factor;
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_Power_Measurement:
                /* case CAN1_SIG_ISENS5_P_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    powerValue = *(int32_t*)value;
                    cans_current_tab.power = (float)(powerValue);
                    cans_current_tab.newPower++;
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_CC_Measurement:
                /* case CAN1_SIG_ISENS6_CC_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    currentcounterValue = *(int32_t*)value;
                    cans_current_tab.previous_timestamp_cc = cans_current_tab.timestamp_cc;
                    cans_current_tab.timestamp_cc = OS_getOSSysTick();
                    cans_current_tab.current_counter = (float)(currentcounterValue);
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
                case CAN0_SIG_IVT_EC_Measurement:
                /* case CAN1_SIG_ISENS7_EC_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct */
                    energycounterValue = *(int32_t*)value;
                    cans_current_tab.energy_counter = (float)(energycounterValue);
                    DB_WriteBlock(&cans_current_tab, DATA_BLOCK_ID_CURRENT_SENSOR);
                    break;
        }
    }
    return 0;
}


uint32_t cans_setstaterequest(uint32_t sigIdx, void *value) {
    DATA_BLOCK_STATEREQUEST_s staterequest_tab;
    uint8_t staterequest;

    DB_ReadBlock(&staterequest_tab, DATA_BLOCK_ID_STATEREQUEST);

    if (value != NULL_PTR) {
        if (sigIdx == CAN0_SIG_ReceiveStateRequest) {
            staterequest = *(uint8_t *)value;
            staterequest_tab.previous_state_request = staterequest_tab.state_request;
            staterequest_tab.state_request = staterequest;
            if ((staterequest_tab.state_request != staterequest_tab.previous_state_request)|| \
                    (OS_getOSSysTick()- staterequest_tab.timestamp) > 3000) {
                staterequest_tab.state_request_pending = staterequest;
            }
            staterequest_tab.state++;
            DB_WriteBlock(&staterequest_tab, DATA_BLOCK_ID_STATEREQUEST);
        }
    }
    return 0;
}


uint32_t cans_getisoguard(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_ISOMETER_s isoguard_tab;
    float canData = 0;

    if (value != NULL_PTR) {
        switch (sigIdx) {
            case CAN0_SIG_InsulationStatus:
            /* First signal call */
            DB_ReadBlock(&isoguard_tab, DATA_BLOCK_ID_ISOGUARD);

            /* Check limits */
            canData = cans_checkLimits((float)isoguard_tab.state, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

            case CAN0_SIG_InsulationValue:
            /* Check limits */
            canData = cans_checkLimits((float)isoguard_tab.resistance_kOhm, sigIdx);
            /* Apply offset and factor */
            *(uint32_t *)value = (uint32_t)((canData + cans_CAN0_signals_tx[sigIdx].offset) * cans_CAN0_signals_tx[sigIdx].factor);
            break;

            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}


uint32_t cans_setdebug(uint32_t sigIdx, void *value) {
    uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    data[0] = *(uint64_t *)value & 0x00000000000000FF;
    data[1] = (*(uint64_t *)value & 0x000000000000FF00) >> 8;
    data[2] = (*(uint64_t *)value & 0x0000000000FF0000) >> 16;
    data[3] = (*(uint64_t *)value & 0x00000000FF000000) >> 24;
    data[4] = (*(uint64_t *)value & 0x000000FF00000000) >> 32;
    data[5] = (*(uint64_t *)value & 0x0000FF0000000000) >> 40;
    data[6] = (*(uint64_t *)value & 0x00FF000000000000) >> 48;
    data[7] = (*(uint64_t *)value & 0xFF00000000000000) >> 56;


    if (value != NULL_PTR) {
        switch (data[0]) {
            case 0x0B:  /* Set Soc directly with value. Unit in CAN message: 0.01 percent --> range 0...10000 means 0%Soc...100%Soc */
                SOC_SetValue(((float)((data[1]) << 8 | (data[2])))/100.0f, ((float)((data[1]) << 8 | (data[2])))/100.0f, ((float)((data[1]) << 8 | (data[2])))/100.0f); /* divide by 100 to get SOC between 0 and 100 */
                break;
            case 0x0E:  /* debug Message for Balancing on pack level */
                if (data[1] == 0) {
                    BAL_SetStateRequest(BAL_STATE_GLOBAL_DISABLE_REQUEST);
                } else {
                    BAL_SetStateRequest(BAL_STATE_GLOBAL_ENABLE_REQUEST);
                }
                break;
            case 0xAA:
                DIAG_Handler(DIAG_CH_DEEP_DISCHARGE_DETECTED, DIAG_EVENT_OK, 0);
                break;

            default:
                break;
        }
    }
    return 0;
}


uint32_t cans_setSWversion(uint32_t sigIdx, void *value) {
    SYS_SendBootMessage(0);
    return 0;
}


float cans_checkLimits(float value, uint32_t sigIdx) {
    float retVal = value;

    if (value < cans_CAN0_signals_tx[sigIdx].min)
        retVal = cans_CAN0_signals_tx[sigIdx].min;

    if (value > cans_CAN0_signals_tx[sigIdx].max)
        retVal = cans_CAN0_signals_tx[sigIdx].max;

    return retVal;
}

/*================== Extern Function Implementations ========================*/
