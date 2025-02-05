/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-2-23       Wayne        First version
*
******************************************************************************/

#ifndef __LCD_LT7381_H__
#define __LCD_LT7381_H__

#include <rtthread.h>
#include <rtdevice.h>

#define SET_RST                       rt_pin_write(BOARD_USING_LT7381_PIN_RESET, 1)
#define CLR_RST                       rt_pin_write(BOARD_USING_LT7381_PIN_RESET, 0)

#define SET_BACKLIGHT_ON              rt_pin_write(BOARD_USING_LT7381_PIN_BACKLIGHT, 1)
#define SET_BACKLIGHT_OFF             rt_pin_write(BOARD_USING_LT7381_PIN_BACKLIGHT, 0)

#define LT7381_ADDR_CMD               NU_PKG_EBI_I80_CMD
#define LT7381_ADDR_DATA              NU_PKG_EBI_I80_DATA

#define lt7381_write_cmd(Cmd)         (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_CMD))) = (Cmd))
#define lt7381_write_data(Data)       (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_DATA))) = (Data))
#define lt7381_read_data()            (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_DATA))))

#define LT7381_WRITE_REG(u16RegAddr)  (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_CMD))) = (u16RegAddr))
#define LT7381_WRITE_DATA(u16Data)    (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_DATA))) = (u16Data))
#define LT7381_READ_STATUS()          (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_CMD))))
#define LT7381_READ_DATA()            (*((volatile uint16_t *)(g_u32AccessBase+(LT7381_ADDR_DATA))))

int rt_hw_lcd_lt7381_init(void);
void lt7381_write_reg(rt_uint16_t cmd, rt_uint16_t data);
void lt7381_set_column(rt_uint16_t StartCol, rt_uint16_t EndCol);
void lt7381_set_page(rt_uint16_t StartPage, rt_uint16_t EndPage);
void lt7381_send_pixel_data(rt_uint16_t color);
void lt7381_lcd_get_pixel(char *color, int x, int y);
void lt7381_send_pixels(rt_uint16_t *pixels, int len);

#if defined(NU_PKG_USING_LT7381_EBI)
    extern rt_uint32_t g_u32AccessBase;
    rt_err_t rt_hw_lcd_lt7381_ebi_init(rt_uint32_t ebi_base);
#endif

#endif /* __LCD_LT7381_H__ */
