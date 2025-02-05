/*
 * SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"

#include "poster_helper.h"
#include "memheap_helper.h"

#include "log_macros.h"
#include "rtthread.h"
#include "rtdevice.h"

//#undef DBG_ENABLE
#define DBG_LEVEL LOG_LVL_INFO
#define DBG_SECTION_NAME  "ml.poster"
#define DBG_COLOR
#include <rtdbg.h>

#define DEF_TMP_ENCODING_SIZE        (128*1024)
#define DEF_JPEG_BITSTREAM_SIZE      (DEF_TMP_ENCODING_SIZE/2)
#define DEF_JPEG_BITSTREAM_B64_SIZE  (DEF_JPEG_BITSTREAM_SIZE)

static const char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static struct rt_work   s_workPubMsg = {0};
static uint8_t *s_pu8JpegBitStreamBuf = RT_NULL;
static uint8_t *s_pu8JpegBitStreamB64Buf = RT_NULL;

static uint32_t s_u32JpegWroteSize = 0;

static uint8_t *b64_encode(uint8_t *pu8DstBuf, const uint8_t *pu8SrcBuf, uint32_t u32InLen)
{
    uint32_t u32OutLen = 4 * ((u32InLen + 2) / 3);

    if (!pu8DstBuf || !pu8SrcBuf || !u32InLen)
        return NULL;

    // Loop through the input data and encode it to Base64
    for (size_t i = 0, j = 0; i < u32InLen;)
    {
        // Extract three octets from input data (or use zero padding)
        uint32_t octet_a = (i < u32InLen) ? pu8SrcBuf[i++] : 0;
        uint32_t octet_b = (i < u32InLen) ? pu8SrcBuf[i++] : 0;
        uint32_t octet_c = (i < u32InLen) ? pu8SrcBuf[i++] : 0;

        // Combine three octets into a 24-bit triple
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        // Encode triple as Base64 characters
        pu8DstBuf[j++] = b64[(triple >> 18) & 0x3F];
        pu8DstBuf[j++] = b64[(triple >> 12) & 0x3F];
        pu8DstBuf[j++] = b64[(triple >> 6) & 0x3F];
        pu8DstBuf[j++] = b64[triple & 0x3F];
    }

    // Handle padding and null-terminate the encoded string
    while (u32OutLen > 0 && pu8DstBuf[u32OutLen - 1] == '=')
    {
        u32OutLen--;
    }
    pu8DstBuf[u32OutLen] = '\0';

    return pu8DstBuf;
}

static void jpeg_write_func(void *context, void *data, int size)
{
    if (context && data && size)
    {
        uint32_t offset = *((uint32_t *)context);
        memcpy(&s_pu8JpegBitStreamBuf[offset], data, size);
        *((uint32_t *)context) = offset + size;
    }
}

static void poster_mqtt_func(struct rt_work *work, void *work_data)
{
    int mqtt_pub_image(const uint8_t *buf, uint32_t len);

#define CONFIG_TRANSCODE_TO_B64
#if defined(CONFIG_TRANSCODE_TO_B64)
    /* Transcode to base64 encoding for MQTT image. */
    b64_encode(s_pu8JpegBitStreamB64Buf, (const uint8_t *)s_pu8JpegBitStreamBuf, s_u32JpegWroteSize);

    /* Publish the image message. */
    mqtt_pub_image(s_pu8JpegBitStreamB64Buf, strlen(s_pu8JpegBitStreamB64Buf));
#else

    /* Publish the image message. */
    mqtt_pub_image(s_pu8JpegBitStreamBuf, s_u32JpegWroteSize);
#endif
}

static uint32_t mqttNotifyInterval = 1000;
int poster_mqtt(S_JPEG_CONTEXT *psJpegECtx)
{
    static uint32_t u32LastPoster = 0;
    if ((mqttNotifyInterval + u32LastPoster) < rt_tick_get())
    {
        if (psJpegECtx && psJpegECtx->pu8SrcImgBuf)
        {
            /* Do JPEG encoding. */
            s_u32JpegWroteSize = 0;
            tje_encode_with_func(jpeg_write_func,
                                 (void *)&s_u32JpegWroteSize,
                                 psJpegECtx->u32Quality,
                                 psJpegECtx->u32ImgWidth,
                                 psJpegECtx->u32ImgHeight,
                                 psJpegECtx->u32NumComponents,
                                 (const unsigned char *)psJpegECtx->pu8SrcImgBuf);

            /* Update last timestamp. */
            u32LastPoster = rt_tick_get();

            return rt_work_submit(&s_workPubMsg, 0);
        }
    }

    return -1;
}

void poster_SetNotifyInterval(uint32_t t)
{
    mqttNotifyInterval = t;
}

static int poster_mqtt_init(void)
{
    s_pu8JpegBitStreamBuf = (uint8_t *) memheap_helper_allocate(evAREANA_AT_HYPERRAM, DEF_JPEG_BITSTREAM_SIZE);
    if (!s_pu8JpegBitStreamBuf)
    {
        LOG_E("failed to allocate JPEG bitstream buffer.");
        return -1;
    }

    s_pu8JpegBitStreamB64Buf = (uint8_t *) memheap_helper_allocate(evAREANA_AT_HYPERRAM, DEF_JPEG_BITSTREAM_B64_SIZE);
    if (!s_pu8JpegBitStreamB64Buf)
    {
        LOG_E("failed to allocate JPEG bitstream B64 buffer.");
        return -1;
    }

    rt_work_init(&s_workPubMsg, poster_mqtt_func, NULL);

    int MqttConnect(int argc, char *argv[]);
    return MqttConnect(1, NULL);
}
INIT_APP_EXPORT(poster_mqtt_init);
