#
# File: BuildEnvironment.mk
# Copyright (c) 2019, Dornerworks Ltd.
#

BUILD_DIR = ./build
CROSS_COMPILE_PREFIX = $(CROSS_COMPILE)

SDK_DIR = ./freedom-e-sdk

#-----------------------------------------------------------
GCC     = $(CROSS_COMPILE_PREFIX)-gcc
OBJCOPY = $(CROSS_COMPILE_PREFIX)-objcopy
OBJDUMP = $(CROSS_COMPILE_PREFIX)-objdump
AR      = $(CROSS_COMPILE_PREFIX)-ar
RANLIB  = $(CROSS_COMPILE_PREFIX)-ranlib
GDB     = $(CROSS_COMPILE_PREFIX)-gdb

# Target Platform: 32(sifive_e) 64(virt) 143
TARGET ?= 143
ifeq ($(TARGET),32)
LINKER_SCRIPT = $(SDK_DIR)/env/freedom-e300-hifive1/flash.lds
ARCH_FLAGS = -march=rv32imac -mabi=ilp32 -mcmodel=medany
QEMU = qemu-system-riscv32 -machine sifive_e -kernel
else
LINKER_SCRIPT = $(SDK_DIR)/env/freedom-e300-hifive1/ram.lds
ARCH_FLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany
QEMU = qemu-system-riscv64 -machine virt -kernel
endif
ARCH_FLAGS += -DTARGET=$(TARGET) -DNO_INIT

# Basic CFLAGS:
CFLAGS  = -Wall -Wextra -O3 -g3 -msmall-data-limit=8 -std=gnu11
CFLAGS += -ffunction-sections -fdata-sections -fno-builtin-printf
CFLAGS += -DDONT_USE_PLIC -DDONT_USE_M_TIME
CFLAGS += -include sys/cdefs.h
CFLAGS += $(ARCH_FLAGS)
# These flags are for outputing *.d dependency files for make
CFLAGS += -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

ASMFLAGS =  -O3 -g3
ASMFLAGS += $(ARCH_FLAGS)
ASMFLAGS += -DportasmHANDLE_INTERRUPT=handle_trap
ASMFLAGS += -msmall-data-limit=8
ASMFLAGS += -ffunction-sections -fdata-sections
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

# Linker arguments __________________________________________
LDFLAGS :=  -Xlinker --gc-sections -Xlinker --defsym=__stack_size=1K
LDFLAGS += -O3 -g3 $(ARCH_FLAGS)
LDFLAGS += -ffunction-sections -fdata-sections
ifeq ($(TARGET),32)
LDFLAGS += --specs=nano.specs
endif
LDFLAGS += -nostartfiles
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += -L../
LDFLAGS += -Wl,--start-group -Wl,--end-group
LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=open -Wl,--wrap=lseek -Wl,--wrap=read -Wl,--wrap=write
LDFLAGS += -Wl,--wrap=fstat -Wl,--wrap=stat -Wl,--wrap=close -Wl,--wrap=link -Wl,--wrap=unlink -Wl,--wrap=execve
LDFLAGS += -Wl,--wrap=fork -Wl,--wrap=getpid -Wl,--wrap=kill -Wl,--wrap=wait -Wl,--wrap=isatty -Wl,--wrap=times
LDFLAGS += -Wl,--wrap=sbrk -Wl,--wrap=_exit -Wl,--wrap=puts -Wl,--wrap=_malloc -Wl,--wrap=_free -Wl,--wrap=_open
LDFLAGS += -Wl,--wrap=_lseek -Wl,--wrap=_read -Wl,--wrap=_write -Wl,--wrap=_fstat -Wl,--wrap=_stat -Wl,--wrap=_close
LDFLAGS += -Wl,--wrap=_link -Wl,--wrap=_unlink -Wl,--wrap=_execve -Wl,--wrap=_fork -Wl,--wrap=_getpid -Wl,--wrap=_kill
LDFLAGS += -Wl,--wrap=_wait -Wl,--wrap=_isatty -Wl,--wrap=_times -Wl,--wrap=_sbrk -Wl,--wrap=__exit -Wl,--wrap=_puts
