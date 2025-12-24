/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-18     Meco Man     The first version
 * 2021-12-17     Wayne        Add input event
 */
#include <lvgl.h>
#include <stdbool.h>
#include <rtdevice.h>
#include "touch.h"

static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;

void nu_touch_inputevent_cb(rt_int16_t x, rt_int16_t y, rt_uint8_t state)
{
    switch (state)
    {
    case RT_TOUCH_EVENT_UP:
        last_state = LV_INDEV_STATE_RELEASED;
        break;
    case RT_TOUCH_EVENT_MOVE:
    case RT_TOUCH_EVENT_DOWN:
        last_x = x;
        last_y = y;
        last_state = LV_INDEV_STATE_PRESSED;
        break;
    }
}

#if defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==8)

static void input_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = last_state;
}

RT_WEAK void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /* Basic initialization */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = input_read;

    /* Register the driver in LVGL and save the created input device object */
    lv_indev_drv_register(&indev_drv);
}

#elif defined(LVGL_VERSION_MAJOR) && (LVGL_VERSION_MAJOR==9)

RT_WEAK void lv_port_other_indev_init(void)
{

}

static void input_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = last_state;
}

RT_WEAK void lv_port_indev_init(void)
{
    lv_indev_t *lv_indev_touch = lv_indev_create();

    lv_indev_set_type(lv_indev_touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_indev_touch,  input_read);

    lv_port_other_indev_init();
}

#endif
