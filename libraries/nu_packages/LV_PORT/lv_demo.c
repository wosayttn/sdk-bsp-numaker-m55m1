/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-6-1       Wayne         First version
 */

#include <lvgl.h>

RT_WEAK void lv_user_gui_init(void)
{
#if LV_USE_DRAW_2DGE
    void lv_draw_2dge_init(void);
    lv_draw_2dge_init();
#endif

#if LV_USE_DRAW_GDMA
    void lv_draw_gdma_init(void);
    lv_draw_gdma_init();
#endif

#if defined(PKG_USING_LVGL_SQUARELINE)
    extern void ui_init(void);
    ui_init();
#else

    /* display demo; you may replace with your LVGL application at here and disable related definitions. */
#if LV_USE_DEMO_BENCHMARK
    void lv_demo_benchmark(void);
    lv_demo_benchmark();

#elif LV_USE_DEMO_WIDGETS
    void lv_demo_widgets(void);
    lv_demo_widgets();

#if defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==9)
    void lv_demo_widgets_start_slideshow(void);
    lv_demo_widgets_start_slideshow();
#endif

#elif (LV_USE_DEMO_MUSIC || LV_USE_DEMO_RTT_MUSIC)
    void lv_demo_music(void);
    lv_demo_music();

#endif

#endif
}
