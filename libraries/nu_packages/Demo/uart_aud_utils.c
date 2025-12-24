#include "rtthread.h"

#if defined(BSP_USING_UART)

#include "rtdevice.h"

#define DBG_TAG  "uart_aud"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <stdlib.h>
#include <string.h>

#include "drv_uart.h"

static struct serial_configure s_sUartConfig = RT_SERIAL_CONFIG_DEFAULT;

static int send_message(rt_device_t serial, int num)
{
    char txbuf[32];
    int i;

    for (i = 0; i < num; i++)
    {
        /* Say Hello */
        rt_snprintf(&txbuf[0], sizeof(txbuf), "Hello World - %d!\r\n", i + 1);
        rt_device_write(serial, 0, &txbuf[0], rt_strlen(txbuf));
    }

    return 0;
}

static void uart_aud(int argc, char **argv)
{
    static rt_device_t dev = RT_NULL;

    /* If the number of arguments less than 2 */
    if (argc < 2)
    {
        rt_kprintf("\n");
        rt_kprintf("uart_aud [OPTION] [PARAM]\n");
        rt_kprintf("         probe <dev_name>      Probe sensor by given name\n");
        rt_kprintf("         send                  send [num] times message (default 1)\n");
        return ;
    }
    else if (!strcmp(argv[1], "send"))
    {
        rt_uint32_t num = 1;

        if (dev == RT_NULL)
        {
            LOG_W("Please probe sensor device first!");
            return ;
        }

        if (argc == 3)
        {
            num = atoi(argv[2]);
        }

        send_message(dev, num);
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "probe"))
        {
            rt_device_t new_dev;

            if (dev)
            {
                rt_device_close(dev);
            }

            new_dev = rt_device_find(argv[2]);
            if (new_dev == RT_NULL)
            {
                LOG_E("Can't find device:%s", argv[2]);
                return;
            }

            rt_device_control(new_dev, RT_DEVICE_CTRL_CONFIG, &s_sUartConfig);

            if (rt_device_open(new_dev, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
            {
                LOG_E("open device failed!");
                return;
            }

            /* Nuvoton private command */
            nu_uart_set_rs485aud((struct rt_serial_device *)new_dev, RT_FALSE);

            dev = new_dev;
        }
        else if (dev == RT_NULL)
        {
            LOG_W("Please probe can device first!");
            return ;
        }
    }
    else
    {
        LOG_W("Unknown command, please enter 'uart_aud' get help information!");
    }

}
MSH_CMD_EXPORT(uart_aud, Nuvoton uart-aud );

#endif //defined(BSP_USING_UART)
