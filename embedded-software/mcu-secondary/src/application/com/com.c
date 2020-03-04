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
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
#include "contactor.h"
#endif
#include "mcu.h"
#include "nvram_cfg.h"
#include "os.h"
#include <string.h>
#include "rtc.h"
#include "uart.h"
#include "stdio.h"

/*================== Macros and Definitions ===============================*/
#define TESTMODE_TIMEOUT 30000

static char com_receivedbyte[UART_COM_RECEIVEBUFFER_LENGTH];

/*================== Constant and Variable Definitions ====================*/
uint8_t printHelp = 0;

static uint8_t com_testmode_enabled = 0;
static uint32_t com_tickcount = 0;

static RTC_Time_s com_Time;
static RTC_Date_s com_Date;

/*================== Function Prototypes ==================================*/

static void COM_getRunTime();
/*================== Function Implementations =============================*/
/* Secondary MCU has no SOX module so setting the SOC leads to an error */
__attribute__((weak)) void SOC_SetValue(float v1, float v2, float v3) {
    printf("ERROR: No SOC on secondary MCU");
}

void COM_printTimeAndDate(void) {
    /* Get time and date */
    RTC_getTime(&com_Time);
    RTC_getDate(&com_Date);

    printf("Date and Time: 20%02d.%02d.%02d - %02d:%02d:%02d\r\n",
      com_Date.Year, com_Date.Month, com_Date.Date, com_Time.Hours,
      com_Time.Minutes, com_Time.Seconds);
}


void COM_StartupInfo(void) {
    /* Get RCC Core Reset Register */
    uint32_t tmp = main_state.CSR;
    printf("........................................\r\n");
    printf("System starting...\r\n");
    printf("RCC Core Reset Register: 0x%x\r\n", (unsigned int)tmp);

    /* Print time and date */
    COM_printTimeAndDate();
    COM_getRunTime();
}

void COM_printHelpCommand(void) {
    if (printHelp == 0)
        return;
    printf("\r\nFollowing commands are available: :\r\n\r\n");

    printf("====================  ========================================================================================================\r\n");
    printf("Command               Description\r\n");
    printf("====================  ========================================================================================================\r\n");
    printf("help                  get available command list\r\n");
    printf("gettime               get system time\r\n");
    printf("getruntime            get runtime since last reset\r\n");
    printf("getoperatingtime      get total operating time\r\n");
    printf("printdiaginfo         get diagnosis entries of DIAG module (entries can only be printed once)\r\n");
    printf("printcontactorinfo    get contactor information (number of switches/hard switches) (entries can only be printed once)\r\n");
    printf("teston                enable testmode, testmode will be disabled after a predefined timeout of 30s when no new command is sent\r\n");
    printf("====================  ========================================================================================================\r\n");

    printf("\r\nFollowing commands are only available during enabled testmode:\r\n\r\n");

    printf("Command                     Description\r\n");
    printf("==========================  ==========================================================================================\r\n");
    printf("testoff                     disable testmode\r\n");
    printf("settime YY MM DD HH MM SS   set mcu time and date (YY-year, MM-month, DD-date, HH-hours, MM-minutes, SS-seconds)\r\n");
    printf("reset                       enforces complete software reset using HAL_NVIC_SystemReset()\r\n");
    printf("watchdogtest                performs watchdog test, watchdog timeout results in system reset (predefined 1s)\r\n");
    printf("setsoc xxx.xxx              set SOC value (000.000%% - 100.000%%)\r\n");
    printf("ceX                         enables contactor number X (only possible if BMS is in no error state)\r\n");
    printf("cdX                         disables contactor number X (only possible if BMS is in no error state)\r\n");
    printf("==========================  ==========================================================================================\r\n");
    printHelp = 0;
}

static void COM_settime(void) {
            /* Command length = 24, +1 for \0, +1 cause index always one step ahead */
        if (strlen(com_receivedbyte) == 26) {
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

            if (IS_RTC_HOUR24(value))  /* Check if value is valid */
                com_Time.Hours = value;
            else
                retVal = 0xFF;

            /* Calculate minutes */
            tmp10 = com_receivedbyte[20] - '0';
            tmp1 = com_receivedbyte[21] - '0';
            value = (tmp10 * 10) + tmp1;

            if (IS_RTC_MINUTES(value))  /* Check if value is valid */
                com_Time.Minutes = value;
            else
                retVal = 0xFF;

           /* Calculate seconds */
            tmp10 = com_receivedbyte[23] - '0';
            tmp1 = com_receivedbyte[24] - '0';
            value = (tmp10 * 10) + tmp1;

            if (IS_RTC_SECONDS(value))  /* Check if value is valid */
                com_Time.Seconds = value;
            else
                retVal = 0xFF;

            if (retVal == 0) {
                /* Set time and date */
                RTC_setTime(&com_Time);
                RTC_setDate(&com_Date);

                printf("Time and date set!\r\n");
            } else {
                /* Print error message */
                printf("Invalid parameter!\r\n%.25s\r\n", com_receivedbyte);
            }

        } else {
            /* Print error message */
            printf("Invalid parameter length!\r\n");
            printf("%.*s\r\n", strlen(com_receivedbyte), com_receivedbyte);
        }
}

static void COM_getRunTime() {
    printf("Runtime: %02dh %02dm %02ds\r\n", os_timer.Timer_h,
       os_timer.Timer_min, os_timer.Timer_sec);
}

uint8_t COM_testMode_Decoder(void) {
    uint8_t commandValid = 0;

    if (strncmp(com_receivedbyte, "testoff", 7) == 0) {/* DISABLE TESTMODE */
        com_testmode_enabled = 0;
        printf("Testmode disabled on request!\r\n");
        commandValid = 1;
    } else if (strncmp(com_receivedbyte, "settime", 7) == 0) {  /* SETTIME */
        COM_settime();
        commandValid = 1;
    } else if (strncmp(com_receivedbyte, "reset", 5) == 0) { /* RESET */
        printf("Software reset!\r\n");
        commandValid = 1;
        HAL_NVIC_SystemReset();
    } else if (strncmp(com_receivedbyte, "watchdogtest", 12) == 0) {
        printf("WDG");
        commandValid = 1;
        OS_taskDelay(1);

        /* disable global interrupt, no saving of interrupt status because of follwing reset */
        (void)MCU_DisableINT();

      while (1) {
          /* stop system and wait for watchdog reset*/
      }

      return commandValid;
    } else if (strncmp(com_receivedbyte, "setsoc", 6) == 0) { /* Set soc */
        /* Set SOC */
        /* Command length = 14, +1 for \0 and +1 cause index always one step ahead "setsoc xxx.xxx"*/
        if (strlen(com_receivedbyte) == 15) {
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
            value = value + (tmp1 * 0.1f) + (tmp10 * 0.01f) + (tmp100 * 0.001f);

            if (0 <= value && value <= 100)
                SOC_SetValue(value, value, value);
        }
        commandValid = 1;
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
    /* SWITCH CONTACTORS */
    } else if (strncmp(com_receivedbyte, "contactor enable", 16) == 0) {
        /* Convert ascii number to int */
        int16_t contNumber;
        contNumber = com_receivedbyte[17] - '0';

        /* Switch contactor */
        if ((0 <= contNumber) && (contNumber <= BS_NR_OF_CONTACTORS)) {
            /* Close contactor */
            if (com_receivedbyte[1] == 'e') {
                printf("Contactor %d", contNumber);

                CONT_SetContactorState(contNumber, CONT_SWITCH_ON);

                printf(" enabled\r\n");
            }
        } else {
             /* Invalid contactor number */
            uint8_t a = BS_NR_OF_CONTACTORS;
            printf("Invalid contactor number! Only %d contactors are connected! \r\n", a);
        }
        commandValid = 1;
    } else if (strncmp(com_receivedbyte, "contactor disable", 17) == 0) {
        /* Convert ascii number to int */
        int16_t contNumber;
        contNumber = com_receivedbyte[18] - '0';

        /* Switch contactor */
        if ((0 <= contNumber) && (contNumber <= BS_NR_OF_CONTACTORS)) {
            /* Open contactor */
                printf("Contactor %d", contNumber);

                CONT_SetContactorState(contNumber, CONT_SWITCH_OFF);

                printf(" disabled\r\n");
        } else {
             /* Invalid contactor number */
            uint8_t a = BS_NR_OF_CONTACTORS;
            printf("Invalid contactor number! Only %d contactors are connected! \r\n", a);
        }
        commandValid = 1;
#endif
    }
    return commandValid;
}

void COM_Decoder(void) {
    uint32_t commandValid = 0;
    /* Command Received - Replace Carrier Return with null character */
    fgets(com_receivedbyte, UART_COM_RECEIVEBUFFER_LENGTH , stdin);

    /* Command received and testmode enabled */
    if (com_testmode_enabled) {
        commandValid = COM_testMode_Decoder();
    }

    if (commandValid > 0) {
    } else if (strncmp(com_receivedbyte, "teston", 6) == 0) { /* ENABLE TESTMODE */
        /* Set timeout */
        com_tickcount = OS_getOSSysTick();

        if (!com_testmode_enabled) {
            /* Enable testmode */
            com_testmode_enabled = 1;

            printf("Testmode enabled!\r\n");
        } else {
            /* Testmode already enabled */
           printf("Testmode already enabled!\r\n");
        }

        /* Reset timeout to TESTMODE_TIMEOUT */
        com_tickcount = OS_getOSSysTick();

        commandValid = 1;
    } else if (strncmp(com_receivedbyte, "help", 4) == 0) { /* PRINT help command */
        /* Print contactor info */
        printHelp = 1;
        commandValid = 1;
#if BUILD_MODULE_ENABLE_CONTACTOR == 1
    } else if (strncmp(com_receivedbyte, "printcontactorinfo", 18) == 0) { /* PRINT CONTACTOR INFO */
        /* Print contactor info */
        DIAG_PrintContactorInfo();
        commandValid = 1;
#endif
    } else if (strncmp(com_receivedbyte, "printdiaginfo", 13) == 0) { /* PRINT DIAG INFO */
        /* Print diag info */
        DIAG_PrintErrors();
        commandValid = 1;
    } else if (strncmp(com_receivedbyte, "gettime", 7) == 0) {  /* GETTIME */
        /* Print time and date */
       COM_printTimeAndDate();
       commandValid = 1;
    } else if (strncmp(com_receivedbyte, "getruntime", 10) == 0) { /* Get runtime */
        /* Print runtime */
        COM_getRunTime();
        commandValid = 1;
#if BUILD_MODULE_ENABLE_NVRAM == 1
    } else if (strncmp(com_receivedbyte, "getoperatingtime", 16) == 0) { /* Get operating time */
        printf("Operating time: %03dd %02dh %02dm %02ds\r\n",
           bkpsram_op_hours.Timer_d, bkpsram_op_hours.Timer_h,
            bkpsram_op_hours.Timer_min, bkpsram_op_hours.Timer_sec);
        commandValid = 1;
#endif
    }
    if (!commandValid) {
        /* Invalid command */
        printf("Invalid command!\r\n%s\r\n", com_receivedbyte);
    }

    /* Timed out --> disable testmode */
    if (com_tickcount + TESTMODE_TIMEOUT < OS_getOSSysTick() && com_testmode_enabled) {
        com_testmode_enabled = 0;
        printf("Testmode disabled on timeout!\r\n");
    }
}
#endif /* BUILD_MODULE_ENABLE_COM */
