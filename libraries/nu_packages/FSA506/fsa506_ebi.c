/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-2-23       Wayne        First version
*
******************************************************************************/

#include <rtconfig.h>

#if defined(NU_PKG_USING_FSA506_EBI)

#include <lcd_fsa506.h>
#include "drv_pdma.h"

#define FSA506_ADDR_CMD    NU_PKG_EBI_I80_CMD
#define FSA506_ADDR_DATA   NU_PKG_EBI_I80_DATA

#define fsa506_write_cmd(Cmd)         (*((volatile uint16_t *)(s_u32AccessBase+(FSA506_ADDR_CMD))) = (Cmd))
#define fsa506_write_data(Data)       (*((volatile uint16_t *)(s_u32AccessBase+(FSA506_ADDR_DATA))) = (Data))
#define fsa506_read_data()            (*((volatile uint16_t *)(s_u32AccessBase+(FSA506_ADDR_DATA))))

#define FSA506_WRITE_REG(u32RegAddr)  (*((volatile uint16_t *)(s_u32AccessBase+(FSA506_ADDR_CMD))) = (u32RegAddr))
#define FSA506_WRITE_DATA(u16Data)    (*((volatile uint16_t *)(s_u32AccessBase+(FSA506_ADDR_DATA))) = (u16Data))
#define FSA506_WRITE_DATA32(u32Data)  (*((volatile uint32_t *)(s_u32AccessBase+(FSA506_ADDR_DATA))) = (u32Data))

static rt_uint32_t s_u32AccessBase = 0;

void fsa506_write_reg(rt_uint16_t reg, rt_uint16_t data)
{
    FSA506_WRITE_REG(reg & 0xFF);
    FSA506_WRITE_DATA(data & 0xFF);
    FSA506_WRITE_REG(0x80);
}

void fsa506_send_pixel_data(rt_uint16_t color)
{
    FSA506_WRITE_REG(0xC1);
    FSA506_WRITE_DATA(color);
    FSA506_WRITE_REG(0x80);
}

void fsa506_send_pixels(rt_uint16_t *pixels, int len)
{
    int count = len / sizeof(rt_uint16_t);

    FSA506_WRITE_REG(0xC1);

#if defined(BSP_USING_HBI)
    if (1) /* Always use CPU-feed */
#else
    if (count < 512)
#endif
    {
        // CPU feed
        int i = 0;
        while (i < count)
        {
            FSA506_WRITE_DATA(pixels[i]);
            i++;
        }
    }
    else
    {
        // PDMA-M2M feed
        nu_pdma_mempush((void *)(s_u32AccessBase + FSA506_ADDR_DATA), (void *)pixels, 16, count);
    }

    FSA506_WRITE_REG(0x80);
}

void fsa506_set_column(uint16_t StartCol, uint16_t EndCol)
{
    fsa506_write_reg(0x0, (StartCol >> 8) & 0xFF);
    fsa506_write_reg(0x1, StartCol & 0xFF);
    fsa506_write_reg(0x2, (EndCol >> 8) & 0xFF);
    fsa506_write_reg(0x3, EndCol & 0xFF);
}

void fsa506_set_page(uint16_t StartPage, uint16_t EndPage)
{
    fsa506_write_reg(0x4, (StartPage >> 8) & 0xFF);
    fsa506_write_reg(0x5, StartPage & 0xFF);
    fsa506_write_reg(0x6, (EndPage >> 8) & 0xFF);
    fsa506_write_reg(0x7, EndPage & 0xFF);
}

void fsa506_lcd_get_pixel(char *color, int x, int y)
{
    //Not supported.
    return;
}

rt_err_t rt_hw_lcd_fsa506_ebi_init(rt_uint32_t fsa506_base)
{
    s_u32AccessBase = fsa506_base;
    return RT_EOK;
}

#endif /* if defined(NU_PKG_USING_FSA506_EBI) */
