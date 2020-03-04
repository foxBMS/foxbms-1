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
 * @file    can.c
 * @author  foxBMS Team
 * @date    12.07.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  CAN
 *
 * @brief   Driver for the CAN module
 *
 * Implementation of the CAN Interrupts, initialization, buffers,
 * receive and transmit interfaces.
 *
 */

/*================== Includes =============================================*/
#include "can.h"

/*================== Macros and Definitions ===============================*/
#define ID_16BIT_FIFO0          (0U)
#define ID_16BIT_FIFO1          (1U)
#define ID_32BIT_FIFO0          (2U)
#define ID_32BIT_FIFO1          (3U)
#define MSK_16BIT_FIFO0         (4U)
#define MSK_16BIT_FIFO1         (5U)
#define MSK_32BIT               (6U)

/*================== Constant and Variable Definitions ====================*/
uint8_t canNode0_listenonly_mode = 0;
uint8_t canNode1_listenonly_mode = 0;

#if CAN_USE_CAN_NODE0
#if CAN0_USE_TX_BUFFER
CAN_TX_BUFFERELEMENT_s can0_txbufferelements[CAN0_TX_BUFFER_LENGTH];
CAN_TX_BUFFER_s can0_txbuffer = {
    .length = CAN0_TX_BUFFER_LENGTH,
    .buffer = &can0_txbufferelements[0],
};
#endif /* CAN0_USE_TX_BUFFER */

#if CAN0_USE_RX_BUFFER
CAN_RX_BUFFERELEMENT_s can0_rxbufferelements[CAN0_RX_BUFFER_LENGTH];
CAN_RX_BUFFER_s can0_rxbuffer = {
    .length = CAN0_RX_BUFFER_LENGTH,
    .buffer = &can0_rxbufferelements[0],
};
#endif /* CAN0_USE_RX_BUFFER */

#if CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0
uint8_t can0_fastLinkIndex[CAN0_BUFFER_BYPASS_NUMBER_OF_IDs];   /* Link Table for bufferBypassing */
#endif

CAN_ERROR_s CAN0_errorStruct = {
    .canError = HAL_CAN_ERROR_NONE,
    .canErrorCounter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
#endif /* CAN_USE_CAN_NODE0 */

#if CAN_USE_CAN_NODE1
#if CAN1_USE_TX_BUFFER
CAN_TX_BUFFERELEMENT_s can1_txbufferelements[CAN1_TX_BUFFER_LENGTH];
CAN_TX_BUFFER_s can1_txbuffer = {
        .length = CAN1_TX_BUFFER_LENGTH,
        .buffer = &can1_txbufferelements[0],
};
#endif /* CAN1_USE_TX_BUFFER */

#if CAN1_USE_RX_BUFFER
CAN_RX_BUFFERELEMENT_s can1_rxbufferelements[CAN1_RX_BUFFER_LENGTH];
CAN_RX_BUFFER_s can1_rxbuffer = {
        .length = CAN1_RX_BUFFER_LENGTH,
        .buffer = &can1_rxbufferelements[0],
};
#endif /* CAN1_USE_RX_BUFFER */

#if CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0
uint8_t can1_fastLinkIndex[CAN1_BUFFER_BYPASS_NUMBER_OF_IDs];   /* Link Table for bufferBypassing */
#endif

CAN_ERROR_s CAN1_errorStruct = {
    .canError = HAL_CAN_ERROR_NONE,
    .canErrorCounter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
#endif /* CAN_USE_CAN_NODE1 */

/* ***********************************************************
 *  Dummies for filter initialization and message reception
 *************************************************************/

CAN_FilterTypeDef sFilterConfig = {
        /* No need to insert here something */
        .FilterActivation = ENABLE,     /* enable the filter */
};

/*================== Function Prototypes ==================================*/
/* Inits */
static uint8_t CAN_GetNextID(CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t numberOfRxIDs, uint8_t startIndex,
        uint8_t filterCase);
static uint8_t CAN_NumberOfNeededFilters(CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t* numberOfDifferentIDs, uint32_t* error);
static uint32_t CAN_InitFilter(CAN_HandleTypeDef* ptrHcan, CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t numberOfRxMsgs);

/* Interrupts */
static void CAN_TxCpltCallback(CAN_NodeTypeDef_e canNode);
static void CAN_RxMsg(CAN_NodeTypeDef_e canNode, CAN_HandleTypeDef* ptrHcan, uint8_t FIFONumber);

/* Buffer/Interpreter */
static STD_RETURN_TYPE_e CAN_BufferBypass(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* rxData, uint8_t DLC,
        uint8_t RTR);
static STD_RETURN_TYPE_e CAN_InterpretReceivedMsg(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* data, uint8_t DLC,
        uint8_t RTR);

/*================== Function Implementations =============================*/

/* ***************************************
 *  Initialization
 ****************************************/

uint32_t CAN_Init(void) {
    uint32_t retval = 0;

#if CAN_USE_CAN_NODE0
    /* DeInit CAN0 handle */
    if (HAL_CAN_DeInit(&hcan0) != HAL_OK) {
        /* Error deintializing handle -> set error bit */
        retval |= STD_ERR_BIT_0;
    }

    /* Init CAN0-handle */
    if (HAL_CAN_Init(&hcan0) != HAL_OK) {
        /* Error intializing handle -> set error bit */
        retval |= STD_ERR_BIT_1;
    }

    /* Configure CAN0 hardware filter */
    retval |= CAN_InitFilter(&hcan0, &can0_RxMsgs[0], can_CAN0_rx_length);

    /* Check if more rx messages are bypassed than received */
#pragma GCC diagnostic push
    /* configurations might exist that use this comparison */
#pragma GCC diagnostic ignored "-Wtype-limits"
    if (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > can_CAN0_rx_length) {
#pragma GCC diagnostic pop
        retval |= STD_ERR_BIT_7;
    }

    /* Enable CAN0 message receive interrupt FIFO0 */
    if (HAL_CAN_ActivateNotification(&hcan0, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        retval |= STD_ERR_BIT_8;
    }
    hcan0.State = HAL_CAN_STATE_READY;
    /* Enable CAN0 message receive interrupt FIFO1 */
    if (HAL_CAN_ActivateNotification(&hcan0, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
        retval |= STD_ERR_BIT_9;
    }
    /* Enable CAN0 Transmit mailbox empty interrupt */
    if (HAL_CAN_ActivateNotification(&hcan0, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) {
        retval |= STD_ERR_BIT_10;
    }

    /* set DBF bit to 0 for CAN activity while in debug mode */
    CLEAR_BIT(hcan0.Instance->MCR, CAN_MCR_DBF);

#if CAN_USE_STANDBY_CONTROL == 1
    IO_WritePin(CAN_0_TRANS_STANDBY_CONTROL, IO_PIN_SET);
#endif /* CAN_USE_STANDBY_CONTROL == 1 */

    /* Start CAN */
    HAL_CAN_Start(&hcan0);
#endif /* CAN_USE_CAN_NODE0 */


#if CAN_USE_CAN_NODE1
    /* DeInit CAN1 handle */
    if (HAL_CAN_DeInit(&hcan1) != HAL_OK) {
        /* Error deintializing handle -> set error bit */
        retval |= STD_ERR_BIT_11;
    }

    /* Init CAN1-handle */
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        /* Error intializing handle -> set error bit */
        retval |= STD_ERR_BIT_12;
    }

    /* Configure CAN1 hardware filter */
    retval |= CAN_InitFilter(&hcan1, &can1_RxMsgs[0], can_CAN1_rx_length);

    /* Check if more RX messages are bypassed than received */
#pragma GCC diagnostic push
    /* configurations might exist that use this comparison */
#pragma GCC diagnostic ignored "-Wtype-limits"
    if (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > can_CAN1_rx_length) {
#pragma GCC diagnostic pop
        retval |= STD_ERR_BIT_13;
    }

    /* Enable CAN1 message receive interrupt FIFO0 */
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        retval |= STD_ERR_BIT_14;
    }
    /* Enable CAN1 message receive interrupt FIFO1 */
    hcan1.State = HAL_CAN_STATE_READY;
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
        retval |= STD_ERR_BIT_15;
    }
    /* Enable CAN1 Transmit mailbox empty interrupt */
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) {
        retval |= STD_ERR_BIT_16;
    }

    /* set DBF bit to 0 for CAN activity while in debug mode */
    CLEAR_BIT(hcan1.Instance->MCR, CAN_MCR_DBF);

#if CAN_USE_STANDBY_CONTROL == 1
    IO_WritePin(CAN_1_TRANS_STANDBY_CONTROL, IO_PIN_SET);
#endif /* CAN_USE_STANDBY_CONTROL == 1 */

    /* Start CAN */
    HAL_CAN_Start(&hcan1);
#endif /* CAN_USE_CAN_NODE1 */
    return retval;
}

/**
 * @brief  Initializes message filtering
 * @retval 0: if no error occurred, otherwise error code
 */
static uint32_t CAN_InitFilter(CAN_HandleTypeDef* ptrHcan, CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t numberOfRxMsgs) {
    /* Contains the occurrence of of the different filter cases     *
     * [0] - ID List mode 16bit routed on FIFO0                     *
     * [1] - ID List mode 16bit routed on FIFO1                     *
     * [2] - ID List mode 32bit routed on FIFO0                     *
     * [3] - ID List mode 32bit routed on FIFO1                     *
     * [4] - Mask mode 16bit routed on FIFO0                        *
     * [5] - Mask mode 16bit routed on FIFO1                        *
     * [6] - Mask mode 32bit                                        */
    uint8_t numberOfDifferentIDs[7] = { 0, 0, 0, 0, 0, 0, 0 };
    static uint8_t filterNumber = 0;   /* Number of the filter to be initialized */
    uint32_t retval = 0;

    /* Calculate number of needed filter banks */
    uint8_t numberNeededFilters = CAN_NumberOfNeededFilters(can_RxMsgs, &numberOfDifferentIDs[0], &retval);
    numberNeededFilters--;  /* Decrement by one because IS_CAN_FILTER_BANK_DUAL checks filter bank numbers starting with 0 */
    if (IS_CAN_FILTER_BANK_DUAL(numberNeededFilters)) {
        uint8_t j = 0;  /* Counts the number of initialized instances per case */
        uint8_t posRxMsgs = 0;  /* Iterator for can_RxMsgs[] */
        uint8_t numberRegistersUsed = 0;  /* Counts how many register space is already used in each filter bank (max. 64bit) */
        uint8_t caseID = 0;  /* indicates the actual filter mode that will be initialized */

        if (ptrHcan->Instance  ==  CAN2) {
            /* Set start slave bank filter */
            sFilterConfig.FilterBank = filterNumber;
        }
        for (caseID = 0; caseID < 2u; caseID++) {
            /* ID List mode 16bit routed on FIFO0 or FIFO1 */

            if (numberOfDifferentIDs[caseID] > 0U) {
                j = 0;
                while (j < numberOfDifferentIDs[caseID]) {
                    /* Until all IDs in that filter case are treated */

                    posRxMsgs = CAN_GetNextID(can_RxMsgs, numberOfRxMsgs, posRxMsgs, caseID);  /*  Get array position of next ID */

                    switch (numberRegistersUsed) {
                        case 0:  /* 1st ID per filter bank */
                            sFilterConfig.FilterIdHigh = ((can_RxMsgs[posRxMsgs].ID << 5)
                                    | can_RxMsgs[posRxMsgs].RTR << 4);
                            j++;
                            break;

                        case 1:  /* 2nd ID */
                            sFilterConfig.FilterIdLow = ((can_RxMsgs[posRxMsgs].ID << 5)
                                    | can_RxMsgs[posRxMsgs].RTR << 4);
                            j++;
                            break;

                        case 2:  /* 3rd ID */
                            sFilterConfig.FilterMaskIdHigh = ((can_RxMsgs[posRxMsgs].ID << 5)
                                    | can_RxMsgs[posRxMsgs].RTR << 4);
                            j++;
                            break;

                        case 3:  /* 4th ID */
                            sFilterConfig.FilterMaskIdLow = ((can_RxMsgs[posRxMsgs].ID << 5)
                                    | can_RxMsgs[posRxMsgs].RTR << 4);
                            j++;
                            break;
                    }
                    numberRegistersUsed = j % 4U;    /* space for 4 IDs a 16 bit in one filter bank */
                    if ((numberRegistersUsed  ==  0 && j > 1U) || (j  ==  numberOfDifferentIDs[caseID])) {
                        /* all registers in filter bank used OR no more IDs in that case */
                        sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
                        sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
                        if (caseID == ID_16BIT_FIFO0) {
                            sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
                        } else if (caseID == ID_16BIT_FIFO1) {
                            sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
                        }
                        sFilterConfig.FilterBank = filterNumber;
                        HAL_CAN_ConfigFilter(ptrHcan, &sFilterConfig);    /* initialize filter bank */
                        filterNumber++;     /* increment filter number */
                    }
                    posRxMsgs++;    /* increment array position to find next valid ID */
                }
                posRxMsgs = 0;      /* reset variables for next case */
                numberRegistersUsed = 0;
            }
        }
        for (caseID = 2; caseID < 6U; caseID++) {
            /* ID List mode 32bit routed on FIFO0 or FIFO1; Mask mode 16bit routed on FIFO0 or FIFO1 */
            j = 0;
            if (numberOfDifferentIDs[caseID] > 0U) {
                while (j < numberOfDifferentIDs[caseID]) {
                    /* Until all IDs in that filter case are treated */

                    posRxMsgs = CAN_GetNextID(can_RxMsgs, numberOfRxMsgs, posRxMsgs, caseID);  /*  Get array position of next ID */

                    switch (numberRegistersUsed) {
                        case 0:  /* first 32bit per filter bank */
                            if (caseID  ==  ID_32BIT_FIFO0 || caseID  ==  ID_32BIT_FIFO1) {  /* list mode 32bit */
                                sFilterConfig.FilterIdHigh = ((can_RxMsgs[posRxMsgs].ID << 3) >> 16);  /* 1 << 2 is for setting IDE bit to receive extended identifiers */
                                sFilterConfig.FilterIdLow = (uint16_t)((can_RxMsgs[posRxMsgs].ID << 3) | 1 << 2
                                        | can_RxMsgs[posRxMsgs].RTR << 1);
                            } else if (caseID  ==  MSK_16BIT_FIFO0 || caseID  ==  MSK_16BIT_FIFO1) {  /* mask mode 16bit */
                                sFilterConfig.FilterIdHigh = ((can_RxMsgs[posRxMsgs].ID << 5)
                                        | can_RxMsgs[posRxMsgs].RTR << 4);
                                sFilterConfig.FilterMaskIdHigh = can_RxMsgs[posRxMsgs].mask;
                                sFilterConfig.FilterIdLow = 0x0000;      /* set second register to 0xFFFF, */
                                sFilterConfig.FilterMaskIdLow = 0xFFFF;  /* otherwise all messages would be received */
                            }
                            j++;
                            break;

                        case 1:  /* second 32bit per filter bank */
                            if (caseID  ==  ID_32BIT_FIFO0 || caseID  ==  ID_32BIT_FIFO1) {  /* list mode 32bit */
                                sFilterConfig.FilterMaskIdHigh = ((can_RxMsgs[posRxMsgs].ID << 3) >> 16);  /*  1 << 2 is for setting IDE bit to receive extended identifiers */
                                sFilterConfig.FilterMaskIdLow = (uint16_t)((can_RxMsgs[posRxMsgs].ID << 3) | 1 << 2
                                        | can_RxMsgs[posRxMsgs].RTR << 1);
                            } else if (caseID  ==  MSK_16BIT_FIFO0 || caseID  ==  MSK_16BIT_FIFO1) {  /* mask mode 16bit */
                                sFilterConfig.FilterIdLow = ((can_RxMsgs[posRxMsgs].ID << 5)
                                        | can_RxMsgs[posRxMsgs].RTR << 4);
                                sFilterConfig.FilterMaskIdLow = can_RxMsgs[posRxMsgs].mask;
                            }
                            j++;
                            break;
                    }
                    numberRegistersUsed = j % 2;    /* Space for two IDs a 32bit or two mask a 16bit */
                    if ((numberRegistersUsed == 0 && j > 1U) || (j == numberOfDifferentIDs[caseID])) {
                        /* all registers in filter bank used OR no more IDs in that case */
                        if (caseID  ==  ID_32BIT_FIFO0 || caseID  ==  ID_32BIT_FIFO1) {
                            sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
                            sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
                            if (caseID  ==  ID_32BIT_FIFO0)
                                sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
                            else
                                sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
                        } else if (caseID  ==  MSK_16BIT_FIFO0 || caseID  ==  MSK_16BIT_FIFO1) {
                            sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
                            sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
                            if (caseID  ==  MSK_16BIT_FIFO0)
                                sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
                            else
                                sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
                        }
                        sFilterConfig.FilterBank = filterNumber;
                        HAL_CAN_ConfigFilter(ptrHcan, &sFilterConfig);    /* initialize filter bank */
                        filterNumber++;     /* increment filter number */
                    }
                    posRxMsgs++;    /* increment array position to find next valid ID */
                }
                posRxMsgs = 0;      /* reset variables for next case */
                numberRegistersUsed = 0;
            }
        }
        j = 0;
        if (numberOfDifferentIDs[MSK_32BIT] > 0U) {
            /* Mask mode 32bit */

            while (j < numberOfDifferentIDs[MSK_32BIT]) {  /*  Get array position of next ID */
                /* Until all IDs in that filter case are treated */
                posRxMsgs = CAN_GetNextID(can_RxMsgs, numberOfRxMsgs, posRxMsgs, MSK_32BIT);

                sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
                sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
                sFilterConfig.FilterIdHigh = ((can_RxMsgs[posRxMsgs].ID << 3) >> 16);  /* 1 << 2 is for setting IDE bit to receive extended identifiers */
                sFilterConfig.FilterIdLow = (uint16_t)((can_RxMsgs[posRxMsgs].ID << 3) | 1 << 2
                        | can_RxMsgs[posRxMsgs].RTR << 1);
                sFilterConfig.FilterMaskIdHigh = can_RxMsgs[posRxMsgs].mask >> 16;
                sFilterConfig.FilterMaskIdLow = (uint16_t)(can_RxMsgs[posRxMsgs].mask);
                sFilterConfig.FilterFIFOAssignment = can_RxMsgs[posRxMsgs].fifo;
                sFilterConfig.FilterBank = filterNumber;
                HAL_CAN_ConfigFilter(ptrHcan, &sFilterConfig);
                filterNumber++;
                posRxMsgs++;
                j++;
            }
        }
    } else {
        /* Too many filterbanks needed! Check the value of CAN_NUMBER_OF_FILTERBANKS */
        /* If correct, try to reduce the IDs through masks or optimize used filter bank space. */
        /* Number of different filter cases can be evaluated in numberOfDifferentIDs[]. One */
        /* filter bank can filter as many messages as followed: */
        /* 4 IDs in list mode 16bit */
        /* 2 IDs in list mode 32bit and mask mode 16bit */
        /* 1 ID in 32bit mask mode */
        retval |= STD_ERR_BIT_6;
    }
    return retval;
}

/**
 * @brief  Returns the number of filters that have to be initialized
 *
 * @param can_RxMsgs:              pointer to receive message struct
 * @param numberOfDifferentIDs:    pointer to array, where to store the specific number of different IDs
 *
 * @retval number of needed filters
 */
static uint8_t CAN_NumberOfNeededFilters(CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t* numberOfDifferentIDs, uint32_t* error) {
    static uint8_t retVal = 0;      /* static so save the number of filters from CAN0 and add to the ones from CAN1 */
    uint16_t can_rx_length = 0;

    if (can_RxMsgs  ==  &can0_RxMsgs[0]) {
        can_rx_length = can_CAN0_rx_length;
    } else if (can_RxMsgs  ==  &can1_RxMsgs[0]) {
        can_rx_length = can_CAN1_rx_length;
    } else {
        can_rx_length = 0;
    }

    for (uint8_t i = 0; i < can_rx_length; i++) {
#if (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0) && (CAN_USE_CAN_NODE0 == 1)

        if (can_RxMsgs  ==  &can0_RxMsgs[0]) {
            /* Set buffer bypass IDs link table */
            for (uint8_t k = 0; k < CAN0_BUFFER_BYPASS_NUMBER_OF_IDs; k++) {
                if (can_RxMsgs[i].ID  ==  can0_bufferBypass_RxMsgs[k]) {
                    /* bypass ID  ==  ID in message receive struct */

                    can0_fastLinkIndex[k] = i;   /* set for can_bufferBypass_RxMsgs[k] link to array index */
                    break;
                }
            }
        }
#endif /* (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0) && (CAN_USE_CAN_NODE0 == 1) */
#if (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) && (CAN_USE_CAN_NODE1 == 1)

        if (can_RxMsgs  ==  &can1_RxMsgs[0]) {
            /* Set buffer bypass IDs link table */
            for (int k = 0; k < CAN1_BUFFER_BYPASS_NUMBER_OF_IDs; k++) {
                if (can1_RxMsgs[i].ID  ==  can1_bufferBypass_RxMsgs[k]) {
                    /* bypass ID  ==  ID in message receive struct */

                    can1_fastLinkIndex[k] = i;   /* set for can1_bufferBypass_RxMsgs[k] link to array index */
                    break;
                }
            }
        }
#endif /* (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) && (CAN_USE_CAN_NODE1 == 1) */
        if (can_RxMsgs[i].mask == 0 && IS_CAN_STDID(can_RxMsgs[i].ID)) {
            /* ID List mode 16bit */

            if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0) {
                numberOfDifferentIDs[ID_16BIT_FIFO0]++;
            } else if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1) {
                numberOfDifferentIDs[ID_16BIT_FIFO1]++;
            } else {
                /* Invalid FIFO selection; check can_RxMsgs[i].fifo value */
                *error |= STD_ERR_BIT_2;
            }
        } else if ((can_RxMsgs[i].mask == 0) && IS_CAN_EXTID(can_RxMsgs[i].ID)) {
            /* ID List mode 32bit */

            if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0) {
                numberOfDifferentIDs[ID_32BIT_FIFO0]++;
            } else if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1) {
                numberOfDifferentIDs[ID_32BIT_FIFO1]++;
            } else {
                /* Invalid FIFO selection; check can_RxMsgs[i].fifo value */
                *error |= STD_ERR_BIT_3;
            }
        } else if (can_RxMsgs[i].mask > 0 && IS_CAN_STDID(can_RxMsgs[i].ID)) {
            /* Mask mode 16bit */

            if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0) {
                numberOfDifferentIDs[MSK_16BIT_FIFO0]++;
            } else if (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1) {
                numberOfDifferentIDs[MSK_16BIT_FIFO1]++;
            } else {
                /* Invalid FIFO selection; check can_RxMsgs[i].fifo value */
                *error |= STD_ERR_BIT_4;
            }
        } else if ((can_RxMsgs[i].mask > 0U) && IS_CAN_EXTID(can_RxMsgs[i].ID)) {
            /* Mask mode 32bit */

            numberOfDifferentIDs[MSK_32BIT]++;
        } else {
            /* Invalid ID > IS_CAN_EXTID; check can_RxMsgs[i].ID value */
            *error |= STD_ERR_BIT_5;
            break;
        }
    }
    for (uint8_t i = 0; i < 2U; i++) {
        if (numberOfDifferentIDs[i] > 0U) {
            retVal += (numberOfDifferentIDs[i] + 2) / 4;  /* 4 IDs per filter; rounding up */
        }
    }
    for (uint8_t i = 2; i < 6U; i++) {
        if (numberOfDifferentIDs[i] > 0U) {
            retVal += (numberOfDifferentIDs[i] + 1) / 2;  /* 2 IDs per filter; rounding up */
        }
    }
    if (numberOfDifferentIDs[MSK_32BIT] > 0U) {
        retVal += numberOfDifferentIDs[6];                /* 1 ID per filter */
    }
    return retVal;
}

/**
 * @brief  Returns the next index of wished filter ID setting in CAN_MSG_RX_TYPE_t can_RxMsgs[CAN_NUMBER_OF_RX_IDs]
 *
 * @param can_RxMsgs:     pointer to receive message struct
 * @param numberOfRxIDs:  count of that type of receive message in can_RxMsgs struct
 * @param startIndex:     index where to start searching
 * @param filterCase:     specifies the object what will be found
 *
 * @retval returns index
 */
static uint8_t CAN_GetNextID(CAN_MSG_RX_TYPE_s* can_RxMsgs, uint8_t numberOfRxIDs, uint8_t startIndex,
        uint8_t filterCase) {
    uint8_t retVal = 0;
    uint8_t i = startIndex;
    while (i < numberOfRxIDs) {
        if ((filterCase  ==  ID_16BIT_FIFO0 && can_RxMsgs[i].mask == 0U) && IS_CAN_STDID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0)) {
            retVal = i;
            break;
        } else if ((filterCase == ID_16BIT_FIFO1) && (can_RxMsgs[i].mask == 0U) && IS_CAN_STDID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1)) {
            retVal = i;
            break;
        } else if ((filterCase == ID_32BIT_FIFO0) && (can_RxMsgs[i].mask == 0U) && !IS_CAN_STDID(can_RxMsgs[i].ID) && IS_CAN_EXTID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0)) {
            retVal = i;
            break;
        } else if ((filterCase == ID_32BIT_FIFO1) && (can_RxMsgs[i].mask == 0U) && !IS_CAN_STDID(can_RxMsgs[i].ID) && IS_CAN_EXTID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1)) {
            retVal = i;
            break;
        } else if ((filterCase == MSK_16BIT_FIFO0) && (can_RxMsgs[i].mask > 0U) && IS_CAN_STDID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO0)) {
            retVal = i;
            break;
        } else if ((filterCase == MSK_16BIT_FIFO1) && (can_RxMsgs[i].mask > 0U) && IS_CAN_STDID(can_RxMsgs[i].ID) && (can_RxMsgs[i].fifo == CAN_FILTER_FIFO1)) {
            retVal = i;
            break;
        } else if ((filterCase == MSK_32BIT) && (can_RxMsgs[i].mask > 0U) && !IS_CAN_STDID(can_RxMsgs[i].ID) && IS_CAN_EXTID(can_RxMsgs[i].ID)) {
            retVal = i;
            break;
        }
        i++;
    }
    return retVal;
}

/* ***************************************
 *  Interrupt handling
 ****************************************/

/**
  * @brief  Transmission Mailbox 0 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
#if CAN0_USE_TX_BUFFER
    if (hcan->Instance  ==  CAN2) {
        CAN_TxCpltCallback(CAN_NODE0);
    }
#endif /* CAN0_USE_TX_BUFFER */
#if CAN1_USE_TX_BUFFER
    /* No need for callback, if no buffer is used */
    if (hcan->Instance  ==  CAN1) {
        /* Transmission complete callback */
        CAN_TxCpltCallback(CAN_NODE1);
    }
#endif /* CAN1_USE_TX_BUFFER */
}

/**
  * @brief  Transmission Mailbox 1 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
#if CAN0_USE_TX_BUFFER
    if (hcan->Instance  ==  CAN2) {
        CAN_TxCpltCallback(CAN_NODE0);
    }
#endif /* CAN0_USE_TX_BUFFER */
#if CAN1_USE_TX_BUFFER
    /* No need for callback, if no buffer is used */
    if (hcan->Instance  ==  CAN1) {
        /* Transmission complete callback */
        CAN_TxCpltCallback(CAN_NODE1);
    }
#endif /* CAN1_USE_TX_BUFFER */
}

/**
  * @brief  Transmission Mailbox 2 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
#if CAN0_USE_TX_BUFFER
    if (hcan->Instance  ==  CAN2) {
        CAN_TxCpltCallback(CAN_NODE0);
    }
#endif /* CAN0_USE_TX_BUFFER */
#if CAN1_USE_TX_BUFFER
    /* No need for callback, if no buffer is used */
    if (hcan->Instance  ==  CAN1) {
        /* Transmission complete callback */
        CAN_TxCpltCallback(CAN_NODE1);
    }
#endif /* CAN1_USE_TX_BUFFER */
}

/**
  * @brief  Rx FIFO 0 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    /* Call callback function to interpret or save RX message in buffer */
    if (hcan->Instance  ==  CAN2) {
        CAN_RxMsg(CAN_NODE0, hcan, CAN_RX_FIFO0);        /* change towards HAL_CAN_IRQHandler */
    }
    if (hcan->Instance  ==  CAN1) {
        CAN_RxMsg(CAN_NODE1, hcan, CAN_RX_FIFO0);        /* change towards HAL_CAN_IRQHandler */
    }
}

/**
  * @brief  Rx FIFO 1 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    /* Call callback function to interpret or save RX message in buffer */
    if (hcan->Instance  ==  CAN2) {
        CAN_RxMsg(CAN_NODE0, hcan, CAN_RX_FIFO1);        /* change towards HAL_CAN_IRQHandler */
    }
    if (hcan->Instance  ==  CAN1) {
        CAN_RxMsg(CAN_NODE1, hcan, CAN_RX_FIFO1);        /* change towards HAL_CAN_IRQHandler */
    }
}

/**
  * @brief  Error CAN callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    CAN_ERROR_s* errorStruct = NULL_PTR;

    if (hcan->Instance == CAN1) {
#if CAN_USE_CAN_NODE1 == 1
        errorStruct = &CAN1_errorStruct;
#endif /* CAN_USE_CAN_NODE1 == 1 */
    } else {
#if CAN_USE_CAN_NODE0 == 1
        errorStruct = &CAN0_errorStruct;
#endif /* CAN_USE_CAN_NODE0 == 1 */
    }

    /* Check Error Warning flag and set error codes */
    if (errorStruct != NULL_PTR) {
        /* Check Error Warning Flag */
        if ((hcan->ErrorCode & HAL_CAN_ERROR_EWG) != 0) {
            /* This bit is set by hardware when the warning limit has been
             * reached (Receive Error Counter or Transmit Error Counter>=96
             * until error counter 127 write error frames dominant on can bus
             * increment error occurrence of error warning state */
            errorStruct->canErrorCounter[0]++;
        }

        /* Check Error Passive Flag */
        if ((hcan->ErrorCode & HAL_CAN_ERROR_EPV) != 0) {
            /* This bit is set by hardware when the Error Passive limit has
             * been reached (Receive Error Counter or Transmit Error Counter
             * > 127) write error frames recessive on can bus increment error
             * occurrence of error passive state */
            errorStruct->canErrorCounter[1]++;
        }
        /* Check Bus-Off Flag */
        if ((hcan->ErrorCode & HAL_CAN_ERROR_BOF) != 0) {
            /* This bit is set by hardware when it enters the bus-off state.
             * The bus-off state is entered on TEC overflow, greater than 255
             * increment error occurrence of bus-off state */
            errorStruct->canErrorCounter[2]++;
        }
        /* Check stuff error flag */
        if ((hcan->ErrorCode & HAL_CAN_ERROR_STF) != 0) {
            /* When five consecutive bits of the same level have been
             * transmitted by a node, it will add a sixth bit of the opposite
             * level to the outgoing bit stream. The receivers will remove this
             * extra bit.This is done to avoid excessive DC components on the
             * bus, but it also gives the receivers an extra opportunity to
             * detect errors: if more than five consecutive bits of the same
             * level occurs on the bus, a Stuff Error is signaled. */
            errorStruct->canErrorCounter[3]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_FOR) != 0) {
            /* FORM ERROR --- Some parts of the CAN message have a fixed format,
             * i.e. the standard defines exactly what levels must occur and
             * when. (Those parts are the CRC Delimiter, ACK Delimiter, End of
             * Frame, and also the Intermission, but there are some extra
             * special error checking rules for that.) If a CAN controller
             * detects an invalid value in one of these fixed fields, a Form
             * Error is signaled. */
            errorStruct->canErrorCounter[4]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_ACK) != 0) {
            /* ACKNOWLEDGMENT ERROR --- All nodes on the bus that correctly
             * receives a message (regardless of their being interested of its
             * contents or not) are expected to send a dominant level in the
             * so-called Acknowledgement Slot in the message. The transmitter
             * will transmit a recessive level here. If the transmitter can
             * detect a dominant level in the ACK slot, an Acknowledgement
             * Error is signaled. */
            errorStruct->canErrorCounter[5]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_BR) != 0) {
            /* BIT RECESSIVE ERROR --- Each transmitter on the CAN bus monitors
             * (i.e. reads back) the transmitted signal level. If the bit level
             * actually read differs from the one transmitted, a Bit Error (No
             * bit error is raised during the arbitration process.) */
            errorStruct->canErrorCounter[6]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_BD) != 0) {
            /* BIT DOMINANT ERROR --- Each transmitter on the CAN bus monitors
             * (i.e. reads back) the transmitted signal level. If the bit level
             * actually read differs from the one transmitted, a Bit Error (No
             *  bit error is raised during the arbitration process.) */
            errorStruct->canErrorCounter[7]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_CRC) != 0) {
            /* CRC ERROR --- Each message features a 15-bit Cyclic Redundancy
             * Checksum (CRC), and any node that detects a different CRC in the
             * message than what it has calculated itself will signal an CRC
             * Error. */
            errorStruct->canErrorCounter[8]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_RX_FOV0) != 0) {
            /* Rx FIFO0 overrun error */
            errorStruct->canErrorCounter[9]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_RX_FOV1) != 0) {
            /* Rx FIFO1 overrun error */
            errorStruct->canErrorCounter[10]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST0) != 0) {
            /* TxMailbox 0 transmit failure due to arbitration lost */
            errorStruct->canErrorCounter[11]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR0) != 0) {
            /* TxMailbox 1 transmit failure due to transmit error */
            errorStruct->canErrorCounter[12]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST1) != 0) {
            /* TxMailbox 0 transmit failure due to arbitration lost */
            errorStruct->canErrorCounter[13]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR1) != 0) {
            /* TxMailbox 1 transmit failure due to transmit error */
            errorStruct->canErrorCounter[14]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST2) != 0) {
            /* TxMailbox 0 transmit failure due to arbitration lost */
            errorStruct->canErrorCounter[15]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR2) != 0) {
            /* TxMailbox 1 transmit failure due to transmit error */
            errorStruct->canErrorCounter[16]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_TIMEOUT) != 0) {
            /* Timeout error */
            errorStruct->canErrorCounter[17]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_NOT_INITIALIZED) != 0) {
            /* Peripheral not initialized */
            errorStruct->canErrorCounter[18]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_NOT_READY) != 0) {
            /* Peripheral not ready */
            errorStruct->canErrorCounter[19]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_NOT_STARTED) != 0) {
            /* Peripheral not started */
            errorStruct->canErrorCounter[20]++;
        }
        if ((hcan->ErrorCode & HAL_CAN_ERROR_PARAM) != 0) {
            /* Parameter error */
            errorStruct->canErrorCounter[21]++;
        }
    }
}

/**
 * @brief  Transmission complete callback in non blocking mode
 *
 * @param  canNode: canNode that transmitted a message
 *
 * @retval none (void)
 */
static void CAN_TxCpltCallback(CAN_NodeTypeDef_e canNode) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    CAN_TX_BUFFER_s* can_txbuffer = NULL;

    if (canNode  ==  CAN_NODE0) {
#if CAN_USE_CAN_NODE0 == 1
        can_txbuffer = &can0_txbuffer;
#endif /* #if CAN_USE_CAN_NODE0 == 1 */
    } else if (canNode  ==  CAN_NODE1) {
#if CAN_USE_CAN_NODE1 == 1
        can_txbuffer = &can1_txbuffer;
#endif /* CAN_USE_CAN_NODE1 == 1 */
    }
    /* Transmit buffer existing, check if message is ready for transmission */
    if (can_txbuffer != NULL) {
        /* No Error during start of transmission */
        if ((can_txbuffer->ptrWrite  ==  can_txbuffer->ptrRead)
                && (can_txbuffer->buffer[can_txbuffer->ptrRead].newMsg == 0)) {
            /* nothing to transmit, buffer is empty */
            retVal = E_NOT_OK;
        } else {
            retVal = CAN_TxMsgBuffer(canNode);
            if (retVal  !=  E_OK) {
                /* Error during transmission, retransmit message later */
                retVal = E_NOT_OK;
            }
        }
    } else {
        /* no transmit buffer active */
        retVal = E_NOT_OK;
    }
}



/* ***************************************
 *  Transmit message
 ****************************************/

STD_RETURN_TYPE_e CAN_TxMsg(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* ptrMsgData, uint32_t msgLength,
        uint32_t RTR) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    CAN_TxHeaderTypeDef canMessage;
    CAN_HandleTypeDef *ptrHcan;
    uint32_t freeMailboxes = 0;
    uint32_t *ptrMailbox = NULL_PTR;

    if (canNode  ==  CAN_NODE0) {
        if (canNode0_listenonly_mode) {
            ptrHcan = NULL;
        } else {
            /* Check if at least one mailbox is free */
            freeMailboxes = HAL_CAN_GetTxMailboxesFreeLevel(&hcan0);
            if (freeMailboxes != 0) {
                ptrHcan = &hcan0;
            } else {
                ptrHcan = NULL;
            }
        }
    } else if (canNode  ==  CAN_NODE1) {
        if (canNode1_listenonly_mode) {
            ptrHcan = NULL;
        } else {
            /* Check if at least one mailbox is free */
            freeMailboxes = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
            if (freeMailboxes != 0) {
                ptrHcan = &hcan1;
            } else {
                ptrHcan = NULL;
            }
        }
    } else {
        ptrHcan = NULL;
    }

    if ((IS_CAN_STDID(msgID) || IS_CAN_EXTID(msgID)) && IS_CAN_DLC(msgLength) && ptrHcan != NULL) {
        if (IS_CAN_STDID(msgID)) {
            canMessage.StdId = msgID;
            canMessage.IDE = CAN_ID_STD;
        } else {
            canMessage.ExtId = msgID;
            canMessage.IDE = CAN_ID_EXT;
        }
        canMessage.DLC = msgLength;
        canMessage.RTR = RTR;
        canMessage.TransmitGlobalTime = DISABLE;

        /* Copy message in TX mailbox and transmit it */
        HAL_CAN_AddTxMessage(ptrHcan, &canMessage, ptrMsgData, ptrMailbox);
    } else {
        retVal = E_NOT_OK;
    }
    return retVal;
}


STD_RETURN_TYPE_e CAN_Send(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* ptrMsgData, uint32_t msgLength,
        uint32_t RTR) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint8_t tmptxbuffer_wr = 0;
    CAN_TX_BUFFER_s* can_txbuffer = NULL_PTR;

    if (canNode  ==  CAN_NODE0) {
#if CAN_USE_CAN_NODE0 == 1
        can_txbuffer = &can0_txbuffer;
#endif /* #if CAN_USE_CAN_NODE0 == 1 */
    } else if (canNode  ==  CAN_NODE1) {
#if CAN_USE_CAN_NODE1 == 1
        can_txbuffer = &can1_txbuffer;
#endif /* #if CAN_USE_CAN_NODE1 == 1 */
    }

    /* Transmit buffer exisiting */
    if (can_txbuffer != NULL_PTR) {
        tmptxbuffer_wr = can_txbuffer->ptrWrite;

        if (tmptxbuffer_wr  ==  can_txbuffer->ptrRead) {
            if (can_txbuffer->buffer[tmptxbuffer_wr].newMsg  ==  0) {
                /* free buffer space for message */

                can_txbuffer->ptrWrite++;
                can_txbuffer->ptrWrite = can_txbuffer->ptrWrite % can_txbuffer->length;
                retVal = E_OK;
            } else {
                /* buffer full */
                retVal = E_NOT_OK;
            }
        } else {
            can_txbuffer->ptrWrite++;
            can_txbuffer->ptrWrite = can_txbuffer->ptrWrite % can_txbuffer->length;
            retVal = E_OK;
        }
    } else {
        retVal = E_NOT_OK;
    }

    if ((can_txbuffer != NULL_PTR) &&
        (retVal  ==  E_OK) &&
        (IS_CAN_STDID(msgID) || IS_CAN_EXTID(msgID)) &&
        (IS_CAN_DLC(msgLength))) {
        /* if buffer free and valid CAN identifier */

        if (IS_CAN_STDID(msgID)) {
            can_txbuffer->buffer[tmptxbuffer_wr].msg.StdId = msgID;
            can_txbuffer->buffer[tmptxbuffer_wr].msg.IDE = 0;
        } else {
            can_txbuffer->buffer[tmptxbuffer_wr].msg.ExtId = msgID;
            can_txbuffer->buffer[tmptxbuffer_wr].msg.IDE = 1;
        }
        can_txbuffer->buffer[tmptxbuffer_wr].newMsg = 1;
        can_txbuffer->buffer[tmptxbuffer_wr].msg.RTR = RTR;
        can_txbuffer->buffer[tmptxbuffer_wr].msg.DLC = msgLength;   /* Data length of the frame that will be transmitted */
        can_txbuffer->buffer[tmptxbuffer_wr].msg.TransmitGlobalTime = DISABLE;

        /* copy message data in handle transmit structure */
        can_txbuffer->buffer[tmptxbuffer_wr].data[0] = ptrMsgData[0];
        can_txbuffer->buffer[tmptxbuffer_wr].data[1] = ptrMsgData[1];
        can_txbuffer->buffer[tmptxbuffer_wr].data[2] = ptrMsgData[2];
        can_txbuffer->buffer[tmptxbuffer_wr].data[3] = ptrMsgData[3];
        can_txbuffer->buffer[tmptxbuffer_wr].data[4] = ptrMsgData[4];
        can_txbuffer->buffer[tmptxbuffer_wr].data[5] = ptrMsgData[5];
        can_txbuffer->buffer[tmptxbuffer_wr].data[6] = ptrMsgData[6];
        can_txbuffer->buffer[tmptxbuffer_wr].data[7] = ptrMsgData[7];

        retVal = E_OK;
    } else {
        retVal = E_NOT_OK;
    }

    return retVal;
}


STD_RETURN_TYPE_e CAN_TxMsgBuffer(CAN_NodeTypeDef_e canNode) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    HAL_StatusTypeDef retHal = HAL_ERROR;
    CAN_TX_BUFFER_s* can_txbuffer = NULL;
    CAN_HandleTypeDef* ptrHcan = NULL;
    uint32_t freeMailboxes = 0;
    uint32_t *ptrMailbox = NULL_PTR;

    if (canNode  ==  CAN_NODE0) {
#if CAN_USE_CAN_NODE0 == 1
        if (!canNode0_listenonly_mode) {
            can_txbuffer = &can0_txbuffer;
            ptrHcan = &hcan0;
        }
#endif /* #if CAN_USE_CAN_NODE0 == 1 */
    } else if (canNode  ==  CAN_NODE1) {
#if CAN_USE_CAN_NODE1 == 1
        if (!canNode1_listenonly_mode) {
            can_txbuffer = &can1_txbuffer;
            ptrHcan = &hcan1;
        }
#endif /* CAN_USE_CAN_NODE1 == 1 */
    }
    /* Check if at least one mailbox is free */
    freeMailboxes = HAL_CAN_GetTxMailboxesFreeLevel(&hcan0);

    if ((can_txbuffer != NULL) && (freeMailboxes != 0)) {
        if ((can_txbuffer->ptrWrite  ==  can_txbuffer->ptrRead)
                && (can_txbuffer->buffer[can_txbuffer->ptrRead].newMsg  ==  0)) {
            /* nothing to transmit, buffer is empty */
            retVal = E_NOT_OK;
        } else {
            /* Copy message into TX mailbox and transmit it */
            retHal = HAL_CAN_AddTxMessage(ptrHcan, &can_txbuffer->buffer[can_txbuffer->ptrRead].msg, can_txbuffer->buffer[can_txbuffer->ptrRead].data, ptrMailbox);

            if (retHal == HAL_OK) {
                /* No Error during start of transmission */
                can_txbuffer->buffer[can_txbuffer->ptrRead].newMsg = 0;    /* Msg is sent, set newMsg to 0, to allow writing of new data in buffer space */
                can_txbuffer->ptrRead++;
                can_txbuffer->ptrRead = can_txbuffer->ptrRead % can_txbuffer->length;
                retVal = E_OK;
            } else {
                retVal = E_NOT_OK;        /* Error during transmission, retransmit message later */
            }
        }
    } else {
        /* no transmit buffer active or TX mailboxes full */
        retVal = E_NOT_OK;
    }
    return retVal;
}

/* ***************************************
 *  Receive message
 ****************************************/

/**
 * @brief  Receives CAN messages and stores them either in RxBuffer or in hcan
 *
 * @param  canNode:    canNode which received the message
 * @param  ptrHcan:    pointer to a CAN_HandleTypeDef structure that contains
 *                     the message information of the specified CAN.
 * @param  FIFONumber: FIFO in which the message has been received
 * @retval none (void)
 */
static void CAN_RxMsg(CAN_NodeTypeDef_e canNode, CAN_HandleTypeDef* ptrHcan, uint8_t FIFONumber) {
    uint8_t bypassLinkIndex = 0;
    CAN_RX_BUFFERELEMENT_s tmpMsgBuffer;
    uint32_t msgID = 0;

#if CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER
    uint32_t* can_bufferbypass_rxmsgs = NULL;
    uint32_t bufferbypasslength = 0;
    CAN_RX_BUFFER_s* can_rxbuffer = NULL;
    CAN_MSG_RX_TYPE_s* can_rxmsgs = NULL;
    uint8_t* can_fastLinkIndex = NULL;
#endif /* CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER */

    /* Set pointer on respective RxBuffer */
    if (canNode  ==  CAN_NODE1) {
#if CAN1_USE_RX_BUFFER && CAN_USE_CAN_NODE1 == 1
        can_rxbuffer = &can1_rxbuffer;
#if (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0u)
        can_rxmsgs = &can1_RxMsgs[0];
        can_bufferbypass_rxmsgs = &can1_bufferBypass_RxMsgs[0];
        bufferbypasslength = CAN1_BUFFER_BYPASS_NUMBER_OF_IDs;
        can_fastLinkIndex = &can1_fastLinkIndex[0];
#endif /* (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0u) */
#endif /* CAN1_USE_RX_BUFFER && CAN_USE_CAN_NODE1 == 1 */
    } else if (canNode  ==  CAN_NODE0) {
#if CAN0_USE_RX_BUFFER && CAN_USE_CAN_NODE0 == 1
        can_rxbuffer = &can0_rxbuffer;
#if (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0U)
        can_rxmsgs = &can0_RxMsgs[0];
        can_bufferbypass_rxmsgs = &can0_bufferBypass_RxMsgs[0];
        bufferbypasslength = CAN0_BUFFER_BYPASS_NUMBER_OF_IDs;
        can_fastLinkIndex = &can0_fastLinkIndex[0];
#endif /* (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0u) */
#endif /* CAN0_USE_RX_BUFFER && CAN_USE_CAN_NODE0 == 1 */
    }

    /* Get message ID */
    HAL_CAN_GetRxMessage(ptrHcan, FIFONumber, &tmpMsgBuffer.msg , &tmpMsgBuffer.data[0]);

#if (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) || (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0)
    if (can_bufferbypass_rxmsgs != NULL) {
        /* only needed when messages are bypassed */

        for (bypassLinkIndex = 0; bypassLinkIndex < bufferbypasslength; bypassLinkIndex++) {
            if ((tmpMsgBuffer.msg.StdId == can_bufferbypass_rxmsgs[bypassLinkIndex]) ||
                (tmpMsgBuffer.msg.ExtId == can_bufferbypass_rxmsgs[bypassLinkIndex])) {
                break;
            }
        }
    }
#endif /* #if (CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) || (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0) */
    if (bypassLinkIndex >= bufferbypasslength && can_rxbuffer != NULL) {
        /* ##### Use buffer / Copy data in buffer ##### */

#if CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER
        /* NO NEED TO DISABLE INTERRUPTS, BECAUSE FUNCTION IS CALLED FROM ISR */

        /* Set to 1 to mark message as new received. Set to 0 when reading message from buffer */
        can_rxbuffer->buffer[can_rxbuffer->ptrWrite].newMsg = 1;

        /* Get message header */
        can_rxbuffer->buffer[can_rxbuffer->ptrWrite].msg = tmpMsgBuffer.msg;

        /* Get the data field */
        for (uint8_t i = 0; i < can_rxbuffer->buffer[can_rxbuffer->ptrWrite].msg.DLC; i++) {
            can_rxbuffer->buffer[can_rxbuffer->ptrWrite].data[i] = tmpMsgBuffer.data[i];
        }

        /* Increment write pointer */
        can_rxbuffer->ptrWrite++;
        can_rxbuffer->ptrWrite = can_rxbuffer->ptrWrite % can_rxbuffer->length;
#endif /* CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER */
    } else if (bypassLinkIndex < bufferbypasslength && can_rxmsgs != NULL && can_fastLinkIndex != NULL) {
        /* ##### Buffer active but bypassed ##### */

#if ((CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) || (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0)) && ((CAN_USE_CAN_NODE0 == 1) || (CAN_USE_CAN_NODE1 == 1))
        /* call buffer bypass callback function */
        if (tmpMsgBuffer.msg.IDE == 0U) {
            if (can_rxmsgs[can_fastLinkIndex[bypassLinkIndex]].func != NULL) {
                can_rxmsgs[can_fastLinkIndex[bypassLinkIndex]].func(tmpMsgBuffer.msg.StdId,
                                                                    tmpMsgBuffer.data,
                                                                    tmpMsgBuffer.msg.DLC,
                                                                    tmpMsgBuffer.msg.RTR);
            } else {
                /* No callback function defined */
                CAN_BufferBypass(canNode, tmpMsgBuffer.msg.StdId, tmpMsgBuffer.data, tmpMsgBuffer.msg.DLC, tmpMsgBuffer.msg.RTR);
            }
        } else {
            if (can_rxmsgs[can_fastLinkIndex[bypassLinkIndex]].func != NULL) {
                can_rxmsgs[can_fastLinkIndex[bypassLinkIndex]].func(tmpMsgBuffer.msg.ExtId,
                                                                    tmpMsgBuffer.data,
                                                                    tmpMsgBuffer.msg.DLC,
                                                                    tmpMsgBuffer.msg.RTR);
            } else {
                /* No callback function defined */
                CAN_BufferBypass(canNode, tmpMsgBuffer.msg.ExtId, tmpMsgBuffer.data, tmpMsgBuffer.msg.DLC, tmpMsgBuffer.msg.RTR);
            }
        }
#endif /* ((CAN1_BUFFER_BYPASS_NUMBER_OF_IDs > 0) || (CAN0_BUFFER_BYPASS_NUMBER_OF_IDs > 0)) && ((CAN_USE_CAN_NODE0 == 1) || (CAN_USE_CAN_NODE1 == 1)) */
    } else {
        /* ##### Buffer not active ##### */

        CAN_MSG_RX_TYPE_s* msgRXstruct;
        uint8_t length;
        if (canNode == CAN_NODE0) {
            msgRXstruct = &can0_RxMsgs[0];
            length = can_CAN0_rx_length;
        } else {
            msgRXstruct = &can1_RxMsgs[0];
            length = can_CAN1_rx_length;
        }

        if (tmpMsgBuffer.msg.IDE == 0U) {
           msgID = tmpMsgBuffer.msg.StdId;
        } else  {
            msgID = tmpMsgBuffer.msg.ExtId;
        }

        /* Search for correct message in RX message array to check for callback */
        uint8_t rxMsg = 0;
        for (; rxMsg < length; rxMsg++) {
            if (msgRXstruct[rxMsg].ID == msgID) {
                break;
            }
        }

        /* Interpret received message */
        if (msgRXstruct[rxMsg].func != NULL) {
            msgRXstruct[rxMsg].func(msgID, &tmpMsgBuffer.data[0], tmpMsgBuffer.msg.DLC, tmpMsgBuffer.msg.RTR);
        } else {
            CAN_InterpretReceivedMsg(canNode, msgID, &tmpMsgBuffer.data[0], tmpMsgBuffer.msg.DLC, tmpMsgBuffer.msg.RTR);
        }
    }
}


STD_RETURN_TYPE_e CAN_ReceiveBuffer(CAN_NodeTypeDef_e canNode, Can_PduType* msg) {
    /* E_OK is returned, if buffer is empty and interpret function is called successful */
    STD_RETURN_TYPE_e retVal = E_NOT_OK;

#if CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER
    CAN_RX_BUFFER_s* can_rxbuffer = NULL;

#if CAN0_USE_RX_BUFFER && CAN_USE_CAN_NODE0 == 1
    if (canNode  ==  CAN_NODE0) {
        can_rxbuffer = &can0_rxbuffer;
    }
#endif /* CAN0_USE_RX_BUFFER && CAN_USE_CAN_NODE0 == 1 */
#if CAN1_USE_RX_BUFFER && CAN_USE_CAN_NODE1 == 1
    if (canNode  ==  CAN_NODE1) {
        can_rxbuffer = &can1_rxbuffer;
    }
#endif /* CAN1_USE_RX_BUFFER && CAN_USE_CAN_NODE1 == 1 */

    if (msg  ==  NULL) {
        /* null pointer to message data struct */
        can_rxbuffer = NULL;
    }

    if ((can_rxbuffer->ptrWrite != can_rxbuffer->ptrRead) &&
            (can_rxbuffer->buffer[can_rxbuffer->ptrRead].newMsg  ==  1) &&
            (can_rxbuffer != NULL)) {
        /* buffer not empty -> read message */
        if (can_rxbuffer->buffer[can_rxbuffer->ptrRead].msg.IDE == 1) {
            /* Extended ID used */
            msg->id = can_rxbuffer->buffer[can_rxbuffer->ptrRead].msg.ExtId;
        } else {
            msg->id = can_rxbuffer->buffer[can_rxbuffer->ptrRead].msg.StdId;
        }
        msg->dlc = can_rxbuffer->buffer[can_rxbuffer->ptrRead].msg.DLC;

        for (uint8_t i = 0; i < 8U; i++) {
            msg->sdu[i] = can_rxbuffer->buffer[can_rxbuffer->ptrRead].data[i];
        }

        /* Set to 0 to mark buffer entry as read. Set to 1 when writing message into buffer */
        can_rxbuffer->buffer[can_rxbuffer->ptrRead].newMsg = 0;

        /* Move to next buffer element */
        can_rxbuffer->ptrRead++;
        can_rxbuffer->ptrRead = can_rxbuffer->ptrRead % can_rxbuffer->length;
        retVal = E_OK;
    }
#endif /* CAN0_USE_RX_BUFFER || CAN1_USE_RX_BUFFER */
    return retVal;
}

/**
 * @brief  Receives a bypassed CAN message and interprets it
 *
 * @param  canNode: canNode on which the message has been received
 * @param  msgID:   message ID
 * @param  data:    pointer to the message data
 * @param  DLC:     length of received data
 * @param  RTR:     RTR bit of received message
 *
 * @retval E_OK if interpreting was successful, otherwise E_NOT_OK
 */
static STD_RETURN_TYPE_e CAN_BufferBypass(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* rxData, uint8_t DLC,
        uint8_t RTR) {
    STD_RETURN_TYPE_e retVal = E_OK;

    /* ***************************************************************
     *  Implement wished functionality of received messages here,
     *
     *  if no callback function in CAN_MSG_RX_TYPE_s struct is defined
     *****************************************************************/

    /* Perform SW reset */
    if (msgID == CAN_ID_SOFTWARE_RESET_MSG && DLC == 8) {
        uint8_t reset = 0;

        /* CAN data = FF FF FF FF FF FF FF FF */
        for (uint8_t i = 0; i < DLC; i++) {
            if (rxData[i] != 0xFF) {
                reset = 1;
            }
        }
#if CAN_SW_RESET_WITH_DEVICE_ID == 1
/*         CAN data = MCU Device ID Byte [0] [1] [2] [3] [4] [5] [6] [7] */
/*         if (rxData[0] == (uint8_t)mcu_unique_deviceID.off0 && data[1] == (uint8_t)(mcu_unique_deviceID.off0 >> 8) &&
                rxData[2] == (uint8_t)(mcu_unique_deviceID.off0 >> 16) && rxData[3] == (uint8_t)(mcu_unique_deviceID.off0 >> 24) &&
                rxData[4] == (uint8_t)mcu_unique_deviceID.off32 && rxData[5] == (uint8_t)(mcu_unique_deviceID.off32 >> 8) &&
                rxData[6] == (uint8_t)(mcu_unique_deviceID.off32 >> 16) && rxData[7] == (uint8_t)(mcu_unique_deviceID.off32 >> 24)) {
            reset = 1;
        }
*/

        if (rxData[0] == 0) {
            if ((CAN_CheckNodeID(&data[5]) == E_OK) || (CAN_CheckUniqueDeviceID(&data[1]) == E_OK) || (CAN_CheckBroadcastID(&data[5]) == E_OK)) {
                reset = 1;
            }
        }
#else /* CAN_SW_RESET_WITH_DEVICE_ID != 1 */
        reset = 1;
#endif /* CAN_SW_RESET_WITH_DEVICE_ID == 1 */

        if (reset == 1)
            HAL_NVIC_SystemReset();
    }
    return retVal;
}

/**
 * @brief  Interprets the received message
 *
 * @param  canNode: canNode on which the message has been received
 * @param  msgID:   message ID
 * @param  data:    pointer to the message data
 * @param  DLC:     length of received data
 * @param  RTR:     RTR bit of received message
 *
 * @return E_OK if interpretation successful, otherwise E_NOT_OK
 */
static STD_RETURN_TYPE_e CAN_InterpretReceivedMsg(CAN_NodeTypeDef_e canNode, uint32_t msgID, uint8_t* data, uint8_t DLC,
        uint8_t RTR) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;


    /* ***************************************************************
     *  Implement wished functionality of received messages here,
     *
     *  if no callback function in CAN_MSG_RX_TYPE_s struct is defined
     *****************************************************************/
    return retVal;
}

/* ***************************************
 *  Sleep mode
 ****************************************/

void CAN_SetSleepMode(CAN_NodeTypeDef_e canNode) {
    if (canNode  ==  CAN_NODE0) {
        HAL_CAN_RequestSleep(&hcan0);
    } else if (canNode  ==  CAN_NODE1) {
        HAL_CAN_RequestSleep(&hcan1);
    }
    return;
}


void CAN_WakeUp(CAN_NodeTypeDef_e canNode) {
    if (canNode  ==  CAN_NODE0) {
        HAL_CAN_WakeUp(&hcan0);
    } else if (canNode  ==  CAN_NODE1) {
        HAL_CAN_WakeUp(&hcan1);
    }
    return;
}
