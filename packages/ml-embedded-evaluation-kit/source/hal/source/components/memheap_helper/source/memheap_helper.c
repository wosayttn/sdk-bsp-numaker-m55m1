/*
 * SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "memheap_helper.h"

#include "log_macros.h"
#include "rtthread.h"
#include "drv_memheap.h"

//#undef DBG_ENABLE
#define DBG_LEVEL LOG_LVL_INFO
#define DBG_SECTION_NAME  "ml.memheap"
#define DBG_COLOR
#include <rtdbg.h>

void *memheap_helper_allocate(E_AREANA_PLACEMENT evAreanaPlacement, uint32_t u32Size)
{
    void *p = RT_NULL;

    switch (evAreanaPlacement)
    {
    case evAREANA_AT_SRAM:
        p = rt_malloc_align(u32Size + 32, 32);
        break;

    case evAREANA_AT_HYPERRAM:
#if defined(BSP_USING_SPIM0)
        p = rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_SPIM0), u32Size + 32);
#endif
        break;
    default:
        break;
    }

    if (p == RT_NULL)
    {
        LOG_E("Failed to allocate from memory heap.(%d, %d Bytes)", evAreanaPlacement, u32Size);

        goto exit_memheap_helper_allocate;
    }

    LOG_I("Allocated from memory heap.(@%08X, %d Bytes)", p, u32Size);

exit_memheap_helper_allocate:

    return p;
}

void memheap_helper_free(E_AREANA_PLACEMENT evAreanaPlacement, void *p)
{
    if (p)
    {
        switch (evAreanaPlacement)
        {
        case evAREANA_AT_SRAM:
            rt_free_align(p);
            break;
        case evAREANA_AT_HYPERRAM:
#if defined(BSP_USING_SPIM0)
            rt_memheap_free(p);
#endif
            break;
        default:
            break;
        }
    }
}
