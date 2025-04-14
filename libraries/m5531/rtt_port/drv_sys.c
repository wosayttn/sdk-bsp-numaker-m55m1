/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2020-11-11      Wayne            First version
*
******************************************************************************/

#include <rtthread.h>
#include "drv_sys.h"

#define LOG_TAG    "drv.sys"
#undef  DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL      LOG_LVL_DBG
#define DBG_COLOR
#include <rtdbg.h>

void nu_sys_check_register(S_NU_REG *psNuReg)
{
    if (psNuReg == RT_NULL)
        return;

    while (psNuReg->vu32RegAddr != 0)
    {
        vu32 vc32RegValue = *((vu32 *)psNuReg->vu32RegAddr);
        vu32 vc32BMValue = vc32RegValue & psNuReg->vu32BitMask;
        LOG_I("[%3s] %32s(0x%08x) %24s(0x%08x): 0x%08x(AndBitMask:0x%08x)",
                   (psNuReg->vu32Value == vc32BMValue) ? "Ok" : "!OK",
                   psNuReg->szVName,
                   psNuReg->vu32Value,
                   psNuReg->szRegName,
                   psNuReg->vu32RegAddr,
                   vc32RegValue,
                   vc32BMValue);
        psNuReg++;
    }

    LOG_HEX("PDMA0", 16, (void *)PDMA0_BASE, sizeof(PDMA_T));
    LOG_HEX("PDMA1", 16, (void *)PDMA1_BASE, sizeof(PDMA_T));
}

