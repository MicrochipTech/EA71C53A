/*******************************************************************************
  NWK Leave Header File

  Company:
    Microchip Technology Inc.

  File Name:
    nwkLeave.h

  Summary:
    This file contains the constants declaration and defines for NWk Leave Mechanism.

  Description:
    This file contains the constants declaration and defines for NWk Leave Mechanism.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#if !defined _NWK_LEAVE_H
#define _NWK_LEAVE_H

/******************************************************************************
                                Includes section
 ******************************************************************************/
#include <private/nwkConfig.h>
#include <sysQueue.h>
#include <appFramework.h>
#include <mac.h>
#include <nwkNeighbor.h>
#include <private/nwkFrame.h>
#include <nldeData.h>
#include <nlmeLeave.h>

/******************************************************************************
                               Definitions section
 ******************************************************************************/
/** Size of NWK leave command payload. */
#define NWK_LEAVE_CMD_PAYLOAD_SIZE 2U
/** Total size of self leave command - NWK header + payload */
#define NWK_LEAVE_ITSELF_CMD_SIZE 18U
/** Total size of unknown child leave command - NWK header + payload */
#define NWK_LEAVE_UNKNOWN_CHILD_CMD_SIZE 18U
/** Total size of child leave command - NWK header + payload */
#define NWK_LEAVE_CHILD_CMD_SIZE 26U
/** Parameters of leave command transmission. */
#define NWK_LEAVE_ITSELF_TX_PARAMETERS \
  {NWK_TX_DELAY_BROADCAST, nwkPrepareLeaveItselfTx,\
   nwkConfirmLeaveItselfTx, true}
#define NWK_LEAVE_CHILD_TX_PARAMETERS \
  {NWK_TX_DELAY_UNICAST_COMMAND, nwkPrepareLeaveChildTx,\
   nwkConfirmLeaveChildTx, true}
#define NWK_LEAVE_UNKNOWN_CHILD_PARAMETERS \
   {NWK_TX_DELAY_UNICAST_COMMAND, nwkPrepareLeaveUnknownChildTx,\
    nwkConfirmLeaveUnknownChildTx, true}

/******************************************************************************
                                  Types section
 ******************************************************************************/
/** Internal states of NLME-LEAVE component */
typedef enum _NwkLeaveState_t
{
  NWK_LEAVE_IDLE_STATE = 0xE6,
  NWK_LEAVE_FIRST_STATE = 0x11,
  NWK_LEAVE_BEGIN_STATE = NWK_LEAVE_FIRST_STATE,
  NWK_LEAVE_SELF_SEND_CMD_STATE = 0x12,
  NWK_LEAVE_SELF_WAIT_CONFIRM_STATE = 0x13,
  NWK_LEAVE_SELF_RESET_MAC_STATE = 0x14,
  NWK_LEAVE_CHILD_SEND_CMD_STATE = 0x15,
  NWK_LEAVE_CHILD_WAIT_CONFIRM_STATE = 0x16,
  NWK_LEAVE_UNKNOWN_CHILD_STATE = 0x17,
  NWK_LEAVE_UNKNOWN_CHILD_WAIT_CONFIRM_STATE = 0x18,
  NWK_LEAVE_CONFIRM_STATE = 0x19,
  NWK_LEAVE_LAST_STATE
} NwkLeaveState_t;

/** Leave Command Options Field */
typedef enum _NwkLeaveCommandOptionsField_t
{
  /** The Rejoin sub-field is a single-bit field. If the value of this
   * sub-field is 1, the device that is leaving from its current parent
   * will rejoin the network. If the value of this sub-field is 0, the device
   * will not rejoin the network. ZigBee spec r17, 3.4.4.3.1.1, page 325. */
  NWK_LEAVE_REJOIN          = 0x20,
  /** The request sub-field is a single-bit field. If the value of this
   * sub-field is 1, then the leave command frame is a request for another
   * device to leave the network. If the value of this sub-field is 0,
   * then the leave command frame is an indication that the sending device
   * plans to leave the network. ZigBee spec r17, 3.4.4.3.1.2, page 326. */
  NWK_LEAVE_REQUEST         = 0x40,
  /** The remove children sub-field is a single-bit field. If this sub-field
   * has a value of 1, then the children of the device that is leaving
   * the network will also be removed. If this sub-field has a value of 0,
   * then the children of the device leaving the network will not be removed.
   * ZigBee spec r17, 3.4.4.3.1.3, page 326. */
  NWK_LEAVE_REMOVE_CHILDREN = 0x80
} NwkLeaveCommandOptionsField_t;

/** Internal variables of NLME-LEAVE component. */
typedef struct _NwkLeave_t
{
  /** Queue of requests from upper layer */
  QueueDescriptor_t queue;
  union
  {
    /** MLME-RESET request primitive's parameters. */
    MAC_ResetReq_t macReset;
    struct
    {
      /** The extended address of the leaved child. */
      ExtAddr_t extAddr;
      /** The short address of the leaved child. */
      ShortAddr_t shortAddr;
      /** The bits indicate the transmission options for the MAC frame. */
      MAC_TxOptions_t txOptions;
    } child;
  } param;
#if (defined NWK_TAKES_CARE_OF_CHILD_ROUTER) || (defined _ENDDEVICE_ && defined NWK_ENDDEVICE_SEND_LEAVE_IND_ON_LEAVE_REQ) || (defined _CHILD_MANAGEMENT_)
  /** Leave request which is used by the leave indication component. */
  NWK_LeaveReq_t leaveReq;
  /** The leave indication component is busy or not. */
  bool leaveIndBusy;
#endif /* _ROUTER_ and NWK_TAKES_CARE_OF_CHILD_ROUTER */
  /** Finite-state machine */
  NwkLeaveState_t state;
  /** Number of children which has been excluded from a network. */
  NwkSizeOfNeighborTable_t leaveChildCount;
} NwkLeave_t;

/******************************************************************************
                              Prototypes section
 ******************************************************************************/
/**************************************************************************//**
  \brief Main task handler of NLME-LEAVE component
 ******************************************************************************/
NWK_PRIVATE void nwkLeaveTaskHandler(void);

/**************************************************************************//**
  \brief Reset NLME-LEAVE component
 ******************************************************************************/
NWK_PRIVATE void nwkResetLeave(void);

/**************************************************************************//**
  \brief Action when a leave command is received.

  \param[in] payload - pointer to NPDU of NWK leave command.
  \param[in] header  - NWK header.
  \param[in] parse   - parsed header fields.
  \return 'true' if continue processing of command packet otherwise 'false'.
 ******************************************************************************/
NWK_PRIVATE bool nwkLeaveFrameInd(const uint8_t *const payload,
  const NwkFrameHeader_t *const header, const NwkParseHeader_t *const parse);

/**************************************************************************//**
  \brief Prepare header and payload of the leave itself command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareLeaveItselfTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of the leave itself command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of the leave command transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmLeaveItselfTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

/******************************************************************************
  \brief nwkLeaveReq idle checking.

  \return true, if nwkLeaveReq performs no activity, false - otherwise.
 ******************************************************************************/
NWK_PRIVATE bool nwkLeaveReqIsIdle(void);

#ifdef _CHILD_MANAGEMENT_
void NWK_SendLeaveReq(MAC_Addr_t addr, MAC_AddrMode_t mode);
#endif

#if defined _ROUTER_ || defined _COORDINATOR_
/**************************************************************************//**
  \brief Prepare header and payload of the leave child command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareLeaveChildTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of the leave child command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of the leave command transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmLeaveChildTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);

#ifdef _CHILD_MANAGEMENT_
/**************************************************************************//**
  \brief Prepare header and payload of the leave unknown child command.

  \param[in] outPkt - pointer to output packet.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkPrepareLeaveUnknownChildTx(NwkOutputPacket_t *const outPkt);

/**************************************************************************//**
  \brief Confirmation of the leave unknown  child command transmission.

  \param[in] outPkt - pointer to output packet.
  \param[in] status - network status of the leave command transmission.
  \return None.
 ******************************************************************************/
NWK_PRIVATE void nwkConfirmLeaveUnknownChildTx(NwkOutputPacket_t *const outPkt,
  const NWK_Status_t status);
#else
  #define nwkPrepareLeaveUnknownChildTx NULL
  #define nwkConfirmLeaveUnknownChildTx NULL
#endif /* _CHILD_MANAGEMENT_ */


#else /* _ROUTER_ or _COORDINATOR_ */

#define nwkPrepareLeaveChildTx NULL
#define nwkConfirmLeaveChildTx NULL

#define nwkPrepareLeaveUnknownChildTx NULL
#define nwkConfirmLeaveUnknownChildTx NULL

#endif /* _ROUTER_ or _COORDINATOR_ */
#endif /* _NWK_LEAVE_H */
/** eof nwkLeave.h */

