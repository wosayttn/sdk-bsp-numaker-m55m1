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

#if defined(PKG_CHERRYUSB_DEVICE_FS)

void USBD_IRQHandler(void)
{
    rt_interrupt_enter();

    void CherryUSB_USBD_IRQHandler(uint8_t busid);
    CherryUSB_USBD_IRQHandler(0);

    rt_interrupt_leave();
}

void usb_dc_low_level_init(void)
{
    void nutool_modclkcfg_init_usbd(void);
    nutool_modclkcfg_init_usbd();

    /* Select USBD */
    SYS->USBPHY = (SYS->USBPHY & ~SYS_USBPHY_USBROLE_Msk) | SYS_USBPHY_OTGPHYEN_Msk;

    SYS_ResetModule(SYS_USBD0RST);

    NVIC_EnableIRQ(USBD_IRQn);
}

void usb_dc_low_level_deinit(void)
{
    NVIC_DisableIRQ(USBD_IRQn);

    SYS->USBPHY &= ~(SYS_USBPHY_USBROLE_Msk | SYS_USBPHY_OTGPHYEN_Msk) ;

    void nutool_modclkcfg_deinit_usbd(void);
    nutool_modclkcfg_deinit_usbd();
}

#endif
