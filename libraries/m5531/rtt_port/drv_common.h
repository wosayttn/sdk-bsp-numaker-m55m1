/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2020-12-12      Wayne        First version
*
******************************************************************************/

#ifndef __DRV_COMMON_H__
#define __DRV_COMMON_H__

#include "NuMicro.h"
#include "drv_gpio.h"

#define NU_MFP_POS(PIN)   ((PIN % 4) * 8)
#define NU_MFP_MSK(PIN)   (0x1ful << NU_MFP_POS(PIN))

void nu_pin_func(rt_base_t pin, int data);

#endif /* __DRV_COMMON_H__ */
