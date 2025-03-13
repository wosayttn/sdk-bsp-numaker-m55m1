/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author           Notes
* 2023-8-8        Wayne            First version
*
******************************************************************************/
#include "rtthread.h"

#if defined(PKG_CHERRYUSB_HOST) && defined(PKG_CHERRYUSB_HOST_EHCI_CUSTOM)

#include "NuMicro.h"
#include "usb_config.h"
#include "usbh_core.h"

void HSUSBH_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    USBH_IRQHandler(0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void USBH1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    void OHCI_IRQHandler(uint8_t busid);
    OHCI_IRQHandler(0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void usb_hc_low_level_init(struct usbh_bus *bus)
{
    if (bus->hcd.reg_base == HSUSBH_BASE)
    {
        uint32_t u32RegLockBackup = SYS_IsRegLocked();

        /* Initialize USB HOST */
        SYS_UnlockReg();

#if !defined(BSP_USING_HSOTG)
        /* Enable USBH clock */
        void nutool_modclkcfg_init_usbh(void);
        nutool_modclkcfg_init_usbh();

        {
            /* Set PHY*/
            SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk)) | (0x1ul << (SYS_USBPHY_HSUSBROLE_Pos));  // Select HSUSBH
            SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_USBROLE_Msk)) | (0x1ul << (SYS_USBPHY_USBROLE_Pos));      // Select USBH
            SYS->USBPHY |= (SYS_USBPHY_HSOTGPHYEN_Msk | SYS_USBPHY_OTGPHYEN_Msk);
            rt_thread_mdelay(1);
            SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
        }
#endif

        /* Enable interrupt */
        NVIC_EnableIRQ(HSUSBH_IRQn);
        NVIC_EnableIRQ(USBH1_IRQn);

        if (u32RegLockBackup)
            SYS_LockReg();
    }
}

void usb_hc_low_level2_init(struct usbh_bus *bus)
{
}

uint8_t usbh_get_port_speed(struct usbh_bus *bus, const uint8_t port)
{
    return USB_SPEED_HIGH;
}

int rt_usbh_initialize(void)
{
    int bus_id = 0;

    usbh_initialize(bus_id, HSUSBH_BASE);

    return 0;
}
INIT_ENV_EXPORT(rt_usbh_initialize);
#endif
