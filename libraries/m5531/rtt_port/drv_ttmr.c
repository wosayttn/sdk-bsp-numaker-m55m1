/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2022-3-15       Wayne            First version
*
******************************************************************************/

#include <rtconfig.h>

#if defined(BSP_USING_TTMR) && defined(RT_USING_HWTIMER)

#include <rtdevice.h>
#include "NuMicro.h"

/* Private define ---------------------------------------------------------------*/
#define NU_TTMR_DEVICE(timer) (nu_timer_t)(timer)

enum
{
    TTMR_START = -1,
#if defined(BSP_USING_TTMR0)
    TTMR0_IDX,
#endif
#if defined(BSP_USING_TTMR1)
    TTMR1_IDX,
#endif
    TTMR_CNT
};

/* Private typedef --------------------------------------------------------------*/
struct nu_timer
{
    rt_hwtimer_t  parent;
    char         *name;
    TTMR_T       *base;
    IRQn_Type     irqn;
    uint32_t      rstidx;
    uint64_t      modid;
};
typedef struct nu_timer *nu_timer_t;

/* Private functions ------------------------------------------------------------*/
static void nu_timer_init(rt_hwtimer_t *timer, rt_uint32_t state);
static rt_err_t nu_timer_start(rt_hwtimer_t *timer, rt_uint32_t cnt, rt_hwtimer_mode_t opmode);
static void nu_timer_stop(rt_hwtimer_t *timer);
static rt_uint32_t nu_timer_count_get(rt_hwtimer_t *timer);
static rt_err_t nu_timer_control(rt_hwtimer_t *timer, rt_uint32_t cmd, void *args);

/* Public functions -------------------------------------------------------------*/


/* Private variables ------------------------------------------------------------*/
static struct nu_timer nu_timer_arr [] =
{
#if defined(BSP_USING_TTMR0)
    { .name = "ttmr0", .base  = TTMR0, .irqn = TTMR0_IRQn, .rstidx = SYS_TTMR0RST, .modid = TTMR0_MODULE  },
#endif
#if defined(BSP_USING_TTMR1)
    { .name = "ttmr1", .base  = TTMR0, .irqn = TTMR1_IRQn, .rstidx = SYS_TTMR1RST, .modid = TTMR1_MODULE  },
#endif
};

static struct rt_hwtimer_info nu_timer_info =
{
    12000000,            /* maximum count frequency */
    46875,               /* minimum count frequency */
    0xFFFFFF,            /* the maximum counter value */
    HWTIMER_CNTMODE_UP,  /* Increment or Decreasing count mode */
};

static struct rt_hwtimer_ops nu_timer_ops =
{
    nu_timer_init,
    nu_timer_start,
    nu_timer_stop,
    nu_timer_count_get,
    nu_timer_control
};

/* Functions define ------------------------------------------------------------*/
static void nu_timer_init(rt_hwtimer_t *timer, rt_uint32_t state)
{
    nu_timer_t psNuTmr = NU_TTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    if (1 == state)
    {
        uint32_t timer_clk;
        struct rt_hwtimer_info *info = &nu_timer_info;

        timer_clk = TTMR_GetModuleClock(psNuTmr->base);
        info->maxfreq = timer_clk;
        info->minfreq = timer_clk / 256;
        TTMR_Open(psNuTmr->base, TTMR_ONESHOT_MODE, 1);
        TTMR_EnableInt(psNuTmr->base);
        NVIC_EnableIRQ(psNuTmr->irqn);
    }
    else
    {
        NVIC_DisableIRQ(psNuTmr->irqn);
        TTMR_DisableInt(psNuTmr->base);
        TTMR_Close(psNuTmr->base);
    }
}

static rt_err_t nu_timer_start(rt_hwtimer_t *timer, rt_uint32_t cnt, rt_hwtimer_mode_t opmode)
{
    rt_err_t ret = RT_EINVAL;
    rt_uint32_t u32OpMode;

    nu_timer_t psNuTmr = NU_TTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    if (cnt <= 1 || cnt > 0xFFFFFF)
    {
        goto exit_nu_timer_start;
    }

    switch (opmode)
    {
    case HWTIMER_MODE_PERIOD:
        u32OpMode = TTMR_PERIODIC_MODE;
        break;

    case HWTIMER_MODE_ONESHOT:
        u32OpMode = TTMR_ONESHOT_MODE;
        break;

    default:
        goto exit_nu_timer_start;
    }

    TTMR_SET_CMP_VALUE(psNuTmr->base, cnt);
    TTMR_SET_OPMODE(psNuTmr->base, u32OpMode);
    TTMR_EnableInt(psNuTmr->base);
    NVIC_EnableIRQ(psNuTmr->irqn);

    TTMR_Start(psNuTmr->base);

    ret = RT_EOK;

exit_nu_timer_start:

    return -(ret);
}

static void nu_timer_stop(rt_hwtimer_t *timer)
{
    nu_timer_t psNuTmr = NU_TTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    NVIC_DisableIRQ(psNuTmr->irqn);
    TTMR_DisableInt(psNuTmr->base);
    TTMR_Stop(psNuTmr->base);
    TTMR_ResetCounter(psNuTmr->base);
}

static rt_uint32_t nu_timer_count_get(rt_hwtimer_t *timer)
{
    nu_timer_t psNuTmr = NU_TTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    return TTMR_GetCounter(psNuTmr->base);
}

static rt_err_t nu_timer_control(rt_hwtimer_t *timer, rt_uint32_t cmd, void *args)
{
    rt_err_t ret = RT_EOK;
    nu_timer_t psNuTmr = NU_TTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    switch (cmd)
    {
    case HWTIMER_CTRL_FREQ_SET:
    {
        uint32_t clk;
        uint32_t pre;

        clk = TTMR_GetModuleClock(psNuTmr->base);
        pre = clk / *((uint32_t *)args) - 1;
        TTMR_SET_PRESCALE_VALUE(psNuTmr->base, pre);
        *((uint32_t *)args) = clk / (pre + 1) ;
    }
    break;

    case HWTIMER_CTRL_STOP:
        TTMR_Stop(psNuTmr->base);
        break;

    default:
        ret = RT_EINVAL;
        break;
    }

    return -(ret);
}

/**
 * All UART interrupt service routine
 */
static void nu_timer_isr(nu_timer_t psNuTmr)
{
    RT_ASSERT(psNuTmr != RT_NULL);

    if (TTMR_GetIntFlag(psNuTmr->base))
    {
        TTMR_ClearIntFlag(psNuTmr->base);
        rt_device_hwtimer_isr(&psNuTmr->parent);
    }
}

static int rt_hw_ttmr_init(void)
{
    int i;
    rt_err_t ret = RT_EOK;
    for (i = (TTMR_START + 1); i < TTMR_CNT; i++)
    {
        CLK_EnableModuleClock(nu_timer_arr[i].modid);

        SYS_ResetModule(nu_timer_arr[i].rstidx);

        /* Register Timer information. */
        nu_timer_arr[i].parent.info = &nu_timer_info;

        /* Register Timer operation. */
        nu_timer_arr[i].parent.ops = &nu_timer_ops;

        /* Register RT hwtimer device. */
        ret = rt_device_hwtimer_register(&nu_timer_arr[i].parent, nu_timer_arr[i].name, &nu_timer_arr[i]);
        RT_ASSERT(ret == RT_EOK);
    }
    return 0;
}

INIT_BOARD_EXPORT(rt_hw_ttmr_init);

#if defined(BSP_USING_TTMR0)
void TTMR0_IRQHandler(void)
{
    rt_interrupt_enter();

    nu_timer_isr((void *)&nu_timer_arr[TTMR0_IDX]);

    rt_interrupt_leave();
}
#endif

#if defined(BSP_USING_TTMR1)
void TTMR1_IRQHandler(void)
{
    rt_interrupt_enter();

    nu_timer_isr((void *)&nu_timer_arr[TTMR1_IDX]);

    rt_interrupt_leave();
}
#endif

#endif //#if (defined(BSP_USING_TTMR) && defined(RT_USING_HWTIMER))
