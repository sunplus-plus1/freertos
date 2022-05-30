PREFIX = ../../../../crossgcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-
TOPDIR = $(abspath .)


CPP = $(PREFIX)cpp
CC = $(PREFIX)gcc
TARGET := m4
BIN := $(TARGET).bin

BUILD_DIR := build

# trace level define in util/trace.h
CFLAGS :=-DTRACE_LEVE=4

FREERTOS_DIR_REL := ../../../FreeRTOS
FREERTOS_DIR := $(abspath $(FREERTOS_DIR_REL))
KERNEL_DIR := $(FREERTOS_DIR)/Source
UTIL_DIR := util

FREERTOS_PLUS_DIR_REL := ../../../FreeRTOS-Plus
FREERTOS_PLUS_DIR := $(abspath $(FREERTOS_PLUS_DIR_REL))

SOURCE_FILES += init/startup.c  syscall.c main.c
SOURCE_FILES += $(KERNEL_DIR)/portable/GCC/ARM_CM4F/port.c
SOURCE_FILES += $(KERNEL_DIR)/tasks.c
SOURCE_FILES += $(KERNEL_DIR)/list.c
SOURCE_FILES += $(KERNEL_DIR)/queue.c
SOURCE_FILES += $(KERNEL_DIR)/timers.c
SOURCE_FILES += $(KERNEL_DIR)/event_groups.c
SOURCE_FILES += ${KERNEL_DIR}/portable/MemMang/heap_3.c

INCLUDE_DIRS += -I.
INCLUDE_DIRS += -ICMSIS
INCLUDE_DIRS += -I$(FREERTOS_DIR)/Demo/Common/include
INCLUDE_DIRS += -I$(KERNEL_DIR)/include
INCLUDE_DIRS += -I$(KERNEL_DIR)/portable/GCC/ARM_CM4F
INCLUDE_DIRS += -I$(UTIL_DIR)

SOURCE_FILES += main_blinky.c
SOURCE_FILES += $(UTIL_DIR)/trace.c
CFLAGS += -DmainCREATE_SIMPLE_BLINKY_DEMO_ONLY=1

DEFINES := -DHEAP3

LD_SRC = ./scripts/q645_m4.ldi
LD_FILE = ./scripts/q645_m4.ld

CONFIG_ROOT=$(TOPDIR)/../../../../.config
DTS_CONFIG_ROOT=$(TOPDIR)/../../../../linux/kernel/arch/arm64/boot/dts
LINUX_DTS=$(shell cat $(CONFIG_ROOT) | grep 'LINUX_DTB=' | sed 's/LINUX_DTB=//g').dts
M4_DTS_DATA_TEMP1=$(shell sed -n -e '/rproc_0_reserved:/,/reg/p' $(DTS_CONFIG_ROOT)/$(LINUX_DTS) | sed -n -e '/reg/p')
M4_DTS_DATA_TEMP2=$(shell echo "$(M4_DTS_DATA_TEMP1)"  | cut -d "x" -f 2 | cut -c 1-8)
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

OBJ_FILES := $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

CPPFLAGS += $(DEFINES)
CFLAGS += $(INCLUDE_DIRS)

.PHONY: clean

$(BUILD_DIR)/$(BIN) : $(OBJ_FILES)
	$(CPP) -P $(CFLAGS) $(LD_SRC) $(LD_FILE)
	$(CC) -ffunction-sections -fdata-sections $(CFLAGS) $(LDFLAGS) $+ -o $(BUILD_DIR)/$(TARGET)
	$(PREFIX)objdump -d -S $(BUILD_DIR)/$(TARGET) > $(BUILD_DIR)/$(TARGET).dis
	$(PREFIX)objcopy -O binary -S $(BUILD_DIR)/$(TARGET) $@
	@rm -rf bin/zmem*
	@hexdump -v -e '1/4 "%08x\n"' $@ > temp.hex
	@python ../../../../ipack/tools/bin2zmem/bin2zmem_ddr4.py temp.hex $(M4_MEM)
	@rm -rf temp.hex
	@cd bin && tar caf zmem.tar.bz2 zmem*.hex && ls -lh zmem.tar.bz2

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

INCLUDES := $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.d)
-include $(INCLUDES)

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -c $< -o $@

clean:
	-rm -rf $(BUILD_DIR)

