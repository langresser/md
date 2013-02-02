include $(dir $(abspath $(lastword $(MAKEFILE_LIST))))config.mk

webos_cpuFlags := -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp #-mthumb #-march=armv7-a

include $(currPath)/webos-gcc.mk

ifndef target
 target := armv7
endif

system_externalSysroot := $(IMAGINE_PATH)/bundle/webos/armv7
CPPFLAGS += -I$(system_externalSysroot)/include
LDLIBS += -L$(system_externalSysroot)/lib
