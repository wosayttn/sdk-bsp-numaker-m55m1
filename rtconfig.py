import os

# toolchains options
ARCH='arm'
CPU='cortex-m55'
CROSS_TOOL='gcc'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')
if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path

if  CROSS_TOOL == 'gcc':
    PLATFORM = 'gcc'
    EXEC_PATH = r'C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\bin'
elif CROSS_TOOL == 'keil':
    PLATFORM 	= 'armclang'
    EXEC_PATH 	= r'C:\Keil_v5'

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

CROSS_TOOL='gcc'

BUILD = ''
#BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC = PREFIX + 'gcc'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    CXX = PREFIX + 'g++'
    LINK = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'
    STRIP = PREFIX + 'strip'

    DEVICE = ' -march=armv8.1-m.main -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -ffreestanding -funwind-tables -fno-strict-aliasing '
    CFLAGS = DEVICE + ' -Dgcc -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable '
    AFLAGS = ' -c ' + DEVICE + ' -x assembler-with-cpp -Wa,-mimplicit-it=thumb '
    LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,Reset_Handler -T ./linking_scripts/M55M1.ld '
    CXXFLAGS = DEVICE + ' -std=c++14 '

    M_CFLAGS = CFLAGS + ' -mlong-calls -fPIC '
    M_CXXFLAGS = CXXFLAGS + ' -mlong-calls -fPIC'
    M_LFLAGS = DEVICE + CXXFLAGS + ' -Wl,--gc-sections,-z,max-page-size=0x4' +\
                                ' -shared -fPIC -nostartfiles -nostdlib -static-libgcc'
    M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'
    #M_BIN_PATH = r'z:\fatdisk\root'

    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -O2 -gdwarf-2 -g'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2 -g'

    CXXFLAGS = CFLAGS

    POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'

elif PLATFORM == 'armclang':
    # toolchains
    CC = 'armclang'
    AS = 'armasm'
    AR = 'armar'
    LINK = 'armlink'
    TARGET_EXT = 'axf'

    DEVICE = ' --cpu Cortex-M55'
    CFLAGS = DEVICE + ' --apcs=interwork'
    AFLAGS = DEVICE
    LFLAGS = DEVICE + ' --info sizes --info totals --info unused --info veneers --list rtthread.map --scatter ./linking_scripts/M55M1.scatter'

    LFLAGS += ' --keep *.o(.rti_fn.*)   --keep *.o(FSymTab) --keep *.o(VSymTab)'

    EXEC_PATH += '/ARM/ARMCLANG/bin'
    print(EXEC_PATH)

    if BUILD == 'debug':
        CFLAGS += ' -g -O0'
        AFLAGS += ' -g'
    else:
        CFLAGS += ' -O2'

    POST_ACTION = 'fromelf --bin $TARGET --output rtthread.bin \nfromelf -z $TARGET'

def dist_handle(BSP_ROOT, dist_dir):
    import sys
    cwd_path = os.getcwd()
    sys.path.append(os.path.join(os.path.dirname(BSP_ROOT), 'tools'))
    from sdk_dist import dist_do_building
    dist_do_building(BSP_ROOT, dist_dir)
