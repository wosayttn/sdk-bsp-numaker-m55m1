// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: SquareLine_Project

#include "ui.h"
#include "rtthread.h"
#include "netdev_ipaddr.h"
#include "netdev.h"

void SetNoticeInterval(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);

    void poster_SetNotifyInterval(uint32_t t);
    poster_SetNotifyInterval((uint32_t)lv_slider_get_value(slider));
}

#define NETDEV_IFCONFIG_MAC_MAX_LEN    6
#define NETDEV_IFCONFIG_DEFAULT_INTERFACE  "e0"
void sysinfo_eventcb(lv_timer_t *timer)
{
    // ShowSystemInfo
    // Date & time
    {
        static char szTZ[16];
        static char szDataTime[32];
        time_t now = (time_t)0;
        int tmp;

        struct timeval tv = { 0 };
        struct timezone tz = { 0 };

        gettimeofday(&tv, &tz);
        now = tv.tv_sec;

        rt_snprintf(szDataTime, sizeof(szDataTime), "%s", ctime(&now));
        if ((tmp = strlen(szDataTime)) > 0)
        {
            szDataTime[tmp - 1] = '\0'; //Remove newline symbol.
        }
        lv_label_set_text(ui_DATETIME, szDataTime);

        rt_snprintf(szTZ, sizeof(szTZ), "UTC%c%d", -tz.tz_minuteswest > 0 ? '+' : '-', -tz.tz_minuteswest / 60);
        lv_label_set_text(ui_TIMEZONE, szTZ);
    }

// Networking
    {
        rt_ubase_t index;
        rt_slist_t *node  = RT_NULL;
        struct netdev *netdev = RT_NULL;
        struct netdev *cur_netdev_list = netdev_list;
        static char szIPAddr[16];
        static char szNetMask[16];
        static char szDNSAddr[16];
        static char szGWAddr[16];
        static char szMACAddr[32];

        if (cur_netdev_list != RT_NULL)
        {
            for (node = &(cur_netdev_list->list); node; node = rt_slist_next(node))
            {
                netdev = rt_list_entry(node, struct netdev, list);

                if (strcmp(netdev->name, NETDEV_IFCONFIG_DEFAULT_INTERFACE))
                    continue;

                if (netdev->hwaddr_len == NETDEV_IFCONFIG_MAC_MAX_LEN)
                {
                    rt_snprintf(szMACAddr, sizeof(szMACAddr), "%02X:%02X:%02X:%02X:%02X:%02X",
                                netdev->hwaddr[0],
                                netdev->hwaddr[1],
                                netdev->hwaddr[2],
                                netdev->hwaddr[3],
                                netdev->hwaddr[4],
                                netdev->hwaddr[5]);
                }

                if (netdev->flags & NETDEV_FLAG_LINK_UP)
                {
                    rt_strncpy(szIPAddr, inet_ntoa(netdev->ip_addr), sizeof(szIPAddr));
                    rt_strncpy(szNetMask, inet_ntoa(netdev->netmask), sizeof(szNetMask));
                    rt_strncpy(szGWAddr, inet_ntoa(netdev->gw), sizeof(szGWAddr));
                    rt_strncpy(szDNSAddr, inet_ntoa(netdev->dns_servers[0]), sizeof(szDNSAddr));
                }
                else
                {
                    rt_strncpy(szIPAddr, "0.0.0.0", sizeof(szIPAddr));
                    rt_strncpy(szNetMask, "0.0.0.0", sizeof(szNetMask));
                    rt_strncpy(szGWAddr, "0.0.0.0", sizeof(szGWAddr));
                    rt_strncpy(szDNSAddr, "0.0.0.0", sizeof(szDNSAddr));
                }

                lv_label_set_text(ui_INTERFACE, NETDEV_IFCONFIG_DEFAULT_INTERFACE);
                lv_label_set_text(ui_MACCADDR, szMACAddr);
                lv_label_set_text(ui_IPADDR, szIPAddr);
                lv_label_set_text(ui_NETMSK, szNetMask);
                lv_label_set_text(ui_GWADDR, szGWAddr);
                lv_label_set_text(ui_DNS, szDNSAddr);

                break;
            } // for
        } // if
    }

    // MQTT
    {
        char *Mqtt_GetHost(void);
        char *Mqtt_GetImagePubTopic(void);
        char *Mqtt_GetLastStatus(void);
        char *Mqtt_GetClientID(void);

        lv_label_set_text(ui_MQTTBROKER1, Mqtt_GetHost());
        lv_label_set_text(ui_MQTTCLIENTID1, Mqtt_GetClientID());
        lv_label_set_text(ui_MQTTCONNECT1, Mqtt_GetLastStatus());
        lv_label_set_text(ui_MQTTTOPIC1, Mqtt_GetImagePubTopic());
    }

}

lv_timer_t *sysinfo_timer;
void ScreenLoadedCB(lv_event_t *e)
{
    sysinfo_timer = lv_timer_create(sysinfo_eventcb, 1000,  NULL);
}

