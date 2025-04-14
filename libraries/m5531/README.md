# M5531 Series

## Supported drivers

| Peripheral | rt_device_class_type | Device name |
| ------ | ----  | :------:  |
| BPWM | RT_Device_Class_Miscellaneous (PWM) | ***bpwm[0-1]*** |
| BPWM (Capture function)| RT_Device_Class_Miscellaneous (Input capture) | ***bpwm[0-1]i[0-5]*** |
| CANFD | RT_Device_Class_CAN | ***canfd[0-1]*** |
| CCAP | RT_Device_Class_Miscellaneous | ***ccap0*** |
| CRC | RT_Device_Class_Miscellaneous (HW Crypto) | ***hwcryto*** |
| CRYPTO | RT_Device_Class_Miscellaneous (HW Crypto) | ***hwcryto*** |
| DAC | RT_Device_Class_Miscellaneous (DAC) | ***dac[0-1]*** |
| DMIC | RT_Device_Class_Sound/RT_Device_Class_Pipe | ***dmic0*** |
| DTCM | RT_USING_MEMHEAP | ***dtcm*** |
| EADC | RT_Device_Class_Miscellaneous (ADC) | ***eadc0*** |
| EBI | N/A | ***N/A*** |
| ECAP | RT_Device_Class_Miscellaneous (Input capture) | ***ecap[0-3]i[0-2]*** |
| EMAC | RT_Device_Class_NetIf | ***e0*** |
| EPWM | RT_Device_Class_Miscellaneous (PWM) | ***epwm[0-1]*** |
| EPWM (Capture function) | RT_Device_Class_Miscellaneous (Input capture) | ***epwm[0-1]i[0-5]*** |
| EQEI | RT_Device_Class_Miscellaneous (Pulse encoder) | ***eqei[0-3]*** |
| FMC | FAL | ***N/A*** |
| GPIO | RT_Device_Class_Miscellaneous (Pin) | ***gpio*** |
| GPIO | RT_Device_Class_I2CBUS | ***softi2c0[0-1]*** |
| I2C | RT_Device_Class_I2CBUS | ***i2c[0-4]*** |
| I2S | RT_Device_Class_Sound/RT_Device_Class_Pipe | ***sound[0, 1]*** |
| ITCM | RT_USING_MEMHEAP | ***itcm*** |
| LPADC | RT_Device_Class_Miscellaneous (ADC) | ***lpadc0*** |
| LPI2C | RT_Device_Class_I2CBUS | ***lpi2c0*** |
| LPSPI | RT_Device_Class_SPIBUS | ***lpspi0*** |
| LPTIMER | RT_Device_Class_Timer | ***lptmr[0,1]*** |
| LPPDMA | N/A | ***N/A*** |
| LPSRAM | RT_USING_MEMHEAP | ***lpsram*** |
| LPUART | RT_Device_Class_Char | ***lpuart0*** |
| PDMA | N/A | ***N/A*** |
| QSPI | RT_Device_Class_SPIBUS | ***qspi[0-1]*** |
| RTC | RT_Device_Class_RTC | ***rtc*** |
| SC (UART function) | RT_Device_Class_Char | ***scuart[0-2]*** |
| SDH | RT_Device_Class_Sdio | ***sdh[0-1]*** |
| SPI | RT_Device_Class_SPIBUS | ***spi[0-10]*** |
| SPI (I2S function) | RT_Device_Class_Sound/RT_Device_Class_Pipe | ***spii2s[0-10]*** |
| SPIM | RT_USING_MEMHEAP | ***spim0*** |
| TIMER | RT_Device_Class_Timer | ***timer[0-3]*** |
| TIMER (PWM function) | RT_Device_Class_Miscellaneous (PWM) | ***tpwm[0-3]*** |
| TRNG | RT_Device_Class_Miscellaneous (HW Crypto) | ***hwcryto*** |
| UART | RT_Device_Class_Char | ***uart[0-7]*** |
| USCI (I2C function) | RT_Device_Class_I2CBUS | ***ui2c[0-1]*** |
| USCI (SPI function) | RT_Device_Class_SPIBUS | ***uspi[0-1]*** |
| USCI (UART function) | RT_Device_Class_Char | ***uuart[0-1]*** |
| WDT | RT_Device_Class_Miscellaneous (Watchdog) | ***wdt*** |

## Resources

* [Download M460 TRM][1]

  [1]: https://www.nuvoton.com/resource-download.jsp?tp_GUID=DA05-M460
