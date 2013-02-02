CPPFLAGS += -I$(genPath) -Isrc -I$(imagineSrcDir)
VPATH += src $(imagineSrcDir)

genConfigH = $(genPath)/config.h
genMetaH = $(genPath)/meta.h

.SUFFIXES: 
.PHONY: all main config metadata-header
all : $(genConfigH) $(genMetaH) main

# config.h is only built if not present, needs manual deletion to update
$(genConfigH) :
	@echo "Generating Config $@"
	@mkdir -p $(@D)
	$(PRINT_CMD)bash $(IMAGINE_PATH)/make/writeConfig.sh $@ "$(configDefs)" "$(configInc)"
config : $(genConfigH)

include metadata/conf.mk
$(genMetaH) :
	@echo "Generating Metadata Header $@"
	@mkdir -p $(@D)
	echo \#define CONFIG_APP_NAME \"$(metadata_name)\" > $@
metadata-header : $(genMetaH)

ifndef NO_LOGGER

include $(imagineSrcDir)/logger/system.mk

endif
