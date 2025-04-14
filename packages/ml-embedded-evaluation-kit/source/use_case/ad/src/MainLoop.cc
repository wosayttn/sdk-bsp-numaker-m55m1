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
#include "InputFiles.hpp"           /* For input data */
#include "AdModel.hpp"              /* Model class for running inference */
#include "UseCaseCommonUtils.hpp"   /* Utils functions */
#include "UseCaseHandler.hpp"       /* Handlers for different user options */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm
{
namespace app
{
namespace ad
{
extern uint8_t *GetModelPointer();
extern size_t GetModelLen();
} /* namespace ad */
static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

enum opcodes
{
    MENU_OPT_RUN_INF_NEXT = 1,       /* Run on next vector */
    MENU_OPT_RUN_INF_CHOSEN,         /* Run on a user provided vector index */
    MENU_OPT_RUN_INF_ALL,            /* Run inference on all */
    MENU_OPT_SHOW_MODEL_INFO,        /* Show model info */
    MENU_OPT_LIST_AUDIO_CLIPS        /* List the current baked audio signals */
};

static void DisplayMenu()
{
    printf("\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Classify next audio signal\n", MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Classify audio signal at chosen index\n", MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run classification on all audio signals\n", MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List audio signals\n\n", MENU_OPT_LIST_AUDIO_CLIPS);
    printf("  Choice: ");
    fflush(stdout);
}

void main_loop()
{
    arm::app::AdModel model;  /* Model wrapper object. */
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
#define DEF_MODEL_FILE_NAME      "ad_medium_int8_vela_H256.tflite"

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

        arm::app::Profiler profiler{"ad"};
        caseContext.Set<arm::app::Profiler &>("profiler", profiler);
        caseContext.Set<arm::app::Model &>("model", model);
        caseContext.Set<uint32_t>("frameLength", arm::app::ad::g_FrameLength);
        caseContext.Set<uint32_t>("frameStride", arm::app::ad::g_FrameStride);
        caseContext.Set<float>("scoreThreshold", arm::app::ad::g_ScoreThreshold);
        caseContext.Set<float>("trainingMean", arm::app::ad::g_TrainingMean);

#if defined(MLEVK_UC_LIVE_DEMO)
        ClassifyVibrationHandlerLive(caseContext);
#else
        constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;
        caseContext.Set<uint32_t>("clipIndex", 0);
        /* Main program loop. */
        bool executionSuccessful = true;

        /* Loop. */
        do
        {
            int menuOption = MENU_OPT_RUN_INF_NEXT;
            if (bUseMenu)
            {
                DisplayMenu();
                menuOption = arm::app::ReadUserInputAsInt();
                printf("\n");
            }
            switch (menuOption)
            {
            case MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = ClassifyVibrationHandler(
                                          caseContext,
                                          caseContext.Get<uint32_t>("clipIndex"),
                                          false);
                break;
            case MENU_OPT_RUN_INF_CHOSEN:
            {
                printf("    Enter the data index [0, %d]: ",
                       NUMBER_OF_FILES - 1);
                fflush(stdout);
                auto audioIndex = static_cast<uint32_t>(
                                      arm::app::ReadUserInputAsInt());
                executionSuccessful = ClassifyVibrationHandler(caseContext,
                                      audioIndex,
                                      false);
                break;
            }
            case MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyVibrationHandler(
                                          caseContext,
                                          caseContext.Get<uint32_t>("clipIndex"),
                                          true);
                break;
            case MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            case MENU_OPT_LIST_AUDIO_CLIPS:
                executionSuccessful = ListFilesHandler(caseContext);
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
