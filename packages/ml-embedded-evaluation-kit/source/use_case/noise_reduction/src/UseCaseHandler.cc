/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include "UseCaseHandler.hpp"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "InputFiles.hpp"
#include "RNNoiseFeatureProcessor.hpp"
#include "RNNoiseModel.hpp"
#include "RNNoiseProcessing.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "log_macros.h"

//#define DEF_DUMP_WAV

namespace arm
{
namespace app
{
bool bDoDenoise = true;

/**
 * @brief            Helper function to increment current audio clip features index.
 * @param[in,out]    ctx   Pointer to the application context object.
 **/
static void IncrementAppCtxClipIdx(ApplicationContext &ctx);

/* Noise reduction inference handler. */
bool NoiseReductionHandler(ApplicationContext &ctx, bool runAll)
{
    constexpr uint32_t dataPsnTxtInfStartX = 20;
    constexpr uint32_t dataPsnTxtInfStartY = 40;

    /* Variables used for memory dumping. */
    size_t memDumpMaxLen            = 0;
    uint8_t *memDumpBaseAddr        = nullptr;
    size_t undefMemDumpBytesWritten = 0;
    size_t *pMemDumpBytesWritten    = &undefMemDumpBytesWritten;
    if (ctx.Has("MEM_DUMP_LEN") && ctx.Has("MEM_DUMP_BASE_ADDR") &&
            ctx.Has("MEM_DUMP_BYTE_WRITTEN"))
    {
        memDumpMaxLen        = ctx.Get<size_t>("MEM_DUMP_LEN");
        memDumpBaseAddr      = ctx.Get<uint8_t *>("MEM_DUMP_BASE_ADDR");
        pMemDumpBytesWritten = ctx.Get<size_t *>("MEM_DUMP_BYTE_WRITTEN");
    }
    *pMemDumpBytesWritten = 0; // Wayne: reset the Written value every run.

    std::reference_wrapper<size_t> memDumpBytesWritten = std::ref(*pMemDumpBytesWritten);
    auto &profiler                                     = ctx.Get<Profiler &>("profiler");

    /* Get model reference. */
    auto &model = ctx.Get<RNNoiseModel &>("model");
    if (!model.IsInited())
    {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

    /* Populate Pre-Processing related parameters. */
    auto audioFrameLen      = ctx.Get<uint32_t>("frameLength");
    auto audioFrameStride   = ctx.Get<uint32_t>("frameStride");
    auto nrNumInputFeatures = ctx.Get<uint32_t>("numInputFeatures");

    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    if (nrNumInputFeatures != inputTensor->bytes)
    {
        printf_err("Input features size must be equal to input tensor size."
                   " Feature size = %" PRIu32 ", Tensor size = %zu.\n",
                   nrNumInputFeatures,
                   inputTensor->bytes);
        return false;
    }

    TfLiteTensor *outputTensor = model.GetOutputTensor(model.m_indexForModelOutput);

    /* Initial choice of index for WAV file. */
    auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

    std::function<const int16_t *(const uint32_t)> audioAccessorFunc = GetAudioArray;
    if (ctx.Has("features"))
    {
        audioAccessorFunc = ctx.Get<std::function<const int16_t *(const uint32_t)>>("features");
    }
    std::function<uint32_t(const uint32_t)> audioSizeAccessorFunc = GetAudioArraySize;
    if (ctx.Has("featureSizes"))
    {
        audioSizeAccessorFunc =
            ctx.Get<std::function<uint32_t(const uint32_t)>>("featureSizes");
    }
    std::function<const char *(const uint32_t)> audioFileAccessorFunc = GetFilename;
    if (ctx.Has("featureFileNames"))
    {
        audioFileAccessorFunc =
            ctx.Get<std::function<const char *(const uint32_t)>>("featureFileNames");
    }

    do
    {
        hal_lcd_clear(COLOR_BLACK);

        auto startDumpAddress = memDumpBaseAddr + memDumpBytesWritten;
        auto currentIndex     = ctx.Get<uint32_t>("clipIndex");

        /* Creating a sliding window through the audio. */
        auto audioDataSlider =
            audio::SlidingWindow<const int16_t>(audioAccessorFunc(currentIndex),
                                                audioSizeAccessorFunc(currentIndex),
                                                audioFrameLen,
                                                audioFrameStride);

        info("Running inference on input feature map %" PRIu32 " => %s\n",
             currentIndex,
             audioFileAccessorFunc(currentIndex));

        memDumpBytesWritten +=
#if defined(DEF_DUMP_WAV)
            DumpDenoisedAudioWAVHeader(memDumpBaseAddr + memDumpBytesWritten,
                                       memDumpMaxLen - memDumpBytesWritten,
                                       (audioDataSlider.TotalStrides() + 1) * audioFrameLen * sizeof(uint16_t),
                                       48000,
                                       16,
                                       1);
#else
            DumpDenoisedAudioHeader(audioFileAccessorFunc(currentIndex),
                                    (audioDataSlider.TotalStrides() + 1) * audioFrameLen,
                                    memDumpBaseAddr + memDumpBytesWritten,
                                    memDumpMaxLen - memDumpBytesWritten);
#endif

        /* Set up pre and post-processing. */
        std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor =
            std::make_shared<rnn::RNNoiseFeatureProcessor>();
        std::shared_ptr<rnn::FrameFeatures> frameFeatures =
            std::make_shared<rnn::FrameFeatures>();

        RNNoisePreProcess preProcess =
            RNNoisePreProcess(inputTensor, featureProcessor, frameFeatures);

        std::vector<int16_t> denoisedAudioFrame(audioFrameLen);
        RNNoisePostProcess postProcess = RNNoisePostProcess(
                                             outputTensor, denoisedAudioFrame, featureProcessor, frameFeatures);

        bool resetGRU = true;

        while (audioDataSlider.HasNext())
        {
            const int16_t *inferenceWindow = audioDataSlider.Next();

            if (!preProcess.DoPreProcess(inferenceWindow, audioFrameLen))
            {
                printf_err("Pre-processing failed.");
                return false;
            }

            /* Reset or copy over GRU states first to avoid TFLu memory overlap issues. */
            if (resetGRU)
            {
                model.ResetGruState();
            }
            else
            {
                /* Copying gru state outputs to gru state inputs.
                 * Call ResetGruState in between the sequence of inferences on unrelated input
                 * data. */
                model.CopyGruStates();
            }

            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            /* Display message on the LCD - inference running. */
            hal_lcd_display_text(str_inf.c_str(),
                                 str_inf.size(),
                                 dataPsnTxtInfStartX,
                                 dataPsnTxtInfStartY,
                                 false);

            info("Inference %zu/%zu\n",
                 audioDataSlider.Index() + 1,
                 audioDataSlider.TotalStrides() + 1);

            /* Run inference over this feature sliding window. */
            if (!RunInference(model, profiler))
            {
                printf_err("Inference failed.");
                return false;
            }
            resetGRU = false;

            /* Carry out post-processing. */
            if (!postProcess.DoPostProcess())
            {
                printf_err("Post-processing failed.");
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_lcd_display_text(str_inf.c_str(),
                                 str_inf.size(),
                                 dataPsnTxtInfStartX,
                                 dataPsnTxtInfStartY,
                                 false);

            if (memDumpMaxLen > 0)
            {
                /* Dump final post processed output to memory. */
                memDumpBytesWritten +=
                    DumpOutputDenoisedAudioFrame(denoisedAudioFrame,
                                                 memDumpBaseAddr + memDumpBytesWritten,
                                                 memDumpMaxLen - memDumpBytesWritten);
            }

        } // while (audioDataSlider.HasNext())

        if (memDumpMaxLen > 0)
        {
            /* Needed to not let the compiler complain about type mismatch. */
            size_t valMemDumpBytesWritten = memDumpBytesWritten;
            info("Output memory dump of %zu bytes written at address 0x%p\n",
                 valMemDumpBytesWritten,
                 startDumpAddress);

#if defined(DEF_DUMP_WAV)
#else
            /* Finish by dumping the footer. */
            memDumpBytesWritten += DumpDenoisedAudioFooter(
                                       memDumpBaseAddr + memDumpBytesWritten,
                                       memDumpMaxLen - memDumpBytesWritten);
#endif
            char szPath[256];
            snprintf(szPath, sizeof(szPath), "%s/ad/rnn_%s", MLEVK_UC_DYNAMIC_LOAD_PATH, (const char *)audioFileAccessorFunc(currentIndex));
            hal_ext_file_export(
                szPath,
                (void *)startDumpAddress,
                memDumpBytesWritten);

        }

        info("All inferences for audio clip complete.\n");
        profiler.PrintProfilingResult();
        IncrementAppCtxClipIdx(ctx);

        std::string clearString{' '};
        hal_lcd_display_text(clearString.c_str(),
                             clearString.size(),
                             dataPsnTxtInfStartX,
                             dataPsnTxtInfStartY,
                             false);

        std::string completeMsg{"Inference complete!"};

        /* Display message on the LCD - inference complete. */
        hal_lcd_display_text(completeMsg.c_str(),
                             completeMsg.size(),
                             dataPsnTxtInfStartX,
                             dataPsnTxtInfStartY,
                             false);

    }
    while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

    return true;
}




#if defined(DEF_DUMP_WAV)

/* Noise reduction inference handler. */
bool NoiseReductionHandlerLive(ApplicationContext &ctx)
{
    /* Variables used for memory dumping. */
    size_t memDumpMaxLen            = 0;
    uint8_t *memDumpBaseAddr        = nullptr;
    size_t undefMemDumpBytesWritten = 0;
    size_t *pMemDumpBytesWritten    = &undefMemDumpBytesWritten;
    if (ctx.Has("MEM_DUMP_LEN") && ctx.Has("MEM_DUMP_BASE_ADDR") &&
            ctx.Has("MEM_DUMP_BYTE_WRITTEN"))
    {
        memDumpMaxLen        = ctx.Get<size_t>("MEM_DUMP_LEN");
        memDumpBaseAddr      = ctx.Get<uint8_t *>("MEM_DUMP_BASE_ADDR");
        pMemDumpBytesWritten = ctx.Get<size_t *>("MEM_DUMP_BYTE_WRITTEN");
    }
    *pMemDumpBytesWritten = 0; // Wayne: reset the Written value every run.

    std::reference_wrapper<size_t> memDumpBytesWritten = std::ref(*pMemDumpBytesWritten);

    auto &profiler = ctx.Get<Profiler &>("profiler");

    /* Get model reference. */
    auto &model = ctx.Get<RNNoiseModel &>("model");
    if (!model.IsInited())
    {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

    /* Populate Pre-Processing related parameters. */
    auto audioFrameLen      = ctx.Get<uint32_t>("frameLength");
    auto audioFrameStride   = ctx.Get<uint32_t>("frameStride");
    auto nrNumInputFeatures = ctx.Get<uint32_t>("numInputFeatures");

    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    if (nrNumInputFeatures != inputTensor->bytes)
    {
        printf_err("Input features size must be equal to input tensor size."
                   " Feature size = %" PRIu32 ", Tensor size = %zu.\n",
                   nrNumInputFeatures,
                   inputTensor->bytes);
        return false;
    }

    TfLiteTensor *outputTensor = model.GetOutputTensor(model.m_indexForModelOutput);

#define MODEL_SAMPLE_RATE        48000
#define MODEL_SAMPLE_BIT         16
#define MODEL_SAMPLE_BYTE        (MODEL_SAMPLE_BIT/8)
#define MODEL_CHANNEL            1
#define AUDIO_RECORD_SECOND      5
#define AUDIO_CLIP_BYTESIZE      (MODEL_SAMPLE_RATE * MODEL_SAMPLE_BYTE * AUDIO_RECORD_SECOND)

    info("MODEL_SAMPLE_RATE: %d\n", MODEL_SAMPLE_RATE);
    info("MODEL_SAMPLE_BIT: %d\n", MODEL_SAMPLE_BIT);
    info("MODEL_SAMPLE_BYTE: %d\n", MODEL_SAMPLE_BYTE);
    info("MODEL_CHANNEL: %d\n", MODEL_CHANNEL);
    info("AUDIO_CLIP_BYTESIZE: %d\n", AUDIO_CLIP_BYTESIZE);
    info("AUDIO_RECORD_SECOND: %d\n", AUDIO_RECORD_SECOND);

    /* Set 48K Sample rate, 16-bit, mono for this model. */
    if (hal_audio_capture_init(MODEL_SAMPLE_RATE, MODEL_SAMPLE_BIT, MODEL_CHANNEL) < 0)
    {
        printf_err("Failed to initialise audio capture device\n");
        return false;
    }

    uint8_t *pu8AudioClipFrameBuf = NULL;
    pu8AudioClipFrameBuf = (uint8_t *)hal_memheap_helper_allocate(
                               evAREANA_AT_HYPERRAM,
                               AUDIO_CLIP_BYTESIZE);
    if (pu8AudioClipFrameBuf == NULL)
    {
        printf_err("Failed to allocate audio clip memory.( %d Bytes)\n", AUDIO_CLIP_BYTESIZE);
        hal_audio_capture_fini();
        return false;
    }

    /* Set up pre and post-processing. */
    std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor = std::make_shared<rnn::RNNoiseFeatureProcessor>();
    std::shared_ptr<rnn::FrameFeatures> frameFeatures = std::make_shared<rnn::FrameFeatures>();

    RNNoisePreProcess preProcess = RNNoisePreProcess(inputTensor, featureProcessor, frameFeatures);

    std::vector<int16_t> denoisedAudioFrame(audioFrameLen);
    RNNoisePostProcess postProcess = RNNoisePostProcess(outputTensor, denoisedAudioFrame, featureProcessor, frameFeatures);

    bool resetGRU = true;
    uint32_t u32ClipByteSize = hal_audio_capture_get_frame(&pu8AudioClipFrameBuf[0], AUDIO_CLIP_BYTESIZE);
    hal_audio_capture_fini();

    /* Creating a sliding window through the audio. */
    auto audioDataSlider = audio::SlidingWindow<const int16_t>((int16 *)pu8AudioClipFrameBuf,
                           u32ClipByteSize / MODEL_SAMPLE_BYTE, // Sample number, not byte number.
                           audioFrameLen,
                           audioFrameStride);

    /* Before de-noise wav */
    auto startDumpAddress = memDumpBaseAddr;
    size_t WavBytesWritten = 0;

    WavBytesWritten +=
        DumpDenoisedAudioWAVHeader(memDumpBaseAddr,
                                   memDumpMaxLen,
                                   u32ClipByteSize,
                                   MODEL_SAMPLE_RATE,
                                   MODEL_SAMPLE_BIT,
                                   MODEL_CHANNEL);

    std::memcpy(memDumpBaseAddr + WavBytesWritten, &pu8AudioClipFrameBuf[0], u32ClipByteSize);
    WavBytesWritten += u32ClipByteSize;

    char szPath[256];
    snprintf(szPath, sizeof(szPath), "%s/ad/before_denoise_%d_%d_%d.wav",
             MLEVK_UC_DYNAMIC_LOAD_PATH,
             MODEL_SAMPLE_RATE,
             MODEL_SAMPLE_BIT,
             MODEL_CHANNEL);

    hal_ext_file_export(
        szPath,
        (void *)startDumpAddress,
        WavBytesWritten);

    /* After de-noise wav */
    memDumpBytesWritten +=
        DumpDenoisedAudioWAVHeader(memDumpBaseAddr,
                                   memDumpMaxLen,
                                   (audioDataSlider.TotalStrides() + 1) * audioFrameLen * sizeof(uint16_t),
                                   MODEL_SAMPLE_RATE,
                                   MODEL_SAMPLE_BIT,
                                   MODEL_CHANNEL);

    while (audioDataSlider.HasNext())
    {
        const int16_t *inferenceWindow = audioDataSlider.Next();

        if (!preProcess.DoPreProcess(inferenceWindow, audioFrameLen))
        {
            printf_err("Pre-processing failed.");
            break;
        }

        /* Reset or copy over GRU states first to avoid TFLu memory overlap issues. */
        if (resetGRU)
        {
            model.ResetGruState();
        }
        else
        {
            /* Copying gru state outputs to gru state inputs.
             * Call ResetGruState in between the sequence of inferences on unrelated input
             * data. */
            model.CopyGruStates();
        }

        /* Run inference over this feature sliding window. */
        if (!RunInference(model, profiler))
        {
            printf_err("Inference failed.");
            break;
        }
        resetGRU = false;

        /* Carry out post-processing. */
        if (!postProcess.DoPostProcess())
        {
            printf_err("Post-processing failed.");
            break;
        }

        /* Dump final post processed output to memory. */
        memDumpBytesWritten +=
            DumpOutputDenoisedAudioFrame(denoisedAudioFrame,
                                         memDumpBaseAddr + memDumpBytesWritten,
                                         memDumpMaxLen - memDumpBytesWritten);

    } // while (audioDataSlider.HasNext())

    snprintf(szPath, sizeof(szPath), "%s/ad/after_denoise_%d_%d_%d.wav",
             MLEVK_UC_DYNAMIC_LOAD_PATH,
             MODEL_SAMPLE_RATE,
             MODEL_SAMPLE_BIT,
             MODEL_CHANNEL);

    hal_ext_file_export(
        szPath,
        (void *)startDumpAddress,
        memDumpBytesWritten);

    if (pu8AudioClipFrameBuf)
        hal_memheap_helper_free(evAREANA_AT_HYPERRAM, pu8AudioClipFrameBuf);

    return false;
}

#else

/* Noise reduction inference handler. */
bool NoiseReductionHandlerLive(ApplicationContext &ctx)
{
    auto &profiler = ctx.Get<Profiler &>("profiler");

    /* Get model reference. */
    auto &model = ctx.Get<RNNoiseModel &>("model");
    if (!model.IsInited())
    {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

    /* Populate Pre-Processing related parameters. */
    auto audioFrameLen      = ctx.Get<uint32_t>("frameLength");
    auto audioFrameStride   = ctx.Get<uint32_t>("frameStride");
    auto nrNumInputFeatures = ctx.Get<uint32_t>("numInputFeatures");

    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    if (nrNumInputFeatures != inputTensor->bytes)
    {
        printf_err("Input features size must be equal to input tensor size."
                   " Feature size = %" PRIu32 ", Tensor size = %zu.\n",
                   nrNumInputFeatures,
                   inputTensor->bytes);
        return false;
    }

    TfLiteTensor *outputTensor = model.GetOutputTensor(model.m_indexForModelOutput);

#define MODEL_SAMPLE_RATE        16000
#define MODEL_SAMPLE_BIT         16
#define MODEL_SAMPLE_BYTE        (MODEL_SAMPLE_BIT/8)
#define MODEL_CHANNEL            1
#define AUDIO_CLIP_BYTESIZE      (4 * audioFrameLen * MODEL_SAMPLE_BYTE)

    info("MODEL_SAMPLE_RATE: %d\n", MODEL_SAMPLE_RATE);
    info("MODEL_SAMPLE_BIT: %d\n", MODEL_SAMPLE_BIT);
    info("MODEL_SAMPLE_BYTE: %d\n", MODEL_SAMPLE_BYTE);
    info("MODEL_CHANNEL: %d\n", MODEL_CHANNEL);
    info("AUDIO_CLIP_BYTESIZE: %d\n", AUDIO_CLIP_BYTESIZE);

    /* Set 48K Sample rate, 16-bit, mono for this model. */
    if (hal_audio_capture_init(MODEL_SAMPLE_RATE, MODEL_SAMPLE_BIT, MODEL_CHANNEL) < 0)
    {
        printf_err("Failed to initialise audio capture device\n");
        return false;
    }
    if (hal_audio_playback_init(MODEL_SAMPLE_RATE, MODEL_SAMPLE_BIT, MODEL_CHANNEL) < 0)
    {
        printf_err("Failed to initialise audio capture device\n");
        return false;
    }

    uint8_t *pu8AudioClipFrameBuf = NULL;
    pu8AudioClipFrameBuf = (uint8_t *)hal_memheap_helper_allocate(
                               evAREANA_AT_SRAM,
                               AUDIO_CLIP_BYTESIZE);
    if (pu8AudioClipFrameBuf == NULL)
    {
        printf_err("Failed to allocate audio clip memory.( %d Bytes)\n", AUDIO_CLIP_BYTESIZE);
        hal_audio_capture_fini();
        return false;
    }

    /* Set up pre and post-processing. */
    std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor = std::make_shared<rnn::RNNoiseFeatureProcessor>();
    std::shared_ptr<rnn::FrameFeatures> frameFeatures = std::make_shared<rnn::FrameFeatures>();

    RNNoisePreProcess preProcess = RNNoisePreProcess(inputTensor, featureProcessor, frameFeatures);

    std::vector<int16_t> denoisedAudioFrame(audioFrameLen);
    RNNoisePostProcess postProcess = RNNoisePostProcess(outputTensor, denoisedAudioFrame, featureProcessor, frameFeatures);

    bool bQuit = false;
    bool resetGRU = true;
    while (!bQuit)
    {
        uint32_t u32ClipByteSize = hal_audio_capture_get_frame(&pu8AudioClipFrameBuf[0], AUDIO_CLIP_BYTESIZE);
        if (!bDoDenoise)
        {
            /* bypass mode. */
            hal_audio_playback_put_frame(&pu8AudioClipFrameBuf[0], u32ClipByteSize);
            continue;
        }

        /* Creating a sliding window through the audio. */
        auto audioDataSlider = audio::SlidingWindow<const int16_t>((int16 *)pu8AudioClipFrameBuf,
                               u32ClipByteSize / MODEL_SAMPLE_BYTE, // Sample number, not byte number.
                               audioFrameLen,
                               audioFrameStride);

        size_t TotalnumByteToBePlay = 0;

        while (audioDataSlider.HasNext())
        {
            const int16_t *inferenceWindow = audioDataSlider.Next();

            if (!preProcess.DoPreProcess(inferenceWindow, audioFrameLen))
            {
                printf_err("Pre-processing failed.");
                bQuit = true;
                break;
            }

            /* Reset or copy over GRU states first to avoid TFLu memory overlap issues. */
            if (resetGRU)
            {
                model.ResetGruState();
            }
            else
            {
                /* Copying gru state outputs to gru state inputs.
                 * Call ResetGruState in between the sequence of inferences on unrelated input
                 * data. */
                model.CopyGruStates();
            }

            /* Run inference over this feature sliding window. */
            if (!RunInference(model, profiler))
            {
                printf_err("Inference failed.");
                bQuit = true;
                break;
            }
            resetGRU = false;

            /* Carry out post-processing. */
            if (!postProcess.DoPostProcess())
            {
                printf_err("Post-processing failed.");
                bQuit = true;
                break;
            }

            {
                size_t numByteToBePlay = denoisedAudioFrame.size() * sizeof(int16_t);
                hal_audio_playback_put_frame((uint8_t *)denoisedAudioFrame.data(), numByteToBePlay);
                TotalnumByteToBePlay += numByteToBePlay;
            }

        } // while (audioDataSlider.HasNext())
    }

    hal_audio_capture_fini();
    hal_audio_playback_fini();

    if (pu8AudioClipFrameBuf)
        hal_memheap_helper_free(evAREANA_AT_SRAM, pu8AudioClipFrameBuf);

    return false;
}

#endif

size_t DumpDenoisedAudioWAVHeader(uint8_t *memAddress,
                                  size_t memSize,
                                  size_t dumpSize,
                                  int sample_rate,
                                  int sample_bit,
                                  int channels)
{
    /* WAV Header structure. */
    struct S_WAV_HDR
    {
        char  riff_id[4];             /* "RIFF" */
        int   riff_datasize;          /* RIFF chunk data size,exclude riff_id[4] and riff_datasize,total - 8 */
        char  riff_type[4];           /* "WAVE" */
        char  fmt_id[4];              /* "fmt " */
        int   fmt_datasize;           /* fmt chunk data size, 16 for pcm */
        short fmt_compression_code;   /* 1 for PCM */
        short fmt_channels;           /* 1(mono) or 2(stereo) */
        int   fmt_sample_rate;        /* samples per second */
        int   fmt_avg_bytes_per_sec;  /* sample_rate * channels * bit_per_sample / 8 */
        short fmt_block_align;        /* number bytes per sample, bit_per_sample * channels / 8 */
        short fmt_bit_per_sample;     /* bits of each sample(8,16,32). */
        char  data_id[4];             /* "data" */
        int   data_datasize;          /* data chunk size,pcm_size - 44 */
    };

    if (memAddress == nullptr)
    {
        return 0;
    }

    size_t numBytesWritten = 0;
    S_WAV_HDR *psWavHdr = (S_WAV_HDR *)memAddress;

    std::memcpy(psWavHdr->riff_id, "RIFF", 4);
    psWavHdr->riff_datasize = dumpSize + sizeof(struct S_WAV_HDR) - 8;
    std::memcpy(psWavHdr->riff_type, "WAVE", 4);
    std::memcpy(psWavHdr->fmt_id, "fmt ", 4);
    psWavHdr->fmt_datasize = 16;
    psWavHdr->fmt_compression_code = 1;
    psWavHdr->fmt_channels = channels;
    psWavHdr->fmt_sample_rate = sample_rate;
    psWavHdr->fmt_bit_per_sample = sample_bit;
    psWavHdr->fmt_avg_bytes_per_sec = psWavHdr->fmt_sample_rate * psWavHdr->fmt_channels * psWavHdr->fmt_bit_per_sample / 8;
    psWavHdr->fmt_block_align = psWavHdr->fmt_bit_per_sample * psWavHdr->fmt_channels / 8;
    std::memcpy(psWavHdr->data_id, "data", 4);
    psWavHdr->data_datasize = dumpSize;

    size_t numBytesToWrite = sizeof(struct S_WAV_HDR);

    numBytesWritten += numBytesToWrite;

    info("Audio Clip dump wav header info (%zu bytes) written to %p\n",
         numBytesWritten,
         memAddress);

    return numBytesWritten;
}

size_t DumpDenoisedAudioHeader(const char *filename,
                               size_t dumpSize,
                               uint8_t *memAddress,
                               size_t memSize)
{

    if (memAddress == nullptr)
    {
        return 0;
    }

    int32_t filenameLength = strlen(filename);
    size_t numBytesWritten = 0;
    size_t numBytesToWrite = 0;
    int32_t dumpSizeByte   = dumpSize * sizeof(int16_t);
    bool overflow          = false;

    /* Write the filename length */
    numBytesToWrite = sizeof(filenameLength);
    if (memSize - numBytesToWrite > 0)
    {
        std::memcpy(memAddress, &filenameLength, numBytesToWrite);
        numBytesWritten += numBytesToWrite;
        memSize -= numBytesWritten;
    }
    else
    {
        overflow = true;
    }

    /* Write file name */
    numBytesToWrite = filenameLength;
    if (memSize - numBytesToWrite > 0)
    {
        std::memcpy(memAddress + numBytesWritten, filename, numBytesToWrite);
        numBytesWritten += numBytesToWrite;
        memSize -= numBytesWritten;
    }
    else
    {
        overflow = true;
    }

    /* Write dumpSize in byte */
    numBytesToWrite = sizeof(dumpSizeByte);
    if (memSize - numBytesToWrite > 0)
    {
        std::memcpy(memAddress + numBytesWritten, &(dumpSizeByte), numBytesToWrite);
        numBytesWritten += numBytesToWrite;
        memSize -= numBytesWritten;
    }
    else
    {
        overflow = true;
    }

    if (false == overflow)
    {
        info("Audio Clip dump header info (%zu bytes) written to %p\n",
             numBytesWritten,
             memAddress);
    }
    else
    {
        printf_err("Not enough memory to dump Audio Clip header.\n");
    }

    return numBytesWritten;
}

size_t DumpDenoisedAudioFooter(uint8_t *memAddress, size_t memSize)
{
    if ((memAddress == nullptr) || (memSize < 4))
    {
        return 0;
    }
    const int32_t eofMarker = -1;
    std::memcpy(memAddress, &eofMarker, sizeof(int32_t));

    return sizeof(int32_t);
}

size_t DumpOutputDenoisedAudioFrame(const std::vector<int16_t> &audioFrame,
                                    uint8_t *memAddress,
                                    size_t memSize)
{
    if (memAddress == nullptr)
    {
        return 0;
    }

    size_t numByteToBeWritten = audioFrame.size() * sizeof(int16_t);
    if (numByteToBeWritten > memSize)
    {
        printf_err("Overflow error: Writing %zu of %zu bytes to memory @ 0x%p.\n",
                   memSize,
                   numByteToBeWritten,
                   memAddress);
        numByteToBeWritten = memSize;
    }

    std::memcpy(memAddress, audioFrame.data(), numByteToBeWritten);
    info("Copied %zu bytes to %p\n", numByteToBeWritten, memAddress);

    return numByteToBeWritten;
}

size_t DumpOutputTensorsToMemory(Model &model, uint8_t *memAddress, const size_t memSize)
{
    const size_t numOutputs = model.GetNumOutputs();
    size_t numBytesWritten  = 0;
    uint8_t *ptr            = memAddress;

    /* Iterate over all output tensors. */
    for (size_t i = 0; i < numOutputs; ++i)
    {
        const TfLiteTensor *tensor = model.GetOutputTensor(i);
        const auto *tData          = tflite::GetTensorData<uint8_t>(tensor);
#if VERIFY_TEST_OUTPUT
        DumpTensor(tensor);
#endif /* VERIFY_TEST_OUTPUT */
        /* Ensure that we don't overflow the allowed limit. */
        if (numBytesWritten + tensor->bytes <= memSize)
        {
            if (tensor->bytes > 0)
            {
                std::memcpy(ptr, tData, tensor->bytes);

                info("Copied %zu bytes for tensor %zu to 0x%p\n", tensor->bytes, i, ptr);

                numBytesWritten += tensor->bytes;
                ptr += tensor->bytes;
            }
        }
        else
        {
            printf_err("Error writing tensor %zu to memory @ 0x%p\n", i, memAddress);
            break;
        }
    }

    info("%zu bytes written to memory @ 0x%p\n", numBytesWritten, memAddress);

    return numBytesWritten;
}

static void IncrementAppCtxClipIdx(ApplicationContext &ctx)
{
    auto curClipIdx = ctx.Get<uint32_t>("clipIndex");
    if (curClipIdx + 1 >= NUMBER_OF_FILES)
    {
        ctx.Set<uint32_t>("clipIndex", 0);
        return;
    }
    ++curClipIdx;
    ctx.Set<uint32_t>("clipIndex", curClipIdx);
}

} /* namespace app */
} /* namespace arm */

#if defined(MLEVK_UC_LIVE_DEMO)
int rnn_denoise_disable(void)
{
    info("Bypass captured audio to speaker.\n");
    arm::app::bDoDenoise = false;

    return 0;
}

int rnn_denoise_enable(void)
{
    info("De-noise captured audio, then playback to speaker.\n");
    arm::app::bDoDenoise = true;

    return 0;
}
MSH_CMD_EXPORT(rnn_denoise_disable, Disable RNN de - noise);
MSH_CMD_EXPORT(rnn_denoise_enable, Enable RNN de - noise);
#endif
