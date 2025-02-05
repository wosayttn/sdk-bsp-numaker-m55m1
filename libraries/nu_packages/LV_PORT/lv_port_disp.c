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

#define LOG_TAG             "lvgl.disp"
#define DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL DBG_ERROR
#define DBG_COLOR
#include <rtdbg.h>

#if !defined(NU_PKG_LVGL_RENDERING_LAYER)
    #define NU_PKG_LVGL_RENDERING_LAYER "lcd"
#endif

#if defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==8)

static void lv_port_disp_partial(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    /* Rendering */
    struct rt_device_rect_info rect;

    rect.x = area->x1;
    rect.y = area->y1;
    rect.width = area->x2 - area->x1 + 1;
    rect.height = area->y2 - area->y1 + 1;

    rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_RECT_UPDATE, &rect);
    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_perf_monitor(struct _lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
    rt_kprintf("Elapsed: %dms, Pixel: %d, Bytes:%d\n", time, px, px * sizeof(lv_color_t));
}

RT_WEAK void lv_port_disp_init(void)
{
    rt_err_t result;
    /* A static or global variable to store the buffers */
    static lv_disp_draw_buf_t disp_buf;
    /* Descriptor of a display driver */
    static lv_disp_drv_t disp_drv;
    static rt_device_t lcd_device = 0;
    struct rt_device_graphic_info info;

    void *buf1 = RT_NULL;

    lcd_device = rt_device_find(NU_PKG_LVGL_RENDERING_LAYER);
    if (lcd_device == 0)
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

    RT_ASSERT(info.bits_per_pixel == 8 || info.bits_per_pixel == 16 ||
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    buf1 = (void *)info.framebuffer;
    rt_kprintf("LVGL: Use one buffers - buf1@%08x, size: %d bytes\n", buf1, info.smem_len);

    /*Initialize `disp_buf` with the buffer(s).*/
    lv_disp_draw_buf_init(&disp_buf, buf1, RT_NULL, info.smem_len / (info.bits_per_pixel / 8));

    result = rt_device_open(lcd_device, 0);
    if (result != RT_EOK)
    {
        LOG_E("error!");
        return;
    }

    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    /*Set a display buffer*/
    disp_drv.draw_buf = &disp_buf;

    /*Write the internal buffer (draw_buf) to the display*/
    disp_drv.flush_cb = lv_port_disp_partial;

    /*Register user_data*/
    disp_drv.user_data = lcd_device;

    /* Called after every refresh cycle to tell the rendering and flushing time + the number of flushed pixels */
    //disp_drv.monitor_cb = lv_port_disp_perf_monitor;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

#elif defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==9)

static void lv_port_disp_partial(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /* Rendering */
    struct rt_device_rect_info rect;

    rect.x = area->x1;
    rect.y = area->y1;
    rect.width = area->x2 - area->x1 + 1;
    rect.height = area->y2 - area->y1 + 1;

    /* Update dirty region. */
    rt_device_control((rt_device_t)lv_display_get_user_data(disp), RTGRAPHIC_CTRL_RECT_UPDATE, &rect);
    lv_disp_flush_ready(disp);
}

RT_WEAK void lv_port_disp_init(void)
{
    rt_err_t result;
    static rt_device_t lcd_device = 0;
    struct rt_device_graphic_info info;
    lv_display_t *disp;

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

    result = rt_device_open(lcd_device, 0);
    if (result != RT_EOK)
    {
        LOG_E("error!");
        RT_ASSERT(0);
    }

    rt_kprintf("LVGL: Use one buffers - buf1@%08x, size: %d bytes\n", info.framebuffer, info.smem_len);

    /*Create display instance*/
    disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    RT_ASSERT(disp != NULL);

    /*Set user-data*/
    lv_display_set_user_data(disp, lcd_device);

    /*Set a flush callback to draw to the display*/
    lv_display_set_flush_cb(disp, lv_port_disp_partial);

    /*Set an initialized buffer*/
    lv_display_set_buffers(disp, info.framebuffer, NULL, info.smem_len, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

#endif
