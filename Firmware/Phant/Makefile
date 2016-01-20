#############################################################
#
# ESP32 Root Level Makefile
#
# Version 1.0
#
# (c) 2015 by CHERTS <sleuthhound@gmail.com>
#
#############################################################

BUILD_BASE	= build
FW_BASE		= firmware

# Base directory for the compiler
XTENSA_TOOLS_ROOT ?= c:/Espressif/xtensa-esp108-elf/bin

# base directory of the ESP32 SDK package, absolute
SDK_BASE	?= c:/Espressif/ESP32_RTOS_SDK
SDK_TOOLS	?= c:/Espressif/utils/ESP32

# esptool path and port
ESPTOOL ?= $(SDK_TOOLS)/esptool_esp32.exe
ESPPORT ?= COM2
# Baud rate for programmer
BAUD ?= 256000

# SPI_SPEED = 40, 26, 20, 80
SPI_SPEED ?= 40
# SPI_MODE: qio, qout, dio, dout
SPI_MODE ?= qio
# SPI_SIZE_MAP
# 0 : 512 KB (256 KB + 256 KB)
# 1 : 256 KB
# 2 : 1024 KB (512 KB + 512 KB)
# 3 : 2048 KB (512 KB + 512 KB)
# 4 : 4096 KB (512 KB + 512 KB)
# 5 : 2048 KB (1024 KB + 1024 KB)
# 6 : 4096 KB (1024 KB + 1024 KB)
SPI_SIZE_MAP ?= 1

ifeq ($(SPI_SPEED), 26.7)
    freqdiv = 1
    flashimageoptions = -ff 26m
else
    ifeq ($(SPI_SPEED), 20)
        freqdiv = 2
        flashimageoptions = -ff 20m
    else
        ifeq ($(SPI_SPEED), 80)
            freqdiv = 15
            flashimageoptions = -ff 80m
        else
            freqdiv = 0
            flashimageoptions = -ff 40m
        endif
    endif
endif

ifeq ($(SPI_MODE), QOUT)
    mode = 1
    flashimageoptions += -fm qout
else
    ifeq ($(SPI_MODE), DIO)
		mode = 2
		flashimageoptions += -fm dio
    else
        ifeq ($(SPI_MODE), DOUT)
            mode = 3
            flashimageoptions += -fm dout
        else
            mode = 0
            flashimageoptions += -fm qio
        endif
    endif
endif

ifeq ($(SPI_SIZE_MAP), 1)
  size_map = 1
  flash = 256
  flashimageoptions += -fs 2m
  blankaddr = 0xFC000
else
  ifeq ($(SPI_SIZE_MAP), 2)
    size_map = 2
    flash = 1024
    flashimageoptions += -fs 8m
    blankaddr = 0xFE000
  else
    ifeq ($(SPI_SIZE_MAP), 3)
      size_map = 3
      flash = 2048
      flashimageoptions += -fs 16m
      blankaddr = 0x1FE000
    else
      ifeq ($(SPI_SIZE_MAP), 4)
		size_map = 4
		flash = 4096
		flashimageoptions += -fs 32m
                blankaddr = 0x3FE000
      else
        ifeq ($(SPI_SIZE_MAP), 5)
          size_map = 5
          flash = 2048
          flashimageoptions += -fs 16m
          blankaddr = 0x1FE000
        else
          ifeq ($(SPI_SIZE_MAP), 6)
            size_map = 6
            flash = 4096
            flashimageoptions += -fs 32m
            blankaddr = 0x3FE000
          else
            size_map = 0
            flash = 512
            flashimageoptions += -fs 4m
            blankaddr = 0x7E000
          endif
        endif
      endif
    endif
  endif
endif

# name for the target project
TARGET = app

# which modules (subdirectories) of the project to include in compiling
MODULES	= user phant
EXTRA_INCDIR = include $(SDK_BASE)/extra_include

# libraries used in this project, mainly provided by the SDK
LIBS = c gcc hal m crypto freertos lwip main net80211 phy pp rtc wpa driver

# compiler flags using during compilation of source files
CFLAGS = -Os -g -O2 -std=gnu90 -Wpointer-arith -Wundef -Werror -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -mno-serialize-volatile -D__ets__ -DICACHE_FLASH

# linker flags used to generate the main object file
LDFLAGS = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

# linker script used for the above linkier step
LD_SCRIPT = eagle.pro.v7.ld

# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SDK_INCDIR	= include include/espressif include/lwip include/lwip/ipv4 include/lwip/ipv6 driver_lib/include

# select which tools to use as compiler, librarian and linker
CC	:= $(XTENSA_TOOLS_ROOT)/xtensa-esp108-elf-gcc
AR	:= $(XTENSA_TOOLS_ROOT)/xtensa-esp108-elf-ar
LD	:= $(XTENSA_TOOLS_ROOT)/xtensa-esp108-elf-gcc
OBJCOPY := $(XTENSA_TOOLS_ROOT)/xtensa-esp108-elf-objcopy
OBJDUMP := $(XTENSA_TOOLS_ROOT)/xtensa-esp108-elf-objdump

# no user configurable options below here
SRC_DIR		:= $(MODULES)
BUILD_DIR	:= $(addprefix $(BUILD_BASE)/,$(MODULES))
SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))
SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
LIBS		:= $(addprefix -l,$(LIBS))
APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)
LD_SCRIPT	:= $(addprefix -T$(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))
INCDIR		:= $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(EXTRA_INCDIR))
MODULE_INCDIR	:= $(addsuffix /include,$(INCDIR))

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

vpath %.c $(SRC_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CFLAGS)  -c $$< -o $$@
endef

.PHONY: all checkdirs clean flash rebuild

all: checkdirs $(TARGET_OUT)

$(TARGET_OUT): $(APP_AR)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(LIBS) $(APP_AR) -Wl,--end-group -o $@
	$(vecho) "Run objcopy, please wait..."
	$(Q) $(OBJCOPY) --only-section .text -O binary $@ eagle.app.v7.text.bin
	$(Q) $(OBJCOPY) --only-section .data -O binary $@ eagle.app.v7.data.bin
	$(Q) $(OBJCOPY) --only-section .rodata -O binary $@ eagle.app.v7.rodata.bin
	$(Q) $(OBJCOPY) --only-section .irom0.text -O binary $@ eagle.app.v7.irom0text.bin
	$(Q) $(OBJCOPY) --only-section .irom1.text -O binary $@ eagle.app.v7.irom1text.bin
	$(Q) rm -f irom0_flash.bin irom1.bin
	$(vecho) "objcopy done"
	$(vecho) "Run gen_appbin_esp32.exe"
	$(Q) $(SDK_TOOLS)/gen_appbin_esp32.exe $@ $(SDK_BASE)/ld
	$(Q) mv eagle.app.flash.bin $(FW_BASE)/$(TARGET).ota
	$(Q) mv irom1.bin $(FW_BASE)/irom1.bin &>/dev/null
	$(Q) mv irom0_flash.bin $(FW_BASE)/irom0_flash.bin &>/dev/null
	$(Q) rm eagle.app.v7.*
	$(vecho) "------------------------------------------------------------------------------"
	$(vecho) "Generate related files successully in folder $(FW_BASE)"	
	$(vecho) "boot.bin ------------------>0x00000"
	$(vecho) "irom1.bin ----------------->0x04000" 
	$(vecho) "irom0_flash.bin ----------->0x40000"
	$(vecho) "blank.bin ----------------->256K-0xFC000, 1MB-0xFE000, 2MB-0x1FE000"
	$(vecho) "user.ota ------------------>(used for OTA)"
	$(vecho) "Done"

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^

checkdirs: $(BUILD_DIR) $(FW_BASE)

$(BUILD_DIR):
	$(Q) mkdir -p $@

$(FW_BASE):
	$(Q) mkdir -p $@

flash: all
	$(ESPTOOL) -p $(ESPPORT) -b $(BAUD) write_flash $(flashimageoptions) 0x00000 $(SDK_BASE)/bin/boot.bin 0x04000 $(FW_BASE)/irom1.bin 0x40000 $(FW_BASE)/irom0_flash.bin

rebuild: clean all

clean:
	$(Q) rm -f $(APP_AR)
	$(Q) rm -f $(TARGET_OUT)
	$(Q) rm -rf $(BUILD_DIR)
	$(Q) rm -rf $(BUILD_BASE)
	$(Q) rm -rf $(FW_BASE)

$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))
