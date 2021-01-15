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
 * @file    interlock_cfg.h
 * @author  foxBMS Team
 * @date    23.09.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  ILCK
 *
 * @brief   Header for the configuration for the driver for the interlock
 *
 */

#ifndef INTERLOCK_CFG_H_
#define INTERLOCK_CFG_H_

/*================== Includes =============================================*/
#include "general.h"

#include "io.h"

/*================== Macros and Definitions ===============================*/

/*
 * The number of defines per contactor must be the same as the length
 *  of the array ilck_contactors_cfg in contactor_cfg.c
 * Every contactor consists of 1 control pin and 1 feedback pin
 * counting together as 1 contactor.
 * E.g. if you have 1 contactor your define has to be:
 *      #define ILCK_INTERLOCK_CONTROL       PIN_MCU_0_INTERLOCK_CONTROL
 *      #define ILCK_INTERLOCK_FEEDBACK      PIN_MCU_0_INTERLOCK_FEEDBACK
 */
#define ILCK_INTERLOCK_CONTROL                  IO_PIN_INTERLOCK_CONTROL
/**
 * Defines the pin where interlock feedback pin is connected to the mcu
 */
#define ILCK_INTERLOCK_FEEDBACK                 IO_PIN_INTERLOCK_FEEDBACK


/**
 * Symbolic names for current flow direction in the battery
 */
typedef enum {
    ILCK_CURRENT_CHARGE     = 0,    /*!<            */
    ILCK_CURRENT_DISCHARGE      = 1,    /*!<       */
} ILCK_CURRENT_DIRECTION_e;



/**
 * ILCK statemachine short time definition in ms
 */

#define ILCK_STATEMACH_SHORTTIME_MS     1


/*================== Constant and Variable Definitions ====================*/

/**
 * Symbolic names for contactors' possible states
 */
typedef enum {
    ILCK_SWITCH_OFF     = 0,    /*!< Contactor off         --> Contactor is open           */
    ILCK_SWITCH_ON      = 1,    /*!< Contactor on          --> Contactor is closed         */
    ILCK_SWITCH_UNDEF   = 2,    /*!< Contactor undefined   --> Contactor state not known   */
} ILCK_ELECTRICAL_STATE_TYPE_s;

/**
 * Symbolic names for the contactors, which are used in
 * the contactor_config[] array
 */
typedef enum {
    ILCK_MAIN_PLUS      = 0,    /*!< Main contactor in the positive path of the powerline      */
    ILCK_PRECHARGE_PLUS = 1,    /*!< Precharge contactor in the positive path of the powerline */
    ILCK_MAIN_MINUS     = 2,    /*!< Main contactor in the negative path of the powerline      */
} ILCK_NAMES_e;

/**
 * Symbolic names defining the electric behavior of the contactor
 */
typedef enum {
    ILCK_FEEDBACK_NORMALLY_OPEN     = 0,    /*!< Feedback line of a contactor is normally open      */
    ILCK_FEEDBACK_NORMALLY_CLOSED   = 1,    /*!< Feedback line of a contactor is normally closed    */
    ILCK_FEEDBACK_TYPE_DONT_CARE    = 0xFF  /*!< Feedback line of the contactor is not used         */
} ILCK_FEEDBACK_TYPE_e;

typedef struct {
    ILCK_ELECTRICAL_STATE_TYPE_s set;
    ILCK_ELECTRICAL_STATE_TYPE_s feedback;
} ILCK_ELECTRICAL_STATE_s;

typedef struct {
    IO_PORTS_e control_pin;
    IO_PORTS_e feedback_pin;
    ILCK_FEEDBACK_TYPE_e feedback_pin_type;
} ILCK_CONFIG_s;

extern ILCK_CONFIG_s ilck_interlock_config;
extern ILCK_ELECTRICAL_STATE_s ilck_interlock_state;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* INTERLOCK_CFG_H_ */
