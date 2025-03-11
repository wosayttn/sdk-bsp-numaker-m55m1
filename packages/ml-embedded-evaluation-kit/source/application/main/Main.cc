/*
 * SPDX-FileCopyrightText: Copyright 2021-2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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

/****************************************************************************\
 *               Main application file for ARM NPU on MPS3 board             *
\****************************************************************************/

#include "hal.h"                    /* our hardware abstraction api */
#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"  /* our inference logic api */

#include <cstdio>

extern void main_loop();

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    __ASM(" .global __ARM_use_no_argv\n");
#endif

/* Print application information. */
static void print_application_intro()
{
    info("ARM_ML_Embedded_Evaluation_Kit port on Nuvoton M55M1 Platform\n");
    info("Version 24.08.0 Build date: " __DATE__ " @ " __TIME__ "\n");
    info("Copyright 2021-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>\n\n");
}

void mlevk_entry(void *p)
{
    if (hal_platform_init())
    {
        /* Application information, UART should have been initialised. */
        print_application_intro();

        /* Enable TensorFlow Lite Micro logging. */
        EnableTFLMLog();

        /* Run the application. */
        main_loop();
    }

    /* This is unreachable without errors. */
    info("program terminating...\n");

    /* Release platform. */
    hal_platform_release();
}

int mlevk_go(void)
{
#define MLEVK_THREAD_PRIORITY   25
#define MLEVK_THREAD_STACK_SIZE (8192)
#define MLEVK_THREAD_TIMESLICE  5

    static struct rt_thread *psMLEVKThread = RT_NULL;
    static void *pvMLEVKThreadStack = RT_NULL;

    rt_thread_t threadCxt ;

    if ((threadCxt = rt_thread_find((char *)"mlevk")) != RT_NULL)
    {
        info("mlevk thread running");
    }

    if (!psMLEVKThread)
    {
        psMLEVKThread = (struct rt_thread *)rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_DTCM), sizeof(struct rt_thread));
    }

    if (!pvMLEVKThreadStack)
    {
        pvMLEVKThreadStack = rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_DTCM), MLEVK_THREAD_STACK_SIZE);
    }

    info("mlevk (Thread@%08x, %d), (Stack@%08x, %d)",
         psMLEVKThread,
         sizeof(struct rt_thread),
         pvMLEVKThreadStack,
         MLEVK_THREAD_STACK_SIZE);

    if (psMLEVKThread && pvMLEVKThreadStack)
    {
        rt_err_t ret = rt_thread_init(
                           psMLEVKThread,
                           "mlevk",
                           mlevk_entry,
                           RT_NULL,
                           pvMLEVKThreadStack,
                           MLEVK_THREAD_STACK_SIZE,
                           MLEVK_THREAD_PRIORITY,
                           MLEVK_THREAD_TIMESLICE);
        if (ret == RT_EOK)
        {
            rt_thread_startup(psMLEVKThread);
        }
        else
        {
            goto exit_mlevk_worker;
        }
    }
    else
    {
        goto exit_mlevk_worker;
    }

    return 0;

exit_mlevk_worker:

    if (psMLEVKThread)
    {
        rt_memheap_free(psMLEVKThread);
        psMLEVKThread = RT_NULL;
    }
    if (pvMLEVKThreadStack)
    {
        rt_memheap_free(pvMLEVKThreadStack);
        pvMLEVKThreadStack = RT_NULL;
    }

    return -1;
}
#if defined(MLEVK_UC_LIVE_DEMO)
    INIT_APP_EXPORT(mlevk_go);
#endif
MSH_CMD_EXPORT(mlevk_go, Start MLEVK);
