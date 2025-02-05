/**************************************************************************//**
 * @file    nu_spim.h
 * @version V1.00
 * @brief   SPIM driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef __NU_SPIM_H__
#define __NU_SPIM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SPIM_Driver SPIM Driver
  @{
*/

/** @addtogroup SPIM_EXPORTED_CONSTANTS SPIM Exported Constants
  @{
*/

#define SPIM_DMM0_SADDR                     (0x82000000UL)  /*!< SPIM0 DMM mode memory map base secure address */
#define SPIM_DMM0_NSADDR                    (0x92000000UL)  /*!< SPIM0 DMM mode memory map base non secure address */

#if defined (SCU_INIT_D0PNS2_VAL) && (SCU_INIT_D0PNS2_VAL & SCU_D0PNS2_SPIM0_Msk)
#define SPIM_DMM0_ADDR                      SPIM_DMM0_NSADDR
#else
#define SPIM_DMM0_ADDR                      SPIM_DMM0_SADDR
#endif

#define SPIM_DMM_SIZE                       (0x2000000UL)       /*!< DMM mode memory mapping size */

#define SPIM_MAX_DLL_LATENCY                (0x20)              /*!< Maximum DLL trim number */
#define SPIM_MAX_RX_DLY_NUM                 (0x10)              /*!< Maximum RX delay trim number */

#define SPIM_DTR_ON                         (0x01)              /* Double data rate mode enable */
#define SPIM_DTR_OFF                        (0x00)              /* Double data rate mode disable */

#define SPIM_OP_ENABLE                      (0x01UL)            /* SPIM Operation Enable */
#define SPIM_OP_DISABLE                     (0x00UL)            /* SPIM Operation Disable */

/*---------------------------------------------------------------------------------------------------------*/
/* SPIM_CTL0 constant definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define SPIM_CTL0_RW_IN(x)                  ((x) ? 0UL : (0x1UL << SPIM_CTL0_QDIODIR_Pos))      /*!< SPIM_CTL0: SPI Interface Direction Select \hideinitializer */

#define SPIM_CTL0_BITMODE_SING              (0UL << SPIM_CTL0_BITMODE_Pos)          /*!< SPIM_CTL0: One bit mode (SPI Interface including DO, DI, HOLD, WP) \hideinitializer */
#define SPIM_CTL0_BITMODE_DUAL              (1UL << SPIM_CTL0_BITMODE_Pos)          /*!< SPIM_CTL0: Two bits mode (SPI Interface including D0, D1, HOLD, WP) \hideinitializer */
#define SPIM_CTL0_BITMODE_QUAD              (2UL << SPIM_CTL0_BITMODE_Pos)          /*!< SPIM_CTL0: Four bits mode (SPI Interface including D0, D1, D2, D3) \hideinitializer */
#define SPIM_CTL0_BITMODE_OCTAL             (3UL << SPIM_CTL0_BITMODE_Pos)          /*!< SPIM_CTL0: Four bits mode (SPI Interface including D0, D1, D2, D3, D4, D5, D6, D7) \hideinitializer */

#define SPIM_CTL0_OPMODE_IO                 (0UL)           /*!< SPIM_CTL0: I/O Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_PAGEWRITE          (1UL)           /*!< SPIM_CTL0: Page Write Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_PAGEREAD           (2UL)           /*!< SPIM_CTL0: Page Read Mode \hideinitializer */
#define SPIM_CTL0_OPMODE_DIRECTMAP          (3UL)           /*!< SPIM_CTL0: Direct Map Mode \hideinitializer */

#define SPIM_DWIDTH_8                       (8UL)   /*!< SPIM_CTL0: 8 bits are transmitted/received in one transmit/receive transaction. */
#define SPIM_DWIDTH_16                      (16UL)  /*!< SPIM_CTL0: 16 bits are transmitted/received in one transmit/receive transaction. */
#define SPIM_DWIDTH_24                      (24UL)  /*!< SPIM_CTL0: 24 bits are transmitted/received in one transmit/receive transaction. */
#define SPIM_DWIDTH_32                      (32UL)  /*!< SPIM_CTL0: 32 bits are transmitted/received in one transmit/receive transaction. */

#define SPIM_BURSTNUM_1                     (0UL)   /*!< SPIM_CTL0: One data TX/RX will be executed in one transfer */
#define SPIM_BURSTNUM_2                     (1UL)   /*!< SPIM_CTL0: Two data TX/RX will be executed in one transfer */
#define SPIM_BURSTNUM_3                     (2UL)   /*!< SPIM_CTL0: Three data TX/RX will be executed in one transfer */
#define SPIM_BURSTNUM_4                     (3UL)   /*!< SPIM_CTL0: Four data TX/RX will be executed in one transfer */

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAW, SPIM_PHDMAR, SPIM_PHDMM constant definitions                  */
/*----------------------------------------------------------------------------*/
#define PHASE_NORMAL_MODE                   (0x0)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Normal Mode for Command/Address/Data Phase. */
#define PHASE_DUAL_MODE                     (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Dual Mode for Command/Address/Data Phase. */
#define PHASE_QUAD_MODE                     (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Quad Mode for Command/Address/Data Phase. */
#define PHASE_OCTAL_MODE                    (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Octal Mode for Command/Address/Data Phase. */

#define PHASE_ORDER_MODE0                   (0x0)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7. */
#define PHASE_ORDER_MODE1                   (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0. */
#define PHASE_ORDER_MODE2                   (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6. */
#define PHASE_ORDER_MODE3                   (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set Byte order of program data to SPI Flash is byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1. */

#define PHASE_WIDTH_8                       (0x1)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 8 bits are transmitted in this phase. */
#define PHASE_WIDTH_16                      (0x2)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 16 bits are transmitted in this phase. */
#define PHASE_WIDTH_24                      (0x3)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 24 bits are transmitted in this phase. */
#define PHASE_WIDTH_32                      (0x4)   /*!< SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM: Set 32 bits are transmitted in this phase. */

/* SPIM_PHDMAR/SPIM_PHDMM read mode opcodes. */
#define PHASE_ENABLE_CONT_READ              (0x01)  /* Continue read mode enable */
#define PHASE_DISABLE_CONT_READM            (0x00)  /* Continue read mode disable */

/* SPIM_PHDMAW/SPIM_PHDMAR/SPIM_PHDMM DTR mode opcodes. */
#define PHASE_ENABLE_DTR                    (0x01)  /* Double data rate mode enable */
#define PHASE_DISABLE_DTR                   (0x00)  /* Double data rate mode disable */

#define PHASE_ENABLE_RDQS                   (0x01)
#define PHASE_DISABLE_RDQS                  (0x00)

/* Clear SPIM_PHDMAR/SPIM_PHDMM Phase Configure. */
#define PHASE_CLR_CMD_Pos                   (0)
#define PHASE_CLR_ADDR_Pos                  (8)
#define PHASE_CLR_READMODE_Pos              (16)
#define PHASE_CLR_DATA_Pos                  (24)

#define SPIM_IO_WRITE_PHASE                 (0x01)  /* SPIM IO write phase mode.  */
#define SPIM_IO_READ_PHASE                  (0x00)  /* SPIM IO read phase mode.  */

/* SPIM Auto Trim Hyper DLL */
#define SPIM_DISABLE_AUTO_TRIM              (0UL)
#define SPIM_AUTO_TRIM_ONCE                 (1UL)
#define SPIM_COND_AUTO_TRIM                 (2UL)

//------------------------------------------------------------------------------
// SPIM Transfer/Receive bit
//------------------------------------------------------------------------------
#define SPIM_BITMODE_1                      (1UL)
#define SPIM_BITMODE_2                      (2UL)
#define SPIM_BITMODE_4                      (4UL)
#define SPIM_BITMODE_8                      (8UL)

//------------------------------------------------------------------------------
// Continue Read Mode Command
//------------------------------------------------------------------------------
#define SPIM_DMM_DISABLE_CREN               (0x00UL)    /*!< SPIM_MODE: Set output data for continue read mode. */
#define SPIM_DMM_ENABLE_CREN                (0x20UL)    /*!< SPIM_MODE: Set output data for continue read mode. */

/*----------------------------------------------------------------------------*/
/* Flash Write Command                                                   */
/*----------------------------------------------------------------------------*/
#define CMD_NORMAL_PAGE_PROGRAM             (0x02UL)    /*!< SPIM_CTL0: Page Program (Page Write Mode Use) \hideinitializer */
#define CMD_NORMAL_PAGE_PROGRAM_4B          (0x12UL)    /*!< SPIM_CTL0: Page Program (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND       (0x32UL)    /*!< SPIM_CTL0: Quad Page program (for Winbond) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_WINBOND_4B    (0x34UL)    /*!< SPIM_CMDCODE: Quad Page program 4 Byte Address (for Winbond) (Page Write Mode Use) */
#define CMD_QUAD_PAGE_PROGRAM_MXIC          (0x38UL)    /*!< SPIM_CTL0: Quad Page program (for MXIC) (Page Write Mode Use) \hideinitializer */
#define CMD_QUAD_PAGE_PROGRAM_EON           (0x40UL)    /*!< SPIM_CTL0: Quad Page Program (for EON) (Page Write Mode Use) \hideinitializer */
#define CMD_OCTAL_PAGE_PROG_MICRON          (0x82UL)    /*!< SPIM_CMDCODE: Octal Page Program (Page Write Mode Use) */
#define CMD_OCTAL_PAGE_PROG_MICRON_4B       (0x84UL)    /*!< SPIM_CMDCODE: Octal Page Program 4 Byte Address (Page Write Mode Use) */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON       (0xC2UL)    /*!< SPIM_CMDCODE: Octal Page Extern Program (Page Write Mode Use) */
#define CMD_OCTAL_EX_PAGE_PROG_MICRON_4B    (0x8EUL)    /*!< SPIM_CMDCODE: Octal Page Extern Program 4 Byte Address (Page Write Mode Use) */

//------------------------------------------------------------------------------
// SPI Flash Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_NORMAL_READ                 (0x03UL)    /*!< SPIM_CMDCODE: Read Data (Page Read Mode Use) */
#define CMD_DMA_NORMAL_READ_4B              (0x13UL)    /*!< SPIM_CMDCODE: Read Data 4 Byte Address( Page Read Mode Use) */
#define CMD_DMA_FAST_READ                   (0x0BUL)    /*!< SPIM_CMDCODE: Fast Read (Page Read Mode Use) */
#define CMD_DMA_FAST_READ_4B                (0x0CUL)    /*!< SPIM_CMDCODE: Fast Read 4 Byte Address (Page Read Mode Use) */
#define CMD_DMA_NORMAL_DTR_READ             (0x0DUL)    /*!< SPIM_CMDCODE: Fast DTR Read (Page Read Mode Use) */

//------------------------------------------------------------------------------
// Dual Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_NORMAL_DUAL_READ            (0x3BUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) */
#define CMD_DMA_NORMAL_DUAL_READ_4B         (0x3CUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) */
#define CMD_DMA_FAST_DUAL_READ              (0xBBUL)    /*!< SPIM_CMDCODE: Fast Read Dual I/O (Page Read Mode Use) */
#define CMD_DMA_FAST_DUAL_READ_4B           (0xBCUL)    /*!< SPIM_CMDCODE: Fast Read Dual I/O 4 Byte Address (Page Read Mode Use) */
#define CMD_DMA_FAST_DUAL_DTR_READ          (0xBDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Dual I/O (Page Read Mode Use) */

//------------------------------------------------------------------------------
// Quad Read Command
//------------------------------------------------------------------------------
#define CMD_DMA_FAST_READ_QUAD_OUTPUT       (0x6BUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output (Page Read Mode Use) */
#define CMD_DMA_FAST_READ_QUAD_OUTPUT_4B    (0x6CUL)    /*!< SPIM_CMDCODE: Fast Read Dual Output 4 Byte Address (Page Read Mode Use) */
#define CMD_DMA_NORMAL_QUAD_READ            (0xE7UL)    /*!< SPIM_CTL0: Fast Read Quad I/O (Page Read Mode Use) */
#define CMD_DMA_FAST_QUAD_READ              (0xEBUL)    /*!< SPIM_CMDCODE: Fast Read Quad I/O (Page Read Mode Use) */
#define CMD_DMA_FAST_QUAD_READ_4B           (0xECUL)    /*!< SPIM_CMDCODE: Fast Read Quad I/O 4 Byte Address (Page Read Mode Use) */
#define CMD_DMA_FAST_QUAD_DTR_READ          (0xEDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Quad I/O (Page Read Mode Use) */

//------------------------------------------------------------------------------
// Micron Octal Read Command
//------------------------------------------------------------------------------
#define CMD_OCTAL_FAST_READ_OUTPUT          (0x8BUL)    /*!< SPIM_CMDCODE: Fast Read Octal Output (Page Read Mode Use) */
#define CMD_OCTAL_FAST_READ_OUTPUT_4B       (0x7CUL)    /*!< SPIM_CMDCODE: Fast Read Octal Output 4 Byte Address (Page Read Mode Use) */
#define CMD_OCTAL_FAST_IO_READ              (0xCBUL)    /*!< SPIM_CMDCODE: Fast Read Octal I/O (Page Read Mode Use) */
#define CMD_OCTAL_FAST_IO_READ_4B           (0xCCUL)    /*!< SPIM_CMDCODE: Fast Read Octal I/O 4 Byte Address (Page Read Mode Use) */
#define CMD_OCTAL_DDR_FAST_READ_OUTPUT      (0x9DUL)    /*!< SPIM_CMDCODE: DTR Fast Read Octal Output (Page Read Mode Use) */
#define CMD_OCTAL_DDR_FAST_IO_READ          (0xFDUL)    /*!< SPIM_CMDCODE: DTR Fast Read Octal I/O (Page Read Mode Use) */

/** @cond HIDDEN_SYMBOLS */

typedef enum
{
    MFGID_UNKNOW    = 0x00U,
    MFGID_SPANSION  = 0x01U,
    MFGID_EON       = 0x1CU,
    MFGID_ISSI      = 0x7FU,
    MFGID_MXIC      = 0xC2U,
    MFGID_WINBOND   = 0xEFU,
    MFGID_MICRON    = 0x2CU,
}
E_MFGID;

/* Flash opcodes. */
#define OPCODE_WREN                         (0x06U)   /* Write enable */
#define OPCODE_RDSR                         (0x05U)   /* Read status register #1*/
#define OPCODE_WRSR                         (0x01U)   /* Write status register #1 */
#define OPCODE_RDSR2                        (0x35U)   /* Read status register #2*/
#define OPCODE_WRSR2                        (0x31U)   /* Write status register #2 */
#define OPCODE_RDSR3                        (0x15U)   /* Read status register #3*/
#define OPCODE_WRSR3                        (0x11U)   /* Write status register #3 */
#define OPCODE_PP                           (0x02U)   /* Page program (up to 256 bytes) */
#define OPCODE_SE_4K                        (0x20U)   /* Erase 4KB sector */
#define OPCODE_BE_32K                       (0x52U)   /* Erase 32KB block */
#define OPCODE_CHIP_ERASE                   (0xC7U)   /* Erase whole flash chip */
#define OPCODE_BE_64K                       (0xD8U)   /* Erase 64KB block */
#define OPCODE_READ_ID                      (0x90U)   /* Read ID */
#define OPCODE_RDID                         (0x9fU)   /* Read JEDEC ID */
#define OPCODE_BRRD                         (0x16U)   /* SPANSION flash - Bank Register Read command  */
#define OPCODE_BRWR                         (0x17U)   /* SPANSION flash - Bank Register write command */
#define OPCODE_NORM_READ                    (0x03U)   /* Read data bytes */
#define OPCODE_FAST_READ                    (0x0BU)   /* Read data bytes */
#define OPCODE_FAST_DUAL_READ               (0x3BU)   /* Read data bytes */
#define OPCODE_FAST_QUAD_READ               (0x6BU)   /* Read data bytes */

/* Micron MT35xU02 octal SPI flash configure register OP code */
#define OPCODE_WR_NVCONFIG                  (0xB1U) /* Write Non-Volatile Configuration Register*/
#define OPCODE_WR_VCONFIG                   (0x81U) /* Write Volatile Configuration Register*/
#define OPCODE_RD_NVCONFIG                  (0xB5U) /* Read Non-Volatile Configuration Register*/
#define OPCODE_RD_VCONFIG                   (0x85U) /* Read Volatile Configuration Register*/
#define OPCODE_RD_FLGREG                    (0x70U) /* Read Volatile Configuration Register*/

/* Used for SST flashes only. */
#define OPCODE_BP                           (0x02U)   /* Byte program */
#define OPCODE_WRDI                         (0x04U)   /* Write disable */
#define OPCODE_AAI_WP                       (0xadU)   /* Auto u32Address increment word program */

/* Used for Macronix flashes only. */
#define OPCODE_EN4B                         (0xb7U)   /* Enter 4-byte mode */
#define OPCODE_EX4B                         (0xe9U)   /* Exit 4-byte mode */

#define OPCODE_RDSCUR                       (0x2bU)
#define OPCODE_WRSCUR                       (0x2fU)

#define OPCODE_RSTEN                        (0x66U)
#define OPCODE_RST                          (0x99U)

#define OPCODE_ENQPI                        (0x38U)
#define OPCODE_EXQPI                        (0xFFU)

/* Status Register bits. */
#define SR_WIP                              (0x1U)    /* Write in progress */
#define SR_WEL                              (0x2U)    /* Write enable latch */
#define SR_QE                               (0x40U)   /* Quad Enable for MXIC */
/* Status Register #2 bits. */
#define SR2_QE                              (0x2U)    /* Quad Enable for Winbond */
/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0                              (0x4U)    /* Block protect 0 */
#define SR_BP1                              (0x8U)    /* Block protect 1 */
#define SR_BP2                              (0x10U)   /* Block protect 2 */
#define SR_SRWD                             (0x80U)   /* SR write protect */
#define SR3_ADR                             (0x01U)   /* 4-byte u32Address mode */

#define SCUR_4BYTE                          (0x04U)   /* 4-byte u32Address mode */

/** @endcond HIDDEN_SYMBOLS */

/* SPIM Define Error Code */
#define SPIM_TIMEOUT                        SystemCoreClock     /*!< SPIM time-out counter (1 second time-out) */
#define SPIM_OK                             ( 0L)               /*!< SPIM operation OK */
#define SPIM_ERR_FAIL                       (-1L)               /*!< SPIM operation failed */
#define SPIM_ERR_TIMEOUT                    (-2L)               /*!< SPIM operation abort due to timeout error */

/* SPIM define flash page size */
#define SPIM_FLH_PAGE_SIZE                  (256UL) /* SPI Flash Write Page Size */

/*@}*/ /* end of group SPIM_EXPORTED_CONSTANTS */

/** @addtogroup SPIM_EXPORTED_FUNCTIONS SPIM Exported Functions
  @{
*/


/*---------------------------------------------------------------------------------------------------------*/
/*  Define Macros and functions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
/**
  * @brief       Enable SPI Flash Mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_SET_FLASH_MODE(spim)   (spim->CTL0 &= ~(SPIM_CTL0_DEVMODE_Msk))

/**
 * @brief   Set 4-byte address.
 * @param[in]   spim
 * @param[in]   x   Enable/Disable 4 bytes address.
 *                  - \ref SPIM_OP_ENABLE
 *                  - \ref SPIM_OP_DISABLE
 * \hideinitializer
 */
#define SPIM_SET_4BYTE_ADDR(spim, x)                             \
    do                                                              \
    {                                                               \
        spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_B4ADDREN_Msk)) |     \
                     (((x) ? 1UL : 0UL) << SPIM_CTL0_B4ADDREN_Pos); \
    } while (0)

/**
 * @details    Enable SPIM interrupt
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_INT(spim)   (spim->CTL0 |= SPIM_CTL0_IEN_Msk)

/**
 * @details    Disable SPIM interrupt
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_INT(spim)  (spim->CTL0 &= ~SPIM_CTL0_IEN_Msk)

/**
 * @details    Is interrupt flag on.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_IS_IF_ON(spim)     ((spim->CTL0 & SPIM_CTL0_IF_Msk) != 0UL)

/**
 * @details    Clear interrupt flag.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_CLR_INT(spim)                                                           \
    do                                                                               \
    {                                                                                \
        spim->CTL0 = (spim->CTL0 & (~SPIM_CTL0_IF_Msk)) | (1UL << SPIM_CTL0_IF_Pos); \
    } while (0)

/**
 * @details     Set transmit/receive bit length
 * @param[in]   spim
 * @param[in]   x   Transmit/Receive Bit Length
 *                  - \ref SPIM_DWIDTH_8  : 8 bits
 *                  - \ref SPIM_DWIDTH_16 : 16 bits
 *                  - \ref SPIM_DWIDTH_24 : 24 bits
 *                  - \ref SPIM_DWIDTH_32 : 32 bits
 * \hideinitializer
 */
#define SPIM_SET_DATA_WIDTH(spim, x)                          \
    do                                                        \
    {                                                         \
        spim->CTL0 = (spim->CTL0 & (~SPIM_CTL0_DWIDTH_Msk)) | \
                     (((x) - 1U) << SPIM_CTL0_DWIDTH_Pos);    \
    } while (0)

/**
 * @details    Get data transmit/receive bit length setting
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_DATA_WIDTH(spim)   \
    (((spim->CTL0 & SPIM_CTL0_DWIDTH_Msk) >> SPIM_CTL0_DWIDTH_Pos) + 1U)

/**
 * @brief   Set data transmit/receive burst number.
 * @param[in]   spim
 * @param[in]   x   Transmit/Receive Burst Number.
 *                  - \ref SPIM_BURSTNUM_1 : one data TX/RX will be executed in one transfer
 *                  - \ref SPIM_BURSTNUM_2 : two data TX/RX will be executed in one transfer
 *                  - \ref SPIM_BURSTNUM_3 : three data TX/RX will be executed in one transfer
 *                  - \ref SPIM_BURSTNUM_4 : four data TX/RX will be executed in one transfer
 * \hideinitializer
 */
#define SPIM_SET_BURST_DATA(spim, x)    \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_BURSTNUM_Msk)) | \
                  ((x - 1UL) << SPIM_CTL0_BURSTNUM_Pos))

/**
 * @details     Get data transmit/receive burst number
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_DATA_NUM(spim) \
    (((spim->CTL0 & SPIM_CTL0_BURSTNUM_Msk) >> SPIM_CTL0_BURSTNUM_Pos) + 1UL)

/**
 * @details     Enable Single Input mode.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_SING_INPUT_MODE(spim)                                                \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_SING | SPIM_CTL0_RW_IN(1UL));                    \
    } while (0)

/**
 * @details     Enable Single Output mode.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_SING_OUTPUT_MODE(spim)                                               \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_SING | SPIM_CTL0_RW_IN(0UL));                    \
    } while (0)

/**
 * @details    Enable Dual Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_DUAL_INPUT_MODE(spim)                                                \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_DUAL | SPIM_CTL0_RW_IN(1UL));                    \
    } while (0)

/**
 * @details    Enable Dual Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_DUAL_OUTPUT_MODE(spim)                                               \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_DUAL | SPIM_CTL0_RW_IN(0UL));                    \
    } while (0)

/**
 * @details    Enable Quad Input mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_QUAD_INPUT_MODE(spim)                                                \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_QUAD | SPIM_CTL0_RW_IN(1UL));                    \
    } while (0)

/**
 * @details    Enable Quad Output mode.
 * \hideinitializer
 */
#define SPIM_ENABLE_QUAD_OUTPUT_MODE(spim)                                               \
    do                                                                                   \
    {                                                                                    \
        spim->CTL0 = (spim->CTL0 & (~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk))) | \
                     (SPIM_CTL0_BITMODE_QUAD | SPIM_CTL0_RW_IN(0UL));                    \
    } while (0)

/**
  * @brief   Enable Octal Input mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_ENABLE_OCTAL_INPUT_MODE(spim)                                              \
    do                                                                                  \
    {                                                                                   \
        (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | \
                      (SPIM_CTL0_BITMODE_OCTAL | SPIM_CTL0_RW_IN(1UL)));                \
    } while (0)

/**
  * @brief  Enable Octal Output mode.
  * @param[in]  spim
  * \hideinitializer
  */
#define SPIM_ENABLE_OCTAL_OUTPUT_MODE(spim)                                             \
    do                                                                                  \
    {                                                                                   \
        (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_BITMODE_Msk | SPIM_CTL0_QDIODIR_Msk)) | \
                      (SPIM_CTL0_BITMODE_OCTAL | SPIM_CTL0_RW_IN(0UL)));                \
    } while (0)

/**
 * @details    Set suspend interval which ranges between 0 and 15.
 * \hideinitializer
 */
#define SPIM_SET_SUSP_INTVL(spim, x)                           \
    do                                                         \
    {                                                          \
        spim->CTL0 = (spim->CTL0 & (~SPIM_CTL0_SUSPITV_Msk)) | \
                     ((x) << SPIM_CTL0_SUSPITV_Pos);           \
    } while (0)

/**
 * @details    Get suspend interval setting
 * \hideinitializer
 */
#define SPIM_GET_SUSP_INTVL(spim) \
    ((spim->CTL0 & SPIM_CTL0_SUSPITV_Msk) >> SPIM_CTL0_SUSPITV_Pos)

/**
 * @details    Get operation mode.
 * \hideinitializer
 */
#define SPIM_GET_OP_MODE(spim)  (spim->CTL0 & SPIM_CTL0_OPMODE_Msk)

/**
 * @brief   Set DTR(Data Transfer Rate) mode.
 * @param[in]   spim
 * @param[in]   x   Double Transfer Rate Mode Enable Bit for Normal I/O Mode
 *                  - \ref SPIM_OP_ENABLE
 *                  - \ref SPIM_OP_DISABLE
 * \hideinitializer
 */
#define SPIM_SET_DTR_MODE(spim, x)  \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_DTR_NORM_Msk) | \
                   (((x) ? 1UL : 0UL) << SPIM_CTL0_DTR_NORM_Pos)))

/**
 * @brief   Get DTR(Data Transfer Rate) mode to be enabled/disabled.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_DTR_MODE(spim) \
    ((spim->CTL0 & SPIM_CTL0_DTR_NORM_Msk) >> SPIM_CTL0_DTR_NORM_Pos)

/**
 * @brief   Set Read DQS Mode.
 * @param[in]   spim
 * @param[in]   x   Read DQS Mode Enable Bit for Normal I/O Mode
 *                  - \ref SPIM_OP_ENABLE
 *                  - \ref SPIM_OP_DISABLE
 * \hideinitializer
 */
#define SPIM_SET_RDQS_MODE(spim, x) \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_RDQS_NORM_Msk)) | \
                  (((x) ? 1UL : 0UL) << SPIM_CTL0_RDQS_NORM_Pos))

/**
 * @brief   Get Read DQS Mode to be enabled/disabled.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_RDQS_MODE(spim)    \
    ((spim->CTL0 & SPIM_CTL0_RDQS_NORM_Msk) >> SPIM_CTL0_RDQS_NORM_Pos)

/**
 * @brief   Set Read Data Byte Order Mode.
 * @param[in]   spim
 * @param[in]   x   Received Data Byte Order of Normal I/O Mode for Octal SPI Flash
 *                  width 8bits
 *                  - \ref SPIM_CTL0_RBONUM_0 : received data from SPI Flash is byte0.
 *                  width 16bits
 *                  - \ref SPIM_CTL0_RBONUM_0 : received data from SPI Flash is byte0, byte1.
 *                  - \ref SPIM_CTL0_RBONUM_1 : received data from SPI Flash is byte1, byte0.
 *                  width 24bits
 *                  - \ref SPIM_CTL0_RBONUM_0 : received data from SPI Flash is byte0, byte1, byte2.
 *                  - \ref SPIM_CTL0_RBONUM_1 : received data from SPI Flash is byte2, byte1, byte0.
 *                  width 32bits
 *                  - \ref SPIM_CTL0_RBONUM_0 : received data from SPI Flash is byte0, byte1, byte2, byte3.
 *                  - \ref SPIM_CTL0_RBONUM_1 : received data from SPI Flash is byte3, byte2, byte1, byte0.
 *                  - \ref SPIM_CTL0_RBONUM_2 : received data from SPI Flash is byte1, byte0, byte3, byte2.
 *                  - \ref SPIM_CTL0_RBONUM_3 : received data from SPI Flash is byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_RBO_MODE(spim, x)  \
    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_RBO_NORM_Msk)) | (x << SPIM_CTL0_RBO_NORM_Pos))

/**
 * @brief   Get Read Data Byte Order Mode.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_RBO_MODE(spim) \
    ((spim->CTL0 & SPIM_CTL0_RBO_NORM_Msk) >> SPIM_CTL0_RBO_NORM_Pos)

/**
 * @brief   Set SPI flash R/W commmand code.
 * @param[in]   spim
 * @param[in]   x   reference SPI Flash Specification.
 * \hideinitializer
 */
#define SPIM_SET_CMD_CODE(spim, x) \
    (spim->CMDCODE = (spim->CMDCODE & ~(SPIM_CMDCODE_CMDCODE_Msk)) | (x))

/**
 * @details    Start operation.
 * \hideinitializer
 */
#define SPIM_SET_GO(spim)   (spim->CTL1 |= SPIM_CTL1_SPIMEN_Msk)

/**
 * @details    Is engine busy.
 * \hideinitializer
 */
#define SPIM_IS_BUSY(spim)  (spim->CTL1 & SPIM_CTL1_SPIMEN_Msk)

/**
 * @details    Wait for free.
 * \hideinitializer
 */
//#define SPIM_WAIT_FREE(spim)    while (spim->CTL1 & SPIM_CTL1_SPIMEN_Msk)

/**
 * @details    Is cache enabled.
 * \hideinitializer
 */
#define SPIM_IS_CACHE_EN(spim)  ((spim->CTL1 & SPIM_CTL1_CACHEOFF_Msk) ? 0 : 1)

/**
 * @details    Invalidate cache.
 * \hideinitializer
 */
#define SPIM_INVALID_CACHE(spim)    (spim->CTL1 |= SPIM_CTL1_CDINVAL_Msk)

/**
 * @details    Set SS(Select Active) to active level.
 * \hideinitializer
 */
#define SPIM_SET_SS_EN(spim, x)                                  \
    do                                                           \
    {                                                            \
        (spim->CTL1 = (spim->CTL1 & ~(SPIM_CTL1_SS_Msk)) |       \
                      ((!(x) ? 1UL : 0UL) << SPIM_CTL1_SS_Pos)); \
    } while (0)

/**
 * @details    Is SS(Select Active) in active level.
 * \hideinitializer
 */
#define SPIM_GET_SS_EN(spim)    (!(spim->CTL1 & SPIM_CTL1_SS_Msk))

/**
 * @brief   Set active level of slave select to be high/low.
 * @param[in]   spim
 * @param[in]   x   Slave Select Active Enable Bit.
 *                  - \ref SPIM_OP_DISABLE : active level
 *                  - \ref SPIM_OP_ENABLE  : inactive level.
 * \hideinitializer
 */
#define SPIM_SET_SS_ACTLVL(spim, x)                                     \
    do                                                                  \
    {                                                                   \
        (spim->CTL1 = (spim->CTL1 & ~(SPIM_CTL1_SSACTPOL_Msk)) |        \
                      ((!!(x) ? 1UL : 0UL) << SPIM_CTL1_SSACTPOL_Pos)); \
    } while (0)

/**
 * @brief   Set idle time interval.
 * @param[in]   spim
 * @param[in]   x   Idle Time Interval. It could be 0 ~ 0xFFFF.
 * \hideinitializer
 */
#define SPIM_SET_IDL_INTVL(spim, x)                             \
    do                                                          \
    {                                                           \
        spim->CTL1 = (spim->CTL1 & ~(SPIM_CTL1_IDLETIME_Msk)) | \
                     ((x) << SPIM_CTL1_IDLETIME_Pos);           \
    } while (0)

/**
 * @brief   Get idle time interval setting.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_IDL_INTVL(spim) \
    ((spim->CTL1 & SPIM_CTL1_IDLETIME_Msk) >> SPIM_CTL1_IDLETIME_Pos)

/**
 * @brief   Set SPIM clock divider.
 * @param[in]   spim
 * @param[in]   x   Clock Divider Register
 *                  SPI Flash For DTR commands 1, 2, 4, 8, 16, 32,??
 *                  Hyper Device Mode only 1 or 2
 * \hideinitializer
 */
#define SPIM_SET_CLOCK_DIVIDER(spim, x)                        \
    do                                                         \
    {                                                          \
        spim->CTL1 = (spim->CTL1 & ~(SPIM_CTL1_DIVIDER_Msk)) | \
                     ((x) << SPIM_CTL1_DIVIDER_Pos);           \
    } while (0)

/**
 * @brief   Get SPIM current clock divider setting.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_CLOCK_DIVIDER(spim)   \
    ((spim->CTL1 & SPIM_CTL1_DIVIDER_Msk) >> SPIM_CTL1_DIVIDER_Pos)

/**
 * @brief   Set sampling clock delay selection for received data, It could be 0 ~ 0xF.
 * @param[in]   spim
 * @param[in]   x   Sampling Clock Delay Selection for Received Data for
 *                  Normal I/O Mode, DMA Write Mode, and Direct Memory Mapping Mode Only.
 *                  It could be 0 ~ 0xF.
 * \hideinitializer
 */
#define SPIM_SET_RXCLKDLY_RDDLYSEL(spim, x)                                  \
    do                                                                       \
    {                                                                        \
        (spim->RXCLKDLY = (spim->RXCLKDLY & ~(SPIM_RXCLKDLY_RDDLYSEL_Msk)) | \
                          ((x) << SPIM_RXCLKDLY_RDDLYSEL_Pos));              \
    } while (0)

/**
 * @brief   Get sampling clock delay selection for received data.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_RXCLKDLY_RDDLYSEL(spim) \
    ((spim->RXCLKDLY & SPIM_RXCLKDLY_RDDLYSEL_Msk) >> SPIM_RXCLKDLY_RDDLYSEL_Pos)

/**
  * @brief   Set DMA/DMM mode SPI flash active SCLK time. It could be 0 ~ 0xFF.
  * @param[in]   spim
  * @param[in]   x   SPI Flash Active SCLK Time for Direct Memory Mapping Mode,
  *                  and DMA Write Mode Only. It could be 0x0 ~ 0xFF.
  * \hideinitializer
  */
#define SPIM_SET_DMADMM_ACTSCLKT(spim, x)  \
    (spim->DMMCTL = (spim->DMMCTL & ~SPIM_DMMCTL_ACTSCLKT_Msk) | \
                    ((((x) & 0xFUL) << SPIM_DMMCTL_ACTSCLKT_Pos) | SPIM_DMMCTL_UACTSCLK_Msk))

/**
  * @brief   Reset SPI flash active SCLK time.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_RESET_DMADMM_ACTSCLK(spim)    (spim->DMMCTL &= ~(SPIM_DMMCTL_UACTSCLK_Msk))

/**
  * @brief   Set DMM mode SPI flash deselect time. It could be 0 ~ 0xFF.
  * @param[in]   spim
  * @param[in]   x   SPI Flash Deselect Time for Direct Memory Mapping Mode Only.
  *                  It could be 0x0 ~ 0xFF
  * \hideinitializer
  */
#define SPIM_SET_DMM_DESELTIM(spim, x)  \
    (spim->DMMCTL = (spim->DMMCTL & ~SPIM_DMMCTL_DESELTIM_Msk) | \
                    (((x)&0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos))

/**
  * @brief   Get current DMM mode SPI flash deselect time setting.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DMM_DESELTIM(spim) \
    ((spim->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos)

/**
  * @brief   Enable DMM burst wrap mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_ENABLE_DMM_BWEN(spim)  (spim->DMMCTL |= SPIM_DMMCTL_BWEN_Msk)

/**
  * @brief   Disable DMM burst wrap mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_DISABLE_DMM_BWEN(spim) (spim->DMMCTL &= ~(SPIM_DMMCTL_BWEN_Msk))

/**
  * @brief   Get DMM burst wrap mode
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DMM_BWEN(spim) \
    ((spim->DMMCTL & SPIM_DMMCTL_BWEN_Msk) >> SPIM_DMMCTL_BWEN_Pos)

/**
  * @brief   Enable DMM mode continuous read mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_ENABLE_DMM_CREN(spim)  (spim->DMMCTL |= SPIM_DMMCTL_CREN_Msk)

/**
  * @brief   Disable DMM mode continuous read mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_DISABLE_DMM_CREN(spim) (spim->DMMCTL &= ~(SPIM_DMMCTL_CREN_Msk))

/**
  * @brief   Get DMM idle state.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DMM_IDLE(spim) ((spim->DMMCTL & SPIM_DMMCTL_DMMIDLE_Msk) >> SPIM_DMMCTL_DMMIDLE_Pos)

/**
  * @brief   Set DMM time-out counter.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_SET_DMM_TOCNTDMM(spim, x)  \
    (spim->DMM_TIMEOUT_INTERVAL = (spim->DMM_TIMEOUT_INTERVAL & ~(SPIM_DMM_TIMEOUT_INTERVAL_TOCNTDMM_Msk)) | \
                                  ((x) <<SPIM_DMM_TIMEOUT_INTERVAL_TOCNTDMM_Pos))

/**
  * @brief   Get DMM time-out state flag.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DMM_TIMEOUT_STS(spim)   \
    ((spim->DMM_TIMEOUT_FLAG_STS & SPIM_DMM_TIMEOUT_FLAG_STS_DMMTOF_Msk) >> SPIM_DMM_TIMEOUT_FLAG_STS_DMMTOF_Pos)

/**
  * @brief   Clear DMM time-out state flag.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_CLR_DMM_TIMEOUT_STS(spim) \
    (spim->DMM_TIMEOUT_FLAG_STS |= SPIM_DMM_TIMEOUT_FLAG_STS_DMMTOF_Msk)

/**
  * @brief   Set dummy cycle number (Only DMA Command Mode). It could be 0 ~ 0xFF.
  * @param[in]   spim
  * @param[in]   x   Dummy Cycle Number for Direct Memory Mapping Mode Only.
  *                  It could be 0x0 ~ 0xFF.
  * \hideinitializer
  */
#define SPIM_SET_DMAR_DC(spim, x)   \
    (spim->CTL2 = (spim->CTL2 & ~(SPIM_CTL2_DC_DMAR_Msk)) | \
                  (((x) & 0xFFUL) << SPIM_CTL2_DC_DMAR_Pos))

/**
  * @brief   Clear dummy cycle number (Only DMA Command Mode).
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_CLEAR_DMAR_DC(spim)    (spim->CTL2 &= ~(SPIM_CTL2_DC_DMAR_Msk))

/**
  * @brief   Set dummy cycle number.
  * @param[in]   spim
  * @param[in]   x   dummy cycle number (Only DMM Command Mode).
  *                  It could be 0 ~ 0xFF.
  * \hideinitializer
  */
#define SPIM_SET_DMM_DC(spim, x)    \
    (spim->CTL2 = (spim->CTL2 & ~(SPIM_CTL2_DC_DMM_Msk)) | \
                  (((x) & 0xFFUL) << SPIM_CTL2_DC_DMM_Pos))

/**
  * @brief   Clear dummy cycle number (Only DMA Command Mode).
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_CLEAR_DMM_DC(spim) (spim->CTL2 &= ~(SPIM_CTL2_DC_DMM_Msk))

/**
  * @brief   Set output data for mode phase.
  * @param[in]   spim
  * @param[in]   x   SPI Flash Continue Read Mode.
  *                  - \ref SPIM_DMM_DISABLE_CREN
  *                  - \ref SPIM_DMM_ENABLE_CREN
  * \hideinitializer
  */
#define SPIM_SET_MODE_DATA(spim, x) \
    (spim->MODE = ((spim->MODE & (~SPIM_MODE_MODEDATA_Msk)) | (x)))

/**
  * @brief   Get Read Data Mode.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_MODE_DATA(spim) \
    ((spim->MODE & SPIM_MODE_MODEDATA_Msk) >> SPIM_MODE_MODEDATA_Pos)

/**
  * @brief   Clear output data for mode phase.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_CLEAR_MODE_DATA(spim)  (spim->MODE &= ~(SPIM_MODE_MODEDATA_Msk))

/**
 * @details    Set sampling clock edge selection for received data
 * \hideinitializer
 */
//#define SPIM_SET_RXCLKDLY_RDEDGE(spim) \
//    (spim->RXCLKDLY |= SPIM_RXCLKDLY_RDEDGE_Msk); \

/**
 * @details    Get sampling clock edge selection for received data
 * \hideinitializer
 */
//#define SPIM_CLR_RXCLKDLY_RDEDGE(spim) \
//    (spim->RXCLKDLY &= ~SPIM_RXCLKDLY_RDEDGE_Msk)

/**
 * @details    Set mode bits data for continuous read mode
 * \hideinitializer
 */
//#define SPIM_SET_DMMCTL_CRMDAT(spim, x)                                          \
//    do                                                                           \
//    {                                                                            \
//        (spim->DMMCTL = (spim->DMMCTL & (~SPIM_DMMCTL_CRMDAT_Msk)) |             \
//                        ((x) << SPIM_DMMCTL_CRMDAT_Pos) | SPIM_DMMCTL_CREN_Msk); \
//    } while (0)

/**
 * @details    Get mode bits data for continuous read mode
 * \hideinitializer
 */
//#define SPIM_GET_DMMCTL_CRMDAT(spim) \
//    ((spim->DMMCTL & SPIM_DMMCTL_CRMDAT_Msk) >> SPIM_DMMCTL_CRMDAT_Pos)

/**
 * @details    Set DMM mode SPI flash deselect time
 * \hideinitializer
 */
//#define SPIM_DMM_SET_DESELTIM(spim, x)                               \
//    do                                                               \
//    {                                                                \
//        spim->DMMCTL = (spim->DMMCTL & ~SPIM_DMMCTL_DESELTIM_Msk) |  \
//                       (((x) & 0x1FUL) << SPIM_DMMCTL_DESELTIM_Pos); \
//    } while (0)

/**
 * @details    Get current DMM mode SPI flash deselect time setting
 * \hideinitializer
 */
//#define SPIM_DMM_GET_DESELTIM(spim) \
//    ((spim->DMMCTL & SPIM_DMMCTL_DESELTIM_Msk) >> SPIM_DMMCTL_DESELTIM_Pos)

/**
 * @details    Enable DMM mode burst wrap mode
 * \hideinitializer
 */
//#define SPIM_DMM_ENABLE_BWEN(spim)  (spim->DMMCTL |= SPIM_DMMCTL_BWEN_Msk)

/**
 * @details    Disable DMM mode burst wrap mode
 * \hideinitializer
 */
//#define SPIM_DMM_DISABLE_BWEN(spim) (spim->DMMCTL &= ~SPIM_DMMCTL_BWEN_Msk)

/**
 * @details    Enable DMM mode continuous read mode
 * \hideinitializer
 */
//#define SPIM_DMM_ENABLE_CREN(spim)  (spim->DMMCTL |= SPIM_DMMCTL_CREN_Msk)

/**
 * @details    Disable DMM mode continuous read mode
 * \hideinitializer
 */
//#define SPIM_DMM_DISABLE_CREN(spim) (spim->DMMCTL &= ~SPIM_DMMCTL_CREN_Msk)

/**
 * @details    Set DMM mode SPI flash active SCLK time
 * \hideinitializer
 */
//#define SPIM_DMM_SET_ACTSCLKT(spim, x)                               \
//    do                                                               \
//    {                                                                \
//        spim->DMMCTL = (spim->DMMCTL & ~SPIM_DMMCTL_ACTSCLKT_Msk)  | \
//                       (((x) & 0xFUL) << SPIM_DMMCTL_ACTSCLKT_Pos) | \
//                       SPIM_DMMCTL_UACTSCLK_Msk;                     \
//    } while (0)

/**
 * @details    Set SPI flash active SCLK time as SPIM default
 * \hideinitializer
 */
//#define SPIM_DMM_SET_DEFAULT_ACTSCLK(spim)  (spim->DMMCTL &= ~SPIM_DMMCTL_UACTSCLK_Msk)

/**
 * @details    Set dummy cycle number (Only for DMM mode and DMA mode)
 * \hideinitializer
 */
#define SPIM_SET_DCNUM(spim, x)                                \
    do                                                         \
    {                                                          \
        spim->CTL2 = (spim->CTL2 & ~SPIM_CTL2_DCNUM_Msk) |     \
                     (((x) & 0x1FUL) << SPIM_CTL2_DCNUM_Pos) | \
                     SPIM_CTL2_USETEN_Msk;                     \
    } while (0)

/**
 * @details    Set dummy cycle number (Only for DMM mode and DMA mode) as SPIM default
 * \hideinitializer
 */
#define SPIM_SET_DEFAULT_DCNUM(spim, x) (spim->CTL2 &= ~SPIM_CTL2_USETEN_Msk)

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAW constant definitions                                           */
/*----------------------------------------------------------------------------*/
/**
 * @brief   Clear Phase DMA Write Setting.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_CLEAR_PHDMAW(spim) (spim->PHDMAW &= ~(0xFFFFFFFF))

/**
 * @brief   Set Write Data Width for Command Phase.
 * @param[in]   spim
 * @param[in]   x   Command Phase Witdh
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_CMD_WIDTH(spim, x)  \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_DW_CMD_Msk)) | \
                    (x << SPIM_PHDMAW_DW_CMD_Pos))

/**
 * @brief   Get Write Data Width for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_CMD_WIDTH(spim) \
    ((spim->PHDMAW & SPIM_PHDMAW_DW_CMD_Msk) >> SPIM_PHDMAW_DW_CMD_Pos)

/**
 * @brief   Set DTR Mode Enable Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_CMD_DTR(spim)    (spim->PHDMAW |= SPIM_PHDMAW_DTR_CMD_Msk)

/**
 * @brief   Set DTR Mode Disable Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_CMD_DTR(spim)   (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_CMD_Msk))

/**
 * @brief   Get DTR Mode Disable Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_CMD_DTR(spim)   \
    ((spim->PHDMAW & SPIM_PHDMAW_DTR_CMD_Msk) >> SPIM_PHDMAW_DTR_CMD_Pos)

/**
 * @brief   Set Bit Mode for Command Phase.
 * @param[in]   spim
 * @param[in]   x   Command Phase Bits Mode.
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_CMD_BITMODE(spim, x)   \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_BM_CMD_Msk)) | \
                    (x << SPIM_PHDMAW_BM_CMD_Pos))

/**
 * @brief   Get Bit Mode for Command Phase
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_CMD_BITMODE(spim) \
    ((spim->PHDMAW & SPIM_PHDMAW_BM_CMD_Msk) >> SPIM_PHDMAW_BM_CMD_Pos)

/**
 * @brief   Set Data Width for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Address Phase Witdh.
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_ADDR_WIDTH(spim, x) \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_DW_ADDR_Msk)) | \
                    (x << SPIM_PHDMAW_DW_ADDR_Pos))

/**
 * @brief   Get Data Width for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAW_ADDR_WIDTH(spim) \
    ((spim->PHDMAW & SPIM_PHDMAW_DW_ADDR_Msk) >> SPIM_PHDMAW_DW_ADDR_Pos)

/**
 * @brief   Set Double Transfer Rate Mode Enable Bit for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_ADDR_DTR(spim)   (spim->PHDMAW |= (SPIM_PHDMAW_DTR_ADDR_Msk))

/**
 * @brief   Set Double Transfer Rate Mode Disable Bit for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_ADDR_DTR(spim)  (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_ADDR_Msk))

/**
 * @brief   Set SPI Interface Bit Mode for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Address Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_ADDR_BITMODE(spim, x)  \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_BM_ADDR_Msk)) | \
                    (x << SPIM_PHDMAW_BM_ADDR_Pos))

/**
 * @brief   Set Double Transfer Rate Mode Enable Bit for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAW_DATA_DTR(spim)   (spim->PHDMAW |= (SPIM_PHDMAW_DTR_DATA_Msk))

/**
 * @brief   Set Double Transfer Rate Mode Disable Bit for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAW_DATA_DTR(spim)  (spim->PHDMAW &= ~(SPIM_PHDMAW_DTR_DATA_Msk))

/**
 * @brief   Set Program Data Byte Order of Program Data Phase for Octal SPI Flash,.
 * @param[in]   spim
 * @param[in]   x   Data Byte Order
 *                  - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                  - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                  - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                  - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_PBO_DATA(spim, x)   \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_PBO_DATA_Msk)) | \
                    (x << SPIM_PHDMAW_PBO_DATA_Pos))

/**
 * @brief   Set SPI Interface Bit Mode for Data Phase.
 * @param[in]   spim
 * @param[in]   x   Data Phase Witdh.
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAW_DATA_BITMODE(spim, x)  \
    (spim->PHDMAW = (spim->PHDMAW & ~(SPIM_PHDMAW_BM_DATA_Msk)) | \
                    (x << SPIM_PHDMAW_BM_DATA_Pos))

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMAR constant definitions                                           */
/*----------------------------------------------------------------------------*/
/**
 * @brief   Clear PHDMAR Setting
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_CLEAR_PHDMAR(spim) (spim->PHDMAR &= ~(0xFFFFFFFF))

/**
 * @brief   Set Read Data Width for Command Phase.
 * @param[in]   spim
 * @param[in]   x   Command Phase Witdh.
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_CMD_WIDTH(spim, x)  \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_DW_CMD_Msk)) | \
                    (x << SPIM_PHDMAR_DW_CMD_Pos))

/**
 * @brief   Get Read Data Width for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_WIDTH(spim) \
    ((spim->PHDMAR & SPIM_PHDMAR_DW_CMD_Msk) >> SPIM_PHDMAR_DW_CMD_Pos)

/**
 * @brief   Enable Double Transfer Rate Mode for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_CMD_DTR(spim)    (spim->PHDMAR |= (SPIM_PHDMAR_DTR_CMD_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_CMD_DTR(spim)   (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_CMD_Msk))

/**
 * @brief   Get Double Transfer Rate Mode Disable Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_DTR(spim)   \
    ((spim->PHDMAR & SPIM_PHDMAR_DTR_CMD_Msk) >> SPIM_PHDMAR_DTR_CMD_Pos)

/**
 * @brief   Set SPI Interface Bit Mode for Command Phase.
 * @param[in]   spim
 * @param[in]   x   Command Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_CMD_BITMODE(spim, x)   \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_BM_CMD_Msk)) | \
                    (x << SPIM_PHDMAR_BM_CMD_Pos))

/**
 * @brief   Get SPI Interface Bit Mode for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_CMD_BITMODE(spim)  \
    ((spim->PHDMAR & SPIM_PHDMAR_BM_CMD_Msk) >> SPIM_PHDMAR_BM_CMD_Pos)

/**
 * @brief   Set Data Width for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Addres Phase Witdh
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_ADDR_WIDTH(spim, x) \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_DW_ADDR_Msk)) | \
                    (x << SPIM_PHDMAR_DW_ADDR_Pos))
/**
 * @brief   Get Data Width for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_ADDR_WIDTH(spim) \
    ((spim->PHDMAR & SPIM_PHDMAR_DW_ADDR_Msk) >> SPIM_PHDMAR_DW_ADDR_Pos)

/**
 * @brief   Enable Double Transfer Rate Mode for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_ADDR_DTR(spim)   (spim->PHDMAR |= (SPIM_PHDMAR_DTR_ADDR_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_ADDR_DTR(spim)  (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_ADDR_Msk))

/**
 * @brief   Get Double Transfer Rate Mode Disable Bit for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_ADDR_DTR(spim)   \
    ((spim->PHDMAR & SPIM_PHDMAR_DTR_ADDR_Msk) >> SPIM_PHDMAR_DTR_ADDR_Pos)

/**
 * @brief   Set SPI Interface Bit Mode for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Addess Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_ADDR_BITMODE(spim, x)  \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_BM_ADDR_Msk)) | \
                    (x << SPIM_PHDMAR_BM_ADDR_Pos))

/**
 * @brief   Set Data Width for Mode Phase.
 * @param[in]   spim
 * @param[in]   x   Read Mode Phase Witdh
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_READ_DATAWIDTH(spim, x)    \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_DW_MODE_Msk)) | \
                    (x << SPIM_PHDMAR_DW_MODE_Pos))

/**
 * @brief   Enable Double Transfer Rate Mode for Read Mode Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_READ_DTR(spim)   (spim->PHDMAR |= (SPIM_PHDMAR_DTR_MODE_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode for Read Mode Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_READ_DTR(spim)  (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_MODE_Msk))

/**
 * @brief   Set SPI Interface Bit Mode for Read Mode Phase.
 * @param[in]   spim
 * @param[in]   x   Read Mode Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_READ_BITMODE(spim, x)  \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_BM_MODE_Msk)) | \
                    (x << SPIM_PHDMAR_BM_MODE_Pos))

/**
 * @brief   Enable Double Transfer Rate Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_DATA_DTR(spim)   (spim->PHDMAR |= (SPIM_PHDMAR_DTR_DATA_Msk))

/**
 * @brief   Get Double Transfer Rate Mode Disable Bit for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMAR_DATA_DTR(spim)   \
    ((spim->PHDMAR & SPIM_PHDMAR_DTR_DATA_Msk) >> SPIM_PHDMAR_DTR_DATA_Pos)

/**
 * @brief   Disable Double Transfer Rate Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_DATA_DTR(spim)  (spim->PHDMAR &= ~(SPIM_PHDMAR_DTR_DATA_Msk))

/**
 * @brief   Enable Read DQS Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMAR_DATA_RDQS(spim)  (spim->PHDMAR |= (SPIM_PHDMAR_RDQS_DATA_Msk))

/**
 * @brief   Disable Read DQS Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMAR_DATA_RDQS(spim)  (spim->PHDMAR &= ~(SPIM_PHDMAR_RDQS_DATA_Msk))

/**
 * @brief   Set Received Data Byte Order of Received Data Phase for Octal SPI Flash.
 * @param[in]   spim
 * @param[in]   x   Data Byte Order
 *                  - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                  - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                  - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                  - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_RBO_DATA(spim, x)   \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_RBO_DATA_Msk)) | \
                    (x << SPIM_PHDMAR_RBO_DATA_Pos))

/**
 * @brief   Set SPI Interface Bit Mode for Data Phase.
 * @param[in]   spim
 * @param[in]   x   Data Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMAR_DATA_BITMODE(spim, x)  \
    (spim->PHDMAR = (spim->PHDMAR & ~(SPIM_PHDMAR_BM_DATA_Msk)) | \
                    (x << SPIM_PHDMAR_BM_DATA_Pos))

/*----------------------------------------------------------------------------*/
/* SPIM_PHDMM constant definitions                                            */
/*----------------------------------------------------------------------------*/
/**
 * @brief   Clear PHDMM Setting
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_CLEAR_PHDMM(spim)  (spim->PHDMM &= ~(0xFFFFFFFF))

/**
 * @brief   Set DMM Mode Data Width for Command Phase.
 * @param[in]   spim
 * @param[in]   x   Command Phase Witdh
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_CMD_WIDTH(spim, x)   \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_DW_CMD_Msk)) | \
                   (x << SPIM_PHDMM_DW_CMD_Pos))

/**
 * @brief   Get DMM Mode Data Width for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_WIDTH(spim)  \
    ((spim->PHDMM & SPIM_PHDMM_DW_CMD_Msk) >> SPIM_PHDMM_DW_CMD_Pos)

/**
 * @brief   Enable DMM Mode Double Transfer Rate Mode for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_CMD_DTR(spim) (spim->PHDMM |= (SPIM_PHDMM_DTR_CMD_Msk))

/**
 * @brief   Disable DMM Mode Double Transfer Rate Mode for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_CMD_DTR(spim)    (spim->PHDMM &= ~(SPIM_PHDMM_DTR_CMD_Msk))

/**
 * @brief   Get DMM Mode Double Transfer Rate Mode Enable Bit for Command Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_DTR(spim)    \
    ((spim->PHDMM & SPIM_PHDMM_DTR_CMD_Msk) >> SPIM_PHDMM_DTR_CMD_Pos)

/**
 * @brief   Set SPI Interface Bit Mode for Command Phase,.
 * @param[in]   spim
 * @param[in]   x   Command Phase Bits Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_CMD_BITMODE(spim, x)    \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_BM_CMD_Msk)) | \
                   (x << SPIM_PHDMM_BM_CMD_Pos))

/**
 * @brief   Get SPI Interface Bit Mode for Command Phase,.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_CMD_BITMODE(spim)   \
    ((spim->PHDMM & SPIM_PHDMM_BM_CMD_Msk) >> SPIM_PHDMM_BM_CMD_Pos)

/**
 * @brief   Set Data Width for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Set Address Width
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_ADDR_WIDTH(spim, x)  \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_DW_ADDR_Msk)) | \
                   (x << SPIM_PHDMM_DW_ADDR_Pos))

/**
 * @brief   Get Data Width for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_GET_PHDMM_ADDR_WIDTH(spim)  \
    ((spim->PHDMM & SPIM_PHDMM_DW_ADDR_Msk) >> SPIM_PHDMM_DW_ADDR_Pos)

/**
 * @brief   Enable Double Transfer Rate Mode for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_ADDR_DTR(spim)    (spim->PHDMM |= (SPIM_PHDMM_DTR_ADDR_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode for Address Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_ADDR_DTR(spim)   (spim->PHDMM &= ~(SPIM_PHDMM_DTR_ADDR_Msk))

/**
 * @brief   Set SPI Interface Bit Mode for Address Phase.
 * @param[in]   spim
 * @param[in]   x   Address Phase Bit Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_ADDR_BITMODE(spim, x)   \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_BM_ADDR_Msk)) | \
                   (x << SPIM_PHDMM_BM_ADDR_Pos))

/**
 * @brief   Set Data Width for Mode Phase.
 * @param[in]   spim
 * @param[in]   x   Read mode phase width
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_READ_DATA_WIDTH(spim, x) \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_DW_MODE_Msk)) | \
                   (x << SPIM_PHDMM_DW_MODE_Pos))

/**
 * @brief   Enable Double Transfer Rate Mode for Mode Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_READ_DTR(spim)    (spim->PHDMM |= (SPIM_PHDMM_DTR_MODE_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode for Mode Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_READ_DTR(spim)   (spim->PHDMM &= ~(SPIM_PHDMM_DTR_MODE_Msk))

/**
 * @brief   Set SPI Interface Bit Mode for Mode Phase.
 * @param[in]   spim
 * @param[in]   x   Read Mode Phase Bit Mode
 *                  - \ref PHASE_NORMAL_MODE
 *                  - \ref PHASE_DUAL_MODE
 *                  - \ref PHASE_QUAD_MODE
 *                  - \ref PHASE_OCTAL_MODE
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_READ_BITMODE(spim, x)   \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_BM_MODE_Msk)) | \
                   (x << SPIM_PHDMM_BM_MODE_Pos))

/**
 * @brief   Enable Double Transfer Rate Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_DATA_DTR(spim)    (spim->PHDMM |= (SPIM_PHDMM_DTR_DATA_Msk))

/**
 * @brief   Disable Double Transfer Rate Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_DATA_DTR(spim)   (spim->PHDMM &= ~(SPIM_PHDMM_DTR_DATA_Msk))

/**
 * @brief   Enable Read DQS Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_ENABLE_PHDMM_DATA_RDQS(spim)   (spim->PHDMM |= (SPIM_PHDMM_RDQS_DATA_Msk))

/**
 * @brief   Disable Read DQS Mode for Data Phase.
 * @param[in]   spim
 * \hideinitializer
 */
#define SPIM_DISABLE_PHDMM_DATA_RDQS(spim)  (spim->PHDMM &= ~(SPIM_PHDMM_RDQS_DATA_Msk))

/**
 * @brief   Set Received Data Byte Order of Received Data Phase for Octal SPI Flash.
 * @param[in]   spim
 * @param[in]   x   Data Byte Order
 *                  - \ref PHASE_ORDER_MODE0 : Byte order byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7.
 *                  - \ref PHASE_ORDER_MODE1 : Byte order byte7, byte6, byte5, byte4, byte3, byte2, byte1, byte0.
 *                  - \ref PHASE_ORDER_MODE2 : Byte order byte1, byte0, byte3, byte2, byte5, byte4, byte7, byte6.
 *                  - \ref PHASE_ORDER_MODE3 : Byte order byte6, byte7, byte4, byte5, byte2, byte3, byte0, byte1.
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_RBO_DATA(spim, x)    \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_RBO_DATA_Msk)) | \
                   (x << SPIM_PHDMM_RBO_DATA_Pos))

/**
 * @brief   Set SPI Interface Bit Mode for Data Phase.
 * @param[in]   spim
 * @param[in]   x   Read mode phase width
 *                  - \ref PHASE_WIDTH_8
 *                  - \ref PHASE_WIDTH_16
 *                  - \ref PHASE_WIDTH_24
 *                  - \ref PHASE_WIDTH_32
 * \hideinitializer
 */
#define SPIM_SET_PHDMM_DATA_BITMODE(spim, x)   \
    (spim->PHDMM = (spim->PHDMM & ~(SPIM_PHDMM_BM_DATA_Msk)) | \
                   (x << SPIM_PHDMM_BM_DATA_Pos))

/*----------------------------------------------------------------------------*/
/* SPIM_DLLx constant definitions                                            */
/*----------------------------------------------------------------------------*/
/**
  * @brief   Enable or Disable DLL0 OLDO.
  * @param[in]   spim
  * @param[in]   x  Enable or Disable
  *                 - \ref SPIM_OP_ENABLE : Enable
  *                 - \ref SPIM_OP_DISABLE : Disable
  * \hideinitializer
  */
#define SPIM_ENABLE_DLLOLDO(spim, x)  \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLOLDO_Msk)) | \
                  (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOLDO_Pos))

/**
  * @brief   Get DLL0 OLDO Enable Bit.
  * @param[in]  spim
  * @return     SPIM_OP_ENABLE : Enable
  *             SPIM_OP_DISABLE: Disable
  * \hideinitializer
  */
#define SPIM_GET_DLLOLDO(spim)  \
    (((spim)->DLL0 & SPIM_DLL0_DLLOLDO_Msk) >> SPIM_DLL0_DLLOLDO_Pos)

/**
  * @brief   Set DLL0 Output Valid Counter Reset.
  * @param[in]   spim
  * @param[in]   x   DLL0 Output Valid Counter Reset.
  *                  It could be 0: Disable or 1: Enable.
  * \hideinitializer
  */
#define SPIM_ENABLE_DLLOVRST(spim, x) \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLOVRST_Msk)) | \
                  (((x) ? 1UL : 0UL) << SPIM_DLL0_DLLOVRST_Pos))

/**
  * @brief   Get DLL0 Output Valid Counter Reset Done.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLOVRST(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLOVRST_Msk) >> SPIM_DLL0_DLLOVRST_Pos)

/**
  * @brief   Get DLL0 Clock Divider Circuit Status Bit.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLCLKON(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLCLKON_Msk) >> SPIM_DLL0_DLLCLKON_Pos)

/**
  * @brief   Get DLL0 Lock Status Bit.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLLOCK(spim)   \
    ((spim->DLL0 & SPIM_DLL0_DLLLOCK_Msk) >> SPIM_DLL0_DLLLOCK_Pos)

/**
  * @brief   Get DLL0 Output Ready Status.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLREADY(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLREADY_Msk) >> SPIM_DLL0_DLLREADY_Pos)

/**
  * @brief   Get DLL0 Auto Trim Ready Status.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLATRDY(spim)  \
    ((spim->DLL0 & SPIM_DLL0_DLLATRDY_Msk) >> SPIM_DLL0_DLLATRDY_Pos)

/**
  * @brief   Get DLL0 Refresh Status Bit.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLREF(spim)    \
    ((spim->DLL0 & SPIM_DLL0_DLL_REF_Msk) >> SPIM_DLL0_DLL_REF_Pos)

/**
  * @brief   Set DLL0 Delay Step Number.
  * @param[in]   spim
  * @param[in]   x   DLL Delay Step Number. It could be 0x0 ~ 0x1F
  * \hideinitializer
  */
#define SPIM_SET_DLLDLY_NUM(spim, x)    \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLL_DNUM_Msk)) | \
                  ((x) << SPIM_DLL0_DLL_DNUM_Pos))

/**
 * @brief   Set DLL Auto Trim.
 * @param[in] spim
 * @param[in] x is starts to count from 0x0 to DLLOVNUM
 *            - \ref SPIM_OP_ENABLE
 *            - \ref SPIM_OP_DISABLE
 * \hideinitializer
 */
#define SPIM_SET_AUTO_TRIM_DLL(spim, x)   \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLATEN_Msk)) | ((x) << SPIM_DLL0_DLLATEN_Pos))


/**
  * @brief  Enable/Disable HYPERDLL Delay Time Self-Test.
  * @param[in]   spim
  * @param[in]   x is starts to count from 0x0 to DLLOVNUM
  *              - \ref SPIM_HYPER_OP_ENABLE
  *              - \ref SPIM_HYPER_OP_DISABLE
  * \hideinitializer
  */
#define SPIM_SET_DLL_SELF_TEST(spim, x) \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLLOOP_Msk)) | ((x) << SPIM_DLL0_DLLLOOP_Pos))

/**
  * @brief  Enable/Disable Selection for Clock Source DQS/RWDS of HYPERDLL Open Loop Delay Line.
  * @param[in]   spim
  * @param[in]   x is starts to count from 0x0 to DLLOVNUM
  *              - \ref SPIM_HYPER_OP_ENABLE
  *              - \ref SPIM_HYPER_OP_DISABLE
  * \hideinitializer
  */
#define SPIM_SET_INTERNAL_RWDS(spim, x) \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_SIGINTEN_Msk)) | ((x) << SPIM_DLL0_SIGINTEN_Pos))

/**
 * @brief   Set DLL Divider. It could be 0 ~ 3.
 * @param[in] spim
 * @param[in] x Divider Number Selection of DLL Internal Clock Divder.
 *              It could be 0 ~ 3.
 * \hideinitializer
 */
#define SPIM_SET_DLLDIV(spim, x)    \
    (spim->DLL0 = (spim->DLL0 & ~(SPIM_DLL0_DLLDIVER_Msk)) | ((x) << SPIM_DLL0_DLLDIVER_Pos))

/**
  * @brief   Set Clock Cycle Number between DLL Lock and DLL Output Valid.
  * @param[in]   spim
  * @param[in]   x   Clock Cycle Number between DLL Lock and DLL Output Valid.
  *                  It could be 0 ~ 0xFFFF.
  * \hideinitializer
  */
#define SPIM_SET_DLLOV_NUM(spim, x)    \
    (spim->DLL1 = (spim->DLL1 & ~(SPIM_DLL1_DLLOVNUM_Msk)) | \
                  ((x) << SPIM_DLL1_DLLOVNUM_Pos))

/**
  * @brief   Get Cycle Number of DLL1 Ouput Valid. It could be 0 ~ 0xFFFF.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLOV_NUM(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLOVNUM_Msk) >> SPIM_DLL1_DLLOVNUM_Pos)

/**
  * @brief   Set Cycle Number between DLL Clock Divider Enable and DLL Lock Valid.
  * @param[in]   spim
  * @param[in]   x   Cycle Number between DLL Clock Divider Enable and DLL Lock Valid.
  *                  It could be 0 ~ 0xFFFF.
  * \hideinitializer
  */
#define SPIM_SET_DLLLOCK_NUM(spim, x)  \
    (spim->DLL1 = (spim->DLL1 & ~(SPIM_DLL1_DLLLKNUM_Msk)) | \
                  ((x) << SPIM_DLL1_DLLLKNUM_Pos))

/**
  * @brief   Get Cycle Number between DLL Clock Divider Enable and DLL Lock Valid.
  * @param[in]   spim
  * \hideinitializer
  */
#define SPIM_GET_DLLLOCK_NUM(spim) \
    ((spim->DLL1 & SPIM_DLL1_DLLLKNUM_Msk) >> SPIM_DLL1_DLLLKNUM_Pos)

/**
  * @brief   Set Cycle Number of between DLL Output Valid and DLL Auto Trim Enable Time.
  * @param[in]   spim
  * @param[in]   x   Cycle Number of between DLL Output Valid and DLL Auto Trim Enable Time.
  *                  It could be 0 ~ 0xFFFF.
  * \hideinitializer
  */
#define SPIM_SET_DLLTRIM_NUM(spim, x) \
    (spim->DLL2 = (spim->DLL2 & ~(SPIM_DLL2_TRIMNUM_Msk)) | \
                  ((x) << SPIM_DLL2_TRIMNUM_Pos))

/**
  * @brief   Set Cycle Number of between DLL OLDO Enable and DLL Clock Divider Enable Time.
  * @param[in]   spim
  * @param[in]   x   Cycle Number of between DLL OLDO Enable and DLL Clock Divider Enable Time.
  *                  It could be 0 ~ 0xFFFF.
  * \hideinitializer
  */
#define SPIM_SET_DLLCLKON_NUM(spim, x)    \
    (spim->DLL2 = (spim->DLL2 & ~(SPIM_DLL2_CLKONNUM_Msk)) | \
                  ((x) << SPIM_DLL2_CLKONNUM_Pos))

/*----------------------------------------------------------------------------*/
/* static inline functions                                                    */
/*----------------------------------------------------------------------------*/
__STATIC_INLINE int32_t SPIM_WaitDMMIdleState(SPIM_T *spim);
__STATIC_INLINE void SPIM_ENABLE_CIPHER(SPIM_T *spim);
__STATIC_INLINE void SPIM_DISABLE_CIPHER(SPIM_T *spim);
__STATIC_INLINE void SPIM_ENABLE_CACHE(SPIM_T *spim);
__STATIC_INLINE void SPIM_DISABLE_CACHE(SPIM_T *spim);
__STATIC_INLINE uint32_t SPIM_GET_DMMADDR(SPIM_T *spim);
__STATIC_INLINE void SPIM_SET_OPMODE(SPIM_T *spim, uint32_t u32OPMode);

/**
  * @brief  Wait Hyper Direct Map Mode Read/Write Done.
  * @param  spim
  * @return SPIM_HYPER_OK          SPIM operation OK.
  *         SPIM_HYPER_ERR_TIMEOUT SPIM operation abort due to timeout error.
  * @note   This function sets SPIM_HYPER_ERR_TIMEOUT, if waiting Hyper Chip time-out.
  */
__STATIC_INLINE int32_t SPIM_WaitDMMIdleState(SPIM_T *spim)
{
    volatile int32_t u32TimeOutCount = SPIM_TIMEOUT;

    /* Wait for DMM mode to be idle. */
    while (SPIM_GET_DMM_IDLE(spim) == SPIM_OP_DISABLE)
    {
        if (--u32TimeOutCount <= 0)
        {
            return SPIM_ERR_TIMEOUT;
        }
    }

    return SPIM_OK;
}

/**
 * @brief   Enable cipher.
 * @param[in]   spim
 * @note    When encryption/decryption of SPIM is enabled,
 *          please set SPIM_SET_DMM_DESELTIM() >= 0x10.
 */
__STATIC_INLINE void SPIM_ENABLE_CIPHER(SPIM_T *spim)
{
    SPIM_WaitDMMIdleState(spim);

    spim->CTL0 &= ~(SPIM_CTL0_CIPHOFF_Msk);

    SPIM_SET_DMM_DESELTIM(spim, 0x12);
}

/**
 * @brief   Disable cipher.
 * @param[in]   spim
 * @note    When encryption/decryption of SPIM is disabled,
 *          please set SPIM_SET_DMM_DESELTIM >= 0x8.
 */
__STATIC_INLINE void SPIM_DISABLE_CIPHER(SPIM_T *spim)
{
    SPIM_WaitDMMIdleState(spim);

    spim->CTL0 |= (SPIM_CTL0_CIPHOFF_Msk);

    SPIM_SET_DMM_DESELTIM(spim, 0x08);
}

/**
 * @brief   Enable cache.
 * @param[in]   spim
 * @note    Minimum time width of SPIM_SS
 *          deselect time = (DESELTIM + 4) * AHB clock cycle time.
 */
__STATIC_INLINE void SPIM_ENABLE_CACHE(SPIM_T *spim)
{
    SPIM_WaitDMMIdleState(spim);

    (spim->CTL1 &= ~(SPIM_CTL1_CACHEOFF_Msk));

    /* Cipher Disabled Set Deselect Time 0x04 */
    if (((spim->CTL0 & SPIM_CTL0_CIPHOFF_Msk) >> SPIM_CTL0_CIPHOFF_Pos) != SPIM_OP_DISABLE)
    {
        SPIM_SET_DMM_DESELTIM(spim, 0x4);
    }
}

/**
 * @brief   Disable cache.
 * @param[in]   spim
 * @note    Minimum time width of SPIM_SS
 *          deselect time = (DESELTIM + 1) * AHB clock cycle time.
 */
__STATIC_INLINE void SPIM_DISABLE_CACHE(SPIM_T *spim)
{
    SPIM_WaitDMMIdleState(spim);

    (spim->CTL1 |= SPIM_CTL1_CACHEOFF_Msk);

    /* Cipher Disabled Set Deselect Time 0x01 */
    if (((spim->CTL0 & SPIM_CTL0_CIPHOFF_Msk) >> SPIM_CTL0_CIPHOFF_Pos) != SPIM_OP_DISABLE)
    {
        SPIM_SET_DMM_DESELTIM(spim, 0x1);
    }
}

/**
 * @brief   Get Direct Map Address.
 * @param[in]  spim
 * @return Direct Mapping Address
 */
__STATIC_INLINE uint32_t SPIM_GET_DMMADDR(SPIM_T *spim)
{
    return ((spim == SPIM0) ? SPIM_DMM0_ADDR : SPIM_DMM0_ADDR);
}

/**
 * @brief       Set operation mode.
 * @param[in]   spim
 * @param[in]   x   SPI Function Operation Mode
 *                  - \ref SPIM_CTL0_OPMODE_IO
 *                  - \ref SPIM_CTL0_OPMODE_PAGEWRITE
 *                  - \ref SPIM_CTL0_OPMODE_PAGEREAD
 *                  - \ref SPIM_CTL0_OPMODE_DIRECTMAP
 * \hideinitializer
 */
__STATIC_INLINE void SPIM_SET_OPMODE(SPIM_T *spim, uint32_t u32OPMode)
{
    SPIM_WaitDMMIdleState(spim);

    (spim->CTL0 = (spim->CTL0 & ~(SPIM_CTL0_OPMODE_Msk)) | ((u32OPMode) << SPIM_CTL0_OPMODE_Pos));
}

/** @addtogroup SPIM_EXPORTED_TYPEDEF SPIM Exported Type Defines
  @{
*/
typedef struct
{
    uint32_t u32CMDCode;        /*!< Page Program Command Code */

    uint32_t u32CMDPhase;       /*!< Command phase mode */
    uint32_t u32CMDWidth;       /*!< Command Width */
    uint32_t u32CMDDTR;         /*!< Command use DTR mode */

    uint32_t u32AddrPhase;      /*!< Address phase mode */
    uint32_t u32AddrWidth;      /*!< Address Width */
    uint32_t u32AddrDTR;        /*!< Address use DTR mode */

    uint32_t u32DataPhase;      /*!< Data phase mode */
    uint32_t u32ByteOrder;      /*!< Data Byte Order */
    uint32_t u32DataDTR;        /*!< Data use DTR mode */
    uint32_t u32RDQS;           /*!< Receive data from SPI Flash when read DQS */

    uint32_t u32DcNum;          /*!< Dummy `cycle count */

    /* Continue read mode only support 0xBB, 0xEB, 0xE7, 0x0D, 0xBD, and 0xED
       in Direct Map Mode */
    uint32_t u32ContRdEn;       /*!< Enable Continue Read mode */
    uint32_t u32RdModePhase;    /*!< Read mode phase mode */
    uint32_t u32RdModeWidth;    /*!< Read mode phase mode */
    uint32_t u32RdModeDTR;      /*!< Read mode use DTR mode */

} SPIM_PHASE_T;  /*!< Structure holds SPIM IO phase info */

/** @} end of group SPIM_EXPORTED_TYPEDEF */
/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t  SPIM_InitFlash(SPIM_T *spim, int clrWP);
uint32_t SPIM_GetSClkFreq(SPIM_T *spim);
void SPIM_ReadJedecId(SPIM_T *spim, uint8_t idBuf[], uint32_t u32NRx, uint32_t u32NBit);
void SPIM_ReadStatusRegister(SPIM_T *spim, uint8_t dataBuf[], uint32_t u32NRx, uint32_t u32NBit);
int32_t SPIM_Enable_4Bytes_Mode(SPIM_T *spim, int isEn, uint32_t u32NBit);
int32_t  SPIM_Is4ByteModeEnable(SPIM_T *spim, uint32_t u32NBit);
void SPIM_SetWriteEnable(SPIM_T *spim, int isEn, uint32_t u32NBit);
int32_t SPIM_SetWrapAroundEnable(SPIM_T *spim, uint32_t u32IsEn);

void SPIM_ChipErase(SPIM_T *spim, uint32_t u32NBit, int isSync);
void SPIM_EraseBlock(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint8_t u8ErsCmd, uint32_t u32NBit, int isSync);

void SPIM_IO_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx, uint8_t pu8TxBuf[], uint8_t wrCmd, uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat);
void SPIM_IO_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t pu8RxBuf[], uint8_t rdCmd, uint32_t u32NBitCmd, uint32_t u32NBitAddr, uint32_t u32NBitDat, int u32NDummy);

void SPIM_DMA_Write(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NTx, uint8_t pu8TxBuf[], uint32_t wrCmd);
int32_t SPIM_DMA_Read(SPIM_T *spim, uint32_t u32Addr, int is4ByteAddr, uint32_t u32NRx, uint8_t pu8RxBuf[], uint32_t u32RdCmd, int isSync);

void SPIM_EnterDirectMapMode(SPIM_T *spim, int is4ByteAddr, uint32_t u32RdCmd, uint32_t u32IdleIntvl);
void SPIM_ExitDirectMapMode(SPIM_T *spim);

void SPIM_SetQuadEnable(SPIM_T *spim, int isEn, uint32_t u32NBit);

void SPIM_WinbondUnlock(SPIM_T *spim, uint32_t u32NBit);

/*----------------------------------------------------------------------------*/
/* DMA/DMM Read/Write Operations by Phase Table                                   */
/*----------------------------------------------------------------------------*/
uint32_t SPIM_PhaseModeToNBit(uint32_t u32Phase);
void SPIM_DMADMM_InitPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, uint32_t u32OPMode);

/*----------------------------------------------------------------------------*/
/* I/O Read/Write Operations by Phase Table                                   */
/*----------------------------------------------------------------------------*/
void SPIM_IO_SendCommandByPhase(SPIM_T *spim, uint32_t u32OPMode, uint32_t u32OpCMD, uint32_t u32CMDPhase, uint32_t u32DTREn);
void SPIM_IO_SendAddressByPhase(SPIM_T *spim, uint32_t u32Is4ByteAddr, uint32_t u32Addr, uint32_t u32AddrPhase, uint32_t u32DTREn);
int32_t SPIM_IO_SendDummyByPhase(SPIM_T *spim, int u32NDummy);
void SPIM_IO_SendDataByPhase(SPIM_T *spim, uint32_t u32OPMode, uint8_t *pu8TRxBuf, uint32_t u32TRxSize, uint32_t u32DataPhase, uint32_t u32DTREn, uint32_t u32RdDQS);

void SPIM_IO_ReadByPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, uint32_t u32Addr, uint8_t *pu8RxBuf, uint32_t u32RdSize);
void SPIM_IO_WriteByPhase(SPIM_T *spim, SPIM_PHASE_T *psPhaseTable, uint32_t u32Addr, uint8_t *pu8TxBuf, uint32_t u32WrSize, uint32_t u32WrDone);

/*----------------------------------------------------------------------------*/
/* Trim related to data read/write strobes                                    */
/*----------------------------------------------------------------------------*/
int32_t SPIM_INIT_DLL(SPIM_T *spim);
int32_t SPIM_SetDLLDelayNum(SPIM_T *spim, uint32_t u32DelayNum);

/** @} end of group SPIM_EXPORTED_FUNCTIONS */
/** @} end of group SPIM_Driver */
/** @} end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __NU_SPIM_H__ */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
