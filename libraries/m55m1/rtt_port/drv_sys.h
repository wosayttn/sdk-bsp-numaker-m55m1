#ifndef __DRV_SYS_H__
#define __DRV_SYS_H__

#include <rtthread.h>
#include "NuMicro.h"

struct nu_module
{
    char      *name;
    void      *m_pvBase;
    uint32_t   u32RstId;
    IRQn_Type  eIRQn;
} ;
typedef struct nu_module *nu_module_t;

typedef struct
{
    vu32     vu32RegAddr;
    char    *szRegName;
    vu32     vu32BitMask;
    char    *szBMName;
    vu32     vu32Value;
    char    *szVName;
} S_NU_REG;

#define SYS_GPA_MFP0    (SYS_BASE + 0x0300)   /*!< [0x0300] GPIOA Multiple Function Control Register 0*/
#define SYS_GPA_MFP1    (SYS_BASE + 0x0304)   /*!< [0x0304] GPIOA Multiple Function Control Register 1*/
#define SYS_GPA_MFP2    (SYS_BASE + 0x0308)   /*!< [0x0308] GPIOA Multiple Function Control Register 2*/
#define SYS_GPA_MFP3    (SYS_BASE + 0x030c)   /*!< [0x030c] GPIOA Multiple Function Control Register 3*/
#define SYS_GPB_MFP0    (SYS_BASE + 0x0310)   /*!< [0x0310] GPIOB Multiple Function Control Register 0*/
#define SYS_GPB_MFP1    (SYS_BASE + 0x0314)   /*!< [0x0314] GPIOB Multiple Function Control Register 1*/
#define SYS_GPB_MFP2    (SYS_BASE + 0x0318)   /*!< [0x0318] GPIOB Multiple Function Control Register 2*/
#define SYS_GPB_MFP3    (SYS_BASE + 0x031c)   /*!< [0x031c] GPIOB Multiple Function Control Register 3*/
#define SYS_GPC_MFP0    (SYS_BASE + 0x0320)   /*!< [0x0320] GPIOC Multiple Function Control Register 0*/
#define SYS_GPC_MFP1    (SYS_BASE + 0x0324)   /*!< [0x0324] GPIOC Multiple Function Control Register 1*/
#define SYS_GPC_MFP2    (SYS_BASE + 0x0328)   /*!< [0x0328] GPIOC Multiple Function Control Register 2*/
#define SYS_GPC_MFP3    (SYS_BASE + 0x032c)   /*!< [0x032c] GPIOC Multiple Function Control Register 3*/
#define SYS_GPD_MFP0    (SYS_BASE + 0x0330)   /*!< [0x0330] GPIOD Multiple Function Control Register 0*/
#define SYS_GPD_MFP1    (SYS_BASE + 0x0334)   /*!< [0x0334] GPIOD Multiple Function Control Register 1*/
#define SYS_GPD_MFP2    (SYS_BASE + 0x0338)   /*!< [0x0338] GPIOD Multiple Function Control Register 2*/
#define SYS_GPD_MFP3    (SYS_BASE + 0x033c)   /*!< [0x033c] GPIOD Multiple Function Control Register 3*/

#define SYS_GPE_MFP0    (SYS_BASE + 0x0340)   /*!< [0x0340] GPIOE Multiple Function Control Register 0*/
#define SYS_GPE_MFP1    (SYS_BASE + 0x0344)   /*!< [0x0344] GPIOE Multiple Function Control Register 1*/
#define SYS_GPE_MFP2    (SYS_BASE + 0x0348)   /*!< [0x0348] GPIOE Multiple Function Control Register 2*/
#define SYS_GPE_MFP3    (SYS_BASE + 0x034c)   /*!< [0x034c] GPIOE Multiple Function Control Register 3*/

#define SYS_GPF_MFP0    (SYS_BASE + 0x0350)   /*!< [0x0350] GPIOF Multiple Function Control Register 0*/
#define SYS_GPF_MFP1    (SYS_BASE + 0x0354)   /*!< [0x0354] GPIOF Multiple Function Control Register 1*/
#define SYS_GPF_MFP2    (SYS_BASE + 0x0358)   /*!< [0x0358] GPIOF Multiple Function Control Register 2*/

#define SYS_GPG_MFP0    (SYS_BASE + 0x0360)   /*!< [0x0360] GPIOG Multiple Function Control Register 0*/
#define SYS_GPG_MFP1    (SYS_BASE + 0x0364)   /*!< [0x0364] GPIOG Multiple Function Control Register 1*/
#define SYS_GPG_MFP2    (SYS_BASE + 0x0368)   /*!< [0x0368] GPIOG Multiple Function Control Register 2*/
#define SYS_GPG_MFP3    (SYS_BASE + 0x036c)   /*!< [0x036c] GPIOG Multiple Function Control Register 3*/

#define SYS_GPH_MFP0    (SYS_BASE + 0x0370)   /*!< [0x0370] GPIOH Multiple Function Control Register 0*/
#define SYS_GPH_MFP1    (SYS_BASE + 0x0374)   /*!< [0x0374] GPIOH Multiple Function Control Register 1*/
#define SYS_GPH_MFP2    (SYS_BASE + 0x0378)   /*!< [0x0378] GPIOH Multiple Function Control Register 2*/
#define SYS_GPH_MFP3    (SYS_BASE + 0x037c)   /*!< [0x037c] GPIOH Multiple Function Control Register 3*/

#define SYS_GPI_MFP1    (SYS_BASE + 0x0384)   /*!< [0x0384] GPIOI Multiple Function Control Register 1*/
#define SYS_GPI_MFP2    (SYS_BASE + 0x0388)   /*!< [0x0388] GPIOI Multiple Function Control Register 2*/
#define SYS_GPI_MFP3    (SYS_BASE + 0x038c)   /*!< [0x038c] GPIOI Multiple Function Control Register 3*/

#define SYS_GPJ_MFP0    (SYS_BASE + 0x0390)   /*!< [0x0390] GPIOJ Multiple Function Control Register 0*/
#define SYS_GPJ_MFP1    (SYS_BASE + 0x0394)   /*!< [0x0394] GPIOJ Multiple Function Control Register 1*/
#define SYS_GPJ_MFP2    (SYS_BASE + 0x0398)   /*!< [0x0398] GPIOJ Multiple Function Control Register 2*/
#define SYS_GPJ_MFP3    (SYS_BASE + 0x039c)   /*!< [0x039c] GPIOJ Multiple Function Control Register 3*/

#define CLK_ACMPCTL     (CLK_BASE + 0x0200)   /*!< [0x0200] ACMP Clock Enable Control Register */
#define CLK_AWFCTL      (CLK_BASE + 0x0204)   /*!< [0x0204] AWF Clock Enable Control Register */
#define CLK_BPWMCTL     (CLK_BASE + 0x0208)   /*!< [0x0208] BPWM Clock Enable Control Register */
#define CLK_CANFDCTL    (CLK_BASE + 0x020c)   /*!< [0x020c] CANFD Clock Enable Control Register */
#define CLK_CCAPCTL     (CLK_BASE + 0x0210)   /*!< [0x0210] CCAP Clock Enable Control Register */
#define CLK_CRCCTL      (CLK_BASE + 0x0214)   /*!< [0x0214] CRC Clock Enable Control Register */
#define CLK_CRYPTOCTL   (CLK_BASE + 0x0218)   /*!< [0x0218] CRYPTO Clock Enable Control Register */
#define CLK_DACCTL      (CLK_BASE + 0x021c)   /*!< [0x021c] DAC Clock Enable Control Register */
#define CLK_DMICCTL     (CLK_BASE + 0x0220)   /*!< [0x0220] DMIC Clock Enable Control Register */
#define CLK_EADCCTL     (CLK_BASE + 0x0224)   /*!< [0x0224] EADC Clock Enable Control Register */
#define CLK_EBICTL      (CLK_BASE + 0x0228)   /*!< [0x0228] EBI Clock Enable Control Register */
#define CLK_ECAPCTL     (CLK_BASE + 0x022c)   /*!< [0x022c] ECAP Clock Enable Control Register */
#define CLK_EMACCTL     (CLK_BASE + 0x0230)   /*!< [0x0230] EMAC Clock Enable Control Register */
#define CLK_EPWMCTL     (CLK_BASE + 0x0234)   /*!< [0x0234] EPWM Clock Enable Control Register */
#define CLK_EQEICTL     (CLK_BASE + 0x0238)   /*!< [0x0238] EQEI Clock Enable Control Register */
#define CLK_FMCCTL      (CLK_BASE + 0x023c)   /*!< [0x023c] FMC Clock Enable Control Register */
#define CLK_GDMACTL     (CLK_BASE + 0x0240)   /*!< [0x0240] GDMA Clock Enable Control Register */
#define CLK_GPIOCTL     (CLK_BASE + 0x0244)   /*!< [0x0244] GPIO Clock Enable Control Register */
#define CLK_HSOTGCTL    (CLK_BASE + 0x0248)   /*!< [0x0248] HSOTG Clock Enable Control Register */
#define CLK_HSUSBDCTL   (CLK_BASE + 0x024c)   /*!< [0x024c] HSUSBD Clock Enable Control Register */
#define CLK_HSUSBHCTL   (CLK_BASE + 0x0250)   /*!< [0x0250] HSUSBH Clock Enable Control Register */
#define CLK_I2CCTL      (CLK_BASE + 0x0254)   /*!< [0x0254] I2C Clock Enable Control Register */
#define CLK_I2SCTL      (CLK_BASE + 0x0258)   /*!< [0x0258] I2S Clock Enable Control Register */
#define CLK_I3CCTL      (CLK_BASE + 0x025c)   /*!< [0x025c] I3C Clock Enable Control Register */
#define CLK_KDFCTL      (CLK_BASE + 0x0260)   /*!< [0x0260] KDF Clock Enable Control Register */
#define CLK_KPICTL      (CLK_BASE + 0x0264)   /*!< [0x0264] KPI Clock Enable Control Register */
#define CLK_KSCTL       (CLK_BASE + 0x0268)   /*!< [0x0268] KS Clock Enable Control Register */
#define CLK_LPADCCTL    (CLK_BASE + 0x026c)   /*!< [0x026c] Low Power ADC Clock Enable Control Register */
#define CLK_LPPDMACTL   (CLK_BASE + 0x0270)   /*!< [0x0270] Low Power PDMA Clock Enable Control Register */
#define CLK_LPGPIOCTL   (CLK_BASE + 0x0274)   /*!< [0x0274] Low Power GPIO Clock Enable Control Register */
#define CLK_LPI2CCTL    (CLK_BASE + 0x0278)   /*!< [0x0278] Low Power I2C Clock Enable Control Register */
#define CLK_LPSPICTL    (CLK_BASE + 0x027c)   /*!< [0x027c] Low Power SPI Clock Enable Control Register */
#define CLK_LPSRAMCTL   (CLK_BASE + 0x0280)   /*!< [0x0280] Low Power SRAM Clock Enable Control Register */
#define CLK_LPTMRCTL    (CLK_BASE + 0x0284)   /*!< [0x0284] Low Power Timer Clock Enable Control Register */
#define CLK_LPUARTCTL   (CLK_BASE + 0x0288)   /*!< [0x0288] Low Power UART Clock Enable Control Register */
#define CLK_NPUCTL      (CLK_BASE + 0x028c)   /*!< [0x028c] NPU Clock Enable Control Register */

#define CLK_OTFCCTL     (CLK_BASE + 0x0294)   /*!< [0x0294] OTFC Clock Enable Control Register */
#define CLK_OTGCTL      (CLK_BASE + 0x0298)   /*!< [0x0298] OTG Clock Enable Control Register  */
#define CLK_PDMACTL     (CLK_BASE + 0x029c)   /*!< [0x029c] PDMA Clock Enable Control Register */
#define CLK_PSIOCTL     (CLK_BASE + 0x02a0)   /*!< [0x02a0] PSIO Clock Enable Control Register */
#define CLK_QSPICTL     (CLK_BASE + 0x02a4)   /*!< [0x02a4] QSPI Clock Enable Control Register */
#define CLK_RTCCTL      (CLK_BASE + 0x02a8)   /*!< [0x02a8] RTC Clock Enable Control Register  */
#define CLK_SCCTL       (CLK_BASE + 0x02ac)   /*!< [0x02ac] SC Clock Enable Control Register   */
#define CLK_SCUCTL      (CLK_BASE + 0x02b0)   /*!< [0x02b0] SCU Clock Enable Control Register  */
#define CLK_SDHCTL      (CLK_BASE + 0x02b4)   /*!< [0x02b4] SDH Clock Enable Control Register  */
#define CLK_SPICTL      (CLK_BASE + 0x02b8)   /*!< [0x02b8] SPI Clock Enable Control Register  */
#define CLK_SPIMCTL     (CLK_BASE + 0x02bc)   /*!< [0x02bc] SPIM Clock Enable Control Register */
#define CLK_SRAMCTL     (CLK_BASE + 0x02c0)   /*!< [0x02c0] System SRAM Clock Enable Control Register */

#define CLK_STCTL       (CLK_BASE + 0x02cc)   /*!< [0x02cc] System Tick Clock Enable Control Register */
#define CLK_TAMPERCTL   (CLK_BASE + 0x02d0)   /*!< [0x02d0] TAMPER Clock Enable Control Register  */
#define CLK_TMRCTL      (CLK_BASE + 0x02d4)   /*!< [0x02d4] Timer Clock Enable Control Register   */
#define CLK_TRNGCTL     (CLK_BASE + 0x02d8)   /*!< [0x02d8] TRNG Clock Enable Control Register */
#define CLK_TTMRCTL     (CLK_BASE + 0x02dc)   /*!< [0x02dc] Tick Timer Clock Enable Control Register */
#define CLK_UARTCTL     (CLK_BASE + 0x02e0)   /*!< [0x02e0] UART Clock Enable Control Register */
#define CLK_USBDCTL     (CLK_BASE + 0x02e4)   /*!< [0x02e4] USBD Clock Enable Control Register */
#define CLK_USBHCTL     (CLK_BASE + 0x02e8)   /*!< [0x02e8] USBH Clock Enable Control Register */
#define CLK_USCICTL     (CLK_BASE + 0x02ec)   /*!< [0x02ec] USCI Clock Enable Control Register */
#define CLK_UTCPDCTL    (CLK_BASE + 0x02f0)   /*!< [0x02f0] UTCPD Clock Enable Control Register */
#define CLK_WDTCTL      (CLK_BASE + 0x02f4)   /*!< [0x02f4] WDT Clock Enable Control Register  */
#define CLK_WWDTCTL     (CLK_BASE + 0x02f8)   /*!< [0x02f8] WWDT Clock Enable Control Register */

#define CLK_SCLKSEL     (CLK_BASE + 0x0400)   /*!< [0x0400] System Clock Source Select Control Register */
#define CLK_BPWMSEL     (CLK_BASE + 0x0404)   /*!< [0x0404] BPWM Clock Source Select Control Register */
#define CLK_CANFDSEL    (CLK_BASE + 0x0408)   /*!< [0x0408] CANFD Clock Source Select Control Register */
#define CLK_CCAPSEL     (CLK_BASE + 0x040c)   /*!< [0x040c] CCAP Clock Source Select Control Register */
#define CLK_CLKOSEL     (CLK_BASE + 0x0410)   /*!< [0x0410] Clock Output Clock Source Select Control Register */
#define CLK_DMICSEL     (CLK_BASE + 0x0414)   /*!< [0x0414] DMIC Clock Source Select Control Register */
#define CLK_EADCSEL     (CLK_BASE + 0x0418)   /*!< [0x0418] EADC Clock Source Select Control Register */
#define CLK_EPWMSEL     (CLK_BASE + 0x041c)   /*!< [0x041c] EPWM Clock Source Select Control Register */
#define CLK_FMCSEL      (CLK_BASE + 0x0420)   /*!< [0x0420] FMC Clock Source Select Control Register */
#define CLK_I2SSEL      (CLK_BASE + 0x0424)   /*!< [0x0424] I2S Clock Source Select Control Register */
#define CLK_I3CSEL      (CLK_BASE + 0x0428)   /*!< [0x0428] I3C Clock Source Select Control Register */
#define CLK_KPISEL      (CLK_BASE + 0x042c)   /*!< [0x042c] KPI Clock Source Select Control Register */
#define CLK_LPADCSEL    (CLK_BASE + 0x0430)   /*!< [0x0430] Low Power ADC Clock Source Select Control Register */
#define CLK_LPSPISEL    (CLK_BASE + 0x0434)   /*!< [0x0434] Low Power SPI Clock Source Select Control Register */
#define CLK_LPTMRSEL    (CLK_BASE + 0x0438)   /*!< [0x0438] Low Power Timer Clock Source Select Control Register */
#define CLK_LPUARTSEL   (CLK_BASE + 0x043c)   /*!< [0x043c] Low Power UART Clock Source Select Control Register */
#define CLK_PSIOSEL     (CLK_BASE + 0x0440)   /*!< [0x0440] PSIO Clock Source Select Control Register */
#define CLK_QSPISEL     (CLK_BASE + 0x0444)   /*!< [0x0444] QSPI Clock Source Select Control Register */
#define CLK_SCSEL       (CLK_BASE + 0x0448)   /*!< [0x0448] SC Clock Source Select Control Register */
#define CLK_SDHSEL      (CLK_BASE + 0x044c)   /*!< [0x044c] SDH Clock Source Select Control Register */
#define CLK_SPISEL      (CLK_BASE + 0x0450)   /*!< [0x0450] SPI Clock Source Select Control Register */
#define CLK_STSEL       (CLK_BASE + 0x0454)   /*!< [0x0454] System Tick Clock Source Select Control Register */
#define CLK_TMRSEL      (CLK_BASE + 0x0458)   /*!< [0x0458] Timer Clock Source Select Control Register  */
#define CLK_TTMRSEL     (CLK_BASE + 0x045c)   /*!< [0x045c] Tick Timer Clock Source Select Control Register */
#define CLK_UARTSEL0    (CLK_BASE + 0x0460)   /*!< [0x0460] UART Clock Source Select Control Register 0 */
#define CLK_UARTSEL1    (CLK_BASE + 0x0464)   /*!< [0x0464] UART Clock Source Select Control Register 1 */
#define CLK_USBSEL      (CLK_BASE + 0x0468)   /*!< [0x0468] USB Clock Source Select Control Register */
#define CLK_WDTSEL      (CLK_BASE + 0x046c)   /*!< [0x046c] WDT Clock Source Select Control Register */
#define CLK_WWDTSEL     (CLK_BASE + 0x0470)   /*!< [0x0470] WWDT Clock Source Select Control Register */
#define CLK_DLLSEL      (CLK_BASE + 0x0474)   /*!< [0x0474] DLL Clock Source Select Control Register */
#define CLK_SCLKDIV     (CLK_BASE + 0x0500)   /*!< [0x0500] SCLK Clock Divider Control Register */
#define CLK_HCLKDIV     (CLK_BASE + 0x0508)   /*!< [0x0508] HCLK Clock Divider Control Register */
#define CLK_PCLKDIV     (CLK_BASE + 0x050c)   /*!< [0x050c] PCLK Clock Divider Control Register */
#define CLK_CANFDDIV    (CLK_BASE + 0x0510)   /*!< [0x0510] CANFD Clock Divider Control Register */
#define CLK_DMICDIV     (CLK_BASE + 0x0514)   /*!< [0x0514] DMIC Clock Divider Control Register */
#define CLK_EADCDIV     (CLK_BASE + 0x0518)   /*!< [0x0518] EADC Clock Divider Control Register */
#define CLK_I2SDIV      (CLK_BASE + 0x051c)   /*!< [0x051c] I2S Clock Divider Control Register */
#define CLK_KPIDIV      (CLK_BASE + 0x0520)   /*!< [0x0520] KPI Clock Divider Control Register */
#define CLK_LPADCDIV    (CLK_BASE + 0x0524)   /*!< [0x0524] Low Power ADC Clock Divider Control Register */
#define CLK_LPUARTDIV   (CLK_BASE + 0x0528)   /*!< [0x0528] Low Power UART Clock Divider Control Register */
#define CLK_PSIODIV     (CLK_BASE + 0x052c)   /*!< [0x052c] PSIO Clock Divider Control Register */
#define CLK_SCDIV       (CLK_BASE + 0x0530)   /*!< [0x0530] SC Clock Divider Control Register */
#define CLK_SDHDIV      (CLK_BASE + 0x0534)   /*!< [0x0534] SDH Clock Divider Control Register */
#define CLK_STDIV       (CLK_BASE + 0x0538)   /*!< [0x0538] System Tick Clock Divider Control Register */
#define CLK_UARTDIV0    (CLK_BASE + 0x053c)   /*!< [0x053c] UART Clock Divider Control Register 0 */
#define CLK_UARTDIV1    (CLK_BASE + 0x0540)   /*!< [0x0540] UART Clock Divider Control Register 1 */
#define CLK_USBDIV      (CLK_BASE + 0x0544)   /*!< [0x0544] USB Clock Divider Control Register */
#define CLK_VSENSEDIV   (CLK_BASE + 0x0548)   /*!< [0x0548] Video Pixel Clock Divider Control Register */

#define NUREG_EXPORT(vu32RegAddr, vu32BitMask, vu32Value)  { vu32RegAddr, #vu32RegAddr, vu32BitMask, #vu32BitMask, vu32Value, #vu32Value }

void nu_sys_check_register(S_NU_REG *psNuReg);

#endif
