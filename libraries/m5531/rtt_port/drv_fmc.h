/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2024-3-25       Wayne            First version
*
******************************************************************************/

#ifndef __DRV_FMC_H__
#define __DRV_FMC_H__

#include <rtthread.h>

#if defined(RT_USING_FAL)

    #include <fal.h>

    extern const struct fal_flash_dev g_falFMC_AP;
    extern const struct fal_flash_dev g_falFMC_LD;
    extern const struct fal_flash_dev g_falFMC_DF;
#endif

int nu_fmc_read(long offset, uint8_t *buf, size_t size);
int nu_fmc_write(long offset, const uint8_t *buf, size_t size);
int nu_fmc_erase(long offset, size_t size);


#endif // __DRV_FMC_H___
