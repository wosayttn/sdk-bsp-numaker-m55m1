/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include "ethosu_cpu_cache.h"

#include "NuMicro.h"                /* For CPU related defintiions */
#include "ethosu_driver.h"          /* Arm Ethos-U driver header */
#include "log_macros.h"             /* Logging macros */

void ethosu_flush_dcache(uint32_t *p, size_t bytes)
{
    UNUSED(p);
    UNUSED(bytes);

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    if (SCB->CCR & SCB_CCR_DC_Msk)
    {
        trace("Cleaning data cache (%08x)\n", p);
        if (p)
            SCB_CleanDCache_by_Addr(p, bytes);
        else
            SCB_CleanDCache();
    }
#endif /* defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U) */
}

void ethosu_invalidate_dcache(uint32_t *p, size_t bytes)
{
    UNUSED(p);
    UNUSED(bytes);

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    if (SCB->CCR & SCB_CCR_DC_Msk)
    {
        trace("Invalidating data cache (%08x)\n", p);
        if (p)
            SCB_InvalidateDCache_by_Addr(p, bytes);
        else
            SCB_InvalidateDCache();
    }
#endif /* defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U) */
}
