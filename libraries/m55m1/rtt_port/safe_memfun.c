/**************************************************************************//**
*
* @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2024-7-15       Wayne        First version
*
******************************************************************************/

#include "NuMicro.h"

#if 0

#define __CPY_WORD(dst, src) \
       *(uint32_t *)(dst) = *(uint32_t *)(src); \
       (dst) = ((uint32_t *)dst) + 1; \
       (src) = ((uint32_t *)src) + 1

#define __CPY_HWORD(dst, src) \
       *(uint16_t *)(dst) = *(uint16_t *)(src); \
       (dst) = ((uint16_t *)dst) + 1; \
       (src) = ((uint16_t *)src) + 1

#define __CPY_BYTE(dst, src) \
       *(uint8_t *)(dst) = *(uint8_t *)(src); \
       (dst) = ((uint8_t *)dst) + 1; \
       (src) = ((uint8_t *)src) + 1

NVT_ITCM
void *memcpy(void *restrict  dst, const void *restrict src, size_t n)
{
    void *ret = dst;
    uint32_t tmp;

    if (0 == n) return ret;

    while (((uintptr_t)src & 0x03UL) != 0UL)
    {
        __CPY_BYTE(dst, src);
        n--;

        if (0 == n) return ret;
    }

    if (((uintptr_t)dst & 0x03UL) == 0UL)
    {
        while (n >= 16UL)
        {
            __CPY_WORD(dst, src);
            __CPY_WORD(dst, src);
            __CPY_WORD(dst, src);
            __CPY_WORD(dst, src);
            n -= 16UL;
        }

        if ((n & 0x08UL) != 0UL)
        {
            __CPY_WORD(dst, src);
            __CPY_WORD(dst, src);
        }

        if ((n & 0x04UL) != 0UL)
        {
            __CPY_WORD(dst, src);
        }

        if ((n & 0x02UL) != 0UL)
        {
            __CPY_HWORD(dst, src);
        }

        if ((n & 0x01UL) != 0UL)
        {
            __CPY_BYTE(dst, src);
        }
    }
    else
    {
        if (((uintptr_t)dst & 1UL) == 0UL)
        {
            while (n >= 4)
            {
                tmp = *(uint32_t *)src;
                src = ((uint32_t *)src) + 1;

                *(volatile uint16_t *)dst = (uint16_t)tmp;
                dst = ((uint16_t *)dst) + 1;
                *(volatile uint16_t *)dst = (uint16_t)(tmp >> 16U);
                dst = ((uint16_t *)dst) + 1;

                n -= 4;
            }
        }
        else
        {
            while (n >= 4)
            {
                tmp = *(uint32_t *)src;
                src = ((uint32_t *)src) + 1;

                *(volatile uint8_t *)dst  = (uint8_t)tmp;
                dst = ((uint8_t *)dst) + 1;
                *(volatile uint16_t *)dst = (uint16_t)(tmp >> 8U);
                dst = ((uint16_t *)dst) + 1;
                *(volatile uint8_t *)dst = (uint8_t)(tmp >> 24U);
                dst = ((uint8_t *)dst) + 1;
                n -= 4;
            }
        }

        while (n > 0)
        {
            __CPY_BYTE(dst, src);
            n--;
        }
    }

    return ret;
}

#endif
