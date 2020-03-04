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
 * @file    syscall.c
 * @author  foxBMS Team
 * @date    11.02.2019 (date of creation)
 * @ingroup OS
 * @prefix  SYSCALL
 *
 * @brief   System wide syscalls.
 *
 *
 */



/*================== Includes =============================================*/
#include "syscall.h"
#include "os.h"
#include <string.h>
#include "uart.h"
#include "stdio.h"

/*================== Macros and Definitions ===============================*/

/**
 * @brief Queue length for the #comReceiveQueue.
 */
#define COM_RECEIVE_QUEUE_LEN  RXBUF_LENGTH

/**
 * @brief Queue length for the #comTransmitQueue.
 */
#define COM_TRANSMIT_QUEUE_LEN TXBUF_LENGTH

/**
 * @brief Queue length for the #comErrorQueue.
 */
#define COM_ERROR_QUEUE_LEN    TXBUF_LENGTH

/**
 * @brief COM Queue item size (in byte).
 */
#define COM_QUEUE_ITEM_SIZE    (1u)

/*================== Constant and Variable Definitions ====================*/

/**
 * @brief Handle for the COM receive queue.
 */
static QueueHandle_t comReceiveQueue;

/**
 * @brief Data Structure of the #comReceiveQueue.
 */
static StaticQueue_t xComReceiveQueue_s;

/**
 * @brief Storage area for the #comReceiveQueue.
 *
 * This must be at least #uxQueueLength * #uxItemSize bytes.
 */
uint8_t ucComReceiveStorageArea[COM_RECEIVE_QUEUE_LEN * COM_QUEUE_ITEM_SIZE];

/**
 * @brief Handle for the COM transmit queue.
 */
static QueueHandle_t comTransmitQueue;

/**
 * @brief Data Structure of the #comTransmitQueue.
 */
static StaticQueue_t xComTransmitQueue_s;

/**
 * @brief Storage area for the #comTransmitQueue.
 *
 * This must be at least #uxQueueLength * #uxItemSize bytes.
 */
uint8_t ucComTransmitStorageArea[COM_TRANSMIT_QUEUE_LEN * COM_QUEUE_ITEM_SIZE];

/**
 * @brief Handle for the COM error queue.
 */
static QueueHandle_t comErrorQueue;

/**
 * @brief Data Structure of the #comErrorQueue.
 */
static StaticQueue_t xComErrorQueue_s;

/**
 * @brief Storage area for the #comErrorQueue.
 *
 * This must be at least #uxQueueLength * #uxItemSize bytes.
 */
uint8_t ucComErrorStorageArea[COM_ERROR_QUEUE_LEN * COM_QUEUE_ITEM_SIZE];

static uint8_t transmitted_char;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/
extern void UART_IntRx(UART_HandleTypeDef *huart) {
    /* Post a byte from the UART RX buffer to the queue. */
    xQueueSendFromISR(comReceiveQueue, huart->pRxBuffPtr - 1, NULL);
}

extern void UART_IntTx(UART_HandleTypeDef *huart) {
    if (xQueueReceiveFromISR(comErrorQueue, &transmitted_char, NULL)) {
        HAL_UART_Transmit_IT(huart, &transmitted_char, 1);
    } else if (xQueueReceiveFromISR(comTransmitQueue, &transmitted_char, NULL)) {
        HAL_UART_Transmit_IT(huart, &transmitted_char, 1);
    }
}

extern int _write(int fd, char *ptr, int len) {
    /* Write "len" of char from "ptr"
     * Return number of char written.
     * Check if file descriptor is in fact stdout
     * not neccessary but "good practice". */
    int retVal = 0;
    if (fd == 1) {
        while (len-- > 0) {
            xQueueSend(comTransmitQueue, ptr++, portMAX_DELAY);
            retVal++;
            __HAL_UART_ENABLE_IT(&uart_cfg[0], UART_IT_TC);
        }
    }

    if (fd == 2) {
        while (len-- > 0) {
            xQueueSend(comErrorQueue, ptr++, 0);
            retVal++;
            __HAL_UART_ENABLE_IT(&uart_cfg[0], UART_IT_TC);
        }
    }

    return retVal;
}

extern int _read(int fd, char *ptr, int len) {
    UBaseType_t messagesInQueue;
    int charactersReceived = 0;
    if (fd == 0) {
        messagesInQueue = uxQueueMessagesWaiting(comReceiveQueue);
        /* Empty queue into buffer if queue is filled,
           if queue is empty, read one element blocking */
        if (messagesInQueue == 0) messagesInQueue = 1;
        while (messagesInQueue != 0) {
            xQueueReceive(comReceiveQueue, ptr++, portMAX_DELAY);
            charactersReceived++;
            messagesInQueue--;
        }
    }
    return charactersReceived;
}

extern void SYSCALL_Init(void) {
    setbuf(stdout, NULL);
    comReceiveQueue = xQueueCreateStatic(COM_RECEIVE_QUEUE_LEN,
    COM_QUEUE_ITEM_SIZE, ucComReceiveStorageArea, &xComReceiveQueue_s);
    comTransmitQueue = xQueueCreateStatic(COM_TRANSMIT_QUEUE_LEN,
    COM_QUEUE_ITEM_SIZE, ucComTransmitStorageArea, &xComTransmitQueue_s);
    comErrorQueue = xQueueCreateStatic(COM_ERROR_QUEUE_LEN,
    COM_QUEUE_ITEM_SIZE, ucComErrorStorageArea, &xComErrorQueue_s);
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
