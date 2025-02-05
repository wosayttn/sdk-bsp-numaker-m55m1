/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2020-1-16       Wayne        First version
*
******************************************************************************/

#include <rtconfig.h>

#if defined(PKG_USING_WAVPLAYER)

#include "wavrecorder.h"
#include "wavplayer.h"
#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#define DEF_MAX_TEST_SECOND  5

/*
  The routine just for test automatically.
  - For record function: Run it w/o parameter.
  - For replay function: Run it with parameter.
*/
static int audio_test(int argc, char **argv)
{

    int smplrate[] = {8000, 16000};
    int smplbit[] = {16};
    //int chnum[] = {1, 2};
    int chnum[] = {1};
    struct wavrecord_info info;
    char strbuf[128];
    int i, j, k;
    int bDoRecording = 1;
    struct stat stat_buf;

    if (argc > 1)
        bDoRecording = 0;

    for (i = 0; i < sizeof(smplrate) / sizeof(int); i++)
    {
        for (j = 0; j < sizeof(smplbit) / sizeof(int); j++)
        {
            for (k = 0; k < sizeof(chnum) / sizeof(int); k++)
            {
                snprintf(strbuf, sizeof(strbuf), "/mnt/sd0/%d_%d_%d.wav", smplrate[i], smplbit[j], chnum[k]);

                rt_kprintf("==========================================================\n");

                if (bDoRecording)
                {
#if defined(PKG_WP_USING_RECORD)
                    rt_kprintf("Recording file at %s\n", strbuf);
                    info.uri = strbuf;
                    info.samplerate = smplrate[i];
                    info.samplebits = smplbit[j];
                    info.channels = chnum[k];
                    rt_kprintf("+++++++++++++++++++++++++++++++++++++++++++\n");
                    wavrecorder_start(&info);
                    rt_thread_mdelay(DEF_MAX_TEST_SECOND * 1000);
                    wavrecorder_stop();
                    rt_kprintf("-------------------------------------------\n");
                    rt_thread_mdelay(DEF_MAX_TEST_SECOND * 1000);
#endif
                }
                else
                {
#if defined(PKG_WP_USING_PLAY)
                    if (stat((const char *)strbuf, &stat_buf) < 0)
                    {
                        rt_kprintf("%s non-exist.\n", strbuf);
                        continue;
                    }

                    rt_kprintf("Replay file at %s\n", strbuf);
                    rt_kprintf("+++++++++++++++++++++++++++++++++++++++++++\n");
                    wavplayer_play(strbuf);
                    rt_thread_mdelay(DEF_MAX_TEST_SECOND * 1000);
                    wavplayer_stop();
                    rt_kprintf("-------------------------------------------\n");
#endif
                }

                rt_kprintf("==========================================================\n");
            } // k
        } // j
    } // i

    return 0;
}

#ifdef FINSH_USING_MSH
    MSH_CMD_EXPORT(audio_test, Audio record / replay);
#endif


static int audio_overnight(int argc, char **argv)
{
    struct wavrecord_info info;
    char strbuf[128];
    struct stat stat_buf;
    int bDoForever = 1;

    if (argc > 1)
        bDoForever = 0;

    do
    {
        snprintf(strbuf, sizeof(strbuf), "/test.wav");

#if defined(PKG_WP_USING_RECORD)
        rt_kprintf("Recording file at %s\n", strbuf);
        info.uri = strbuf;
        info.samplerate = 16000;
        info.samplebits = 16;
        info.channels = 1;
        wavrecorder_start(&info);
        rt_thread_mdelay(DEF_MAX_TEST_SECOND * 1000);
        wavrecorder_stop();
        rt_thread_mdelay(1000);
#endif

#if defined(PKG_WP_USING_PLAY)
        if (stat((const char *)strbuf, &stat_buf) < 0)
        {
            rt_kprintf("%s non-exist.\n", strbuf);
            break;
        }

        rt_kprintf("Replay file at %s\n", strbuf);
        wavplayer_play(strbuf);
        rt_thread_mdelay(DEF_MAX_TEST_SECOND * 1000);
        wavplayer_stop();
        rt_thread_mdelay(1000);
#endif
    }
    while (bDoForever);

    return 0;
}

#ifdef FINSH_USING_MSH
    MSH_CMD_EXPORT(audio_overnight, auto test record / replay);
#endif

#endif /* PKG_USING_WAVPLAYER */
