/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-17     Wayne        The first version
 */
#include <lvgl.h>
#include "drv_common.h"

#define LOG_TAG             "lvgl.disp"
#define DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL DBG_ERROR
#define DBG_COLOR
#include <rtdbg.h>

#if !defined(NU_PKG_LVGL_RENDERING_LAYER)
    #define NU_PKG_LVGL_RENDERING_LAYER "lcd"
#endif

#if !defined(NU_PKG_LVGL_RENDERING_FULL_REFRESH)
    #define NU_PKG_LVGL_RENDERING_FULL_REFRESH  0
#endif

static uint32_t u32FirstFlush = 0;
static struct rt_device_graphic_info info = {0};
static void *buf3_next = RT_NULL;

#if defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==8)

static void lv_port_disp_full_refresh(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    lv_disp_draw_buf_t *pdb = disp_drv->draw_buf;

    rt_hw_cpu_dcache_clean(color_p, pdb->size * sizeof(lv_color_t));

    /* Use PANDISPLAY without H/W copying */
    rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_PAN_DISPLAY, color_p);

    /* Anti-tearing */
    {
        if (buf3_next)
        {
            /* vsync-none: Use triple screen-sized buffers. */
            if (pdb->buf1 == color_p)
                pdb->buf1 = buf3_next;
            else
                pdb->buf2 = buf3_next;

            pdb->buf_act = buf3_next;
            buf3_next = color_p;
        }
        else
        {
            /* vsync-after: Use ping-pong screen-sized buffers only.*/
            rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_WAIT_VSYNC, RT_NULL);
        }
    }

    if (!u32FirstFlush)
    {
        /* Enable backlight at first flushing. */
        rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_POWERON, RT_NULL);
        u32FirstFlush = 1;

#if defined(NU_OVERLAY_COLOR_KEY)
        int nu_disp_overlay_set_colkey(uint32_t u32ColKey);
        nu_disp_overlay_set_colkey(NU_OVERLAY_COLOR_KEY);
#endif

    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_partial(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    /* Rendering */
    int32_t x, y;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    lv_color_t *pDisp;

    pDisp = (lv_color_t *)info.framebuffer + (info.width * area->y1 + area->x1);

    /* Update dirty region. */
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            pDisp[x] = color_p[x];
        }
        pDisp += info.width;
        color_p += w;
    }

    if (!u32FirstFlush)
    {
        /* Point to first screen buffer address. */
        rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_PAN_DISPLAY, info.framebuffer);

        /* Enable backlight at first flushing. */
        rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_POWERON, RT_NULL);

#if defined(NU_OVERLAY_COLOR_KEY)
        int nu_disp_overlay_set_colkey(uint32_t u32ColKey);
        nu_disp_overlay_set_colkey(NU_OVERLAY_COLOR_KEY);
#endif

        u32FirstFlush = 1;
    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_perf_monitor(struct _lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
    rt_kprintf("Elapsed: %dms, Pixel: %d, Bytes:%d\n", time, px, px * sizeof(lv_color_t));
}

RT_WEAK void lv_port_disp_init(void)
{
    /* A static or global variable to store the buffers */
    static lv_disp_draw_buf_t disp_buf;
    /* Descriptor of a display driver */
    static lv_disp_drv_t disp_drv;
    static rt_device_t lcd_device = 0;

    rt_err_t result;
    void *buf1 = RT_NULL;
    void *buf2 = RT_NULL;
    uint32_t u32FBSize;

    lcd_device = rt_device_find(NU_PKG_LVGL_RENDERING_LAYER);
    if (lcd_device == RT_NULL)
    {
        LOG_E("error!");
        return;
    }

    /* get framebuffer address */
    result = rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_INFO, &info);
    if (result != RT_EOK && info.framebuffer == RT_NULL)
    {
        LOG_E("error!");
        /* get device information failed */
        return;
    }

    /* Disable backlight at startup. */
    rt_device_control(lcd_device, RTGRAPHIC_CTRL_POWEROFF, RT_NULL);

    RT_ASSERT(info.bits_per_pixel == 8 || info.bits_per_pixel == 16 ||
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    u32FBSize = info.height * info.width * (info.bits_per_pixel / 8);

    /* Basic initialization */
    lv_disp_drv_init(&disp_drv);

    /* Set the resolution of the display */
    disp_drv.hor_res = info.width;
    disp_drv.ver_res = info.height;

#if defined(NU_PKG_LVGL_RENDERING_FULL_REFRESH) && (NU_PKG_LVGL_RENDERING_FULL_REFRESH==1)
    disp_drv.full_refresh = 1;
    disp_drv.flush_cb = lv_port_disp_full_refresh;
    buf1 = (void *)((uint32_t)info.framebuffer & (~UNCACHEABLE));
    buf2 = (void *)((uint32_t)buf1 + u32FBSize);
    buf3_next = (void *)((uint32_t)buf2 + u32FBSize);

    LOG_I("LVGL: Triple screen-sized buffers(full_refresh) - buf1@%08x, buf2@%08x, buf3_next@%08x", buf1, buf2, buf3_next);
#else
    disp_drv.full_refresh = 0;
    disp_drv.flush_cb = lv_port_disp_partial;
    buf1 = (void *)(((uint32_t)info.framebuffer & (~UNCACHEABLE)) + u32FBSize);
    buf2 = (void *)((uint32_t)buf1 + u32FBSize);

    LOG_I("LVGL: Two screen-sized buffers - buf1@%08x, buf2@%08x", buf1, buf2);
#endif

    /* Initialize `disp_buf` with the buffer(s). */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, info.width * info.height);

    /* Set a display buffer */
    disp_drv.draw_buf = &disp_buf;

    /* Write the internal buffer (draw_buf) to the display */
    disp_drv.flush_cb = lv_port_disp_partial;

    /* Register user_data */
    disp_drv.user_data = lcd_device;

    /* Called after every refresh cycle to tell the rendering and flushing time + the number of flushed pixels */
    //disp_drv.monitor_cb = lv_port_disp_perf_monitor;

    /* Finally register the driver */
    lv_disp_drv_register(&disp_drv);

    result = rt_device_open(lcd_device, 0);
    if (result != RT_EOK)
    {
        LOG_E("error!");
        return;
    }
}

#elif defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==9)

static void lv_port_disp_full_refresh(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    rt_hw_cpu_dcache_clean(px_map, lv_area_get_size(area) * (LV_COLOR_DEPTH / 8));

    /* Use PANDISPLAY without H/W copying */
    rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_PAN_DISPLAY, px_map);

    /* Anti-tearing */
    {
        /* vsync-after: Use ping-pong screen-sized buffers only.*/
        rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_WAIT_VSYNC, RT_NULL);
    }

    if (!u32FirstFlush)
    {
        /* Enable backlight at first flushing. */
        rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_POWERON, RT_NULL);
        u32FirstFlush = 1;

#if defined(NU_OVERLAY_COLOR_KEY)
        int nu_disp_overlay_set_colkey(uint32_t u32ColKey);
        nu_disp_overlay_set_colkey(NU_OVERLAY_COLOR_KEY);
#endif
    }

    lv_display_flush_ready(disp);
}

static void lv_port_disp_partial(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /* Update dirty region. */
    int32_t x, y;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

#if (LV_COLOR_DEPTH==16)
        uint16_t *pDisp = (uint16_t *)info.framebuffer + (LV_HOR_RES_MAX * area->y1 + area->x1);
        uint16_t *pSrc = (uint16_t *)px_map;
#elif (LV_COLOR_DEPTH==32)
        uint32_t *pDisp = (uint32_t *)info.framebuffer + (LV_HOR_RES_MAX * area->y1 + area->x1);
        uint32_t *pSrc = (uint32_t *)px_map;
#endif

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            pDisp[x] = pSrc[x];
        }
        pDisp += LV_HOR_RES_MAX;
        pSrc += w;
    }

    if (!u32FirstFlush)
    {
        /* Point to first screen buffer address. */
        rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_PAN_DISPLAY, info.framebuffer);

        /* Enable backlight at first flushing. */
        rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_POWERON, RT_NULL);
        u32FirstFlush = 1;
    }

    lv_display_flush_ready(disp);
}

RT_WEAK void lv_port_disp_init(void)
{
    rt_err_t result;
    static rt_device_t lcd_device = 0;
    lv_display_t *disp;
    void *buf1, *buf2;
    uint32_t u32FBSize;

    lcd_device = rt_device_find(NU_PKG_LVGL_RENDERING_LAYER);
    if (lcd_device == 0)
    {
        LOG_E("error!");
        RT_ASSERT(0);
    }

    /* get lcd info */
    result = rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_INFO, &info);
    if (result != RT_EOK && info.framebuffer == RT_NULL)
    {
        LOG_E("error!");
        /* get device information failed */
        RT_ASSERT(0);
    }

    RT_ASSERT(info.bits_per_pixel == 8 || info.bits_per_pixel == 16 ||
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    u32FBSize = info.height * info.width * (info.bits_per_pixel / 8);
    buf1 = (void *)((uint32_t)info.framebuffer & (~UNCACHEABLE));
    buf2 = (void *)((uint32_t)buf1 + u32FBSize);

    /*Create display instance*/
    disp = lv_display_create(info.width, info.height);
    RT_ASSERT(disp != NULL);

#if defined(NU_PKG_LVGL_RENDERING_FULL_REFRESH) && (NU_PKG_LVGL_RENDERING_FULL_REFRESH==1)
    lv_color_format_t cf = lv_display_get_color_format(disp);

    lv_display_set_flush_cb(disp, lv_port_disp_full_refresh); /*Set a flush callback to draw to the display*/
    lv_display_set_buffers(disp, buf1, buf2, u32FBSize, LV_DISPLAY_RENDER_MODE_FULL); /*Set an initialized buffer*/

    LOG_I("Use two screen-size buffer, buf1: 0x%08x, buf2: 0x%08x: 0x%08x", buf1, buf2);
#else
    buf3_next = (void *)(buf2 + u32FBSize);

    lv_display_set_flush_cb(disp, lv_port_disp_partial);               /*Set a flush callback to draw to the display*/
    lv_display_set_buffers(disp, buf2, buf3_next, u32FBSize, LV_DISPLAY_RENDER_MODE_PARTIAL); /*Set an initialized buffer*/

    LOG_I("Use two screen-size shadow buffer, 0x%08x, 0x%08x.", buf2, buf3_next);
#endif

    /*Set user-data*/
    lv_display_set_user_data(disp, lcd_device);

    result = rt_device_open(lcd_device, 0);
    if (result != RT_EOK)
    {
        LOG_E("error!");
        RT_ASSERT(0);
    }
}

#endif
