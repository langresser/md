ifndef inc_main
inc_main := 1

include $(IMAGINE_PATH)/make/imagineAppBase.mk

SRC += main/Main.cc

include ../EmuFramework/common.mk

CPPFLAGS += -DSUPPORT_16BPP_RENDER -DLSB_FIRST \
-DSysDDec=float -DSysLDDec=float -DNO_SYSTEM_PICO -DNO_SCD
# -DNO_SVP -DNO_SYSTEM_PBC

GPLUS := genplus-gx

# Genesis Plus includes
CPPFLAGS += -Isrc/$(GPLUS) -Isrc/$(GPLUS)/m68k -Isrc/$(GPLUS)/z80 -Isrc/$(GPLUS)/input_hw \
-Isrc/$(GPLUS)/sound -Isrc/$(GPLUS)/cart_hw -Isrc/$(GPLUS)/cart_hw/svp

# Genesis Plus sources
GPLUS_SRC += $(GPLUS)/system.cc $(GPLUS)/genesis.cc $(GPLUS)/io_ctrl.cc $(GPLUS)/loadrom.cc \
$(GPLUS)/mem68k.cc $(GPLUS)/membnk.cc $(GPLUS)/memz80.cc $(GPLUS)/state.cc $(GPLUS)/vdp_ctrl.cc \
$(GPLUS)/vdp_render.cc

#ifeq ($(ENV), android)
 #GPLUS_SRC += $(GPLUS)/m68k/cyclone/Cyclone.s $(GPLUS)/m68k/cyclone/m68k.cc
#else
 GPLUS_SRC += $(GPLUS)/m68k/musashi/m68kcpu.cc
#endif

GPLUS_SRC += $(GPLUS)/z80/z80.cc

GPLUS_SRC += $(GPLUS)/sound/sound.cc $(GPLUS)/sound/ym2612.cc $(GPLUS)/sound/Fir_Resampler.cc \
$(GPLUS)/sound/ym2413.cc \
$(GPLUS)/sound/sn76489.cc $(GPLUS)/sound/blip.cc #$(GPLUS)/sound/eq.c

GPLUS_SRC += $(GPLUS)/cart_hw/eeprom.cc $(GPLUS)/cart_hw/areplay.cc $(GPLUS)/cart_hw/ggenie.cc \
$(GPLUS)/cart_hw/md_cart.cc $(GPLUS)/cart_hw/sram.cc \
$(GPLUS)/cart_hw/svp/svp.cc $(GPLUS)/cart_hw/svp/ssp16.cc \
$(GPLUS)/cart_hw/sms_cart.cc

GPLUS_SRC += $(GPLUS)/input_hw/input.cc $(GPLUS)/input_hw/activator.cc $(GPLUS)/input_hw/gamepad.cc \
$(GPLUS)/input_hw/lightgun.cc $(GPLUS)/input_hw/mouse.cc $(GPLUS)/input_hw/teamplayer.cc \
$(GPLUS)/input_hw/xe_a1p.cc \
$(GPLUS)/input_hw/sportspad.cc $(GPLUS)/input_hw/paddle.cc

#GPLUS_SRC += scd/scd.cc scd/LC89510.cc scd/cd_sys.cc scd/buffering.cc scd/gfx_cd.cc scd/pcm.cc

GPLUS_OBJ := $(addprefix $(objDir)/,$(GPLUS_SRC:.cc=.o))

SRC += $(GPLUS_SRC) fileio/fileio.cc #scd/cd_file.cc

include $(IMAGINE_PATH)/make/package/unzip.mk

ifeq ($(ENV), iOS)
configDefs += CONFIG_AUDIO_COREAUDIO_MED_BUFFER_FRAMES
include $(IMAGINE_PATH)/make/package/stdc++.mk
endif

ifndef target
target := mdemu
endif

OTHER_ALL_OBJ = $(GPLUS_OBJ)

include $(IMAGINE_PATH)/make/imagineAppTarget.mk

endif
