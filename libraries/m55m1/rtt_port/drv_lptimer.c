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

#if defined(BSP_USING_LPTMR) && defined(RT_USING_HWTIMER)

#include <rtdevice.h>
#include "NuMicro.h"

/* Private define ---------------------------------------------------------------*/
#define NU_LPTMR_DEVICE(timer) (nu_timer_t)(timer)

enum
{
    LPTMR_START = -1,
#if defined(BSP_USING_LPTMR0)
    LPTMR0_IDX,
#endif
#if defined(BSP_USING_LPTMR1)
    LPTMR1_IDX,
#endif
    LPTMR_CNT
};

/* Private typedef --------------------------------------------------------------*/
struct nu_timer
{
    rt_hwtimer_t  parent;
    char         *name;
    LPTMR_T      *base;
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
#if defined(BSP_USING_LPTMR0)
    { .name = "lptmr0", .base  = LPTMR0, .irqn = LPTMR0_IRQn, .rstidx = SYS_LPTMR0RST, .modid = LPTMR0_MODULE  },
#endif
#if defined(BSP_USING_LPTMR1)
    { .name = "lptmr1", .base  = LPTMR1, .irqn = LPTMR1_IRQn, .rstidx = SYS_LPTMR1RST, .modid = LPTMR1_MODULE  },
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
    nu_timer_t psNuTmr = NU_LPTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    if (1 == state)
    {
        uint32_t timer_clk;
        struct rt_hwtimer_info *info = &nu_timer_info;

        timer_clk = LPTMR_GetModuleClock(psNuTmr->base);
        info->maxfreq = timer_clk;
        info->minfreq = timer_clk / 256;
        LPTMR_Open(psNuTmr->base, LPTMR_ONESHOT_MODE, 1);
        LPTMR_EnableInt(psNuTmr->base);
        NVIC_EnableIRQ(psNuTmr->irqn);
    }
    else
    {
        NVIC_DisableIRQ(psNuTmr->irqn);
        LPTMR_DisableInt(psNuTmr->base);
        LPTMR_Close(psNuTmr->base);
    }
}

static rt_err_t nu_timer_start(rt_hwtimer_t *timer, rt_uint32_t cnt, rt_hwtimer_mode_t opmode)
{
    rt_err_t ret = RT_EINVAL;
    rt_uint32_t u32OpMode;

    nu_timer_t psNuTmr = NU_LPTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    if (cnt <= 1 || cnt > 0xFFFFFF)
    {
        goto exit_nu_timer_start;
    }

    switch (opmode)
    {
    case HWTIMER_MODE_PERIOD:
        u32OpMode = LPTMR_PERIODIC_MODE;
        break;

    case HWTIMER_MODE_ONESHOT:
        u32OpMode = LPTMR_ONESHOT_MODE;
        break;

    default:
        goto exit_nu_timer_start;
    }

    LPTMR_SET_CMP_VALUE(psNuTmr->base, cnt);
    LPTMR_SET_OPMODE(psNuTmr->base, u32OpMode);
    LPTMR_EnableInt(psNuTmr->base);
    NVIC_EnableIRQ(psNuTmr->irqn);

    LPTMR_Start(psNuTmr->base);

    ret = RT_EOK;

exit_nu_timer_start:

    return -(ret);
}

static void nu_timer_stop(rt_hwtimer_t *timer)
{
    nu_timer_t psNuTmr = NU_LPTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    NVIC_DisableIRQ(psNuTmr->irqn);
    LPTMR_DisableInt(psNuTmr->base);
    LPTMR_Stop(psNuTmr->base);
    LPTMR_ResetCounter(psNuTmr->base);
}

static rt_uint32_t nu_timer_count_get(rt_hwtimer_t *timer)
{
    nu_timer_t psNuTmr = NU_LPTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    return LPTMR_GetCounter(psNuTmr->base);
}

static rt_err_t nu_timer_control(rt_hwtimer_t *timer, rt_uint32_t cmd, void *args)
{
    rt_err_t ret = RT_EOK;
    nu_timer_t psNuTmr = NU_LPTMR_DEVICE(timer);
    RT_ASSERT(psNuTmr != RT_NULL);

    switch (cmd)
    {
    case HWTIMER_CTRL_FREQ_SET:
    {
        uint32_t clk;
        uint32_t pre;

        clk = LPTMR_GetModuleClock(psNuTmr->base);
        pre = clk / *((uint32_t *)args) - 1;
        LPTMR_SET_PRESCALE_VALUE(psNuTmr->base, pre);
        *((uint32_t *)args) = clk / (pre + 1) ;
    }
    break;

    case HWTIMER_CTRL_STOP:
        LPTMR_Stop(psNuTmr->base);
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

    if (LPTMR_GetIntFlag(psNuTmr->base))
    {
        LPTMR_ClearIntFlag(psNuTmr->base);
        rt_device_hwtimer_isr(&psNuTmr->parent);
    }
}

static int rt_hw_lptmr_init(void)
{
    int i;
    rt_err_t ret = RT_EOK;
    for (i = (LPTMR_START + 1); i < LPTMR_CNT; i++)
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

INIT_BOARD_EXPORT(rt_hw_lptmr_init);

#if defined(BSP_USING_LPTMR0)
void LPTMR0_IRQHandler(void)
{
    rt_interrupt_enter();

    nu_timer_isr((void *)&nu_timer_arr[LPTMR0_IDX]);

    rt_interrupt_leave();
}
#endif

#if defined(BSP_USING_LPTMR1)
void LPTMR1_IRQHandler(void)
{
    rt_interrupt_enter();

    nu_timer_isr((void *)&nu_timer_arr[LPTMR1_IDX]);

    rt_interrupt_leave();
}
#endif

#endif //#if (defined(BSP_USING_LPTMR) && defined(RT_USING_HWTIMER))
