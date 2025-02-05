/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2022-3-12       Wayne        First version
*
******************************************************************************/

#include <rtthread.h>
#include "board.h"

#define LOG_TAG         "mnt"
#define DBG_ENABLE
#define DBG_SECTION_NAME "mnt"
#define DBG_LEVEL DBG_ERROR
#define DBG_COLOR
#include <rtdbg.h>

#include <dfs_fs.h>
#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#if defined(RT_USING_FAL)
    #include <fal.h>
#endif

#ifdef RT_USING_DFS_MNTTABLE
const struct dfs_mount_tbl mount_table[] =
{
    { "sd0", "/mnt/sd0", "elm", 0, RT_NULL },
    { "sd0p0", "/mnt/sd0p0", "elm", 0, RT_NULL },
    { "sd0p1", "/mnt/sd0p1", "elm", 0, RT_NULL },
};
#endif

#if defined(PKG_USING_RAMDISK) && defined(BOARD_USING_EXTERNAL_HYPERRAM) && defined(BSP_USING_SPIM0)
#include "drv_memheap.h"

extern rt_err_t ramdisk_init(const char *dev_name, rt_uint8_t *disk_addr, rt_size_t block_size, rt_size_t num_block);
int ramdisk_device_init(void)
{
    rt_err_t result = RT_EOK;

#define DEF_RAMDISK_SIZE    (1 * 1024 * 1024)

    void *pvRAMDiskStart = rt_memheap_alloc(nu_memheap_get(NU_MEMHEAP_SPIM0), DEF_RAMDISK_SIZE);
    RT_ASSERT(pvRAMDiskStart != RT_NULL);

    /* Create a 4MB RAMDISK */
    result = ramdisk_init(RAMDISK_NAME, (rt_uint8_t *)pvRAMDiskStart, 512, DEF_RAMDISK_SIZE / 512);
    RT_ASSERT(result == RT_EOK);

    return 0;
}
INIT_DEVICE_EXPORT(ramdisk_device_init);

/* Recursive mkdir */
static int mkdir_p(const char *dir, const mode_t mode)
{
    int ret = -1;
    char *tmp = NULL;
    char *p = NULL;
    struct stat sb;
    rt_size_t len;

    if (!dir)
        goto exit_mkdir_p;

    /* Copy path */
    /* Get the string length */
    len = strlen(dir);
    tmp = rt_strdup(dir);

    /* Remove trailing slash */
    if (tmp[len - 1] == '/')
    {
        tmp[len - 1] = '\0';
        len--;
    }

    /* check if path exists and is a directory */
    if (stat(tmp, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
        {
            ret = 0;
            goto exit_mkdir_p;
        }
    }

    /* Recursive mkdir */
    for (p = tmp + 1; p - tmp <= len; p++)
    {
        if ((*p == '/') || (p - tmp == len))
        {
            *p = 0;

            /* Test path */
            if (stat(tmp, &sb) != 0)
            {
                /* Path does not exist - create directory */
                if (mkdir(tmp, mode) < 0)
                {
                    goto exit_mkdir_p;
                }
            }
            else if (!S_ISDIR(sb.st_mode))
            {
                /* Not a directory */
                goto exit_mkdir_p;
            }
            if (p - tmp != len)
                *p = '/';
        }
    }

    ret = 0;

exit_mkdir_p:

    if (tmp)
        rt_free(tmp);

    return ret;
}

/* Initialize the filesystem */
int filesystem_init(void)
{
    rt_err_t result = RT_EOK;

    // ramdisk as root
    if (!rt_device_find(RAMDISK_NAME))
    {
        LOG_E("cannot find %s device", RAMDISK_NAME);
    }
    else
    {
        /* Format these ramdisk */
        result = (rt_err_t)dfs_mkfs("elm", RAMDISK_NAME);
        RT_ASSERT(result == RT_EOK);

        /* mount ramdisk0 as root directory */
        if (dfs_mount(RAMDISK_NAME, "/", "elm", 0, RT_NULL) == 0)
        {
            LOG_I("ramdisk mounted on \"/\".");

            /* now you can create dir dynamically. */
            mkdir_p("/mnt", 0x777);
            mkdir_p("/cache", 0x777);
            mkdir_p("/download", 0x777);
            mkdir_p("/mnt/ram_usbd", 0x777);
            mkdir_p("/mnt/filesystem", 0x777);
            mkdir_p("/mnt/sd0", 0x777);
            mkdir_p("/mnt/sd0p0", 0x777);
            mkdir_p("/mnt/sd0p1", 0x777);
            mkdir_p("/mnt/udisk", 0x777);
        }
        else
        {
            LOG_E("Failed to mount %s", RAMDISK_NAME);
        }
    }

exit_filesystem_init:

    return -result;
}
INIT_ENV_EXPORT(filesystem_init);
#endif

