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
#include "rtthread.h"

#if defined(RT_USING_MEMHEAP)

#include <rthw.h>
#include "NuMicro.h"
#include "drv_memheap.h"

#define LOG_TAG    "drv.mh"
#define DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL   LOG_LVL_INFO
#define DBG_COLOR
#include <rtdbg.h>

#if !defined(SPIM_HYPER_DMM0_SIZE)
    #define SPIM_HYPER_DMM0_SIZE  (0x800000)
#endif

#define LPSRAM_MEMHEAP_NAME    "lpsram"
#define LPSRAM_MEMHEAP_SIZE    (8*1024)
#define LPSRAM_MEMHEAP_BEGIN   ((void*)LPSRAM_BASE)

#define ITCM_MEMHEAP_NAME      "itcm"
#if defined(__ARMCC_VERSION)
    extern uint32_t Image$$ITCM$$ZI$$Limit;

    #define ITCM_MEMHEAP_BEGIN ((void *)&Image$$ITCM$$ZI$$Limit)
    #define ITCM_MEMHEAP_SIZE  ((ITCM_BASE + ITCM_SIZE) - (uint32_t)ITCM_MEMHEAP_BEGIN)
#else
    extern uint32_t __itcm_end__;

    #define ITCM_MEMHEAP_BEGIN ((void *)&__itcm_end__)
    #define ITCM_MEMHEAP_SIZE  ((ITCM_BASE + ITCM_SIZE) - (uint32_t)ITCM_MEMHEAP_BEGIN)
#endif

#define DTCM_MEMHEAP_NAME      "dtcm"
#if defined(__ARMCC_VERSION)
    extern uint32_t Image$$DTCM$$ZI$$Limit;

    #define DTCM_MEMHEAP_BEGIN ((void *)&Image$$DTCM$$ZI$$Limit)
    #define DTCM_MEMHEAP_SIZE  ((DTCM_BASE + DTCM_SIZE) - (uint32_t)DTCM_MEMHEAP_BEGIN)
#else
    extern uint32_t __dtcm_end__;

    #define DTCM_MEMHEAP_BEGIN ((void *)&__dtcm_end__)
    #define DTCM_MEMHEAP_SIZE  ((DTCM_BASE + DTCM_SIZE) - (uint32_t)DTCM_MEMHEAP_BEGIN)
#endif

#define SPIM0_MEMHEAP_NAME     "spim0"
#define SPIM0_MEMHEAP_SIZE     (SPIM_HYPER_DMM0_SIZE)
#define SPIM0_MEMHEAP_BEGIN    ((void*)SPIM_HYPER_DMM0_SADDR)

struct nu_memheap
{
    struct rt_memheap memheap;
    char *name;
    void *mem_base;
    rt_size_t mem_size;
};
typedef struct nu_memheap *nu_memheap_t;

static struct nu_memheap nu_memheap_arr[] =
{
    {
        .name       = LPSRAM_MEMHEAP_NAME,
        .mem_base   = LPSRAM_MEMHEAP_BEGIN,
        .mem_size   = LPSRAM_MEMHEAP_SIZE,
    },
    {
        .name       = DTCM_MEMHEAP_NAME,
        .mem_base   = DTCM_MEMHEAP_BEGIN,
    },
    {
        .name       = ITCM_MEMHEAP_NAME,
        .mem_base   = ITCM_MEMHEAP_BEGIN,
    },
#if defined(BSP_USING_SPIM0)
    {
        .name       = SPIM0_MEMHEAP_NAME,
        .mem_base   = SPIM0_MEMHEAP_BEGIN,
        .mem_size   = SPIM0_MEMHEAP_SIZE,
    },
#endif
};

struct rt_memheap *nu_memheap_get(E_NU_MEMHEAP_IDX idx)
{
    if ((idx > NU_MEMHEAP_START) && (idx < NU_MEMHEAP_CNT))
        return &nu_memheap_arr[idx].memheap;

    return RT_NULL;
}

int rt_hw_memheap_init(void)
{
    int i;
    rt_err_t ret = RT_EOK;

#if defined(BSP_USING_SPIM)
    int rt_hw_spim_init(void);
    rt_hw_spim_init();
#endif

    nu_memheap_arr[NU_MEMHEAP_DTCM].mem_size = DTCM_MEMHEAP_SIZE;
    nu_memheap_arr[NU_MEMHEAP_ITCM].mem_size = ITCM_MEMHEAP_SIZE;

    for (i = (NU_MEMHEAP_START + 1); i < NU_MEMHEAP_CNT; i++)
    {
        LOG_I("Initial %s memheap: Begin@%08x , remain@%08x\n",
              nu_memheap_arr[i].name,
              nu_memheap_arr[i].mem_base,
              nu_memheap_arr[i].mem_size);

        ret = rt_memheap_init(&nu_memheap_arr[i].memheap,
                              nu_memheap_arr[i].name,
                              (void *)nu_memheap_arr[i].mem_base,
                              (rt_size_t)nu_memheap_arr[i].mem_size);
        RT_ASSERT(RT_EOK == ret);
    }

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_memheap_init);

#endif /* #if defined(RT_USING_MEMHEAP) */
