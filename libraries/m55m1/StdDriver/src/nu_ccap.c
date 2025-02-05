/**************************************************************************//**
 * @file     ccap.c
 * @version  V1.00
 * @brief    CCAP driver source file
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CCAP_Driver CCAP Driver
  @{
*/

/** @addtogroup CCAP_EXPORTED_FUNCTIONS CCAP Exported Functions
  @{
*/

/**
 * @brief      Initialize sensor input settings and output format for CCAP
 *
 * @param[in]  u32Param  The bits corresponding VSP, HSP, PCLK, INFMT, SNRTYPE, OUTFMT and PDORD configurations.
 *             - VSP     Sensor Vsync Polarity. It should be either \ref CCAP_PAR_VSP_LOW or \ref CCAP_PAR_VSP_HIGH
 *             - HSP     Sensor Hsync Polarity. It should be either \ref CCAP_PAR_HSP_LOW or \ref CCAP_PAR_HSP_HIGH
 *             - PCLK    Sensor Pixel Clock Polarity. It should be either \ref CCAP_PAR_PCLKP_LOW or \ref CCAP_PAR_PCLKP_HIGH
 *             - INFMT   Sensor Input Data Format. It should be either \ref CCAP_PAR_INFMT_YUV422 or \ref CCAP_PAR_INFMT_RGB565
 *             - SNRTYPE Sensor Input Type. It should be either \ref CCAP_PAR_SENTYPE_CCIR601 or \ref CCAP_PAR_SENTYPE_CCIR656
 *             - OUTFMT  Image Data Format Output to System Memory. It should be (PACKET_FMT | PLANAR_FMT)
 *                 - PACKET_FMT
 *                       - \ref CCAP_PAR_OUTFMT_YUV422
 *                       - \ref CCAP_PAR_OUTFMT_ONLY_Y
 *                         \ref CCAP_PAR_OUTFMT_RGB555
 *                       - \ref CCAP_PAR_OUTFMT_RGB565
 *                 - PLANAR_FMT
 *                       - \ref CCAP_PAR_OUTFMT_YUV422P
 *                       - \ref CCAP_PAR_OUTFMT_YUV420P
 *             - PDORD   Sensor Input Data Order. It should be one of the following settings
 *                       - \ref CCAP_PAR_INDATORD_YUYV
 *                       - \ref CCAP_PAR_INDATORD_YVYU
 *                       - \ref CCAP_PAR_INDATORD_UYVY
 *                       - \ref CCAP_PAR_INDATORD_VYUY
 *                       - \ref CCAP_PAR_INDATORD_RGGB
 *                       - \ref CCAP_PAR_INDATORD_BGGR
 *                       - \ref CCAP_PAR_INDATORD_GBRG
 *                       - \ref CCAP_PAR_INDATORD_GRBG
 * @param[in]  u32OutFormat Image Data Output Format. It should be combination of following settings
 *                       - \ref CCAP_CTL_PKTEN
 *                       - \ref CCAP_CTL_PLNEN
 *
 * @return     None
 *
 * @details    Initialize the Camera Capture Interface.
 *             Note: Old settings will be overwritten.
 */
void CCAP_Open(CCAP_T *ccap, uint32_t u32Param, uint32_t u32OutFormat)
{
    ccap->PARM = 0x0;
    ccap->PAR  = (ccap->PAR & ~(CCAP_PAR_VSP_Msk | CCAP_PAR_HSP_Msk | CCAP_PAR_PCLKP_Msk |
                                CCAP_PAR_PLNFMT_Msk | CCAP_PAR_OUTFMT_Msk | CCAP_PAR_INDATORD_Msk |
                                CCAP_PAR_SENTYPE_Msk | CCAP_PAR_INFMT_Msk)) | u32Param;
    ccap->CTL  = (ccap->CTL & ~(CCAP_CTL_PKTEN_Msk | CCAP_CTL_PLNEN_Msk)) | u32OutFormat;
}

/**
 * @brief      Initialize CCAP CMOS sensor input settings and output format for packet and planar output
 *
 * @param[in]  u32SensorSetting  Including CMOS sensor VSP, HSP, PCLK and SNRTYPE settings.
 *             - VSP     Sensor Vsync Polarity. It should be either \ref CCAP_PAR_VSP_LOW or \ref CCAP_PAR_VSP_HIGH
 *             - HSP     Sensor Hsync Polarity. It should be either \ref CCAP_PAR_HSP_LOW or \ref CCAP_PAR_HSP_HIGH
 *             - PCLK    Sensor Pixel Clock Polarity. It should be either \ref CCAP_PAR_PCLKP_LOW or \ref CCAP_PAR_PCLKP_HIGH
 *             - SNRTYPE Sensor Input Type. It should be either \ref CCAP_PAR_SENTYPE_CCIR601 or \ref CCAP_PAR_SENTYPE_CCIR656
 * @param[in]  u32SensorColorFmt  Sensor Image Data Input Color Format. It should be (INFMT | INDATORD)
 *             - INFMT      Sensor Input Data Format.
 *             - INDATORD   Sensor Input Data Order.
 *              | INFMT                      | INDATORD                          |
 *              | :--------------------------| :---------------------------------|
 *              |\ref CCAP_PAR_INFMT_YUV422  |\ref CCAP_PAR_INDATORD_YUYV        |
 *              |\ref CCAP_PAR_INFMT_YUV422  |\ref CCAP_PAR_INDATORD_YVYU        |
 *              |\ref CCAP_PAR_INFMT_YUV422  |\ref CCAP_PAR_INDATORD_UYVY        |
 *              |\ref CCAP_PAR_INFMT_YUV422  |\ref CCAP_PAR_INDATORD_VYUY        |
 *              |----------------------------------------------------------------|
 *              |\ref CCAP_PAR_INFMT_RGB565  |\ref CCAP_PAR_INDATORD_RGGB        |
 *              |\ref CCAP_PAR_INFMT_RGB565  |\ref CCAP_PAR_INDATORD_BGGR        |
 *              |\ref CCAP_PAR_INFMT_RGB565  |\ref CCAP_PAR_INDATORD_GBRG        |
 *              |\ref CCAP_PAR_INFMT_RGB565  |\ref CCAP_PAR_INDATORD_GRBG        |
 *              |----------------------------------------------------------------|
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_RGB  |
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_RBG  |
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_GRB  |
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_GBR  |
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_BRG  |
 *              |\ref CCAP_PAR_INFMT_RGB888  |\ref CCAP_PAR_INDATORD_RGB888_BGR  |
 *              |----------------------------------------------------------------|
 * @param[in]  u32OutPacketFmt Image Packet Data Output Format. It should be one of the following settings
 *             - \ref CCAP_PAR_OUTFMT_YUV422
 *             - \ref CCAP_PAR_OUTFMT_ONLY_Y
 *             - \ref CCAP_PAR_OUTFMT_RGB555
 *             - \ref CCAP_PAR_OUTFMT_RGB565
 *             - \ref CCAP_PAR_OUTFMT_RGB888_U8
 *             - \ref CCAP_PAR_OUTFMT_BGR888_U8
 *             - \ref CCAP_PAR_OUTFMT_RGB888_I8
 *             - \ref CCAP_PAR_OUTFMT_BGR888_I8
 *             - \ref CCAP_PAR_OUTFMT_ARGB888_U8
 *             - \ref CCAP_PAR_OUTFMT_BGRA888_U8
 *             - \ref CCAP_PAR_OUTFMT_ARGB888_I8
 *             - \ref CCAP_PAR_OUTFMT_BGRA888_I8
 *             - \ref CCAP_PAR_DISABLE
 * @param[in]  u32OutPlanarFmt Image Planar Data Output Format. It should be one of the following settings
 *             - \ref CCAP_PAR_OUTFMT_YUV422P
 *             - \ref CCAP_PAR_OUTFMT_YUV420P
 *             - \ref CCAP_PAR_DISABLE
 *
 * @return     None
 *
 * @details    Initialize CMOS sensor polarity, type, input formart and data order and
 *             CCAP output data format and order of packet and planar pipes.
 *             Note: Old settings will be overwritten.
 */
void CCAP_OpenPipes(CCAP_T *ccap, uint32_t u32SensorSetting, uint32_t u32SensorDataFmt, uint32_t u32OutPacketFmt, uint32_t u32OutPlanarFmt)
{
    ccap->PAR   = 0x0;
    ccap->PARM  = 0x0;

    ccap->PAR   = u32SensorSetting & (CCAP_PAR_FBB_Msk | CCAP_PAR_VSP_Msk | CCAP_PAR_HSP_Msk | CCAP_PAR_PCLKP_Msk | CCAP_PAR_SENTYPE_Msk);
    ccap->PAR   = ccap->PAR | (u32SensorDataFmt & (CCAP_PAR_INDATORD_Msk | CCAP_PAR_INFMT_Msk));
    ccap->PARM  = (((u32SensorDataFmt & BIT1) ? 1 : 0) << CCAP_PARM_INFMTH_Pos);
    ccap->PARM  = ccap->PARM | (((u32SensorDataFmt & BIT1) ? 1 : 0) << CCAP_PARM_INDATORDH_Pos);

    if (u32OutPacketFmt == CCAP_PKT_DISABLED)
    {
        ccap->CTL &= ~CCAP_CTL_PKTEN_Msk;
    }
    else
    {
        ccap->PAR  = CCAP->PAR  | (u32OutPacketFmt & CCAP_PAR_OUTFMT_Msk);
        ccap->PARM = CCAP->PARM | (u32OutPacketFmt & CCAP_PARM_OUTFMTH_Msk);
        ccap->PARM = CCAP->PARM | (u32OutPacketFmt & CCAP_PARM_RGB888OUTORD_Msk);
        ccap->CTL  |= CCAP_CTL_PKTEN_Msk;
    }

    if (u32OutPlanarFmt == CCAP_PLN_DISABLED)
    {
        ccap->CTL &= ~CCAP_CTL_PLNEN_Msk;
    }
    else
    {
        ccap->PAR = (CCAP->PAR & ~CCAP_PAR_PLNFMT_Msk) | u32OutPlanarFmt;
        ccap->CTL |= CCAP_CTL_PLNEN_Msk;
    }
}


/**
 * @brief      Set Cropping Window Starting Address and Size
 *
 * @param[in]  u32VStart: Cropping Window Vertical Starting Address. It should be 0 ~ 0x7FF.
 * @param[in]  u32HStart: Cropping Window Horizontal Starting Address. It should be 0 ~ 0xFFF.
 * @param[in]  u32Height: Cropping Window Height. It should be 0 ~ 0x7FF.
 * @param[in]  u32Width:  Cropping Window Width. It should be 0 ~ 0xFFF.
 *
 * @return     None
 *
 * @details    This function is used to set cropping window starting address and size.
 */
void CCAP_SetCroppingWindow(CCAP_T *ccap, uint32_t u32VStart, uint32_t u32HStart, uint32_t u32Height, uint32_t u32Width)
{
    ccap->CWSP = (ccap->CWSP & ~(CCAP_CWSP_CWSADDRV_Msk | CCAP_CWSP_CWSADDRH_Msk))
                 | (((u32VStart << 16) | u32HStart));

    ccap->CWS = (ccap->CWS & ~(CCAP_CWS_CWH_Msk | CCAP_CWS_CWW_Msk))
                | ((u32Height << 16) | u32Width);
}

/**
 * @brief      Stop Camera Capture Interface
 *
 * @param[in]  u32FrameComplete:
 *             - \ref TRUE:  Capture module disables the CCAP module automatically after a frame has been captured.
 *             - \ref FALSE: Stop Capture module now.
 *
 * @retval     CCAP_OK          CCAP operation OK.
 * @retval     CCAP_ERR_TIMEOUT CCAP operation abort due to timeout error.
 *
 * @details    If u32FrameComplete is set to TRUE then get a new frame and disable CCAP module.
 */
int32_t CCAP_Stop(CCAP_T *ccap, uint32_t u32FrameComplete)
{
    uint32_t u32TimeOutCnt = SystemCoreClock << 1; /* 2 second */

    if (u32FrameComplete == FALSE)
        ccap->CTL &= ~CCAP_CTL_CCAPEN;
    else
    {
        ccap->CTL |= CCAP_CTL_SHUTTER_Msk;

        while (!CCAP_IS_STOPPED(ccap))
        {
            if (--u32TimeOutCnt == 0) return CCAP_ERR_TIMEOUT;
        }
    }

    return CCAP_OK;
}

/**
 * @brief      Set Packet Scaling Factor
 *
 * @param[in]  u32VNumerator:   Packet Scaling Vertical Factor N. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32VDenominator: Packet Scaling Vertical Factor M. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32HNumerator:   Packet Scaling Horizontal Factor N. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32HDenominator: Packet Scaling Horizontal Factor M. It should be 0x0 ~ 0xFFFF.
 *
 * @return     None
 *
 * @details    This function is used to set Packet Scaling Vertical and Horizontal Factor register.
 */
void CCAP_SetPacketScaling(CCAP_T *ccap, uint32_t u32VNumerator, uint32_t u32VDenominator, uint32_t u32HNumerator, uint32_t u32HDenominator)
{
    uint32_t u32NumeratorL, u32NumeratorH;
    uint32_t u32DenominatorL, u32DenominatorH;

    u32NumeratorL = u32VNumerator & 0xFF;
    u32NumeratorH = u32VNumerator >> 8;
    u32DenominatorL = u32VDenominator & 0xFF;
    u32DenominatorH = u32VDenominator >> 8;
    ccap->PKTSL = (ccap->PKTSL & ~(CCAP_PKTSL_PKTSVNL_Msk | CCAP_PKTSL_PKTSVML_Msk))
                  | ((u32NumeratorL << CCAP_PKTSL_PKTSVNL_Pos) | (u32DenominatorL << CCAP_PKTSL_PKTSVML_Pos));
    ccap->PKTSM = (ccap->PKTSM & ~(CCAP_PKTSM_PKTSVNH_Msk | CCAP_PKTSM_PKTSVMH_Msk))
                  | ((u32NumeratorH << CCAP_PKTSL_PKTSVNL_Pos) | (u32DenominatorH << CCAP_PKTSL_PKTSVML_Pos));

    u32NumeratorL = u32HNumerator & 0xFF;
    u32NumeratorH = u32HNumerator >> 8;
    u32DenominatorL = u32HDenominator & 0xFF;
    u32DenominatorH = u32HDenominator >> 8;
    ccap->PKTSL = (ccap->PKTSL & ~(CCAP_PKTSL_PKTSHNL_Msk | CCAP_PKTSL_PKTSHML_Msk))
                  | ((u32NumeratorL << CCAP_PKTSL_PKTSHNL_Pos) | (u32DenominatorL << CCAP_PKTSL_PKTSHML_Pos));
    ccap->PKTSM = (ccap->PKTSM & ~(CCAP_PKTSM_PKTSHNH_Msk | CCAP_PKTSM_PKTSHMH_Msk))
                  | ((u32NumeratorH << CCAP_PKTSL_PKTSHNL_Pos) | (u32DenominatorH << CCAP_PKTSL_PKTSHML_Pos));
}

/**
 * @brief      Set Planar Scaling Factor
 *
 * @param[in]  u32VNumerator:   Planar Scaling Vertical Factor N. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32VDenominator: Planar Scaling Vertical Factor M. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32HNumerator:   Planar Scaling Horizontal Factor N. It should be 0x0 ~ 0xFFFF.
 * @param[in]  u32HDenominator: Planar Scaling Horizontal Factor M. It should be 0x0 ~ 0xFFFF.
 *
 * @return     None
 *
 * @details    This function is used to set Planar Scaling Vertical and Horizontal Factor register.
 */
void CCAP_SetPlanarScaling(CCAP_T *ccap, uint32_t u32VNumerator, uint32_t u32VDenominator, uint32_t u32HNumerator, uint32_t u32HDenominator)
{
    uint32_t u32NumeratorL, u32NumeratorH;
    uint32_t u32DenominatorL, u32DenominatorH;

    u32NumeratorL = u32VNumerator & 0xFF;
    u32NumeratorH = u32VNumerator >> 8;
    u32DenominatorL = u32VDenominator & 0xFF;
    u32DenominatorH = u32VDenominator >> 8;
    ccap->PLNSL = (ccap->PLNSL & ~(CCAP_PLNSL_PLNSVNL_Msk | CCAP_PLNSL_PLNSVML_Msk))
                  | ((u32NumeratorL << CCAP_PLNSL_PLNSVNL_Pos) | (u32DenominatorL << CCAP_PLNSL_PLNSVML_Pos));
    ccap->PLNSM = (ccap->PLNSM & ~(CCAP_PLNSM_PLNSVNH_Msk | CCAP_PLNSM_PLNSVMH_Msk))
                  | ((u32NumeratorH << CCAP_PLNSL_PLNSVNL_Pos) | (u32DenominatorH << CCAP_PLNSL_PLNSVML_Pos));

    u32NumeratorL = u32HNumerator & 0xFF;
    u32NumeratorH = u32HNumerator >> 8;
    u32DenominatorL = u32HDenominator & 0xFF;
    u32DenominatorH = u32HDenominator >> 8;
    ccap->PLNSL = (ccap->PLNSL & ~(CCAP_PLNSL_PLNSHNL_Msk | CCAP_PLNSL_PLNSHML_Msk))
                  | ((u32NumeratorL << CCAP_PLNSL_PLNSHNL_Pos) | (u32DenominatorL << CCAP_PLNSL_PLNSHML_Pos));
    ccap->PLNSM = (ccap->PLNSM & ~(CCAP_PLNSM_PLNSHNH_Msk | CCAP_PLNSM_PLNSHMH_Msk))
                  | ((u32NumeratorH << CCAP_PLNSL_PLNSHNL_Pos) | (u32DenominatorH << CCAP_PLNSL_PLNSHML_Pos));
}

/**
 * @brief      Set Motion Detection region
 *
 * @param[in]  u32Y             Motion Detection region vertical start address. It could be 0 ~ 239.
 * @param[in]  u32X             Motion Detection region horizontal start address. It could be 0 ~ 319.
 * @param[in]  u32Height        Motion Detection region height. It could be 0 ~ 240.
 * @param[in]  u32Width         Motion Detection region width. It could be 0 ~ 320.
 * @param[in]  u32Sensitivity   Motion detection sensitivity. It could be 0 ~ 100.
 *                              0:   Least sensitive
 *                              100: Most sensitive
 * @return     CCAP_OK                      Success
 *             CCAP_ERR_INVALID_MD_REGION   Invalid motion detection region
 *             CCAP_ERR_INVALID_PARAM       Invalid sensitivity value
 *
 * @details    This function is used to set cropping window starting address and size.
 */
int32_t CCAP_MD_SetRegionSensitivity(CCAP_T *ccap, uint32_t u32Y, uint32_t u32X, uint32_t u32Height, uint32_t u32Width, uint32_t u32Sensitivity)
{
    int32_t  x, y;
    uint32_t u32WinStartX, u32WinStartY, u32WinEndX, u32WinEndY;
    uint32_t u32WinIdx, u32Threshold;

    if ((u32Y + u32Height) > CCAP_MD_HEIGHT || (u32X + u32Width) > CCAP_MD_WIDTH)
        return CCAP_ERR_INVALID_MD_REGION;

    if (u32Height == 0 || u32Width == 0)
        return CCAP_ERR_INVALID_MD_REGION;

    if (u32Sensitivity > 100)
        return CCAP_ERR_INVALID_PARAM;

    u32Threshold = CCAP_MD_MAX_WINDOW_SAD - ((u32Sensitivity * CCAP_MD_MAX_WINDOW_SAD) / 100);

    for (x = 0; x < 4; x++)
    {
        for (y = 0; y < 4; y++)
        {
            u32WinIdx    = x + y * 4;
            u32WinStartX = x * CCAP_MD_CELL_WIDTH;
            u32WinStartY = y * CCAP_MD_CELL_HEIGHT;
            u32WinEndX   = u32WinStartX + CCAP_MD_CELL_WIDTH;
            u32WinEndY   = u32WinStartY + CCAP_MD_CELL_HEIGHT;

            if ((u32X >= u32WinEndX) || (u32Y >= u32WinEndY))
                continue;

            if ((u32X >= u32WinStartX) && (u32X < u32WinEndX))
            {
                if ((u32Y >= u32WinStartY) && (u32Y < u32WinEndY))
                {
                    ccap->MDCTL |= (1 << u32WinIdx);
                    CCAP_MD_SET_WIN_THRESHOLD(ccap, u32WinIdx, u32Threshold);
                    continue;
                }
            }

            if ((u32X <= u32WinStartX) && (u32Y <= u32WinStartY))
            {
                if (((u32X + u32Width) > u32WinStartX) && ((u32Y + u32Height) > u32WinStartY))
                {
                    ccap->MDCTL |= (1 << u32WinIdx);
                    CCAP_MD_SET_WIN_THRESHOLD(ccap, u32WinIdx, u32Threshold);
                }
            }
        }
    }

    return CCAP_OK;
}

/**
 * @brief     Set CCAP motion detection total sensitivity level
 *
 * @param[in]  u32Sensitivity   Motion detection sensitivity. It could be 0 ~ 100.
 *                              0:   Least sensitive
 *                              100: Most sensitive
 * @return    CCAP_OK                   Success
 *            CCAP_ERR_INVALID_PARAM    Invalid parameter
 * @details   Set Camera Capture Interface motion detection total threshold.
 *            If total SAD > total threshold, CCAP_INTSTS_MDINTF_MODE1_Msk is raised.
 */
int32_t CCAP_MD_SetGlobalSensitivity(CCAP_T *ccap, uint32_t u32Sensitivity)
{
    CCAP_MD_SET_TOTAL_THRESHOLD(ccap, CCAP_MD_MAX_TOTAL_SAD - ((u32Sensitivity * CCAP_MD_MAX_TOTAL_SAD) / 100));

    return CCAP_OK;
}

/** @} end of group CCAP_EXPORTED_FUNCTIONS */
/** @} end of group CCAP_Driver */
/** @} end of group Standard_Driver */
