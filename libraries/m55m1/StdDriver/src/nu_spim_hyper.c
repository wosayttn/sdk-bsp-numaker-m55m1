/**************************************************************************//**
 * @file    spim_hyper.c
 * @version V1.00
 * @brief   SPIM_HYPER driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SPIM_HYPER_Driver SPIM_HYPER Driver
  @{
*/

/** @addtogroup SPIM_HYPER_EXPORTED_FUNCTIONS SPIM_HYPER Exported Functions
  @{
*/

/**
  * @brief      Wait Hyper Bus interface Idle
  * @param      spim     Specify SPIM peripheral
  * @retval     SPIM_HYPER_OK          SPIM operation OK.
  *             SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @details    This function is used to wait Hyper Bus interface idle.
  */
static int32_t spim_hyper_wait_cmdidle(SPIM_T *spim)
{
    volatile int32_t i32TimeOutCnt = SPIM_HYPER_TIMEOUT;

    while (spim->HYPER_CMD != SPIM_HYPER_CMD_IDLE)
    {
        if (--i32TimeOutCnt <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    return SPIM_HYPER_OK;
}

/**
  * @brief      Enable Hyper Bus Mode
  * @param      spim         Specify SPIM peripheral
  * @param[in]  u32HyperMode Select HyperRAM or HyperFlash Device
  *                         - \ref SPIM_HYPERRAM_MODE
  *                         - \ref SPIM_HYPERFLASH_MODE
  * @param[in]  u32Div       Hyper Bus device clock divider, the setting values of DIVIDER are only 1 and 2.
  * @return     None.
  * @details    This function is used to enable SPIM Hyper Bus Mode and set the clock divider.
  */
void SPIM_HYPER_Init(SPIM_T *spim, uint32_t u32HyperMode, uint32_t u32Div)
{
    /* Enable SPIM Hyper Bus Mode */
    SPIM_HYPER_ENABLE_HYPMODE(spim, u32HyperMode);

    SPIM_HYPER_SET_CLKDIV(spim, (((u32Div != 1) || (u32Div != 2)) ? 1 : u32Div));
}

/**
  * @brief      SPIM Start Transfer And Wait Busy Status.
  * @param      spim
  * @param      u32IsSync   Wait Busy Status
  * @return     SPIM_HYPER_OK          SPIM write done.
  *             SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  */
int32_t SPIM_HYPER_WaitSPIMENDone(SPIM_T *spim, uint32_t u32IsSync)
{
    volatile int32_t i32TimeOutCount = SPIM_HYPER_TIMEOUT;

    /* Start SPIM Transfer, wait for operation complete according to u32IsSync */
    SPIM_HYPER_SET_GO(spim);

    if (u32IsSync)  /* Wait SPIM Busy status if u32IsSync is set */
    {
        while ((SPIM_HYPER_GET_INT(spim) ? SPIM_HYPER_GET_INTSTS(spim) : SPIM_HYPER_IS_BUSY(spim)))
        {
            if (--i32TimeOutCount <= 0)
            {
                return SPIM_HYPER_ERR_TIMEOUT;
            }
        }
    }

    return SPIM_HYPER_OK;
}

/**
  * @brief  Reset hyper chip function
  * @param  spim
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_Reset(SPIM_T *spim)
{
    spim->HYPER_CMD = SPIM_HYPER_CMD_RESET;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Exit from Hybrid sleep and deep Power down function
  * @param  spim
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_ExitHSAndDPD(SPIM_T *spim)
{
    spim->HYPER_CMD = SPIM_HYPER_CMD_EXIT_HS_PD;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Read hyper chip register space
  * @param  spim
  * @param  u32Addr Address of hyper chip register space
  *                 - \ref SPIM_HYPER_HRAM_ID_REG0       : 0x0000_0000 = Identification Register 0
  *                 - \ref SPIM_HYPER_HRAM_ID_REG1       : 0x0000_0002 = Identification Register 1
  *                 - \ref SPIM_HYPER_HRAM_CONFIG_REG0   : 0x0000_1000 = Configuration Register 0
  *                 - \ref SPIM_HYPER_HRAM_CONFIG_REG1   : 0x0000_1002 = Configuration Register 1
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_FAIL    SPIM operation Fail.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_ReadHyperRAMReg(SPIM_T *spim, uint32_t u32Addr)
{
    if ((u32Addr != SPIM_HYPER_HRAM_ID_REG0) &&
            (u32Addr != SPIM_HYPER_HRAM_ID_REG1) &&
            (u32Addr != SPIM_HYPER_HRAM_CONFIG_REG0) &&
            (u32Addr != SPIM_HYPER_HRAM_CONFIG_REG1))
    {
        return SPIM_HYPER_ERR_FAIL;
    }

    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_HRAM_REGISTER;

    spim_hyper_wait_cmdidle(spim);

    return (spim->HYPER_RDATA & 0xFFFF); // Return 16-bit value
}

/**
  * @brief  Write Hyper Chip register space
  * @param  spim
  * @param  u32Addr Address of Hyper Chip register space
  *                 - \ref SPIM_HYPER_HRAM_ID_REG0     : 0x0000_0000 = Identification Register 0
  *                 - \ref SPIM_HYPER_HRAM_ID_REG1     : 0x0000_0002 = Identification Register 1
  *                 - \ref SPIM_HYPER_HRAM_CONFIG_REG0 : 0x0000_1000 = Configuration Register 0
  *                 - \ref SPIM_HYPER_HRAM_CONFIG_REG1 : 0x0000_1002 = Configuration Register 1
  * @param  u32Value Configure HyperRAM Register Value
  * @return SPIM_HYPER_OK      SPIM operation OK.
  *         SPIM_HYPER_ERR_FAIL    SPIM operation Fail.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_WriteHyperRAMReg(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Value)
{
    if ((u32Addr != SPIM_HYPER_HRAM_ID_REG0) &&
            (u32Addr != SPIM_HYPER_HRAM_ID_REG1) &
            (u32Addr != SPIM_HYPER_HRAM_CONFIG_REG0) &&
            (u32Addr != SPIM_HYPER_HRAM_CONFIG_REG1))
    {
        return SPIM_HYPER_ERR_FAIL;
    }

    SPIM_HYPER_CLEAR_WDATA(spim);

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = u32Value;

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_HRAM_REGISTER;

    spim_hyper_wait_cmdidle(spim);

    return SPIM_HYPER_OK;
}

/**
  * @brief  Read 1 word from hyper chip space
  * @param  spim
  * @param  u32Addr  Address of hyper chip space
  * @return The 16 bit data of hyper chip space.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
int16_t SPIM_HYPER_Read1Word(SPIM_T *spim, uint32_t u32Addr)
{
    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_1_WORD;

    spim_hyper_wait_cmdidle(spim);

    return (spim->HYPER_RDATA & 0xFFFF);
}

/**
  * @brief  Read 2 word from hyper chip space
  * @param  spim
  * @param  u32Addr  Address of hyper chip space
  * @return The 32bit data of hyper chip space.
  */
int32_t SPIM_HYPER_Read2Word(SPIM_T *spim, uint32_t u32Addr)
{
    spim->HYPER_ADR = u32Addr;

    spim->HYPER_CMD = SPIM_HYPER_CMD_READ_2_WORD;

    spim_hyper_wait_cmdidle(spim);

    return (spim->HYPER_RDATA & 0xFFFFFFFF);
}

/**
  * @brief  Write 1 byte to hyper chip space
  * @param  spim
  * @param  u32Addr  Address of hyper chip space
  * @param  u8Data   8 bits data to be written to hyper chip space
  * @return SPIM_HYPER_OK      SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_Write1Byte(SPIM_T *spim, uint32_t u32Addr, uint8_t u8Data)
{
    SPIM_HYPER_CLEAR_WDATA(spim);

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = (uint8_t)(u8Data & 0xFF);

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_1_BYTE;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Write 2 bytes to Hyper Chip space
  * @param  spim
  * @param  u32Addr  Address of Hyper Chip space
  * @param  u16Data  16 bits data to be written to Hyper Chip space
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_Write2Byte(SPIM_T *spim, uint32_t u32Addr, uint16_t u16Data)
{
    SPIM_HYPER_CLEAR_WDATA(spim);

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = (uint16_t)(u16Data & 0xFFFF);

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_2_BYTE;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Write 3 bytes to Hyper Chip space
  * @param  spim
  * @param  u32Addr  Address of Hyper Chip space
  * @param  u32Data  24 bits data to be written to Hyper Chip space
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_Write3Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data)
{
    SPIM_HYPER_CLEAR_WDATA(spim);

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = (u32Data & 0xFFFFFF);

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_3_BYTE;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Write 4 byte to hyper chip space
  * @param  spim
  * @param  u32Addr  Address of hyper chip space
  * @param  u32Data  32 bits data to be written to hyper chip space
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_Write4Byte(SPIM_T *spim, uint32_t u32Addr, uint32_t u32Data)
{
    SPIM_HYPER_CLEAR_WDATA(spim);

    spim->HYPER_ADR = u32Addr;
    spim->HYPER_WDATA = (u32Data & 0xFFFFFFFF);

    spim->HYPER_CMD = SPIM_HYPER_CMD_WRITE_4_BYTE;

    return spim_hyper_wait_cmdidle(spim);
}

/**
  * @brief  Write data to HyperBus Module.
  * @param  spim
  * @param  u32Addr     Start address to write.
  * @param  pu8WrBuf    Transmit buffer.
  * @param  u32NTx      Number of bytes to write.
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_DMAWrite(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8WrBuf, uint32_t u32NTx)
{
    SPIM_HYPER_SET_OPMODE(spim, SPIM_HYPER_OPMODE_PAGEWRITE);  /* Switch to DMA Write mode.   */

    spim->SRAMADDR = (uint32_t) pu8WrBuf;
    spim->DMACNT = u32NTx;
    spim->FADDR = u32Addr;

    return SPIM_HYPER_WaitSPIMENDone(spim, SPIM_HYPER_OP_ENABLE);
}

/**
  * @brief  Read data from HyperBus Module.
  * @param  spim
  * @param  u32Addr     Start address to read.
  * @param  pu8RdBuf    Receive buffer.
  * @param  u32NRx      Number of bytes to read.
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_DMARead(SPIM_T *spim, uint32_t u32Addr, uint8_t *pu8RdBuf, uint32_t u32NRx)
{
    SPIM_HYPER_SET_OPMODE(spim, SPIM_HYPER_OPMODE_PAGEREAD);   /* Switch to DMA Read mode.  */
    spim->SRAMADDR = (uint32_t) pu8RdBuf;               /* SRAM u32Address. */
    spim->DMACNT = u32NRx;                              /* Transfer length. */
    spim->FADDR = u32Addr;                              /* Flash u32Address. */

    return SPIM_HYPER_WaitSPIMENDone(spim, SPIM_HYPER_OP_ENABLE);
}

/**
  * @brief  SPIM Hyper Mode Enter DMM Mode
  * @param  spim
  * @return None.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
void SPIM_HYPER_EnterDirectMapMode(SPIM_T *spim)
{
    SPIM_HYPER_CLR_DMM_TIMEOUT_STS(spim);
    SPIM_HYPER_SET_OPMODE(spim, SPIM_HYPER_OPMODE_DIRECTMAP);  /* Switch to DMA Write mode.   */
}

/**
  * @brief  SPIM Hyper Mode Exit DMM Mode
  * @param  spim
  * @return None.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT if waiting Hyper Chip time-out.
  */
void SPIM_HYPER_ExitDirectMapMode(SPIM_T *spim)
{
    SPIM_HYPER_SET_OPMODE(spim, SPIM_HYPER_OPMODE_IO);       /* Switch back to Normal mode.  */
}

/**
  * @brief  Wait Hyper Direct Map Mode Read/Write Done.
  * @param  spim
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
int32_t SPIM_HYPER_DMMDone(SPIM_T *spim)
{
    volatile int32_t u32TimeOutCount = SPIM_HYPER_TIMEOUT;

    SPIM_HYPER_ENABLE_DMMDONE(spim);       /* HyperBus DMM Mode Done.  */

    while (SPIM_HYPER_GET_DMMDONE(spim))
    {
        if (--u32TimeOutCount <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    return SPIM_HYPER_OK;
}

/**
  * @brief      Initialize SPIM Hyper DLL.
  * @param      spim     SPIM port handler
  * @retval     SPIM_HYPER_OK          SPIM operation OK.
  *             SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @details    This function is used to initialize SPIM Hyper bus DLL.
  * @note       When system wake up reset, please calling this function reinintialize DLL.
  */
int32_t SPIM_HYPER_INIT_DLL(SPIM_T *spim)
{
    volatile int i32Timeout = SPIM_HYPER_TIMEOUT;
    uint32_t u32Div = 0;

    u32Div = SPIM_HYPER_GET_CLKDIV(spim);

    if (u32Div >= 3)
    {
        u32Div = 3;
    }

    SPIM_HYPER_SET_DLLDIV(spim, u32Div);
    /* SPIM starts to send DLL reference clock to DLL circuit
    that the frequency is the same as the SPIM output bus clock. */
    SPIM_HYPER_ENABLE_DLLOLDO(spim, SPIM_HYPER_OP_ENABLE);

    /* User asserts this control register to 0x1,
       the DLL circuit begins searching for lock with DLL reference clock. */
    SPIM_HYPER_ENABLE_DLLOVRST(spim, SPIM_HYPER_OP_ENABLE);

    i32Timeout = SPIM_HYPER_TIMEOUT;

    /* Polling the DLL status register DLLCKON to 0x1,
       and the value 0x1 indicates that clock divider circuit inside DLL is enabled. */
    while (SPIM_HYPER_GET_DLLOVRST(spim) == SPIM_HYPER_OP_ENABLE)
    {
        if (--i32Timeout <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    i32Timeout = SPIM_HYPER_TIMEOUT;

    /* Polling the DLL status register DLLCKON to 0x1,
       and the value 0x1 indicates that clock divider circuit inside DLL is enabled. */
    while (SPIM_HYPER_GET_DLLCLKON(spim) != SPIM_HYPER_OP_ENABLE)
    {
        if (--i32Timeout <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    i32Timeout = SPIM_HYPER_TIMEOUT;

    /* Polling the DLL status register DLLLOCK to 0x1,
       and the value 0x1 indicates that DLL circuit is in lock state */
    while (SPIM_HYPER_GET_DLLLOCK(spim) != SPIM_HYPER_OP_ENABLE)
    {
        if (--i32Timeout <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    i32Timeout = SPIM_HYPER_TIMEOUT;

    /* Polling the DLL status register DLLREADY to 0x1,
       and the value 0x1 indicates that output of DLL circuit is ready. */
    while (SPIM_HYPER_GET_DLLREADY(spim) != SPIM_HYPER_OP_ENABLE)
    {
        if (--i32Timeout <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    return SPIM_HYPER_OK;
}

/**
  * @brief      Set SPIM DLL delay number.
  * @param      spim   SPIM port handler
  * @param      u32DelayNum   The delay number of SPIM DLL.
  * @retval     SPIM_HYPER_OK          SPIM operation OK.
  *             SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @details    This function is used to set SPIM DLL delay number.
  *             The delay number is used to adjust the sampling edge of input clock
  *             to SPIM.
  */
int32_t SPIM_HYPER_SetDLLDelayNum(SPIM_T *spim, uint32_t u32DelayNum)
{
    volatile int i32Timeout = SPIM_HYPER_TIMEOUT;

    if (SPIM_HYPER_GET_DLLOLDO(spim) != SPIM_HYPER_OP_ENABLE)
    {
        SPIM_HYPER_INIT_DLL(spim);
    }

    /* Set this valid delay number to control register DLL_DNUM. */
    SPIM_HYPER_SET_DLLDLY_NUM(spim, u32DelayNum);

    i32Timeout = SPIM_HYPER_TIMEOUT;

    /* Polling DLL status register DLL_REF to 0
       to know the updating flow of DLL delay step number is finish or not. */
    while (SPIM_HYPER_GET_DLLREF(spim) != SPIM_HYPER_OP_DISABLE)
    {
        if (--i32Timeout <= 0)
        {
            return SPIM_HYPER_ERR_TIMEOUT;
        }
    }

    return SPIM_HYPER_OK;
}

/** @} end of group SPIM_HYPER_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_HYPER_Driver */
/** @} end of group Standard_Driver */
