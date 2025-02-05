/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-2-23       Wayne        First version
*
******************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#include "rtconfig.h"
#include "drv_gpio.h"
#include "NuMicro.h"

// <o> Internal SRAM memory size[Kbytes]
#define SRAM_SIZE         (0x150000)
#define SRAM_END          (0x20100000 + SRAM_SIZE)

#if defined(__ARMCC_VERSION)
    extern int Image$$RW_RAM$$ZI$$Limit;
    #define HEAP_BEGIN      ((void *)&Image$$RW_RAM$$ZI$$Limit)
#else
    extern int __bss_end__;
    #define HEAP_BEGIN      ((void *)&__bss_end__)
#endif

#define HEAP_END        (void *)SRAM_END

#if defined(PKG_USING_RAMDISK)
    #define RAMDISK_NAME         "rd0"
    #define MOUNT_POINT_RAMDISK0 "/"
#endif

void rt_hw_board_init(void);
void rt_hw_cpu_reset(void);

#endif /* BOARD_H_ */
