/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-9-19       Wayne        First version
*
******************************************************************************/

#include <rtthread.h>

#if defined(RT_USING_CAN)

#include "rtdevice.h"

#define DBG_TAG  "canutil"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <stdlib.h>
#include <string.h>

static void canutils_receiver(void *parameter)
{
    int i;
    rt_err_t res;
    rt_device_t dev = (rt_device_t)parameter;

    res = rt_device_control(dev, RT_CAN_CMD_SET_BAUD, (void *)CAN500kBaud);
    RT_ASSERT(res == RT_EOK);

    res = rt_device_control(dev, RT_CAN_CMD_SET_MODE, (void *)RT_CAN_MODE_NORMAL);
    RT_ASSERT(res == RT_EOK);

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[] =
    {
        /* {(id),       (ide),  (rtr), (mode),  (mask),         (hdr_bank),     (rxfifo),      (ind),    (args)} */

        /* Must fully match 0x678 ID and STD-ID. */
        {0x678,         0,     0,       1,      0xFFFFFFFF,          0,              RT_NULL,    RT_NULL,   RT_NULL},

        /* Must fully match 0x678 ID and EXT-ID. */
        {0x678,         1,     0,       1,      0xFFFFFFFF,          1,              RT_NULL,    RT_NULL,   RT_NULL},

        /* Must fully match 0x123 ID and STD-ID. */
        {0x123,         0,     0,       1,      0xFFFFFFFF,          2,              RT_NULL,    RT_NULL,   RT_NULL},

        /* Must fully match 0x123 ID and EXT-ID. */
        {0x123,         1,     0,       1,      0xFFFFFFFF,          3,              RT_NULL,    RT_NULL,   RT_NULL},
    };

    struct rt_can_filter_config cfg = {sizeof(items) / sizeof(struct rt_can_filter_item), 1, items};

    res = rt_device_control(dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif
    static struct rt_can_msg rxmsg[32] = {0};

    while (1)
    {
        int size;
        int fid;

        for (fid = 0; fid < sizeof(rxmsg) / sizeof(struct rt_can_msg); fid++)
        {
            rxmsg[fid].hdr_index = -1;
        }

        if ((size = rt_device_read(dev, 0, &rxmsg, sizeof(rxmsg))) > 0)
        {
            for (fid = 0; fid < size / sizeof(struct rt_can_msg); fid++)
            {
                rt_kprintf("[%s: MsgRXBoxID:%d] ID: 0x%08X, %s, %s, length: %d",
                           dev->parent.name,
                           rxmsg[fid].hdr_index,
                           rxmsg[fid].id,
                           (rxmsg[fid].ide  == RT_CAN_STDID) ? "Standard ID" : "Extended ID",
                           (rxmsg[fid].rtr == RT_CAN_RTR) ? "RTR frame" : "DATA frame",
                           rxmsg[fid].len);

                if (rxmsg[fid].rtr == RT_CAN_DTR)
                {
                    rt_kprintf(", Data: ");
                    for (i = 0; i < rxmsg[fid].len; i++)
                    {
                        rt_kprintf("%02x ", rxmsg[fid].data[i]);
                    }
                }

                rt_kprintf("\n");

            } // for

        } // if
    }
}

static rt_thread_t canutils_receiver_new(rt_device_t dev)
{
    rt_thread_t thread = rt_thread_create("can_rx", canutils_receiver, (void *)dev, 4096, 25, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("create can_rx thread ok!\n");
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }

    return thread;
}


static int canutils_sendmsg(rt_device_t dev, int msg_size)
{
    int i;
    int ret;
    struct rt_can_msg msg = {0};

    msg.data[0] = 0x00;
    msg.data[1] = 0x11;
    msg.data[2] = 0x22;
    msg.data[3] = 0x33;
    msg.data[4] = 0x44;
    msg.data[5] = 0x55;
    msg.data[6] = 0x66;
    msg.data[7] = 0x77;
    msg.ide = RT_CAN_STDID;
    msg.rtr = RT_CAN_DTR;

    for (i = 0; i < msg_size; i++)
    {
        msg.id = (0x78 + i) % 0x800;
        msg.len = 2;

#if 1
        if ((ret = rt_device_write(dev, 0, &msg, sizeof(struct rt_can_msg))) != sizeof(struct rt_can_msg))
        {
            rt_kprintf("[%s][%d] send failure! %d\n", dev->parent.name, i, ret);
        }
        else
        {
            rt_kprintf("[%s][%d] send success!\n", dev->parent.name, i);
        }
#else
        ret = rt_device_write(dev, 0, &msg, sizeof(msg));
#endif
    }

    return i;
}

static void canutils_sender(void *parameter)
{
    int i;
    rt_err_t res;
    rt_device_t dev = (rt_device_t)parameter;
    while (1)
    {
        canutils_sendmsg(dev, 10000);
    }

}

static rt_thread_t canutils_sender_new(rt_device_t dev)
{
    rt_thread_t thread = rt_thread_create("can_tx", canutils_sender, (void *)dev, 4096, 25, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
        rt_kprintf("create can_tx thread ok!\n");
    }
    else
    {
        rt_kprintf("create can_tx thread failed!\n");
    }

    return thread;
}



static void canutils(int argc, char **argv)
{
    static rt_device_t dev = RT_NULL;
    static rt_thread_t thread_receiver = RT_NULL;
    static rt_thread_t thread_sender = RT_NULL;

    /* If the number of arguments less than 2 */
    if (argc < 2)
    {
        rt_kprintf("\n");
        rt_kprintf("can     [OPTION] [PARAM]\n");
        rt_kprintf("         probe <dev_name>      Probe sensor by given name\n");
        rt_kprintf("         send                  Read [num] times sensor (default 5)\n");
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

        if (thread_sender == RT_NULL)
            canutils_sendmsg(dev, num);
    }
    else if (!strcmp(argv[1], "sender"))
    {
        rt_uint32_t num = 1;

        if (dev == RT_NULL)
        {
            LOG_W("Please probe sensor device first!");
            return ;
        }

        if (thread_sender == RT_NULL)
            thread_sender = canutils_sender_new(dev);
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "probe"))
        {
            rt_device_t new_dev;
            rt_thread_t new_thread;

            new_dev = rt_device_find(argv[2]);
            if (new_dev == RT_NULL)
            {
                LOG_E("Can't find device:%s", argv[2]);
                return;
            }

            if (rt_device_open(new_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
            {
                LOG_E("open device failed!");
                return;
            }

            new_thread = canutils_receiver_new(new_dev);
            if (thread_receiver)
            {
                // Suspend thread;
            }
            thread_receiver = new_thread;

            if (dev)
            {
                rt_device_close(dev);
            }
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
        LOG_W("Unknown command, please enter 'canutils' get help information!");
    }

}
MSH_CMD_EXPORT(canutils, can test function);

#endif
