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
#include "hal.h"                    /* Brings in platform definitions. */
#include "Classifier.hpp"           /* Classifier. */
#include "InputFiles.hpp"           /* For input images. */
#include "Labels.hpp"               /* For label strings. */
#include "VisualWakeWordModel.hpp" /* Model class for running inference. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

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
namespace vww
{
extern uint8_t *GetModelPointer();
extern size_t GetModelLen();
} /* namespace vww */
#endif
} /* namespace app */
} /* namespace arm */

using ViusalWakeWordClassifier = arm::app::Classifier;

void main_loop()
{
    arm::app::VisualWakeWordModel model;  /* Model wrapper object. */
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
#define DEF_MODEL_FILE_NAME      "vww4_128_128_INT8_vela_H256.tflite"
#define DEF_LABEL_FILE_NAME      "visual_wake_word_labels.txt"
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

    if (hal_ext_file_import(MLEVK_UC_DYNAMIC_LOAD_PATH "/" DEF_LABEL_FILE_NAME, &pvLabelBufAddr, &u32LabelBufLen) < 0)
    {
        printf_err("Failed to load label - %s\n", MLEVK_UC_DYNAMIC_LOAD_PATH "/" DEF_LABEL_FILE_NAME);
        goto exit_main_loop;
    }

#else
    void *pvModelBufAddr = arm::app::vww::GetModelPointer();
    uint32_t u32ModelBufLen = arm::app::vww::GetModelLen();
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

        arm::app::Profiler profiler{"vww"};
        caseContext.Set<arm::app::Profiler &>("profiler", profiler);
        caseContext.Set<arm::app::Model &>("model", model);

        ViusalWakeWordClassifier classifier;  /* Classifier wrapper object. */
        caseContext.Set<arm::app::Classifier &>("classifier", classifier);

        std::vector <std::string> labels;
#if defined(MLEVK_UC_DYNAMIC_LOAD)
        LoadLabelsVector(labels, pvLabelBufAddr, u32LabelBufLen);
#else
        GetLabelsVector(labels);
#endif
        caseContext.Set<const std::vector <std::string>&>("labels", labels);

#if defined(MLEVK_UC_LIVE_DEMO)
        ClassifyImageHandlerLive(caseContext);
#else
        caseContext.Set<uint32_t>("imgIndex", 0);
        /* Loop. */
        bool executionSuccessful = true;
        constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;
        do
        {
            int menuOption = common::MENU_OPT_RUN_INF_NEXT;
            if (bUseMenu)
            {
                DisplayCommonMenu();
                menuOption = arm::app::ReadUserInputAsInt();
                printf("\n");
            }

            switch (menuOption)
            {
            case common::MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = ClassifyImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), false);
                break;
            case common::MENU_OPT_RUN_INF_CHOSEN:
            {
                printf("    Enter the image index [0, %d]: ", NUMBER_OF_FILES - 1);
                fflush(stdout);
                auto imgIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                executionSuccessful = ClassifyImageHandler(caseContext, imgIndex, false);
                break;
            }
            case common::MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyImageHandler(caseContext, caseContext.Get<uint32_t>("imgIndex"), true);
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
