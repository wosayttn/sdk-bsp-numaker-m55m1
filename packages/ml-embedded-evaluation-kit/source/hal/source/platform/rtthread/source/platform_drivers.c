/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include "platform_drivers.h"

#include <string.h>

#if defined(ARM_NPU)
    #include "ethosu_npu_init.h"
#endif /* ARM_NPU */

static const char *s_platform_name = "rtthread";

int platform_init(void)
{
#if defined(ARM_NPU)

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init()))
    {
        return state;
    }

#endif /* ARM_NPU */

    return 0;
}

void platform_release(void)
{

#if defined(ARM_NPU)

    arm_ethosu_npu_deinit();

#endif

}

const char *platform_name(void)
{
    return s_platform_name;
}
