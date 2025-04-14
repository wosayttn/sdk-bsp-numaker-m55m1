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
#ifndef MEMHEAP_HELPER_H
#define MEMHEAP_HELPER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum
{
    evAREANA_AT_SRAM,
    evAREANA_AT_HYPERRAM,
} E_AREANA_PLACEMENT;

void *memheap_helper_allocate(E_AREANA_PLACEMENT evAreanaPlacement, uint32_t u32Size);

void memheap_helper_free(E_AREANA_PLACEMENT evAreanaPlacement, void *p);

#endif /* MEMHEAP_HELPER_H */
