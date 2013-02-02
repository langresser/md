include $(IMAGINE_PATH)/make/iOS-metadata.mk
iOS_targetPath := target/iOS
ios_bundleDirectory = $(iOS_metadata_bundleName).app
iOS_appBundlePath := $(iOS_targetPath)/Applications/$(ios_bundleDirectory)
iOS_deviceAppBundlePath := /Applications/$(ios_bundleDirectory)
iOS_deviceExecPath := $(iOS_deviceAppBundlePath)/$(iOS_metadata_exec)
ios_resourcePath := res/ios
ios_iconPath := res/icons/iOS
iOS_plistTxt := $(iOS_targetPath)/Info.txt
iOS_plist := $(ios_resourcePath)/Info.plist

# Host/IP of the iOS device to install the app over SSH
ios_installHost := iphone4s

ifndef config_iOS_noArmv6

iOS_armv6Exec := $(iOS_targetPath)/bin-debug/$(iOS_metadata_exec)-armv6
ios-armv6 :
	$(MAKE) -f ios-armv6.mk
$(iOS_armv6Exec) : ios-armv6

iOS_armv6ReleaseExec := $(iOS_targetPath)/bin-release/$(iOS_metadata_exec)-armv6
ios-armv6-release :
	$(MAKE) -f ios-armv6-release.mk
$(iOS_armv6ReleaseExec) : ios-armv6-release

ios-armv6-install : $(iOS_armv6Exec)
	ssh root@$(ios_installHost) rm -f $(iOS_deviceExecPath)
	scp $(iOS_armv6Exec) root@$(ios_installHost):$(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod a+x $(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod gu+s $(iOS_deviceExecPath)

endif

ifndef config_iOS_noArmv7

iOS_armv7Exec := $(iOS_targetPath)/bin-debug/$(iOS_metadata_exec)-armv7
ios-armv7 :
	$(MAKE) -f ios-armv7.mk
$(iOS_armv7Exec) : ios-armv7

iOS_armv7ReleaseExec := $(iOS_targetPath)/bin-release/$(iOS_metadata_exec)-armv7
ios-armv7-release :
	$(MAKE) -f ios-armv7-release.mk
$(iOS_armv7ReleaseExec) : ios-armv7-release 

ios-armv7-install : $(iOS_armv7Exec)
	ssh root@$(ios_installHost) rm -f $(iOS_deviceExecPath)
	scp $(iOS_armv7Exec) root@$(ios_installHost):$(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod a+x $(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod gu+s $(iOS_deviceExecPath)

endif

ios-build : $(iOS_armv6Exec) $(iOS_armv7Exec)

ios-release-build : $(iOS_armv6ReleaseExec) $(iOS_armv7ReleaseExec)

ios_fatExec := $(iOS_targetPath)/bin-debug/$(iOS_metadata_exec)

$(ios_fatExec) : $(iOS_armv6Exec) $(iOS_armv7Exec)
	@mkdir -p $(@D)
	lipo -create $^ -output $@

ios_fatReleaseExec := $(iOS_targetPath)/bin-release/$(iOS_metadata_exec)

$(ios_fatReleaseExec) : $(iOS_armv6ReleaseExec) $(iOS_armv7ReleaseExec)
	@mkdir -p $(@D)
	lipo -create $^ -output $@

ifdef iOS_metadata_setuid

ios_setuidLauncher := $(ios_resourcePath)/$(iOS_metadata_exec)_

$(ios_setuidLauncher) :
	echo '#!/bin/bash' > $@
	echo 'dir=$$(dirname "$$0")' >> $@
	echo 'exec "$${dir}"/$(iOS_metadata_exec) "$$@"' >> $@
	chmod a+x $@

endif

ios-resources-install : $(iOS_plist) $(ios_setuidLauncher)
	ssh root@$(ios_installHost) mkdir -p $(iOS_deviceAppBundlePath)
	scp $(ios_resourcePath)/* root@$(ios_installHost):$(iOS_deviceAppBundlePath)/
	scp $(ios_iconPath)/* root@$(ios_installHost):$(iOS_deviceAppBundlePath)/
	ssh root@$(ios_installHost) chmod -R a+r $(iOS_deviceAppBundlePath)

ios-install : $(ios_fatExec)
	ssh root@$(ios_installHost) rm -f $(iOS_deviceExecPath)
	scp $< root@$(ios_installHost):$(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod a+x $(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod gu+s $(iOS_deviceExecPath)

ios-release-install : $(ios_fatReleaseExec)
	ssh root@$(ios_installHost) rm -f $(iOS_deviceExecPath)
	scp $< root@$(ios_installHost):$(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod a+x $(iOS_deviceExecPath)
	ssh root@$(ios_installHost) chmod gu+s $(iOS_deviceExecPath)

# metadata

$(iOS_plistTxt) : metadata/conf.mk $(metadata_confDeps)
	bash $(IMAGINE_PATH)/tools/genIOSMeta.sh $(iOS_gen_metadata_args) $@
$(iOS_plist) : $(iOS_plistTxt)
	@mkdir -p $(@D)
	plutil -convert binary1 -o $@ $<
ios-metadata : $(iOS_plist)

# tar package

# Note: a version of tar with proper --transform support is needed for this rule
iOS_tar := $(iOS_targetPath)/$(iOS_metadata_bundleName)-$(iOS_metadata_version)-iOS.tar.gz
$(iOS_tar) : # depends on $(ios_fatReleaseExec) $(iOS_plist) $(ios_setuidLauncher)
	chmod a+x $(ios_fatReleaseExec)
	chmod gu+s $(ios_fatReleaseExec)
	tar -chzf $@ $(ios_fatReleaseExec) $(ios_resourcePath)/* $(ios_iconPath)/* \
	--transform='s,^$(iOS_targetPath)/bin-release/,$(ios_bundleDirectory)/,;s,^$(ios_resourcePath)/,$(ios_bundleDirectory)/,;s,^$(ios_iconPath)/,$(ios_bundleDirectory)/,'
ios-release-tar : $(iOS_tar)

ios-release-ready : $(iOS_tar)
	cp $(iOS_tar) ../releases-bin/iOS

ios-release-check :
	@echo "Checking compiled release version of $(iOS_metadata_bundleName) $(iOS_metadata_version)"
	strings $(iOS_appBundlePath)/exec | grep " $(iOS_metadata_version)"

.PHONY: ios-armv6 ios-armv7 ios-armv6-release ios-armv7-release ios-metadata ios-release-build ios-armv7-install \
 ios-armv6-install ios-release-install ios-release-tar ios-release-ready ios-release-check ios-resources-install
