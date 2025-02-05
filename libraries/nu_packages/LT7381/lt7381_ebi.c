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

#if defined(NU_PKG_USING_LT7381_EBI)

#include <lcd_lt7381.h>
#include "drv_pdma.h"

rt_uint32_t g_u32AccessBase = 0;

void lt7381_write_reg(rt_uint16_t reg, rt_uint16_t data)
{
    LT7381_WRITE_REG(reg & 0xFF);
    LT7381_WRITE_DATA(data & 0xFF);
}

void lt7381_send_pixel_data(rt_uint16_t color)
{
    LT7381_WRITE_DATA(color);
}

void lt7381_set_column(uint16_t StartCol, uint16_t EndCol)
{
    uint16_t ActiveX = EndCol - StartCol + 1;

    lt7381_write_reg(0x56, StartCol);
    lt7381_write_reg(0x57, StartCol >> 8);
    lt7381_write_reg(0x5A, ActiveX);
    lt7381_write_reg(0x5B, ActiveX >> 8);
}

void lt7381_set_page(uint16_t StartPage, uint16_t EndPage)
{
    uint16_t ActiveY = EndPage - StartPage + 1;

    lt7381_write_reg(0x58, StartPage);
    lt7381_write_reg(0x59, StartPage >> 8);
    lt7381_write_reg(0x5C, ActiveY);
    lt7381_write_reg(0x5D, ActiveY >> 8);
}

/*
  0: Memory Write FIFO is not full.
  1: Memory Write FIFO is full.
*/
static uint32_t lt7381_vram_fifo_isfull(void)
{
    return (LT7381_READ_STATUS() & BIT7);
}

/*
  0: Memory FIFO is not empty.
  1: Memory FIFO is empty.
*/
static uint32_t lt7381_vram_fifo_isempty(void)
{
    return (LT7381_READ_STATUS() & BIT6);
}

void lt7381_lcd_get_pixel(char *color, int x, int y)
{
    //Not supported.
    return;
}

void lt7381_send_pixels(rt_uint16_t *pixels, int byte_len)
{
    int count = byte_len / sizeof(uint16_t);

    /* Set Graphic Read/Write position */
    lt7381_write_reg(0x5F, 0);
    lt7381_write_reg(0x60, 0);
    lt7381_write_reg(0x61, 0);
    lt7381_write_reg(0x62, 0);

    /* Memory Data Read/Write Port */
    LT7381_WRITE_REG(0x04);

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
            while (lt7381_vram_fifo_isfull());
            LT7381_WRITE_DATA(pixels[i]);

            i++;
        }
    }
    else
    {
        // PDMA-M2M feed
        nu_pdma_mempush((void *)(g_u32AccessBase + (LT7381_ADDR_DATA)), (void *)pixels, 16, count);
    }

    while (!lt7381_vram_fifo_isempty());
}

rt_err_t rt_hw_lcd_lt7381_ebi_init(rt_uint32_t lt7381_base)
{
    g_u32AccessBase = lt7381_base;
    return RT_EOK;
}

#endif /* if defined(NU_PKG_USING_LT7381_EBI) */
