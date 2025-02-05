#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 32
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 1024

/* kservice optimization */

#define RT_KSERVICE_USING_STDLIB
#define RT_KSERVICE_USING_TINY_SIZE
#define RT_KPRINTF_USING_LONGLONG
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x50000
#define RT_USING_CACHE

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_LEGACY
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 8
#define DFS_FILESYSTEM_TYPES_MAX 8
#define DFS_FD_MAX 32
#define RT_USING_DFS_MNTTABLE
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 8
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_DEVFS
#define RT_USING_FAL
#define FAL_DEBUG_CONFIG
#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 8192
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 1024
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_RTC
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 2048
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 2048
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 16
#define RT_USING_SPI
#define RT_USING_AUDIO
#define RT_AUDIO_REPLAY_MP_BLOCK_SIZE 8192
#define RT_AUDIO_REPLAY_MP_BLOCK_COUNT 2
#define RT_AUDIO_RECORD_PIPE_SIZE 8192
#define RT_USING_SENSOR
#define RT_USING_SENSOR_CMD
#define RT_USING_TOUCH
#define RT_TOUCH_PIN_IRQ

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_DEVIO
#define RT_USING_POSIX_POLL
#define RT_USING_POSIX_SELECT
#define RT_USING_POSIX_SOCKET
#define RT_USING_POSIX_DELAY
#define RT_USING_POSIX_CLOCK

/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */

#define RT_USING_CPLUSPLUS

/* Network */

#define RT_USING_SAL
#define SAL_INTERNET_CHECK

/* Docking with protocol stacks */

#define SAL_USING_LWIP
#define SAL_USING_POSIX
#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0
#define RT_USING_LWIP
#define RT_USING_LWIP212
#define RT_USING_LWIP_VER_NUM 0x20102
#define RT_LWIP_MEM_ALIGNMENT 32
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.31.55"
#define RT_LWIP_GWADDR "192.168.31.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 64
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 32
#define RT_LWIP_TCP_SND_BUF 11680
#define RT_LWIP_TCP_WND 23360
#define RT_LWIP_TCPTHREAD_PRIORITY 3
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 128
#define RT_LWIP_TCPTHREAD_STACKSIZE 2048
#define LWIP_NO_RX_THREAD
#define LWIP_NO_TX_THREAD
#define RT_LWIP_ETHTHREAD_PRIORITY 5
#define RT_LWIP_ETHTHREAD_STACKSIZE 2048
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 64
#define RT_LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define RT_LWIP_NETIF_LOOPBACK
#define LWIP_NETIF_LOOPBACK 1
#define RT_LWIP_STATS
#define RT_LWIP_USING_HW_CHECKSUM
#define RT_LWIP_USING_PING

/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_USING_ISR_LOG
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define RT_USING_UTEST
#define UTEST_THR_STACK_SIZE 4096
#define UTEST_THR_PRIORITY 20

/* RT-Thread Utestcases */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* CYW43012 WiFi */


/* BL808 WiFi */


/* CYW43439 WiFi */

#define PKG_USING_NETUTILS
#define PKG_NETUTILS_IPERF
#define IPERF_THREAD_STACK_SIZE 2048
#define PKG_NETUTILS_NTP
#define NTP_USING_AUTO_SYNC
#define NTP_AUTO_SYNC_FIRST_DELAY 5
#define NTP_AUTO_SYNC_PERIOD 3600
#define NETUTILS_NTP_HOSTNAME "time.stdtime.gov.tw"
#define NETUTILS_NTP_HOSTNAME2 "tick.stdtime.gov.tw"
#define NETUTILS_NTP_HOSTNAME3 "tock.stdtime.gov.tw"
#define PKG_USING_NETUTILS_LATEST_VERSION
#define PKG_NETUTILS_VER_NUM 0x99999

/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */

#define PKG_USING_LVGL
#define PKG_LVGL_THREAD_PRIO 20
#define PKG_LVGL_THREAD_STACK_SIZE 4096
#define PKG_LVGL_DISP_REFR_PERIOD 33
#define PKG_LVGL_USING_SQUARELINE
#define PKG_LVGL_USING_V09010
#define PKG_LVGL_VER_NUM 0x009010

/* u8g2: a monochrome graphic library */


/* tools packages */


/* system packages */

/* enhanced kernel services */

#define PKG_USING_RT_VSNPRINTF_FULL
#define PKG_VSNPRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_WRITEBACK_SPECIFIER
#define PKG_VSNPRINTF_SUPPORT_LONG_LONG
#define PKG_VSNPRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER
#define PKG_VSNPRINTF_INTEGER_BUFFER_SIZE 32
#define PKG_VSNPRINTF_DECIMAL_BUFFER_SIZE 32
#define PKG_VSNPRINTF_DEFAULT_FLOAT_PRECISION 6
#define PKG_VSNPRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL 9
#define PKG_VSNPRINTF_LOG10_TAYLOR_TERMS 4
#define PKG_USING_RT_VSNPRINTF_FULL_LATEST_VERSION

/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */

#define PKG_USING_RAMDISK
#define PKG_USING_RAMDISK_LATEST_VERSION

/* peripheral libraries and drivers */

/* HAL & SDK Drivers */

/* STM32 HAL & SDK Drivers */


/* Kendryte SDK */


/* sensors drivers */

#define PKG_USING_MPU6XXX
#define PKG_USING_MPU6XXX_LATEST_VERSION
#define PKG_USING_MPU6XXX_ACCE
#define PKG_USING_MPU6XXX_GYRO
#define PKG_USING_MPU6XXX_MAG

/* touch drivers */


/* AI packages */


/* Signal Processing and Control Algorithm Packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */


/* Arduino libraries */


/* Projects and Demos */


/* Sensors */


/* Display */


/* Timing */


/* Data Processing */


/* Data Storage */

/* Communication */


/* Device Control */


/* Other */


/* Signal IO */


/* Uncategorized */

/* Hardware Drivers Config */

/* On-chip Peripheral Drivers */

#define SOC_FAMILY_NUMICRO
#define SOC_SERIES_M55M1
#define BSP_USE_STDDRIVER_SOURCE
#define BSP_USING_GDMA
#define BSP_USING_PDMA
#define NU_PDMA_MEMFUN_ACTOR_MAX 2
#define NU_PDMA_SGTBL_POOL_SIZE 32
#define BSP_USING_GPIO
#define BSP_USING_EMAC
#define BSP_USING_DMIC
#define BSP_USING_DMIC0
#define DMIC0_MIC_CLOCK_MIN 1000000
#define DMIC0_MIC_CLOCK_MAX 6000000
#define DMIC0_MIC_ENABLE_CHANNEL 0x2
#define BSP_USING_RTC
#define BSP_USING_CCAP
#define BSP_USING_CCAP0
#define BSP_USING_UART
#define BSP_USING_UART0
#define BSP_USING_UART8
#define BSP_USING_UART8_TX_DMA
#define BSP_USING_UART8_RX_DMA
#define BSP_USING_I2C
#define BSP_USING_I2C0
#define BSP_USING_I2C1
#define BSP_USING_I2C3
#define BSP_USING_SDH
#define BSP_USING_SDH0
#define BSP_USING_SPI
#define BSP_USING_SPI_PDMA
#define BSP_USING_SPI0_NONE
#define BSP_USING_SPI1_NONE
#define BSP_USING_SPI2
#define BSP_USING_SPI2_PDMA
#define BSP_USING_SPI3_NONE
#define BSP_USING_I2S
#define BSP_USING_I2S0
#define NU_I2S_DMA_FIFO_SIZE 2048
#define BSP_USING_EBI
#define BSP_USING_SPIM
#define BSP_USING_SPIM0
#define SPIM_HYPER_DMM0_SIZE 0x800000

/* On-board Peripheral Drivers */

#define BSP_USING_NULINKME
#define BOARD_USING_DIGITAL_MICROPHONE
#define BOARD_USING_RTL8201FI
#define BOARD_USING_NAU8822
#define BOARD_USING_STORAGE_SDCARD
#define BOARD_USING_EXTERNAL_HYPERRAM
#define BOARD_USING_HYPERRAM_SIZE 8388608

/* Board extended module drivers */

#define BOARD_USING_LCD_LT7381
#define BOARD_USING_LT7381_EBI_PORT 0
#define BOARD_USING_LT7381_PIN_BACKLIGHT 101
#define BOARD_USING_LT7381_PIN_RESET 118
#define BOARD_USING_LT7381_PIN_DISPLAY 112
#define BOARD_USING_FT5446
#define BOARD_USING_SENSOR0
#define BOARD_USING_SENSON0_ID 0

/* Nuvoton Packages Config */

#define NU_PKG_USING_UTILS
#define NU_PKG_USING_NAU8822
#define BSP_LCD_BPP 16
#define BSP_LCD_WIDTH 800
#define BSP_LCD_HEIGHT 480
#define NU_PKG_USING_LT7381
#define NU_PKG_USING_LT7381_EBI
#define NU_PKG_LT7381_WITH_OFFSCREEN_FRAMEBUFFER
#define NU_PKG_LT7381_LINE_BUFFER_NUMBER 480
#define NU_PKG_EBI_I80_CMD 0x0
#define NU_PKG_EBI_I80_DATA 0x2
#define NU_PKG_USING_TPC
#define NU_PKG_USING_TPC_FT5446
#define UTEST_CMD_PREFIX "bsp.nuvoton.utest."
#define BOARD_USE_UTEST

/* ml-embedded-evaluation-kit */

#define NU_PKG_USING_MLEVK
#define MLEVK_UC_OBJECT_DETECTION
#define MLEVK_UC_LIVE_DEMO
#define MLEVK_UC_AREANA_DYNAMIC_ALLOCATE
#define MLEVK_UC_AREANA_PLACE_SRAM
#define MLEVK_UC_LCD_RENDERING_DEVICE "lcd"
#define MLEVK_UC_AUDIO_CAPTURE_DEVICE "dmic0"
#define MLEVK_UC_AUDIO_PLAYBACK_DEVICE "sound0"

#endif
