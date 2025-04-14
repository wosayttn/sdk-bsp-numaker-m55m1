/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-3-15       Wayne        First version
*
******************************************************************************/

#include <rtthread.h>
#include "rthw.h"

#include "drv_uart.h"
#include "drv_gpio.h"
#include "board.h"

#define LOG_TAG    "drv.common"
#undef  DBG_ENABLE
#define DBG_SECTION_NAME   LOG_TAG
#define DBG_LEVEL      LOG_LVL_DBG
#define DBG_COLOR
#include <rtdbg.h>

/**
 * This function will initial.
 */
RT_WEAK void rt_hw_board_init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Configure MPU memory region defined in mpu_config_M5531.h */
    InitPreDefMPURegion(NULL, 0);

    /* Init System/modules clock */
    void nutool_modclkcfg_init();
    nutool_modclkcfg_init();

    /* Init all pin function setting */
    void nutool_pincfg_init(void);
    nutool_pincfg_init();

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

#if defined(RT_USING_HEAP)
    rt_system_heap_init(HEAP_BEGIN, HEAP_END);
#endif /* RT_USING_HEAP */

#if defined(BSP_USING_UART)
    rt_hw_uart_init();
#endif

#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#if defined(RT_USING_COMPONENTS_INIT)
    rt_components_board_init();
#endif
}

/**
 * The time delay function.
 *
 * @param microseconds.
 */
void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

#define NU_MFP_POS(PIN)   ((PIN % 4) * 8)
#define NU_MFP_MSK(PIN)   (0x1ful << NU_MFP_POS(PIN))
void nu_pin_set_function(rt_base_t pin, int data)
{
    uint32_t GPx_MFPx_org;
    uint32_t pin_index      = NU_GET_PINS(pin);
    uint32_t port_index     = NU_GET_PORT(pin);
    __IO uint32_t *GPx_MFPx = ((__IO uint32_t *) &SYS->GPA_MFP0) + port_index * 4 + (pin_index / 4);
    uint32_t MFP_Msk        = NU_MFP_MSK(pin_index);

    GPx_MFPx_org = *GPx_MFPx;
    *GPx_MFPx    = (GPx_MFPx_org & (~MFP_Msk)) | data;

    //rt_kprintf("Port[%d]-Pin[%d] Addr[%08x] Data[%08x] %08x -> %08x\n", port_index, pin_index, GPx_MFPx, data, GPx_MFPx_org, *GPx_MFPx);
}

void nu_read_uid(uint32_t *id)
{
    uint32_t u32RegLockBackup = SYS_IsRegLocked();

    SYS_UnlockReg();

    /* Enable FMC ISP function */
    FMC_Open();

    /* Read Unique ID */
    id[0] = FMC_ReadUID(0);
    id[1] = FMC_ReadUID(1);
    id[2] = FMC_ReadUID(2);
    id[3] = 0;

    /* Disable FMC ISP function */
    FMC_Close();

    if (u32RegLockBackup)
        SYS_LockReg();
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

void rt_hw_cpu_reset(void)
{
    SYS_UnlockReg();

    SYS_ResetChip();
}

int reboot(int argc, char **argv)
{
    rt_hw_cpu_reset();
    return 0;
}
MSH_CMD_EXPORT(reboot, Reboot System);

#if defined(RT_USING_SPI)
/**
  * Attach the spi device to SPI bus, this function must be used after initialization.
  */
rt_err_t rt_hw_spi_device_attach(const char *bus_name, const char *device_name, rt_base_t pin)
{
    RT_ASSERT(bus_name != RT_NULL);
    RT_ASSERT(device_name != RT_NULL);

    rt_err_t ret = RT_EOK;
    struct rt_spi_device *spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    RT_ASSERT(spi_device != RT_NULL);

    rt_uint32_t *cs_pin = (rt_uint32_t *)rt_malloc(sizeof(rt_uint32_t));
    RT_ASSERT(cs_pin != RT_NULL);

    *cs_pin = pin;
    rt_pin_mode(pin, PIN_MODE_OUTPUT);
    rt_pin_write(pin, PIN_HIGH);

    ret = rt_spi_bus_attach_device(spi_device, device_name, bus_name, (void *)cs_pin);
    RT_ASSERT(ret == RT_EOK);

    return ret;
}
#endif

void devmem(int argc, char *argv[])
{
    /**
     * @brief  This function may interact with critical system registers. Use with caution.
     *
     * @details
     * This function has the potential to access and modify important system registers
     * as part of its operation. Ensure proper validation and system state checks
     * before calling this function. Improper or careless usage may lead to system
     * instability or unintended behavior.
    */
    volatile unsigned int u32Addr;
    unsigned int value = 0, mode = 0;

    if (argc < 2 || argc > 3)
    {
        goto exit_devmem;
    }

    if (argc == 3)
    {
        if (sscanf(argv[2], "0x%x", &value) != 1)
            goto exit_devmem;
        mode = 1; //Write
    }

    if (sscanf(argv[1], "0x%x", &u32Addr) != 1)
        goto exit_devmem;
    else if (!u32Addr || u32Addr & (4 - 1))
        goto exit_devmem;

    if (mode)
    {
        *((volatile uint32_t *)u32Addr) = value;
    }
    rt_kprintf("0x%08x\n", *((volatile uint32_t *)u32Addr));

    return;
exit_devmem:
    rt_kprintf("Read: devmem <physical address in hex>\n");
    rt_kprintf("Write: devmem <physical address in hex> <value in hex format>\n");
    return;
}
MSH_CMD_EXPORT(devmem, dump device registers);

#if defined(RT_USING_ULOG)
void devmem2(int argc, char *argv[])
{
    /**
     * @brief  This function may interact with critical system registers. Use with caution.
     *
     * @details
     * This function has the potential to access and modify important system registers
     * as part of its operation. Ensure proper validation and system state checks
     * before calling this function. Improper or careless usage may lead to system
     * instability or unintended behavior.
    */

    volatile unsigned int u32Addr;
    unsigned int value = 0, word_count = 1;

    if (argc < 2 || argc > 3)
    {
        goto exit_devmem;
    }

    if (argc == 3)
    {
        if (sscanf(argv[2], "%d", &value) != 1)
            goto exit_devmem;
        word_count = value;
    }

    if (sscanf(argv[1], "0x%x", &u32Addr) != 1)
        goto exit_devmem;
    else if (!u32Addr || u32Addr & (4 - 1))
        goto exit_devmem;

    if (word_count > 0)
    {
        LOG_HEX("devmem", 16, (void *)u32Addr, word_count * sizeof(rt_base_t));
    }

    return;
exit_devmem:
    rt_kprintf("devmem2: <physical address in hex> <count in dec>\n");
    return;
}
MSH_CMD_EXPORT(devmem2, dump device registers);
#endif

#if defined(RT_USING_CPU_FFS)
int __rt_ffs(int value)
{
    if (!value) return 0;
    return __CLZ(__RBIT(value)) + 1;
}
#endif

