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
 * @file    database_cfg.c
 * @author  foxBMS Team
 * @date    18.08.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  DATA
 *
 * @brief   Database configuration
 *
 * Configuration of database module
 *
 */

/*================== Includes ===============================================*/
#include "database_cfg.h"

/*================== Macros and Definitions =================================*/

/*================== Static Constant and Variable Definitions ===============*/
/**
 * data block: cell voltage
 */
static DATA_BLOCK_CELLVOLTAGE_s data_block_cellvoltage;

/**
 * data block: cell temperature
 */
static DATA_BLOCK_CELLTEMPERATURE_s data_block_celltemperature;

/**
 * data block: sox
 */
static DATA_BLOCK_SOX_s data_block_sox;

/**
 * data block: sof
 */
static DATA_BLOCK_SOF_s data_block_sof;

/**
 * data block: balancing control
 */
static DATA_BLOCK_BALANCING_CONTROL_s data_block_control_balancing;

/**
 * data block: balancing feedback
 */
static DATA_BLOCK_BALANCING_FEEDBACK_s data_block_feedback_balancing;

/**
 * data block: current measurement
 */
static DATA_BLOCK_CURRENT_SENSOR_s data_block_curr_sensor;

/**
 * data block: ADC
 */
static DATA_BLOCK_HW_INFO_s data_block_hwinfo;

/**
 * data block: can state request
 */
static DATA_BLOCK_STATEREQUEST_s data_block_staterequest;

/**
 * data block: LTC minimum and maximum values
 */
static DATA_BLOCK_MINMAX_s data_block_minmax;

/**
 * data block: isometer measurement
 */
static DATA_BLOCK_ISOMETER_s data_block_isometer;

/**
 * data block: error flags
 */
static DATA_BLOCK_ERRORSTATE_s data_block_errors;

/**
 * data block: maximum safety limit violations
 */
static DATA_BLOCK_MSL_FLAG_s data_block_MSL;

/**
 * data block: recommended safety limit violations
 */
static DATA_BLOCK_RSL_FLAG_s data_block_RSL;

/**
 * data block: maximum operating limit violations
 */
static DATA_BLOCK_MOL_FLAG_s data_block_MOL;

/**
 * data block: moving mean current and power
 */
static DATA_BLOCK_MOVING_AVERAGE_s data_block_mov_average;

/**
 * data block: contactor feedback
 */
static DATA_BLOCK_CONTFEEDBACK_s data_block_contfeedback;

/**
 * data block: interlock feedback
 */
static DATA_BLOCK_ILCKFEEDBACK_s data_block_ilckfeedback;

/**
 * data block: slave control
 */
static DATA_BLOCK_SLAVE_CONTROL_s data_block_slave_control;

/**
 * data block: system state
 */
static DATA_BLOCK_SYSTEMSTATE_s data_block_systemstate;

/**
 * data block: open wire check
 */
static DATA_BLOCK_OPENWIRE_s data_block_open_wire;

/**
 * data block: LTC diagnosis values
 */
static DATA_BLOCK_LTC_DEVICE_PARAMETER_s data_block_ltc_diagnosis;

/**
 * data block: LTC ADC accuracy verification
 */
static DATA_BLOCK_LTC_ADC_ACCURACY_s data_block_ltc_adc_accuracy;

/**
 * data block: LTC ADC accuracy verification
 */
static DATA_BLOCK_ALLGPIOVOLTAGE_s data_block_ltc_allgpiovoltages;

/**
 * @brief channel configuration of database (data blocks)
 *
 * all data block managed by database are listed here (address,size,consistency type)
 *
 */
static DATA_BASE_HEADER_s  data_base_header[] = {
    {
        (void*)(&data_block_cellvoltage),
        sizeof(DATA_BLOCK_CELLVOLTAGE_s)
    },
    {
        (void*)(&data_block_celltemperature),
        sizeof(DATA_BLOCK_CELLTEMPERATURE_s)
    },
    {
        (void*)(&data_block_sox),
        sizeof(DATA_BLOCK_SOX_s)
    },
    {
        (void*)(&data_block_control_balancing),
        sizeof(DATA_BLOCK_BALANCING_CONTROL_s)
    },
    {
        (void*)(&data_block_feedback_balancing),
        sizeof(DATA_BLOCK_BALANCING_FEEDBACK_s)
    },
    {
        (void*)(&data_block_curr_sensor),
        sizeof(DATA_BLOCK_CURRENT_SENSOR_s)
    },
    {
        (void*)(&data_block_hwinfo),
        sizeof(DATA_BLOCK_HW_INFO_s)
    },
    {
        (void*)(&data_block_staterequest),
        sizeof(DATA_BLOCK_STATEREQUEST_s)
    },
    {
        (void*)(&data_block_minmax),
        sizeof(DATA_BLOCK_MINMAX_s)
    },
    {
        (void*)(&data_block_isometer),
        sizeof(DATA_BLOCK_ISOMETER_s)
    },
    {
        (void*)(&data_block_slave_control),
        sizeof(DATA_BLOCK_SLAVE_CONTROL_s)
    },
    {
        (void*)(&data_block_open_wire),
        sizeof(DATA_BLOCK_OPENWIRE_s)
    },
    {
        (void*)(&data_block_ltc_diagnosis),
        sizeof(DATA_BLOCK_LTC_DEVICE_PARAMETER_s)
    },
    {
        (void*)(&data_block_ltc_adc_accuracy),
        sizeof(DATA_BLOCK_LTC_ADC_ACCURACY_s)
    },
    {
        (void*)(&data_block_errors),
        sizeof(DATA_BLOCK_ERRORSTATE_s)
    },
    {
        (void*)(&data_block_MSL),
        sizeof(DATA_BLOCK_MSL_FLAG_s)
    },
    {
        (void*)(&data_block_RSL),
        sizeof(DATA_BLOCK_RSL_FLAG_s)
    },
    {
        (void*)(&data_block_MOL),
        sizeof(DATA_BLOCK_MOL_FLAG_s)
    },
    {
        (void*)(&data_block_mov_average),
        sizeof(DATA_BLOCK_MOVING_AVERAGE_s)
    },
    {
        (void*)(&data_block_contfeedback),
        sizeof(DATA_BLOCK_CONTFEEDBACK_s)
    },
    {
        (void*)(&data_block_ilckfeedback),
        sizeof(DATA_BLOCK_ILCKFEEDBACK_s)
    },
    {
        (void*)(&data_block_systemstate),
        sizeof(DATA_BLOCK_SYSTEMSTATE_s)
    },
    {
        (void*)(&data_block_sof),
        sizeof(DATA_BLOCK_SOF_s)
    },
    {
        (void*)(&data_block_ltc_allgpiovoltages),
        sizeof(DATA_BLOCK_ALLGPIOVOLTAGE_s)
    },
};


/*================== Extern Constant and Variable Definitions ===============*/

/**
 * @brief device configuration of database
 *
 * all attributes of device configuration are listed here (pointer to channel list, number of channels)
 */
const DATA_BASE_HEADER_DEV_s data_base_dev = {
    .nr_of_blockheader  = sizeof(data_base_header)/sizeof(DATA_BASE_HEADER_s),    /* number of blocks (and block headers) */
    .blockheaderptr     = &data_base_header[0],
};

/*================== Static Function Prototypes =============================*/

/*================== Static Function Implementations ========================*/

/*================== Extern Function Implementations ========================*/
