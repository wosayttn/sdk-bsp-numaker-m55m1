/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2024-1-31       Wayne            First version
*
******************************************************************************/

#include <rtconfig.h>

#if defined(BSP_USING_DMIC)

#include <rtdevice.h>
#include "drv_pdma.h"

/* Private define ---------------------------------------------------------------*/
#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG
#define DBG_SECTION_NAME  "dmic"
#define DBG_COLOR
#include <rtdbg.h>

#if !defined(NU_DMIC_DMA_FIFO_SIZE)
    #define NU_DMIC_DMA_FIFO_SIZE  RT_AUDIO_RECORD_PIPE_SIZE
#endif

#if !defined(NU_DMIC_DMA_BUF_BLOCK_NUMBER)
    #define NU_DMIC_DMA_BUF_BLOCK_NUMBER (2)
#endif

#if ( (NU_DMIC_DMA_FIFO_SIZE % NU_DMIC_DMA_BUF_BLOCK_NUMBER) != 0 )
    #error "Please give an aligned definition"
#endif

#define NU_DMIC_DMA_BUF_BLOCK_SIZE  (NU_DMIC_DMA_FIFO_SIZE/NU_DMIC_DMA_BUF_BLOCK_NUMBER)

typedef enum
{
    NU_DMIC_DAI_CAPTURE,
    NU_DMIC_DAI_CNT
} E_DMIC_DAI;

enum
{
    DMIC_START = -1,
#if defined(BSP_USING_DMIC0)
    DMIC_IDX,
#endif
    DMIC_CNT
};

struct nu_dmic_dai
{
    int32_t pdma_perp;
    int32_t pdma_chanid;
    rt_uint8_t *fifo;
    int16_t  fifo_block_idx;
    nu_pdma_desc_t pdma_descs[NU_DMIC_DMA_BUF_BLOCK_NUMBER];
};
typedef struct nu_dmic_dai *nu_dmic_dai_t;

struct nu_dmic
{
    struct rt_audio_device audio;
    struct rt_audio_configure config;

    char *name;
    DMIC_T *base;
    uint32_t rst;

    struct nu_dmic_dai dmic_dais[NU_DMIC_DAI_CNT];
};
typedef struct nu_dmic *nu_dmic_t;

/* Private functions ------------------------------------------------------------*/
static rt_err_t nu_dmic_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps);
static rt_err_t nu_dmic_configure(struct rt_audio_device *audio, struct rt_audio_caps *caps);
static rt_err_t nu_dmic_init(struct rt_audio_device *audio);
static rt_err_t nu_dmic_start(struct rt_audio_device *audio, int stream);
static rt_err_t nu_dmic_stop(struct rt_audio_device *audio, int stream);
static void nu_dmic_buffer_info(struct rt_audio_device *audio, struct rt_audio_buf_info *info);

/* Private variables ------------------------------------------------------------*/
static struct nu_dmic nu_dmic_arr[] =
{
#if defined(BSP_USING_DMIC0)
    {
        .name = "dmic0",
        .base  = DMIC0,
        .rst   = SYS_DMIC0RST,
        .dmic_dais =
        {
            [NU_DMIC_DAI_CAPTURE] = {
                .pdma_perp = NU_PDMA_SET_REQ_SRC_ID(LPPDMA_IDX, LPPDMA_DMIC0_RX),
            }
        }
    },
#endif
};

static void nu_pdma_dmic_rx_cb(void *pvUserData, uint32_t u32EventFilter)
{
    nu_dmic_t psNuDMIC = (nu_dmic_t)pvUserData;
    nu_dmic_dai_t psNuDMICDai;

    RT_ASSERT(psNuDMIC);
    psNuDMICDai = &psNuDMIC->dmic_dais[NU_DMIC_DAI_CAPTURE];

    if (u32EventFilter & NU_PDMA_EVENT_TRANSFER_DONE)
    {
        // Report a buffer ready.
        rt_uint8_t *pbuf_old = &psNuDMICDai->fifo[psNuDMICDai->fifo_block_idx * NU_DMIC_DMA_BUF_BLOCK_SIZE] ;
        psNuDMICDai->fifo_block_idx = (psNuDMICDai->fifo_block_idx + 1) % NU_DMIC_DMA_BUF_BLOCK_NUMBER;

        /* Report upper layer. */
        rt_audio_rx_done(&psNuDMIC->audio, pbuf_old, NU_DMIC_DMA_BUF_BLOCK_SIZE);
    }
}

static rt_err_t nu_dmic_pdma_sc_config(nu_dmic_t psNuDMIC, E_DMIC_DAI dai)
{
    rt_err_t result = RT_EOK;
    nu_dmic_dai_t psNuDMICDai;
    int i, dw;
    nu_pdma_cb_handler_t pfm_pdma_cb;
    struct nu_pdma_chn_cb sChnCB;
    struct rt_audio_configure *pconfig;

    RT_ASSERT(psNuDMIC);

    pconfig = &psNuDMIC->config;
    psNuDMICDai = &psNuDMIC->dmic_dais[dai];

    switch ((int)dai)
    {
    case NU_DMIC_DAI_CAPTURE:
        break;

    default:
        return -RT_EINVAL;
    }

    pfm_pdma_cb = nu_pdma_dmic_rx_cb;

    /* Register ISR callback function */
    sChnCB.m_eCBType = eCBType_Event;
    sChnCB.m_pfnCBHandler = pfm_pdma_cb;
    sChnCB.m_pvUserData = (void *)psNuDMIC;

    nu_pdma_filtering_set(psNuDMICDai->pdma_chanid, NU_PDMA_EVENT_TRANSFER_DONE);
    result = nu_pdma_callback_register(psNuDMICDai->pdma_chanid, &sChnCB);
    RT_ASSERT(result == RT_EOK);

    /* >=24bit, use 32-bit data width */
    /* < 24bit, use sample-bit data width */
    dw = (pconfig->samplebits >= 24) ? 32 : pconfig->samplebits;

    for (i = 0; i < NU_DMIC_DMA_BUF_BLOCK_NUMBER; i++)
    {
        /* Setup dma descriptor entry */
        result = nu_pdma_desc_setup(psNuDMICDai->pdma_chanid,   // Channel ID
                                    psNuDMICDai->pdma_descs[i], // this descriptor
                                    dw, // 16 bits or 32 bit
                                    (uint32_t)&psNuDMIC->base->FIFO, //RXFIFO
                                    (uint32_t)&psNuDMICDai->fifo[0] + (i * NU_DMIC_DMA_BUF_BLOCK_SIZE), //Memory
                                    (int32_t)NU_DMIC_DMA_BUF_BLOCK_SIZE / (dw / 8), // Transfer count
                                    psNuDMICDai->pdma_descs[(i + 1) % NU_DMIC_DMA_BUF_BLOCK_NUMBER], // Next descriptor
                                    0);  // Interrupt assert when every SG-table done.
        RT_ASSERT(result == RT_EOK);
    }

    /* Assign head descriptor */
    result = nu_pdma_sg_transfer(psNuDMICDai->pdma_chanid, psNuDMICDai->pdma_descs[0], 0);
    RT_ASSERT(result == RT_EOK);

    return result;
}

static rt_bool_t nu_dmic_capacity_check(struct rt_audio_configure *pconfig)
{
    uint32_t u32ChEn = 0, i;
    switch (pconfig->samplebits)
    {
    case 16:
    case 24:
        /* case 24: PDMA constrain */
        break;
    default:
        goto exit_nu_dmic_capacity_check;
    }

    for (i = 0; i < 4; i++)
    {
        if (DMIC0_MIC_ENABLE_CHANNEL & (1 << i))
        {
            u32ChEn++;
        }
    }

    if (pconfig->channels > u32ChEn)
    {
        LOG_W("Channel Enabling number(%d) is less than configuration(%d).", u32ChEn, pconfig->channels);
        goto exit_nu_dmic_capacity_check;
    }

    return RT_TRUE;

exit_nu_dmic_capacity_check:

    return RT_FALSE;
}

static rt_err_t nu_dmic_dai_setup(nu_dmic_t psNuDMIC, struct rt_audio_configure *pconfig)
{
    rt_err_t result = RT_EOK;

    /* Open DMIC */
    if (nu_dmic_capacity_check(pconfig) == RT_TRUE)
    {
        uint32_t u32RealSmplRate;

        LOG_I("%d %d %d\n", pconfig->samplerate, pconfig->channels, pconfig->samplebits);

        /* Disable all channel */
        DMIC_Close(psNuDMIC->base);

        /* Reset DMIC DSP */
        DMIC_ResetDSP(psNuDMIC->base);

        DMIC_Open(psNuDMIC->base);

        u32RealSmplRate = DMIC_SetSampleRateEx(psNuDMIC->base, pconfig->samplerate, DMIC0_MIC_CLOCK_MIN, DMIC0_MIC_CLOCK_MAX);

        /* Set FIFO trigger depth to 16 */
        DMIC_ENABLE_FIFOTH_INT(psNuDMIC->base, 16);

        /* Set FIFO Width 16-bits */
        if (pconfig->samplebits == 16)
            DMIC_SetFIFOWidth(psNuDMIC->base, DMIC_FIFOWIDTH_16);
        else if (pconfig->samplebits == 24)
            DMIC_SetFIFOWidth(psNuDMIC->base, DMIC_FIFOWIDTH_24);

        /* Clear FIFO */
        DMIC_ClearFIFO(psNuDMIC->base);

        /* Set volume */
        DMIC_SetDSPGainVolume(psNuDMIC->base, DMIC0_MIC_ENABLE_CHANNEL, 25);

        psNuDMIC->base->DIV = (psNuDMIC->base->DIV & ~(0xf << 28)) | (0xf << 28);

        /* Enable channels */
        DMIC_EnableChMsk(psNuDMIC->base, DMIC0_MIC_ENABLE_CHANNEL);

        if (u32RealSmplRate != pconfig->samplerate)
            LOG_W("Real sample rate: %d Hz, preferred sample rate: %d Hz\n", u32RealSmplRate, pconfig->samplerate);

        LOG_I("Open DMIC.");
    }
    else
        result = -RT_EINVAL;

    return result;
}

static rt_err_t nu_dmic_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    nu_dmic_t psNuDMIC;

    RT_ASSERT(audio);
    RT_ASSERT(caps);

    psNuDMIC = (nu_dmic_t)audio;

    switch (caps->main_type)
    {
    case AUDIO_TYPE_QUERY:
        switch (caps->sub_type)
        {
        case AUDIO_TYPE_QUERY:
            caps->udata.mask = AUDIO_TYPE_INPUT;
            break;
        default:
            result = -RT_ERROR;
            break;
        } // switch (caps->sub_type)
        break;

    case AUDIO_TYPE_INPUT:
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            caps->udata.config.channels = psNuDMIC->config.channels;
            caps->udata.config.samplebits = psNuDMIC->config.samplebits;
            caps->udata.config.samplerate = psNuDMIC->config.samplerate;
            break;
        case AUDIO_DSP_SAMPLERATE:
            caps->udata.config.samplerate = psNuDMIC->config.samplerate;
            break;
        case AUDIO_DSP_CHANNELS:
            caps->udata.config.channels = psNuDMIC->config.channels;
            break;
        case AUDIO_DSP_SAMPLEBITS:
            caps->udata.config.samplebits = psNuDMIC->config.samplebits;
            break;
        default:
            result = -RT_ERROR;
            break;
        } // switch (caps->sub_type)
        break;

    default:
        result = -RT_ERROR;
        break;

    } // switch (caps->main_type)

    return result;
}

static rt_err_t nu_dmic_configure(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    nu_dmic_t psNuDMIC;

    RT_ASSERT(audio != RT_NULL);
    RT_ASSERT(caps != RT_NULL);

    psNuDMIC = (nu_dmic_t)audio;

    switch (caps->main_type)
    {
    case AUDIO_TYPE_MIXER:
    {
        int i32Value   = caps->udata.value;
        switch (caps->sub_type)
        {
        case AUDIO_MIXER_MUTE:
        {
            (i32Value > 0) ? DMIC_EnableMute(psNuDMIC->base, DMIC0_MIC_ENABLE_CHANNEL) :
            DMIC_DisableMute(psNuDMIC->base, DMIC0_MIC_ENABLE_CHANNEL);
        }
        break;

        case AUDIO_MIXER_VOLUME:
        {
            if ((i32Value >= 0) && (i32Value <= 100))
                DMIC_SetDSPGainVolume(psNuDMIC->base, DMIC0_MIC_ENABLE_CHANNEL, (i32Value / 10) + 20);
        }
        break;

        default:
            return -RT_ERROR;
        } //switch (caps->sub_type)
    } // case AUDIO_TYPE_MIXER:
    break;

    case AUDIO_TYPE_INPUT:
    {
        rt_bool_t bNeedReset = RT_FALSE;
        int stream = AUDIO_STREAM_RECORD;
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
        {
            if (rt_memcmp(&psNuDMIC->config, &caps->udata.config, sizeof(struct rt_audio_configure)) != 0)
            {
                rt_memcpy(&psNuDMIC->config, &caps->udata.config, sizeof(struct rt_audio_configure));
                bNeedReset = RT_TRUE;
            }
        }
        break;

        case AUDIO_DSP_SAMPLEBITS:
        {
            if (psNuDMIC->config.samplerate != caps->udata.config.samplebits)
            {
                psNuDMIC->config.samplerate = caps->udata.config.samplebits;
                bNeedReset = RT_TRUE;
            }
        }
        break;

        case AUDIO_DSP_SAMPLERATE:
        {
            if (psNuDMIC->config.samplerate != caps->udata.config.samplerate)
            {
                psNuDMIC->config.samplerate = caps->udata.config.samplerate;
                bNeedReset = RT_TRUE;
            }
        }
        break;

        default:
            result = -RT_ERROR;
            break;
        } // switch (caps->sub_type)

        if (bNeedReset)
        {
            return nu_dmic_start(audio, stream);
        }
    } // case AUDIO_TYPE_INPUT:
    break;

    default:
        result = -RT_ERROR;
        break;
    } // switch (caps->main_type)

    return result;
}

static rt_err_t nu_dmic_init(struct rt_audio_device *audio)
{
    rt_err_t result = RT_EOK;
    nu_dmic_t psNuDMIC = (nu_dmic_t)audio;

    RT_ASSERT(audio != RT_NULL);

    /* Reset this module */
    SYS_ResetModule(psNuDMIC->rst);

    return -(result);
}

static rt_err_t nu_dmic_start(struct rt_audio_device *audio, int stream)
{
    nu_dmic_t psNuDMIC = (nu_dmic_t)audio;

    RT_ASSERT(audio);

    /* Restart all: DMIC. */
    nu_dmic_stop(audio, stream);
    if (nu_dmic_dai_setup(psNuDMIC, &psNuDMIC->config) != RT_EOK)
        return -RT_ERROR;

    switch (stream)
    {
    case AUDIO_STREAM_RECORD:
    {
        nu_dmic_pdma_sc_config(psNuDMIC, NU_DMIC_DAI_CAPTURE);

        DMIC_ENABLE_LPPDMA(psNuDMIC->base);

        LOG_I("Start record.");
    }
    break;

    default:
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t nu_dmic_stop(struct rt_audio_device *audio, int stream)
{
    nu_dmic_t psNuDMIC = (nu_dmic_t)audio;
    nu_dmic_dai_t psNuDMICDai = RT_NULL;

    RT_ASSERT(audio);

    switch (stream)
    {
    case AUDIO_STREAM_RECORD:
        psNuDMICDai = &psNuDMIC->dmic_dais[NU_DMIC_DAI_CAPTURE];

        DMIC_DISABLE_LPPDMA(psNuDMIC->base);

        DMIC_Close(psNuDMIC->base);

        LOG_I("Stop record.");
        break;

    default:
        return -RT_EINVAL;
    }

    /* Stop DMA transfer. */
    nu_pdma_channel_terminate(psNuDMICDai->pdma_chanid);

    /* Close DMIC */
    LOG_I("Close DMIC.");

    /* Silence */
    rt_memset((void *)psNuDMICDai->fifo, 0, NU_DMIC_DMA_FIFO_SIZE);
    psNuDMICDai->fifo_block_idx = 0;

    return RT_EOK;
}

static void nu_dmic_buffer_info(struct rt_audio_device *audio, struct rt_audio_buf_info *info)
{
    nu_dmic_t psNuDMIC = (nu_dmic_t)audio;

    RT_ASSERT(audio);
    RT_ASSERT(info);

    info->buffer = (rt_uint8_t *)psNuDMIC->dmic_dais[NU_DMIC_DAI_CAPTURE].fifo ;
    info->total_size = NU_DMIC_DMA_FIFO_SIZE;
    info->block_size = NU_DMIC_DMA_BUF_BLOCK_SIZE;
    info->block_count = NU_DMIC_DMA_BUF_BLOCK_NUMBER;

    //rt_kprintf("info->buffer=%08x\n", (uint32_t)info->buffer);
    //rt_kprintf("info->total_size=%d\n", (uint32_t)info->total_size);
    //rt_kprintf("info->block_size=%d\n", (uint32_t)info->block_size);
    //rt_kprintf("info->block_count=%d\n", (uint32_t)info->block_count);

    return;
}

static struct rt_audio_ops nu_dmic_audio_ops =
{
    .getcaps     = nu_dmic_getcaps,
    .configure   = nu_dmic_configure,

    .init        = nu_dmic_init,
    .start       = nu_dmic_start,
    .stop        = nu_dmic_stop,
    .transmit    = RT_NULL,
    .buffer_info = nu_dmic_buffer_info
};

static rt_err_t nu_hw_dmic_pdma_allocate(nu_dmic_dai_t psNuDMICDai)
{
    /* Allocate DMIC nu_dma channel */
    if ((psNuDMICDai->pdma_chanid = nu_pdma_channel_allocate(psNuDMICDai->pdma_perp)) < 0)
    {
        goto nu_hw_dmic_pdma_allocate;
    }

    return RT_EOK;

nu_hw_dmic_pdma_allocate:

    return -(RT_ERROR);
}

static NVT_NONCACHEABLE uint8_t s_DMABufPool[DMIC_CNT * NU_DMIC_DAI_CNT][NU_DMIC_DMA_FIFO_SIZE];

int rt_hw_dmic_init(void)
{
    int i, j;
    rt_err_t ret;
    nu_dmic_dai_t psNuDMICDai;

    LOG_I("Target Output Clock Min: %d Hz, Max: %d Hz", DMIC0_MIC_CLOCK_MIN, DMIC0_MIC_CLOCK_MAX);
    LOG_I("Target Channel Mask:0x%x", DMIC0_MIC_ENABLE_CHANNEL);

    PMC_SetDMIC_SRAMPowerMode(PMC_SRAM_NORMAL);

    for (j = (DMIC_START + 1); j < DMIC_CNT; j++)
    {
        for (i = 0; i < NU_DMIC_DAI_CNT; i++)
        {
            uint8_t *pu8ptr = (uint8_t *)&s_DMABufPool[j * i][0];
            psNuDMICDai = &nu_dmic_arr[j].dmic_dais[i];
            psNuDMICDai->fifo = pu8ptr;

            psNuDMICDai->pdma_chanid = -1;
            psNuDMICDai->fifo_block_idx = 0;

            ret = nu_hw_dmic_pdma_allocate(psNuDMICDai) ;
            RT_ASSERT(ret == RT_EOK);

            ret = nu_pdma_sgtbls_allocate(psNuDMICDai->pdma_chanid, &psNuDMICDai->pdma_descs[0], NU_DMIC_DMA_BUF_BLOCK_NUMBER);
            RT_ASSERT(ret == RT_EOK);
        }

        /* Register ops of audio device */
        nu_dmic_arr[j].audio.ops  = &nu_dmic_audio_ops;

        /* Register device, RW: it is with replay and record functions. */
        rt_audio_register(&nu_dmic_arr[j].audio, nu_dmic_arr[j].name, RT_DEVICE_FLAG_RDWR, &nu_dmic_arr[j]);
    }
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_dmic_init);

#endif //#if defined(BSP_USING_DMIC)
