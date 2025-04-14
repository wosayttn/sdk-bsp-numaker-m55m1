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
#include "NuMicro.h"

#if defined(PKG_CHERRYUSB_DEVICE_HS)

void HSUSBD_IRQHandler(void)
{
    rt_interrupt_enter();

    void CherryUSB_USBD_IRQHandler(uint8_t busid);
    CherryUSB_USBD_IRQHandler(0);

    rt_interrupt_leave();
}

void usb_dc_low_level_init(void)
{
    uint32_t u32RegLockBackup = SYS_IsRegLocked();

    /* Initialize USB PHY */
    SYS_UnlockReg();

#if !defined(BSP_USING_HSOTG)
    void nutool_modclkcfg_init_hsusbd(void);
    nutool_modclkcfg_init_hsusbd();

    {
        /* Set PHY*/
        SYS->USBPHY  = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk));  // Select HSUSBD
        SYS->USBPHY &= ~SYS_USBPHY_HSUSBACT_Msk;
        SYS->USBPHY |= (SYS_USBPHY_HSOTGPHYEN_Msk);
        rt_thread_delay(20);
        SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
    }
#endif

    SYS_ResetModule(SYS_HSUSBD0RST);

    if (u32RegLockBackup)
        SYS_LockReg();
}

void usb_dc_low_level_deinit(void)
{
    uint32_t u32RegLockBackup = SYS_IsRegLocked();

    SYS_UnlockReg();

#if !defined(BSP_USING_HSOTG)

    SYS->USBPHY &= ~(SYS_USBPHY_HSUSBACT_Msk | SYS_USBPHY_HSOTGPHYEN_Msk);

    void nutool_modclkcfg_deinit_hsusbd(void);
    nutool_modclkcfg_deinit_hsusbd();

#endif

    if (u32RegLockBackup)
        SYS_LockReg();
}
#endif
