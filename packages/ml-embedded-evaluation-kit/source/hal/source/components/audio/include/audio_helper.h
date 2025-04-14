/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef AUDIO_HELPER_H
#define AUDIO_HELPER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief   Initialise the audio
 * @return  0 if successful, error code otherwise.
 **/
int audio_capture_init(uint32_t u32SmplRate, uint32_t u32SmplBit, uint32_t u32ChnNum);
int audio_playback_init(uint32_t u32SmplRate, uint32_t u32SmplBit, uint32_t u32ChnNum);

/**
 * @brief   Finalize the audio
 * @return  0 if successful, error code otherwise.
 **/
void audio_capture_fini(void);
void audio_playback_fini(void);

/**
 * @brief       get the audio frame buffer from audio device.
 * @return      Available bytes.
 **/
uint32_t audio_capture_get_frame(uint8_t *pu8BufAddr, uint32_t u32BufLen);
uint32_t audio_playback_put_frame(uint8_t *pu8BufAddr, uint32_t u32BufLen);

uint32_t audio_transcode_pcm16to8(uint8_t *pu8BufAddr, uint32_t u32BufLen);

#endif /* AUDIO_HELPER_H */
