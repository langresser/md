ifndef inc_base
inc_base := 1

include $(IMAGINE_PATH)/make/package/sdl.mk

configDefs += CONFIG_BASE_SDL CONFIG_INPUT

SRC += base/sdl/main.cc

endif
