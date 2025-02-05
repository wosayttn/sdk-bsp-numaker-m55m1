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

#if defined(BSP_USING_PDMA)

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <drv_pdma.h>
#include <nu_bitutil.h>
#include "drv_sys.h"
#include "drv_memheap.h"

#define LOG_TAG    "drv.pdma"
#undef  DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL      LOG_LVL_WARNING
#define DBG_COLOR
#include <rtdbg.h>

/* Private define ---------------------------------------------------------------*/
// RT_DEV_NAME_PREFIX pdma

#ifndef NU_PDMA_MEMFUN_ACTOR_MAX
    #define NU_PDMA_MEMFUN_ACTOR_MAX (4)
#endif

#define NU_PDMA_SG_TBL_MAXSIZE         (NU_PDMA_SG_LIMITED_DISTANCE/sizeof(DSCT_T))

#define NU_PDMA_CH_MAX    (PDMA_CNT*PDMA_CH_MAX)     /* Specify maximum channels of PDMA */
#define NU_PDMA_CH_Msk    ((1<<PDMA_CH_MAX)-1)

#define NU_PDMA_GET_MOD_CHIDX(ch)   ((ch&NU_PDMA_CHN_IDX_Msk)>>NU_PDMA_CHN_IDX_Pos)
#define NU_PDMA_GET_MOD_IDX(ch)     ((ch&NU_PDMA_IDX_Msk)>>NU_PDMA_IDX_Pos)
#define NU_PDMA_GET_BASE(ch)        ((PDMA_T *)nu_pdma_arr[NU_PDMA_GET_MOD_IDX(ch)].m_pvBase)

#define NU_PDMA_GET_ARRAY_IDX(iModChnID)  ((NU_PDMA_GET_IDX(iModChnID)*PDMA_CH_MAX)+NU_PDMA_GET_CHN_ID(iModChnID))


/* Private typedef --------------------------------------------------------------*/
struct nu_pdma_periph_ctl
{
    uint32_t     m_u32Peripheral;
    nu_pdma_memctrl_t  m_eMemCtl;
};
typedef struct nu_pdma_periph_ctl nu_pdma_periph_ctl_t;

struct nu_pdma_chn
{
    struct nu_pdma_chn_cb  m_sCB_Event;
    struct nu_pdma_chn_cb  m_sCB_Trigger;
    struct nu_pdma_chn_cb  m_sCB_Disable;

    nu_pdma_desc_t        *m_ppsSgtbl;
    uint32_t               m_u32WantedSGTblNum;

    uint32_t               m_u32EventFilter;
    uint32_t               m_u32IdleTimeout_us;
    nu_pdma_periph_ctl_t   m_spPeripCtl;
};
typedef struct nu_pdma_chn nu_pdma_chn_t;

struct nu_pdma_memfun_actor
{
    int         m_i32ModChnID;
    uint32_t    m_u32Result;
    rt_sem_t    m_psSemMemFun;
} ;
typedef struct nu_pdma_memfun_actor *nu_pdma_memfun_actor_t;

/* Private functions ------------------------------------------------------------*/
static int nu_pdma_peripheral_query(uint32_t u32PeriphType);
static void nu_pdma_init(void);
static void nu_pdma_channel_enable(int i32ModChnID);
static void nu_pdma_channel_disable(int i32ModChnID);
static void nu_pdma_channel_reset(int i32ModChnID);
static rt_err_t nu_pdma_timeout_set(int i32ModChnID, int i32Timeout_us);
static void nu_pdma_periph_ctrl_fill(int i32ModChnID, int i32CtlPoolIdx);
static rt_size_t nu_pdma_memfun(void *dest, void *src, uint32_t u32DataWidth, unsigned int u32TransferCnt, nu_pdma_memctrl_t eMemCtl);
static void nu_pdma_memfun_cb(void *pvUserData, uint32_t u32Events);
static void nu_pdma_memfun_actor_init(void);
static int nu_pdma_memfun_employ(void);
int nu_pdma_non_transfer_count_get(int32_t i32ModChnID);

/* Private variables ------------------------------------------------------------*/
static volatile int nu_pdma_inited = 0;
static volatile uint32_t nu_pdma_chn_mask_arr[PDMA_CNT] = {0};
static nu_pdma_chn_t nu_pdma_chn_arr[NU_PDMA_CH_MAX];
static volatile uint32_t nu_pdma_memfun_actor_mask = 0;
static volatile uint32_t nu_pdma_memfun_actor_maxnum = 0;
static rt_sem_t nu_pdma_memfun_actor_pool_sem = RT_NULL;
static rt_mutex_t nu_pdma_memfun_actor_pool_lock = RT_NULL;

const static struct nu_module nu_pdma_arr[] =
{
    {
        .name = "pdma0",
        .m_pvBase = (void *)PDMA0,
        .u32RstId = SYS_PDMA0RST,
        .eIRQn = PDMA0_IRQn
    },
    {
        .name = "pdma1",
        .m_pvBase = (void *)PDMA1,
        .u32RstId = SYS_PDMA1RST,
        .eIRQn = PDMA1_IRQn
    },
    {
        .name = "lppdma",
        .m_pvBase = (void *)LPPDMA,
        .u32RstId = SYS_LPPDMA0RST,
        .eIRQn = LPPDMA_IRQn
    }
};

static const nu_pdma_periph_ctl_t g_nu_pdma_peripheral_ctl_pool[ ] =
{
    // M2M
    { PDMA_MEM,   eMemCtl_SrcInc_DstInc },

    // M2P
    { PDMA_UART0_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART1_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART2_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART3_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART4_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART5_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART6_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART7_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART8_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_UART9_TX, eMemCtl_SrcInc_DstFix },

    { PDMA_USCI0_TX, eMemCtl_SrcInc_DstFix },

    { PDMA_QSPI0_TX, eMemCtl_SrcInc_DstFix },
    { PDMA_QSPI1_TX, eMemCtl_SrcInc_DstFix },

    { PDMA_SPI0_TX,  eMemCtl_SrcInc_DstFix },
    { PDMA_SPI1_TX,  eMemCtl_SrcInc_DstFix },
    { PDMA_SPI2_TX,  eMemCtl_SrcInc_DstFix },
    { PDMA_SPI3_TX,  eMemCtl_SrcInc_DstFix },

    { PDMA_I2S0_TX,  eMemCtl_SrcInc_DstFix },
    { PDMA_I2S1_TX,  eMemCtl_SrcInc_DstFix },

    // P2M
    { PDMA_UART0_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART1_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART2_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART3_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART4_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART5_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART6_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART7_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART8_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_UART9_RX, eMemCtl_SrcFix_DstInc },

    { PDMA_USCI0_RX, eMemCtl_SrcFix_DstInc },

    { PDMA_QSPI0_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_QSPI1_RX, eMemCtl_SrcFix_DstInc },

    { PDMA_SPI0_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_SPI1_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_SPI2_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_SPI3_RX, eMemCtl_SrcFix_DstInc },

    { PDMA_I2S0_RX, eMemCtl_SrcFix_DstInc },
    { PDMA_I2S1_RX, eMemCtl_SrcFix_DstInc },


    // LPPDMA
    // M2M
    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_MEM),        eMemCtl_SrcInc_DstInc },

    // M2P
    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_LPUART0_TX),  eMemCtl_SrcInc_DstFix },
    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_LPSPI0_TX),   eMemCtl_SrcInc_DstFix },

    // P2M
    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_LPUART0_RX),  eMemCtl_SrcFix_DstInc },
    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_LPSPI0_RX),   eMemCtl_SrcFix_DstInc },

    { NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_DMIC0_RX),    eMemCtl_SrcFix_DstInc },
};
#define NU_PERIPHERAL_SIZE ( sizeof(g_nu_pdma_peripheral_ctl_pool) / sizeof(nu_pdma_periph_ctl_t) )

static struct nu_pdma_memfun_actor nu_pdma_memfun_actor_arr[NU_PDMA_MEMFUN_ACTOR_MAX];

static int nu_pdma_check_is_lppdma(uint32_t i32ModChnID)
{
    uint32_t mod_idx = NU_PDMA_GET_MOD_IDX(i32ModChnID);

    return (mod_idx == LPPDMA_IDX);
}

static int nu_pdma_check_is_nonallocated(uint32_t i32ModChnID)
{
    uint32_t mod_idx = NU_PDMA_GET_MOD_IDX(i32ModChnID);

    RT_ASSERT(mod_idx < PDMA_CNT);
    return !(nu_pdma_chn_mask_arr[mod_idx] & (1 << NU_PDMA_GET_MOD_CHIDX(i32ModChnID)));
}

static int nu_pdma_peripheral_query(uint32_t u32PeriphType)
{
    int idx = 0;

    while (idx < NU_PERIPHERAL_SIZE)
    {
        if (g_nu_pdma_peripheral_ctl_pool[idx].m_u32Peripheral == u32PeriphType)
            return idx;
        idx++;
    }

    // Not such peripheral
    return -1;
}

static void nu_pdma_periph_ctrl_fill(int i32ModChnID, int i32CtlPoolIdx)
{
    nu_pdma_chn_t *psPdmaChann = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)];

    psPdmaChann->m_spPeripCtl.m_u32Peripheral = NU_PDMA_GET_REQ_SRC_ID(g_nu_pdma_peripheral_ctl_pool[i32CtlPoolIdx].m_u32Peripheral);
    psPdmaChann->m_spPeripCtl.m_eMemCtl = g_nu_pdma_peripheral_ctl_pool[i32CtlPoolIdx].m_eMemCtl;
}

/**
 * Hardware PDMA Initialization
 */
static void nu_pdma_init(void)
{
    int i, latest = 0;
    if (nu_pdma_inited)
        return;

    rt_memset(nu_pdma_chn_arr, 0x00, sizeof(nu_pdma_chn_arr));

    for (i = (PDMA_START + 1); i < PDMA_CNT; i++)
    {
        PDMA_T *psPDMA = (PDMA_T *)nu_pdma_arr[i].m_pvBase;
        nu_pdma_chn_mask_arr[i] = ~(NU_PDMA_CH_Msk);

        SYS_ResetModule(nu_pdma_arr[i].u32RstId);

        /* Initialize PDMA setting */
        PDMA_Open(psPDMA, PDMA_CH_Msk);
        PDMA_Close(psPDMA);

        /* Enable PDMA interrupt */
        NVIC_EnableIRQ(nu_pdma_arr[i].eIRQn);
    }

    nu_pdma_inited = 1;
}

static inline void nu_pdma_channel_enable(int i32ModChnID)
{
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);
    int u32ModChannId = NU_PDMA_GET_MOD_CHIDX(i32ModChnID);

    /* Clean descriptor table control register. */
    PDMA->DSCT[u32ModChannId].CTL = 0UL;

    /* Enable the channel */
    PDMA->CHCTL |= (1 << u32ModChannId);
}

static inline void nu_pdma_channel_disable(int i32ModChnID)
{
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);

    PDMA->CHCTL &= ~(1 << NU_PDMA_GET_MOD_CHIDX(i32ModChnID));
}

static inline void nu_pdma_channel_reset(int i32ModChnID)
{
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);
    int u32ModChannId = NU_PDMA_GET_MOD_CHIDX(i32ModChnID);

    PDMA->CHRST = (1 << u32ModChannId);

    /* Wait for cleared channel CHCTL. */
    while ((PDMA->CHCTL & (1 << u32ModChannId)));
}

void nu_pdma_channel_reset_user(int i32ChannID)
{
    nu_pdma_channel_reset(i32ChannID);
}

void nu_pdma_channel_terminate(int i32ModChnID)
{
    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_pdma_channel_terminate;

    LOG_I("[%s] %d", __func__, i32ModChnID);

    /* Reset specified channel. */
    nu_pdma_channel_reset(i32ModChnID);

    /* Enable specified channel after reset. */
    nu_pdma_channel_enable(i32ModChnID);

exit_pdma_channel_terminate:

    return;
}

static rt_err_t nu_pdma_timeout_set(int i32ModChnID, int i32Timeout_us)
{
    rt_err_t ret = RT_EINVAL;
    PDMA_T *PDMA = NULL;
    uint32_t u32ModChannId;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_timeout_set;
    else if (nu_pdma_check_is_lppdma(i32ModChnID))
        goto exit_nu_pdma_timeout_set;

    PDMA = NU_PDMA_GET_BASE(i32ModChnID);

    u32ModChannId = NU_PDMA_GET_MOD_CHIDX(i32ModChnID);

    nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_u32IdleTimeout_us = i32Timeout_us;

    if (i32Timeout_us)
    {
        uint32_t u32ToClk_Max = 1000000 / (CLK_GetHCLK0Freq() / (1 << 8));
        uint32_t u32Divider     = (i32Timeout_us / u32ToClk_Max) / (1 << 16);
        uint32_t u32TOutCnt     = (i32Timeout_us / u32ToClk_Max) % (1 << 16);

        LOG_I("CLK_GetHCLK0Freq(): %d, u32ToClk_Max: %d, u32Divider: %d, u32TOutCnt:%d",
              CLK_GetHCLK0Freq(), u32ToClk_Max, u32Divider, u32TOutCnt);

        PDMA_DisableTimeout(PDMA,  1 << u32ModChannId);
        PDMA_EnableInt(PDMA, u32ModChannId, PDMA_INT_TIMEOUT);    // Interrupt type

        if (u32Divider > 7)
        {
            u32Divider = 7;
            u32TOutCnt = (1 << 16);
        }

        if (u32ModChannId < 8)
            PDMA->TOUTPSC0_7 = (PDMA->TOUTPSC0_7 & ~(0x7ul << (PDMA_TOUTPSC0_7_TOUTPSC0_Pos * u32ModChannId))) | (u32Divider << (PDMA_TOUTPSC0_7_TOUTPSC0_Pos * u32ModChannId));
        else
            PDMA->TOUTPSC8_15 = (PDMA->TOUTPSC8_15 & ~(0x7ul << (PDMA_TOUTPSC8_15_TOUTPSC8_Pos * (u32ModChannId % 8)))) | (u32Divider << (PDMA_TOUTPSC8_15_TOUTPSC8_Pos * (u32ModChannId % 8)));

        PDMA_SetTimeOut(PDMA,  u32ModChannId, 1, u32TOutCnt);

        ret = RT_EOK;
    }
    else
    {
        PDMA_DisableInt(PDMA, u32ModChannId, PDMA_INT_TIMEOUT);    // Interrupt type
        PDMA_DisableTimeout(PDMA,  1 << u32ModChannId);
    }

exit_nu_pdma_timeout_set:

    return -(ret);
}

int nu_pdma_channel_allocate(int32_t i32PeripType)
{
    int i32PeripCtlIdx, j;

    nu_pdma_init();

    if ((j = NU_PDMA_GET_IDX(i32PeripType)) >= PDMA_CNT)
        goto exit_nu_pdma_channel_allocate;

    if ((i32PeripCtlIdx = nu_pdma_peripheral_query(i32PeripType)) < 0)
        goto exit_nu_pdma_channel_allocate;

    for (; j < PDMA_CNT; j++)
    {
        pdma_chid_t mod_chn_id;

        /* Find the position of first '0' in nu_pdma_chn_mask_arr[j]. */
        mod_chn_id.u16ChnIdx = nu_cto(nu_pdma_chn_mask_arr[j]);
        if (mod_chn_id.u16ChnIdx < PDMA_CH_MAX)
        {
            mod_chn_id.u16ModIdx = j;

            nu_pdma_chn_mask_arr[j] |= (1 << mod_chn_id.u16ChnIdx);

            rt_memset(&nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(mod_chn_id.u32ChID)], 0x00, sizeof(nu_pdma_chn_t));

            /* Set idx number of g_nu_pdma_peripheral_ctl_pool */
            nu_pdma_periph_ctrl_fill(mod_chn_id.u32ChID, i32PeripCtlIdx);

            /* Reset channel */
            nu_pdma_channel_terminate(mod_chn_id.u32ChID);

            return mod_chn_id.u32ChID;
        }
    }

exit_nu_pdma_channel_allocate:

    // No channel available
    return -(RT_ERROR);
}

rt_err_t nu_pdma_channel_free(int i32ModChnID)
{
    rt_err_t ret = RT_EINVAL;

    if (!nu_pdma_inited)
        goto exit_nu_pdma_channel_free;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_channel_free;

    if ((NU_PDMA_GET_MOD_IDX(i32ModChnID) < NU_PDMA_CH_MAX) && (NU_PDMA_GET_MOD_IDX(i32ModChnID) >= 0))
    {
        nu_pdma_chn_mask_arr[NU_PDMA_GET_MOD_IDX(i32ModChnID)] &= ~(1 << NU_PDMA_GET_MOD_CHIDX(i32ModChnID));
        nu_pdma_channel_disable(i32ModChnID);
        ret =  RT_EOK;
    }

exit_nu_pdma_channel_free:

    return -(ret);
}

rt_err_t nu_pdma_filtering_set(int i32ModChnID, uint32_t u32EventFilter)
{
    rt_err_t ret = RT_EINVAL;
    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_filtering_set;

    nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_u32EventFilter = u32EventFilter;

    ret = RT_EOK;

exit_nu_pdma_filtering_set:

    return -(ret) ;
}

uint32_t nu_pdma_filtering_get(int i32ModChnID)
{
    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_filtering_get;

    return nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_u32EventFilter;

exit_nu_pdma_filtering_get:

    return 0;
}

rt_err_t nu_pdma_callback_register(int i32ModChnID, nu_pdma_chn_cb_t psChnCb)
{
    rt_err_t ret = RT_EINVAL;
    nu_pdma_chn_cb_t psChnCb_Current = RT_NULL;

    RT_ASSERT(psChnCb != RT_NULL);

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_callback_register;

    switch (psChnCb->m_eCBType)
    {
    case eCBType_Event:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Event;
        break;
    case eCBType_Trigger:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Trigger;
        break;
    case eCBType_Disable:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Disable;
        break;
    default:
        goto exit_nu_pdma_callback_register;
    }

    psChnCb_Current->m_pfnCBHandler = psChnCb->m_pfnCBHandler;
    psChnCb_Current->m_pvUserData = psChnCb->m_pvUserData;

    ret = RT_EOK;

exit_nu_pdma_callback_register:

    return -(ret) ;
}

nu_pdma_cb_handler_t nu_pdma_callback_hijack(int i32ModChnID, nu_pdma_cbtype_t eCBType, nu_pdma_chn_cb_t psChnCb_Hijack)
{
    nu_pdma_chn_cb_t psChnCb_Current = RT_NULL;
    struct nu_pdma_chn_cb sChnCB_Tmp;

    RT_ASSERT(psChnCb_Hijack != NULL);

    sChnCB_Tmp.m_pfnCBHandler = RT_NULL;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_callback_hijack;

    switch (eCBType)
    {
    case eCBType_Event:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Event;
        break;
    case eCBType_Trigger:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Trigger;
        break;
    case eCBType_Disable:
        psChnCb_Current = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_sCB_Disable;
        break;
    default:
        goto exit_nu_pdma_callback_hijack;
    }

    /* Backup */
    sChnCB_Tmp.m_pfnCBHandler = psChnCb_Current->m_pfnCBHandler;
    sChnCB_Tmp.m_pvUserData = psChnCb_Current->m_pvUserData;

    /* Update */
    psChnCb_Current->m_pfnCBHandler = psChnCb_Hijack->m_pfnCBHandler;
    psChnCb_Current->m_pvUserData = psChnCb_Hijack->m_pvUserData;

    /* Restore */
    psChnCb_Hijack->m_pfnCBHandler = sChnCB_Tmp.m_pfnCBHandler;
    psChnCb_Hijack->m_pvUserData = sChnCB_Tmp.m_pvUserData;

exit_nu_pdma_callback_hijack:

    return sChnCB_Tmp.m_pfnCBHandler;
}

int nu_pdma_non_transfer_count_get(int32_t i32ModChnID)
{
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);
    return ((PDMA->DSCT[NU_PDMA_GET_MOD_CHIDX(i32ModChnID)].CTL & PDMA_DSCT_CTL_TXCNT_Msk) >> PDMA_DSCT_CTL_TXCNT_Pos) + 1;
}

int nu_pdma_transferred_byte_get(int32_t i32ModChnID, int32_t i32TriggerByteLen)
{
    int i32BitWidth = 0;
    int cur_txcnt = 0;
    PDMA_T *PDMA;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_transferred_byte_get;

    PDMA = NU_PDMA_GET_BASE(i32ModChnID);

    i32BitWidth = PDMA->DSCT[NU_PDMA_GET_MOD_CHIDX(i32ModChnID)].CTL & PDMA_DSCT_CTL_TXWIDTH_Msk;
    i32BitWidth = (i32BitWidth == PDMA_WIDTH_8) ? 1 : (i32BitWidth == PDMA_WIDTH_16) ? 2 : (i32BitWidth == PDMA_WIDTH_32) ? 4 : 0;

    cur_txcnt = nu_pdma_non_transfer_count_get(i32ModChnID);

    return (i32TriggerByteLen - (cur_txcnt) * i32BitWidth);

exit_nu_pdma_transferred_byte_get:

    return -1;
}

nu_pdma_memctrl_t nu_pdma_channel_memctrl_get(int i32ModChnID)
{
    nu_pdma_memctrl_t eMemCtrl = eMemCtl_Undefined;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_channel_memctrl_get;

    eMemCtrl = nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_spPeripCtl.m_eMemCtl;

exit_nu_pdma_channel_memctrl_get:

    return eMemCtrl;
}

rt_err_t nu_pdma_channel_memctrl_set(int i32ModChnID, nu_pdma_memctrl_t eMemCtrl)
{
    rt_err_t ret = RT_EINVAL;
    nu_pdma_chn_t *psPdmaChann = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)];

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_channel_memctrl_set;
    else if ((eMemCtrl < eMemCtl_SrcFix_DstFix) || (eMemCtrl > eMemCtl_SrcInc_DstInc))
        goto exit_nu_pdma_channel_memctrl_set;

    /* PDMA_MEM/SAR_FIX/BURST mode is not supported. */
    if ((psPdmaChann->m_spPeripCtl.m_u32Peripheral == PDMA_MEM) &&
            ((eMemCtrl == eMemCtl_SrcFix_DstInc) || (eMemCtrl == eMemCtl_SrcFix_DstFix)))
        goto exit_nu_pdma_channel_memctrl_set;

    nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_spPeripCtl.m_eMemCtl = eMemCtrl;

    ret = RT_EOK;

exit_nu_pdma_channel_memctrl_set:

    return -(ret);
}

static void nu_pdma_channel_memctrl_fill(nu_pdma_memctrl_t eMemCtl, uint32_t *pu32SrcCtl, uint32_t *pu32DstCtl)
{
    switch ((int)eMemCtl)
    {
    case eMemCtl_SrcFix_DstFix:
        *pu32SrcCtl = PDMA_SAR_FIX;
        *pu32DstCtl = PDMA_DAR_FIX;
        break;
    case eMemCtl_SrcFix_DstInc:
        *pu32SrcCtl = PDMA_SAR_FIX;
        *pu32DstCtl = PDMA_DAR_INC;
        break;
    case eMemCtl_SrcInc_DstFix:
        *pu32SrcCtl = PDMA_SAR_INC;
        *pu32DstCtl = PDMA_DAR_FIX;
        break;
    case eMemCtl_SrcInc_DstInc:
        *pu32SrcCtl = PDMA_SAR_INC;
        *pu32DstCtl = PDMA_DAR_INC;
        break;
    default:
        break;
    }
}

/* This is for Scatter-gather DMA. */
rt_err_t nu_pdma_desc_setup(int i32ModChnID, nu_pdma_desc_t dma_desc, uint32_t u32DataWidth, uint32_t u32AddrSrc,
                            uint32_t u32AddrDst, int32_t i32TransferCnt, nu_pdma_desc_t next, uint32_t u32BeSilent)
{
    nu_pdma_periph_ctl_t *psPeriphCtl = NULL;
    PDMA_T *PDMA = NULL;

    uint32_t u32SrcCtl = 0;
    uint32_t u32DstCtl = 0;

    rt_err_t ret = RT_EINVAL;

    if (!dma_desc)
        goto exit_nu_pdma_desc_setup;
    else if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_desc_setup;
    else if (!(u32DataWidth == 8 || u32DataWidth == 16 || u32DataWidth == 32))
        goto exit_nu_pdma_desc_setup;
    else if ((u32AddrSrc % (u32DataWidth / 8)) || (u32AddrDst % (u32DataWidth / 8)))
        goto exit_nu_pdma_desc_setup;
    else if (i32TransferCnt > NU_PDMA_MAX_TXCNT)
        goto exit_nu_pdma_desc_setup;

    PDMA = NU_PDMA_GET_BASE(i32ModChnID);

    psPeriphCtl = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_spPeripCtl;

    nu_pdma_channel_memctrl_fill(psPeriphCtl->m_eMemCtl, &u32SrcCtl, &u32DstCtl);

    dma_desc->CTL = ((i32TransferCnt - 1) << PDMA_DSCT_CTL_TXCNT_Pos) |
                    ((u32DataWidth == 8) ? PDMA_WIDTH_8 : (u32DataWidth == 16) ? PDMA_WIDTH_16 : PDMA_WIDTH_32) |
                    u32SrcCtl |
                    u32DstCtl |
                    PDMA_OP_BASIC;

    dma_desc->SA = u32AddrSrc;
    dma_desc->DA = u32AddrDst;
    dma_desc->NEXT = 0;  /* Terminating node by default. */

    if (psPeriphCtl->m_u32Peripheral == PDMA_MEM)
    {
        /* For M2M transfer */
        dma_desc->CTL |= (PDMA_REQ_BURST | PDMA_BURST_32);
    }
    else
    {
        /* For P2M and M2P transfer */
        dma_desc->CTL |= (PDMA_REQ_SINGLE);
    }

    if (next)
    {
        /* Link to Next and modify to scatter-gather DMA mode. */
        dma_desc->CTL = (dma_desc->CTL & ~PDMA_DSCT_CTL_OPMODE_Msk) | PDMA_OP_SCATTER;
        dma_desc->NEXT = (uint32_t)next;
    }

    /* Be silent */
    if (u32BeSilent)
        dma_desc->CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    ret = RT_EOK;

exit_nu_pdma_desc_setup:

    return -(ret);
}

rt_err_t nu_pdma_sgtbls_allocate(int i32ModChnID, nu_pdma_desc_t *ppsSgtbls, int num)
{
    int i;
    nu_pdma_desc_t psSgTblHead;

    RT_ASSERT(ppsSgtbls != NULL);
    RT_ASSERT(num > 0);

    if (nu_pdma_check_is_lppdma(i32ModChnID))
    {
        psSgTblHead = rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_LPSRAM), RT_ALIGN(sizeof(DSCT_T) * num, 32));
    }
    else
    {
        psSgTblHead = (nu_pdma_desc_t) rt_malloc_align(RT_ALIGN(sizeof(DSCT_T) * num, 32), 32);
    }
    RT_ASSERT(psSgTblHead != RT_NULL);

    rt_memset((void *)psSgTblHead, 0, RT_ALIGN(sizeof(DSCT_T) * num, 32));

    for (i = 0; i < num; i++)
        ppsSgtbls[i] = &psSgTblHead[i];

    return RT_EOK;
}

void nu_pdma_sgtbls_free(int i32ModChnID, nu_pdma_desc_t *ppsSgtbls, int num)
{
    nu_pdma_desc_t psSgTblHead;

    RT_ASSERT(ppsSgtbls != NULL);
    psSgTblHead = *ppsSgtbls;
    RT_ASSERT(psSgTblHead != NULL);

    if (nu_pdma_check_is_lppdma(i32ModChnID))
    {
        rt_memheap_free(psSgTblHead);
    }
    else
    {
        rt_free_align(psSgTblHead);
    }
}

static void _nu_pdma_transfer(int i32ModChnID, uint32_t u32Peripheral, nu_pdma_desc_t head, uint32_t u32IdleTimeout_us)
{
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);
    nu_pdma_chn_t *psPdmaChann = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)];

#if defined(RT_USING_CACHE)
    /* Writeback data in dcache to memory before transferring. */
    {
        static uint32_t bNonCacheAlignedWarning = 1;
        nu_pdma_desc_t next = head;
        while (next != NULL)
        {
            uint32_t u32TxCnt     = ((next->CTL & PDMA_DSCT_CTL_TXCNT_Msk) >> PDMA_DSCT_CTL_TXCNT_Pos) + 1;
            uint32_t u32DataWidth = (1 << ((next->CTL & PDMA_DSCT_CTL_TXWIDTH_Msk) >> PDMA_DSCT_CTL_TXWIDTH_Pos));
            uint32_t u32SrcCtl    = (next->CTL & PDMA_DSCT_CTL_SAINC_Msk);
            uint32_t u32DstCtl    = (next->CTL & PDMA_DSCT_CTL_DAINC_Msk);
            uint32_t u32FlushLen  = u32TxCnt * u32DataWidth;

            /* Flush Src buffer into memory. */
            if ((u32SrcCtl == PDMA_SAR_INC)) // for M2P, M2M
                rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH_INVALIDATE, (void *)next->SA, (int32_t)u32FlushLen);

            /* Flush Dst buffer into memory. */
            if ((u32DstCtl == PDMA_DAR_INC)) // for P2M, M2M
                rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH_INVALIDATE, (void *)next->DA, (int32_t)u32FlushLen);

            /* Flush descriptor into memory */
            if (!((uint32_t)next & BIT31))
                rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH_INVALIDATE, (void *)next, sizeof(DSCT_T));

            if (bNonCacheAlignedWarning)
            {
                if ((u32FlushLen & (DCACHE_LINE_SIZE - 1)) ||
                        (next->SA & (DCACHE_LINE_SIZE - 1)) ||
                        (next->DA & (DCACHE_LINE_SIZE - 1)) ||
                        ((uint32_t)next & (DCACHE_LINE_SIZE - 1)))
                {
                    /*
                        Race-condition avoidance between DMA-transferring and DCache write-back:
                        Source, destination, DMA descriptor address and length should be aligned at len(CACHE_LINE_SIZE)
                    */
                    bNonCacheAlignedWarning = 0;
                }
            }

            next = (nu_pdma_desc_t)next->NEXT;

            if (next == head) break;
        }
    }
#endif

    PDMA_DisableTimeout(PDMA,  1 << NU_PDMA_GET_MOD_CHIDX(i32ModChnID));

    PDMA_EnableInt(PDMA, NU_PDMA_GET_MOD_CHIDX(i32ModChnID), PDMA_INT_TRANS_DONE);

    nu_pdma_timeout_set(i32ModChnID, u32IdleTimeout_us);

    /* Set scatter-gather mode and head */
    /* Take care the head structure, you should make sure cache-coherence. */
    PDMA_SetTransferMode(PDMA,
                         NU_PDMA_GET_MOD_CHIDX(i32ModChnID),
                         u32Peripheral,
                         (head->NEXT != 0) ? 1 : 0,
                         (uint32_t)head);

    /* If peripheral is M2M, trigger it. */
    if (u32Peripheral == PDMA_MEM)
    {
        PDMA_Trigger(PDMA, NU_PDMA_GET_MOD_CHIDX(i32ModChnID));
    }
    else if (psPdmaChann->m_sCB_Trigger.m_pfnCBHandler)
    {
        psPdmaChann->m_sCB_Trigger.m_pfnCBHandler(psPdmaChann->m_sCB_Trigger.m_pvUserData, psPdmaChann->m_sCB_Trigger.m_u32Reserved);
    }
}

static void _nu_pdma_sgtbls_free(nu_pdma_chn_t *psPdmaChann)
{
    if (psPdmaChann->m_ppsSgtbl)
    {
        rt_free_align((void *)psPdmaChann->m_ppsSgtbl);
        psPdmaChann->m_ppsSgtbl = RT_NULL;
        psPdmaChann->m_u32WantedSGTblNum = 0;
    }
}

static rt_err_t _nu_pdma_transfer_chain(int i32ModChnID, uint32_t u32DataWidth, uint32_t u32AddrSrc, uint32_t u32AddrDst, uint32_t u32TransferCnt, uint32_t u32IdleTimeout_us)
{
    int i = 0;
    rt_err_t ret = RT_ERROR;
    nu_pdma_periph_ctl_t *psPeriphCtl = NULL;
    nu_pdma_chn_t *psPdmaChann = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)];

    nu_pdma_memctrl_t eMemCtl = nu_pdma_channel_memctrl_get(i32ModChnID);

    rt_uint32_t u32Offset = 0;
    rt_uint32_t u32TxCnt = 0;

    psPeriphCtl = &psPdmaChann->m_spPeripCtl;

    if (psPdmaChann->m_u32WantedSGTblNum != (u32TransferCnt / NU_PDMA_MAX_TXCNT + 1))
    {
        if (psPdmaChann->m_u32WantedSGTblNum > 0)
        {
            nu_pdma_sgtbls_free(i32ModChnID, psPdmaChann->m_ppsSgtbl, psPdmaChann->m_u32WantedSGTblNum);
            _nu_pdma_sgtbls_free(psPdmaChann);
        }

        psPdmaChann->m_u32WantedSGTblNum = u32TransferCnt / NU_PDMA_MAX_TXCNT + 1;

        psPdmaChann->m_ppsSgtbl = (nu_pdma_desc_t *)rt_malloc_align(sizeof(nu_pdma_desc_t) * psPdmaChann->m_u32WantedSGTblNum, 32);
        if (!psPdmaChann->m_ppsSgtbl)
            goto exit__nu_pdma_transfer_chain;

        ret = nu_pdma_sgtbls_allocate(i32ModChnID, psPdmaChann->m_ppsSgtbl, psPdmaChann->m_u32WantedSGTblNum);
        if (ret != RT_EOK)
            goto exit__nu_pdma_transfer_chain;
    }

    for (i = 0; i < psPdmaChann->m_u32WantedSGTblNum; i++)
    {
        u32TxCnt = (u32TransferCnt > NU_PDMA_MAX_TXCNT) ? NU_PDMA_MAX_TXCNT : u32TransferCnt;

        ret = nu_pdma_desc_setup(i32ModChnID,
                                 psPdmaChann->m_ppsSgtbl[i],
                                 u32DataWidth,
                                 (eMemCtl & 0x2ul) ? u32AddrSrc + u32Offset : u32AddrSrc, /* Src address is Inc or not. */
                                 (eMemCtl & 0x1ul) ? u32AddrDst + u32Offset : u32AddrDst, /* Dst address is Inc or not. */
                                 u32TxCnt,
                                 ((i + 1) == psPdmaChann->m_u32WantedSGTblNum) ? RT_NULL : psPdmaChann->m_ppsSgtbl[i + 1],
                                 ((i + 1) == psPdmaChann->m_u32WantedSGTblNum) ? 0 : 1); // Silent, w/o TD interrupt

        if (ret != RT_EOK)
            goto exit__nu_pdma_transfer_chain;

        u32TransferCnt -= u32TxCnt;
        u32Offset += (u32TxCnt * u32DataWidth / 8);
    }

    _nu_pdma_transfer(i32ModChnID, psPeriphCtl->m_u32Peripheral, psPdmaChann->m_ppsSgtbl[0], u32IdleTimeout_us);

    ret = RT_EOK;

    return ret;

exit__nu_pdma_transfer_chain:

    _nu_pdma_sgtbls_free(psPdmaChann);

    return -(ret);
}

rt_err_t nu_pdma_transfer(int i32ModChnID, uint32_t u32DataWidth, uint32_t u32AddrSrc, uint32_t u32AddrDst, uint32_t u32TransferCnt, uint32_t u32IdleTimeout_us)
{
    rt_err_t ret = RT_EINVAL;
    PDMA_T *PDMA = NU_PDMA_GET_BASE(i32ModChnID);
    nu_pdma_desc_t head;
    nu_pdma_chn_t *psPdmaChann;

    nu_pdma_periph_ctl_t *psPeriphCtl = NULL;

    if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_transfer;
    else if (!u32TransferCnt)
        goto exit_nu_pdma_transfer;
    else if (u32TransferCnt > NU_PDMA_MAX_TXCNT)
        return _nu_pdma_transfer_chain(i32ModChnID, u32DataWidth, u32AddrSrc, u32AddrDst, u32TransferCnt, u32IdleTimeout_us);

    psPdmaChann = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)];
    psPeriphCtl = &psPdmaChann->m_spPeripCtl;

    head = &PDMA->DSCT[NU_PDMA_GET_MOD_CHIDX(i32ModChnID)];

    ret = nu_pdma_desc_setup(i32ModChnID,
                             head,
                             u32DataWidth,
                             u32AddrSrc,
                             u32AddrDst,
                             u32TransferCnt,
                             RT_NULL,
                             0);
    if (ret != RT_EOK)
        goto exit_nu_pdma_transfer;

    _nu_pdma_transfer(i32ModChnID, psPeriphCtl->m_u32Peripheral, head, u32IdleTimeout_us);

    ret = RT_EOK;

exit_nu_pdma_transfer:

    return -(ret);
}

rt_err_t nu_pdma_sg_transfer(int i32ModChnID, nu_pdma_desc_t head, uint32_t u32IdleTimeout_us)
{
    rt_err_t ret = RT_EINVAL;
    nu_pdma_periph_ctl_t *psPeriphCtl = NULL;

    if (!head)
        goto exit_nu_pdma_sg_transfer;
    else if (nu_pdma_check_is_nonallocated(i32ModChnID))
        goto exit_nu_pdma_sg_transfer;

    psPeriphCtl = &nu_pdma_chn_arr[NU_PDMA_GET_ARRAY_IDX(i32ModChnID)].m_spPeripCtl;

    _nu_pdma_transfer(i32ModChnID, psPeriphCtl->m_u32Peripheral, head, u32IdleTimeout_us);

    ret = RT_EOK;

exit_nu_pdma_sg_transfer:

    return -(ret);
}

void pdma_isr(PDMA_T *PDMA, int idx)
{
    int i;

    uint32_t intsts = PDMA_GET_INT_STATUS(PDMA);
    uint32_t abtsts = PDMA_GET_ABORT_STS(PDMA);
    uint32_t tdsts  = PDMA_GET_TD_STS(PDMA);
    uint32_t unalignsts  = PDMA_GET_ALIGN_STS(PDMA);
    uint32_t reqto  = intsts & PDMA_INTSTS_REQTOFn_Msk;
    uint32_t reqto_ch = (reqto >> PDMA_INTSTS_REQTOFn_Pos);

    int allch_sts = (reqto_ch | tdsts | abtsts | unalignsts);

    // Abort
    if (intsts & PDMA_INTSTS_ABTIF_Msk)
    {
        // Clear all Abort flags
        PDMA_CLR_ABORT_FLAG(PDMA, abtsts);
    }

    // Transfer done
    if (intsts & PDMA_INTSTS_TDIF_Msk)
    {
        // Clear all transfer done flags
        PDMA_CLR_TD_FLAG(PDMA, tdsts);
    }

    // Unaligned
    if (intsts & PDMA_INTSTS_ALIGNF_Msk)
    {
        // Clear all Unaligned flags
        PDMA_CLR_ALIGN_FLAG(PDMA, unalignsts);
    }

    // Timeout
    if (reqto)
    {
        // Clear all Timeout flags
        PDMA->INTSTS = reqto;
    }

    // Find the position of first '1' in allch_sts.
    while ((i = nu_ctz(allch_sts)) < PDMA_CH_MAX)
    {
        int module_id = idx;
        int j = i + (module_id * PDMA_CH_MAX);
        int ch_mask = (1 << i);

        if (nu_pdma_chn_mask_arr[module_id] & ch_mask)
        {
            int ch_event = 0;
            nu_pdma_chn_t *dma_chn = &nu_pdma_chn_arr[j];

            if (dma_chn->m_sCB_Event.m_pfnCBHandler)
            {
                if (abtsts & ch_mask)
                {
                    ch_event |= NU_PDMA_EVENT_ABORT;
                }

                if (tdsts & ch_mask)
                {
                    ch_event |= NU_PDMA_EVENT_TRANSFER_DONE;
                }

                if (unalignsts & ch_mask)
                {
                    ch_event |= NU_PDMA_EVENT_ALIGNMENT;
                }

                if (reqto_ch & ch_mask)
                {
                    PDMA_DisableTimeout(PDMA,  ch_mask);
                    ch_event |= NU_PDMA_EVENT_TIMEOUT;
                }

                //rt_kprintf("module_id=%d, ch_event=%08x\n", j, ch_event);

                if (dma_chn->m_sCB_Disable.m_pfnCBHandler)
                    dma_chn->m_sCB_Disable.m_pfnCBHandler(dma_chn->m_sCB_Disable.m_pvUserData, dma_chn->m_sCB_Disable.m_u32Reserved);

                if (dma_chn->m_u32EventFilter & ch_event)
                    dma_chn->m_sCB_Event.m_pfnCBHandler(dma_chn->m_sCB_Event.m_pvUserData, ch_event);

                if (reqto_ch & ch_mask)
                    nu_pdma_timeout_set(j, dma_chn->m_u32IdleTimeout_us);

            }//if(dma_chn->handler)

        } //if (nu_pdma_chn_mask & ch_mask)

        // Clear the served bit.
        allch_sts &= ~ch_mask;

    } //while
}

void PDMA0_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    pdma_isr(PDMA0, PDMA0_IDX);

    /* leave interrupt */
    rt_interrupt_leave();
}

void PDMA1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    pdma_isr(PDMA1, PDMA1_IDX);

    /* leave interrupt */
    rt_interrupt_leave();
}

void LPPDMA_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    pdma_isr((PDMA_T *)LPPDMA, LPPDMA_IDX);

    /* leave interrupt */
    rt_interrupt_leave();
}

static void nu_pdma_memfun_actor_init(void)
{
    int i = 0 ;
    nu_pdma_init();
    for (i = 0; i < NU_PDMA_MEMFUN_ACTOR_MAX; i++)
    {
        rt_memset(&nu_pdma_memfun_actor_arr[i], 0, sizeof(struct nu_pdma_memfun_actor));
        if (-(RT_ERROR) != (nu_pdma_memfun_actor_arr[i].m_i32ModChnID = nu_pdma_channel_allocate(PDMA_MEM)))
        {
            nu_pdma_memfun_actor_arr[i].m_psSemMemFun = rt_sem_create("memactor_sem", 0, RT_IPC_FLAG_FIFO);
            RT_ASSERT(nu_pdma_memfun_actor_arr[i].m_psSemMemFun != RT_NULL);
        }
        else
            break;
    }
    if (i)
    {
        nu_pdma_memfun_actor_maxnum = i;
        nu_pdma_memfun_actor_mask = ~(((1 << i) - 1));

        nu_pdma_memfun_actor_pool_sem = rt_sem_create("mempool_sem", nu_pdma_memfun_actor_maxnum, RT_IPC_FLAG_FIFO);
        RT_ASSERT(nu_pdma_memfun_actor_pool_sem != RT_NULL);

        nu_pdma_memfun_actor_pool_lock = rt_mutex_create("mempool_lock", RT_IPC_FLAG_PRIO);
        RT_ASSERT(nu_pdma_memfun_actor_pool_lock != RT_NULL);
    }
}

static void nu_pdma_memfun_cb(void *pvUserData, uint32_t u32Events)
{
    rt_err_t result = RT_EOK;

    nu_pdma_memfun_actor_t psMemFunActor = (nu_pdma_memfun_actor_t)pvUserData;
    psMemFunActor->m_u32Result = u32Events;

    result = rt_sem_release(psMemFunActor->m_psSemMemFun);
    RT_ASSERT(result == RT_EOK);
}

static int nu_pdma_memfun_employ(void)
{
    int idx = -1 ;
    rt_err_t result = RT_EOK;

    /* Headhunter */
    if (nu_pdma_memfun_actor_pool_sem &&
            ((result = rt_sem_take(nu_pdma_memfun_actor_pool_sem, RT_WAITING_FOREVER)) == RT_EOK))
    {
        RT_ASSERT(result == RT_EOK);

        result = rt_mutex_take(nu_pdma_memfun_actor_pool_lock, RT_WAITING_FOREVER);
        RT_ASSERT(result == RT_EOK);

        /* Find the position of first '0' in nu_pdma_memfun_actor_mask. */
        idx = nu_cto(nu_pdma_memfun_actor_mask);
        if (idx != 32)
        {
            nu_pdma_memfun_actor_mask |= (1 << idx);
        }
        else
        {
            idx = -1;
        }
        result = rt_mutex_release(nu_pdma_memfun_actor_pool_lock);
        RT_ASSERT(result == RT_EOK);
    }

    return idx;
}

static rt_size_t nu_pdma_memfun(void *dest, void *src, uint32_t u32DataWidth, unsigned int u32TransferCnt, nu_pdma_memctrl_t eMemCtl)
{
    nu_pdma_memfun_actor_t psMemFunActor = NULL;
    struct nu_pdma_chn_cb sChnCB;
    rt_err_t result = RT_ERROR;

    int idx;
    rt_size_t ret = 0;

    /* Employ actor */
    while ((idx = nu_pdma_memfun_employ()) < 0);

    psMemFunActor = &nu_pdma_memfun_actor_arr[idx];

    /* Set PDMA memory control to eMemCtl. */
    nu_pdma_channel_memctrl_set(psMemFunActor->m_i32ModChnID, eMemCtl);

    /* Register ISR callback function */
    sChnCB.m_eCBType = eCBType_Event;
    sChnCB.m_pfnCBHandler = nu_pdma_memfun_cb;
    sChnCB.m_pvUserData = (void *)psMemFunActor;

    nu_pdma_filtering_set(psMemFunActor->m_i32ModChnID, NU_PDMA_EVENT_ABORT | NU_PDMA_EVENT_TRANSFER_DONE);
    nu_pdma_callback_register(psMemFunActor->m_i32ModChnID, &sChnCB);

    psMemFunActor->m_u32Result = 0;

    //rt_kprintf("idx=%d, src@%08x -> dest@%08x, u32DataWidth: %08x, u32TransferCnt:%d\n",
    //           idx, src, dest, u32DataWidth, u32TransferCnt);

    /* Trigger it */
    nu_pdma_transfer(psMemFunActor->m_i32ModChnID,
                     u32DataWidth,
                     (uint32_t)src,
                     (uint32_t)dest,
                     u32TransferCnt,
                     0);

    /* Wait it done. */
    result = rt_sem_take(psMemFunActor->m_psSemMemFun, RT_WAITING_FOREVER);
    RT_ASSERT(result == RT_EOK);

    /* Give result if get NU_PDMA_EVENT_TRANSFER_DONE.*/
    if (psMemFunActor->m_u32Result & NU_PDMA_EVENT_TRANSFER_DONE)
    {
        ret +=  u32TransferCnt;
    }
    else
    {
        ret += (u32TransferCnt - nu_pdma_non_transfer_count_get(psMemFunActor->m_i32ModChnID));
    }

    /* Terminate it if get ABORT event */
    if (psMemFunActor->m_u32Result & NU_PDMA_EVENT_ABORT)
    {
        nu_pdma_channel_terminate(psMemFunActor->m_i32ModChnID);
    }

    result = rt_mutex_take(nu_pdma_memfun_actor_pool_lock, RT_WAITING_FOREVER);
    RT_ASSERT(result == RT_EOK);

    nu_pdma_memfun_actor_mask &= ~(1 << idx);

    result = rt_mutex_release(nu_pdma_memfun_actor_pool_lock);
    RT_ASSERT(result == RT_EOK);

    /* Fire actor */
    result = rt_sem_release(nu_pdma_memfun_actor_pool_sem);
    RT_ASSERT(result == RT_EOK);

    return ret;
}

rt_size_t nu_pdma_mempush(void *dest, void *src, uint32_t data_width, unsigned int transfer_count)
{
    if (data_width == 8 || data_width == 16 || data_width == 32)
        return nu_pdma_memfun(dest, src, data_width, transfer_count, eMemCtl_SrcInc_DstFix);
    return 0;
}

void *nu_pdma_memcpy(void *dest, void *src, unsigned int count)
{
    int i = 0;
    uint32_t u32Offset = 0;
    uint32_t u32Remaining = count;

    for (i = 4; (i > 0) && (u32Remaining > 0) ; i >>= 1)
    {
        uint32_t u32src   = (uint32_t)src + u32Offset;
        uint32_t u32dest  = (uint32_t)dest + u32Offset;

        if (((u32src % i) == (u32dest % i)) &&
                ((u32src % i) == 0) &&
                (RT_ALIGN_DOWN(u32Remaining, i) >= i))
        {
            uint32_t u32TXCnt = u32Remaining / i;
            if (u32TXCnt != nu_pdma_memfun((void *)u32dest, (void *)u32src, i * 8, u32TXCnt, eMemCtl_SrcInc_DstInc))
                goto exit_nu_pdma_memcpy;

            u32Offset += (u32TXCnt * i);
            u32Remaining -= (u32TXCnt * i);
        }
    }

    if (count == u32Offset)
        return dest;

exit_nu_pdma_memcpy:

    return NULL;
}

/**
 * PDMA memfun actor initialization
 */
int rt_hw_pdma_memfun_init(void)
{
    nu_pdma_memfun_actor_init();
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_pdma_memfun_init);
#endif // #if defined(BSP_USING_PDMA)
