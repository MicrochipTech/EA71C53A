/**
 * \file
 *
 * \brief AES Advanced Encryption Standard(Sync) functionality declaration.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef HPL_AES_H_INCLUDED
#define HPL_AES_H_INCLUDED


enum aes_action { AES_DECRYPT, AES_ENCRYPT };

enum aes_keysize { AES_KEY_128, AES_KEY_192, AES_KEY_256 };

typedef enum
{
  SM_SET_KEY_COMMAND = 0,
  SM_ECB_ENCRYPT_COMMAND,
  SM_CBC_ENCRYPT_COMMAND,
  SM_ECB_DECRYPT_COMMAND,
  SM_CBC_DECRYPT_COMMAND,
} HAL_AesCmd_t;

typedef struct
{

  //! AES command.
  HAL_AesCmd_t aesCmd;
  //! Pointer to input data for AES command.
  uint8_t *text;
  //! Size of input/outpu data in blocks.
  uint8_t blockCount; // Block is 128-bit data.

  void (*encryptConf)(void);
} HAL_AES_EncryptReq_t;

void HAL_AES_EncryptReq(HAL_AES_EncryptReq_t *reqParams);
void HAL_SmReset (void);

#endif /* HPL_AES_H_INCLUDED */
