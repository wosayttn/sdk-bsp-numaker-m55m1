/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2020-1-16       Wayne        First version
*
******************************************************************************/

#include <rtdevice.h>
#include "board.h"

#if defined(BOARD_USING_NAU8822) && defined(NU_PKG_USING_NAU8822)
#include "acodec_nau8822.h"
S_NU_NAU8822_CONFIG sCodecConfig =
{
    .i2c_bus_name = "i2c3",

    .i2s_bus_name = "sound0",

    .pin_phonejack_en = NU_GET_PININDEX(NU_PD, 1),

    .pin_phonejack_det = NU_GET_PININDEX(NU_PD, 0),
};

int rt_hw_nau8822_port(void)
{
    if (nu_hw_nau8822_init(&sCodecConfig) != RT_EOK)
        return -1;

    return 0;
}
INIT_COMPONENT_EXPORT(rt_hw_nau8822_port);
#endif /* BOARD_USING_NAU8822 */

#if defined(BOARD_USING_LCD_LT7381) && defined(NU_PKG_USING_LT7381_EBI)
#include "drv_ebi.h"
#include "lcd_lt7381.h"
#if defined(PKG_USING_GUIENGINE)
    #include <rtgui/driver.h>
#endif
int rt_hw_lt7381_port(void)
{
    rt_err_t ret = RT_EOK;

    /* Open ebi BOARD_USING_LT7381_EBI_PORT */
    ret = nu_ebi_init(BOARD_USING_LT7381_EBI_PORT, EBI_BUSWIDTH_16BIT, EBI_TIMING_NORMAL, EBI_OPMODE_CACCESS | EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);
    if (ret != RT_EOK)
        return ret;

    /* Optimization timing. */
    EBI_SetBusTiming(BOARD_USING_LT7381_EBI_PORT, EBI_TCTL_RAHDOFF_Msk | EBI_TCTL_WAHDOFF_Msk | (4 << EBI_TCTL_TACC_Pos), EBI_MCLKDIV_2);

    if (rt_hw_lcd_lt7381_ebi_init(EBI_BANK0_BASE_ADDR + BOARD_USING_LT7381_EBI_PORT * EBI_MAX_SIZE) != RT_EOK)
        return -1;

    rt_hw_lcd_lt7381_init();

#if defined(PKG_USING_GUIENGINE)
    rt_device_t lcd_lt7381 = rt_device_find("lcd");
    if (lcd_lt7381)
    {
        rtgui_graphic_set_device(lcd_lt7381);
    }
#endif

    rt_kprintf("BOARD_USING_LT7381_EBI_PORT: %d\n", BOARD_USING_LT7381_EBI_PORT);
    rt_kprintf("BOARD_USING_LT7381_PIN_RESET: %d, (P%c%d)\n",
               BOARD_USING_LT7381_PIN_RESET,
               'A' + NU_GET_PORT(BOARD_USING_LT7381_PIN_RESET),
               NU_GET_PINS(BOARD_USING_LT7381_PIN_RESET));
    rt_kprintf("BOARD_USING_LT7381_PIN_BACKLIGHT: %d, (P%c%d)\n",
               BOARD_USING_LT7381_PIN_BACKLIGHT,
               'A' + NU_GET_PORT(BOARD_USING_LT7381_PIN_BACKLIGHT),
               NU_GET_PINS(BOARD_USING_LT7381_PIN_BACKLIGHT));

    return 0;
}
INIT_COMPONENT_EXPORT(rt_hw_lt7381_port);
#endif /* BOARD_USING_LCD_LT7381 */

#if defined(BOARD_USING_FT5446) && defined(NU_PKG_USING_TPC_FT5446)
#include "ft5446.h"

#define FT5446_RST_PIN   NU_GET_PININDEX(NU_PD, 10)
#define FT5446_IRQ_PIN   NU_GET_PININDEX(NU_PF, 6)

extern int tpc_sample(const char *name);
int rt_hw_ft5446_port(void)
{
    struct rt_touch_config cfg;
    rt_base_t rst_pin = FT5446_RST_PIN;
    cfg.dev_name = "i2c1";
    cfg.irq_pin.pin = FT5446_IRQ_PIN;
    cfg.irq_pin.mode = PIN_MODE_INPUT_PULLUP;
    cfg.user_data = &rst_pin;

    rt_hw_ft5446_init("ft5446", &cfg);
    return tpc_sample("ft5446");

}
INIT_ENV_EXPORT(rt_hw_ft5446_port);
#endif /* #if defined(BOARD_USING_FT5446) && defined(NU_PKG_USING_TPC_FT5446) */

#if defined(BOARD_USING_LCD_FSA506) && defined(NU_PKG_USING_FSA506_EBI)
#include "drv_ebi.h"
#include "lcd_fsa506.h"
#if defined(PKG_USING_GUIENGINE)
    #include <rtgui/driver.h>
#endif
int rt_hw_fsa506_port(void)
{
    rt_err_t ret = RT_EOK;

    /* Open ebi BOARD_USING_FSA506_EBI_PORT */
    ret = nu_ebi_init(BOARD_USING_FSA506_EBI_PORT, EBI_BUSWIDTH_16BIT, EBI_TIMING_SLOW, EBI_OPMODE_CACCESS | EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);
    if (ret != RT_EOK)
        return ret;

    if (rt_hw_lcd_fsa506_ebi_init(EBI_BANK0_BASE_ADDR + BOARD_USING_FSA506_EBI_PORT * EBI_MAX_SIZE) != RT_EOK)
        return -1;

    rt_hw_lcd_fsa506_init();

#if defined(PKG_USING_GUIENGINE)
    rt_device_t lcd_fsa506 = rt_device_find("lcd");
    if (lcd_fsa506)
    {
        rtgui_graphic_set_device(lcd_fsa506);
    }
#endif

    rt_kprintf("BOARD_USING_FSA506_EBI_PORT: %d\n", BOARD_USING_FSA506_EBI_PORT);
    rt_kprintf("BOARD_USING_FSA506_PIN_RESET: %d, (P%c%d)\n",
               BOARD_USING_FSA506_PIN_RESET,
               'A' + NU_GET_PORT(BOARD_USING_FSA506_PIN_RESET),
               NU_GET_PINS(BOARD_USING_FSA506_PIN_RESET));
    rt_kprintf("BOARD_USING_FSA506_PIN_BACKLIGHT: %d, (P%c%d)\n",
               BOARD_USING_FSA506_PIN_BACKLIGHT,
               'A' + NU_GET_PORT(BOARD_USING_FSA506_PIN_BACKLIGHT),
               NU_GET_PINS(BOARD_USING_FSA506_PIN_BACKLIGHT));

    return 0;
}
INIT_COMPONENT_EXPORT(rt_hw_fsa506_port);
#endif /* BOARD_USING_LCD_FSA506 */

#if defined(BOARD_USING_ST1663I) && defined(NU_PKG_USING_TPC_ST1663I)
#include "st1663i.h"

#define ST1663I_RST_PIN   NU_GET_PININDEX(NU_PD, 10)
#define ST1663I_IRQ_PIN   NU_GET_PININDEX(NU_PF, 6)

extern int tpc_sample(const char *name);
int rt_hw_st1663i_port(void)
{
    struct rt_touch_config cfg;
    rt_base_t rst_pin = ST1663I_RST_PIN;
    cfg.dev_name = "i2c1";
    cfg.irq_pin.pin = ST1663I_IRQ_PIN;
    cfg.irq_pin.mode = PIN_MODE_INPUT_PULLUP;
    cfg.user_data = &rst_pin;

    rt_hw_st1663i_init("st1663i", &cfg);
    return tpc_sample("st1663i");

}
INIT_ENV_EXPORT(rt_hw_st1663i_port);
#endif /* if defined(BOARD_USING_ST1663I) && defined(NU_PKG_USING_TPC_ST1663I) */

#if defined(BOARD_USING_SENSOR0)
#include "ccap_sensor.h"

#define SENSOR0_RST_PIN    NU_GET_PININDEX(NU_PG, 11)
#define SENSOR0_PD_PIN     NU_GET_PININDEX(NU_PD, 12)

ccap_sensor_io sIo_sensor0 =
{
    .RstPin          = SENSOR0_RST_PIN,
    .PwrDwnPin       = SENSOR0_PD_PIN,
    .I2cName         = "i2c0"
};

int rt_hw_sensor0_port(void)
{
    rt_kprintf("SENSOR0_RST_PIN: %d, (P%c%d)\n",
               SENSOR0_RST_PIN,
               'A' + NU_GET_PORT(SENSOR0_RST_PIN),
               NU_GET_PINS(SENSOR0_RST_PIN));
    rt_kprintf("SENSOR0_PD_PIN: %d, (P%c%d)\n",
               SENSOR0_PD_PIN,
               'A' + NU_GET_PORT(SENSOR0_PD_PIN),
               NU_GET_PINS(SENSOR0_PD_PIN));

    return  nu_ccap_sensor_create(&sIo_sensor0, (ccap_sensor_id)BOARD_USING_SENSON0_ID, "sensor0");
}
INIT_COMPONENT_EXPORT(rt_hw_sensor0_port);

#endif /* BOARD_USING_SENSOR0 */

#if defined(BOARD_USING_MPU6500) && defined(PKG_USING_MPU6XXX)

#include "sensor_inven_mpu6xxx.h"

int rt_hw_mpu6xxx_port(void)
{
    struct rt_sensor_config cfg;
    rt_base_t mpu_int = NU_GET_PININDEX(NU_PB, 2);

    cfg.intf.dev_name = "lpi2c0";
    cfg.intf.arg = (void *)MPU6XXX_ADDR_DEFAULT;
    cfg.irq_pin.pin = mpu_int;

    return rt_hw_mpu6xxx_init("mpu", &cfg);
}
INIT_APP_EXPORT(rt_hw_mpu6xxx_port);
#endif /* BOARD_USING_MPU6500 */


#if defined(BOARD_USING_ESP8266)

static int rt_hw_esp8266_port(void)
{
    rt_base_t esp_rst_pin = NU_GET_PININDEX(NU_PI, 13);

    /* ESP8266 reset pin PI.13 */
    rt_pin_mode(esp_rst_pin, PIN_MODE_OUTPUT);
    rt_pin_write(esp_rst_pin, 1);

    return 0;
}
INIT_COMPONENT_EXPORT(rt_hw_esp8266_port);

#endif /* BOARD_USING_ESP8266  */

#include "drv_sys.h"
static S_NU_REG s_NuReg_arr[] =
{
    /* PDMA0/1 */
    NUREG_EXPORT(CLK_PDMACTL, CLK_PDMACTL_PDMA0CKEN_Msk, CLK_PDMACTL_PDMA0CKEN_Msk),
    NUREG_EXPORT(CLK_PDMACTL, CLK_PDMACTL_PDMA1CKEN_Msk, CLK_PDMACTL_PDMA1CKEN_Msk),

    {0}
};

void nu_check_register(void)
{
    nu_sys_check_register(&s_NuReg_arr[0]);
}
MSH_CMD_EXPORT(nu_check_register, Check registers);
