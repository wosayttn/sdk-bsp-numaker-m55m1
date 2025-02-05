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
#ifndef HAL_AUDIO_H
#define HAL_AUDIO_H
/**
 * This file is the top level abstraction for the CAMERA related functions
 **/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "audio_helper.h"

#define hal_audio_capture_init(x, y, z)         audio_capture_init(x, y, z)
#define hal_audio_capture_fini()                audio_capture_fini()
#define hal_audio_capture_get_frame(x, y)       audio_capture_get_frame(x, y)
#define hal_audio_transcode_pcm16to8(x, y)      audio_transcode_pcm16to8(x, y)

#define hal_audio_playback_init(x, y, z)        audio_playback_init(x, y, z)
#define hal_audio_playback_fini()               audio_playback_fini()
#define hal_audio_playback_put_frame(x, y)      audio_playback_put_frame(x, y)

#endif /* HAL_AUDIO_H */
