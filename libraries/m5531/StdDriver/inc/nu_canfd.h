/****************************************************************************
 * @file     nu_canfd.h
 * @version  V1.00
 * @brief    M5531 series CAN FD driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __NU_CANFD_H__
#define __NU_CANFD_H__

#if defined ( __CC_ARM   )
    #pragma anon_unions
#endif

#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CANFD_Driver CAN_FD Driver
  @{
*/

/** @addtogroup CANFD_EXPORTED_CONSTANTS CAN_FD Exported Constants
  @{
*/

#define CANFD_OP_CAN_MODE     0
#define CANFD_OP_CAN_FD_MODE  1

/* Reserved number of elements in Message RAM - used for calculation of start addresses within RAM Configuration
   some element_numbers set to less than max, to stay altogether below 1536 words of MessageRAM requirement*/
#define CANFD_MAX_11_BIT_FTR_ELEMS    128ul  /*!<  maximum is 128 11-bit Filter */
#define CANFD_MAX_29_BIT_FTR_ELEMS    64ul   /*!<  maximum is  64 29-bit Filter */
#define CANFD_MAX_RX_FIFO0_ELEMS      64ul   /*!<  maximum is  64 Rx FIFO 0 elements */
#define CANFD_MAX_RX_FIFO1_ELEMS      64ul   /*!<  maximum is  64 Rx FIFO 1 elements */
#define CANFD_MAX_RX_BUF_ELEMS        64ul   /*!<  maximum is  64 Rx Buffers */
#define CANFD_MAX_TX_BUF_ELEMS        32ul   /*!<  maximum is  32 Tx Buffers */
#define CANFD_MAX_TX_EVENT_FIFO_ELEMS  32ul   /*!<  maximum is  32 Tx Event FIFO elements */

/* CAN FD sram size  */
#define CANFD_SRAM_SIZE          (1536 * sizeof(uint32_t))
#define CANFD_SRAM_OFFSET        0x200ul

/* CAN FD sram address  */
#define CANFD_SRAM_BASE_ADDR(psCanfd)  ((uint32_t)psCanfd + CANFD_SRAM_OFFSET)

/* CAN FD  Mask all interrupt */
#define CANFD_INT_ALL_SIGNALS         0x3FFFFFFFul

/* Maximum size of a CAN FD frame. Must be a valid CAN FD value */
#define CANFD_MAX_MESSAGE_BYTES     64

/* Maximum size of a CAN FD frame. Must be a valid CAN FD value */
#define CANFD_MAX_MESSAGE_WORDS     (CANFD_MAX_MESSAGE_BYTES / 4)

/* Receive message buffer helper macro */
#define CANFD_RX_BUFFER_STD(id, mbIdx)               ((7UL << 27) | ((id & 0x7FF) << 16) | (mbIdx & 0x3F))

/* Receive message buffer extended helper macro - low */
#define CANFD_RX_BUFFER_EXT_LOW(id, mbIdx)           ((7UL << 29) | (id & 0x1FFFFFFFUL))

/*  Receive message buffer extended helper macro - high */
#define CANFD_RX_BUFFER_EXT_HIGH(id, mbIdx)          (mbIdx & 0x3FUL)

/*  CAN FD Rx FIFO 0 Mask helper macro. */
#define CANFD_RX_FIFO0_STD_MASK(match, mask)         ((2UL << 30) | (1UL << 27) | ((match & 0x7FF) << 16) | (mask & 0x7FF))

/* CAN FD Rx FIFO 0 extended Mask helper macro - low. */
#define CANFD_RX_FIFO0_EXT_MASK_LOW(match)           ((1UL << 29) | (match & 0x1FFFFFFF))

/* CAN FD Rx FIFO 0 extended Mask helper macro - high. */
#define CANFD_RX_FIFO0_EXT_MASK_HIGH(mask)           ((2UL << 30) | (mask & 0x1FFFFFFF))

/* CAN FD Rx FIFO 1 Mask helper macro. */
#define CANFD_RX_FIFO1_STD_MASK(match, mask)         ((2UL << 30) | (2UL << 27) | ((match & 0x7FF) << 16) | (mask & 0x7FF))

/* CANFD Rx FIFO 1 extended Mask helper macro - low. */
#define CANFD_RX_FIFO1_EXT_MASK_LOW(match)           ((2UL << 29) | (match & 0x1FFFFFFF))

/* CANFD Rx FIFO 1 extended Mask helper macro - high. */
#define CANFD_RX_FIFO1_EXT_MASK_HIGH(mask)           ((2UL << 30) | (mask & 0x1FFFFFFF))

/* CANFD SID Filter Element Address. */
#define CANFD_GET_SID_ADDR(psCanfd, Idx)  ((CANFD_SID_FILTER_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->SIDFC & CANFD_SIDFC_FLSSA_Msk) + (Idx * sizeof(CANFD_SID_FILTER_T))))

/* CANFD XID Filter Element Address. */
#define CANFD_GET_XID_ADDR(psCanfd, Idx)  ((CANFD_XID_FILTER_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->XIDFC & CANFD_XIDFC_FLESA_Msk) + (Idx * sizeof(CANFD_XID_FILTER_T))))

/* CANFD TX BUFFER Element Address. */
#define CANFD_GET_TXBUF_ADDR(psCanfd, Idx)  ((CANFD_TX_BUF_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->TXBC & CANFD_TXBC_TBSA_Msk) + (Idx * sizeof(CANFD_TX_BUF_T))))

/* CANFD RX BUFFER Element Address. */
#define CANFD_GET_RXBUF_ADDR(psCanfd, Idx)  ((CANFD_RX_BUF_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->RXBC & CANFD_RXBC_RBSA_Msk) + (Idx * sizeof(CANFD_RX_BUF_T))))

/* CANFD RX FIFO0 Element Address. */
#define CANFD_GET_RF0BUF_ADDR(psCanfd, Idx)  ((CANFD_RX_BUF_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->RXF0C & CANFD_RXF0C_F0SA_Msk) + (Idx * sizeof(CANFD_RX_BUF_T))))

/* CANFD RX FIFO1 Element Address. */
#define CANFD_GET_RF1BUF_ADDR(psCanfd, Idx)  ((CANFD_RX_BUF_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->RXF1C & CANFD_RXF1C_F1SA_Msk) + (Idx * sizeof(CANFD_RX_BUF_T))))

/* CANFD TX EVENT FIFO Element Address. */
#define CANFD_GET_TXEVENT_ADDR(psCanfd, Idx)  ((CANFD_TX_EVENT_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (psCanfd->TXEFC & CANFD_TXEFC_EFSA_Msk) + (Idx * sizeof(CANFD_TX_EVENT_T))))

/* CANFD Set Standard ID to Register */
#define CANFD_SET_SID(id)     ((id & 0x7FF) << (29 - 11))

/* CANFD Get Standard ID from Register */
#define CANFD_GET_SID(id)     (id >> (29 - 11))

/**
 *    @brief        Get Monitors the Module CAN Communication State Flag
 *
 *    @param[in]    canfd    The pointer of the specified CANFD module
 *
 *    @retval       0 Synchronizing - node is synchronizing on CANFD communication.
 *    @retval       1 Idle - node is neither receiver nor transmitter.
 *    @retval       2 Receiver - node is operating as receiver.
 *    @retval       3 Transmitter - node is operating as transmitter.
 *
 *    @details      This macro get the module CANFD communication state.
 *    \hideinitializer
 */
#define CANFD_GET_COMMUNICATION_STATE(canfd)    (((canfd)->PSR  & CANFD_PSR_ACT_Msk) >> CANFD_PSR_ACT_Pos)


/* CAN FD frame data field size. */
typedef enum
{
    eCANFD_BYTE8  = 0, /*!< 8 byte data field. */
    eCANFD_BYTE12 = 1, /*!< 12 byte data field. */
    eCANFD_BYTE16 = 2, /*!< 16 byte data field. */
    eCANFD_BYTE20 = 3, /*!< 20 byte data field. */
    eCANFD_BYTE24 = 4, /*!< 24 byte data field. */
    eCANFD_BYTE32 = 5, /*!< 32 byte data field. */
    eCANFD_BYTE48 = 6, /*!< 48 byte data field. */
    eCANFD_BYTE64 = 7  /*!< 64 byte data field. */
} E_CANFD_DATA_FIELD_SIZE;

/* CAN FD Tx FIFO/Queue Mode. */
typedef enum
{
    eCANFD_QUEUE_MODE = 0, /*!< Tx FIFO operation. */
    eCANFD_FIFO_MODE = 1   /*!< Tx Queue operation. */
} E_CANFD_MODE;

/* CAN FD Test & Bus monitor Mode. */
typedef enum
{
    eCANFD_NORMAL = 0,              /*!< None, Normal mode. */

    /*
    Support:
    (1) to receive data frames
    (2) to receive remote frames
    (3) to give acknowledge to valid frames
    Not support:
    (1) data frames sending
    (2) remote frames sending
    (3) active error frames or overload frames sending
    */
    eCANFD_RESTRICTED_OPERATION,    /*!< Receive external RX frame and always keep recessive state or send dominate bit on ACK bit on TX pin. */

    /*
    Support:
    (1) to receive valid data frames
    (2) to receive valid remote frames
    Not support:
    (1) transmission start
    (2) acknowledge to valid frames
    */
    eCANFD_BUS_MONITOR,             /*!< Receive external RX frame and always keep recessive state on TX pin. */

    /*
    Support:
    (1) Loopback
    (2) Also send out frames
    Not support:
    (1) to receive external frame
    */
    eCANFD_LOOPBACK_EXTERNAL,       /*!< Won't receive external RX frame. */
    /*
    Support:
    (1) Loopback
    Not support:
    (1) to receive external frame
    (2) transmission start
    */
    eCANFD_LOOPBACK_INTERNAL        /*!< Won't receive external RX frame and always keep recessive state on TX pin */
} E_CANFD_TEST_MODE;

/* TX Buffer Configuration Parameters  */
typedef struct
{
    E_CANFD_DATA_FIELD_SIZE eDataFieldSize;     /*!< TX Buffer Data Field Size (8byte .. 64byte) */
    E_CANFD_MODE            eModeSel;           /*!< select: CANFD_QUEUE_MODE/CANFD_FIFO_MODE */
    uint32_t                u32ElemCnt;         /*!< Elements in FIFO/Queue */
    uint32_t                u32DBufNumber;      /*!< Number of dedicated TX buffers */
} CANFD_TX_BUF_CONFIG_T;

/* Nominal Bit Timing Parameters */
typedef struct
{
    uint32_t u32BitRate;          /*!< Transceiver baud rate in bps */
    uint16_t u16TDCOffset;        /*!< Transceiver Delay Compensation Offset */
    uint16_t u16TDCFltrWin;       /*!< Transceiver Delay Compensation Filter Window Length */
    uint8_t  u8TDC;               /*!< Transceiver Delay Compensation (1:Yes, 0:No) */
} CANFD_NBT_CONFIG_T;


/* Data Bit Timing Parameters */
typedef struct
{
    uint32_t u32BitRate;          /*!< Transceiver baud rate in bps */
    uint16_t u16TDCOffset;        /*!< Transceiver Delay Compensation Offset */
    uint16_t u16TDCFltrWin;       /*!< Transceiver Delay Compensation Filter Window Length */
    uint8_t  u8TDC;               /*!< Transceiver Delay Compensation (1:Yes, 0:No) */
} CANFD_DBT_CONFIG_T;

/*! CAN FD protocol timing characteristic configuration structure. */
typedef struct
{
    uint8_t  u8PreDivider;        /*!< Global Clock Division Factor. */
    uint16_t u16NominalPrescaler; /*!< Nominal clock prescaler. */
    uint8_t  u8NominalRJumpwidth; /*!< Nominal Re-sync Jump Width. */
    uint8_t  u8NominalPhaseSeg1;  /*!< Nominal Phase Segment 1. */
    uint8_t  u8NominalPhaseSeg2;  /*!< Nominal Phase Segment 2. */
    uint8_t  u8NominalPropSeg;    /*!< Nominal Propagation Segment. */
    uint8_t  u8DataPrescaler;     /*!< Data clock prescaler. */
    uint8_t  u8DataRJumpwidth;    /*!< Data Re-sync Jump Width. */
    uint8_t  u8DataPhaseSeg1;     /*!< Data Phase Segment 1. */
    uint8_t  u8DataPhaseSeg2;     /*!< Data Phase Segment 2. */
    uint8_t  u8DataPropSeg;       /*!< Data Propagation Segment. */

} CANFD_TIMEING_CONFIG_T;

/* CAN FD module configuration structure. */
typedef struct
{
    CANFD_NBT_CONFIG_T sNormBitRate;       /*!< Normal bit rate. */
    CANFD_DBT_CONFIG_T sDataBitRate;       /*!< Data bit rate.   */
    CANFD_TIMEING_CONFIG_T sConfigBitTing; /*!< Bit timing config*/
    uint8_t bFDEn;                         /*!< 1 == FD Operation enabled. */
    uint8_t bBitRateSwitch;                /*!< 1 == Bit Rate Switch enabled (only evaluated in HW, if FD operation enabled). */
    E_CANFD_TEST_MODE evTestMode;          /*!< See E_CANFD_TEST_MODE declaration. */
} CANFD_FD_BT_CONFIG_T;

/* CAN FD Message RAM Partitioning - i.e. Start Addresses (BYTE) */
typedef struct
{
    uint32_t u32SIDFC_FLSSA; /*!<Standard ID Filter Configuration */
    uint32_t u32XIDFC_FLESA; /*!<Extended ID Filter Configuration */
    uint32_t u32RXF0C_F0SA;  /*!< RX FIFO 0 Start Address */
    uint32_t u32RXF1C_F1SA;  /*!< RX FIFO 1 Start Address */
    uint32_t u32RXBC_RBSA;   /*!< Rx Buffer Configuration */
    uint32_t u32TXEFC_EFSA;  /*!< Tx Event FIFO Configuration */
    uint32_t u32TXBC_TBSA;   /*!< Tx Buffer Configuration */
} CANFD_RAM_PART_T;

/*CAN FD element size structure */
typedef struct
{
    uint32_t  u32SIDFC;          /*!< Standard Message ID Filter element size in words  */
    uint32_t  u32XIDFC;          /*!< Extended Message ID Filter element size in words  */
    uint32_t  u32RxFifo0;        /*!< Rx FIFO0 element size in words  */
    uint32_t  u32RxFifo1;        /*!< Rx FIFO1 element size in words  */
    uint32_t  u32RxBuf;          /*!< Rx Buffer element size in words */
    uint32_t  u32TxBuf;          /*!< Tx Buffer element size in words */
    uint32_t  u32TxFifoQueue;    /*!< Tx FIFO/Queue element size in words(Be aware that the sum of Tx buff and Tx FIFO/Queue may be not greater than 32) */
    uint32_t  u32TxEventFifo;    /*!< Tx Event FIFO element size in words(Be aware that the sum of Tx buff and Tx FIFO/Queue may be not greater than 32)*/
    uint32_t  u32UserDef;        /*!< Element for user define */
} CANFD_ELEM_SIZE_T;

/* CAN FD Message frame structure */
typedef struct
{
    CANFD_FD_BT_CONFIG_T    sBtConfig;        /*!< Bit Timing Configuration */
    CANFD_RAM_PART_T        sMRamStartAddr;   /*!< Absolute Byte Start Addresses for Element Types in Message RAM */
    CANFD_ELEM_SIZE_T       sElemSize;        /*!< Size of Elements in Message RAM (RX Elem. in FIFO0, in FIFO1, TX Buffer) given in words */
    CANFD_TX_BUF_CONFIG_T   sTxConfig;        /*!< TX Buffer Configuration  */
    uint32_t                u32MRamSize;      /*!< Size of the Message RAM: number of bytes */
} CANFD_FD_T;

/* RX Buffer and FIFO element definition */
typedef struct
{
    /* R0 in 32-bit defintion. */
    /* SID is placed high first 11-bit. */
    uint32_t  ID : 29;

    /* Remote Transmission Request */
    /* 0 = Received frame is a data frame */
    /* 1 = Received frame is a remote frame */
    uint32_t  RTR : 1;

    /* Extended ID */
    /* 0: 11-bit standard ID */
    /* 1: 29-bit extended ID */
    uint32_t  XTD : 1;

    /* Error State Indication */
    /* 0: Transmitting node is error active. */
    /* 1: Transmitting node is error passive. */
    uint32_t  ESI : 1;

    /* R1 in 32-bit defintion. */
    /* Rx Timestamp */
    /* Timestamp Counter value captured on start of frame reception. */
    /* Resolution depending on configuration of the Timestamp Counter Prescaler TCP (CANFD_TSCC[19:16]). */
    uint32_t  RXTS: 16;

    /* Data Length Code */
    /* 0-8 = CAN + CAN FD: received frame has 0-8 data bytes */
    /* 9-15 = CAN: received frame has 8 data bytes */
    /* 9-15 = CAN FD: received frame has 12/16/20/24/32/48/64 data bytes */
    uint32_t  DLC  : 4;

    /* Bit Rate Switch */
    /* 0 = Frame received without bit rate switching */
    /* 1 = Frame received with bit rate switching */
    uint32_t  BRS : 1;

    /* FD Format */
    /* 0 = Classical CAN frame format */
    /* 1 = CAN FD frame format (new DLC-coding and CRC) */
    uint32_t  FDF : 1;

    /* Reserved */
    uint32_t  : 2;

    /* Filter Index */
    /* 0-127=Index of matching Rx acceptance ?lter element (invalid if ANMF = '1'). */
    uint32_t  FIDX: 7;

    /* Accepted Non-matching Frame */
    /* Acceptance of non-matching frames may be enabled via ANFS (CANFD_GFC[5:4]) and ANFE (CANFD_GFC[3:2]). */
    /* 0 = Received frame matching filter index FIDX */
    /* 1 = Received frame did not match any Rx filter element */
    uint32_t  ANMF: 1;

    /* R2~Rn */
    /* 64-Byte Data Array  */
    uint8_t  DB[64];

} CANFD_RX_BUF_T;


/* TX Buffer and FIFO element definition */
typedef struct
{
    /* T0 in 32-bit defintion. */
    /* Identifier, SID is placed high first 11-bit. */
    uint32_t  ID : 29;

    /* Remote Transmission Request */
    /* 0 = Transmit data frame */
    /* 1 = Transmit remote frame */
    uint32_t  RTR : 1;

    /* Extended Identi?er */
    /* 0 = 11-bit standard identifier */
    /* 1 = 29-bit extended identifier */
    uint32_t  XTD : 1;

    /* Error State Indicator */
    /* 0 = ESI bit in CAN FD format depends only on error passive flag */
    /* 1 = ESI bit in CAN FD format transmitted recessive */
    uint32_t  ESI : 1;

    /* T1 in 32-bit defintion. */
    uint32_t  : 8;

    /* Message Marker[15:8], High byte */
    uint32_t  MM_H: 8;

    /* DLC: Data Length Code */
    /* 0-8 = CAN + CAN FD: received frame has 0-8 data bytes */
    /* 9-15 = CAN: received frame has 8 data bytes */
    /* 9-15 = CAN FD: received frame has 12/16/20/24/32/48/64 data bytes */
    uint32_t  DLC  : 4;

    /* BRS: Bit Rate Switch */
    /* 0 = Frame received without bit rate switching */
    /* 1 = Frame received with bit rate switching */
    uint32_t  BRS : 1;

    /* FDF: FD Format */
    /* 0 = Classical CAN frame format */
    /* 1 = CAN FD frame format (new DLC-coding and CRC) */
    uint32_t  FDF : 1;

    /* Reserved */
    uint32_t  : 1;

    /* EFC: Event FIFO Control */
    /* 0 = Don't store Tx events */
    /* 1 = Store Tx events */
    uint32_t  EFC : 1;

    /* Message Marker[7:0], Low byte */
    uint32_t  MM_L : 8;

    /* DB: T2~Tn Data bytes ( 64 Bytes) */
    /* 64-Byte Data Array  */
    uint8_t  DB[64];

} CANFD_TX_BUF_T;

/* TX Event FIFO element definition */
typedef struct
{
    /* E0 in 32-bit defintion. */
    /* Identifier, SID is placed high first 11-bit. */
    uint32_t  ID : 29;

    /* Remote Transmission Request */
    /* 0 = Transmit data frame */
    /* 1 = Transmit remote frame */
    uint32_t  RTR : 1;

    /* Extended Identifier */
    /* 0 = 11-bit standard Identifier */
    /* 1 = 29-bit extended Identifier */
    uint32_t  XTD : 1;

    /* Error State Indicator */
    /* 0 = ESI bit in CAN FD format depends only on error passive flag */
    /* 1 = ESI bit in CAN FD format transmitted recessive */
    uint32_t  ESI : 1;

    /* E1 in 32-bit defintion. */
    /* TXTS: Tx Timestamp */
    /* Timestamp Counter value captured on start of frame transmission. */
    /* Resolution depending on configuration of the Timestamp Counter Prescaler TCP (CANFD_TSCC[19:16]). */
    uint32_t  TXTS  : 16;

    /* DLC: Data Length Code */
    /* 0-8 = CAN + CAN FD: received frame has 0-8 data bytes */
    /* 9-15 = CAN: received frame has 8 data bytes */
    /* 9-15 = CAN FD: received frame has 12/16/20/24/32/48/64 data bytes */
    uint32_t  DLC  : 4;

    /* BRS: Bit Rate Switch */
    /* 0 = Frame received without bit rate switching */
    /* 1 = Frame received with bit rate switching */
    uint32_t  BRS : 1;

    /* FDF: FD Format */
    /* 0 = Classical CAN frame format */
    /* 1 = CAN FD frame format (new DLC-coding and CRC) */
    uint32_t  FDF : 1;

    /* ET: Event Type */
    /* 00 = Reserved */
    /* 01 = Tx event */
    /* 10 = Transmission in spite of cancellation (always set for transmissions in DAR mode) */
    /* 11 = Reserved */
    uint32_t  ET: 2;

    /* MM: Message Marker */
    uint32_t  MM : 8;

} CANFD_TX_EVENT_T;

/* Standard Message ID Filter Element */
typedef struct
{
    /* SFID2: Standard Filter ID 2 */
    uint32_t  SFID2 : 11;

    uint32_t  : 5;

    /* SFID1: Standard Filter ID 1 */
    uint32_t  SFID1 : 11;

    /* SFEC: Standard Filter Element Configuration */
    /* 000b = Disable filter element */
    /* 001b = Store in Rx FIFO 0 if filter matches */
    /* 010b = Store in Rx FIFO 1 if filter matches */
    /* 011b = Reject ID if filter matches, not intended to be used with Sync messages */
    /* 100b = Set priority if filter matches, not intended to be used with Sync messages, no storage */
    /* 101b = Set priority and store in FIFO 0 if filter matches */
    /* 110b = Set priority and store in FIFO 1 if filter matches */
    /* 111b = Store into Rx Buffer or as debug message, configuration of SFT[1:0] ignored */
    uint32_t  SFEC : 3;

    /* SFT: Standard Filter Type */
    /* 00 = Range filter from SFID1 to SFID2 (SFID2 >= SFID1) */
    /* 01 = Dual ID filter for SFID1 or SFID2 */
    /* 10 = Classic filter: SFID1 = filter, SFID2 = mask */
    /* 11 = Filter element disabled */
    uint32_t  SFT : 2;

} CANFD_SID_FILTER_T;

/* Extended Message ID Filter Element */
typedef struct
{
    /* EFID1: Extended Filter ID 1 */
    uint32_t  EFID1 : 29;

    /* EFEC: Extended Filter Element Configuration */
    /* 000b = Disable filter element */
    /* 001b = Store in Rx FIFO 0 if filter matches */
    /* 010b = Store in Rx FIFO 1 if filter matches */
    /* 011b = Reject ID if filter matches */
    /* 100b = Set priority if filter matches */
    /* 101b = Set priority and store in FIFO 0 if filter matches */
    /* 110b = Set priority and store in FIFO 1 if filter matches */
    /* 111b = Store into Rx Buffer, configuration of EFT[1:0] ignored */
    uint32_t  EFEC : 3;

    /* EFID2: Extended Filter ID 2 */
    uint32_t  EFID2 : 29;

    /* Reserved */
    uint32_t  : 1;

    /* EFT: Extended Filter Type */
    /* 00b = Range filter from EFID1 to EFID2 (EFID2 >- EFID1) */
    /* 01b = Dual ID filter for EFID1 or EFID2 */
    /* 10b = Classic filter: EFID1 = filter, EFID2 = mask */
    /* 11b = Range filter from EFID1 to EFID2 (EFID2 >= EFID1), CANFD_XIDAM mask not applied. */
    uint32_t  EFT : 2;

} CANFD_XID_FILTER_T ;

/* Accept Non-matching Frames (GFC Register) */
typedef enum
{
    eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0 = 0x0,  /*!< Accept Non-Masking Frames in Rx FIFO 0. */
    eCANFD_ACC_NON_MATCH_FRM_RX_FIFO1 = 0x1,  /*!< Accept Non-Masking Frames in Rx FIFO 1. */
    eCANFD_REJ_NON_MATCH_FRM   = 0x3          /*!< Reject Non-Matching Frames. */
} E_CANFD_ACC_NON_MATCH_FRM;


/* Standard ID Filter Element Type */
typedef enum
{
    eCANFD_SID_FLTR_TYPE_RANGE     = 0x0, /*!< Range filter from SFID1 to SFID2. */
    eCANFD_SID_FLTR_TYPE_DUAL      = 0x1, /*!< Dual ID filter for SFID1 or SFID2. */
    eCANFD_SID_FLTR_TYPE_CLASSIC   = 0x2, /*!< Classic filter: SFID1 = filter, SFID2 = mask. */
    eCANFD_SID_FLTR_TYPE_DIS       = 0x3  /*!< Filter element disabled */
} E_CANFD_SID_FLTR_ELEM_TYPE;

/* Extended ID Filter Element Type */
typedef enum
{
    eCANFD_XID_FLTR_TYPE_RANGE      = 0x0,  /*!< Range filter from EFID1 to EFID2. */
    eCANFD_XID_FLTR_TYPE_DUAL       = 0x1,  /*!< Dual ID filter for EFID1 or EFID2. */
    eCANFD_XID_FLTR_TYPE_CLASSIC    = 0x2,  /*!< Classic filter: EFID1=filter, EFID2=mask */
    eCANFD_XID_FLTR_TYPE_RANGE_XIDAM_NOT_APP     = 0x3   /*!< XID range filter from EFID1 to EFID2(EFID2 > EFID1), XIDAM not applied */
}  E_CANFD_XID_FLTR_ELEM_TYPE;

/* Filter Element Configuration - Can be used for SFEC(Standard Id filter configuration) and EFEC(Extended Id filter configuration) */
typedef enum
{
    eCANFD_FLTR_ELEM_DIS         = 0x0,            /*!< Filter Element Disable  */
    eCANFD_FLTR_ELEM_STO_FIFO0   = 0x1,            /*!< Filter Element Store In Fifo0  */
    eCANFD_FLTR_ELEM_STO_FIFO1   = 0x2,            /*!< Filter Element Store In Fifo1  */
    eCANFD_FLTR_ELEM_REJ_ID      = 0x3,            /*!< Filter Element RejectId  */
    eCANFD_FLTR_ELEM_SET_PRI     = 0x4,            /*!< Filter Element Set Priority */
    eCANFD_FLTR_ELEM_SET_PRI_STO_FIFO0 = 0x5,      /*!< Filter Element Set Priority And Store In Fifo0  */
    eCANFD_FLTR_ELEM_SET_PRI_STO_FIFO1 = 0x6,      /*!< Filter Element Set Priority And Store In Fifo1  */
    eCANFD_FLTR_ELEM_STO_RX_BUF_OR_DBG_MSG = 0x7   /*!< Filter Element Store In Rx Buf Or Debug Msg  */
} E_CANFD_FLTR_CONFIG;

/* Message Type in Message RAM. */
typedef enum
{
    eCANFD_MSG_RF0          = 0x0,              /*!< RX FIFO0  */
    eCANFD_MSG_RF1          = 0x1,              /*!< RX FIFO1  */
    eCANFD_MSG_DRB          = 0x2,              /*!< Dedicated RX Buffe  */
    eCANFD_MSG_DTB          = 0x3,              /*!< Dedicated TX Buffe  */
    eCANFD_MSG_TEF          = 0x4,              /*!< TX EVENT FIFO */
    eCANFD_MSG_SID          = 0x5,              /*!< Standard ID Filter */
    eCANFD_MSG_XID          = 0x6,              /*!< Extended ID Filter */
} E_CANFD_MSG;

#define CANFD_TIMEOUT            SystemCoreClock    /*!< CANFD time-out counter (1 second time-out) */
#define CANFD_OK                 ( 0L)              /*!< CANFD operation OK */
#define CANFD_ERR_FAIL           (-1L)              /*!< CANFD operation failed */
#define CANFD_ERR_TIMEOUT        (-2L)              /*!< CANFD operation abort due to timeout error */

uint32_t CANFD_GetDefaultConfig(CANFD_FD_T *psConfig, uint8_t u8OpMode);
void CANFD_Open(CANFD_T *psCanfd, CANFD_FD_T *psCanfdStr);
void *CANFD_Transfer(CANFD_T *psCanfd, E_CANFD_MSG eCanfdMsg, void *psUserData, uint32_t u32Idx);
void CANFD_Close(CANFD_T *psCanfd);
void CANFD_EnableInt(CANFD_T *psCanfd, uint32_t u32IntLine0, uint32_t u32IntLine1, uint32_t u32TXBTIE, uint32_t u32TXBCIE);
void CANFD_DisableInt(CANFD_T *psCanfd, uint32_t u32IntLine0, uint32_t u32IntLine1, uint32_t u32TXBTIE, uint32_t u32TXBCIE);
void CANFD_SetGFC(CANFD_T *psCanfd, E_CANFD_ACC_NON_MATCH_FRM eNMStdFrm, E_CANFD_ACC_NON_MATCH_FRM eEMExtFrm, uint32_t u32RejRmtStdFrm, uint32_t u32RejRmtExtFrm);
uint32_t CANFD_GetRxFifoWaterLvl(CANFD_T *psCanfd, uint32_t u32RxFifoNum);
void CANFD_TxBufCancelReq(CANFD_T *psCanfd, uint32_t u32TxBufIdx);
uint32_t CANFD_IsTxBufCancelFin(CANFD_T *psCanfd, uint32_t u32TxBufIdx);
uint32_t CANFD_IsTxBufTransmitOccur(CANFD_T *psCanfd, uint32_t u32TxBufIdx);
uint32_t CANFD_GetTxEvntFifoWaterLvl(CANFD_T *psCanfd);
void CANFD_GetBusErrCount(CANFD_T *psCanfd, uint8_t *pu8TxErrBuf, uint8_t *pu8RxErrBuf);
int32_t CANFD_RunToNormal(CANFD_T *psCanfd, uint8_t u8Enable);
void CANFD_ClearStatusFlag(CANFD_T *psCanfd, uint32_t u32InterruptFlag);
uint32_t CANFD_GetStatusFlag(CANFD_T *psCanfd, uint32_t u32IntTypeFlag);
uint32_t CANFD_DecodeDLC(uint32_t u32Dlc);
uint32_t CANFD_EncodeDLC(uint32_t u32NumOfBytes);

/** @} end of group CANFD_EXPORTED_FUNCTIONS */

/** @} end of group CANFD_Driver */

/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#if defined ( __CC_ARM   )
    #pragma no_anon_unions
#endif

#endif /* __NU_CANFD_H__ */
