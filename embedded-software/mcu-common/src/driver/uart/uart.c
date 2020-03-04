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
 * @file    uart.c
 * @author  foxBMS Team
 * @date    23.09.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  UART
 *
 * @brief   Driver for the UART
 *
 * This uart module handles sending/receiving requests using UART/RS232.
 * In its current implementation it's interrupt driven, may change in the future
 * in order to support DMA.
 *
 */

/*================== Includes =============================================*/
#include "uart.h"

/*================== Macros and Definitions ===============================*/

#define MSG_NOT_BUSY    0
#define MSG_BUSY        1


/*================== Constant and Variable Definitions ====================*/

#ifndef UART_USEOS
volatile unsigned char Msg0SendBusy = 0;
uint8_t rxbuf[RXBUF_LENGTH];

uint8_t *wrpoi_rxbuf = &rxbuf[0];
uint8_t *rdpoi_rxbuf = &rxbuf[0];

uint8_t txbuf[TXBUF_LENGTH];


uint8_t *wrpoi_txbuf = &txbuf[0];
uint8_t *rdpoi_txbuf = &txbuf[0];
#endif

uint8_t received_char = 0;



/*================== Constant and Variable Definitions ====================*/
#ifndef UART_USEOS
char uart_com_receivedbyte[UART_COM_RECEIVEBUFFER_LENGTH];
uint8_t uart_com_receive_slot;
#endif
/*================== Function Prototypes ==================================*/
#ifndef UART_USEOS
static uint8_t *UART_txbuf_copy(uint8_t *ptrb, uint8_t *ptra);
static uint8_t *UART_txbuf_copy_length(uint8_t *ptrb, uint8_t *ptra, uint16_t length);
#endif
/*================== Function Implementations =============================*/

/* empty function declarations to fix bare-metal build */
__attribute__((weak)) void UART_IntTx(UART_HandleTypeDef *huart) {
}

__attribute__((weak)) void UART_IntRx(UART_HandleTypeDef *huart) {
}

extern void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  UART_IntTx(huart);
}

extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  UART_IntRx(huart);
  HAL_UART_Receive_IT(&uart_cfg[0], &received_char, 1);
}

void UART_Init(/*UART_HandleTypeDef *uart_cfg*/ void) {
    for (int i = 0; i < uart_cfg_length; i++) {
        HAL_UART_Init(&uart_cfg[i]);
    }
    HAL_UART_Receive_IT(&uart_cfg[0], &received_char, 1);
}

#ifndef UART_USEOS
/**
 * @brief UART_IntRx is responsible for handling receive requests.
 *
 * This is invoked from with the interrupt handler and processes
 * userdata on receive event.
 * It copies data from data register into a ringbuffer,
 * provides some very basic application example.
 */
void UART_IntRx(UART_HandleTypeDef *huart) {
/*   *asc0_wrpoi_rxbuf=(ASC0_RBUF.U & 0xFF);       read 8-Bit receive buffer register (like ASC0_usGetData(void) ) */
    *wrpoi_rxbuf = (uint8_t)(uart_cfg[0].Instance->DR/* & huart3.Mask*/);
    /*pointer handling of ringbuffer*/
    if (++wrpoi_rxbuf >= &rxbuf[RXBUF_LENGTH])
        wrpoi_rxbuf = &rxbuf[0];
    /* check overwrite of read pointer */

    /* todo: this is just a very simple debug/demonstration implementation for decoder */
    if (uart_com_receive_slot < UART_COM_RECEIVEBUFFER_LENGTH-1) {
    *(uart_com_receivedbyte + uart_com_receive_slot) = *rdpoi_rxbuf;
    uart_com_receive_slot++;
    }

    /*pointer handling of ringbuffer*/
    if (++rdpoi_rxbuf >= &rxbuf[RXBUF_LENGTH])
       rdpoi_rxbuf = &rxbuf[0];
}


/**
 * @brief UART_IntTx is responsible for handling send requests.
 *
 * This is invoked from with the interrupt handler and processes
 * userdata on send event.
 * It copies data from a ringbuffer into the data register
 * as long as the read pointer doesn't match the write pointer
 * (which means there's still unsent data in the ringbuffer)
 */
static void UART_IntTx(UART_HandleTypeDef *huart) {
    if (rdpoi_txbuf >= &txbuf[TXBUF_LENGTH])
        rdpoi_txbuf = &txbuf[0];

    if (rdpoi_txbuf != wrpoi_txbuf) {
        Msg0SendBusy = MSG_BUSY;
/*       ASC0_vSendData(*asc0_rdpoi_txbuf++);   send when pointers unequal (values in buffer) */
        uart_cfg[0].Instance->DR = (uint16_t) (*rdpoi_txbuf++);
    } else {
        /* Disable the UART Transmit Data Register Empty Interrupt */
        CLEAR_BIT(uart_cfg[0].Instance->CR1, USART_CR1_TXEIE);

        /* Enable the UART Transmit Complete Interrupt */
        SET_BIT(uart_cfg[0].Instance->CR1, USART_CR1_TCIE);

        Msg0SendBusy = MSG_NOT_BUSY;
    }
}
void UART_vWrite(const uint8_t *source) {
    wrpoi_txbuf = UART_txbuf_copy(wrpoi_txbuf, (uint8_t*) source);
    if (Msg0SendBusy == MSG_NOT_BUSY) {
        /* Enable the UART Transmit Data Register Empty Interrupt */
        SET_BIT(uart_cfg[0].Instance->CR1, USART_CR1_TXEIE);
    }
}

void UART_vWrite_intbuf(const uint8_t *source, uint16_t length) {
    wrpoi_txbuf = UART_txbuf_copy_length(wrpoi_txbuf, (uint8_t*) source, length);
    if (Msg0SendBusy == MSG_NOT_BUSY) {
        /* Enable the UART Transmit Data Register Empty Interrupt */
        SET_BIT(uart_cfg[0].Instance->CR1, USART_CR1_TXEIE);
    }
}

/**
 * @brief UART_txbuf_copy is a helper function.
 *
 * This function just copies content from buffer a to ringbuffer b.
 *
 * @return (type: uint8_t)
 */
static uint8_t *UART_txbuf_copy(uint8_t *ptrb, uint8_t *ptra) {
    uint8_t *tmpPtr = ptra;

    while (*ptra) {
        *ptrb++=*ptra++;
        if (ptrb >= &txbuf[TXBUF_LENGTH]) {
            ptrb = &txbuf[0];
        }
    }
    /*  do not send NULL-Value */

    /* Clear buffer a */
    while (*tmpPtr) {
        *tmpPtr = 0;
        tmpPtr++;
    }

    return ptrb;
}

/**
 * @brief UART_txbuf_copy_length is a helper function.
 *
 * This function just copies a given length of data from buffer a to ringbuffer b.
 * Difference to uint8_t *UART_txbuf_copy (uint8_t *ptrb, uint8_t *ptra) is, that it will stop
 * at an ASCII_NULL character.
 *
 * @return (type: uint8_t)
 */
static uint8_t *UART_txbuf_copy_length(uint8_t *ptrb, uint8_t *ptra, uint16_t length) {
    uint8_t *tmpPtr = ptra;
    uint8_t tmp = 0;

    while (tmp < length) {
        *ptrb++=*ptra++;
        if (ptrb >= &txbuf[TXBUF_LENGTH]) {
            ptrb = &txbuf[0];
        }
        tmp++;
    }
    /*  do not send NULL-Value */
    tmp = 0;
    /* Clear buffer a */
    while (tmp < length) {
        *tmpPtr = 0;
        tmpPtr++;
        tmp++;
    }

    return ptrb;
}
#endif
