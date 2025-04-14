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
#ifndef HAL_CAMERA_H
#define HAL_CAMERA_H
/**
 * This file is the top level abstraction for the CAMERA related functions
 **/

#include "camera_img.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define hal_camera_init(x, y)                   camera_init(x, y)
#define hal_camera_fini()                       camera_fini()
#define hal_camera_get_frame(x)                 camera_get_frame(x)
#define hal_camera_sync()                       camera_sync()
#define hal_camera_oneshot()                    camera_oneshot()

#endif /* HAL_CAMERA_H */
