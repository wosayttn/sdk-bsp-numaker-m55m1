/**************************************************************************//**
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2024-3-4        Wayne        First version
*
******************************************************************************/

#include <rtconfig.h>

#if defined(BSP_USING_LPADC)

#include <rtdevice.h>
#include "NuMicro.h"

#define CONFIG_MAX_CHN_NUM  24

/* Private define ---------------------------------------------------------------*/
enum
{
    LPADC_START = -1,
#if defined(BSP_USING_LPADC0)
    LPADC0_IDX,
#endif
    LPADC_CNT
};

/* Private Typedef --------------------------------------------------------------*/
struct nu_lpadc
{
    struct rt_adc_device dev;
    char       *name;
    LPADC_T     *base;
    uint32_t    chn_msk;
    uint32_t    max_chn_num;
};
typedef struct nu_lpadc *nu_lpadc_t;

/* Private functions ------------------------------------------------------------*/
static rt_err_t nu_lpadc_enabled(struct rt_adc_device *device, rt_uint32_t channel, rt_bool_t enabled);
static rt_err_t nu_lpadc_convert(struct rt_adc_device *device, rt_uint32_t channel, rt_uint32_t *value);
static rt_uint8_t nu_lpadc_get_resolution(struct rt_adc_device *device);
static rt_int16_t nu_lpadc_get_vref(struct rt_adc_device *device);
static rt_err_t nu_lpadc_control(rt_device_t device, int cmd, void *args);

/* Private variables ------------------------------------------------------------*/

static struct nu_lpadc nu_lpadc_arr [] =
{
#if defined(BSP_USING_LPADC0)
    {
        .name = "lpadc0",
        .base = LPADC0,
        .chn_msk = 0,
        .max_chn_num = CONFIG_MAX_CHN_NUM,
    },
#endif
};

static const struct rt_adc_ops nu_lpadc_ops =
{
    .enabled = nu_lpadc_enabled,
    .convert = nu_lpadc_convert,
    .get_resolution = nu_lpadc_get_resolution,
    .get_vref = nu_lpadc_get_vref,
    .control = nu_lpadc_control
};
typedef struct rt_adc_ops *rt_adc_ops_t;

static rt_uint32_t s_u32BuiltInBandGapValue = 0;

static rt_uint8_t nu_lpadc_get_resolution(struct rt_adc_device *device)
{
    return 12; /* 12-bit */
}

static rt_uint32_t _lpadc_convert(nu_lpadc_t psNuLPADC, rt_uint32_t channel)
{
#define CONFIG_EXT_SMPL_TIME           20

    /* Clear the A/D interrupt flag for safe */
    LPADC_CLR_INT_FLAG(psNuLPADC->base, LPADC_ADF_INT);

    /* Set sample module extended sampling time. */
    LPADC_SetExtendSampleTime(psNuLPADC->base, 0, CONFIG_EXT_SMPL_TIME);

    /* Enable the sample module interrupt */
    LPADC_EnableInt(psNuLPADC->base, LPADC_ADF_INT);

    /* Trigger to start A/D conversion */
    LPADC_START_CONV(psNuLPADC->base);

    /* Polling the ADF flag. */
    while (LPADC_GET_INT_FLAG(psNuLPADC->base, LPADC_ADF_INT) == 0);

    /* Disable the sample module interrupt */
    LPADC_DisableInt(psNuLPADC->base, LPADC_ADF_INT);

    /* Get the conversion result. */
    return LPADC_GET_CONVERSION_DATA(psNuLPADC->base, channel);
}

static rt_int16_t nu_lpadc_get_vref(struct rt_adc_device *device)
{
    nu_lpadc_t psNuLPADC = (nu_lpadc_t)device;
    rt_uint16_t u32VBG;

    /* Force to enable internal voltage band-gap. */
    SYS_UnlockReg();

    nu_lpadc_enabled(device, 29, RT_TRUE);
    u32VBG = _lpadc_convert(psNuLPADC, 29);
    nu_lpadc_enabled(device, 29, RT_FALSE);

    /* Use Conversion result of Band-gap to calculating AVdd */
    /*
      u16Vref    s_u32BuiltInBandGapValue
    ---------- = -------------------------
       3072          i32ConversionData
    */
    return (3072 * s_u32BuiltInBandGapValue / u32VBG);
}

/* nu_lpadc_enabled - Enable ADC clock and wait for ready */
static rt_err_t nu_lpadc_enabled(struct rt_adc_device *device, rt_uint32_t channel, rt_bool_t enabled)
{
    nu_lpadc_t psNuLPADC = (nu_lpadc_t)device;

    RT_ASSERT(device);

    if (channel >= psNuLPADC->max_chn_num)
        return -(RT_EINVAL);

    if (enabled)
    {
        uint32_t u32ChnMsk = psNuLPADC->chn_msk | (0x1 << channel);

        if (psNuLPADC->chn_msk != u32ChnMsk)
        {
            LPADC_Close(psNuLPADC->base);

            /* Set input mode as single-end, Single-cycle scan mode and select channels */
            LPADC_Open(psNuLPADC->base, LPADC_ADCR_DIFFEN_SINGLE_END, LPADC_ADCR_ADMD_SINGLE_CYCLE, u32ChnMsk);

            psNuLPADC->chn_msk = u32ChnMsk;
        }
    }
    else
    {
        psNuLPADC->chn_msk &= ~(0x1 << channel);

        if (psNuLPADC->chn_msk == 0)
        {
            LPADC_Close(psNuLPADC->base);
        }
    }

    return RT_EOK;
}

static rt_err_t nu_lpadc_convert(struct rt_adc_device *device, rt_uint32_t channel, rt_uint32_t *value)
{
    nu_lpadc_t psNuLPADC = (nu_lpadc_t)device;
    rt_err_t ret = RT_ERROR;

    RT_ASSERT(device);
    RT_ASSERT(value);

    if (channel >= psNuLPADC->max_chn_num)
    {
        *value = 0xFFFFFFFF;
        ret = RT_EINVAL;
        goto exit_nu_lpadc_convert;
    }

    if ((psNuLPADC->chn_msk & (1 << channel)) == 0)
    {
        *value = 0xFFFFFFFF;
        ret = RT_EBUSY;
        goto exit_nu_lpadc_convert;
    }

    /* Get the conversion result. */
    *value = _lpadc_convert(psNuLPADC, channel);

    ret = RT_EOK;

exit_nu_lpadc_convert:

    return -(ret);
}

static rt_err_t nu_lpadc_control(rt_device_t device, int cmd, void *args)
{
    nu_lpadc_t psNuLPADC = (nu_lpadc_t)device;

    RT_ASSERT(device);

    switch (cmd)
    {
    default:
        break;
    }

    return -RT_EINVAL;
}

int rt_hw_lpadc_init(void)
{
    int i;
    rt_err_t result;

    /* Invoke ISP function to read built-in band-gap A/D conversion result*/
    SYS_UnlockReg();
    FMC_Open();
    s_u32BuiltInBandGapValue = FMC_ReadBandGap();
    FMC_Close();

    for (i = (LPADC_START + 1); i < LPADC_CNT; i++)
    {
        result = rt_hw_adc_register(&nu_lpadc_arr[i].dev, nu_lpadc_arr[i].name, &nu_lpadc_ops, NULL);
        RT_ASSERT(result == RT_EOK);
    }

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_lpadc_init);

#endif //#if defined(BSP_USING_LPADC)
