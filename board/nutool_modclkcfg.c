#include "NuMicro.h"
#include "rtconfig.h"

void nutool_modclkcfg_init_acmp01(void)
{
    CLK_EnableModuleClock(ACMP01_MODULE);

    return;
}

void nutool_modclkcfg_deinit_acmp01(void)
{
    CLK_DisableModuleClock(ACMP01_MODULE);

    return;
}

void nutool_modclkcfg_init_bpwm0(void)
{
    CLK_EnableModuleClock(BPWM0_MODULE);
    CLK_SetModuleClock(BPWM0_MODULE, CLK_BPWMSEL_BPWM0SEL_PCLK0, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_bpwm0(void)
{
    CLK_DisableModuleClock(BPWM0_MODULE);

    return;
}

void nutool_modclkcfg_init_bpwm1(void)
{
    CLK_EnableModuleClock(BPWM1_MODULE);
    CLK_SetModuleClock(BPWM1_MODULE, CLK_BPWMSEL_BPWM1SEL_PCLK2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_bpwm1(void)
{
    CLK_DisableModuleClock(BPWM1_MODULE);

    return;
}

void nutool_modclkcfg_init_canfd0(void)
{
    CLK_SetModuleClock(CANFD0_MODULE, CLK_CANFDSEL_CANFD0SEL_HCLK0, CLK_CANFDDIV_CANFD0DIV(2));
    CLK_EnableModuleClock(CANFD0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_canfd0(void)
{
    CLK_DisableModuleClock(CANFD0_MODULE);

    return;
}

void nutool_modclkcfg_init_canfd1(void)
{
    CLK_SetModuleClock(CANFD1_MODULE, CLK_CANFDSEL_CANFD1SEL_HCLK0, CLK_CANFDDIV_CANFD1DIV(2));
    CLK_EnableModuleClock(CANFD1_MODULE);

    return;
}

void nutool_modclkcfg_deinit_canfd1(void)
{
    CLK_DisableModuleClock(CANFD1_MODULE);

    return;
}

void nutool_modclkcfg_init_crc(void)
{
    CLK_EnableModuleClock(CRC0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_crc(void)
{
    CLK_DisableModuleClock(CRC0_MODULE);

    return;
}

void nutool_modclkcfg_init_crpt(void)
{
    CLK_EnableModuleClock(CRYPTO0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_crpt(void)
{
    CLK_DisableModuleClock(CRYPTO0_MODULE);

    return;
}

void nutool_modclkcfg_init_dac(void)
{
    CLK_EnableModuleClock(DAC01_MODULE);

    return;
}

void nutool_modclkcfg_deinit_dac(void)
{
    CLK_DisableModuleClock(DAC01_MODULE);

    return;
}

void nutool_modclkcfg_init_eadc0(void)
{
    CLK_EnableModuleClock(EADC0_MODULE);
    CLK_SetModuleClock(EADC0_MODULE, CLK_EADCSEL_EADC0SEL_PCLK0, CLK_EADCDIV_EADC0DIV(12));

    return;
}

void nutool_modclkcfg_deinit_eadc0(void)
{
    CLK_DisableModuleClock(EADC0_MODULE);

    return;
}

void nutool_modclkcfg_init_lpadc0(void)
{
    CLK_EnableModuleClock(LPADC0_MODULE);
    CLK_SetModuleClock(LPADC0_MODULE, CLK_LPADCSEL_LPADC0SEL_HIRC, CLK_LPADCDIV_LPADC0DIV(1));

    return;
}

void nutool_modclkcfg_deinit_lpadc0(void)
{
    CLK_DisableModuleClock(LPADC0_MODULE);

    return;
}

void nutool_modclkcfg_init_ebi(void)
{
    CLK_EnableModuleClock(EBI0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_ebi(void)
{
    CLK_DisableModuleClock(EBI0_MODULE);

    return;
}

void nutool_modclkcfg_init_ecap0(void)
{
    CLK_EnableModuleClock(ECAP0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_ecap0(void)
{
    CLK_DisableModuleClock(ECAP0_MODULE);

    return;
}

void nutool_modclkcfg_init_ecap1(void)
{
    CLK_EnableModuleClock(ECAP1_MODULE);

    return;
}

void nutool_modclkcfg_deinit_ecap1(void)
{
    CLK_DisableModuleClock(ECAP1_MODULE);

    return;
}

void nutool_modclkcfg_init_ecap2(void)
{
    CLK_EnableModuleClock(ECAP2_MODULE);

    return;
}

void nutool_modclkcfg_deinit_ecap2(void)
{
    CLK_DisableModuleClock(ECAP2_MODULE);

    return;
}

void nutool_modclkcfg_init_ecap3(void)
{
    CLK_EnableModuleClock(ECAP3_MODULE);

    return;
}

void nutool_modclkcfg_deinit_ecap3(void)
{
    CLK_DisableModuleClock(ECAP3_MODULE);

    return;
}

void nutool_modclkcfg_init_emac(void)
{
    CLK_EnableModuleClock(EMAC0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_emac(void)
{
    CLK_DisableModuleClock(EMAC0_MODULE);

    return;
}

void nutool_modclkcfg_init_epwm0(void)
{
    CLK_EnableModuleClock(EPWM0_MODULE);
    CLK_SetModuleClock(EPWM0_MODULE, CLK_EPWMSEL_EPWM0SEL_PCLK0, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_epwm0(void)
{
    CLK_DisableModuleClock(EPWM0_MODULE);

    return;
}

void nutool_modclkcfg_init_epwm1(void)
{
    CLK_EnableModuleClock(EPWM1_MODULE);
    CLK_SetModuleClock(EPWM1_MODULE, CLK_EPWMSEL_EPWM1SEL_PCLK2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_epwm1(void)
{
    CLK_DisableModuleClock(EPWM1_MODULE);

    return;
}

void nutool_modclkcfg_init_hsotg(void)
{
    CLK_EnableModuleClock(HSOTG0_MODULE);

    /* Select HSOTG PHY Reference clock frequency which is from HXT*/
    HSOTG_SET_PHY_REF_CLK(HSOTG_PHYCTL_FSEL_24_0M);

    return;
}

void nutool_modclkcfg_deinit_hsotg(void)
{
    CLK_DisableModuleClock(HSOTG0_MODULE);

    return;
}

void nutool_modclkcfg_init_hsusbd(void)
{
    CLK_EnableModuleClock(HSUSBD0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_hsusbd(void)
{
    CLK_DisableModuleClock(HSUSBD0_MODULE);

    return;
}

void nutool_modclkcfg_init_i2c0(void)
{
    CLK_EnableModuleClock(I2C0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_i2c0(void)
{
    CLK_DisableModuleClock(I2C0_MODULE);

    return;
}

void nutool_modclkcfg_init_i2c1(void)
{
    CLK_EnableModuleClock(I2C1_MODULE);

    return;
}

void nutool_modclkcfg_deinit_i2c1(void)
{
    CLK_DisableModuleClock(I2C1_MODULE);

    return;
}

void nutool_modclkcfg_init_i2c2(void)
{
    CLK_EnableModuleClock(I2C2_MODULE);

    return;
}

void nutool_modclkcfg_deinit_i2c2(void)
{
    CLK_DisableModuleClock(I2C2_MODULE);

    return;
}

void nutool_modclkcfg_init_i2c3(void)
{
    CLK_EnableModuleClock(I2C3_MODULE);

    return;
}

void nutool_modclkcfg_deinit_i2c3(void)
{
    CLK_DisableModuleClock(I2C3_MODULE);

    return;
}

void nutool_modclkcfg_init_lpi2c0(void)
{
    CLK_EnableModuleClock(LPI2C0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_lpi2c0(void)
{
    CLK_DisableModuleClock(LPI2C0_MODULE);

    return;
}

void nutool_modclkcfg_init_i2s0(void)
{
    CLK_EnableModuleClock(I2S0_MODULE);
    CLK_SetModuleClock(I2S0_MODULE, CLK_I2SSEL_I2S0SEL_APLL1_DIV2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_i2s0(void)
{
    CLK_DisableModuleClock(I2S0_MODULE);

    return;
}

void nutool_modclkcfg_init_i2s1(void)
{
    CLK_EnableModuleClock(I2S1_MODULE);
    CLK_SetModuleClock(I2S1_MODULE, CLK_I2SSEL_I2S1SEL_APLL1_DIV2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_i2s1(void)
{
    CLK_DisableModuleClock(I2S1_MODULE);

    return;
}

void nutool_modclkcfg_init_isp(void)
{
    CLK_EnableModuleClock(ISP0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_isp(void)
{
    CLK_DisableModuleClock(ISP0_MODULE);

    return;
}

void nutool_modclkcfg_init_otg(void)
{
    CLK_EnableModuleClock(OTG0_MODULE);
    CLK_SetModuleClock(OTG0_MODULE, MODULE_NoMsk, CLK_USBDIV_USBDIV(1));

    return;
}

void nutool_modclkcfg_deinit_otg(void)
{
    CLK_DisableModuleClock(OTG0_MODULE);

    return;
}

void nutool_modclkcfg_init_pdma(void)
{
    CLK_EnableModuleClock(PDMA0_MODULE);
    CLK_EnableModuleClock(PDMA1_MODULE);
    CLK_EnableModuleClock(LPPDMA0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_pdma(void)
{
    CLK_DisableModuleClock(PDMA0_MODULE);
    CLK_DisableModuleClock(PDMA1_MODULE);
    CLK_DisableModuleClock(LPPDMA0_MODULE);

    return;
}

void nutool_modclkcfg_init_eqei0(void)
{
    CLK_EnableModuleClock(EQEI0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_eqei0(void)
{
    CLK_DisableModuleClock(EQEI0_MODULE);

    return;
}

void nutool_modclkcfg_init_eqei1(void)
{
    CLK_EnableModuleClock(EQEI1_MODULE);

    return;
}

void nutool_modclkcfg_deinit_eqei1(void)
{
    CLK_DisableModuleClock(EQEI1_MODULE);

    return;
}

void nutool_modclkcfg_init_eqei2(void)
{
    CLK_EnableModuleClock(EQEI2_MODULE);

    return;
}

void nutool_modclkcfg_deinit_eqei2(void)
{
    CLK_DisableModuleClock(EQEI2_MODULE);

    return;
}

void nutool_modclkcfg_init_eqei3(void)
{
    CLK_EnableModuleClock(EQEI3_MODULE);

    return;
}

void nutool_modclkcfg_deinit_eqei3(void)
{
    CLK_DisableModuleClock(EQEI3_MODULE);

    return;
}

void nutool_modclkcfg_init_qspi0(void)
{
    CLK_EnableModuleClock(QSPI0_MODULE);
    CLK_SetModuleClock(QSPI0_MODULE, CLK_QSPISEL_QSPI0SEL_PCLK0, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_qspi0(void)
{
    CLK_DisableModuleClock(QSPI0_MODULE);

    return;
}

void nutool_modclkcfg_init_qspi1(void)
{
    CLK_EnableModuleClock(QSPI1_MODULE);
    CLK_SetModuleClock(QSPI1_MODULE, CLK_QSPISEL_QSPI1SEL_PCLK2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_qspi1(void)
{
    CLK_DisableModuleClock(QSPI1_MODULE);

    return;
}

void nutool_modclkcfg_init_rtc(void)
{
    CLK_EnableModuleClock(RTC0_MODULE);

    /* Set LXT as RTC clock source */
    RTC_SetClockSource(RTC_CLOCK_SOURCE_LXT);

    return;
}

void nutool_modclkcfg_deinit_rtc(void)
{
    CLK_DisableModuleClock(RTC0_MODULE);

    return;
}

void nutool_modclkcfg_init_sc0(void)
{
    CLK_EnableModuleClock(SC0_MODULE);
    CLK_SetModuleClock(SC0_MODULE, CLK_SCSEL_SC0SEL_HXT, CLK_SCDIV_SC0DIV(1));

    return;
}

void nutool_modclkcfg_deinit_sc0(void)
{
    CLK_DisableModuleClock(SC0_MODULE);

    return;
}

void nutool_modclkcfg_init_sc1(void)
{
    CLK_EnableModuleClock(SC1_MODULE);
    CLK_SetModuleClock(SC1_MODULE, CLK_SCSEL_SC1SEL_HXT, CLK_SCDIV_SC1DIV(1));

    return;
}

void nutool_modclkcfg_deinit_sc1(void)
{
    CLK_DisableModuleClock(SC1_MODULE);

    return;
}

void nutool_modclkcfg_init_sc2(void)
{
    CLK_EnableModuleClock(SC2_MODULE);
    CLK_SetModuleClock(SC2_MODULE, CLK_SCSEL_SC2SEL_HXT, CLK_SCDIV_SC2DIV(1));

    return;
}

void nutool_modclkcfg_deinit_sc2(void)
{
    CLK_DisableModuleClock(SC2_MODULE);

    return;
}

void nutool_modclkcfg_init_sdh0(void)
{
    CLK_EnableModuleClock(SDH0_MODULE);
    CLK_SetModuleClock(SDH0_MODULE, CLK_SDHSEL_SDH0SEL_HCLK0, CLK_SDHDIV_SDH0DIV(2));

    return;
}

void nutool_modclkcfg_deinit_sdh0(void)
{
    CLK_DisableModuleClock(SDH0_MODULE);

    return;
}

void nutool_modclkcfg_init_sdh1(void)
{
    CLK_EnableModuleClock(SDH1_MODULE);
    CLK_SetModuleClock(SDH1_MODULE, CLK_SDHSEL_SDH1SEL_HCLK0, CLK_SDHDIV_SDH1DIV(2));

    return;
}

void nutool_modclkcfg_deinit_sdh1(void)
{
    CLK_DisableModuleClock(SDH1_MODULE);

    return;
}

void nutool_modclkcfg_init_spi0(void)
{
    CLK_EnableModuleClock(SPI0_MODULE);
    CLK_SetModuleClock(SPI0_MODULE, CLK_SPISEL_SPI0SEL_PCLK0, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_spi0(void)
{
    CLK_DisableModuleClock(SPI0_MODULE);

    return;
}

void nutool_modclkcfg_init_spi1(void)
{
    CLK_EnableModuleClock(SPI1_MODULE);
    CLK_SetModuleClock(SPI1_MODULE, CLK_SPISEL_SPI1SEL_PCLK2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_spi1(void)
{
    CLK_DisableModuleClock(SPI1_MODULE);

    return;
}

void nutool_modclkcfg_init_spi2(void)
{
    CLK_EnableModuleClock(SPI2_MODULE);
    CLK_SetModuleClock(SPI2_MODULE, CLK_SPISEL_SPI2SEL_PCLK0, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_spi2(void)
{
    CLK_DisableModuleClock(SPI2_MODULE);

    return;
}

void nutool_modclkcfg_init_spi3(void)
{
    CLK_EnableModuleClock(SPI3_MODULE);
    CLK_SetModuleClock(SPI3_MODULE, CLK_SPISEL_SPI3SEL_PCLK2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_spi3(void)
{
    CLK_DisableModuleClock(SPI3_MODULE);

    return;
}

void nutool_modclkcfg_init_lpspi0(void)
{
    CLK_EnableModuleClock(LPSPI0_MODULE);
    CLK_SetModuleClock(LPSPI0_MODULE, CLK_LPSPISEL_LPSPI0SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_lpspi0(void)
{
    CLK_DisableModuleClock(LPSPI0_MODULE);

    return;
}

void nutool_modclkcfg_init_spim0(void)
{
    CLK_EnableModuleClock(SPIM0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_spim0(void)
{
    CLK_DisableModuleClock(SPIM0_MODULE);

    return;
}

void nutool_modclkcfg_init_otfc0(void)
{
    CLK_EnableModuleClock(OTFC0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_otfc0(void)
{
    CLK_DisableModuleClock(OTFC0_MODULE);

    return;
}

void nutool_modclkcfg_init_systick(void)
{
    CLK_EnableSysTick(CLK_STSEL_ST0SEL_HXT, 0);

    return;
}

void nutool_modclkcfg_deinit_systick(void)
{
    CLK_DisableSysTick();

    return;
}

void nutool_modclkcfg_init_tmr0(void)
{
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMRSEL_TMR0SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_tmr0(void)
{
    CLK_DisableModuleClock(TMR0_MODULE);

    return;
}

void nutool_modclkcfg_init_tmr1(void)
{
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_TMRSEL_TMR1SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_tmr1(void)
{
    CLK_DisableModuleClock(TMR1_MODULE);

    return;
}

void nutool_modclkcfg_init_tmr2(void)
{
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_TMRSEL_TMR2SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_tmr2(void)
{
    CLK_DisableModuleClock(TMR2_MODULE);

    return;
}

void nutool_modclkcfg_init_tmr3(void)
{
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_TMRSEL_TMR3SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_tmr3(void)
{
    CLK_DisableModuleClock(TMR3_MODULE);

    return;
}

void nutool_modclkcfg_init_ttmr0(void)
{
    CLK_EnableModuleClock(TTMR0_MODULE);
    CLK_SetModuleClock(TTMR0_MODULE, CLK_TTMRSEL_TTMR0SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_ttmr0(void)
{
    CLK_DisableModuleClock(TTMR0_MODULE);

    return;
}

void nutool_modclkcfg_init_ttmr1(void)
{
    CLK_EnableModuleClock(TTMR1_MODULE);
    CLK_SetModuleClock(TTMR1_MODULE, CLK_TTMRSEL_TTMR1SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_ttmr1(void)
{
    CLK_DisableModuleClock(TTMR1_MODULE);

    return;
}


void nutool_modclkcfg_init_lptmr0(void)
{
    CLK_EnableModuleClock(LPTMR0_MODULE);
    CLK_SetModuleClock(LPTMR0_MODULE, CLK_LPTMRSEL_LPTMR0SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_lptmr0(void)
{
    CLK_DisableModuleClock(LPTMR0_MODULE);

    return;
}

void nutool_modclkcfg_init_lptmr1(void)
{
    CLK_EnableModuleClock(LPTMR1_MODULE);
    CLK_SetModuleClock(LPTMR1_MODULE, CLK_LPTMRSEL_LPTMR1SEL_HIRC, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_lptmr1(void)
{
    CLK_DisableModuleClock(LPTMR1_MODULE);

    return;
}

void nutool_modclkcfg_init_uart0(void)
{
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_UARTSEL0_UART0SEL_HXT, CLK_UARTDIV0_UART0DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart0(void)
{
    CLK_DisableModuleClock(UART0_MODULE);

    return;
}

void nutool_modclkcfg_init_uart1(void)
{
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_UARTSEL0_UART1SEL_HXT, CLK_UARTDIV0_UART1DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart1(void)
{
    CLK_DisableModuleClock(UART1_MODULE);

    return;
}

void nutool_modclkcfg_init_uart2(void)
{
    CLK_EnableModuleClock(UART2_MODULE);
    CLK_SetModuleClock(UART2_MODULE, CLK_UARTSEL0_UART2SEL_HXT, CLK_UARTDIV0_UART2DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart2(void)
{
    CLK_DisableModuleClock(UART2_MODULE);

    return;
}

void nutool_modclkcfg_init_uart3(void)
{
    CLK_EnableModuleClock(UART3_MODULE);
    CLK_SetModuleClock(UART3_MODULE, CLK_UARTSEL0_UART3SEL_HXT, CLK_UARTDIV0_UART3DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart3(void)
{
    CLK_DisableModuleClock(UART3_MODULE);

    return;
}

void nutool_modclkcfg_init_uart4(void)
{
    CLK_EnableModuleClock(UART4_MODULE);
    CLK_SetModuleClock(UART4_MODULE, CLK_UARTSEL0_UART4SEL_HXT, CLK_UARTDIV0_UART4DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart4(void)
{
    CLK_DisableModuleClock(UART4_MODULE);

    return;
}

void nutool_modclkcfg_init_uart5(void)
{
    CLK_EnableModuleClock(UART5_MODULE);
    CLK_SetModuleClock(UART5_MODULE, CLK_UARTSEL0_UART5SEL_HXT, CLK_UARTDIV0_UART5DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart5(void)
{
    CLK_DisableModuleClock(UART5_MODULE);

    return;
}

void nutool_modclkcfg_init_uart6(void)
{
    CLK_EnableModuleClock(UART6_MODULE);
    CLK_SetModuleClock(UART6_MODULE, CLK_UARTSEL0_UART6SEL_HXT, CLK_UARTDIV0_UART6DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart6(void)
{
    CLK_DisableModuleClock(UART6_MODULE);

    return;
}

void nutool_modclkcfg_init_uart7(void)
{
    CLK_EnableModuleClock(UART7_MODULE);
    CLK_SetModuleClock(UART7_MODULE, CLK_UARTSEL0_UART7SEL_HXT, CLK_UARTDIV0_UART7DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart7(void)
{
    CLK_DisableModuleClock(UART7_MODULE);

    return;
}

void nutool_modclkcfg_init_uart8(void)
{
    CLK_EnableModuleClock(UART8_MODULE);
    CLK_SetModuleClock(UART8_MODULE, CLK_UARTSEL1_UART8SEL_HXT, CLK_UARTDIV1_UART8DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart8(void)
{
    CLK_DisableModuleClock(UART8_MODULE);

    return;
}

void nutool_modclkcfg_init_uart9(void)
{
    CLK_EnableModuleClock(UART9_MODULE);
    CLK_SetModuleClock(UART9_MODULE, CLK_UARTSEL1_UART9SEL_HXT, CLK_UARTDIV1_UART9DIV(1));

    return;
}

void nutool_modclkcfg_deinit_uart9(void)
{
    CLK_DisableModuleClock(UART9_MODULE);

    return;
}

void nutool_modclkcfg_init_lpuart0(void)
{
    CLK_EnableModuleClock(LPUART0_MODULE);
    CLK_SetModuleClock(LPUART0_MODULE, CLK_LPUARTSEL_LPUART0SEL_HIRC, CLK_LPUARTDIV_LPUART0DIV(1));

    return;
}

void nutool_modclkcfg_deinit_lpuart0(void)
{
    CLK_DisableModuleClock(LPUART0_MODULE);

    return;
}


void nutool_modclkcfg_init_usbd(void)
{
    CLK_EnableModuleClock(USBD0_MODULE);
    CLK_SetModuleClock(USBD0_MODULE, CLK_USBSEL_USBSEL_APLL1_DIV2, CLK_USBDIV_USBDIV(1));

    return;
}

void nutool_modclkcfg_deinit_usbd(void)
{
    CLK_DisableModuleClock(USBD0_MODULE);

    return;
}

void nutool_modclkcfg_init_usbh(void)
{
    CLK_EnableModuleClock(USBH0_MODULE);
    CLK_SetModuleClock(USBH0_MODULE, CLK_USBSEL_USBSEL_APLL1_DIV2, CLK_USBDIV_USBDIV(1));

    CLK_EnableModuleClock(HSUSBH0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_usbh(void)
{
    CLK_DisableModuleClock(USBH0_MODULE);
    CLK_DisableModuleClock(HSUSBH0_MODULE);

    return;
}

void nutool_modclkcfg_init_usci0(void)
{
    CLK_EnableModuleClock(USCI0_MODULE);

    return;
}

void nutool_modclkcfg_deinit_usci0(void)
{
    CLK_DisableModuleClock(USCI0_MODULE);

    return;
}

void nutool_modclkcfg_init_wdt(void)
{
    CLK_EnableModuleClock(WDT0_MODULE);
    CLK_SetModuleClock(WDT0_MODULE, CLK_WDTSEL_WDT0SEL_LXT, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_wdt(void)
{
    CLK_DisableModuleClock(WDT0_MODULE);

    return;
}

void nutool_modclkcfg_init_dmic0(void)
{
    CLK_EnableModuleClock(DMIC0_MODULE);
    CLK_SetModuleClock(DMIC0_MODULE, CLK_DMICSEL_DMIC0SEL_APLL1_DIV2, MODULE_NoMsk);

    return;
}

void nutool_modclkcfg_deinit_dmic0(void)
{
    CLK_DisableModuleClock(DMIC0_MODULE);

    return;
}

void nutool_modclkcfg_init_lpgpio0(void)
{
    CLK_EnableModuleClock(LPGPIO0_MODULE);
}

void nutool_modclkcfg_deinit_lpgpio0(void)
{
    CLK_DisableModuleClock(LPGPIO0_MODULE);
}

void nutool_modclkcfg_init_npu0(void)
{
    /* Enable FMC0 module clock to keep FMC clock when CPU idle but NPU running*/
    CLK_EnableModuleClock(FMC0_MODULE);
    CLK_EnableModuleClock(NPU0_MODULE);
}

void nutool_modclkcfg_deinit_npu0(void)
{
    CLK_DisableModuleClock(NPU0_MODULE);
    CLK_DisableModuleClock(FMC0_MODULE);
}

void nutool_modclkcfg_init_base(void)
{
    /* Enable clock */
    CLK_EnableXtalRC(CLK_SRCCTL_LXTEN_Msk | CLK_SRCCTL_HXTEN_Msk | CLK_SRCCTL_HIRCEN_Msk | CLK_SRCCTL_HIRC48MEN_Msk);

    /* Wait for clock ready */
    CLK_WaitClockReady(/*CLK_STATUS_LXTSTB_Msk |*/ CLK_STATUS_HXTSTB_Msk | CLK_STATUS_HIRCSTB_Msk | CLK_STATUS_HIRC48MSTB_Msk);

    /* Enable APLL1 96MHz clock */
    CLK_EnableAPLL(CLK_APLLCTL_APLLSRC_HXT, 96000000, CLK_APLL1_SELECT);

    /* Enable all GPIO clock */
    CLK_EnableModuleClock(GPIOA_MODULE);
    CLK_EnableModuleClock(GPIOB_MODULE);
    CLK_EnableModuleClock(GPIOC_MODULE);
    CLK_EnableModuleClock(GPIOD_MODULE);
    CLK_EnableModuleClock(GPIOE_MODULE);
    CLK_EnableModuleClock(GPIOF_MODULE);
    CLK_EnableModuleClock(GPIOG_MODULE);
    CLK_EnableModuleClock(GPIOH_MODULE);
    CLK_EnableModuleClock(GPIOI_MODULE);
    CLK_EnableModuleClock(GPIOJ_MODULE);

    return;
}

void Reset_Handler_PreInit(void)
{
    /* Enable PLL0 and set to __HSI clock frequency, source is from HIRC and switch SCLK clock source to PLL0 */
    CLK_SetBusClock(CLK_SCLKSEL_SCLKSEL_APLL0, CLK_APLLCTL_APLLSRC_HXT, __HSI);
}

void nutool_modclkcfg_init(void)
{
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    nutool_modclkcfg_init_base();

    /* Enable module clock and set clock source */
#if defined(BSP_USING_ACMP01)
    nutool_modclkcfg_init_acmp01();
#endif
#if defined(BSP_USING_BPWM0)
    nutool_modclkcfg_init_bpwm0();
#endif
#if defined(BSP_USING_BPWM1)
    nutool_modclkcfg_init_bpwm1();
#endif
#if defined(BSP_USING_CAN0)
    nutool_modclkcfg_init_can0();
#endif
#if defined(BSP_USING_CAN1)
    nutool_modclkcfg_init_can1();
#endif
#if defined(BSP_USING_CRC)
    nutool_modclkcfg_init_crc();
#endif
#if defined(BSP_USING_CRYPTO)
    nutool_modclkcfg_init_crpt();
#endif
#if defined(BSP_USING_DAC)
    nutool_modclkcfg_init_dac();
#endif
#if defined(BSP_USING_DMIC0)
    nutool_modclkcfg_init_dmic0();
#endif
#if defined(BSP_USING_EADC0)
    nutool_modclkcfg_init_eadc0();
#endif
#if defined(BSP_USING_EBI)
    nutool_modclkcfg_init_ebi();
#endif
#if defined(BSP_USING_SPIM0)
    nutool_modclkcfg_init_spim0();
#endif
#if defined(BSP_USING_OTFC0)
    nutool_modclkcfg_init_otfc0();
#endif
#if defined(BSP_USING_ECAP0)
    nutool_modclkcfg_init_ecap0();
#endif
#if defined(BSP_USING_ECAP1)
    nutool_modclkcfg_init_ecap1();
#endif
#if defined(BSP_USING_ECAP2)
    nutool_modclkcfg_init_ecap2();
#endif
#if defined(BSP_USING_ECAP3)
    nutool_modclkcfg_init_ecap3();
#endif
#if defined(BSP_USING_EMAC)
    nutool_modclkcfg_init_emac();
#endif
#if defined(BSP_USING_EPWM0)
    nutool_modclkcfg_init_epwm0();
#endif
#if defined(BSP_USING_EPWM1)
    nutool_modclkcfg_init_epwm1();
#endif
#if defined(BSP_USING_FMC)
    //nutool_modclkcfg_init_fmcidle();
#endif
#if defined(BSP_USING_HSOTG)
    nutool_modclkcfg_init_hsotg();
#endif
#if defined(BSP_USING_HSUSBD)
    nutool_modclkcfg_init_hsusbd();
#endif
#if defined(BSP_USING_I2C0)
    nutool_modclkcfg_init_i2c0();
#endif
#if defined(BSP_USING_I2C1)
    nutool_modclkcfg_init_i2c1();
#endif
#if defined(BSP_USING_I2C2)
    nutool_modclkcfg_init_i2c2();
#endif
#if defined(BSP_USING_I2C3)
    nutool_modclkcfg_init_i2c3();
#endif

#if defined(BSP_USING_I2S0)
    nutool_modclkcfg_init_i2s0();
#endif
#if defined(BSP_USING_I2S1)
    nutool_modclkcfg_init_i2s1();
#endif
#if defined(BSP_USING_FMC)
    nutool_modclkcfg_init_isp();
#endif

#if defined(BSP_USING_OTG) || defined(BSP_USING_USBH)
    nutool_modclkcfg_init_otg();
#endif

#if defined(BSP_USING_PDMA)
    nutool_modclkcfg_init_pdma();
#endif

#if defined(BSP_USING_EQEI0)
    nutool_modclkcfg_init_eqei0();
#endif
#if defined(BSP_USING_EQEI1)
    nutool_modclkcfg_init_eqei1();
#endif

#if defined(BSP_USING_QSPI0)
    nutool_modclkcfg_init_qspi0();
#endif
#if defined(BSP_USING_QSPI1)
    nutool_modclkcfg_init_qspi1();
#endif

#if defined(BSP_USING_RTC)
    nutool_modclkcfg_init_rtc();
#endif

#if defined(BSP_USING_SCUART0)
    nutool_modclkcfg_init_sc0();
#endif
#if defined(BSP_USING_SCUART1)
    nutool_modclkcfg_init_sc1();
#endif
#if defined(BSP_USING_SCUART2)
    nutool_modclkcfg_init_sc2();
#endif

#if defined(BSP_USING_SDH0)
    nutool_modclkcfg_init_sdh0();
#endif
#if defined(BSP_USING_SDH1)
    nutool_modclkcfg_init_sdh1();
#endif

#if defined(BSP_USING_SPI0) || defined(BSP_USING_SPII2S0)
    nutool_modclkcfg_init_spi0();
#endif
#if defined(BSP_USING_SPI1) || defined(BSP_USING_SPII2S1)
    nutool_modclkcfg_init_spi1();
#endif
#if defined(BSP_USING_SPI2) || defined(BSP_USING_SPII2S2)
    nutool_modclkcfg_init_spi2();
#endif
#if defined(BSP_USING_SPI3) || defined(BSP_USING_SPII2S3)
    nutool_modclkcfg_init_spi3();
#endif


#if defined(BSP_USING_TMR0)
    nutool_modclkcfg_init_tmr0();
#endif
#if defined(BSP_USING_TMR1)
    nutool_modclkcfg_init_tmr1();
#endif
#if defined(BSP_USING_TMR2)
    nutool_modclkcfg_init_tmr2();
#endif
#if defined(BSP_USING_TMR3)
    nutool_modclkcfg_init_tmr3();
#endif

#if defined(BSP_USING_TTMR0)
    nutool_modclkcfg_init_ttmr0();
#endif
#if defined(BSP_USING_TTMR1)
    nutool_modclkcfg_init_ttmr1();
#endif


#if defined(BSP_USING_UART0)
    nutool_modclkcfg_init_uart0();
#endif
#if defined(BSP_USING_UART1)
    nutool_modclkcfg_init_uart1();
#endif
#if defined(BSP_USING_UART2)
    nutool_modclkcfg_init_uart2();
#endif
#if defined(BSP_USING_UART3)
    nutool_modclkcfg_init_uart3();
#endif
#if defined(BSP_USING_UART4)
    nutool_modclkcfg_init_uart4();
#endif
#if defined(BSP_USING_UART5)
    nutool_modclkcfg_init_uart5();
#endif
#if defined(BSP_USING_UART6)
    nutool_modclkcfg_init_uart6();
#endif
#if defined(BSP_USING_UART7)
    nutool_modclkcfg_init_uart7();
#endif
#if defined(BSP_USING_UART8)
    nutool_modclkcfg_init_uart8();
#endif
#if defined(BSP_USING_UART9)
    nutool_modclkcfg_init_uart9();
#endif

#if defined(BSP_USING_USBD) || defined(BSP_USING_USBH)
    nutool_modclkcfg_init_usbd();
#endif

#if defined(BSP_USING_USBH) || defined(BSP_USING_HSUSBH)
    nutool_modclkcfg_init_usbh();
#endif

#if defined(BSP_USING_USCI0)
    nutool_modclkcfg_init_usci0();
#endif

#if defined(BSP_USING_WDT)
    nutool_modclkcfg_init_wdt();
#endif

#if defined(BSP_USING_CANFD0)
    nutool_modclkcfg_init_canfd0();
#endif
#if defined(BSP_USING_CANFD1)
    nutool_modclkcfg_init_canfd1();
#endif

#if defined(BSP_USING_LPADC0)
    nutool_modclkcfg_init_lpadc0();
#endif

#if defined(BSP_USING_LPGPIO)
    nutool_modclkcfg_init_lpgpio0();
#endif
#if defined(BSP_USING_LPI2C0)
    nutool_modclkcfg_init_lpi2c0();
#endif
#if defined(BSP_USING_LPSPI0)
    nutool_modclkcfg_init_lpspi0();
#endif
#if defined(BSP_USING_LPTMR0)
    nutool_modclkcfg_init_lptmr0();
#endif
#if defined(BSP_USING_LPTMR1)
    nutool_modclkcfg_init_lptmr1();
#endif
#if defined(BSP_USING_LPUART0)
    nutool_modclkcfg_init_lpuart0();
#endif

#if defined(NU_PKG_USING_MLEVK)
    nutool_modclkcfg_init_npu0();
#endif

    nutool_modclkcfg_init_systick();

    return;
}

