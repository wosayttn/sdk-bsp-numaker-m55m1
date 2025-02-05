/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2024-01-29      Wayne            First version
*
******************************************************************************/
#include "rtconfig.h"

#if defined(RT_USING_MEMHEAP)

#include <rthw.h>

typedef enum
{
    NU_MEMHEAP_START = -1,
    NU_MEMHEAP_LPSRAM,
    NU_MEMHEAP_DTCM,
    NU_MEMHEAP_ITCM,
#if defined(BSP_USING_SPIM0)
    NU_MEMHEAP_SPIM0,
#endif
    NU_MEMHEAP_CNT
} E_NU_MEMHEAP_IDX;

struct rt_memheap *nu_memheap_get(E_NU_MEMHEAP_IDX idx);

#endif /* #if defined(RT_USING_MEMHEAP) */
