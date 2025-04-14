/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
// Auto-generated file
// ** DO NOT EDIT **

#ifndef TIMING_ADAPTER_SETTINGS_H
#define TIMING_ADAPTER_SETTINGS_H

#define TA0_BASE       (0x58103000)
#define TA1_BASE       (0x58103200)

/* Timing adapter settings for AXI0 */
#if defined(TA0_BASE)

#define TA0_MAXR           (8)
#define TA0_MAXW           (8)
#define TA0_MAXRW          (0)
#define TA0_RLATENCY       (32)
#define TA0_WLATENCY       (32)
#define TA0_PULSE_ON       (3999)
#define TA0_PULSE_OFF      (1)
#define TA0_BWCAP          (4000)
#define TA0_PERFCTRL       (0)
#define TA0_PERFCNT        (0)
#define TA0_MODE           (1)
#define TA0_HISTBIN        (0)
#define TA0_HISTCNT        (0)

#endif /* defined(TA0_BASE) */

/* Timing adapter settings for AXI1 */
#if defined(TA1_BASE)

#define TA1_MAXR           (2)
#define TA1_MAXW           (0)
#define TA1_MAXRW          (0)
#define TA1_RLATENCY       (64)
#define TA1_WLATENCY       (0)
#define TA1_PULSE_ON       (320)
#define TA1_PULSE_OFF      (80)
#define TA1_BWCAP          (50)
#define TA1_PERFCTRL       (0)
#define TA1_PERFCNT        (0)
#define TA1_MODE           (1)
#define TA1_HISTBIN        (0)
#define TA1_HISTCNT        (0)

#endif /* defined(TA1_BASE) */

#endif /* TIMING_ADAPTER_SETTINGS_H */
