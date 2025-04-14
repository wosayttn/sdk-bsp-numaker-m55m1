/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-3-16       Wayne        First version
*
******************************************************************************/

#include "rtconfig.h"

#if defined(BSP_USING_GDMA)

#include <rtdevice.h>
#include "NuMicro.h"
#include "dma350_ch_drv.h"
#include "dma350_lib.h"

#if defined(__ICCARM__)
    #include "arm_cmse.h" // patch from EWARM 9.50.2 service pack
#else
    #include <arm_cmse.h>
#endif

static rt_sem_t s_xGDMASem = RT_NULL;

/* DMA350 driver structures */
static const struct dma350_dev_cfg_t GDMA_DEV_CFG_S =
{
    .dma_sec_cfg = (DMASECCFG_TypeDef *)(GDMA_S + 0x0UL),
    .dma_sec_ctrl = (DMASECCTRL_TypeDef *)(GDMA_S + 0x100UL),
    .dma_nsec_ctrl = (DMANSECCTRL_TypeDef *)(GDMA_S + 0x200UL),
    .dma_info = (DMAINFO_TypeDef *)(GDMA_S + 0xF00UL)
};

static struct dma350_dev_data_t GDMA_DEV_DATA_S =
{
    .state = 0
};

struct dma350_dev_t GDMA_DEV_S =
{
    &(GDMA_DEV_CFG_S),
    &(GDMA_DEV_DATA_S)
};

static struct dma350_ch_dev_t GDMA_CH0_DEV_S =
{
    .cfg = {
        .ch_base = (DMACH_TypeDef *)(GDMA_S + 0x1000UL),
        .channel = 0
    },
    .data = {0}
};

struct dma350_ch_dev_t *const GDMA_CH_DEV_S[] =
{
    &GDMA_CH0_DEV_S
};

void gdmaWaitForCompletion(struct dma350_ch_dev_t *dev, enum dma350_lib_exec_type_t exec_type)
{
    union dma350_ch_status_t status;

    switch (exec_type)
    {
    case DMA350_LIB_EXEC_IRQ:
        dma350_ch_enable_intr(dev, DMA350_CH_INTREN_DONE);
        dma350_ch_cmd(dev, DMA350_CH_CMD_ENABLECMD);
        if (dma350_ch_is_stat_set(dev, DMA350_CH_STAT_ERR))
        {
            RT_ASSERT(0);
        }

        if (s_xGDMASem)
            rt_sem_take(s_xGDMASem, RT_WAITING_FOREVER);

        break;

    case DMA350_LIB_EXEC_BLOCKING:
        dma350_ch_disable_intr(dev, DMA350_CH_INTREN_DONE);
        dma350_ch_cmd(dev, DMA350_CH_CMD_ENABLECMD);
        status = dma350_ch_wait_status(dev);
        if (!status.b.STAT_DONE || status.b.STAT_ERR)
        {
            RT_ASSERT(0);
        }
        break;

    default:
        RT_ASSERT(0);
    }
}

void GDMACH0_IRQHandler(void)
{
    /* Clear interrupt status. */
    union dma350_ch_status_t status = dma350_ch_get_status(GDMA_CH_DEV_S[0]);

    if (status.b.STAT_DONE)
    {
        GDMA_CH_DEV_S[0]->cfg.ch_base->CH_STATUS = DMA350_CH_STAT_DONE;

        if (s_xGDMASem)
            rt_sem_release(s_xGDMASem);
    }
}

static int rt_hw_gdma_init(void)
{
    s_xGDMASem = rt_sem_create("gdma_sem", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(s_xGDMASem);

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable GDMA0 clock source */
    CLK_EnableModuleClock(GDMA0_MODULE);

    /* Reset GDMA module */
    SYS_ResetModule(SYS_GDMA0RST);

    dma350_init(&GDMA_DEV_S);

    /* Enable NVIC for GDMA CH0 */
    NVIC_EnableIRQ(GDMACH0_IRQn);

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_gdma_init);

#endif
