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
 * @file    chksum.h
 * @author  foxBMS Team
 * @date    28.08.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  CHK
 *
 * @brief   Header for the driver for the checksum calculation
 *
 */

#ifndef CHKSUM_H_
#define CHKSUM_H_


/*================== Includes =============================================*/
#include "general.h"
#include "version.h"

/*================== Macros and Definitions ===============================*/


/*================== Constant and Variable Definitions ====================*/

typedef enum {
      CHK_CHECKSUM_FAILED           = 0xFF,
      CHK_CHECKSUM_PASSED          = 0x00,
  /*...*/
} CHK_CHECKSUM_STATUS_e;


typedef struct {
    uint32_t                 checksum; /* 16 bit checksum for validating the Application SW */
    uint32_t                 resetstatus; /*  last reset status */
    CHK_CHECKSUM_STATUS_e    checksumstatus;
} CHK_STATUS_s;

extern CHK_STATUS_s chk_status;
/*================== Function Prototypes ==================================*/

/**
 * @brief CHK_crc32 provides crc32 algorithm.
 *
 * This function provides crc32 checksum functionality with hardware support.
 * It has been modified to match the common implementations used in zlib/WinZip/...
 * RCC_CRC_CLK needs to be enabled.
 *
 * @return (type: uint32_t)
 */
uint32_t CHK_crc32(uint8_t* data, uint32_t len);
extern STD_RETURN_TYPE_e CHK_Flashchecksum(const VER_ValidStruct_s *valid_struct);

/*================== Function Implementations =============================*/






#endif /* CHKSUM_H_ */
