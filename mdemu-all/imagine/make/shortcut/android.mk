include $(IMAGINE_PATH)/make/android-metadata.mk

ifndef android_minSDK 
 android_minSDK := 9
endif

ifeq ($(android_minSDK), 4)
 config_android_noArmv7=1 # only build ARMv6 for older Android OS
endif

ifndef android_targetSDK
 android_targetSDK := $(android_minSDK)
 ifeq ($(android_minSDK), 5)
  android_targetSDK := 8
 endif
endif

android_hasSDK9 := $(shell expr $(android_minSDK) \>= 9)

ifeq ($(android_hasSDK9), 1)
 android_soName := main
else
 android_soName := imagine
endif

android_targetPath := target/android-$(android_minSDK)

# metadata

android_manifestXml := $(android_targetPath)/AndroidManifest.xml
$(android_manifestXml) : metadata/conf.mk $(metadata_confDeps)
	@mkdir -p $(@D)
	bash $(IMAGINE_PATH)/tools/genAndroidMeta.sh $(android_gen_metadata_args) --min-sdk=$(android_minSDK) $@
android-metadata : $(android_manifestXml)

# project dir

# user-specified ant.properties
ifdef ANDROID_ANT_PROPERTIES

android_antProperties := $(android_targetPath)/ant.properties

$(android_antProperties) :
	@mkdir -p $(@D)
	ln -s $(ANDROID_ANT_PROPERTIES) $@

endif 

ifneq ($(wildcard res/android/assets),)
android_assetsPath := $(android_targetPath)/assets
$(android_assetsPath) :
	@mkdir -p $(@D)
	ln -s ../../res/android/assets $@
endif

ifneq ($(wildcard res/icons/icon-48.png),)
android_drawableMdpiIconPath := $(android_targetPath)/res/drawable-mdpi/icon.png
$(android_drawableMdpiIconPath) :
	@mkdir -p $(@D)
	ln -s ../../../../res/icons/icon-48.png $@
endif

ifneq ($(wildcard res/icons/icon-72.png),)
android_drawableHdpiIconPath := $(android_targetPath)/res/drawable-hdpi/icon.png
$(android_drawableHdpiIconPath) :
	@mkdir -p $(@D)
	ln -s ../../../../res/icons/icon-72.png $@
endif

ifneq ($(wildcard res/icons/icon-96.png),)
android_drawableXhdpiIconPath := $(android_targetPath)/res/drawable-xhdpi/icon.png
$(android_drawableXhdpiIconPath) :
	@mkdir -p $(@D)
	ln -s ../../../../res/icons/icon-96.png $@
endif

android_drawableIconPaths := $(android_drawableMdpiIconPath) $(android_drawableHdpiIconPath)
ifeq ($(android_hasSDK9), 1)
 android_drawableIconPaths += $(android_drawableXhdpiIconPath)
endif

android_imagineJavaSrcPath := $(android_targetPath)/src/com/imagine

$(android_imagineJavaSrcPath) :
	@mkdir -p $(@D)
	ln -s $(IMAGINE_PATH)/src/base/android/java/sdk-$(android_minSDK)/imagine $@

android_stringsXml := $(android_targetPath)/res/values/strings.xml

$(android_stringsXml) : metadata/conf.mk
	@mkdir -p $(@D)
	echo -e "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<resources>\n\t<string name=\"app_name\">$(android_metadata_name)</string>\n</resources>" > $@

android_buildXml := $(android_targetPath)/build.xml

$(android_buildXml) : | $(android_manifestXml) $(android_stringsXml) $(android_imagineJavaSrcPath) \
$(android_drawableIconPaths) $(android_assetsPath) $(android_antProperties)
	android update project -p $(@D) -n $(android_metadata_project) -t android-$(android_targetSDK)
	rm $(@D)/proguard.cfg

ifneq ($(wildcard res/android/proguard.cfg),)
android_proguardConfPath := $(android_targetPath)/proguard.cfg
$(android_proguardConfPath) : | $(android_buildXml)
	@mkdir -p $(@D)
	ln -s ../../res/android/proguard.cfg $@
endif

# native libs

ifndef config_android_noArmv6

android_armv6SOPath := $(android_targetPath)/lib-debug/armeabi/lib$(android_soName).so
android-armv6 :
	$(MAKE) -j3 -f android-$(android_minSDK)-armv6.mk
$(android_armv6SOPath) : android-armv6

android_armv6ReleaseSOPath := $(android_targetPath)/lib-release/armeabi/lib$(android_soName).so
android-armv6-release :
	$(MAKE) -j3 -f android-$(android_minSDK)-armv6-release.mk
$(android_armv6ReleaseSOPath) : android-armv6-release

endif

ifndef config_android_noArmv7

android_armv7SOPath := $(android_targetPath)/libs-debug/armeabi-v7a/lib$(android_soName).so
android-armv7 :
	$(MAKE) -j3 -f android-$(android_minSDK)-armv7.mk
$(android_armv7SOPath) : android-armv7

android_armv7ReleaseSOPath := $(android_targetPath)/libs-release/armeabi-v7a/lib$(android_soName).so
android-armv7-release :
	$(MAKE) -j3 -f android-$(android_minSDK)-armv7-release.mk
$(android_armv7ReleaseSOPath) : android-armv7-release

endif

android-release : $(android_armv6ReleaseSOPath) $(android_armv7ReleaseSOPath)

# apks

ifdef V
 antVerbose := -verbose
endif

android_projectDeps := $(android_buildXml) $(android_proguardConfPath)

ifndef android_antTarget
 android_antTarget := release
endif

android_apkPath := $(android_targetPath)/bin-debug/$(android_metadata_project)-$(android_antTarget).apk
android-apk : $(android_projectDeps) $(android_armv7SOPath) $(android_armv6SOPath)
	rm -f $(android_targetPath)/bin-debug/$(android_metadata_project)-$(android_antTarget)-unsigned.apk.d
	rm -f $(android_targetPath)/bin-debug/$(android_metadata_project).ap_.d
	rm -f $(android_targetPath)/bin-debug/classes.dex.d
	cd $(android_targetPath) && ANT_OPTS=-Dimagine.path=$(IMAGINE_PATH) ant $(antVerbose) -Dout.dir=bin-debug \
-Dnative.libs.absolute.dir=libs-debug -Djar.libs.dir=libs-debug $(android_antTarget)

android-install : android-apk
	adb install -r $(android_apkPath)

android-install-only :
	adb install -r $(android_apkPath)

android_apkReleasePath := $(android_targetPath)/bin-release/$(android_metadata_project)-$(android_antTarget).apk
android-release-apk : $(android_projectDeps) $(android_armv7ReleaseSOPath) $(android_armv6ReleaseSOPath)
	rm -f $(android_targetPath)/bin-release/$(android_metadata_project)-$(android_antTarget)-unsigned.apk.d
	rm -f $(android_targetPath)/bin-release/$(android_metadata_project).ap_.d
	rm -f $(android_targetPath)/bin-release/classes.dex.d
	cd $(android_targetPath) && ANT_OPTS=-Dimagine.path=$(IMAGINE_PATH) ant $(antVerbose) -Dout.dir=bin-release \
-Dnative.libs.absolute.dir=libs-release -Djar.libs.dir=libs-debug $(android_antTarget)

android-release-install : android-release-apk
	adb install -r $(android_apkReleasePath)

android-release-install-only :
	adb install -r $(android_apkReleasePath)

android-release-ready : 
	cp $(android_apkReleasePath) ../releases-bin/android/$(android_metadata_project)-$(android_minSDK)-$(android_metadata_version).apk

android-check :
	@echo "Checking compiled debug version of $(android_metadata_project) (SDK $(android_minSDK)) $(android_metadata_version)"
	@strings $(android_targetPath)/libs-debug/*/*.so | grep " $(android_metadata_version)"

android-release-check :
	@echo "Checking compiled release version of $(android_metadata_project) (SDK $(android_minSDK)) $(android_metadata_version)"
	@strings $(android_targetPath)/libs-release/*/*.so | grep " $(android_metadata_version)"

.PHONY: android-metadata  \
android-armv6 android-armv7 android-armv6-release android-armv7-release android-release \
android-apk android-install android-install-only \
android-release-apk android-release-install android-release-install-only \
android-release-ready \
android-check android-release-check
