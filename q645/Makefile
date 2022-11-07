PREFIX = ../../crossgcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-
CC = $(PREFIX)gcc
CPP = $(PREFIX)cpp


TARGET := m4
BIN := $(TARGET).bin

BUILD_DIR := build

# trace level define in Inc/trace.h
CFLAGS :=-DTRACE_LEVE=4

FREERTOS_DIR_REL := Libraries/FreeRTOS
FREERTOS_SRC := $(abspath $(FREERTOS_DIR_REL))
CMSIS_PATH = Libraries/CMSIS
CMSIS_LIB = $(CMSIS_PATH)/Device/SP645
SP645_LIB = Libraries/Driver
USER_SRC_PATH = User
CLI_LIB = Libraries/FreeRTOS-Plus-CLI
CLI_DEMO_PATH = FreeRTOS_Plus_CLI_Demos

# system C file
source_FILES += Libraries/System/syscall.c
# CMSIS C file
source_FILES += $(CMSIS_LIB)/source/startup.c
source_FILES += $(CMSIS_LIB)/source/system_sp645.c
# Freertos C file
source_FILES += $(FREERTOS_SRC)/portable/GCC/ARM_CM4F/port.c
source_FILES += $(FREERTOS_SRC)/tasks.c
source_FILES += $(FREERTOS_SRC)/list.c
source_FILES += $(FREERTOS_SRC)/queue.c
source_FILES += $(FREERTOS_SRC)/timers.c
source_FILES += $(FREERTOS_SRC)/event_groups.c
source_FILES += ${FREERTOS_SRC}/portable/MemMang/heap_3.c
# driver C file
source_FILES += $(SP645_LIB)/source/sp645_hal.c
source_FILES += $(SP645_LIB)/source/sp645_hal_cortex.c
source_FILES += $(SP645_LIB)/source/sp645_hal_stc.c
source_FILES += $(SP645_LIB)/source/sp645_hal_timer.c
source_FILES += $(SP645_LIB)/source/sp645_hal_gpio.c
source_FILES += $(SP645_LIB)/source/sp645_hal_uart.c
# src C file
#source_FILES += $(USER_SRC_PATH)/source/main.c
source_FILES += $(USER_SRC_PATH)/source/sp645_it.c
source_FILES += $(USER_SRC_PATH)/source/main_blinky.c
source_FILES += $(USER_SRC_PATH)/source/trace.c
source_FILES += $(USER_SRC_PATH)/source/serial.c
# CLI c file
source_FILES += $(CLI_LIB)/FreeRTOS_CLI.c
source_FILES += $(CLI_DEMO_PATH)/Sample-CLI-commands.c
source_FILES += $(CLI_DEMO_PATH)/UARTCommandConsole.c

# *.h
include_DIRS += -I.
include_DIRS += -I$(USER_SRC_PATH)/include
include_DIRS += -I$(CMSIS_PATH)/include
include_DIRS += -I$(CMSIS_LIB)/include
include_DIRS += -I$(SP645_LIB)/include
include_DIRS += -I$(FREERTOS_SRC)/include
include_DIRS += -I$(FREERTOS_SRC)/portable/GCC/ARM_CM4F
include_DIRS += -I$(CLI_LIB)

CFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

DEFINES := -DHEAP3

LD_SRC = $(CMSIS_LIB)/source/sp645_m4.ldi
LD_FILE = $(CMSIS_LIB)/source/sp645_m4.ld

CONFIG_ROOT= ../../.config
DTS_CONFIG_ROOT= ../../linux/kernel/arch/arm64/boot/dts
LINUX_DTS=$(shell cat $(CONFIG_ROOT) | grep 'LINUX_DTB=' | sed 's/LINUX_DTB=//g').dts

M4_DTS_DATA_TEMP1=$(shell sed -n -e '/rproc_0_reserved:/,/reg/p' $(DTS_CONFIG_ROOT)/$(LINUX_DTS) | sed -n -e '/reg/p')
M4_DTS_DATA_TEMP2=$(shell echo "$(M4_DTS_DATA_TEMP1)" | sed -n 's/^.*\([0-9a-fA-F]\{8\}\).*$$/\1/p' )
M4_MEM=0x$(M4_DTS_DATA_TEMP2)

CFLAGS += -DM4_RUN_ADDRESS=$(M4_MEM)

LDFLAGS = -T $(LD_FILE) -specs=nano.specs --specs=rdimon.specs -lc -lrdimon
LDFLAGS += -Xlinker -Map=${BUILD_DIR}/$(TARGET).map

CFLAGS += -nostartfiles -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Wno-error=implicit-function-declaration
CFLAGS += -Wno-builtin-declaration-mismatch -Werror

ifeq ($(DEBUG), 1)
    CFLAGS += -ggdb3 -Og
else
    CFLAGS += -O3
endif
CFLAGS += -fstrict-aliasing -Wstrict-aliasing -Wno-error=address-of-packed-member

OBJ_FILES := $(source_FILES:%.c=$(BUILD_DIR)/%.o)
OBJ_FILES += $(BUILD_DIR)/main.o

CPPFLAGS += $(DEFINES)
CFLAGS += $(include_DIRS)

DATE_COMPILE = $(shell date --rfc-3339=second)
#CFLAGS += -DDATE_COMPILE='"$(DATE_COMPILE)"'

.PHONY: clean

$(BUILD_DIR)/$(BIN) : $(OBJ_FILES)
	$(CPP) -P $(CFLAGS) $(LD_SRC) $(LD_FILE)
	$(CC) -ffunction-sections -fdata-sections $(CFLAGS) $(LDFLAGS) $+ -o $(BUILD_DIR)/$(TARGET)
	$(PREFIX)objdump -d -S $(BUILD_DIR)/$(TARGET) > $(BUILD_DIR)/$(TARGET).dis
	$(PREFIX)objcopy -O binary -S $(BUILD_DIR)/$(TARGET) $@
ifeq ($(PACK_HERE), 1)
	@rm -rf bin/zmem*
	@hexdump -v -e '1/4 "%08x\n"' $@ > temp.hex
	@python ../../ipack/tools/bin2zmem/bin2zmem_ddr4.py temp.hex $(M4_MEM)
	@rm -rf temp.hex
	@cd bin && tar caf zmem.tar.bz2 zmem*.hex && ls -lh zmem.tar.bz2
endif
	@echo "build done !!!!!"
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

includeS := $(source_FILES:%.c=$(BUILD_DIR)/%.d)
-include $(includeS)

${BUILD_DIR}/main.o : $(USER_SRC_PATH)/source/main.c Makefile
	-mkdir -p $(@D)
#	data_compile=`date --rfc-3339=second`;
	$(CC) -DDATE_COMPILE='"$(DATE_COMPILE)"' $(CFLAGS) $(CPPFLAGS) -MMD -c $< -o $@

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -c $< -o $@

clean:
	@-rm -rf $(BUILD_DIR)

