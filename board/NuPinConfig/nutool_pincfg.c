/****************************************************************************
 * @file     nutool_pincfg.c
 * @version  V1.28.0009
 * @Date     Tue Jan 16 2024 17:34:02 GMT+0800 (Taipei Standard Time)
 * @brief    NuMicro generated code file
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2013-2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/********************
MCU:M55M1H2ES(LQFP176)
********************/

#include "NuMicro.h"
#include "rtconfig.h"

void nutool_pincfg_init_canfd0(void)
{
    SET_CANFD0_RXD_PJ11();
    SET_CANFD0_TXD_PJ10();

    return;
}

void nutool_pincfg_deinit_canfd0(void)
{
    SET_GPIO_PJ11();
    SET_GPIO_PJ10();

    return;
}

void nutool_pincfg_init_ccap(void)
{
    SET_CCAP_DATA0_PF7();
    SET_CCAP_DATA1_PF8();
    SET_CCAP_DATA2_PF9();
    SET_CCAP_DATA3_PF10();
    SET_CCAP_DATA4_PF11();
    SET_CCAP_DATA5_PG4();
    SET_CCAP_DATA6_PG3();
    SET_CCAP_DATA7_PG2();
    SET_CCAP_SCLK_PG10();
    SET_CCAP_PIXCLK_PG9();
    SET_CCAP_HSYNC_PD7();
    SET_CCAP_VSYNC_PG12();

    return;
}

void nutool_pincfg_deinit_ccap(void)
{
    SET_GPIO_PF7();
    SET_GPIO_PF11();
    SET_GPIO_PF10();
    SET_GPIO_PF9();
    SET_GPIO_PF8();
    SET_GPIO_PG3();
    SET_GPIO_PG2();
    SET_GPIO_PG4();
    SET_GPIO_PG10();
    SET_GPIO_PG9();
    SET_GPIO_PD7();
    SET_GPIO_PG12();

    return;
}

void nutool_pincfg_init_dmic0(void)
{
    SET_DMIC0_CLKLP_PB6();
    SET_DMIC0_DAT_PB5();
    SET_DMIC0_CLK_PB4();

    return;
}

void nutool_pincfg_deinit_dmic0(void)
{
    SET_GPIO_PB6();
    SET_GPIO_PB5();
    SET_GPIO_PB4();

    return;
}

void nutool_pincfg_init_eadc0(void)
{
    SET_EADC0_CH7_PB7();
    SET_EADC0_CH17_PD11();
    SET_EADC0_CH16_PD10();

    return;
}

void nutool_pincfg_deinit_eadc0(void)
{
    SET_GPIO_PB7();
    SET_GPIO_PD11();
    SET_GPIO_PD10();

    return;
}

void nutool_pincfg_init_ebi(void)
{
    SET_EBI_AD0_PA5();
    SET_EBI_AD1_PA4();
    SET_EBI_AD2_PC2();
    SET_EBI_AD3_PC3();
    SET_EBI_AD4_PC4();
    SET_EBI_AD5_PC5();
    SET_EBI_AD6_PD8();
    SET_EBI_AD7_PD9();
    SET_EBI_AD8_PE14();
    SET_EBI_AD9_PE15();
    SET_EBI_AD10_PE1();
    SET_EBI_AD11_PE0();
    SET_EBI_AD12_PH8();
    SET_EBI_AD13_PH9();
    SET_EBI_AD14_PH10();
    SET_EBI_AD15_PH11();
    SET_EBI_nWR_PJ9();
    SET_EBI_nRD_PJ8();
    SET_EBI_nCS0_PD14();
    SET_EBI_ADR0_PH7();

    GPIO_SetSlewCtl(PA, (BIT4 | BIT5), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PC, (BIT2 | BIT3 | BIT4 | BIT5), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PD, (BIT8 | BIT9), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PE, (BIT14 | BIT15), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PE, (BIT0 | BIT1), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PH, (BIT7 | BIT8 | BIT9 | BIT10 | BIT11), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PJ, (BIT8 | BIT9), GPIO_SLEWCTL_HIGH);
    GPIO_SetSlewCtl(PD, BIT14, GPIO_SLEWCTL_HIGH);

    return;
}

void nutool_pincfg_deinit_ebi(void)
{
    SET_GPIO_PA4();
    SET_GPIO_PA5();
    SET_GPIO_PC1();
    SET_GPIO_PC0();
    SET_GPIO_PC5();
    SET_GPIO_PC4();
    SET_GPIO_PD9();
    SET_GPIO_PD8();
    SET_GPIO_PD14();
    SET_GPIO_PE1();
    SET_GPIO_PE0();
    SET_GPIO_PE15();
    SET_GPIO_PE14();
    SET_GPIO_PH11();
    SET_GPIO_PH10();
    SET_GPIO_PH9();
    SET_GPIO_PH8();
    SET_GPIO_PH7();
    SET_GPIO_PJ9();
    SET_GPIO_PJ8();

    return;
}

void nutool_pincfg_init_emac0(void)
{
    SET_EMAC0_RMII_CRSDV_PA7();
    SET_EMAC0_RMII_RXERR_PA6();
    SET_EMAC0_RMII_RXD0_PC7();
    SET_EMAC0_RMII_RXD1_PC6();
    SET_EMAC0_RMII_REFCLK_PC8();
    SET_EMAC0_RMII_TXD1_PE11();
    SET_EMAC0_RMII_TXD0_PE10();
    SET_EMAC0_RMII_MDIO_PE9();
    SET_EMAC0_RMII_MDC_PE8();
    SET_EMAC0_RMII_TXEN_PE12();

    /* Set TX pins Slew Rate to FAST0. */
    GPIO_SetSlewCtl(PE, (BIT10 | BIT11 | BIT12), GPIO_SLEWCTL_FAST0);

    /* Set Phy Reset Pin to High. */
    GPIO_SetMode(PE, BIT13, GPIO_MODE_OUTPUT);
    PE13 = 1;

    return;
}

void nutool_pincfg_deinit_emac0(void)
{
    SET_GPIO_PA7();
    SET_GPIO_PA6();
    SET_GPIO_PC7();
    SET_GPIO_PC6();
    SET_GPIO_PC8();
    SET_GPIO_PE11();
    SET_GPIO_PE10();
    SET_GPIO_PE9();
    SET_GPIO_PE8();
    SET_GPIO_PE12();

    return;
}

void nutool_pincfg_init_epwm1(void)
{
    SET_EPWM1_CH2_PC10();
    SET_EPWM1_CH3_PC9();

    return;
}

void nutool_pincfg_deinit_epwm1(void)
{
    SET_GPIO_PC10();
    SET_GPIO_PC9();

    return;
}

void nutool_pincfg_init_hsusb(void)
{
    SET_HSUSB_VBUS_EN_PJ13();
    SET_HSUSB_VBUS_ST_PJ12();

    return;
}

void nutool_pincfg_deinit_hsusb(void)
{
    SET_GPIO_PJ13();
    SET_GPIO_PJ12();

    return;
}

void nutool_pincfg_init_i2c0(void)
{
    SET_I2C0_SDA_PH3();
    SET_I2C0_SCL_PH2();

    return;
}

void nutool_pincfg_deinit_i2c0(void)
{
    SET_GPIO_PH3();
    SET_GPIO_PH2();

    return;
}

void nutool_pincfg_init_i2c1(void)
{
    SET_I2C1_SDA_PB10();
    SET_I2C1_SCL_PB11();

    return;
}

void nutool_pincfg_deinit_i2c1(void)
{
    SET_GPIO_PB10();
    SET_GPIO_PB11();

    return;
}

void nutool_pincfg_init_i2c3(void)
{
    SET_I2C3_SDA_PG1();
    SET_I2C3_SCL_PG0();

    return;
}

void nutool_pincfg_deinit_i2c3(void)
{
    SET_GPIO_PG1();
    SET_GPIO_PG0();

    return;
}

void nutool_pincfg_init_i2s0(void)
{
    SET_I2S0_MCLK_PI7();
    SET_I2S0_BCLK_PI6();
    SET_I2S0_LRCK_PI10();
    SET_I2S0_DO_PI9();
    SET_I2S0_DI_PI8();

    return;
}

void nutool_pincfg_deinit_i2s0(void)
{
    SET_GPIO_PI7();
    SET_GPIO_PI6();
    SET_GPIO_PI10();
    SET_GPIO_PI9();
    SET_GPIO_PI8();

    return;
}

void nutool_pincfg_init_i3c0(void)
{
    SET_I3C0_SCL_PB1();
    SET_I3C0_SDA_PB0();

    return;
}

void nutool_pincfg_deinit_i3c0(void)
{
    SET_GPIO_PB1();
    SET_GPIO_PB0();

    return;
}

void nutool_pincfg_init_ice(void)
{
    SET_ICE_CLK_PF1();
    SET_ICE_DAT_PF0();

    return;
}

void nutool_pincfg_deinit_ice(void)
{
    SET_GPIO_PF1();
    SET_GPIO_PF0();

    return;
}

void nutool_pincfg_init_lpi2c0(void)
{
    SET_LPI2C0_SDA_PC11();
    SET_LPI2C0_SCL_PC12();

    return;
}

void nutool_pincfg_deinit_lpi2c0(void)
{
    SET_GPIO_PC11();
    SET_GPIO_PC12();

    return;
}

void nutool_pincfg_init_pb(void)
{
    SET_GPIO_PB2();
    SET_GPIO_PB12();

    return;
}

void nutool_pincfg_deinit_pb(void)
{
    SET_GPIO_PB2();
    SET_GPIO_PB12();

    return;
}

void nutool_pincfg_init_pc(void)
{
    SET_GPIO_PC13();

    return;
}

void nutool_pincfg_deinit_pc(void)
{
    SET_GPIO_PC13();

    return;
}

void nutool_pincfg_init_pd(void)
{
    SET_GPIO_PD0();
    SET_GPIO_PD5();
    SET_GPIO_PD6();
    SET_GPIO_PD12();

    return;
}

void nutool_pincfg_deinit_pd(void)
{
    SET_GPIO_PD0();
    SET_GPIO_PD5();
    SET_GPIO_PD6();
    SET_GPIO_PD12();

    return;
}

void nutool_pincfg_init_pe(void)
{
    SET_GPIO_PE13();

    return;
}

void nutool_pincfg_deinit_pe(void)
{
    SET_GPIO_PE13();

    return;
}

void nutool_pincfg_init_pf(void)
{
    SET_GPIO_PF6();

    return;
}

void nutool_pincfg_deinit_pf(void)
{
    SET_GPIO_PF6();

    return;
}

void nutool_pincfg_init_pg(void)
{
    SET_GPIO_PG6();
    SET_GPIO_PG5();
    SET_GPIO_PG15();
    SET_GPIO_PG14();

    return;
}

void nutool_pincfg_deinit_pg(void)
{
    SET_GPIO_PG6();
    SET_GPIO_PG5();
    SET_GPIO_PG15();
    SET_GPIO_PG14();

    return;
}

void nutool_pincfg_init_ph(void)
{
    SET_GPIO_PH1();
    SET_GPIO_PH0();
    SET_GPIO_PH6();
    SET_GPIO_PH4();

    return;
}

void nutool_pincfg_deinit_ph(void)
{
    SET_GPIO_PH1();
    SET_GPIO_PH0();
    SET_GPIO_PH6();
    SET_GPIO_PH4();

    return;
}

void nutool_pincfg_init_pi(void)
{
    SET_GPIO_PI11();
    SET_GPIO_PI13();

    return;
}

void nutool_pincfg_deinit_pi(void)
{
    SET_GPIO_PI11();
    SET_GPIO_PI13();

    return;
}

void nutool_pincfg_init_qspi0(void)
{
    SET_QSPI0_SS_PA3();
    SET_QSPI0_CLK_PA2();
    SET_QSPI0_MISO0_PA1();
    SET_QSPI0_MOSI0_PA0();

    return;
}

void nutool_pincfg_deinit_qspi0(void)
{
    SET_GPIO_PA3();
    SET_GPIO_PA2();
    SET_GPIO_PA1();
    SET_GPIO_PA0();

    return;
}

void nutool_pincfg_init_sd0(void)
{
    SET_SD0_nCD_PD13();
    SET_SD0_DAT1_PE3();
    SET_SD0_DAT0_PE2();
    SET_SD0_CMD_PE7();
    SET_SD0_CLK_PE6();
    SET_SD0_DAT3_PE5();
    SET_SD0_DAT2_PE4();

    return;
}

void nutool_pincfg_deinit_sd0(void)
{
    SET_GPIO_PD13();
    SET_GPIO_PE3();
    SET_GPIO_PE2();
    SET_GPIO_PE7();
    SET_GPIO_PE6();
    SET_GPIO_PE5();
    SET_GPIO_PE4();

    return;
}

void nutool_pincfg_init_spi2(void)
{
    SET_SPI2_SS_PA11();
    SET_SPI2_CLK_PA10();
    SET_SPI2_MISO_PA9();
    SET_SPI2_MOSI_PA8();

    return;
}

void nutool_pincfg_deinit_spi2(void)
{
    SET_GPIO_PA11();
    SET_GPIO_PA10();
    SET_GPIO_PA9();
    SET_GPIO_PA8();

    return;
}

void nutool_pincfg_init_spim0(void)
{
    SET_SPIM0_CLKN_PH12();
    SET_SPIM0_CLK_PH13();
    SET_SPIM0_D2_PJ5();
    SET_SPIM0_D3_PJ6();
    SET_SPIM0_D4_PH14();
    SET_SPIM0_D5_PH15();
    SET_SPIM0_D6_PG13();
    SET_SPIM0_D7_PG14();
    SET_SPIM0_MISO_PJ4();
    SET_SPIM0_MOSI_PJ3();
    SET_SPIM0_RESETN_PJ2();
    SET_SPIM0_RWDS_PG15();
    SET_SPIM0_SS_PJ7();

    PG->SMTEN |= (GPIO_SMTEN_SMTEN13_Msk |
                  GPIO_SMTEN_SMTEN14_Msk |
                  GPIO_SMTEN_SMTEN15_Msk);
    PH->SMTEN |= (GPIO_SMTEN_SMTEN12_Msk |
                  GPIO_SMTEN_SMTEN13_Msk |
                  GPIO_SMTEN_SMTEN14_Msk |
                  GPIO_SMTEN_SMTEN15_Msk);
    PJ->SMTEN |= (GPIO_SMTEN_SMTEN2_Msk |
                  GPIO_SMTEN_SMTEN3_Msk |
                  GPIO_SMTEN_SMTEN4_Msk |
                  GPIO_SMTEN_SMTEN5_Msk |
                  GPIO_SMTEN_SMTEN6_Msk |
                  GPIO_SMTEN_SMTEN7_Msk);

    /* Set SPIM I/O pins as FAST1 slew rate. */
    GPIO_SetSlewCtl(PG, (BIT13 | BIT14 | BIT15), GPIO_SLEWCTL_FAST0);
    GPIO_SetSlewCtl(PH, (BIT12 | BIT13 | BIT14 | BIT15), GPIO_SLEWCTL_FAST0);
    GPIO_SetSlewCtl(PJ, (BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7), GPIO_SLEWCTL_FAST0);

    return;
}

void nutool_pincfg_deinit_spim0(void)
{
    SET_GPIO_PH12();
    SET_GPIO_PH13();
    SET_GPIO_PJ5();
    SET_GPIO_PJ6();
    SET_GPIO_PH14();
    SET_GPIO_PH15();
    SET_GPIO_PG13();
    SET_GPIO_PG14();
    SET_GPIO_PJ4();
    SET_GPIO_PJ3();
    SET_GPIO_PJ2();
    SET_GPIO_PG15();
    SET_GPIO_PJ7();

    return;
}

void nutool_pincfg_init_uart0(void)
{
    SET_UART0_RXD_PB12();
    SET_UART0_TXD_PB13();

    return;
}

void nutool_pincfg_deinit_uart0(void)
{
    SET_GPIO_PB12();
    SET_GPIO_PB13();

    return;
}

void nutool_pincfg_init_uart1(void)
{
    SET_UART1_TXD_PB3();

    return;
}

void nutool_pincfg_deinit_uart1(void)
{
    SET_GPIO_PB3();

    return;
}

void nutool_pincfg_init_uart8(void)
{
    SET_UART8_nRTS_PI15();
    SET_UART8_nCTS_PI14();
    SET_UART8_RXD_PJ1();
    SET_UART8_TXD_PJ0();

    return;
}

void nutool_pincfg_deinit_uart8(void)
{
    SET_GPIO_PI15();
    SET_GPIO_PI14();
    SET_GPIO_PJ1();
    SET_GPIO_PJ0();

    return;
}

void nutool_pincfg_init_usb(void)
{
    SET_USB_VBUS_PA12();
    SET_USB_D_MINUS_PA13();
    SET_USB_D_PLUS_PA14();
    SET_USB_OTG_ID_PA15();

    SET_USB_VBUS_ST_PB14();
    SET_USB_VBUS_EN_PB15();

    return;
}

void nutool_pincfg_deinit_usb(void)
{
    SET_GPIO_PA15();
    SET_GPIO_PA12();
    SET_GPIO_PB14();
    SET_GPIO_PB15();

    return;
}

void nutool_pincfg_init_x32(void)
{
    SET_X32_IN_PF5();
    SET_X32_OUT_PF4();

    return;
}

void nutool_pincfg_deinit_x32(void)
{
    SET_GPIO_PF5();
    SET_GPIO_PF4();

    return;
}

void nutool_pincfg_init_xt1(void)
{
    SET_XT1_IN_PF3();
    SET_XT1_OUT_PF2();

    return;
}

void nutool_pincfg_deinit_xt1(void)
{
    SET_GPIO_PF3();
    SET_GPIO_PF2();

    return;
}

void nutool_pincfg_init(void)
{
    //SYS->GPA_MFP0 = 0x03030303UL;
    //SYS->GPA_MFP1 = 0x03030000UL;
    //SYS->GPA_MFP2 = 0x04040404UL;
    //SYS->GPA_MFP3 = 0x0E0E0E0EUL;
    //SYS->GPB_MFP0 = 0x06001010UL;
    //SYS->GPB_MFP1 = 0x01011010UL;
    //SYS->GPB_MFP2 = 0x01010F0FUL;
    //SYS->GPB_MFP3 = 0x00000000UL;
    //SYS->GPC_MFP0 = 0x02020202UL;
    //SYS->GPC_MFP1 = 0x03030202UL;
    //SYS->GPC_MFP2 = 0x160C0C03UL;
    //SYS->GPC_MFP3 = 0x00000016UL;
    //SYS->GPD_MFP0 = 0x00000000UL;
    //SYS->GPD_MFP1 = 0x10101000UL;
    //SYS->GPD_MFP2 = 0x01010202UL;
    //SYS->GPD_MFP3 = 0x00020300UL;
    //SYS->GPE_MFP0 = 0x03030202UL;
    //SYS->GPE_MFP1 = 0x03030303UL;
    //SYS->GPE_MFP2 = 0x03030303UL;
    //SYS->GPE_MFP3 = 0x02020003UL;
    //SYS->GPF_MFP0 = 0x0A0A0E0EUL;
    //SYS->GPF_MFP1 = 0x07000A0AUL;
    //SYS->GPF_MFP2 = 0x07070707UL;
    //SYS->GPG_MFP0 = 0x07070909UL;
    //SYS->GPG_MFP1 = 0x00000007UL;
    //SYS->GPG_MFP2 = 0x07070700UL;
    //SYS->GPG_MFP3 = 0x00000707UL;
    //SYS->GPH_MFP0 = 0x06060000UL;
    //SYS->GPH_MFP1 = 0x02000505UL;
    //SYS->GPH_MFP2 = 0x02020202UL;
    //SYS->GPH_MFP3 = 0x10101010UL;
    //SYS->GPI_MFP1 = 0x06060000UL;
    //SYS->GPI_MFP2 = 0x00060606UL;
    //SYS->GPI_MFP3 = 0x07070000UL;
    //SYS->GPJ_MFP0 = 0x10100707UL;
    //SYS->GPJ_MFP1 = 0x10101010UL;
    //SYS->GPJ_MFP2 = 0x0B0B0202UL;
    //SYS->GPJ_MFP3 = 0x00000F0FUL;

    nutool_pincfg_init_canfd0();
    nutool_pincfg_init_ccap();
    nutool_pincfg_init_dmic0();
    nutool_pincfg_init_eadc0();
    nutool_pincfg_init_ebi();
    nutool_pincfg_init_emac0();
    nutool_pincfg_init_epwm1();
    nutool_pincfg_init_hsusb();
    nutool_pincfg_init_i2c0();
    nutool_pincfg_init_i2c1();
    nutool_pincfg_init_i2c3();
    nutool_pincfg_init_i2s0();
    nutool_pincfg_init_i3c0();
    nutool_pincfg_init_ice();
    nutool_pincfg_init_lpi2c0();
    nutool_pincfg_init_pb();
    nutool_pincfg_init_pc();
    nutool_pincfg_init_pd();
    nutool_pincfg_init_pe();
    nutool_pincfg_init_pf();
    nutool_pincfg_init_pg();
    nutool_pincfg_init_ph();
    nutool_pincfg_init_pi();
    nutool_pincfg_init_qspi0();
    nutool_pincfg_init_sd0();
    nutool_pincfg_init_spi2();
    nutool_pincfg_init_spim0();
    nutool_pincfg_init_uart0();
    nutool_pincfg_init_uart1();
    nutool_pincfg_init_uart8();
    nutool_pincfg_init_usb();
    nutool_pincfg_init_x32();
    nutool_pincfg_init_xt1();

    /* Vref connect to extern pin */
    SYS->VREFCTL = (SYS->VREFCTL & ~SYS_VREFCTL_VREFCTL_Msk) | SYS_VREFCTL_VREF_PIN;

    return;
}

void nutool_pincfg_deinit(void)
{
    nutool_pincfg_deinit_canfd0();
    nutool_pincfg_deinit_ccap();
    nutool_pincfg_deinit_dmic0();
    nutool_pincfg_deinit_eadc0();
    nutool_pincfg_deinit_ebi();
    nutool_pincfg_deinit_emac0();
    nutool_pincfg_deinit_epwm1();
    nutool_pincfg_deinit_hsusb();
    nutool_pincfg_deinit_i2c0();
    nutool_pincfg_deinit_i2c1();
    nutool_pincfg_deinit_i2c3();
    nutool_pincfg_deinit_i2s0();
    nutool_pincfg_deinit_i3c0();
    nutool_pincfg_deinit_ice();
    nutool_pincfg_deinit_lpi2c0();
    nutool_pincfg_deinit_pb();
    nutool_pincfg_deinit_pc();
    nutool_pincfg_deinit_pd();
    nutool_pincfg_deinit_pe();
    nutool_pincfg_deinit_pf();
    nutool_pincfg_deinit_pg();
    nutool_pincfg_deinit_ph();
    nutool_pincfg_deinit_pi();
    nutool_pincfg_deinit_qspi0();
    nutool_pincfg_deinit_sd0();
    nutool_pincfg_deinit_spi2();
    nutool_pincfg_deinit_spim0();
    nutool_pincfg_deinit_uart0();
    nutool_pincfg_deinit_uart1();
    nutool_pincfg_deinit_uart8();
    nutool_pincfg_deinit_usb();
    nutool_pincfg_deinit_x32();
    nutool_pincfg_deinit_xt1();

    return;
}

/*** (C) COPYRIGHT 2013-2024 Nuvoton Technology Corp. ***/
