/*
 * SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lcd_img.h"

#include "log_macros.h"
#include "rtthread.h"
#include "font_9x15_h.h"

#include <string.h>
#include <assert.h>

#if !defined(MLEVK_UC_LCD_RENDERING_DEVICE)
    #define MLEVK_UC_LCD_RENDERING_DEVICE      "lcd"
#endif

#if defined(MLEVK_UC_LCD_DISPLAY_RIGHT)
    #define SHADOW_BUFFER_X_OFFSET               (s_info.width / 2)
    #define SHADOW_BUFFER_WIDTH                  (s_info.width / 2)
    #define NU_PKG_MLEVK_RENDERING_SHIFT         (((BSP_LCD_BPP/8) * BSP_LCD_WIDTH * BSP_LCD_HEIGHT)/2)
#endif

#if !defined(SHADOW_BUFFER_WIDTH)
    #define SHADOW_BUFFER_WIDTH               (s_info.width)
#endif

#if !defined(SHADOW_BUFFER_HEIGHT)
    #define SHADOW_BUFFER_HEIGHT              (s_info.height)
#endif

#if !defined(SHADOW_BUFFER_X_OFFSET)
    #define SHADOW_BUFFER_X_OFFSET            0
#endif

#if !defined(SHADOW_BUFFER_Y_OFFSET)
    #define SHADOW_BUFFER_Y_OFFSET            0
#endif

#if !defined(NU_PKG_MLEVK_RENDERING_SHIFT)
    #define NU_PKG_MLEVK_RENDERING_SHIFT      0
#endif

#define SHADOW_BUFFER_START                   (s_info.framebuffer + NU_PKG_MLEVK_RENDERING_SHIFT)

static rt_device_t s_lcd = RT_NULL;
static struct rt_device_graphic_info    s_info = {0};

#define BG_COLOR  0                     /* Background colour                  */
#define TXT_COLOR 1                     /* Text colour                        */

/**
* Text and background colour
*/
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */

static volatile unsigned short Color[2] = {Black, White};


int lcd_get_width(void)
{
    return SHADOW_BUFFER_WIDTH;
}

int lcd_get_height(void)
{
    return SHADOW_BUFFER_HEIGHT;
}

static int show_title(void)
{
    const char title[] = "Arm ML embedded code samples";
    lcd_set_text_color(White);
    return lcd_display_text(title, strlen(title), 10, 0, false);
}

static void _SetTextColor(unsigned short color)
{
    Color[TXT_COLOR] = color;
}

static void _SetBackColor(unsigned short color)
{
    Color[BG_COLOR] = color;
}

/**
 * @brief       Converts a gray value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel.
 * @return      16 bit RGB565 value.
 */
static inline uint16_t _Gray8_to_RGB565(uint8_t *src_uchar)
{
    uint16_t val_r = (*src_uchar >> 3);
    uint16_t val_g = (*src_uchar >> 2);
    return ((val_r << 11) | (val_g << 5) | val_r);
}

/**
 * @brief       Converts an RGB888 value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel for R (assumed to
 *                          be RGB format).
 * @return      16 bit RGB565 value.
 */
static inline uint16_t _RGB888_to_RGB565(uint8_t *src_uchar)
{
    uint16_t val_r = (*src_uchar >> 3) & 0x1F;
    uint16_t val_g = (*(src_uchar + 1) >> 2) & 0x3F;
    uint16_t val_b = (*(src_uchar + 2) >> 3) & 0x1F;
    return ((val_r << 11) | (val_g << 5) | val_b);
}

/* Helper typedef to encapsulate the colour conversion function
 * signatures */
typedef uint16_t (* std_clr_2_lcd_clr_fn)(uint8_t *src_uchar);

static void _DisplayImage(const void *data, const uint32_t width,
                          const uint32_t height, const uint32_t channels,
                          const uint32_t pos_x, const uint32_t pos_y,
                          const uint32_t downsample_factor)
{
    if (s_lcd)
    {
        uint32_t i, j = 0; /* for loops */
        const uint32_t x_incr = channels * downsample_factor; /* stride. */
        const uint32_t y_incr = channels * width * (downsample_factor - 1); /* skip rows. */
        uint8_t *src_unsigned = (uint8_t *)data; /* temporary pointer. */
        std_clr_2_lcd_clr_fn cvt_clr_fn = 0; /* colour conversion function. */
        uint16_t *pu16Buf = (uint16_t *)SHADOW_BUFFER_START;

        /* Based on number of channels, we decide which of the above functions to use. */
        switch (channels)
        {
        case 1:
            cvt_clr_fn = _Gray8_to_RGB565;
            break;

        case 2:
            break;

        case 3:
            cvt_clr_fn = _RGB888_to_RGB565;
            break;

        default:
            printf_err("number of channels not supported by display\n");
            return;
        }

        /* Loop over the image. */
        if (channels != 2)
        {
            for (j = height; j != 0; j -= downsample_factor)
            {
                for (i = width; i != 0; i -= downsample_factor)
                {
                    *pu16Buf = cvt_clr_fn(src_unsigned);
                    src_unsigned += x_incr;
                    pu16Buf++;
                }

                /* Skip rows if needed. */
                src_unsigned += y_incr;
            }
        }

        {
            struct rt_device_rect_info rect;

            rect.x = pos_x + SHADOW_BUFFER_X_OFFSET;
            rect.y = pos_y + SHADOW_BUFFER_Y_OFFSET;
            rect.width = width / downsample_factor;
            rect.height = height / downsample_factor;
            rect.framebuffer = (channels == 2) ? (rt_uint8_t *)data : (rt_uint8_t *)SHADOW_BUFFER_START;
            rt_device_control(s_lcd, RTGRAPHIC_CTRL_RECT_UPDATE2, &rect);
        }
    }
}

static void _DrawChar(
    uint32_t x, uint32_t y,
    uint32_t cw, uint32_t ch,
    uint8_t *c)
{
    if (s_lcd)
    {
        uint32_t i, j, k, pixs;
        uint16_t *pu16Buf = (uint16_t *)SHADOW_BUFFER_START;

        /* Heatlh check: out of bounds? */
        if ((x + cw) > SHADOW_BUFFER_WIDTH || (y + ch) > SHADOW_BUFFER_HEIGHT)
        {
            return;
        }

        k  = (cw + 7) / 8;
        if (k == 1)
        {
            for (j = 0; j < ch; ++j)
            {
                pixs = *(uint8_t *)c;
                c += 1;

                for (i = 0; i < cw; ++i)
                {
                    *pu16Buf = Color[(pixs >> i) & 1];
                    pu16Buf++;
                }
            }
        }
        else if (k == 2)
        {
            for (j = 0; j < ch; ++j)
            {
                pixs = *(uint16_t *)c;
                c += 2;

                for (i = 0; i < cw; ++i)
                {
                    *pu16Buf = Color[(pixs >> i) & 1];
                    pu16Buf++;
                }
            }
        }

        {
            struct rt_device_rect_info rect;

            rect.x = x + SHADOW_BUFFER_X_OFFSET;
            rect.y = y + SHADOW_BUFFER_Y_OFFSET;
            rect.width = cw;
            rect.height = ch;
            rect.framebuffer = SHADOW_BUFFER_START;
            rt_device_control(s_lcd, RTGRAPHIC_CTRL_RECT_UPDATE2, &rect);
        }
    }
}


static void _DisplayChar(
    uint32_t ln, uint32_t col,
    uint8_t fi, uint8_t c)
{
    c -= 32;
    switch (fi)
    {
    case 0: /* Font 9 x 15. */
        _DrawChar(col * 9, ln * 15, 9, 15,
                  (uint8_t *)&Font_9x15_h[c * 15]);
        break;
    }
}

static void _DisplayString(
    uint32_t ln, uint32_t col,
    uint8_t fi, char *s)
{
    while (*s)
    {
        _DisplayChar(ln, col++, fi, *s++);
    }
}

int lcd_fini(void)
{
    if (s_lcd)
        rt_device_close(s_lcd);

    s_lcd = RT_NULL;
}

int lcd_init(void)
{
    rt_err_t result;

    s_lcd = rt_device_find(MLEVK_UC_LCD_RENDERING_DEVICE);
    if (s_lcd == RT_NULL)
    {
        rt_kprintf("Fail to find %s\n", MLEVK_UC_LCD_RENDERING_DEVICE);
        goto fail_lcd_init;
    }

    /* get graphics info */
    result = rt_device_control(s_lcd, RTGRAPHIC_CTRL_GET_INFO, &s_info);
    if ((result != RT_EOK) && (s_info.framebuffer == RT_NULL))
    {
        rt_kprintf("Fail to get graphics s_info.\n");
        goto fail_lcd_init;
    }

    /* Open device */
    result = rt_device_open(s_lcd, 0);
    if (result != RT_EOK)
    {
        rt_kprintf("Fail to open ml %s device.\n", MLEVK_UC_LCD_RENDERING_DEVICE);
        goto fail_lcd_init;
    }

    //lcd_clear(Black);

    return 0;

fail_lcd_init:

    s_lcd = RT_NULL;

    return -1;
}

int lcd_display_image(const uint8_t *data, const uint32_t width,
                      const uint32_t height, const uint32_t channels,
                      const uint32_t pos_x, const uint32_t pos_y,
                      const uint32_t downsample_factor)
{
    /* Health checks */
    assert(data);
    if ((pos_x + (width / downsample_factor) > SHADOW_BUFFER_WIDTH) ||
            (pos_y + (height / downsample_factor) > SHADOW_BUFFER_HEIGHT))
    {
        printf_err("Invalid image size for given location!\n");
        return 1;
    }

    if (1 == channels || 2 == channels || 3 == channels)
    {
        _DisplayImage(data, width, height, channels, pos_x, pos_y, downsample_factor);
    }
    else
    {
        printf_err("Only single and three channel images are supported!\n");
        return 1;
    }

    return 0;
}

int lcd_display_text(const char *str, const size_t str_sz,
                     const uint32_t pos_x, const uint32_t pos_y,
                     const bool allow_multiple_lines)
{
    /* We use a font 0 which is 9x15. */
    const uint32_t x_span =  9; /* Each character is this  9 pixels "wide". */
    const uint32_t y_span = 15; /* Each character is this 15 pixels "high". */

    if (str_sz == 0)
    {
        return 1;
    }

    /* If not within the LCD bounds, return error. */
    if (((pos_x + x_span) > SHADOW_BUFFER_WIDTH) || ((pos_y + y_span) > SHADOW_BUFFER_HEIGHT))
    {
        return 1;
    }
    else
    {
        const uint8_t font_idx = 0; /* We are using the custom font = 0 */

        const uint32_t col = pos_x / x_span;
        const uint32_t max_cols = SHADOW_BUFFER_WIDTH / x_span - 1;
        const uint32_t max_lines = SHADOW_BUFFER_HEIGHT / y_span - 1;

        uint32_t i = 0;
        uint32_t current_line = pos_y / y_span;
        uint32_t current_col = col;

        /* Display the string on the LCD. */
        for (i = 0; i < str_sz; ++i)
        {
            if (allow_multiple_lines)
            {

                /* If the next character won't fit. */
                if (current_col > max_cols)
                {
                    current_col = col;

                    /* If the next line won't fit. */
                    if (++current_line  > max_lines)
                    {
                        return 1;
                    }
                }
            }

            _DisplayChar(current_line, current_col++, font_idx, str[i]);
        }
    }

    return 0;
}

int lcd_display_box(const uint32_t pos_x, const uint32_t pos_y,
                    const uint32_t width, const uint32_t height, const uint16_t color)
{
    if (s_lcd)
    {
        /* If not within the LCD bounds, return error. */
        if (pos_x > SHADOW_BUFFER_WIDTH || pos_y > SHADOW_BUFFER_HEIGHT)
        {
            return 1;
        }
        else
        {
            uint32_t x, y;
            uint16_t *pu16Buf = (uint16_t *)SHADOW_BUFFER_START;
            struct rt_device_rect_info rect;

            for (y = 0; y < height; y++)
            {
                for (x = 0; x < width; x++)
                {
                    *pu16Buf = color;
                    pu16Buf++;
                }
            }

            rect.x = pos_x + SHADOW_BUFFER_X_OFFSET;
            rect.y = pos_y + SHADOW_BUFFER_Y_OFFSET;
            rect.width = width;
            rect.height = height;
            rect.framebuffer = SHADOW_BUFFER_START;
            rt_device_control(s_lcd, RTGRAPHIC_CTRL_RECT_UPDATE2, &rect);
        }

    }
    return 0;
}

int lcd_clear(const uint16_t color)
{
    if (s_lcd)
    {
        uint32_t x, y;
        uint16_t *pu16Buf = (uint16_t *)SHADOW_BUFFER_START;
        struct rt_device_rect_info rect;

        for (y = 0; y < SHADOW_BUFFER_HEIGHT; y++)
        {
            for (x = 0; x < SHADOW_BUFFER_WIDTH; x++)
            {
                *pu16Buf = color;
                pu16Buf++;
            }
        }

        rect.x = 0 + SHADOW_BUFFER_X_OFFSET;
        rect.y = 0 + SHADOW_BUFFER_Y_OFFSET;
        rect.width = SHADOW_BUFFER_WIDTH;
        rect.height = SHADOW_BUFFER_HEIGHT;
        rect.framebuffer = SHADOW_BUFFER_START;
        rt_device_control(s_lcd, RTGRAPHIC_CTRL_RECT_UPDATE2, &rect);
    }

    return 0;
}

int lcd_set_text_color(const uint16_t color)
{
    _SetTextColor(color);
    return 0;
}

