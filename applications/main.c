/**************************************************************************//**
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2020-1-16       Wayne        First version
*
******************************************************************************/

#include <rtconfig.h>
#include <rtdevice.h>
#include <drv_gpio.h>
#include "NuMicro.h"

#if defined(RT_USING_PIN)
/* defined the LEDR pin: PH4 */
#define LEDR   NU_GET_PININDEX(NU_PH, 4)

/* defined the LEDY pin: PD6 */
#define LEDY   NU_GET_PININDEX(NU_PD, 6)

/* defined the LEDG pin: PD5 */
#define LEDG   NU_GET_PININDEX(NU_PD, 5)

/* defined the BTN0 pin: PI11 */
#define BTN0   NU_GET_PININDEX(NU_PI, 11)

/* defined the BTN1 pin: PH1 */
#define BTN1   NU_GET_PININDEX(NU_PH, 1)

static uint32_t u32Key1 = BTN0;
static uint32_t u32Key2 = BTN1;

void nu_button_cb(void *args)
{
    uint32_t u32Key = *((uint32_t *)(args));

    switch (u32Key)
    {
    case BTN0:
        rt_pin_write(LEDR, PIN_HIGH);
        break;
    case BTN1:
        rt_pin_write(LEDR, PIN_LOW);
        break;
    }
}
#endif

int main(int argc, char **argv)
{
#if defined(RT_USING_PIN)
    /* set pin mode to output */
    rt_pin_mode(LEDR, PIN_MODE_OUTPUT);
    rt_pin_mode(LEDY, PIN_MODE_OUTPUT);
    rt_pin_mode(LEDG, PIN_MODE_OUTPUT);

    /* set BTN0 pin mode to input */
    rt_pin_mode(BTN0, PIN_MODE_INPUT_PULLUP);

    /* set BTN1 pin mode to input */
    rt_pin_mode(BTN1, PIN_MODE_INPUT_PULLUP);

    rt_pin_attach_irq(BTN0, PIN_IRQ_MODE_FALLING, nu_button_cb, &u32Key1);
    rt_pin_irq_enable(BTN0, PIN_IRQ_ENABLE);

    rt_pin_attach_irq(BTN1, PIN_IRQ_MODE_FALLING, nu_button_cb, &u32Key2);
    rt_pin_irq_enable(BTN1, PIN_IRQ_ENABLE);

    while (1)
    {
        rt_pin_write(LEDG, PIN_LOW);
        rt_pin_write(LEDY, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LEDG, PIN_HIGH);
        rt_pin_write(LEDY, PIN_LOW);
        rt_thread_mdelay(500);
    }
#endif

    return 0;
}
