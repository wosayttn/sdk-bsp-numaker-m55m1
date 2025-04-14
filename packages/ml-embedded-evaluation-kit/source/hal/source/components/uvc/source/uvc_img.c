/*
 * SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "uvc_img.h"
#include "log_macros.h"
#include "NuMicro.h"
#include "uvc.h"
#include "string.h"

/*--------------------------------------------------------------------------*/
/* Global variables */
volatile uint32_t gMaxPacketSize;
volatile uint8_t gAppConnected = 0;
volatile UVC_STATUS_T uvcStatus __attribute__((aligned(4)));
/****************************************************/

static volatile uint8_t gAltInterface = 0;
static volatile uint8_t g_u8UVC_PD = 0;
static volatile uint8_t g_u8UVC_FID = 0;
static volatile UVC_INFO_T uvcInfo __attribute__((aligned(4)));
static volatile UVC_PU_INFO_T uvcPuInfo __attribute__((aligned(4))) = {0, 70, 7, -10, 10, 1, 0, 20, 2, -30, 30, 0, 0, 40, 4, 0, 50, 5, 1, 5, 3, 0, 2, 2};
static volatile VIDEOSTREAMCMDDATA  VideoStreamCmdCtlData  __attribute__((aligned(4)));


static void UVC_ClassRequest(void)
{
    uint32_t volatile Data;

    if (gUsbCmd.bmRequestType & 0x80)
    {
        /* Device to host */
        /* Video Control Requests */
        /* Unit and Terminal Control Requests */
        /* Interface Control Requests */
        if (gUsbCmd.wIndex == 0x0000 && (gUsbCmd.wValue == VC_REQUEST_ERROR_CODE_CONTROL))  //Only send to Video Control interface ID(00)
        {
            HSUSBD_PrepareCtrlIn((uint8_t *)&uvcInfo.u8ErrCode, gUsbCmd.wLength);
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
        }
        //Camera Terminal Control Requests
        else if (gUsbCmd.wIndex == 0x0100)  //Camera Terminal ID(01) and Video Control interface ID(00)
        {
            uvcInfo.u8ErrCode = EC_Invalid_Control;
            /* STALL control pipe */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
        }
        ///Videostreaming Requests
        else if (gUsbCmd.wIndex == 0x0001)  //VideoStreaming interface(0x01)
        {
            if ((gUsbCmd.wValue == VS_PROBE_CONTROL) || (gUsbCmd.wValue == VS_COMMIT_CONTROL))
            {
                if ((gUsbCmd.bRequest == UVC_GET_INFO) || (gUsbCmd.bRequest == UVC_GET_LEN))
                {
                    if (gUsbCmd.bRequest == UVC_GET_INFO)
                        Data = 3;//Info
                    else
                        Data = 26;//Length
                    HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
                }
                else
                {
                    if ((uvcInfo.VSCmdCtlData.bFormatIndex != uvcInfo.u32FormatIndx) || (uvcInfo.VSCmdCtlData.bFrameIndex != uvcInfo.u32FrameIndx))
                    {
                        uvcStatus.FormatIndex = uvcInfo.u32FormatIndx = uvcInfo.VSCmdCtlData.bFormatIndex;
                        uvcStatus.FrameIndex = uvcInfo.u32FrameIndx = uvcInfo.VSCmdCtlData.bFrameIndex;

                        uvcInfo.bChangeData = 1;
                    }
                    switch (uvcInfo.VSCmdCtlData.bFrameIndex)
                    {
                    case UVC_QVGA://320*240(3)
                        uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_QVGA;
                        break;
                    case UVC_VGA://640*480(1)
                        uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_VGA;
                        break;
                    }
                    //************Depend on the specificed frame*****************//

                    uvcInfo.VSCmdCtlData.dwMaxPayloadTransferSize = uvcInfo.IsoMaxPacketSize[gAltInterface];

                    uvcStatus.MaxVideoFrameSize = uvcInfo.VSCmdCtlData.dwMaxVideoFrameSize;
                    uvcInfo.VSCmdCtlData.dwMaxPayloadTransferSize = gMaxPacketSize;
                    uvcInfo.VSCmdCtlData.wCompQuality = 0x3D;
                    HSUSBD_PrepareCtrlIn((uint8_t *)&uvcInfo.VSCmdCtlData, gUsbCmd.wLength);
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
                }
            }
            else if ((gUsbCmd.wValue == VS_STILL_PROBE_CONTROL) || (gUsbCmd.wValue == VS_STILL_COMMIT_CONTROL))
            {
                if ((gUsbCmd.bRequest == UVC_GET_INFO) || (gUsbCmd.bRequest == UVC_GET_LEN))
                {
                    if (gUsbCmd.bRequest == UVC_GET_INFO)
                        Data = 3;//Info
                    else
                        Data = 11;//Length

                    HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
                }
                else
                {
                    switch (uvcInfo.VSStillCmdCtlData.bFrameIndex)
                    {
                    case UVC_STILL_QVGA://320*240(5)
                        uvcInfo.VSStillCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_QVGA;
                        break;
                    case UVC_STILL_VGA://640*480(3)
                        uvcInfo.VSStillCmdCtlData.dwMaxVideoFrameSize = UVC_SIZE_VGA;
                        break;
                    }

                    uvcStatus.snapshotFormatIndex = uvcInfo.VSStillCmdCtlData.bFormatIndex;
                    uvcStatus.snapshotFrameIndex = uvcInfo.VSStillCmdCtlData.bFrameIndex;
                    uvcStatus.snapshotMaxVideoFrameSize = uvcInfo.VSStillCmdCtlData.dwMaxPayloadTranferSize = uvcInfo.IsoMaxPacketSize[gAltInterface];
                    HSUSBD_PrepareCtrlIn((uint8_t *)&uvcInfo.VSStillCmdCtlData, gUsbCmd.wLength);
                    HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
                }

            }
            else if (gUsbCmd.wValue == VS_STILL_IMAGE_TRIGGER_CONTROL)
            {
                if (gUsbCmd.bRequest == UVC_GET_INFO)
                    Data = 3;//Info
                else
                    Data = uvcStatus.StillImage;//Trigger
                HSUSBD_PrepareCtrlIn((uint8_t *)&Data, gUsbCmd.wLength);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_INTKIEN_Msk);
            }
        }
        else
        {
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
        }
    }
    else
    {
        /* Host to device */
        /* Video Probe Control Request */
        if (gUsbCmd.wIndex == 0x0100)
        {
            uvcInfo.u8ErrCode = EC_Invalid_Request;
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
        }
        else if (gUsbCmd.wIndex == 0x0001)
        {
            if (gUsbCmd.wValue == VS_STILL_IMAGE_TRIGGER_CONTROL)
                HSUSBD_CtrlOut((uint8_t *)&uvcStatus.StillImage, gUsbCmd.wLength);
            else if ((gUsbCmd.wValue == VS_PROBE_CONTROL) || (gUsbCmd.wValue == VS_COMMIT_CONTROL))
                HSUSBD_CtrlOut((uint8_t *)&uvcInfo.VSCmdCtlData, gUsbCmd.wLength);
            else if ((gUsbCmd.wValue == VS_STILL_PROBE_CONTROL) || (gUsbCmd.wValue == VS_STILL_COMMIT_CONTROL))
                HSUSBD_CtrlOut((uint8_t *)&uvcInfo.VSStillCmdCtlData, gUsbCmd.wLength);
            /* Status stage */
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
        }
        else
        {
            /* Setup error, stall the device */
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALLEN_Msk);
        }
    }
}

static void UVC_InitForHighSpeed(void)
{
    gMaxPacketSize = EPA_MAX_PKT_SIZE;
    /* EPA ==> ISO IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE, EPA_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPA, ISO_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_ISO, HSUSBD_EP_CFG_DIR_IN);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE, EPC_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPC, INT_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
}

static void UVC_InitForFullSpeed(void)
{
    gMaxPacketSize = EPA_OTHER_MAX_PKT_SIZE;
    /* EPA ==> Interrupt IN endpoint, address 1 */
    HSUSBD_SetEpBufAddr(EPA, EPA_BUF_BASE, EPA_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPA, EPA_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPA, ISO_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_ISO, HSUSBD_EP_CFG_DIR_IN);

    /* EPC ==> Interrupt IN endpoint, address 3 */
    HSUSBD_SetEpBufAddr(EPC, EPC_BUF_BASE, EPC_BUF_LEN);
    HSUSBD_SET_MAX_PAYLOAD(EPC, EPC_OTHER_MAX_PKT_SIZE);
    HSUSBD_ConfigEp(EPC, INT_IN_EP_NUM, HSUSBD_EP_CFG_TYPE_INT, HSUSBD_EP_CFG_DIR_IN);
}

static void UVC_SetInterface(uint32_t u32AltInterface)
{
    gAltInterface = gAppConnected = u32AltInterface;
    if (gUsbCmd.wValue == 0)
    {
        /* stop usbd dma and flush FIFO */
        HSUSBD_ResetDMA();
        g_hsusbd_DmaDone = 1;
        HSUSBD->EP[EPA].EPRSPCTL |= HSUSBD_EPRSPCTL_FLUSH_Msk;
    }
}


NVT_ITCM void HSUSBD_IRQHandler(void)
{
    volatile uint32_t IrqStL, IrqSt;

    IrqStL = HSUSBD->GINTSTS & HSUSBD->GINTEN;    /* get interrupt status */

    if (!IrqStL)    return;

    /* USB interrupt */
    if (IrqStL & HSUSBD_GINTSTS_USBIF_Msk)
    {
        IrqSt = HSUSBD->BUSINTSTS & HSUSBD->BUSINTEN;

        if (IrqSt & HSUSBD_BUSINTSTS_SOFIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SOFIF_Msk);

        if (IrqSt & HSUSBD_BUSINTSTS_RSTIF_Msk)
        {
            HSUSBD_SwReset();
            gAppConnected = 0;

            HSUSBD_ResetDMA();
            HSUSBD->EP[EPA].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
            HSUSBD->EP[EPB].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;

            if (HSUSBD->OPER & 0x04)  /* high speed */
                UVC_InitForHighSpeed();
            else                    /* full speed */
                UVC_InitForFullSpeed();
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_SET_ADDR(0);
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RSTIF_Msk);
            HSUSBD_CLR_CEP_INT_FLAG(0x1ffc);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_RESUMEIF_Msk)
        {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RESUMEIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_SUSPENDIF_Msk)
        {
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SUSPENDIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_HISPDIF_Msk)
        {
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_HISPDIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_DMADONEIF_Msk)
        {
            g_hsusbd_DmaDone = 1;
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_DMADONEIF_Msk);

            if (HSUSBD->DMACTL & HSUSBD_DMACTL_DMARD_Msk)
            {
                if (g_hsusbd_ShortPacket == 1)
                {
                    HSUSBD->EP[EPA].EPRSPCTL = (HSUSBD->EP[EPA].EPRSPCTL & 0x10) | HSUSBD_EP_RSPCTL_SHORTTXEN;    // packet end
                    g_hsusbd_ShortPacket = 0;
                }
            }
        }

        if (IrqSt & HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk)
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_PHYCLKVLDIF_Msk);

        if (IrqSt & HSUSBD_BUSINTSTS_VBUSDETIF_Msk)
        {
            if (HSUSBD_IS_ATTACHED())
            {
                /* USB Plug In */
                HSUSBD_ENABLE_USB();
            }
            else
            {
                gAppConnected = 0;
                /* USB Un-plug */
                HSUSBD_DISABLE_USB();
            }
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_VBUSDETIF_Msk);
        }
    }

    if (IrqStL & HSUSBD_GINTSTS_CEPIF_Msk)
    {
        IrqSt = HSUSBD->CEPINTSTS & HSUSBD->CEPINTEN;

        if (IrqSt & HSUSBD_CEPINTSTS_SETUPTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPTKIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_SETUPPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPPKIF_Msk);
            HSUSBD_ProcessSetupPacket();
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_OUTTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_OUTTKIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_INTKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
            if (!(IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk))
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk);
                HSUSBD_CtrlIn();
            }
            else
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_TXPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_PINGIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_PINGIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_TXPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            if (g_hsusbd_CtrlInSize)
            {
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_INTKIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_INTKIEN_Msk);
            }
            else
            {
                if (g_hsusbd_CtrlZero == 1)
                    HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);
                HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
                HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            }
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_RXPKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_RXPKIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_NAKIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_NAKIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_STALLIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STALLIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_ERRIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_ERRIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk)
        {
            HSUSBD_UpdateDeviceState();
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_BUFFULLIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFFULLIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk)
        {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_BUFEMPTYIF_Msk);
            goto exit_HSUSBD_IRQHandler;
        }
    }

    /* interrupt in */
    if (IrqStL & HSUSBD_GINTSTS_EPAIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPA].EPINTSTS & HSUSBD->EP[EPA].EPINTEN;

        HSUSBD_ENABLE_EP_INT(EPA, 0);
        HSUSBD_CLR_EP_INT_FLAG(EPA, IrqSt);
    }
    /* interrupt out */
    if (IrqStL & HSUSBD_GINTSTS_EPBIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPB].EPINTSTS & HSUSBD->EP[EPB].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPB, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPCIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPC].EPINTSTS & HSUSBD->EP[EPC].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPC, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPDIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPD].EPINTSTS & HSUSBD->EP[EPD].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPD, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPEIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPE].EPINTSTS & HSUSBD->EP[EPE].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPE, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPFIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPF].EPINTSTS & HSUSBD->EP[EPF].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPF, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPGIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPG].EPINTSTS & HSUSBD->EP[EPG].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPG, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPHIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPH].EPINTSTS & HSUSBD->EP[EPH].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPH, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPIIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPI].EPINTSTS & HSUSBD->EP[EPI].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPI, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPJIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPJ].EPINTSTS & HSUSBD->EP[EPJ].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPJ, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPKIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPK].EPINTSTS & HSUSBD->EP[EPK].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPK, IrqSt);
    }

    if (IrqStL & HSUSBD_GINTSTS_EPLIF_Msk)
    {
        IrqSt = HSUSBD->EP[EPL].EPINTSTS & HSUSBD->EP[EPL].EPINTEN;
        HSUSBD_CLR_EP_INT_FLAG(EPL, IrqSt);
    }

exit_HSUSBD_IRQHandler:

    __ISB();
    __DSB();
}


static void Enable_USBD_Clock_PHY()
{
    int i;

    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSOTGPHYEN_Msk;
    for (i = 0; i < 0x1000; i++)  // delay
    {
        __NOP();
    }

    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable IP clock */
    CLK_EnableModuleClock(HSUSBD0_MODULE);
}

void UVC_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    Enable_USBD_Clock_PHY();

    /* lock protected registers */
    SYS_LockReg();

    HSUSBD_Open(&gsHSInfo, UVC_ClassRequest, UVC_SetInterface);

    /* Configure USB controller */
    /* Enable USB BUS, CEP and EPA global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk | HSUSBD_GINTEN_EPAIEN_Msk | HSUSBD_GINTEN_EPCIEN_Msk);
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);

    /*****************************************************/
    /* Control endpoint */
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE, CEP_BUF_LEN);
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);

    /*****************************************************/
    UVC_InitForHighSpeed();

    uvcInfo.u8ErrCode = 0;
    uvcInfo.bChangeData = 0;
    uvcStatus.StillImage = 0;

    uvcInfo.IsoMaxPacketSize[0] = gMaxPacketSize;
    uvcInfo.IsoMaxPacketSize[1] = gMaxPacketSize;
    uvcInfo.IsoMaxPacketSize[2] = gMaxPacketSize;

    //Initialize Video Capture Filter Control Value
    uvcInfo.CapFilter.Brightness = 1;
    uvcInfo.CapFilter.POWER_LINE_FREQUENCY = 2;
    uvcInfo.CapFilter.Brightness = 1;
    uvcInfo.CapFilter.Contrast = 2;
    uvcInfo.CapFilter.Hue = 3;
    uvcInfo.CapFilter.Saturation = 4;
    uvcInfo.CapFilter.Sharpness = 5;
    uvcInfo.CapFilter.Gamma = 3;
    uvcInfo.CapFilter.Backlight = 7;

    /* Initialize Video Probe and Commit Control data */
    memset((uint8_t *)&uvcInfo.VSCmdCtlData, 0x0, sizeof(VIDEOSTREAMCMDDATA));
    uvcInfo.VSCmdCtlData.bmHint = 0x0100;
    uvcInfo.VSCmdCtlData.bFormatIndex = 1;
    uvcInfo.VSCmdCtlData.bFrameIndex = 1;
    uvcStatus.FormatIndex = 1;
    uvcStatus.FrameIndex = 1;
    uvcInfo.VSCmdCtlData.dwFrameInterval = 0x028b0a; //0x051615;

    /* Initialize Still Image Command data */
    memset((uint8_t *)&uvcInfo.VSStillCmdCtlData, 0x0, sizeof(VIDEOSTREAMSTILLCMDDATA));

    uvcInfo.VSStillCmdCtlData.bFormatIndex = 1;
    uvcInfo.VSStillCmdCtlData.bFrameIndex = 1;
    uvcStatus.snapshotFormatIndex = 1;
    uvcStatus.snapshotFrameIndex = 1;

    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(HSUSBD_IRQn);
}

static void UVC_ActiveDMA(uint32_t u32Addr, uint32_t u32Len)
{
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk);

    HSUSBD_SET_DMA_ADDR(u32Addr);
    HSUSBD_SET_DMA_LEN(u32Len);
    g_hsusbd_DmaDone = 0;
    HSUSBD_ENABLE_DMA();
    while (g_hsusbd_Configured)
    {
        if (g_hsusbd_DmaDone)
            break;

        if ((HSUSBD_IS_ATTACHED() == 0) || (gAppConnected == 0))
        {
            if (HSUSBD->DMACTL & 0x20)
            {
                HSUSBD_ResetDMA();
                HSUSBD->EP[EPA].EPRSPCTL |= HSUSBD_EPRSPCTL_FLUSH_Msk;
            }
            break;
        }
    }
}

typedef struct  __attribute__((__packed__))
{
    uint8_t bHeaderLength;
    union
    {
        struct
        {
            uint8_t u1FID: 1;
            uint8_t u1EOF: 1;
            uint8_t u1PTS: 1;
            uint8_t u1SCR: 1;
            uint8_t u1RES: 1;
            uint8_t u1STI: 1;
            uint8_t u1ERR: 1;
            uint8_t u1EOH: 1;
        };
        uint8_t bmHeaderInfo;
    };
    uint32_t dwPresentationTime;
    uint8_t  scrSourceClock[6];
}
S_UVC_VID_DAT_FMT;

static void HSUSB_EPA_MEMCPY(uint8_t *ps8Addr, uint32_t u32Len)
{
    int i = 0;

    for (i = 0; i < u32Len; i++)
    {
        HSUSBD->EP[EPA].EPDAT_BYTE = ps8Addr[i];
    }
}

static void UVC_IsoIn(uint32_t u32Addr, uint32_t u32Len)
{
    S_UVC_VID_DAT_FMT sPktHdr = {0};

    if (uvcInfo.bChangeData == 1)
    {
        uvcInfo.bChangeData = 0;
        if (HSUSBD->DMACNT & 0x20)
        {
            HSUSBD_ResetDMA();
            uvcStatus.bReady = TRUE;
        }
    }

    /* iso in, dma read, epnum = 1 */
    HSUSBD_SET_DMA_READ(ISO_IN_EP_NUM);
    while (u32Len > 0)
    {
        uint32_t XferLen = (u32Len > USBD_MAX_DMA_LEN) ? USBD_MAX_DMA_LEN : u32Len;

        HSUSBD_ENABLE_EP_INT(EPA, HSUSBD_EPINTEN_TXPKIEN_Msk | HSUSBD_EPINTEN_SHORTTXIEN_Msk);
        if (XferLen == USBD_MAX_DMA_LEN)
        {
            g_hsusbd_ShortPacket = 0;
            /* Start of Transfer Payload Data */
            sPktHdr.bHeaderLength = sizeof(S_UVC_VID_DAT_FMT);
            sPktHdr.u1FID = (g_u8UVC_FID & 0x01);
            sPktHdr.u1STI = (g_u8UVC_PD > 0) ? 1 : 0;
            sPktHdr.u1EOH = 1;
        }
        else
        {
            g_hsusbd_ShortPacket = 1;
            g_u8UVC_FID++;
            sPktHdr.u1EOF = 1;
        }

        while (1)
        {
            if (HSUSBD_GET_EP_INT_FLAG(EPA) & HSUSBD_EPINTSTS_BUFEMPTYIF_Msk)
            {
                HSUSB_EPA_MEMCPY((uint8_t *)&sPktHdr, sizeof(S_UVC_VID_DAT_FMT));
                UVC_ActiveDMA(u32Addr, XferLen);
                u32Addr += XferLen;
                break;
            }
        }

        u32Len -= XferLen;
    }
}

int uvc_init(void)
{
    int i;

    uint32_t u32RegLockLevel = SYS_IsRegLocked();

    /* Unlock protected registers */
    if (u32RegLockLevel)
        SYS_UnlockReg();

    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSOTGPHYEN_Msk;
    for (i = 0; i < 0x1000; i++)  // delay
    {
        __NOP();
    }
    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;

    /* Enable IP clock */
    CLK_EnableModuleClock(HSUSBD0_MODULE);

    /* Lock protected registers */
    if (u32RegLockLevel)
        SYS_LockReg();

    HSUSBD_Open(&gsHSInfo, UVC_ClassRequest, UVC_SetInterface);

    /* Configure USB controller */
    /* Enable USB BUS, CEP and EPA global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk | HSUSBD_GINTEN_EPAIEN_Msk | HSUSBD_GINTEN_EPCIEN_Msk);
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_DMADONEIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);

    /*****************************************************/
    /* Control endpoint */
    HSUSBD_SetEpBufAddr(CEP, CEP_BUF_BASE, CEP_BUF_LEN);
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk);

    /*****************************************************/
    UVC_InitForHighSpeed();

    uvcInfo.u8ErrCode = 0;
    uvcInfo.bChangeData = 0;
    uvcStatus.StillImage = 0;

    uvcInfo.IsoMaxPacketSize[0] = gMaxPacketSize;
    uvcInfo.IsoMaxPacketSize[1] = gMaxPacketSize;
    uvcInfo.IsoMaxPacketSize[2] = gMaxPacketSize;

    //Initialize Video Capture Filter Control Value
    uvcInfo.CapFilter.Brightness = 1;
    uvcInfo.CapFilter.POWER_LINE_FREQUENCY = 2;
    uvcInfo.CapFilter.Brightness = 1;
    uvcInfo.CapFilter.Contrast = 2;
    uvcInfo.CapFilter.Hue = 3;
    uvcInfo.CapFilter.Saturation = 4;
    uvcInfo.CapFilter.Sharpness = 5;
    uvcInfo.CapFilter.Gamma = 3;
    uvcInfo.CapFilter.Backlight = 7;

    /* Initialize Video Probe and Commit Control data */
    memset((uint8_t *)&uvcInfo.VSCmdCtlData, 0x0, sizeof(VIDEOSTREAMCMDDATA));
    uvcInfo.VSCmdCtlData.bmHint = 0x0100;
    uvcInfo.VSCmdCtlData.bFormatIndex = 2;
    uvcInfo.VSCmdCtlData.bFrameIndex = 1;
    uvcStatus.FormatIndex = 2;
    uvcStatus.FrameIndex = 1;
    uvcInfo.VSCmdCtlData.dwFrameInterval = 0x051615;

    /* Initialize Still Image Command data */
    memset((uint8_t *)&uvcInfo.VSStillCmdCtlData, 0x0, sizeof(VIDEOSTREAMSTILLCMDDATA));

    uvcInfo.VSStillCmdCtlData.bFormatIndex = 2;
    uvcInfo.VSStillCmdCtlData.bFrameIndex = 1;
    uvcStatus.snapshotFormatIndex = 2;
    uvcStatus.snapshotFrameIndex = 1;

    /* Enable HSUSBD interrupt */
    NVIC_EnableIRQ(HSUSBD_IRQn);

    HSUSBD_Start();

    return 0;
}

void uvc_fini(void)
{
    int i;

    uint32_t u32RegLockLevel = SYS_IsRegLocked();

    /* Disable HSUSBD interrupt */
    NVIC_DisableIRQ(HSUSBD_IRQn);

    /* Finiliez USB engine */
    HSUSBD_DISABLE_PHY();

    /* Enable IP clock */
    CLK_DisableModuleClock(HSUSBD0_MODULE);

    /* Unlock protected registers */
    if (u32RegLockLevel)
        SYS_UnlockReg();

    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBACT_Msk;

    /* Lock protected registers */
    if (u32RegLockLevel)
        SYS_LockReg();
}

int uvc_is_connected(void)
{
    return ((gAppConnected != 0) && HSUSBD_IS_ATTACHED());
}

int uvc_send_image(uint32_t u32Addr, uint32_t u32XferSize)
{
    if ((u32XferSize != 0) && uvc_is_connected())
    {
        uvcStatus.bReady = 0;
        HSUSBD_ResetDMA();
        g_u8UVC_PD = UVC_PH_Preview;

        SCB_CleanDCache_by_Addr((void *)u32Addr, u32XferSize);
        UVC_IsoIn(u32Addr, u32XferSize);

        return 1;
    }

    return 0;
}

