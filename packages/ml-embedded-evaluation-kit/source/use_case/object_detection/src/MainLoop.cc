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
#include "hal.h"                      /* Brings in platform definitions. */
#include "InputFiles.hpp"             /* For input images. */
#include "YoloFastestModel.hpp"       /* Model class for running inference. */
#include "UseCaseHandler.hpp"         /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"     /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

#undef ACTIVATION_BUF_SZ
#define ACTIVATION_BUF_SZ       (444000)

namespace arm
{
namespace app
{
#if defined(MLEVK_UC_AREANA_DYNAMIC_ALLOCATE)
    static uint8_t *tensorArena;
#else
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
#endif
#if !defined(MLEVK_UC_DYNAMIC_LOAD)
namespace object_detection
{
extern uint8_t *GetModelPointer();
extern size_t GetModelLen();
} /* namespace object_detection */
#endif
} /* namespace app */
} /* namespace arm */

static void DisplayDetectionMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run detection on next ifm\n", common::MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Run detection ifm at chosen index\n", common::MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run detection on all ifm\n", common::MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", common::MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List ifm\n", common::MENU_OPT_LIST_IFM);
    printf("  %u. Quit\n\n", common::MENU_OPT_QUIT);
    printf("  Choice: ");
    fflush(stdout);
}

void main_loop()
{
    arm::app::YoloFastestModel model;  /* Model wrapper object. */
    void *pvAreanaBufAddr = arm::app::tensorArena;
    uint32_t u32AreanaBufLen = ACTIVATION_BUF_SZ;

#if defined(MLEVK_UC_AREANA_DYNAMIC_ALLOCATE)
    pvAreanaBufAddr = (uint8_t *)hal_memheap_helper_allocate(
#if defined(MLEVK_UC_AREANA_PLACE_SRAM)
                          evAREANA_AT_SRAM,
#elif defined(MLEVK_UC_AREANA_PLACE_HYPERRAM)
                          evAREANA_AT_HYPERRAM,
#endif
                          ACTIVATION_BUF_SZ);
    if (pvAreanaBufAddr == NULL)
    {
        printf_err("Failed to allocate tensorAreana cache memory.( %d Bytes)\n", ACTIVATION_BUF_SZ);
        return;
    }
#endif

#if defined(MLEVK_UC_DYNAMIC_LOAD)
#define DEF_MODEL_FILE_NAME      "yolo-fastest_192_face_v4_vela_H256.tflite"

    void *pvModelBufAddr = NULL;
    uint32_t u32ModelBufLen = 0;
    void *pvLabelBufAddr = NULL;
    uint32_t u32LabelBufLen = 0;

    hal_ext_file_list(MLEVK_UC_DYNAMIC_LOAD_PATH);
    if (hal_ext_file_import(MLEVK_UC_DYNAMIC_LOAD_PATH "/" DEF_MODEL_FILE_NAME, &pvModelBufAddr, &u32ModelBufLen) < 0)
    {
        printf_err("Failed to load model - %s\n", MLEVK_UC_DYNAMIC_LOAD_PATH "/" DEF_MODEL_FILE_NAME);
        goto exit_main_loop;
    }

#else
    void *pvModelBufAddr = arm::app::object_detection::GetModelPointer();
    uint32_t u32ModelBufLen = arm::app::object_detection::GetModelLen();
#endif

    /* Load the model. */
    if (!model.Init((uint8_t *)pvAreanaBufAddr,
                    u32AreanaBufLen,
                    (const uint8_t *)pvModelBufAddr,
                    u32ModelBufLen))
    {
        printf_err("Failed to initialise model\n");
        goto exit_main_loop;
    }
    else
    {
        /* Instantiate application context. */
        arm::app::ApplicationContext caseContext;

        arm::app::Profiler profiler{"object_detection"};
        caseContext.Set<arm::app::Profiler &>("profiler", profiler);
        caseContext.Set<arm::app::Model &>("model", model);

#if defined(MLEVK_UC_LIVE_DEMO)
        ObjectDetectionHandlerLive(caseContext);
#else
        caseContext.Set<uint32_t>("imgIndex", 0);

        /* Loop. */
        bool executionSuccessful = true;

        /* Loop. */
        constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;

        /* Loop. */
        do
        {
            int menuOption = common::MENU_OPT_RUN_INF_NEXT;
            if (bUseMenu)
            {
                DisplayDetectionMenu();
                menuOption = arm::app::ReadUserInputAsInt();
                printf("\n");
            }

            switch (menuOption)
            {
            case common::MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = ObjectDetectionHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false);
                break;
            case common::MENU_OPT_RUN_INF_CHOSEN:
            {
                printf("    Enter the image index [0, %d]: ", NUMBER_OF_FILES - 1);
                fflush(stdout);
                auto imgIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                executionSuccessful = ObjectDetectionHandler(caseContext, imgIndex, false);
                break;
            }
            case common::MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ObjectDetectionHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), true);
                break;
            case common::MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            case common::MENU_OPT_LIST_IFM:
                executionSuccessful = ListFilesHandler(caseContext);
                break;
            case common::MENU_OPT_QUIT:
                executionSuccessful = false;
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
            }
        }
        while (executionSuccessful && bUseMenu);

#endif
    }

exit_main_loop:

#if defined(MLEVK_UC_AREANA_DYNAMIC_ALLOCATE)
    if (pvAreanaBufAddr)
    {
        hal_memheap_helper_free(
#if defined(MLEVK_UC_AREANA_PLACE_SRAM)
            evAREANA_AT_SRAM,
#elif defined(MLEVK_UC_AREANA_PLACE_HYPERRAM)
            evAREANA_AT_HYPERRAM,
#endif
            pvAreanaBufAddr);
#endif
    }

#if defined(MLEVK_UC_DYNAMIC_LOAD)
    if (pvModelBufAddr)
    {
        hal_ext_file_release(pvModelBufAddr);
    }

    if (pvLabelBufAddr)
    {
        hal_ext_file_release(pvLabelBufAddr);
    }
#endif

    info("Main loop terminated.\n");
}
