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

#if defined(BSP_USING_SPIM) && defined(RT_USING_MEMHEAP)

#include <rthw.h>
#include "NuMicro.h"
#include "hyperram_code.h"

#if !defined(SPIM_HYPER_DMM0_SIZE)
    #define SPIM_HYPER_DMM0_SIZE  (0x800000)
#endif

#define LOG_TAG    "drv.spim"
#define DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL   LOG_LVL_INFO
#define DBG_COLOR
#include <rtdbg.h>

typedef enum
{
    SPIM_START = -1,
#if defined(BSP_USING_SPIM0)
    SPIM0_IDX,
#endif
    SPIM_CNT,
} E_NU_SPIM_IDX;

struct nu_spim
{
    char *name;
    SPIM_T *base;
    OTFC_T *otfc_base;
    void *mem_base;
    rt_size_t mem_size;
};
typedef struct nu_spim *nu_spim_t;

/* SPIM cipher key User defined. */
static uint32_t s_au32AESKey[8] =
{
    0x93484D6F, //Key0
    0x2F7A7F2A, //Key1
    0x063FF08A, //Key2
    0x7A29E38E, //Key3
    0x7A29E38E, //Scramble
    0x063FF08A, //NONCE0
    0x2F7A7F2A, //NONCE1
    0x93484D6F, //NONCE2
};

static struct nu_spim nu_spim_arr[] =
{
#if defined(BSP_USING_SPIM0)
    {
        .name     = "spim0",
        .base     = SPIM0,
#if defined(BSP_USING_OTFC0)
        .otfc_base = OTFC0,
#else
        .otfc_base = RT_NULL,
#endif
        .mem_base = (void *)SPIM_HYPER_DMM0_SADDR,
        .mem_size = SPIM_HYPER_DMM0_SIZE
    },
#endif
};

int rt_hw_spim_init(void)
{
    int i;
    rt_err_t ret = RT_EOK;

    for (i = (SPIM_START + 1); i < SPIM_CNT; i++)
    {
        uint32_t u32TDN = HyperRAM_Init(nu_spim_arr[i].base);
        rt_kprintf("%s Init Done, Delay number: %d\n", nu_spim_arr[i].name, u32TDN);

        SPIM_HYPER_EnterDirectMapMode(nu_spim_arr[i].base);

        if (nu_spim_arr[i].otfc_base != RT_NULL)
        {
            OTFC_SetKeyFromKeyReg(nu_spim_arr[i].otfc_base,
                                  s_au32AESKey,
                                  OTFC_PR_0,
                                  (uint32_t)nu_spim_arr[i].mem_base,
                                  nu_spim_arr[i].mem_size);

            OTFC_ENABLE_PR(nu_spim_arr[i].otfc_base, OTFC_PR_0);

            SPIM_ENABLE_CIPHER(nu_spim_arr[i].base);

            LOG_I("%s Enabled Cipher function ", nu_spim_arr[i].name);
        }

    }

    return 0;
}

#endif /* #if defined(BSP_USING_SPIM) && defined(RT_USING_MEMHEAP) */
