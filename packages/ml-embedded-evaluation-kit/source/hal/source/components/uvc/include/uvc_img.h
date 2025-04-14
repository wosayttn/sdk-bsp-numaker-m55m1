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
#ifndef UVC_IMG_H
#define UVC_IMG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief   Initialise the UVC device
 * @return  0 if successful, error code otherwise.
 **/
int uvc_init(void);

/**
 * @brief   Finalize the UVC device
 * @return  None
 **/
void uvc_fini(void);

/**
 * @brief   Send image
 * @return   0 if successful, error code otherwise.
 **/
int uvc_send_image(uint32_t u32Addr, uint32_t u32XferSize);

/**
 * @brief   Check App is connected
 * @return   1 if connected, not connected otherwise.
 **/
int uvc_is_connected(void);

#endif /* UVC_IMG_H */
