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
#include "mmu.h"

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

#ifndef UNCACHEABLE
    #define UNCACHEABLE    (0x80000000)
#endif

static uint32_t u32FirstFlush = 0;
static struct rt_device_graphic_info info = {0};
static void *buf3_next = RT_NULL;

#if defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==8)

#include "lv_gpu_n9h30_2dge.h"

static void lv_port_disp_full_refresh(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    lv_disp_draw_buf_t *pdb = disp_drv->draw_buf;

    mmu_clean_dcache((uint32_t)color_p, pdb->size * sizeof(lv_color_t));

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
    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_port_disp_partial(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t area_w = lv_area_get_width(area);
    int32_t area_h = lv_area_get_height(area);

    lv_draw_sw_blend_dsc_t blend_flush = {0};
    lv_draw_ctx_t draw_flush = {0};
    lv_area_t flush_area = {0, 0, info.width - 1, info.height - 1 };

    //rt_kprintf("[nu_flush %d %08x] %dx%d %d %d\n", lv_area_get_size(area), color_p, area_w, area_h, area->x1, area->y1 );

    blend_flush.blend_area = area;
    blend_flush.src_buf    = color_p;
    blend_flush.mask_buf   = NULL;
    blend_flush.opa        = LV_OPA_MAX;
    blend_flush.blend_mode = LV_BLEND_MODE_NORMAL;

    draw_flush.buf         = info.framebuffer;
    draw_flush.clip_area   = &flush_area;
    draw_flush.buf_area    = &flush_area;

    lv_draw_n9h30_2dge_blend(&draw_flush, (const lv_draw_sw_blend_dsc_t *)&blend_flush);

    if (!u32FirstFlush)
    {
        /* Point to first screen buffer address. */
        rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_PAN_DISPLAY, info.framebuffer);

        /* Enable backlight at first flushing. */
        rt_device_control((rt_device_t)disp_drv->user_data, RTGRAPHIC_CTRL_POWERON, RT_NULL);
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
#if (LV_GPU_USE_N9H30_2DGE==1)
    disp_drv.draw_ctx_init = lv_draw_n9h30_2dge_ctx_init;
    disp_drv.draw_ctx_deinit = lv_draw_n9h30_2dge_ctx_init;
    disp_drv.draw_ctx_size = sizeof(lv_draw_n9h30_2dge_ctx_t);
#endif

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
    mmu_clean_dcache((uint32_t)px_map, lv_area_get_size(area) * (LV_COLOR_DEPTH / 8));

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
    }

    lv_display_flush_ready(disp);
}


static volatile int32_t s_i32GDMAIdx = -1;

static bool _isConstrained(uint32_t px_size, const lv_area_t *area)
{
    /* 2DGE limitation? */
    if (px_size == 2)
    {
        uint32_t dst_stride = px_size * lv_area_get_width(area);

        /* Check Hardware constraint: The stride must be a word-alignment. */
        bool bAlignedWord = ((dst_stride & 0x3) == 0) &&
                            (((area->x1 * px_size) & 0x3) == 0) ? true : false;

        if (!bAlignedWord)
            goto exit_isConstrained;
    }

    return false;

exit_isConstrained:

    return true;
}

#include "nu_2d.h"

static void lv_port_disp_partial(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t px_size = info.bits_per_pixel / 8;

    /* Update dirty region. */
#if defined(BSP_USING_GDMA)
    if ((lv_area_get_size(area) == (info.width * info.height)))
    {
        /* Only catch full-screen case due to GDMA constraint(No stride feature). */
#define FULL_SCREEN_BYTE_COUNT (info.width * info.height * px_size)

        void GDMA_M2M_Transfer(uint32_t u32DestAddr, uint32_t u32SrcAddr, uint32_t u32TransferSize);
        GDMA_M2M_Transfer((uint32_t)info.framebuffer, (uint32_t)px_map, FULL_SCREEN_BYTE_COUNT);

        s_i32GDMAIdx = 0;
    }
    else
#endif
#if LV_USE_DRAW_2DGE
        if (!_isConstrained(px_size, area))
        {
            int32_t dest_w = lv_area_get_width(area);
            int32_t dest_h = lv_area_get_height(area);
            int32_t dest_stride = info.width * px_size;

            ge2dInit(px_size * 8, info.width, info.height, (void *)info.framebuffer);

            ge2dBitblt_SetAlphaMode(0, 0, 0);
            ge2dBitblt_SetDrawMode(0, 0, 0);

            const uint8_t *dest_start_buf = info.framebuffer + (area->y1 * dest_stride);

            /* Clean. then invalidate cache line of dest buffer to memory */
            mmu_clean_invalidated_dcache((uint32_t)dest_start_buf, dest_stride * dest_h);

            /* Flush cache line of src buffer to memory */
            mmu_clean_dcache((uint32_t)px_map, dest_w * dest_h * px_size);

            ge2dSpriteBlt_Screen(area->x1, area->y1, dest_w, dest_h, (void *)px_map);
        }
        else
#endif
        {
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
                    pDisp[x] = pSrc[x];

                pDisp += LV_HOR_RES_MAX;
                pSrc += w;
            }
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

static void lv_port_disp_flush_wait(lv_display_t *disp)
{
    if (s_i32GDMAIdx == 0) // Is GDMA channel?
    {
        void GDMA_WaitForCompletion(void);
        GDMA_WaitForCompletion();

        s_i32GDMAIdx = -1;
    }
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
    lv_display_set_flush_wait_cb(disp, lv_port_disp_flush_wait);       /*Set a flush wait callback*/
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
