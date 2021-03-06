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

#if defined(__ICCARM__)
#ifdef __FPGA__
#include <pic32cx1012bz25048_fpga.h>
#else // SOC
#include <pic32cx1012bz25048.h>
#endif
#elif defined (__GNUC__)
#include <xc.h>
#endif

#include <hal_aes_sync.h>
#include <halDbg.h>

#define DRIVER_VERSION 0x00000001u


#define ERR_NONE 0
#define ERR_INVALID_DATA -1

static inline void aes_sync_cfb_crypt_first_unaligned_data(const enum aes_action enc, const uint8_t **input,
                                                           uint8_t **output, uint32_t *length, uint8_t *iv,
                                                           uint32_t *iv_ofst, uint32_t block_bytes);

static inline void aes_sync_cfb_crypt_last_unaligned_data(struct aes_sync_descriptor *descr, const enum aes_action enc,
                                                          const uint8_t *input, uint8_t *output, uint32_t length,
                                                          uint8_t *iv, uint32_t *iv_ofst);
/**
 * \brief Initialize AES.
 */
int32_t aes_sync_init(struct aes_sync_descriptor *descr, void *const hw)
{
	halAssert((uint8_t)(descr && hw), HAL_AES);

	return _aes_sync_init(&descr->dev, hw);
}

/**
 * \brief Deinitialize AES.
 */
int32_t aes_sync_deinit(struct aes_sync_descriptor *descr)
{
	halAssert((uint8_t)(descr && 0xff), HAL_AES);

	return _aes_sync_deinit(&descr->dev);
}

/**
 * \brief Enable AES
 */
int32_t aes_sync_enable(struct aes_sync_descriptor *descr)
{
	halAssert((uint8_t)(descr && 0xff), HAL_AES);

	return _aes_sync_enable(&descr->dev);
}

/**
 * \brief Disable AES
 */
int32_t aes_sync_disable(struct aes_sync_descriptor *descr)
{
	halAssert((uint8_t)(descr && 0xff), HAL_AES);

	return _aes_sync_disable(&descr->dev);
}

/**
 * \brief              Set AES Key (encryption)
 */
int32_t aes_sync_set_encrypt_key(struct aes_sync_descriptor *descr, const uint8_t *key, const enum aes_keysize size)
{
	halAssert(descr && key, HAL_AES);
	return _aes_sync_set_key(&descr->dev, key, size);
}

/**
 * \brief              Set AES Key (decryption)
 */
int32_t aes_sync_set_decrypt_key(struct aes_sync_descriptor *descr, const uint8_t *key, const enum aes_keysize size)
{
	halAssert(descr && key, HAL_AES);
	return _aes_sync_set_key(&descr->dev, key, size);
}

/**
 * \brief              AES-ECB block encryption/decryption
 */
int32_t aes_sync_ecb_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                           uint8_t *output)
{
	halAssert(descr && input && output, HAL_AES);
	return _aes_sync_ecb_crypt(&descr->dev, enc, input, output);
}

/**
 * \brief              AES-CBC block encryption/decryption
 */
int32_t aes_sync_cbc_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                           uint8_t *output, uint32_t length, uint8_t iv[16])
{
	halAssert(descr && input && output && length && iv, HAL_AES);
	return _aes_sync_cbc_crypt(&descr->dev, enc, input, output, length, iv);
}

/**
 * \brief  AES-CFB encryption/decryption first block unaligned data
 */
static inline void aes_sync_cfb_crypt_first_unaligned_data(const enum aes_action enc, const uint8_t **input,
                                                           uint8_t **output, uint32_t *length, uint8_t *iv,
                                                           uint32_t *iv_ofst, uint32_t block_bytes)
{
	if (*iv_ofst) {
		while (((*iv_ofst) < block_bytes) && *length) {
			**output = (**input) ^ iv[*iv_ofst];
			/* Update IV for next crypt */
			iv[*iv_ofst] = (enc == AES_ENCRYPT) ? (**output) : (**input);
			(*input)++;
			(*output)++;
			(*iv_ofst)++;
			(*length)--;
		}
		*iv_ofst &= (block_bytes - 1);
	}
}

/**
 * \brief  AES-CFB encryption/decryption last block unaligned data
 */
static inline void aes_sync_cfb_crypt_last_unaligned_data(struct aes_sync_descriptor *descr, const enum aes_action enc,
                                                          const uint8_t *input, uint8_t *output, uint32_t length,
                                                          uint8_t *iv, uint32_t *iv_ofst)
{
	if (length) {
		_aes_sync_ecb_crypt(&descr->dev, AES_ENCRYPT, iv, iv);
		while ((*iv_ofst) < length) {
			*output = (*input) ^ iv[*iv_ofst];
			/* Update IV for next crypt */
			iv[*iv_ofst] = (enc == AES_ENCRYPT) ? (*output) : (*input);
			input++;
			output++;
			(*iv_ofst)++;
		}
	}
}

/**
 * \brief              AES-CFB128 block encryption/decryption
 */
int32_t aes_sync_cfb128_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                              uint8_t *output, uint32_t length, uint8_t *iv, uint32_t *iv_ofst)
{
	halAssert(descr && input && output && length && iv && iv_ofst && *iv_ofst < 16, HAL_AES);

	aes_sync_cfb_crypt_first_unaligned_data(enc, &input, &output, &length, iv, iv_ofst, 16);
	/* if left length more than 1 block, then use cfb128 encrypt */
	if (length >> 4) {
		_aes_sync_cfb128_crypt(&descr->dev, enc, input, output, (length & ~0xF), iv);
		input += (length & ~0xF);
		output += (length & ~0xF);
		length = length & 0xF;
	}
	aes_sync_cfb_crypt_last_unaligned_data(descr, enc, input, output, length, iv, iv_ofst);

	return ERR_NONE;
}

/**
 * \brief              AES-CFB64 block encryption/decryption
 */
int32_t aes_sync_cfb64_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                             uint8_t *output, uint32_t length, uint8_t *iv, uint32_t *iv_ofst)
{
	halAssert(descr && input && output && length && iv && iv_ofst && *iv_ofst < 8, HAL_AES);

	aes_sync_cfb_crypt_first_unaligned_data(enc, &input, &output, &length, iv, iv_ofst, 8);
	/* if left length more than 1 block, then use cfb64 encrypt */
	if (length >> 3) {
		_aes_sync_cfb64_crypt(&descr->dev, enc, input, output, (length & ~0x7), iv);
		input += (length & ~0x7);
		output += (length & ~0x7);
		length = length & 0x7;
	}
	aes_sync_cfb_crypt_last_unaligned_data(descr, enc, input, output, length, iv, iv_ofst);

	return ERR_NONE;
}

/**
 * \brief              AES-CFB32 block encryption/decryption
 *
 * \param[in]  desc    AES descriptor
 * \param[in]  enc     AES_SYNC_ENCRYPT or AES_SYNC_DECRYPT
 * \param[in]  input   buffer holding the input data
 * \param[out] output  buffer holding the output data
 * \param[out] length  byte length of the input data
 * \param[in, out] iv      initialization Vector (updated after use)
 * \param[in, out] iv_ofst  offset in IV (updated after use)
 *
 * \return             ERR_NONE if successful
 */
int32_t aes_sync_cfb32_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                             uint8_t *output, uint32_t length, uint8_t *iv, uint32_t *iv_ofst)
{
	halAssert(descr && input && output && length && iv && iv_ofst && *iv_ofst < 4, HAL_AES);

	aes_sync_cfb_crypt_first_unaligned_data(enc, &input, &output, &length, iv, iv_ofst, 4);
	/* if left length more than 1 block, then use cfb32 encrypt */
	if (length >> 2) {
		_aes_sync_cfb32_crypt(&descr->dev, enc, input, output, (length & ~0x3), iv);
		input += (length & ~0x3);
		output += (length & ~0x3);
		length = length & 0x3;
	}
	aes_sync_cfb_crypt_last_unaligned_data(descr, enc, input, output, length, iv, iv_ofst);

	return ERR_NONE;
}

/**
 * \brief              AES-CFB16 block encryption/decryption
 */
int32_t aes_sync_cfb16_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                             uint8_t *output, uint32_t length, uint8_t *iv, uint32_t *iv_ofst)
{
	halAssert(descr && input && output && length && iv && iv_ofst && *iv_ofst < 2, HAL_AES);

	aes_sync_cfb_crypt_first_unaligned_data(enc, &input, &output, &length, iv, iv_ofst, 2);

	/* if left length more than 1 block, then use cfb16 encrypt */
	if (length >> 1) {
		_aes_sync_cfb16_crypt(&descr->dev, enc, input, output, (length & ~0x1), iv);
		input += (length & ~0x1);
		output += (length & ~0x1);
		length = length & 0x1;
	}
	aes_sync_cfb_crypt_last_unaligned_data(descr, enc, input, output, length, iv, iv_ofst);

	return ERR_NONE;
}

/**
 * \brief              AES-CFB8 block encryption/decryption
 */
int32_t aes_sync_cfb8_crypt(struct aes_sync_descriptor *descr, const enum aes_action enc, const uint8_t *input,
                            uint8_t *output, uint32_t length, uint8_t *iv)
{
	halAssert(descr && input && output && length && iv,HAL_AES);
	return _aes_sync_cfb8_crypt(&descr->dev, enc, input, output, length, iv);
}

/**
 * \brief              AES-OFB block encryption/decryption
 */
int32_t aes_sync_ofb_crypt(struct aes_sync_descriptor *descr, const uint8_t *input, uint8_t *output, uint32_t length,
                           uint8_t *iv, uint32_t *iv_ofst)
{
	halAssert(descr && input && output && length && iv && iv_ofst && *iv_ofst < 16, HAL_AES);

	/* Encrypt first unaligned block data */
	if (*iv_ofst) {
		while ((*iv_ofst) < 16 && length) {
			*output = (uint8_t)((*input) ^ iv[*iv_ofst]);
			input++;
			output++;
			(*iv_ofst)++;
			length--;
		}
		*iv_ofst &= 0xF;
	}

	/* if left length more than 1 block, then use cfb128 encrypt */
	if (length >> 4) {
		_aes_sync_ofb_crypt(&descr->dev, input, output, (length & ~0xF), iv);
		input += (length & ~0xF);
		output += (length & ~0xF);
		length   = length & 0xF;
		*iv_ofst = 0;
	}
	/* Encrypt last unaligned data and update iv */
	if (length) {
		_aes_sync_ecb_crypt(&descr->dev, AES_ENCRYPT, iv, iv);
		while ((*iv_ofst) < length) {
			*output = (*input) ^ iv[*iv_ofst];
			input++;
			output++;
			(*iv_ofst)++;
		}
	}
	return ERR_NONE;
}

/**
 * \brief              AES-CTR block encryption/decryption
 */
int32_t aes_sync_ctr_crypt(struct aes_sync_descriptor *descr, const uint8_t *input, uint8_t *output, uint32_t length,
                           uint8_t buffer[16], uint8_t nc[16], uint32_t *nc_ofst)
{
	int32_t i;
	halAssert(descr && input && output && length && nc && nc_ofst && *nc_ofst < 16, HAL_AES);

	/* Encrypt first unaligned block data */
	if (*nc_ofst) {
		while ((*nc_ofst) < 16 && length) {
			*output++ = (*input) ^ buffer[*nc_ofst];
			input++;
			(*nc_ofst)++;
			length--;
		}
		*nc_ofst &= 0xF;
	}
	/* if left length more than 1 block, then use ctr encrypt */
	if (length >> 4) {
		_aes_sync_ctr_crypt(&descr->dev, input, output, (length & ~0xF), nc);
		input += (length & ~0xF);
		output += (length & ~0xF);
		length = length & 0xF;
	}
	/* Encrypt last unaligned data and update buffer */
	if (length) {
		_aes_sync_ecb_crypt(&descr->dev, AES_ENCRYPT, nc, buffer);
		for (i = 16; i > 0; i--) {
			if (++nc[i - 1] != 0)
				break;
		}
		while (*nc_ofst < length) {
			*output++ = (*input) ^ buffer[*nc_ofst];
			input++;
			(*nc_ofst)++;
		}
	}

	return ERR_NONE;
}

/**
 * \brief              AES-GCM block encryption/decryption
 */
int32_t aes_sync_gcm_crypt_and_tag(struct aes_sync_descriptor *const descr, const enum aes_action enc,
                                   const uint8_t *input, uint8_t *output, uint32_t length, const uint8_t *iv,
                                   uint32_t iv_len, const uint8_t *aad, uint32_t aad_len, uint8_t *tag,
                                   uint32_t tag_len)
{
	halAssert(descr && iv && iv_len, HAL_AES);
	halAssert((input && output && length) || (!length), HAL_AES);
	halAssert(((aad && aad_len) || !aad_len), HAL_AES);
	halAssert((tag && tag_len && (tag_len <= 16)) || !tag_len, HAL_AES);
	return _aes_sync_gcm_crypt_and_tag(&descr->dev, enc, input, output, length, iv, iv_len, aad, aad_len, tag, tag_len);
}

/**
 * \brief              AES-GCM block encryption
 */
int32_t aes_sync_gcm_auth_decrypt(struct aes_sync_descriptor *const descr, const uint8_t *input, uint8_t *output,
                                  uint32_t length, const uint8_t *iv, uint32_t iv_len, const uint8_t *aad,
                                  uint32_t aad_len, const uint8_t *tag, uint32_t tag_len)
{
	uint8_t  vtag[16];
	uint32_t index;
	int32_t  result;

	halAssert(descr && iv && iv_len, HAL_AES);
	halAssert((input && output && length) || (!length), HAL_AES);
	halAssert(((aad && aad_len) || !aad_len), HAL_AES);
	halAssert((tag && tag_len && (tag_len <= 16)) || !tag_len, HAL_AES);

	result = _aes_sync_gcm_crypt_and_tag(
	    &descr->dev, AES_DECRYPT, input, output, length, iv, iv_len, aad, aad_len, vtag, tag_len);
	if (result != ERR_NONE) {
		return result;
	}
	for (index = 0; index < tag_len; index++) {
		if (vtag[index] != tag[index]) {
			return ERR_INVALID_DATA;
		}
	}
	return ERR_NONE;
}

/**
 * \brief              AES-GCM block start
 */
int32_t aes_sync_gcm_start(struct aes_sync_descriptor *const descr, const enum aes_action enc, const uint8_t *iv,
                           uint32_t iv_len, const uint8_t *aad, uint32_t aad_len)
{
	halAssert(descr && iv && iv_len, HAL_AES);
	halAssert((aad && aad_len) || !aad_len, HAL_AES);
	return _aes_sync_gcm_start(&descr->dev, enc, iv, iv_len, aad, aad_len);
}

/**
 * \brief              AES-GCM block update
 */
int32_t aes_sync_gcm_update(struct aes_sync_descriptor *const descr, const uint8_t *input, uint8_t *output,
                            uint32_t length)
{
	halAssert((uint8_t)(descr && 0xff), HAL_AES);
	halAssert((input && output && length) || (!length), HAL_AES);
	return _aes_sync_gcm_update(&descr->dev, input, output, length);
}

/**
 * \brief              AES-GCM block finish
 */
int32_t aes_sync_gcm_finish(struct aes_sync_descriptor *const descr, uint8_t *tag, uint32_t tag_len)
{
	halAssert(descr && ((tag && tag_len && (tag_len <= 16)) || !tag_len), HAL_AES);
	return _aes_sync_gcm_finish(&descr->dev, tag, tag_len);
}

/**
 * \brief              AES-CCM block encryption/decryption
 */
int32_t aes_sync_ccm_crypt_and_tag(struct aes_sync_descriptor *const descr, const enum aes_action enc,
                                   const uint8_t *input, uint8_t *output, uint32_t length, const uint8_t *iv,
                                   uint32_t iv_len, const uint8_t *aad, uint32_t aad_len, uint8_t *tag,
                                   uint32_t tag_len)
{
	halAssert(descr && input && output && length && iv, HAL_AES);
	halAssert((aad && aad_len) || !aad_len, HAL_AES);
	/* Length requirements follow the SP800-38C A.1 */
	halAssert(tag && (tag_len >= 4) && (tag_len <= 16) && !(tag_len % 2), HAL_AES);
	halAssert((iv_len >= 7) && (iv_len <= 13), HAL_AES);

	return _aes_sync_ccm_crypt_and_tag(&descr->dev, enc, input, output, length, iv, iv_len, aad, aad_len, tag, tag_len);
}

/**
 * \brief              AES-CCM block authenticated decryption
 */
int32_t aes_sync_ccm_auth_decrypt(struct aes_sync_descriptor *const descr, const uint8_t *input, uint8_t *output,
                                  uint32_t length, const uint8_t *iv, uint32_t iv_len, const uint8_t *aad,
                                  uint32_t aad_len, const uint8_t *tag, uint32_t tag_len)
{
	uint8_t  vtag[16];
	uint32_t index;
	int32_t  result;

	halAssert(descr && input && output && length && iv, HAL_AES);
	halAssert((aad && aad_len) || !aad_len, HAL_AES);
	/* Length requirements follow the SP800-38C A.1 */
	halAssert(tag && (tag_len >= 4) && (tag_len <= 16) && !(tag_len % 2), HAL_AES);
	halAssert((iv_len >= 7) && (iv_len <= 13), HAL_AES);

	result = _aes_sync_ccm_crypt_and_tag(
	    &descr->dev, AES_DECRYPT, input, output, length, iv, iv_len, aad, aad_len, vtag, tag_len);
	if (result != ERR_NONE) {
		return result;
	}
	for (index = 0; index < tag_len; index++) {
		if (vtag[index] != tag[index]) {
			return ERR_INVALID_DATA;
		}
	}
	return ERR_NONE;
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t aes_sync_get_version(void)
{
	return DRIVER_VERSION;
}


/*Bitcloud Wrappers Implementation*/

#include <sysQueue.h>
#include <halTaskManager.h>
#include <sysAssert.h>

struct aes_sync_descriptor CRYPTOGRAPHY_0;


static DECLARE_QUEUE(halAesRequestQueue);


HAL_AES_EncryptReq_t req;

void HAL_AES_EncryptReq(HAL_AES_EncryptReq_t *reqParams_s)
{
  memcpy(&req,reqParams_s , sizeof(HAL_AES_EncryptReq_t));

  halPostTask(HAL_SM_REQ);
}	

void HAL_SmReset(void)
{
  resetQueue(&halAesRequestQueue);
  halClearTask(HAL_SM_REQ);
}

void halAesInit(void)
{
  aes_sync_init(&CRYPTOGRAPHY_0, AES_REGS); // To be called in sys init
}

void halSmRequestHandler (void)
{
  HAL_AES_EncryptReq_t *reqParams = NULL;

  reqParams = &req; //getQueueElem(&halAesRequestQueue);
   if (NULL != reqParams)
   {
	switch (reqParams->aesCmd)
	{
		case   SM_SET_KEY_COMMAND :
			aes_sync_set_encrypt_key(&CRYPTOGRAPHY_0, reqParams->text, AES_KEY_128);
		break;
		case   SM_ECB_ENCRYPT_COMMAND :
			aes_sync_ecb_crypt(&CRYPTOGRAPHY_0, AES_ENCRYPT, reqParams->text,reqParams->text);
			
		break;
		case   SM_ECB_DECRYPT_COMMAND :
			aes_sync_ecb_crypt(&CRYPTOGRAPHY_0, AES_DECRYPT, reqParams->text,reqParams->text);
		break;
		default:
			SYS_E_ASSERT_FATAL(false, HAL_AES);
			break;
	}
	
	SYS_E_ASSERT_FATAL(reqParams->encryptConf, HAL_AES);
	reqParams->encryptConf();
   }
  
}




