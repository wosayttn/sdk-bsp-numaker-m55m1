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

#if defined(NU_PKG_USING_LT7381)

#include <rtdevice.h>
#include <lcd_lt7381.h>

//
// Physical display size
//
#define XSIZE_PHYS 800
#define YSIZE_PHYS 480

#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
    #if !defined(NU_PKG_LT7381_LINE_BUFFER_NUMBER)
        #define NU_PKG_LT7381_LINE_BUFFER_NUMBER   YSIZE_PHYS
    #endif
#endif

#define LCD_VBPD        20
#define LCD_VFPD        12
#define LCD_VSPW        3
#define LCD_HBPD        140
#define LCD_HFPD        160
#define LCD_HSPW        20

#define LT_BIT0        (1<<0)
#define LT_BIT1        (1<<1)
#define LT_BIT2        (1<<2)
#define LT_BIT3        (1<<3)
#define LT_BIT4        (1<<4)
#define LT_BIT5        (1<<5)
#define LT_BIT6        (1<<6)
#define LT_BIT7        (1<<7)

#define PIXEL_CLOCK ((LCD_HBPD + LCD_HFPD + LCD_HSPW + XSIZE_PHYS) * \
                    (LCD_VBPD + LCD_VFPD + LCD_VSPW + YSIZE_PHYS) * 60)

#define PIXEL_CLOCK_MZ (PIXEL_CLOCK/1000000)

#if ((((PIXEL_CLOCK) % 1000000)/100000) > 5)
    #define DIVIDER   (PIXEL_CLOCK_MZ+1)
#else
    #define DIVIDER   (PIXEL_CLOCK_MZ)
#endif

#define SCLK   DIVIDER
#define MCLK   (5*DIVIDER)
#define CCLK   (5*DIVIDER)

#if (SCLK>65)
    #undef SCLK
    #define SCLK   65
#endif

#if (MCLK>100)
    #undef MCLK
    #define MCLK   100
#endif

#if (CCLK>100)
    #undef CCLK
    #define CCLK   100
#endif

#define SDRAM_ITV ((64000000 / 8192) / (1000 / MCLK) - 2)

#define XI_IN    XI_12M
typedef enum
{
    XI_4M,
    XI_5M,
    XI_8M,
    XI_10M,
    XI_12M,
    XI_CNT
} lt7381_xi_opt;

typedef struct
{
    uint16_t lpllOD_sclk;
    uint16_t lpllOD_cclk;
    uint16_t lpllOD_mclk;
    uint16_t lpllR_sclk;
    uint16_t lpllR_cclk;
    uint16_t lpllR_mclk;
    uint16_t lpllN_mclk;
    uint16_t lpllN_cclk;
    uint16_t lpllN_sclk;
} lt7381_pll_t;

#define lt7381_delay_ms(ms)    rt_thread_mdelay(ms)

static void lt7381_fillscreen(rt_uint16_t color);

static struct rt_device_graphic_info g_LT7381Info =
{
    .bits_per_pixel = 16,
    .pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565,
    .framebuffer = RT_NULL,
    .width = XSIZE_PHYS,
    .pitch = XSIZE_PHYS * 2,
    .height = YSIZE_PHYS
};

static const lt7381_pll_t s_PllSettings[XI_CNT] =
{
    {3, 3, 3, 2, 2, 2, (2 * MCLK), (2 * CCLK), (2 * SCLK) }, // XI_4M
    {3, 3, 3, 5, 5, 5, (4 * MCLK), (4 * CCLK), (4 * SCLK) }, // XI_5M
    {3, 3, 3, 2, 2, 2, (1 * MCLK), (1 * CCLK), (1 * SCLK) }, // XI_8M
    {3, 3, 3, 5, 5, 5, (2 * MCLK), (2 * CCLK), (2 * SCLK) }, // XI_10M
    {3, 3, 3, 3, 3, 3, (1 * MCLK), (1 * CCLK), (1 * SCLK) }  // XI_12M
};
static struct rt_mutex s_mutex;

static void lt7381_hw_reset(void)
{
    /* Hardware reset */
    SET_RST;
    lt7381_delay_ms(50);
    CLR_RST;
    lt7381_delay_ms(50);
    SET_RST;
    lt7381_delay_ms(50);
}

static void lt7381_sw_reset(void)
{
    LT7381_WRITE_REG(0x00);
    LT7381_WRITE_DATA(0x01);
    lt7381_delay_ms(100);
}

static int lt7381_wait_ready(void)
{
    int32_t i = 0;
    uint8_t system_ok = 0;

    while (i < 10)
    {
        if ((LT7381_READ_STATUS() & LT_BIT1) == 0x00)
        {
            lt7381_delay_ms(10);
            LT7381_WRITE_REG(0x01);
            if ((LT7381_READ_DATA() & LT_BIT7) == LT_BIT7)
            {
                break;
            }
            else
            {
                lt7381_delay_ms(10);
                LT7381_WRITE_REG(0x01);
                LT7381_WRITE_DATA(LT_BIT7);
            }
        }

        if ((i % 5) == 0)
            lt7381_hw_reset();

        i++;
    }

    lt7381_delay_ms(100);

    i = 100;
    while ((i > 0) && (LT7381_READ_STATUS() & LT_BIT1))
    {
        lt7381_delay_ms(1);
        i--;
    };

    if (i == 0)
        return -1;

    return 0;
}

/*
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
static void lt7381_display_on(void)
{
    volatile uint16_t temp;

    LT7381_WRITE_REG(0x12);
    temp = LT7381_READ_DATA();
    temp |= LT_BIT6;
    LT7381_WRITE_DATA(temp);
}

static void lt7381_initial_pll(void)
{
    LT7381_WRITE_REG(0x05);
    LT7381_WRITE_DATA((s_PllSettings[XI_IN].lpllOD_sclk << 6) |
                      (s_PllSettings[XI_IN].lpllR_sclk << 1) |
                      ((s_PllSettings[XI_IN].lpllN_sclk >> 8) & 0x1));

    LT7381_WRITE_REG(0x07);
    LT7381_WRITE_DATA((s_PllSettings[XI_IN].lpllOD_mclk << 6) |
                      (s_PllSettings[XI_IN].lpllR_mclk << 1) |
                      ((s_PllSettings[XI_IN].lpllN_mclk >> 8) & 0x1));

    LT7381_WRITE_REG(0x09);
    LT7381_WRITE_DATA((s_PllSettings[XI_IN].lpllOD_cclk << 6) |
                      (s_PllSettings[XI_IN].lpllR_cclk << 1) |
                      ((s_PllSettings[XI_IN].lpllN_cclk >> 8) & 0x1));

    LT7381_WRITE_REG(0x06);
    LT7381_WRITE_DATA(s_PllSettings[XI_IN].lpllN_sclk);

    LT7381_WRITE_REG(0x08);
    LT7381_WRITE_DATA(s_PllSettings[XI_IN].lpllN_mclk);

    LT7381_WRITE_REG(0x0a);
    LT7381_WRITE_DATA(s_PllSettings[XI_IN].lpllN_cclk);

    LT7381_WRITE_REG(0x00);
    lt7381_delay_ms(1);
    LT7381_WRITE_DATA(0x80);
    lt7381_delay_ms(1);
}

static int lt7381_initial_sdram(void)
{
    int i32Timeout = 1000;
    lt7381_write_reg(0xe0, 0x20);
    lt7381_write_reg(0xe1, 0x03);  //CAS:2=0x02，CAS:3=0x03

    lt7381_write_reg(0xe2, SDRAM_ITV);
    lt7381_write_reg(0xe3, SDRAM_ITV >> 8);
    lt7381_write_reg(0xe4, 0x01);

    /*  0: SDRAM is not ready for access
        1: SDRAM is ready for access        */
    do
    {
        lt7381_delay_ms(1);
        i32Timeout--;
    }
    while ((i32Timeout > 0) && ((LT7381_READ_STATUS() & LT_BIT2) == 0x00));

    lt7381_delay_ms(1);

    if (i32Timeout == 0)
        return -1;

    return 0;
}

static void lt7381_initial_panel(void)
{
    //**[01h]**//
    /*
        00b: 24-bits output.
        01b: 18-bits output, unused pins are set as GPIO.
        10b: 16-bits output, unused pins are set as GPIO.
        11b: LVDS, all 24-bits unused output pins are set as GPIO.
    */
    LT7381_WRITE_REG(0x01);
    LT7381_WRITE_DATA((LT7381_READ_DATA() & ~LT_BIT3) | LT_BIT4);

    /*
    Parallel Host Data Bus Width Selection
        0: 8-bit Parallel Host Data Bus.
        1: 16-bit Parallel Host Data Bus.*/
    LT7381_WRITE_REG(0x01);
    LT7381_WRITE_DATA(LT7381_READ_DATA() | LT_BIT0);

    //**[02h]**//
    /* RGB_16bpp, RGB565 */
    LT7381_WRITE_REG(0x02);
    LT7381_WRITE_DATA((LT7381_READ_DATA() & ~LT_BIT7) | LT_BIT6);

    /* MemWrite_Left_Right_Top_Down */
    LT7381_WRITE_REG(0x02);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT2 | LT_BIT1));

    //**[03h]**//
    /* Set graphics mode */
    LT7381_WRITE_REG(0x03);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~LT_BIT2);

    /* Set memory using sdram */
    LT7381_WRITE_REG(0x03);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT1 | LT_BIT0));

    //**[12h]**//
    /*
    PCLK Inversion
    0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
    1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
    */
    LT7381_WRITE_REG(0x12);
    LT7381_WRITE_DATA(LT7381_READ_DATA() | LT_BIT7);

    /*
    Vertical Scan direction
    0 : From Top to Bottom
    1 : From bottom to Top
    PIP window will be disabled when VDIR set as 1.
    */
    LT7381_WRITE_REG(0x12);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~LT_BIT3);

    /*
    parallel PDATA[23:0] Output Sequence
    000b : RGB.
    001b : RBG.
    010b : GRB.
    011b : GBR.
    100b : BRG.
    101b : BGR.
    */
    LT7381_WRITE_REG(0x12);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT0 | LT_BIT1 | LT_BIT2));

    //**[13h]**//
    /*
    HSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    LT7381_WRITE_REG(0x13);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT7));

    /*
    VSYNC Polarity
    0 : Low active.
    1 : High active.
    */
    LT7381_WRITE_REG(0x13);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT6));

    /*
    DE Polarity
    0 : High active.
    1 : Low active.
    */
    LT7381_WRITE_REG(0x13);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT5));

    //**[14h][15h][1Ah][1Bh]**//
    /*
    [14h] Horizontal Display Width Setting Bit[7:0]
    [15h] Horizontal Display Width Fine Tuning (HDWFT) [3:0]
    The register specifies the LCD panel horizontal display width in
    the unit of 8 pixels resolution.
    Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR

    [1Ah] Vertical Display Height Bit[7:0]
    Vertical Display Height(Line) = VDHR + 1
    [1Bh] Vertical Display Height Bit[10:8]
    Vertical Display Height(Line) = VDHR + 1
    */
    lt7381_write_reg(0x14, (XSIZE_PHYS < 8) ? 0 : ((XSIZE_PHYS / 8) - 1));
    lt7381_write_reg(0x15, (XSIZE_PHYS < 8) ? XSIZE_PHYS : XSIZE_PHYS % 8);
    lt7381_write_reg(0x1A, (YSIZE_PHYS - 1));
    lt7381_write_reg(0x1B, (YSIZE_PHYS - 1) >> 8);

    //**[16h][17h][18h][19]**//
    /*
    [16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
    This register specifies the horizontal non-display period. Also
    called back porch.
    Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
    [17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
    This register specifies the fine tuning for horizontal non-display
    period; it is used to support the SYNC mode panel. Each level of
    this modulation is 1-pixel.
    Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
    [18h] HSYNC Start Position[4:0]
    The starting position from the end of display area to the
    beginning of HSYNC. Each level of this modulation is 8-pixel.
    Also called front porch.
    HSYNC Start Position(pixels) = (HSTR + 1)x8
    [19h] HSYNC Pulse Width(HPW) [4:0]
    The period width of HSYNC.
    HSYNC Pulse Width(pixels) = (HPW + 1)x8
    */
    lt7381_write_reg(0x16, (LCD_HBPD < 8) ? 0 : (LCD_HBPD / 8) - 1);
    lt7381_write_reg(0x17, (LCD_HBPD < 8) ? LCD_HBPD : (LCD_HBPD % 8));

    lt7381_write_reg(0x18, (LCD_HFPD < 8) ? 0 : ((LCD_HFPD / 8) - 1));
    lt7381_write_reg(0x19, (LCD_HSPW < 8) ? 0 : ((LCD_HSPW / 8) - 1));

    //**[1Ch][1Dh][1Eh][1Fh]**//
    /*
    [1Ch] Vertical Non-Display Period Bit[7:0]
    Vertical Non-Display Period(Line) = (VNDR + 1)
    [1Dh] Vertical Non-Display Period Bit[9:8]
    Vertical Non-Display Period(Line) = (VNDR + 1)
    [1Eh] VSYNC Start Position[7:0]
    The starting position from the end of display area to the beginning of VSYNC.
    VSYNC Start Position(Line) = (VSTR + 1)
    [1Fh] VSYNC Pulse Width[5:0]
    The pulse width of VSYNC in lines.
    VSYNC Pulse Width(Line) = (VPWR + 1)
    */
    lt7381_write_reg(0x1C, (LCD_VBPD - 1));
    lt7381_write_reg(0x1D, (LCD_VBPD - 1) >> 8);

    lt7381_write_reg(0x1E, LCD_VFPD  - 1);
    lt7381_write_reg(0x1F, LCD_VSPW  - 1);

    //**[5Eh]**//
    /*
    Canvas addressing mode
    0: Block mode (X-Y coordination addressing)
    1: linear mode
    */
    LT7381_WRITE_REG(0x5E);
    LT7381_WRITE_DATA(LT7381_READ_DATA() & ~(LT_BIT2));

    /*
    Canvas image color depth & memory R/W data width
    In Block Mode:
    00: 8bpp
    01: 16bpp
    1x: 24bpp

    In Linear Mode:
    X0: 8-bits memory data read/write.
    X1: 16-bits memory data read/write
    */
    LT7381_WRITE_REG(0x5E);
    LT7381_WRITE_DATA((LT7381_READ_DATA() & ~(LT_BIT1)) | LT_BIT0);
}

static void lt7381_initial_main_window(void)
{
    //**[10h]**//
    /*
    Main Window Color Depth Setting
    00b: 8-bpp generic TFT, i.e. 256 colors.
    01b: 16-bpp generic TFT, i.e. 65K colors.
    1xb: 24-bpp generic TFT, i.e. 1.67M colors.
    */
    LT7381_WRITE_REG(0x10);
    LT7381_WRITE_DATA((LT7381_READ_DATA() & ~(LT_BIT3)) | LT_BIT2);

    //**[20h][21h][22h][23h]**//
    /*
    [20h] Main Image Start Address[7:2]
    [21h] Main Image Start Address[15:8]
    [22h] Main Image Start Address [23:16]
    [23h] Main Image Start Address [31:24]
    */
    lt7381_write_reg(0x20, 0x0);
    lt7381_write_reg(0x21, 0x0 >> 8);
    lt7381_write_reg(0x22, 0x0 >> 16);
    lt7381_write_reg(0x23, 0x0 >> 24);

    //**[24h][25h]**//
    /*
    [24h] Main Image Width [7:0]
    [25h] Main Image Width [12:8]
    Unit: Pixel.
    It must be divisible by 4. MIW Bit [1:0] tie to ?? internally.
    The value is physical pixel number. Maximum value is 8188 pixels
    */
    lt7381_write_reg(0x24, XSIZE_PHYS);
    lt7381_write_reg(0x25, XSIZE_PHYS >> 8);

    //**[26h][27h]**//
    /*
    [26h] Main Window Upper-Left corner X-coordination [7:0]
    [27h] Main Window Upper-Left corner X-coordination [12:8]
    Reference Main Image coordination.
    Unit: Pixel
    It must be divisible by 4. MWULX Bit [1:0] tie to ?? internally.
    X-axis coordination plus Horizontal display width cannot large than 8188.

    [28h] Main Window Upper-Left corner Y-coordination [7:0]
    [29h] Main Window Upper-Left corner Y-coordination [12:8]
    Reference Main Image coordination.
    Unit: Pixel
    Range is between 0 and 8191.
    */
    lt7381_write_reg(0x26, 0x0);
    lt7381_write_reg(0x27, 0x0 >> 8);
    lt7381_write_reg(0x28, 0x0);
    lt7381_write_reg(0x29, 0x0 >> 8);

    //**[50h][51h][52h][53h][54h][55h]**//
    /*
    [50h] Start address of Canvas [7:0]
    [51h] Start address of Canvas [15:8]
    [52h] Start address of Canvas [23:16]
    [53h] Start address of Canvas [31:24]
    [54h] Canvas image width [7:2]
    [55h] Canvas image width [12:8]
    */
    lt7381_write_reg(0x50, 0x0);
    lt7381_write_reg(0x51, 0x0 >> 8);
    lt7381_write_reg(0x52, 0x0 >> 16);
    lt7381_write_reg(0x53, 0x0 >> 24);
    lt7381_write_reg(0x54, XSIZE_PHYS);
    lt7381_write_reg(0x55, XSIZE_PHYS >> 8);

    //**[56h][57h][58h][59h][5Ah][5Bh][5Ch][5Dh]**//
    /*
    [56h] Active Window Upper-Left corner X-coordination [7:0]
    [57h] Active Window Upper-Left corner X-coordination [12:8]
    [58h] Active Window Upper-Left corner Y-coordination [7:0]
    [59h] Active Window Upper-Left corner Y-coordination [12:8]
    [5Ah] Width of Active Window [7:0]
    [5Bh] Width of Active Window [12:8]
    [5Ch] Height of Active Window [7:0]
    [5Dh] Height of Active Window [12:8]
    */
    lt7381_write_reg(0x56, 0x0);
    lt7381_write_reg(0x57, 0x0 >> 8);
    lt7381_write_reg(0x58, 0x0);
    lt7381_write_reg(0x59, 0x0 >> 8);
    lt7381_write_reg(0x5A, XSIZE_PHYS);
    lt7381_write_reg(0x5B, XSIZE_PHYS >> 8);
    lt7381_write_reg(0x5C, YSIZE_PHYS);
    lt7381_write_reg(0x5D, YSIZE_PHYS >> 8);
}

static rt_err_t lt7381_pin_init(void)
{
    rt_pin_mode(BOARD_USING_LT7381_PIN_BACKLIGHT, PIN_MODE_OUTPUT);
    SET_BACKLIGHT_OFF;

    rt_pin_mode(BOARD_USING_LT7381_PIN_RESET, PIN_MODE_OUTPUT);
    SET_RST;

    return RT_EOK;
}

static rt_err_t lt7381_lcd_init(rt_device_t dev)
{
    static int bInit = 0;

    rt_mutex_take(&s_mutex, RT_WAITING_FOREVER);

    if (bInit)
    {
        rt_mutex_release(&s_mutex);
        return RT_EOK;
    }

    SET_BACKLIGHT_OFF;

    /* Hardware reset */
    lt7381_delay_ms(100);
    lt7381_hw_reset();
    lt7381_delay_ms(100);

    if (lt7381_wait_ready() < 0)
        goto exit_lt7381_lcd_init;

    lt7381_initial_pll();

    if (lt7381_initial_sdram() < 0)
        goto exit_lt7381_lcd_init;

    lt7381_initial_panel();

    lt7381_display_on();

    lt7381_initial_main_window();

    SET_BACKLIGHT_ON;

    bInit = 1;
    rt_mutex_release(&s_mutex);

    return RT_EOK;

exit_lt7381_lcd_init:

    rt_kprintf("Failed to initial LCD\n");
    rt_mutex_release(&s_mutex);

    return -RT_ERROR;
}

#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
static void lt7381_fillrect(uint16_t *pixels, struct rt_device_rect_info *pRectInfo)
{
    lt7381_set_column(pRectInfo->x, pRectInfo->x + pRectInfo->width - 1);
    lt7381_set_page(pRectInfo->y, pRectInfo->y + pRectInfo->height - 1);

    lt7381_send_pixels(pixels, pRectInfo->height * pRectInfo->width * 2);
}
#endif

static void lt7381_fillscreen(rt_uint16_t color)
{
#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
    struct rt_device_rect_info rectinfo;
    int filled_line_num = 0;

    while (filled_line_num < YSIZE_PHYS)
    {
        int pixel_count;
        rectinfo.x = 0;
        rectinfo.y = filled_line_num;
        rectinfo.width = XSIZE_PHYS;
        rectinfo.height = (NU_PKG_LT7381_LINE_BUFFER_NUMBER < YSIZE_PHYS) ? NU_PKG_LT7381_LINE_BUFFER_NUMBER : YSIZE_PHYS;

        pixel_count = XSIZE_PHYS * NU_PKG_LT7381_LINE_BUFFER_NUMBER;
        rt_uint16_t *pu16ShadowBuf = (rt_uint16_t *)g_LT7381Info.framebuffer;

        while (pixel_count > 0)
        {
            *pu16ShadowBuf++ = color;
            pixel_count--;
        }
        lt7381_fillrect((uint16_t *)g_LT7381Info.framebuffer, &rectinfo);
        filled_line_num += NU_PKG_LT7381_LINE_BUFFER_NUMBER;
    }
#else
    lt7381_set_column(0, (XSIZE_PHYS - 1));
    lt7381_set_page(0, (YSIZE_PHYS - 1));

    for (int i = 0; i < (XSIZE_PHYS * YSIZE_PHYS); i++)
        lt7381_send_pixel_data(color);
#endif
}

static void lt7381_lcd_set_pixel(const char *color, int x, int y)
{
    lt7381_set_column(x, x);
    lt7381_set_page(y, y);

    lt7381_send_pixel_data(*(uint16_t *)color);
}

static void lt7381_lcd_draw_hline(const char *pixel, int x1, int x2, int y)
{
    lt7381_set_column(x1, x2);
    lt7381_set_page(y, y);

    lt7381_send_pixels((rt_uint16_t *)pixel, (x2 - x1) * 2);
}

static void lt7381_lcd_draw_vline(const char *pixel, int x, int y1, int y2)
{
    lt7381_set_column(x, x);
    lt7381_set_page(y1, y2);

    lt7381_send_pixels((rt_uint16_t *)pixel, (y2 - y1) * 2);
}

static void lt7381_lcd_blit_line(const char *pixels, int x, int y, rt_size_t size)
{
    lt7381_set_column(x, x + size);
    lt7381_set_page(y, y);

    lt7381_send_pixels((rt_uint16_t *)pixels, size * 2);
}

static rt_err_t lt7381_lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t lt7381_lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lt7381_lcd_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info;

        info = (struct rt_device_graphic_info *) args;
        RT_ASSERT(info != RT_NULL);
        rt_memcpy(args, (void *)&g_LT7381Info, sizeof(struct rt_device_graphic_info));
    }
    break;

    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        rt_mutex_take(&s_mutex, RT_WAITING_FOREVER);

#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
        struct rt_device_rect_info *psRectInfo = (struct rt_device_rect_info *)args;
        rt_uint16_t *pixels  = (rt_uint16_t *)g_LT7381Info.framebuffer;
        RT_ASSERT(args);

        lt7381_fillrect(pixels, psRectInfo);
#else
        /* nothong to be done */
#endif

        rt_mutex_release(&s_mutex);
    }
    break;

    case RTGRAPHIC_CTRL_RECT_UPDATE2:
    {
        rt_mutex_take(&s_mutex, RT_WAITING_FOREVER);

#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
        struct rt_device_rect_info *psRectInfo = (struct rt_device_rect_info *)args;
        rt_uint16_t *pixels  = (rt_uint16_t *)psRectInfo->framebuffer;
        RT_ASSERT(args);

        lt7381_fillrect(pixels, psRectInfo);
#else
        /* nothong to be done */
#endif

        rt_mutex_release(&s_mutex);
    }
    break;

    default:
        return -RT_ERROR;
    }

    return RT_EOK;
}

static struct rt_device lcd_device;
static struct rt_device_graphic_ops lt7381_ops =
{
    lt7381_lcd_set_pixel,
    lt7381_lcd_get_pixel,
    lt7381_lcd_draw_hline,
    lt7381_lcd_draw_vline,
    lt7381_lcd_blit_line
};

int rt_hw_lcd_lt7381_init(void)
{
    lt7381_pin_init();

    /* register lcd device */
    lcd_device.type = RT_Device_Class_Graphic;
    lcd_device.init = lt7381_lcd_init;
    lcd_device.open = lt7381_lcd_open;
    lcd_device.close = lt7381_lcd_close;
    lcd_device.control = lt7381_lcd_control;
    lcd_device.read = RT_NULL;
    lcd_device.write = RT_NULL;

    lcd_device.user_data = &lt7381_ops;

#if defined(NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER)
#if defined(BSP_USING_SPIM0)
#include "drv_memheap.h"
    g_LT7381Info.framebuffer = rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_SPIM0), g_LT7381Info.pitch * NU_PKG_LT7381_LINE_BUFFER_NUMBER + 32);
#elif defined(BSP_USING_HBI)
    struct rt_memheap *nu_hyperram_get_memheap(const char *name);
    g_LT7381Info.framebuffer = rt_memheap_alloc(nu_hyperram_get_memheap("hbi"), g_LT7381Info.pitch * NU_PKG_LT7381_LINE_BUFFER_NUMBER + 32);
#else
    g_LT7381Info.framebuffer = rt_malloc_align((g_LT7381Info.pitch * NU_PKG_LT7381_LINE_BUFFER_NUMBER) + 32, 32);
#endif
    RT_ASSERT(g_LT7381Info.framebuffer != RT_NULL);
    g_LT7381Info.smem_len = g_LT7381Info.pitch * NU_PKG_LT7381_LINE_BUFFER_NUMBER;
#endif
    rt_mutex_init(&s_mutex, "lcd_mutex", RT_IPC_FLAG_PRIO);

    /* register graphic device driver */
    //rt_device_register(&lcd_device, "lcd", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    rt_device_register(&lcd_device, "lcd", RT_DEVICE_FLAG_RDWR);

    return 0;
}

#ifdef RT_USING_FINSH
#define LINE_LEN 32
static void lcd_test(int argc, char *argv[])
{
    uint16_t pixels[LINE_LEN];
    uint16_t color;
    int x, y, i;
    x = y = 100;

    lt7381_lcd_init(NULL);

    color = 0x0; //Black, RGB
    rt_kprintf("Brush 0x%X on screen.\n", color);
    lt7381_fillscreen(color);
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    color = 0xffff; //White, RGB
    rt_kprintf("Brush 0x%X on screen.\n", color);
    lt7381_fillscreen(color);
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    color = 0x1f; //Blue, RGB
    rt_kprintf("Brush 0x%X on screen.\n", color);
    lt7381_fillscreen(color);
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    color = 0x07e0; //Green, RGB
    rt_kprintf("Brush 0x%X on screen.\n", color);
    lt7381_fillscreen(color);
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    color = 0xf800; //Red, RGB
    rt_kprintf("Brush 0x%X on screen.\n", color);
    lt7381_fillscreen(color);
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    color = 0xffff; //White, RGB
    rt_kprintf("lcd draw hline, pixel: 0x%X, x1: %d, x2: %d, y: %d\n", color, x, x + 20, y);
    lt7381_lcd_draw_hline((const char *)&color, x, x + 20, y);

    color = 0xffff; //White, RGB
    rt_kprintf("lcd draw vline, pixel: 0x%X, x: %d, y: %d\n", color, y, y + 20);
    lt7381_lcd_draw_vline((const char *)&color, x, y, y + 20);

    for (i = 0; i < LINE_LEN; i++)
        pixels[i] = 20 + i * 5;

    x = y = 50;
    rt_kprintf("lcd blit line, start: x: %d, y: %d\n", x, y);
    lt7381_lcd_blit_line((const char *)&pixels[0], x, y, LINE_LEN);

    x = y = 200;
    color = 0x07E0; //Green, RGB
    rt_kprintf("lcd set pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);
    lt7381_lcd_set_pixel((const char *)&color, x, y);
    color = 0x0;
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    x = y = 200;
    color = 0x1f; //Blue, RGB
    rt_kprintf("lcd set pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);
    lt7381_lcd_set_pixel((const char *)&color, x, y);
    color = 0x0;
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);

    x = y = 200;
    color = 0xf800; //Red, RGB
    rt_kprintf("lcd set pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);
    lt7381_lcd_set_pixel((const char *)&color, x, y);
    color = 0x0;
    lt7381_lcd_get_pixel((char *)&color, x, y);
    rt_kprintf("lcd get pixel, pixel: 0x%X, x: %d, y: %d\n", color, x, y);
}
MSH_CMD_EXPORT(lcd_test, test lcd display);
#endif

#endif /* if defined(NU_PKG_USING_LT7381) */
