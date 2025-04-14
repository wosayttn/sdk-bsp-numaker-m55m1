/****************************************************************************
 * @file     canfd.c
 * @version  V1.00
 * @brief    M5531 series CAN FD driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"
#include "string.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Minimum number of time quanta in a bit. */
#define MIN_TIME_QUANTA    8ul
/* Maximum number of time quanta in a bit. */
#define MAX_TIME_QUANTA    25ul
/* Number of receive FIFOs (1 - 2) */
#define CANFD_NUM_RX_FIFOS  2ul

/*CANFD max nominal bit rate*/
#define MAX_NOMINAL_BAUDRATE (1000000UL)

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CANFD_Driver CAN_FD Driver
  @{
*/

/** @addtogroup CANFD_EXPORTED_FUNCTIONS CAN_FD Exported Functions
  @{
*/

/**
 * @brief       Rx FIFO Configuration for RX_FIFO_0 and RX_FIFO_1.
 *
 * @param[in]   psCanfd          The pointer to CAN FD module base address.
 * @param[in]   u32RxFifoNum     0: RX FIFO_0, 1: RX_FIFO_1.
 * @param[in]   psRamConfig      Rx FIFO Size in number of configuration ram address.
 * @param[in]   psElemSize       Rx FIFO Size in number of Rx FIFO elements (element number (max. = 64)).
 * @param[in]   u32FifoWM        Watermark in number of Rx FIFO elements
 * @param[in]   eFifoSize        Maximum data field size that should be stored in this Rx FIFO
 *                               (configure BYTE64 if you are unsure, as this is the largest data field allowed in CAN FD)
 *
 * @return      None.
 *
 * @details     Rx FIFO Configuration for RX_FIFO_0 and RX_FIFO_1.
 */
static void CANFD_InitRxFifo(CANFD_T *psCanfd, uint32_t u32RxFifoNum, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize, uint32_t u32FifoWM, E_CANFD_DATA_FIELD_SIZE eFifoSize)
{
    CANFD_RX_BUF_T *psHeadAddr;
    uint32_t u32Size;

    /* ignore if index is too high */
    if (u32RxFifoNum > CANFD_NUM_RX_FIFOS) return;

    /* ignore if index is too high */
    if (psElemSize-> u32RxFifo0 > CANFD_MAX_RX_FIFO0_ELEMS) return;

    /* ignore if index is too high */
    if (psElemSize-> u32RxFifo1 > CANFD_MAX_RX_FIFO1_ELEMS) return;

    switch (u32RxFifoNum)
    {
    case 0:
        if (psElemSize-> u32RxFifo0)
        {
            /* set size of Rx FIFO 0, set offset, blocking mode */
            psCanfd->RXF0C = (psRamConfig->u32RXF0C_F0SA) |
                             (psElemSize->u32RxFifo0 << CANFD_RXF0C_F0S_Pos) |
                             (u32FifoWM << CANFD_RXF0C_F0WM_Pos);

            psCanfd->RXESC = (psCanfd->RXESC & (~CANFD_RXESC_F0DS_Msk)) |
                             (eFifoSize << CANFD_RXESC_F0DS_Pos);

            /*Get the RX FIFO 0 Start Address in the RAM*/
            psHeadAddr = CANFD_GET_RF0BUF_ADDR(psCanfd, 0);
            u32Size = eFifoSize;

            if (u32Size < 5U)
            {
                u32Size += 4U;
            }
            else
            {
                u32Size = u32Size * 4U - 10U;
            }

            /*Clear the RX FIFO 0 Memory*/
            memset((void *)psHeadAddr, 0x00, (u32Size * 4 * psElemSize->u32RxFifo0));
        }
        else
        {
            psCanfd->RXF0C = 0;
        }

        break;

    case 1:
        if (psElemSize-> u32RxFifo1)
        {
            /* set size of Rx FIFO 1, set offset, blocking mode */
            psCanfd->RXF1C = (psRamConfig->u32RXF1C_F1SA) |
                             (psElemSize->u32RxFifo1 << CANFD_RXF1C_F1S_Pos) |
                             (u32FifoWM << CANFD_RXF1C_F1WM_Pos);
            psCanfd->RXESC = (psCanfd->RXESC & (~CANFD_RXESC_F1DS_Msk)) |
                             (eFifoSize << CANFD_RXESC_F1DS_Pos);

            /*Get the RX FIFO 1 Start Address in the RAM*/
            psHeadAddr = CANFD_GET_RF1BUF_ADDR(psCanfd, 0);

            u32Size = eFifoSize;

            if (u32Size < 5U)
            {
                u32Size += 4U;
            }
            else
            {
                u32Size = u32Size * 4U - 10U;
            }

            /*Clear the RX FIFO 0 Memory*/
            memset((uint32_t *)psHeadAddr, 0x00, (u32Size * 4 * psElemSize->u32RxFifo1));
        }
        else
        {
            psCanfd->RXF1C = 0;
        }

        break;
    }
}


/**
 * @brief       Function configures the data structures used by a dedicated Rx Buffer.
 *
 * @param[in]   psCanfd          The pointer to CAN FD module base address.
 * @param[in]   psRamConfig      Tx buffer configuration ram address.
 * @param[in]   psElemSize       Tx buffer configuration element size.
 * @param[in]   eTxBufSize       Maximum data field size that should be stored in a dedicated Tx Buffer
 *                              (configure BYTE64 if you are unsure, as this is the largest data field allowed in CAN FD)largest data field allowed in CAN FD)
 *
 * @return      None.
 *
 * @details     Function configures the data structures used by a dedicated Rx Buffer.
 */
static void CANFD_InitTxDBuf(CANFD_T *psCanfd, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize, E_CANFD_DATA_FIELD_SIZE eTxBufSize)
{
    CANFD_TX_BUF_T *psHeadAddr;
    uint32_t u32Size;

    /*Note: Be aware that the sum of TFQS and NDTB may be not greater than 32. There is no check
            for erroneous configurations. The Tx Buffers section in the Message RAM starts with the
            dedicated Tx Buffers.*/
    if ((psElemSize->u32TxBuf + psElemSize->u32TxFifoQueue) > 32)
    {
        psElemSize->u32TxBuf = 16;
        psElemSize->u32TxFifoQueue = 16;
    }

    /*Setting the Tx Buffer Start Address*/
    psCanfd->TXBC = (((psElemSize->u32TxFifoQueue & 0x3F) << CANFD_TXBC_TFQS_Pos) |
                     ((psElemSize->u32TxBuf & 0x3F) << CANFD_TXBC_NDTB_Pos) |
                     (psRamConfig->u32TXBC_TBSA & CANFD_TXBC_TBSA_Msk));

    /*Setting the Tx Buffer Data Field Size*/
    psCanfd->TXESC = (psCanfd->TXESC & (~CANFD_TXESC_TBDS_Msk)) | (eTxBufSize <<  CANFD_TXESC_TBDS_Pos);

    /*Get the TX Buffer Start Address in the RAM*/
    psHeadAddr = CANFD_GET_TXBUF_ADDR(psCanfd, 0);

    /*Get the Buffer Data Field Size*/
    u32Size = eTxBufSize;

    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }

    /*Clear the TX Buffer Memory*/
    memset((void *)psHeadAddr, 0x00, (u32Size * 4 * psElemSize->u32TxBuf));
}


/**
 * @brief       Function configures the data structures used by a dedicated Rx Buffer.
 *
 * @param[in]   psCanfd          The pointer to CAN FD module base address.
 * @param[in]   psRamConfig      Rx buffer configuration ram address.
 * @param[in]   psElemSize       Rx buffer configuration element size.
 * @param[in]   eRxBufSize       Maximum data field size that should be stored in a dedicated Rx Buffer
 *                              (configure BYTE64 if you are unsure, as this is the largest data field allowed in CAN FD)largest data field allowed in CAN FD)
 *
 * @return      None.
 *
 * @details     Function configures the data structures used by a dedicated Rx Buffer.
 */
static void CANFD_InitRxDBuf(CANFD_T *psCanfd, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize, E_CANFD_DATA_FIELD_SIZE eRxBufSize)
{
    /*Get the Buffer Data Field Size*/
    uint32_t u32Size = eRxBufSize;

    /*Get the RX Buffer Start Address in the RAM*/
    CANFD_RX_BUF_T *psHeadAddr = CANFD_GET_RXBUF_ADDR(psCanfd, 0);

    /*Setting the Rx Buffer Start Address*/
    psCanfd->RXBC = (psRamConfig->u32RXBC_RBSA & CANFD_RXBC_RBSA_Msk);

    /*Setting the Rx Buffer Data Field Size*/
    psCanfd->RXESC = (psCanfd->RXESC & (~CANFD_RXESC_RBDS_Msk)) | (eRxBufSize <<  CANFD_RXESC_RBDS_Pos);

    if (u32Size < 5U)
    {
        u32Size += 4U;
    }
    else
    {
        u32Size = u32Size * 4U - 10U;
    }

    /*Clear the RX Buffer Memory*/
    memset((void *)psHeadAddr, 0x00, (u32Size * 4 * psElemSize->u32RxBuf));
}

/**
 * @brief       Init Tx event fifo
 *
 * @param[in]   psCanfd          The pointer to CAN FD module base address.
 * @param[in]   psRamConfig      Tx Event Fifo configuration ram address.
 * @param[in]   psElemSize       Tx Event Fifo configuration element size
 * @param[in]   u32FifoWaterLvl  FIFO water level
 *
 * @return      None.
 *
 * @details     Init Tx event fifo.
 */
static void CANFD_InitTxEvntFifo(CANFD_T *psCanfd, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize, uint32_t u32FifoWaterLvl)
{
    /* Set TX Event FIFO element size,watermark,start address. */
    psCanfd->TXEFC = (u32FifoWaterLvl << CANFD_TXEFC_EFWM_Pos) |
                     (psElemSize->u32TxEventFifo << CANFD_TXEFC_EFS_Pos) |
                     (psRamConfig->u32TXEFC_EFSA & CANFD_TXEFC_EFSA_Msk);
}


/**
 * @brief       Configures the register SIDFC for the 11-bit Standard Message ID Filter elements.
 *
 * @param[in]   psCanfd           The pointer to CAN FD module base address.
 * @param[in]   psRamConfig       Standard ID filter configuration ram address
 * @param[in]   psElemSize        Standard ID filter configuration element size
 *
 * @return      None.
 *
 * @details     Function configures the data structures used by a dedicated Rx Buffer.
 */
static void CANFD_ConfigSIDFC(CANFD_T *psCanfd, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize)
{
    /*Get the Filter List Standard Start Address in the RAM*/
    CANFD_SID_FILTER_T *psHeadAddr = CANFD_GET_SID_ADDR(psCanfd, 0);

    /*Setting the Filter List Standard Start Address and List Size  */
    psCanfd->SIDFC = ((psElemSize->u32SIDFC & 0xFF) << CANFD_SIDFC_LSS_Pos) | (psRamConfig->u32SIDFC_FLSSA & CANFD_SIDFC_FLSSA_Msk);

    /*Clear the Filter List Memory*/
    memset((void *)psHeadAddr, 0x00, (psElemSize->u32SIDFC * sizeof(CANFD_SID_FILTER_T)));
}


/**
 * @brief       Configures the register XIDFC for the 29-bit Extended Message ID Filter elements.
 *
 * @param[in]   psCanfd           The pointer to CAN FD module base address.
 * @param[in]   psRamConfig       Extended ID filter configuration ram address
 * @param[in]   psElemSize        Extended ID filter configuration element size
 *
 * @return      None.
 *
 * @details     Configures the register XIDFC for the 29-bit Extended Message ID Filter elements.
 */
static void CANFD_ConfigXIDFC(CANFD_T *psCanfd, CANFD_RAM_PART_T *psRamConfig, CANFD_ELEM_SIZE_T *psElemSize)
{
    /*Get the Filter List Standard Start Address in the RAM*/
    CANFD_XID_FILTER_T *psHeadAddr = CANFD_GET_XID_ADDR(psCanfd, 0);

    /*Setting the Filter List Extended Start Address and List Size  */
    psCanfd->XIDFC = ((psElemSize->u32XIDFC & 0xFF) << CANFD_XIDFC_LSE_Pos) | (psRamConfig->u32XIDFC_FLESA & CANFD_XIDFC_FLESA_Msk);

    /*Clear the Filter List Memory*/
    memset((void *)psHeadAddr, 0x00, (psElemSize->u32XIDFC * sizeof(CANFD_XID_FILTER_T)));
}

/**
 * @brief       Calculates the CAN FD RAM buffer address.
 *
 * @param[in]   psConfigAddr  CAN FD element star address structure.
 * @param[in]   psConfigSize  CAN FD element size structure.
 *
 * @return      None.
 *
 * @details     Calculates the CAN FD RAM buffer address.
 */
static uint32_t CANFD_CalculateRamAddress(CANFD_RAM_PART_T *psConfigAddr, CANFD_ELEM_SIZE_T *psConfigSize)
{
    uint32_t u32RamAddrOffset = 0;

    /* Get the Standard Message ID Filter element address */
    if (psConfigSize->u32SIDFC > 0)
    {
        psConfigAddr->u32SIDFC_FLSSA = 0;
        u32RamAddrOffset += psConfigSize->u32SIDFC * sizeof(CANFD_SID_FILTER_T);
    }

    /* Get the Extended Message ID Filter element address */
    if (psConfigSize->u32XIDFC > 0)
    {
        psConfigAddr->u32XIDFC_FLESA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32XIDFC * sizeof(CANFD_XID_FILTER_T);
    }

    /* Get the Rx FIFO0 element address */
    if (psConfigSize->u32RxFifo0 > 0)
    {
        psConfigAddr->u32RXF0C_F0SA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxFifo0 * sizeof(CANFD_RX_BUF_T);
    }

    /* Get the Rx FIFO1 element address */
    if (psConfigSize->u32RxFifo1 > 0)
    {
        psConfigAddr->u32RXF1C_F1SA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxFifo1 * sizeof(CANFD_RX_BUF_T);
    }

    /* Get the Rx Buffer element address */
    if (psConfigSize->u32RxBuf > 0)
    {
        psConfigAddr->u32RXBC_RBSA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32RxBuf * sizeof(CANFD_RX_BUF_T);
    }

    /* Get the TX Event FIFO element address */
    if (psConfigSize->u32TxEventFifo > 0)
    {
        psConfigAddr->u32TXEFC_EFSA = u32RamAddrOffset;
        u32RamAddrOffset += psConfigSize->u32TxEventFifo * sizeof(CANFD_TX_EVENT_T);
    }

    /* Get the Tx Buffer and Tx FIFO/Queue element address */
    if ((psConfigSize->u32TxBuf + psConfigSize->u32TxFifoQueue) > 0)
    {
        if ((psConfigSize->u32TxBuf + psConfigSize->u32TxFifoQueue) > 32)
        {
            psConfigSize->u32TxBuf = 16;
            psConfigSize->u32TxFifoQueue = 16;
        }

        psConfigAddr->u32TXBC_TBSA = u32RamAddrOffset;
        u32RamAddrOffset += (psConfigSize->u32TxBuf + psConfigSize->u32TxFifoQueue) * sizeof(CANFD_TX_BUF_T);
    }

    return u32RamAddrOffset;
}

/**
 * @brief       Sets the CAN FD protocol timing characteristic.
 *
 * @param[in]   psCanfd     The pointer of the specified CANFD module.
 * @param[in]   psConfig    Pointer to the timing configuration structure.
 *
 * @return      None.
 *
 * @details     This function gives user settings to CAN bus timing characteristic.
 *              The function is for an experienced user. For less experienced users, call
 *              the CANFD_Open() and fill the baud rate field with a desired value.
 *              This provides the default timing characteristics to the module.
 */
static void CANFD_SetTimingConfig(CANFD_T *psCanfd, const CANFD_TIMEING_CONFIG_T *psConfig)
{
    if (psCanfd == (CANFD_T *)CANFD0)
    {
        /* Get CANF D0 clock divider number */
        CLK->CANFDDIV = (CLK->CANFDDIV & ~CLK_CANFDDIV_CANFD0DIV_Msk) | CLK_CANFDDIV_CANFD0DIV(psConfig->u8PreDivider);
    }
    else if (psCanfd == (CANFD_T *)CANFD1)
    {
        /* Set CANFD1 clock divider number */
        CLK->CANFDDIV = (CLK->CANFDDIV & ~CLK_CANFDDIV_CANFD1DIV_Msk) | CLK_CANFDDIV_CANFD1DIV(psConfig->u8PreDivider);
    }
    else
    {
        return;
    }

    /* configuration change enable */
    psCanfd->CCCR |= CANFD_CCCR_CCE_Msk;

    /* nominal bit rate */
    psCanfd->NBTP = (((psConfig->u8NominalRJumpwidth & 0x7F) - 1) << 25) +
                    (((psConfig->u16NominalPrescaler & 0x1FF) - 1) << 16) +
                    ((((psConfig->u8NominalPhaseSeg1 + psConfig->u8NominalPropSeg) & 0xFF) - 1) << 8) +
                    (((psConfig->u8NominalPhaseSeg2 & 0x7F) - 1) << 0);


    /* canfd->DBTP */
    if (psCanfd->CCCR & CANFD_CCCR_FDOE_Msk)
    {

        psCanfd->DBTP = (((psConfig->u8DataPrescaler & 0x1F) - 1) << 16) +
                        ((((psConfig->u8DataPhaseSeg1 + psConfig->u8DataPropSeg) & 0x1F) - 1) << 8) +
                        (((psConfig->u8DataPhaseSeg2 & 0xF) - 1) << 4) +
                        (((psConfig->u8DataRJumpwidth & 0xF) - 1) << 0);

    }
}

/**
 * @brief       Get the segment values.
 *
 * @param[in]   u32NominalBaudRate  The nominal speed in bps.
 * @param[in]   u32DataBaudRate     The data speed in bps.
 * @param[in]   u32Ntq              Number of nominal time quanta per bit.
 * @param[in]   u32Dtq              Number of data time quanta per bit.
 * @param[in]   psConfig            Passed is a configuration structure, on return the configuration is stored in the structure
 *
 * @return      None.
 *
 * @details     Calculates the segment values for a single bit time for nominal and data baudrates.
 */
static void CANFD_GetSegments(uint32_t u32NominalBaudRate, uint32_t u32DataBaudRate, uint32_t u32Ntq, uint32_t u32Dtq, CANFD_TIMEING_CONFIG_T *psConfig)
{
    float ideal_sp;
    int int32P1;

    /* get ideal sample point */
    if (u32NominalBaudRate >= 1000000)     ideal_sp = 0.750;
    else if (u32NominalBaudRate >= 800000) ideal_sp = 0.800;
    else                                   ideal_sp = 0.875;

    /* distribute time quanta */
    int32P1 = (int)(u32Ntq * ideal_sp);

    /* can controller doesn't separate prop seg and phase seg 1 */
    psConfig->u8NominalPropSeg = 0;

    /* subtract one TQ for sync seg */
    psConfig->u8NominalPhaseSeg1 = int32P1 - 1;
    psConfig->u8NominalPhaseSeg2 = u32Ntq - int32P1;

    /* sjw is 20% of total TQ, rounded to nearest int */
    psConfig->u8NominalRJumpwidth = (u32Ntq + (5 - 1)) / 5;

    /* if using baud rate switching then distribute time quanta for data rate */
    if (u32Dtq > 0)
    {
        /* get ideal sample point */
        if (u32DataBaudRate >= 1000000)     ideal_sp = 0.750;
        else if (u32DataBaudRate >= 800000) ideal_sp = 0.800;
        else                                ideal_sp = 0.875;

        /* distribute time quanta */
        int32P1 = (int)(u32Dtq * ideal_sp);

        /* can controller doesn't separate prop seg and phase seg 1 */
        psConfig->u8DataPropSeg = 0;

        /* subtract one TQ for sync seg */
        psConfig->u8DataPhaseSeg1 = int32P1 - 1;
        psConfig->u8DataPhaseSeg2 = u32Dtq - int32P1;

        /* sjw is 20% of total TQ, rounded to nearest int */
        psConfig->u8DataRJumpwidth = (u32Dtq + (5 - 1)) / 5;
    }
    else
    {
        psConfig->u8DataPropSeg = 0;
        psConfig->u8DataPhaseSeg1 = 0;
        psConfig->u8DataPhaseSeg2 = 0;
        psConfig->u8DataRJumpwidth = 0;
    }
}

/**
 * @brief       Calculates the CAN controller timing values for specific baudrates.
 *
 * @param[in]   psCanfd             Pointer to CAN FD configuration structure.
 * @param[in]   u32NominalBaudRate  The nominal speed in bps.
 * @param[in]   u32DataBaudRate     The data speed in bps. Zero to disable baudrate switching.
 * @param[in]   u32SourceClock_Hz   CAN FD Protocol Engine clock source frequency in Hz.
 * @param[in]   psConfig            Passed is a configuration structure, on return the configuration is stored in the structure
 *
 * @return      true if timing configuration found, false if failed to find configuration.
 *
 * @details     Calculates the CAN controller timing values for specific baudrates.
 */
static uint32_t CANFD_CalculateTimingValues(CANFD_T *psCanfd, uint32_t u32NominalBaudRate, uint32_t u32DataBaudRate, uint32_t u32SourceClock_Hz, CANFD_TIMEING_CONFIG_T *psConfig)
{
    int i32Nclk2;
    int i32Ntq;
    int i32Dclk;
    int i32Dclk2;
    int i32Dtq;

    /* observe baud rate maximums */
    if (u32NominalBaudRate > MAX_NOMINAL_BAUDRATE) u32NominalBaudRate = MAX_NOMINAL_BAUDRATE;

    for (i32Ntq = MAX_TIME_QUANTA; i32Ntq >= MIN_TIME_QUANTA; i32Ntq--)
    {
        int i32Nclk = u32NominalBaudRate * i32Ntq;

        for (psConfig->u16NominalPrescaler = 0x001; psConfig->u16NominalPrescaler <= 0x400; (psConfig->u16NominalPrescaler)++)
        {
            i32Nclk2 = i32Nclk * psConfig->u16NominalPrescaler;

            if (((u32SourceClock_Hz / i32Nclk2) <= 256) && ((u32SourceClock_Hz % i32Nclk2) == 0))
            {

                psConfig->u8PreDivider = u32SourceClock_Hz / i32Nclk2;

                /* FD Enabled */
                if (psCanfd->CCCR & CANFD_CCCR_FDOE_Msk)
                {
                    /* Exception case: Let u32DataBaudRate is same with u32NominalBaudRate. */
                    if (u32DataBaudRate == 0)
                        u32DataBaudRate = u32NominalBaudRate;

                    /* if baudrates are the same and the solution for nominal will work for
                       data, then use the nominal settings for both */
                    if ((u32DataBaudRate == u32NominalBaudRate) && (psConfig->u16NominalPrescaler <= 0x20))
                    {
                        i32Dtq = i32Ntq;
                        psConfig->u8DataPrescaler = (uint8_t)psConfig->u16NominalPrescaler;
                        CANFD_GetSegments(u32NominalBaudRate, u32DataBaudRate, i32Ntq, i32Dtq, psConfig);
                        return TRUE;
                    }

                    /* calculate data settings */
                    for (i32Dtq = MAX_TIME_QUANTA; i32Dtq >= MIN_TIME_QUANTA; i32Dtq--)
                    {
                        i32Dclk = u32DataBaudRate * i32Dtq;

                        for (psConfig->u8DataPrescaler = 0x01; psConfig->u8DataPrescaler <= 0x20; (psConfig->u8DataPrescaler)++)
                        {
                            i32Dclk2 = i32Dclk * psConfig->u8DataPrescaler;

                            if (u32SourceClock_Hz == ((uint32_t)i32Dclk2 * psConfig->u8PreDivider))
                            {
                                CANFD_GetSegments(u32NominalBaudRate, u32DataBaudRate, i32Ntq, i32Dtq, psConfig);
                                return TRUE;
                            }
                        }
                    }
                } // if (psCanfd->CCCR & CANFD_CCCR_FDOE_Msk)
                else
                {
                    psConfig->u8DataPrescaler = 0;
                    CANFD_GetSegments(u32NominalBaudRate, 0, i32Ntq, 0, psConfig);
                    return TRUE;
                }
            } // if (((u32SourceClock_Hz / i32Nclk2) <= 5) && ((u32SourceClock_Hz % i32Nclk2) == 0))
        }
    }

    /* failed to find solution */
    return FALSE;
}

/**
 * @brief       Get the default configuration structure.
 *
 * @param[in]   psConfig       Pointer to CAN FD configuration structure.
 * @param[in]   u8OpMode       Setting the CAN FD Operating mode.
 *
 * @return      None.
 *
 * @details     This function initializes the CAN FD configure structure to default value.
 *              The default value are:
 *              sNormBitRate.u32BitRate   = 500000bps;
 *              u32DataBaudRate     = 0(CAN mode) or 1000000(CAN FD mode) ;
 *              u32MRamSize         = 6144 bytes (1536 words);
 *              bEnableLoopBack     = FALSE;
 *              bBitRateSwitch      = FALSE(CAN Mode) or TRUE(CAN FD Mode);
 *              bFDEn               = FALSE(CAN Mode) or TRUE(CAN FD Mode);
*/
uint32_t CANFD_GetDefaultConfig(CANFD_FD_T *psConfig, uint8_t u8OpMode)
{
    if (psConfig->sElemSize.u32UserDef == 0)
        memset(psConfig, 0, sizeof(CANFD_FD_T));

    psConfig->sBtConfig.sNormBitRate.u32BitRate = 500000;

    if (u8OpMode == CANFD_OP_CAN_MODE)
    {
        psConfig->sBtConfig.sDataBitRate.u32BitRate = 0;
        psConfig->sBtConfig.bFDEn = FALSE;
        psConfig->sBtConfig.bBitRateSwitch = FALSE;
    }
    else
    {
        psConfig->sBtConfig.sDataBitRate.u32BitRate = 1000000;
        psConfig->sBtConfig.bFDEn = TRUE;
        psConfig->sBtConfig.bBitRateSwitch = TRUE;
    }

    /* Set normal mode by default */
    psConfig->sBtConfig.evTestMode = eCANFD_NORMAL;

    /*Get the CAN FD memory size(number of byte)*/
    psConfig->u32MRamSize  = CANFD_SRAM_SIZE;

    if (psConfig->sElemSize.u32UserDef == 0)
    {
        /* CAN FD Standard message ID elements as 16 elements    */
        psConfig->sElemSize.u32SIDFC = 16;
        /* CAN FD Extended message ID elements as 16 elements    */
        psConfig->sElemSize.u32XIDFC = 16;
        /* CAN FD TX Buffer elements as 16 elements    */
        psConfig->sElemSize.u32TxBuf = 16;
        /* CAN FD Tx FIFO/Queue elements as 16 elements    */
        psConfig->sElemSize.u32TxFifoQueue = 16;
        /* CAN FD RX Buffer elements as 16 elements    */
        psConfig->sElemSize.u32RxBuf = 16;
        /* CAN FD RX FIFO0 elements as 16 elements    */
        psConfig->sElemSize.u32RxFifo0 = 16;
        /* CAN FD RX FIFO1 elements as 16 elements    */
        psConfig->sElemSize.u32RxFifo1 = 16;
        /* CAN FD TX Event FIFO elements as 16 elements    */
        psConfig->sElemSize.u32TxEventFifo = 16;
    }

    /*Calculates the CAN FD RAM buffer address*/
    return CANFD_CalculateRamAddress(&psConfig->sMRamStartAddr, &psConfig->sElemSize);
}

/**
 * @brief       Encode the Data Length Code.
 *
 * @param[in]   u32NumOfBytes Number of bytes in a message.
 *
 * @return      Data Length Code.
 *
 * @details     Converts number of bytes in a message into a Data Length Code.
 */
uint32_t CANFD_EncodeDLC(uint32_t u32NumOfBytes)
{
    if (u32NumOfBytes <= 8) return u32NumOfBytes;
    else if (u32NumOfBytes <= 12) return 9;
    else if (u32NumOfBytes <= 16) return 10;
    else if (u32NumOfBytes <= 20) return 11;
    else if (u32NumOfBytes <= 24) return 12;
    else if (u32NumOfBytes <= 32) return 13;
    else if (u32NumOfBytes <= 48) return 14;
    else return 15;
}

/**
 * @brief       Decode the Data Length Code.
 *
 * @param[in]   u32Dlc   Data Length Code.
 *
 * @return      Number of bytes in a message.
 *
 * @details     Converts a Data Length Code into a number of message bytes.
 */
uint32_t CANFD_DecodeDLC(uint32_t u32Dlc)
{
    if (u32Dlc <= 8) return u32Dlc;
    else if (u32Dlc == 9) return 12;
    else if (u32Dlc == 10) return 16;
    else if (u32Dlc == 11) return 20;
    else if (u32Dlc == 12) return 24;
    else if (u32Dlc == 13) return 32;
    else if (u32Dlc == 14) return 48;
    else return 64;
}

/**
 * @brief       Config message ram and Set bit-time.
 *
 * @param[in]   psCanfd     The pointer to CAN FD module base address.
 * @param[in]   psCanfdStr  message ram setting and bit-time setting
 *
 * @return      None.
 *
 * @details     Converts a Data Length Code into a number of message bytes.
 */
void CANFD_Open(CANFD_T *psCanfd, CANFD_FD_T *psCanfdStr)
{
    uint32_t u32CanFdClock = 0;

    if (psCanfd == (CANFD_T *)CANFD0)
    {
        CLK_EnableModuleClock(CANFD0_MODULE);
        SYS_ResetModule(SYS_CANFD0RST);

        if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD0SEL_Msk) == CLK_CANFDSEL_CANFD0SEL_HXT)
        {
            u32CanFdClock = CLK_GetHXTFreq();
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD0SEL_Msk) == CLK_CANFDSEL_CANFD0SEL_APLL0_DIV2)
        {
            u32CanFdClock = CLK_GetAPLL0ClockFreq() / 2;
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD0SEL_Msk) == CLK_CANFDSEL_CANFD0SEL_HCLK0)
        {
            u32CanFdClock = CLK_GetHCLK0Freq();
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD0SEL_Msk) == CLK_CANFDSEL_CANFD0SEL_HIRC)
        {
            u32CanFdClock = __HIRC;
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD0SEL_Msk) == CLK_CANFDSEL_CANFD0SEL_HIRC48M_DIV4)
        {
            u32CanFdClock = __HIRC48M / 4;
        }

    }
    else if (psCanfd == (CANFD_T *)CANFD1)
    {
        CLK_EnableModuleClock(CANFD1_MODULE);
        SYS_ResetModule(SYS_CANFD1RST);

        if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD1SEL_Msk) == CLK_CANFDSEL_CANFD1SEL_HXT)
        {
            u32CanFdClock = __HXT;
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD1SEL_Msk) == CLK_CANFDSEL_CANFD1SEL_APLL0_DIV2)
        {
            u32CanFdClock = CLK_GetAPLL0ClockFreq() / 2;
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD1SEL_Msk) == CLK_CANFDSEL_CANFD1SEL_HCLK0)
        {
            u32CanFdClock = CLK_GetHCLK0Freq();
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD1SEL_Msk) == CLK_CANFDSEL_CANFD1SEL_HIRC)
        {
            u32CanFdClock = __HIRC;
        }
        else if ((CLK->CANFDSEL & CLK_CANFDSEL_CANFD1SEL_Msk) == CLK_CANFDSEL_CANFD1SEL_HIRC48M_DIV4)
        {
            u32CanFdClock = __HIRC48M / 4;
        }
    }
    else
    {
        return;
    }

    /* Initialization & un-lock */
    CANFD_RunToNormal(psCanfd, FALSE);

    if (psCanfdStr->sBtConfig.bBitRateSwitch)
    {
        /* enable FD and baud-rate switching */
        psCanfd->CCCR |= CANFD_CCCR_BRSE_Msk;
    }

    if (psCanfdStr->sBtConfig.bFDEn)
    {
        /*FD Operation enabled*/
        psCanfd->CCCR |= CANFD_CCCR_FDOE_Msk;
    }

    /*Clear the Rx Fifo0 element setting */
    psCanfd->RXF0C = 0;

    /*Clear the Rx Fifo1 element setting */
    psCanfd->RXF1C = 0;

    /* calculate and apply timing */
    if (CANFD_CalculateTimingValues(psCanfd, psCanfdStr->sBtConfig.sNormBitRate.u32BitRate, psCanfdStr->sBtConfig.sDataBitRate.u32BitRate,
                                    u32CanFdClock, &psCanfdStr->sBtConfig.sConfigBitTing))
    {
        CANFD_SetTimingConfig(psCanfd, &psCanfdStr->sBtConfig.sConfigBitTing);
    }

    /* Configures the Standard ID Filter element */
    if (psCanfdStr->sElemSize.u32SIDFC != 0)
        CANFD_ConfigSIDFC(psCanfd, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize);

    /*Configures the Extended ID Filter element */
    if (psCanfdStr->sElemSize.u32XIDFC != 0)
        CANFD_ConfigXIDFC(psCanfd, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize);

    /*Configures the Tx Buffer element */
    if (psCanfdStr->sElemSize.u32TxBuf != 0 || psCanfdStr->sElemSize.u32TxFifoQueue != 0)
        CANFD_InitTxDBuf(psCanfd, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize, eCANFD_BYTE64);

    /*Configures the Rx Buffer element */
    if (psCanfdStr->sElemSize.u32RxBuf != 0)
        CANFD_InitRxDBuf(psCanfd, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize, eCANFD_BYTE64);

    /*Configures the Rx Fifo0 element */
    if (psCanfdStr->sElemSize.u32RxFifo0 != 0)
        CANFD_InitRxFifo(psCanfd, 0, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize, 0, eCANFD_BYTE64);

    /*Configures the Rx Fifo1 element */
    if (psCanfdStr->sElemSize.u32RxFifo1 != 0)
        CANFD_InitRxFifo(psCanfd, 1, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize, 0, eCANFD_BYTE64);

    /*Configures the Tx Event FIFO element */
    if (psCanfdStr->sElemSize.u32TxEventFifo != 0)
        CANFD_InitTxEvntFifo(psCanfd, &psCanfdStr->sMRamStartAddr, &psCanfdStr->sElemSize, 0);

    /*Reject all Non-matching Frames Extended ID and Frames Standard ID,Reject all remote frames with 11-bit standard IDs and 29-bit extended IDs */
    CANFD_SetGFC(psCanfd, eCANFD_REJ_NON_MATCH_FRM, eCANFD_REJ_NON_MATCH_FRM, 1, 1);

    /* Test mode configuration */
    switch (psCanfdStr->sBtConfig.evTestMode)
    {
    case eCANFD_RESTRICTED_OPERATION:
        psCanfd->CCCR |= (CANFD_CCCR_TEST_Msk | CANFD_CCCR_ASM_Msk);
        break;

    case eCANFD_BUS_MONITOR:
        psCanfd->CCCR |= (CANFD_CCCR_TEST_Msk | CANFD_CCCR_MON_Msk);
        break;

    case eCANFD_LOOPBACK_EXTERNAL:
        psCanfd->CCCR |= CANFD_CCCR_TEST_Msk;
        psCanfd->TEST |= CANFD_TEST_LBCK_Msk;
        break;

    case eCANFD_LOOPBACK_INTERNAL:
        psCanfd->CCCR |= (CANFD_CCCR_TEST_Msk | CANFD_CCCR_MON_Msk);
        psCanfd->TEST |= CANFD_TEST_LBCK_Msk;
        break;

    case eCANFD_NORMAL:  /* Normal mode */
    default:
        psCanfd->CCCR &= ~(CANFD_CCCR_MON_Msk | CANFD_CCCR_TEST_Msk | CANFD_CCCR_ASM_Msk);
        psCanfd->TEST &= ~CANFD_TEST_LBCK_Msk;
        break;
    }

}

/**
 * @brief       Close the CAN FD Bus.
 *
 * @param[in]   psCanfd   The pointer to CANFD module base address.
 *
 * @return      None.
 *
 * @details     Disable the CAN FD clock and Interrupt.
 */
void CANFD_Close(CANFD_T *psCanfd)
{
    if (psCanfd == (CANFD_T *)CANFD0)
    {
        CLK_DisableModuleClock(CANFD0_MODULE);
    }
    else if (psCanfd == (CANFD_T *)CANFD1)
    {
        CLK_DisableModuleClock(CANFD1_MODULE);
    }
}

/**
 * @brief       Enables CAN FD interrupts according to provided mask .
 *
 * @param[in]   psCanfd          The pointer of the specified CAN FD module.
 * @param[in]   u32IntLine0      The Interrupt Line 0 type select.
 * @param[in]   u32IntLine1      The Interrupt Line 1 type select.
 *                              - \ref CANFD_IE_ARAE_Msk     : Access to Reserved Address Interrupt
 *                              - \ref CANFD_IE_PEDE_Msk     : Protocol Error in Data Phase Interrupt
 *                              - \ref CANFD_IE_PEAE_Msk     : Protocol Error in Arbitration Phase Interrupt
 *                              - \ref CANFD_IE_WDIE_Msk     : Watchdog Interrupt
 *                              - \ref CANFD_IE_BOE_Msk      : Bus_Off Status Interrupt
 *                              - \ref CANFD_IE_EWE_Msk      : Warning Status Interrupt
 *                              - \ref CANFD_IE_EPE_Msk      : Error Passive Interrupt
 *                              - \ref CANFD_IE_ELOE_Msk     : Error Logging Overflow Interrupt
 *                              - \ref CANFD_IE_BEUE_Msk     : Bit Error Uncorrected Interrupt
 *                              - \ref CANFD_IE_BECE_Msk     : Bit Error Corrected Interrupt
 *                              - \ref CANFD_IE_DRXE_Msk     : Message stored to Dedicated Rx Buffer Interrupt
 *                              - \ref CANFD_IE_TOOE_Msk     : Timeout Occurred Interrupt
 *                              - \ref CANFD_IE_MRAFE_Msk    : Message RAM Access Failure Interrupt
 *                              - \ref CANFD_IE_TSWE_Msk     : Timestamp Wraparound Interrupt
 *                              - \ref CANFD_IE_TEFLE_Msk    : Tx Event FIFO Event Lost Interrupt
 *                              - \ref CANFD_IE_TEFFE_Msk    : Tx Event FIFO Full Interrupt
 *                              - \ref CANFD_IE_TEFWE_Msk    : Tx Event FIFO Watermark Reached Interrupt
 *                              - \ref CANFD_IE_TEFNE_Msk    : Tx Event FIFO New Entry Interrupt
 *                              - \ref CANFD_IE_TFEE_Msk     : Tx FIFO Empty Interrupt
 *                              - \ref CANFD_IE_TCFE_Msk     : Transmission Cancellation Finished Interrupt
 *                              - \ref CANFD_IE_TCE_Msk      : Transmission Completed Interrupt
 *                              - \ref CANFD_IE_HPME_Msk     : High Priority Message Interrupt
 *                              - \ref CANFD_IE_RF1LE_Msk    : Rx FIFO 1 Message Lost Interrupt
 *                              - \ref CANFD_IE_RF1FE_Msk    : Rx FIFO 1 Full Interrupt
 *                              - \ref CANFD_IE_RF1WE_Msk    : Rx FIFO 1 Watermark Reached Interrupt
 *                              - \ref CANFD_IE_RF1NE_Msk    : Rx FIFO 1 New Message Interrupt
 *                              - \ref CANFD_IE_RF0LE_Msk    : Rx FIFO 0 Message Lost Interrupt
 *                              - \ref CANFD_IE_RF0FE_Msk    : Rx FIFO 0 Full Interrupt
 *                              - \ref CANFD_IE_RF0WE_Msk    : Rx FIFO 0 Watermark Reached Interrupt
 *                              - \ref CANFD_IE_RF0NE_Msk    : Rx FIFO 0 New Message Interrupt
 *
 * @param[in]   u32TXBTIE        Enable Tx Buffer Transmission 0-31 Interrupt.
 * @param[in]   u32TXBCIE        Enable Tx Buffer Cancellation Finished 0-31 Interrupt.
 * @return      None.
 *
 * @details     This macro enable specified CAN FD interrupt.
 */
void CANFD_EnableInt(CANFD_T *psCanfd, uint32_t u32IntLine0, uint32_t u32IntLine1, uint32_t u32TXBTIE, uint32_t u32TXBCIE)
{
    /*Setting the CANFD Interrupt Enabling*/
    psCanfd->IE |= u32IntLine0 | u32IntLine1;

    if (u32IntLine0 != 0)
    {
        /* Select specified interrupt event of Line0. */
        psCanfd->ILS &= ~u32IntLine0;
        /* Enable Line0 interrupt. */
        psCanfd->ILE |= CANFD_ILE_ENT0_Msk;
    }

    if (u32IntLine1 != 0)
    {
        /* Select specified interrupt event of Line1. */
        psCanfd->ILS |= u32IntLine1;
        /* Enable Line1 interrupt. */
        psCanfd->ILE |= CANFD_ILE_ENT1_Msk;
    }

    /*Setting the Tx Buffer Transmission Interrupt Enable*/
    psCanfd->TXBTIE |= u32TXBTIE;

    /*Tx Buffer Cancellation Finished Interrupt Enable*/
    psCanfd->TXBCIE |= u32TXBCIE;
}

/**
 * @brief       Disables CAN FD interrupts according to provided mask .
 *
 * @param[in]   psCanfd          The pointer of the specified CAN FD module.
 * @param[in]   u32IntLine0      The Interrupt Line 0 type select.
 * @param[in]   u32IntLine1      The Interrupt Line 1 type select.
 *                              - \ref CANFD_IE_ARAE_Msk     : Access to Reserved Address Interrupt
 *                              - \ref CANFD_IE_PEDE_Msk     : Protocol Error in Data Phase Interrupt
 *                              - \ref CANFD_IE_PEAE_Msk     : Protocol Error in Arbitration Phase Interrupt
 *                              - \ref CANFD_IE_WDIE_Msk     : Watchdog Interrupt
 *                              - \ref CANFD_IE_BOE_Msk      : Bus_Off Status Interrupt
 *                              - \ref CANFD_IE_EWE_Msk      : Warning Status Interrupt
 *                              - \ref CANFD_IE_EPE_Msk      : Error Passive Interrupt
 *                              - \ref CANFD_IE_ELOE_Msk     : Error Logging Overflow Interrupt
 *                              - \ref CANFD_IE_BEUE_Msk     : Bit Error Uncorrected Interrupt
 *                              - \ref CANFD_IE_BECE_Msk     : Bit Error Corrected Interrupt
 *                              - \ref CANFD_IE_DRXE_Msk     : Message stored to Dedicated Rx Buffer Interrupt
 *                              - \ref CANFD_IE_TOOE_Msk     : Timeout Occurred Interrupt
 *                              - \ref CANFD_IE_MRAFE_Msk    : Message RAM Access Failure Interrupt
 *                              - \ref CANFD_IE_TSWE_Msk     : Timestamp Wraparound Interrupt
 *                              - \ref CANFD_IE_TEFLE_Msk    : Tx Event FIFO Event Lost Interrupt
 *                              - \ref CANFD_IE_TEFFE_Msk    : Tx Event FIFO Full Interrupt
 *                              - \ref CANFD_IE_TEFWE_Msk    : Tx Event FIFO Watermark Reached Interrupt
 *                              - \ref CANFD_IE_TEFNE_Msk    : Tx Event FIFO New Entry Interrupt
 *                              - \ref CANFD_IE_TFEE_Msk     : Tx FIFO Empty Interrupt
 *                              - \ref CANFD_IE_TCFE_Msk     : Transmission Cancellation Finished Interrupt
 *                              - \ref CANFD_IE_TCE_Msk      : Transmission Completed Interrupt
 *                              - \ref CANFD_IE_HPME_Msk     : High Priority Message Interrupt
 *                              - \ref CANFD_IE_RF1LE_Msk    : Rx FIFO 1 Message Lost Interrupt
 *                              - \ref CANFD_IE_RF1FE_Msk    : Rx FIFO 1 Full Interrupt
 *                              - \ref CANFD_IE_RF1WE_Msk    : Rx FIFO 1 Watermark Reached Interrupt
 *                              - \ref CANFD_IE_RF1NE_Msk    : Rx FIFO 1 New Message Interrupt
 *                              - \ref CANFD_IE_RF0LE_Msk    : Rx FIFO 0 Message Lost Interrupt
 *                              - \ref CANFD_IE_RF0FE_Msk    : Rx FIFO 0 Full Interrupt
 *                              - \ref CANFD_IE_RF0WE_Msk    : Rx FIFO 0 Watermark Reached Interrupt
 *                              - \ref CANFD_IE_RF0NE_Msk    : Rx FIFO 0 New Message Interrupt
 *
 * @param[in]   u32TXBTIE        Disable Tx Buffer Transmission 0-31 Interrupt.
 * @param[in]   u32TXBCIE        Disable Tx Buffer Cancellation Finished 0-31 Interrupt.
 * @return      None.
 *
 * @details     This macro disable specified CAN FD interrupt.
 */
void CANFD_DisableInt(CANFD_T *psCanfd, uint32_t u32IntLine0, uint32_t u32IntLine1, uint32_t u32TXBTIE, uint32_t u32TXBCIE)
{
    psCanfd->IE &= ~(u32IntLine0 | u32IntLine1);

    if (u32IntLine0 != 0)
    {
        /* Cancel specified interrupt event of Line0. */
        psCanfd->ILS |= u32IntLine0;
    }
    if (psCanfd->ILS == ~0)
    {
        /* Disable Line0 interrupt */
        psCanfd->ILE &= ~CANFD_ILE_ENT0_Msk;
    }

    if (u32IntLine1 != 0)
    {
        /* Select specified interrupt event of Line1. */
        psCanfd->ILS &= ~u32IntLine1;
    }
    if (psCanfd->ILS == 0)
    {
        /* Disable Line1 interrupt */
        psCanfd->ILE &= ~CANFD_ILE_ENT1_Msk;
    }

    /*Setting the Tx Buffer Transmission Interrupt Disable*/
    psCanfd->TXBTIE &= ~u32TXBTIE;

    /*Tx Buffer Cancellation Finished Interrupt Disable*/
    psCanfd->TXBCIE &= ~u32TXBCIE;
}

/**
 * @brief       Global Filter Configuration (GFC).
 *
 * @param[in]   psCanfd          The pointer to CAN FD module base address.
 * @param[in]   eNMStdFrm        Accept/Reject Non-Matching Standard(11-bits) Frames.
 * @param[in]   eEMExtFrm        Accept/Reject Non-Matching Extended(29-bits) Frames.
 * @param[in]   u32RejRmtStdFrm  Reject/Filter Remote Standard Frames.
 * @param[in]   u32RejRmtExtFrm  Reject/Filter Remote Extended Frames.
 *
 * @return      None.
 *
 * @details     Global Filter Configuration.
 */
void CANFD_SetGFC(CANFD_T *psCanfd, E_CANFD_ACC_NON_MATCH_FRM eNMStdFrm, E_CANFD_ACC_NON_MATCH_FRM eEMExtFrm, uint32_t u32RejRmtStdFrm, uint32_t u32RejRmtExtFrm)
{
    psCanfd->GFC &= ~(CANFD_GFC_ANFS_Msk | CANFD_GFC_ANFE_Msk | CANFD_GFC_RRFS_Msk | CANFD_GFC_RRFE_Msk);
    psCanfd->GFC = (eNMStdFrm << CANFD_GFC_ANFS_Pos) |
                   (eEMExtFrm << CANFD_GFC_ANFE_Pos) |
                   (u32RejRmtStdFrm << CANFD_GFC_RRFS_Pos) |
                   (u32RejRmtExtFrm << CANFD_GFC_RRFE_Pos);
}

/**
 * @brief       Get Rx FIFO water level.
 *
 * @param[in]   psCanfd         The pointer to CANFD module base address.
 * @param[in]   u32RxFifoNum    0: RX FIFO_0, 1: RX_FIFO_1
 *
 * @return      Rx FIFO water level.
 *
 * @details     Get Rx FIFO water level.
 */
uint32_t CANFD_GetRxFifoWaterLvl(CANFD_T *psCanfd, uint32_t u32RxFifoNum)
{
    uint32_t u32WaterLevel = 0;

    if (u32RxFifoNum == 0)
        u32WaterLevel = ((psCanfd->RXF0C & CANFD_RXF0C_F0WM_Msk) >> CANFD_RXF0C_F0WM_Pos);
    else
        u32WaterLevel = ((psCanfd->RXF1C & CANFD_RXF1C_F1WM_Msk) >> CANFD_RXF1C_F1WM_Pos);

    return u32WaterLevel;
}

/**
 * @brief       Cancel a Tx buffer transmission request.
 *
 * @param[in]   psCanfd         The pointer to CANFD module base address.
 * @param[in]   u32TxBufIdx     Tx buffer index number
 *
 * @return      None.
 *
 * @details     Cancel a Tx buffer transmission request.
 */
void CANFD_TxBufCancelReq(CANFD_T *psCanfd, uint32_t u32TxBufIdx)
{
    psCanfd->TXBCR = psCanfd->TXBCR | (0x1ul << u32TxBufIdx);
}

/**
 * @brief       Checks if a Tx buffer cancellation request has been finished or not.
 *
 * @param[in]   psCanfd         The pointer to CAN FD module base address.
 * @param[in]   u32TxBufIdx     Tx buffer index number
 *
 * @return      0: cancellation finished.
 *              1: cancellation pending
 *
 * @details     Checks if a Tx buffer cancellation request has been finished or not.
 */
uint32_t CANFD_IsTxBufCancelFin(CANFD_T *psCanfd, uint32_t u32TxBufIdx)
{
    /* wait for completion */
    return ((psCanfd->TXBCR & (0x1ul << u32TxBufIdx)) >> u32TxBufIdx);
}

/**
 * @brief       Checks if a Tx buffer transmission has occurred or not.
 *
 * @param[in]   psCanfd         The pointer to CAN FD module base address.
 * @param[in]   u32TxBufIdx     Tx buffer index number
 *
 * @return     0: No transmission occurred.
 *             1: Transmission occurred
 *
 * @details     Checks if a Tx buffer transmission has occurred or not.
 */
uint32_t CANFD_IsTxBufTransmitOccur(CANFD_T *psCanfd, uint32_t u32TxBufIdx)
{
    return ((psCanfd->TXBTO & (0x1ul << u32TxBufIdx)) >> u32TxBufIdx);
}

/**
 * @brief       Get Tx event fifo water level
 *
 * @param[in]   psCanfd       The pointer to CANFD module base address.
 *
 * @return      Tx event fifo water level.
 *
 * @details     Get Tx event fifo water level.
 */
uint32_t CANFD_GetTxEvntFifoWaterLvl(CANFD_T *psCanfd)
{

    return ((psCanfd->TXEFC & CANFD_TXEFC_EFWM_Msk) >> CANFD_TXEFC_EFWM_Pos);
}

/**
 * @brief       Get CAN FD interrupts status.
 *
 * @param[in]   psCanfd         The pointer of the specified CAN FD module.
 * @param[in]   u32IntTypeFlag  Interrupt Type Flag, should be
 *                              - \ref CANFD_IR_ARA_Msk     : Access to Reserved Address interrupt Indicator
 *                              - \ref CANFD_IR_PED_Msk     : Protocol Error in Data Phase interrupt Indicator
 *                              - \ref CANFD_IR_PEA_Msk     : Protocol Error in Arbitration Phase interrupt Indicator
 *                              - \ref CANFD_IR_WDI_Msk     : Watchdog interrupt Indicator
 *                              - \ref CANFD_IR_BO_Msk      : Bus_Off Status interrupt Indicator
 *                              - \ref CANFD_IR_EW_Msk      : Warning Status interrupt Indicator
 *                              - \ref CANFD_IR_EP_Msk      : Error Passive interrupt Indicator
 *                              - \ref CANFD_IR_ELO_Msk     : Error Logging Overflow interrupt Indicator
 *                              - \ref CANFD_IR_DRX_Msk     : Message stored to Dedicated Rx Buffer interrupt Indicator
 *                              - \ref CANFD_IR_TOO_Msk     : Timeout Occurred interrupt Indicator
 *                              - \ref CANFD_IR_MRAF_Msk    : Message RAM Access Failure interrupt Indicator
 *                              - \ref CANFD_IR_TSW_Msk     : Timestamp Wraparound interrupt Indicator
 *                              - \ref CANFD_IR_TEFL_Msk    : Tx Event FIFO Event Lost interrupt Indicator
 *                              - \ref CANFD_IR_TEFF_Msk    : Tx Event FIFO Full Indicator
 *                              - \ref CANFD_IR_TEFW_Msk    : Tx Event FIFO Watermark Reached Interrupt Indicator
 *                              - \ref CANFD_IR_TEFN_Msk    : Tx Event FIFO New Entry Interrupt Indicator
 *                              - \ref CANFD_IR_TFE_Msk     : Tx FIFO Empty Interrupt Indicator
 *                              - \ref CANFD_IR_TCF_Msk     : Transmission Cancellation Finished Interrupt Indicator
 *                              - \ref CANFD_IR_TC_Msk      : Transmission Completed interrupt Indicator
 *                              - \ref CANFD_IR_HPM_Msk     : High Priority Message Interrupt Indicator
 *                              - \ref CANFD_IR_RF1L_Msk    : Rx FIFO 1 Message Lost Interrupt Indicator
 *                              - \ref CANFD_IR_RF1F_Msk    : Rx FIFO 1 Full Interrupt Indicator
 *                              - \ref CANFD_IR_RF1W_Msk    : Rx FIFO 1 Watermark Reached Interrupt Indicator
 *                              - \ref CANFD_IR_RF1N_Msk    : Rx FIFO 1 New Message Interrupt Indicator
 *                              - \ref CANFD_IR_RF0L_Msk    : Rx FIFO 0 Message Lost Interrupt Indicator
 *                              - \ref CANFD_IR_RF0F_Msk    : Rx FIFO 0 Full Interrupt Indicator
 *                              - \ref CANFD_IR_RF0W_Msk    : Rx FIFO 0 Watermark Reached Interrupt Indicator
 *                              - \ref CANFD_IR_RF0N_Msk    : Rx FIFO 0 New Message Interrupt Indicator
 *
 * @return      None.
 *
 * @details     This function gets all CAN FD interrupt status flags.
 */
uint32_t CANFD_GetStatusFlag(CANFD_T *psCanfd, uint32_t u32IntTypeFlag)
{
    return (psCanfd->IR & u32IntTypeFlag);
}

/**
 * @brief       Clears the CAN FD module interrupt flags
 *
 * @param[in]   psCanfd           The pointer of the specified CANFD module.
 * @param[in]   u32InterruptFlag  The specified interrupt of CAN FD module
 *                               - \ref CANFD_IR_ARA_Msk     : Access to Reserved Address interrupt Indicator
 *                               - \ref CANFD_IR_PED_Msk     : Protocol Error in Data Phase interrupt Indicator
 *                               - \ref CANFD_IR_PEA_Msk     : Protocol Error in Arbitration Phase interrupt Indicator
 *                               - \ref CANFD_IR_WDI_Msk     : Watchdog interrupt Indicator
 *                               - \ref CANFD_IR_BO_Msk      : Bus_Off Status interrupt Indicator
 *                               - \ref CANFD_IR_EW_Msk      : Warning Status interrupt Indicator
 *                               - \ref CANFD_IR_EP_Msk      : Error Passive interrupt Indicator
 *                               - \ref CANFD_IR_ELO_Msk     : Error Logging Overflow interrupt Indicator
 *                               - \ref CANFD_IR_DRX_Msk     : Message stored to Dedicated Rx Buffer interrupt Indicator
 *                               - \ref CANFD_IR_TOO_Msk     : Timeout Occurred interrupt Indicator
 *                               - \ref CANFD_IR_MRAF_Msk    : Message RAM Access Failure interrupt Indicator
 *                               - \ref CANFD_IR_TSW_Msk     : Timestamp Wraparound interrupt Indicator
 *                               - \ref CANFD_IR_TEFL_Msk    : Tx Event FIFO Event Lost interrupt Indicator
 *                               - \ref CANFD_IR_TEFF_Msk    : Tx Event FIFO Full Indicator
 *                               - \ref CANFD_IR_TEFW_Msk    : Tx Event FIFO Watermark Reached Interrupt Indicator
 *                               - \ref CANFD_IR_TEFN_Msk    : Tx Event FIFO New Entry Interrupt Indicator
 *                               - \ref CANFD_IR_TFE_Msk     : Tx FIFO Empty Interrupt Indicator
 *                               - \ref CANFD_IR_TCF_Msk     : Transmission Cancellation Finished Interrupt Indicator
 *                               - \ref CANFD_IR_TC_Msk      : Transmission Completed interrupt Indicator
 *                               - \ref CANFD_IR_HPM_Msk     : High Priority Message Interrupt Indicator
 *                               - \ref CANFD_IR_RF1L_Msk    : Rx FIFO 1 Message Lost Interrupt Indicator
 *                               - \ref CANFD_IR_RF1F_Msk    : Rx FIFO 1 Full Interrupt Indicator
 *                               - \ref CANFD_IR_RF1W_Msk    : Rx FIFO 1 Watermark Reached Interrupt Indicator
 *                               - \ref CANFD_IR_RF1N_Msk    : Rx FIFO 1 New Message Interrupt Indicator
 *                               - \ref CANFD_IR_RF0L_Msk    : Rx FIFO 0 Message Lost Interrupt Indicator
 *                               - \ref CANFD_IR_RF0F_Msk    : Rx FIFO 0 Full Interrupt Indicator
 *                               - \ref CANFD_IR_RF0W_Msk    : Rx FIFO 0 Watermark Reached Interrupt Indicator
 *                               - \ref CANFD_IR_RF0N_Msk    : Rx FIFO 0 New Message Interrupt Indicator
 *
 * @return      None.
 *
 * @details     This function clears CAN FD interrupt status flags.
 */
void CANFD_ClearStatusFlag(CANFD_T *psCanfd, uint32_t u32InterruptFlag)
{
    /* Write 1 to clear status flag. */
    psCanfd->IR |= u32InterruptFlag;
}

/**
 * @brief       Gets the CAN FD Bus Error Counter value.
 *
 * @param[in]   psCanfd        The pointer of the specified CAN FD module.
 * @param[in]   pu8TxErrBuf    TxErrBuf Buffer to store Tx Error Counter value.
 * @param[in]   pu8RxErrBuf    RxErrBuf Buffer to store Rx Error Counter value.
 *
 * @return      None.
 *
 * @details     This function gets the CAN FD Bus Error Counter value for both Tx and Rx direction.
 *              These values may be needed in the upper layer error handling.
 */
void CANFD_GetBusErrCount(CANFD_T *psCanfd, uint8_t *pu8TxErrBuf, uint8_t *pu8RxErrBuf)
{
    if (pu8TxErrBuf)
    {
        *pu8TxErrBuf = (uint8_t)((psCanfd->ECR >> CANFD_ECR_TEC_Pos) & CANFD_ECR_TEC_Msk);
    }

    if (pu8RxErrBuf)
    {
        *pu8RxErrBuf = (uint8_t)((psCanfd->ECR >> CANFD_ECR_REC_Pos) & CANFD_ECR_REC_Msk);
    }
}

/**
 * @brief       CAN FD Run to the Normal Operation.
 *
 * @param[in]   psCanfd        The pointer of the specified CAN FD module.
 * @param[in]   u8Enable       TxErrBuf Buffer to store Tx Error Counter value.
 *
 * @retval      CANFD_OK          CANFD operation OK.
 * @retval      CANFD_ERR_TIMEOUT CANFD operation abort due to timeout error.
 *
 * @details     This function gets the CAN FD Bus Error Counter value for both Tx and Rx direction.
 *              These values may be needed in the upper layer error handling.
 */
int32_t CANFD_RunToNormal(CANFD_T *psCanfd, uint8_t u8Enable)
{
    uint32_t u32TimeOutCount = SystemCoreClock; // 1 second timeout

    if (u8Enable)
    {
        /* start operation */
        psCanfd->CCCR &= ~(CANFD_CCCR_CCE_Msk | CANFD_CCCR_INIT_Msk);

        /* Check INIT bit is released */
        while (psCanfd->CCCR & CANFD_CCCR_INIT_Msk)
        {
            if (--u32TimeOutCount == 0) return CANFD_ERR_TIMEOUT;
        }
    }
    else
    {
        /* init mode */
        psCanfd->CCCR |= CANFD_CCCR_INIT_Msk | CANFD_CCCR_CCE_Msk;

        /* Wait run to INIT mode */
        while (!(psCanfd->CCCR & CANFD_CCCR_INIT_Msk))
        {
            if (--u32TimeOutCount == 0) return CANFD_ERR_TIMEOUT;
        }
    }

    return CANFD_OK;
}

void *CANFD_Transfer(CANFD_T *psCanfd, E_CANFD_MSG eCanfdMsg, void *psUserData, uint32_t u32Idx)
{
    void *pvRetMsgBufAddr = NULL;

    switch (eCanfdMsg)
    {
    case eCANFD_MSG_RF0: /*!< RX FIFO0  */
    case eCANFD_MSG_RF1: /*!< RX FIFO1  */
    {
        uint32_t msgLostBit;
        __I  uint32_t *pRXFS;
        __IO uint32_t *pRXFC, *pRXFA;

        if (eCanfdMsg == eCANFD_MSG_RF0)
        {
            pRXFS = &(psCanfd->RXF0S);
            pRXFC = &(psCanfd->RXF0C);
            pRXFA = &(psCanfd->RXF0A);
            msgLostBit = CANFD_IR_RF0L_Msk;
        }
        else
        {
            pRXFS = &(psCanfd->RXF1S);
            pRXFC = &(psCanfd->RXF1C);
            pRXFA = &(psCanfd->RXF1A);
            msgLostBit = CANFD_IR_RF1L_Msk;
        }

        /* if FIFO is not empty */
        if ((*pRXFS & CANFD_RXF0S_F0FL_Msk) > 0)
        {
            uint8_t GetIndex = (uint8_t)((*pRXFS & CANFD_RXF0S_F0GI_Msk) >> CANFD_RXF0S_F0GI_Pos);

            CANFD_RX_BUF_T *psRxBuffer = (CANFD_RX_BUF_T *)(CANFD_SRAM_BASE_ADDR(psCanfd) + (*pRXFC & CANFD_RXF0C_F0SA_Msk) + (GetIndex * sizeof(CANFD_RX_BUF_T)));

            /* read the message */
            if (psUserData)
                memcpy((void *)psUserData, psRxBuffer, sizeof(CANFD_RX_BUF_T));

            /* we got the message */
            *pRXFA = GetIndex;

            /* check for overflow */
            if ((*pRXFS & CANFD_RXF0S_RF0L_Msk))
            {
                /* clear overflow flag */
                psCanfd->IR = msgLostBit;
            }

            pvRetMsgBufAddr = (void *)psRxBuffer;
        }
    }
    break;

    case eCANFD_MSG_DRB: /*!< Dedicated RX Buffe  */
    {
        if (u32Idx < CANFD_MAX_RX_BUF_ELEMS)
        {
            uint32_t newData = 0;

            if (u32Idx < 32)
                newData = (psCanfd->NDAT1 >> u32Idx) & 1;
            else
                newData = (psCanfd->NDAT2 >> (u32Idx - 32)) & 1;

            /* new message is waiting to be read */
            if (newData)
            {
                /* get memory location of rx buffer */
                CANFD_RX_BUF_T *psRxBuffer = CANFD_GET_RXBUF_ADDR(psCanfd, u32Idx);

                /* read the message */
                if (psUserData)
                {
                    memcpy((void *)psUserData, psRxBuffer, sizeof(CANFD_RX_BUF_T));

                    /* clear 'new data' flag */
                    if (u32Idx < 32)
                        psCanfd->NDAT1 = psCanfd->NDAT1 | (1UL << u32Idx);
                    else
                        psCanfd->NDAT2 = psCanfd->NDAT2 | (1UL << (u32Idx - 32));

                    pvRetMsgBufAddr = (void *) psRxBuffer;
                }
            }
        }
    }
    break;

    case eCANFD_MSG_DTB: /*!< Dedicated TX Buffe  */
    {
        /* Check it is over max TX buffer numbers or not.*/
        if (u32Idx >= CANFD_MAX_TX_BUF_ELEMS)
            goto EXIT_CANFD_Transfer;

        /* Transmission is pending in this message buffer */
        if ((psCanfd->TXBRP & (1UL << u32Idx)))
            goto EXIT_CANFD_Transfer;

        if (psUserData)
        {
            CANFD_TX_BUF_T *psDstTxBuf = CANFD_GET_TXBUF_ADDR(psCanfd, u32Idx);

            memcpy((void *)psDstTxBuf, psUserData, sizeof(CANFD_TX_BUF_T));

            /* Request append new Tx buffer. */
            psCanfd->TXBAR = (1 << u32Idx);

            pvRetMsgBufAddr = (void *)psDstTxBuf;
        }
    }
    break;

    case eCANFD_MSG_TEF: /*!< TX EVENT FIFO */
    {
        if ((psCanfd->TXEFS & CANFD_TXEFS_EFFL_Msk) > 0)
        {
            uint8_t GetIndex = (uint8_t)((psCanfd->TXEFS & CANFD_TXEFS_EFGI_Msk) >> CANFD_TXEFS_EFGI_Pos);

            CANFD_TX_EVENT_T *psTxEvtBuf = CANFD_GET_TXEVENT_ADDR(psCanfd, GetIndex);

            /* Read the message */
            if (psUserData)
            {
                memcpy(psUserData, (void *)psTxEvtBuf, sizeof(CANFD_TX_EVENT_T));

                /* Got the message */
                psCanfd->TXEFA = GetIndex;
            }

            /* check for overflow */
            if ((psCanfd->TXEFS & CANFD_TXEFS_TEFL_Msk))
            {
                /* clear overflow flag */
                psCanfd->IR |= CANFD_IR_TEFF_Msk;
            }

            pvRetMsgBufAddr = (void *)psTxEvtBuf;
        }
    }
    break;

    case eCANFD_MSG_SID: /*!< Standard ID Filter */
    {
        /* ignore if index is too high */
        if (u32Idx >= CANFD_MAX_11_BIT_FTR_ELEMS)
            goto EXIT_CANFD_Transfer;

        if (psUserData)
        {
            /*Get the Filter List Configuration Address in the RAM*/
            CANFD_SID_FILTER_T *psFilter = CANFD_GET_SID_ADDR(psCanfd, u32Idx);

            /* Write the Extended ID filter element to RAM */
            memcpy((void *)psFilter, (void *)psUserData, sizeof(CANFD_SID_FILTER_T));

            pvRetMsgBufAddr = (void *)psFilter;
        }
    }
    break;

    case eCANFD_MSG_XID: /*!< Extended ID Filter */
    {
        /* Ignore if index is too high */
        if (u32Idx >= CANFD_MAX_29_BIT_FTR_ELEMS)
            goto EXIT_CANFD_Transfer;

        if (psUserData)
        {
            /* Get the Filter List Configuration Address in the RAM */
            CANFD_XID_FILTER_T *psFilter = CANFD_GET_XID_ADDR(psCanfd, u32Idx);

            /* Write the Extended ID filter element to RAM */
            memcpy((void *)psFilter, psUserData, sizeof(CANFD_XID_FILTER_T));

            pvRetMsgBufAddr = (void *)psFilter;
        }
    }
    break;

    default:
        break;
    }

EXIT_CANFD_Transfer:

    return pvRetMsgBufAddr;
}


/** @} end of group CANFD_EXPORTED_FUNCTIONS */

/** @} end of group CANFD_Driver */

/** @} end of group Standard_Driver */
