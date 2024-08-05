OUTPATH   = build
CURRENT_DIR = $(shell pwd)

ifdef MCU_START
BOARD                 = -DAT_START_F437_V1
MK                    = -DAT32F437ZMT7
FLASH_LD              = linker/AT32F437xM_FLASH.ld
OPENOCD_TARGET_SCRIPT = /scripts/target/at32f437xM.cfg
SECTOR_SIZE           = -DSECTOR_SIZE=4096

PROJECT   = $(OUTPATH)/BOOTLOADER_AT32F435_437_MCU_START
endif

ifdef MCU_TARGETED
BOARD                 = -DCUSTOM_K3P
MK                    = -DAT32F437ZGT7
FLASH_LD              = linker/AT32F437xG_FLASH.ld
OPENOCD_TARGET_SCRIPT = /scripts/target/at32f437xG.cfg
SECTOR_SIZE           = -DSECTOR_SIZE=2048

PROJECT   = $(OUTPATH)/BOOTLOADER_AT32F435_437_MCU_TARGETED
endif

ifdef DEBUG
FLAGS_ENTRY           +=-DDEBUG
endif

SOURCES_S              =  ../device_support/startup/gcc/startup_at32f435_437.s
SOURCES_C              =  $(wildcard src/*.c)

SOURCES_POOL_LIBS      =  $(wildcard ../libs/src/*.c)
SOURCES_C              += $(SOURCES_POOL_LIBS)

#core_support havent any .c file
SOURCES_CORE_SUPPORT   =  $(wildcard ../core_support/*.c)
SOURCES_C              += $(SOURCES_CORE_SUPPORT)

SOURCES_DEVICE_SUPPORT =  $(wildcard ../device_support/*.c)
SOURCES_C              += $(SOURCES_DEVICE_SUPPORT)

SOURCES_FIRMWARE_LIB   =  $(wildcard ../firmware_lib/src/*.c)
SOURCES_C              += $(SOURCES_FIRMWARE_LIB)

#SOURCES_BOARD          =  $(wildcard ../at32f435_437_board/*.c)
#SOURCES_C              += $(SOURCES_BOARD)

SOURCES_I2C_MIDDLE     =  $(wildcard ../i2c_application_library/*.c)
SOURCES_C              += $(SOURCES_I2C_MIDDLE)

SOURCES_XPRINTF        =  $(wildcard ../xprintf/*.c)
SOURCES_C              += $(SOURCES_XPRINTF)

#SOURCES_FREERTOS       =  $(wildcard ../freertos/*.c)
#SOURCES_C              += $(SOURCES_FREERTOS)

#SOURCES_FREERTOS_PORT  =  $(wildcard ../freertos/portable/GCC/ARM_CM4F/*.c)
#SOURCES_C              += $(SOURCES_FREERTOS_PORT)

SOURCES = $(SOURCES_S) $(SOURCES_C)

OBJS    = $(SOURCES_S: .s=.o) $(SOURCES_C: .c=.o) 

INCLUDES = -Iinc

INC_POOL_LIBS       = -I../libs/inc
INCLUDES            += $(INC_POOL_LIBS)

INC_CORE_SUPPORT    = -I../core_support
INCLUDES            += $(INC_CORE_SUPPORT)

INC_DEVICE_SUPPORT  = -I../device_support
INCLUDES            += $(INC_DEVICE_SUPPORT)

INC_FIRMWARE_LIB    = -I../firmware_lib/inc
INCLUDES            += $(INC_FIRMWARE_LIB)

#INC_BOARD           = -I../at32f435_437_board
#INCLUDES            += $(INC_BOARD)

INC_I2C_MIDDLE      = -I../i2c_application_library
INCLUDES            += $(INC_I2C_MIDDLE)

INC_XPRINTF         = -I../xprintf
INCLUDES            += $(INC_XPRINTF)

#INC_FREERTOS        = -I../freertos/include
#INCLUDES            += $(INC_FREERTOS)
#INC_FREERTOS_PORT   = -I../freertos/portable/GCC/ARM_CM4F
#INCLUDES            += $(INC_FREERTOS_PORT)
#INC_FREERTOS_MEM    = -I../freertos/portable/MemMang
#INCLUDES            += $(INC_FREERTOS_MEM)

INC_TASKS           = -Isrc
INCLUDES            += $(INC_TASKS)


DEFINES = -DAT32 -DAT32F4 $(MK) $(BOARD) $(SECTOR_SIZE)#-DHEAP_SIZE=$(HEAP_SIZE)

PREFIX  = arm-none-eabi
CC      = $(PREFIX)-gcc
AS      = $(PREFIX)-as
AR      = $(PREFIX)-ar
LD      = $(PREFIX)-gcc
NM      = $(PREFIX)-nm
OBJCOPY = $(PREFIX)-objcopy
OBJDUMP = $(PREFIX)-objdump
READELF = $(PREFIX)-readelf
SIZE    = $(PREFIX)-size
GDB     = gdb-multiarch
RM      = rm -f

OPENOCD_DIR = $(PWD)/../OpenOCD

MCUFLAGS = -mcpu=cortex-m4       \
           -mlittle-endian       \
           -mfloat-abi=hard      \
           -mthumb               \
           -mfpu=fpv4-sp-d16     \
           -mno-unaligned-access

DEBUG_OPTIMIZE_FLAGS = -O0       \
                       -ggdb     \
                       -gdwarf-2
CFLAGS =  -Wall    \
          -Wextra  \
          --pedantic

CFLAGS_EXTRA = -nostartfiles       \
               -nodefaultlibs      \
               -nostdlib           \
               -fdata-sections     \
               -ffunction-sections \
               --specs=nano.specs  \
               --specs=nosys.specs
#init-array in .ld
CFLAGS +=  $(DEFINES) $(FLAGS_ENTRY) $(MCUFLAGS) $(DEBUG_OPTIMIZE_FLAGS) $(CFLAGS_EXTRA) $(INCLUDES)

LDFLAGS = -static $(MCUFLAGS) -Wl,--start-group -lgcc -lc -lg -Wl,--end-group \
          -Wl,--gc-sections -T $(FLASH_LD) 

.PHONY: dirs all clean flash erase

all: clean dirs $(PROJECT).bin $(PROJECT).asm
dirs: $(OUTPATH)
$(OUTPATH):
	mkdir -p $(OUTPATH)
clean:
#$(RM) $(OBJS) $(PROJECT).bin $(PROJECT).asm
	rm -rf $(OUTPATH)

flash: $(PROJECT).bin
	$(OPENOCD_DIR)/bin/openocd 						  		\
	-f $(OPENOCD_DIR)/scripts/interface/atlink.cfg    		\
	-f $(OPENOCD_DIR)$(OPENOCD_TARGET_SCRIPT)             	\
	-c "init; reset halt; flash write_image erase $(PROJECT).bin 0x08000000; reset; exit"

openocd-server:
	$(OPENOCD_DIR)/bin/openocd  							\
	-f $(OPENOCD_DIR)/scripts/interface/atlink.cfg 			\
	-f $(OPENOCD_DIR)$(OPENOCD_TARGET_SCRIPT)

OPENOCD_P=3333
openocd-gdb:
	$(GDB) --eval-command="target extended-remote localhost:$(OPENOCD_P)" \
	--eval-command="load" $(PROJECT).elf

$(PROJECT).elf: $(OBJS)
%.elf:
	$(LD) $(OBJS) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@
	$(SIZE) -A $@
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
%.asm: %.elf
	$(OBJDUMP) -dwh $< > $@
