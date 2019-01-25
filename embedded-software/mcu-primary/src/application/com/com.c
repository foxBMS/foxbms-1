/**
 *
 * @copyright &copy; 2010 - 2019, Fraunhofer-Gesellschaft zur Foerderung der
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
 * @file    com.c
 * @author  foxBMS Team
 * @date    28.08.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  COM
 *
 * @brief   Driver for the COM module.
 *
 * Generic communication module which handles pre-defined user input and output.
 * In its current state it only uses UART/RS232 for communication
 *
 */



/*================== Includes =============================================*/
#include "com.h"

#if BUILD_MODULE_ENABLE_COM == 1
#include "contactor.h"
#include "mcu.h"
#include "nvram_cfg.h"
#include "os.h"
#include "sox.h"
#include <string.h>
#include "rtc.h"
#include "uart.h"
#include "stdio.h"

/*================== Macros and Definitions ===============================*/
#define TESTMODE_TIMEOUT 30000

#define com_receivedbyte     uart_com_receivedbyte
#define com_receive_slot     uart_com_receive_slot

/*================== Constant and Variable Definitions ====================*/
uint8_t printHelp = 0;

static uint8_t com_testmode_enabled = 0;
static uint32_t com_tickcount = 0;

static RTC_Time_s com_Time;
static RTC_Date_s com_Date;

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/
extern int _write(int fd, char *ptr, int len) {
  /* Write "len" of char from "ptr"
   * Return number of char written.
   * Check if file descriptor is in fact stdout
   * not neccessary but "good practice". */

    if (fd == 1) {
        UART_uartWrite_intbuf((uint8_t*)ptr, len);
    }

    return len;
}
void COM_uartWrite(const uint8_t *source) {
    OS_TaskEnter_Critical();
    UART_vWrite(source);
    OS_TaskExit_Critical();
}

void UART_uartWrite_intbuf(const uint8_t *source, uint16_t length) {
    OS_TaskEnter_Critical();
    UART_vWrite_intbuf(source, length);
    OS_TaskExit_Critical();
}

void COM_printTimeAndDate(void) {
    /* Get time and date */
    RTC_getTime(&com_Time);
    RTC_getDate(&com_Date);

    DEBUG_PRINTF(("Date and Time: 20%02d.%02d.%02d - %02d:%02d:%02d\r\n",
      com_Date.Year, com_Date.Month, com_Date.Date, com_Time.Hours,
      com_Time.Minutes, com_Time.Seconds));
}


void COM_StartupInfo(void) {
    /* Get RCC Core Reset Register */
    uint32_t tmp = main_state.CSR;
    DEBUG_PRINTF(("........................................\r\n"));
    DEBUG_PRINTF(("System starting...\r\n"));
    DEBUG_PRINTF(("RCC Core Reset Register: 0x%x\r\n", (unsigned int)tmp));

    /* Print time and date */
    COM_printTimeAndDate();


    DEBUG_PRINTF(("Runtime: %02dh %02dm %02ds\r\n", os_timer.Timer_h,
      os_timer.Timer_min, os_timer.Timer_sec));
}

void COM_printHelpCommand(void) {
    static uint8_t cnt = 0;

    if (printHelp == 0)
        return;

    switch (cnt) {
        case 0:
            DEBUG_PRINTF(("\r\nFollowing commands are available: :\r\n\r\n"));

            DEBUG_PRINTF(("====================  ========================================================================================================\r\n"));
            DEBUG_PRINTF(("Command               Description\r\n"));
            DEBUG_PRINTF(("====================  ========================================================================================================\r\n"));
            break;

        case 1:
            DEBUG_PRINTF(("help                  get available command list\r\n"));
            DEBUG_PRINTF(("gettime               get system time\r\n"));
            DEBUG_PRINTF(("getruntime            get runtime since last reset\r\n"));
            DEBUG_PRINTF(("getoperatingtime      get total operating time\r\n"));
            break;

        case 2:
            DEBUG_PRINTF(("printdiaginfo         get diagnosis entries of DIAG module (entries can only be printed once)\r\n"));
            DEBUG_PRINTF(("printcontactorinfo    get contactor information (number of switches/hard switches) (entries can only be printed once)\r\n"));
            DEBUG_PRINTF(("teston                enable testmode, testmode will be disabled after a predefined timeout of 30s when no new command is sent\r\n"));
            break;

        case 3:
            DEBUG_PRINTF(("====================  ========================================================================================================\r\n"));

            DEBUG_PRINTF(("\r\nFollowing commands are only available during enabled testmode:\r\n\r\n"));

            DEBUG_PRINTF(("==========================  ==========================================================================================\r\n"));
            break;

        case 4:
            DEBUG_PRINTF(("Command                     Description\r\n"));
            DEBUG_PRINTF(("==========================  ==========================================================================================\r\n"));
            DEBUG_PRINTF(("testoff                     disable testmode\r\n"));
            break;

        case 5:
            DEBUG_PRINTF(("settime YY MM DD HH MM SS   set mcu time and date (YY-year, MM-month, DD-date, HH-hours, MM-minutes, SS-seconds)\r\n"));
            DEBUG_PRINTF(("reset                       enforces complete software reset using HAL_NVIC_SystemReset()\r\n"));
            DEBUG_PRINTF(("watchdogtest                performs watchdog test, watchdog timeout results in system reset (predefined 1s)\r\n"));
            break;

        case 6:
            DEBUG_PRINTF(("setsoc xxx.xxx              set SOC value (000.000%% - 100.000%%)\r\n"));
            DEBUG_PRINTF(("ceX                         enables contactor number X (only possible if BMS is in no error state)\r\n"));
            DEBUG_PRINTF(("cdX                         disables contactor number X (only possible if BMS is in no error state)\r\n"));
            break;

        case 7:
            DEBUG_PRINTF(("==========================  ==========================================================================================\r\n"));
            break;

        default:
            break;
    }

    cnt++;

    if (cnt == 8) {
        printHelp = 0;
        cnt = 0;
    }
}


void COM_Decoder(void) {
    /* Command Received - Replace Carrier Return with null character */
    if (com_receivedbyte[com_receive_slot - 1] == '\r') {
        com_receivedbyte[com_receive_slot - 1] = '\0';

        /* ENABLE TESTMODE */
        if (strcmp(com_receivedbyte, "teston") == 0) {
            /* Set timeout */
            com_tickcount = OS_getOSSysTick();

            if (!com_testmode_enabled) {
                /* Enable testmode */
                com_testmode_enabled = 1;

                DEBUG_PRINTF(("Testmode enabled!\r\n"));
            } else {
                /* Testmode already enabled */
                DEBUG_PRINTF(("Testmode already enabled!\r\n"));
            }

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* PRINT help command */
        if (strcmp(com_receivedbyte, "help") == 0) {
            /* Print contactor info */
            printHelp = 1;

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* PRINT CONTACTOR INFO */
        if (strcmp(com_receivedbyte, "printcontactorinfo") == 0) {
            /* Print contactor info */
            DIAG_PrintContactorInfo();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* PRINT DIAG INFO */
        if (strcmp(com_receivedbyte, "printdiaginfo") == 0) {
            /* Print diag info */
            DIAG_PrintErrors();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* GETTIME */
        if (strcmp(com_receivedbyte, "gettime") == 0) {
            /* Print time and date */
            COM_printTimeAndDate();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* Get runtime */
        if (strcmp(com_receivedbyte, "getruntime") == 0) {
            /* Print runtime */
                DEBUG_PRINTF(("Runtime: %02dh %02dm %02ds\r\n", os_timer.Timer_h,
                    os_timer.Timer_min, os_timer.Timer_sec));

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* Get operating time */
        if (strcmp(com_receivedbyte, "getoperatingtime") == 0) {
            DEBUG_PRINTF(("Operating time: %03dd %02dh %02dm %02ds\r\n",
                bkpsram_op_hours.Timer_d, bkpsram_op_hours.Timer_h,
                bkpsram_op_hours.Timer_min, bkpsram_op_hours.Timer_sec));

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            return;
        }

        /* Command received and testmode enabled */
        if (com_testmode_enabled) {
            /* DISABLE TESTMODE */
            if (strcmp(com_receivedbyte, "testoff") == 0) {
                com_testmode_enabled = 0;
                DEBUG_PRINTF(("Testmode disabled on request!\r\n"));

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;
                return;
            }

            /* SETTIME */
/*             if (strcmp(com_receivedbyte, "settime") == 0) {*/
            if (com_receivedbyte[0] == 's' && com_receivedbyte[1] == 'e' && com_receivedbyte[2] == 't'
                    && com_receivedbyte[3] == 't' && com_receivedbyte[4] == 'i' && com_receivedbyte[5] == 'm'
                            && com_receivedbyte[6] == 'e') {
                /* Command length = 24, +1 for \0, +1 cause index always one step ahead */
                if (com_receive_slot == 26) {
                    /* Set time and date only if command length is like expected */
                    uint8_t value, tmp10, tmp1, retVal = 0;

                    /* Calculate year */
                    tmp10 = com_receivedbyte[8] - '0';
                    tmp1 = com_receivedbyte[9] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_YEAR(value))  /* Check if value is valid */
                        com_Date.Year = value;
                    else
                        retVal = 0xFF;

                    /* Calculate month */
                    tmp10 = com_receivedbyte[11] - '0';
                    tmp1 = com_receivedbyte[12] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_MONTH(value))  /* Check if value is valid */
                        com_Date.Month = value;
                    else
                        retVal = 0xFF;

                    /* Calculate day */
                    tmp10 = com_receivedbyte[14] - '0';
                    tmp1 = com_receivedbyte[15] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_DATE(value))  /* Check if value is valid */
                        com_Date.Date = value;
                    else
                        retVal = 0xFF;

                    /* Calculate hours */
                    tmp10 = com_receivedbyte[17] - '0';
                    tmp1 = com_receivedbyte[18] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_HOUR24(value)) {
                        /* Check if value is valid */
                        com_Time.Hours = value;
                    } else {
                        retVal = 0xFF;
                    }

                    /* Calculate minutes */
                    tmp10 = com_receivedbyte[20] - '0';
                    tmp1 = com_receivedbyte[21] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_MINUTES(value)) {
                        /* Check if value is valid */
                        com_Time.Minutes = value;
                    } else {
                        retVal = 0xFF;
                    }

                    /* Calculate seconds */
                    tmp10 = com_receivedbyte[23] - '0';
                    tmp1 = com_receivedbyte[24] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if (IS_RTC_SECONDS(value)) {
                        /* Check if value is valid */
                        com_Time.Seconds = value;
                    } else {
                        retVal = 0xFF;
                    }

                    if (retVal == 0) {
                        /* Set time and date */
                        RTC_setTime(&com_Time);
                        RTC_setDate(&com_Date);

                        DEBUG_PRINTF(("Time and date set!\r\n"));
                    } else {
                        /* Print error message */
                        DEBUG_PRINTF(("Invalid parameter!\r\n%.25s\r\n", com_receivedbyte));
                    }

                } else {
                    /* Print error message */
                    DEBUG_PRINTF(("Invalid parameter length!\r\n"));
                    DEBUG_PRINTF(("%.*s\r\n", com_receive_slot, com_receivedbyte));
                }

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;

                /* Reset timeout to TESTMODE_TIMEOUT */
                com_tickcount = OS_getOSSysTick();

                return;
            }

            /* RESET */
            if (strcmp(com_receivedbyte, "reset") == 0) {
                DEBUG_PRINTF(("Software reset!\r\n"));

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;

                HAL_NVIC_SystemReset();
                return;
            }

            if (strcmp(com_receivedbyte, "watchdogtest") == 0) {
                DEBUG_PRINTF(("WDG"));
              /*  DEBUG_PRINTF(("\r\n"); */


                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;
                OS_taskDelay(1);

                /* disable global interrupt, no saving of interrupt status because of follwing reset */
                (void)MCU_DisableINT();

                while (1) {
                    /* stop system and wait for watchdog reset*/
                }
                return;
            }

            /* Set soc */
            if (com_receivedbyte[0] == 's' && com_receivedbyte[1] == 'e' && com_receivedbyte[2] == 't'
                                && com_receivedbyte[3] == 's' && com_receivedbyte[4] == 'o' && com_receivedbyte[5] == 'c') {
                /* Set SOC */
                /* Command length = 14, +1 for \0 and +1 cause index always one step ahead "setsoc xxx.xxx"*/
                if (com_receive_slot == 15) {
                    /* Set time and date only if command length is like expected */
                    uint8_t tmp100 = 0, tmp10 = 0, tmp1 = 0;
                    float value = 0;

                    /* Calculate year */
                    tmp100 = com_receivedbyte[7] - '0';
                    tmp10 = com_receivedbyte[8] - '0';
                    tmp1 = com_receivedbyte[9] - '0';
                    value = (tmp100 * 100) + (tmp10 * 10) + tmp1;

                    tmp1 = com_receivedbyte[11] - '0';
                    tmp10 = com_receivedbyte[12] - '0';
                    tmp100 = com_receivedbyte[13] - '0';
                    value = value + (tmp1 * 0.1) + (tmp10 * 0.01) + (tmp100 * 0.001);

                    if (0 <= value && value <= 100)
                        SOC_SetValue(value, value, value);
                }
            }


            /* SWITCH CONTACTORS */
            switch (com_receivedbyte[0]) {
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
            case 'c':
                {} /* added to get rid of compile error */
                /* Convert ascii number to int */
                uint8_t contNumber;
                contNumber = com_receivedbyte[2] - '0';

                /* Switch contactor */
                if ((0 <= contNumber) && (contNumber <= BS_NR_OF_CONTACTORS)) {
                    /* Close contactor */
                    if (com_receivedbyte[1] == 'e') {
                        DEBUG_PRINTF(("Contactor %d", contNumber));

                        CONT_SetContactorState(contNumber, CONT_SWITCH_ON);

                        DEBUG_PRINTF((" enabled\r\n"));
                    } else if (com_receivedbyte[1] == 'd') {
                        /* Open contactor */
                        DEBUG_PRINTF(("Contactor %d", contNumber));

                        CONT_SetContactorState(contNumber, CONT_SWITCH_OFF);

                        DEBUG_PRINTF((" disabled\r\n"));
                    } else {
                        /* Invalid command */
                        DEBUG_PRINTF(("Invalid command!\r\n"));
                    }
                } else {
                    /* Invalid contactor number */
                    uint8_t a = BS_NR_OF_CONTACTORS;
                    DEBUG_PRINTF(("Invalid contactor number! Only %d contactors are connected! \r\n", a));
                }
                break;
#endif
            default:
                DEBUG_PRINTF(("Invalid command!\r\n%s\r\n", com_receivedbyte));
                break;
            }

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = OS_getOSSysTick();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            return;
        }


        /* Invalid command */
        DEBUG_PRINTF(("Invalid command!\r\n%s\r\n", com_receivedbyte));

        /* Clear received command */
        memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
        com_receive_slot = 0;
    }

    /* Timed out --> disable testmode */
    if (com_tickcount + TESTMODE_TIMEOUT < OS_getOSSysTick() && com_testmode_enabled) {
        com_testmode_enabled = 0;
        DEBUG_PRINTF(("Testmode disabled on timeout!\r\n"));
    }
}
#endif /* BUILD_MODULE_ENABLE_COM */
