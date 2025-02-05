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
#ifndef EXT_FILE_H
#define EXT_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief   List file in specific folder path.
 * @return  0 on sucess, -1 if failure.
 **/
int ext_file_list(const char *pcDirPath);

/**
 * @brief   import file from external storage.
 * @return  0 on sucess, -1 if failure.
 **/
int ext_file_import(const char *pcPath, void **ppvBufAddr, uint32_t *pu32BufSize);

/**
 * @brief   export file to external storage.
 * @return  0 on sucess, -1 if failure.
 **/
int ext_file_export(const char *pcPath, void *pvBufAddr, uint32_t u32BufSize);

/**
 * @brief   get buffer crc32 checksum
 * @return  checksum value
 **/
uint32_t ext_file_checksum(uint32_t *ptr, uint32_t len);

/**
 * @brief   release memory resource
 * @return  none
 **/
void ext_file_release(void *pvBufAddr);

#endif /* EXT_FILE_H */
