# general Android ARM setup, included by ARMv6/ARMv7 makefiles
ifndef android_minSDK 
 android_minSDK := 9
endif
android_ndkArch := arm

ifeq ($(origin CC), default)
 CC := arm-linux-androideabi-gcc
endif

android_cpuFlags += -mthumb-interwork
CPPFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__
COMPILE_FLAGS += -fpic -fno-short-enums

include $(currPath)/android-gcc.mk
