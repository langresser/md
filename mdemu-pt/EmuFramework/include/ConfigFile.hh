/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#pragma once

#include <util/number.h>

static void readKeyConfig2(Io *io, uint buttonIdx, /*uint cat,*/uint readSize)
{
	uint8 numBtns;
	io->readVar(&numBtns);
	logMsg("key config for idx %d, %d btns", buttonIdx, numBtns);
	readSize--;
	if(readSize != numBtns*4U)
	{
		logMsg("skipping %d byte key config, expected %d", readSize, numBtns*4);
		io->seekRel(readSize);
		return;
	}
	iterateTimes(numBtns, i)
	{
		uint16 devId;
		io->readVar(&devId);
		uint16 savedButton;
		io->readVar(&savedButton);

		if(!devId)
			devId = Config::envIsPS3 ? InputEvent::DEV_PS3PAD : InputEvent::DEV_KEYBOARD;
		if(savedButton >= Input::devButtonCount(devId))
			continue;

		using namespace EmuControls;
		keyConfig.key(devId)[buttonIdx] = savedButton;
		logMsg("read %s key idx %d, %s", InputEvent::devTypeName(devId),
			buttonIdx, Input::buttonName(devId, savedButton));
	}
}

static void writeKeyConfig2(Io *io, uint buttonIdx, uint16 cfgKey)
{
	int keyDiff = 0;

	// find how many device keys differ from base default
	forEachDInArray(supportedInputDev, dev)
	{
		assert(buttonIdx < keyConfig.totalKeys);
		uint key = keyConfig.key(dev)[buttonIdx];
		keyDiff += EmuControls::profileManager(dev).isKeyBaseDefault(key, buttonIdx) ? 0 : 1;
	}

	if(keyDiff)
	{
		logMsg("writing key config %d", cfgKey);
		io->writeVar(uint16(2 + 1 + (keyDiff*4)));
		io->writeVar(cfgKey);

		uint8 numBtns = keyDiff;
		io->writeVar(&numBtns);
		int wroteKeys = 0;

		forEachDInArray(supportedInputDev, dev)
		{
			uint key = keyConfig.key(dev)[buttonIdx];
			if(EmuControls::profileManager(dev).isKeyBaseDefault(key, buttonIdx))
				continue;

			uint16 devId = dev;
			io->writeVar(&devId);
			uint16 savedButton = key;
			io->writeVar(&savedButton);
			wroteKeys++;
			logMsg("wrote %s key idx %d, %s", InputEvent::devTypeName(dev),
					buttonIdx, Input::buttonName(dev, savedButton));
		}
		assert(keyDiff == wroteKeys);
	}
}

static OptionBase *cfgFileOption[] =
{
	&optionAutoSaveState,
	&optionSound,
	&optionSoundRate,
	&optionAspectRatio,
	&optionImageZoom,
	&optionImgFilter,
	&optionRelPointerDecel,
	&optionLargeFonts,
	&optionPauseUnfocused,
	&optionGameOrientation,
	&optionMenuOrientation,
	&optionTouchCtrl,
	&optionTouchCtrlAlpha,
	&optionTouchCtrlSize,
	&optionTouchDpadDeadzone,
	&optionTouchCtrlBtnSpace,
	&optionTouchCtrlBtnStagger,
	&optionTouchCtrlDpadPos,
	&optionTouchCtrlFaceBtnPos,
	&optionTouchCtrlCenterBtnPos,
	&optionTouchCtrlTriggerBtnPos,
	&optionTouchCtrlMenuPos,
	&optionTouchCtrlFFPos,
	&optionFrameSkip,
	&optionDPI,
	&optionVibrateOnPush,
	&optionRecentGames,
	&optionNotificationIcon,
	&optionTitleBar,
	&optionBackNavigation,
	&optionRememberLastMenu,
	#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	&optionDirectTexture,
	&optionGLSyncHack,
	#endif
	#ifdef CONFIG_BASE_IOS_ICADE
	&optionICade,
	#endif
};

static void loadConfigFile()
{
	FsSys::cPath configFilePath;
    snprintf(configFilePath, sizeof(configFilePath), "%s/config", Base::documentsPath());
    logMsg("loadConfigFile %s", configFilePath);

    Io* io = IoSys::create(configFilePath, IO_CREATE_KEEP);
    if(io == 0)
	{
		logMsg("no config file");
		return;
	}
    
	uint8 blockSize;
	io->readVar(&blockSize);
    
	if(blockSize != 2)
	{
		logErr("can't read config with block size %d", blockSize);
		return;
	}
    
	while(!io->eof())
	{
		uint16 size;
		io->readVar(&size);
        
		if(!size)
		{
			logMsg("invalid 0 size block, skipping rest of config");
			return;
		}
		if(size < 3) // all blocks are at least a 2 byte key + 1 byte or more of data
		{
			logMsg("skipping %d byte block", size);
			io->seekRel(size);
			continue;
		}
        
		uint16 key;
		io->readVar(&key);
		size -= 2;
        
		logMsg("got config key %u", key);
		switch(key)
		{
			default:
			{
				if(!EmuSystem::readConfig(io, key, size))
				{
					logMsg("skipping unknown key %u", (uint)key);
					io->seekRel(size);
				}
			}
                bcase CFGKEY_SOUND: optionSound.readFromIO(io, size);
                bcase CFGKEY_SOUND_RATE: optionSoundRate.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_DISPLAY: optionTouchCtrl.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_ALPHA: optionTouchCtrlAlpha.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_SIZE: optionTouchCtrlSize.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_DPAD_POS: optionTouchCtrlDpadPos.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_FACE_BTN_POS: optionTouchCtrlFaceBtnPos.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_MENU_POS: optionTouchCtrlMenuPos.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_FF_POS: optionTouchCtrlFFPos.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_FACE_BTN_SPACE: optionTouchCtrlBtnSpace.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_FACE_BTN_STAGGER: optionTouchCtrlBtnStagger.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_DPAD_DEADZONE: optionTouchDpadDeadzone.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_CENTER_BTN_POS: optionTouchCtrlCenterBtnPos.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_TRIGGER_BTN_POS: optionTouchCtrlTriggerBtnPos.readFromIO(io, size);
                bcase CFGKEY_AUTO_SAVE_STATE: optionAutoSaveState.readFromIO(io, size);
                bcase CFGKEY_FRAME_SKIP: optionFrameSkip.readFromIO(io, size);
                bcase CFGKEY_LAST_DIR:

                bcase CFGKEY_FONT_Y_PIXELS: optionLargeFonts.readFromIO(io, size);
                bcase CFGKEY_GAME_ORIENTATION: optionGameOrientation.readFromIO(io, size);
                bcase CFGKEY_MENU_ORIENTATION: optionMenuOrientation.readFromIO(io, size);
                bcase CFGKEY_GAME_IMG_FILTER: optionImgFilter.readFromIO(io, size);
                bcase CFGKEY_GAME_ASPECT_RATIO: optionAspectRatio.readFromIO(io, size);
                bcase CFGKEY_IMAGE_ZOOM: optionImageZoom.readFromIO(io, size);
                bcase CFGKEY_DPI: optionDPI.readFromIO(io, size);
                bcase CFGKEY_TOUCH_CONTROL_VIRBRATE: optionVibrateOnPush.readFromIO(io, size);
                bcase CFGKEY_RECENT_GAMES: optionRecentGames.readFromIO(io, size);
                bcase CFGKEY_SWAPPED_GAMEPAD_CONFIM:
			{
				uint8 b;
				io->readVar(&b);
				input_swappedGamepadConfirm = b;
			}
                bcase CFGKEY_PAUSE_UNFOCUSED: optionPauseUnfocused.readFromIO(io, size);
                bcase CFGKEY_NOTIFICATION_ICON: optionNotificationIcon.readFromIO(io, size);
                bcase CFGKEY_TITLE_BAR: optionTitleBar.readFromIO(io, size);
                bcase CFGKEY_BACK_NAVIGATION: optionBackNavigation.readFromIO(io, size);
                bcase CFGKEY_REMEMBER_LAST_MENU: optionRememberLastMenu.readFromIO(io, size);
#if defined(CONFIG_BASE_ANDROID)
                bcase CFGKEY_REL_POINTER_DECEL: optionRelPointerDecel.readFromIO(io, size);
#endif
#if defined(SUPPORT_ANDROID_DIRECT_TEXTURE)
                bcase CFGKEY_DIRECT_TEXTURE: optionDirectTexture.readFromIO(io, size);
                bcase CFGKEY_GL_SYNC_HACK: optionGLSyncHack.readFromIO(io, size);
#endif
#ifdef CONFIG_BASE_IOS_ICADE
                bcase CFGKEY_ICADE: optionICade.readFromIO(io, size);
#endif
                // start gui keys
                bcase CFGKEY_KEY_LOAD_GAME: readKeyConfig2(io, 0, size);
                bcase CFGKEY_KEY_OPEN_MENU: readKeyConfig2(io, 1, size);
                bcase CFGKEY_KEY_SAVE_STATE: readKeyConfig2(io, 2, size);
                bcase CFGKEY_KEY_LOAD_STATE: readKeyConfig2(io, 3, size);
                bcase CFGKEY_KEY_FAST_FORWARD: readKeyConfig2(io, 4, size);
                bcase CFGKEY_KEY_SCREENSHOT: readKeyConfig2(io, 5, size);
                //#ifndef CONFIG_ENV_WEBOS
                bcase CFGKEY_KEY_EXIT: readKeyConfig2(io, 6, size);
                //#endif
		}
	}
	io->close();
}

static void saveConfigFile()
{
	FsSys::cPath configFilePath;

	snprintf(configFilePath, sizeof(configFilePath), "%s/config", Base::documentsPath());
    Io* io = IoSys::create(configFilePath);
    if(!io)
	{
		logMsg("not writing config file");
		return;
	}
    
	uchar blockHeaderSize = 2;
	io->writeVar(blockHeaderSize);
    
	forEachDInArray(cfgFileOption, e)
	{
		if(!e->isDefault())
		{
			io->writeVar((uint16)e->ioSize());
			e->writeToIO(io);
		}
	}
    
	if(input_swappedGamepadConfirm == 1)
	{
		io->writeVar((uint16)3);
		io->writeVar((uint16)CFGKEY_SWAPPED_GAMEPAD_CONFIM);
		io->writeVar((uint8)input_swappedGamepadConfirm);
	}
    
	writeKeyConfig2(io, 0, CFGKEY_KEY_LOAD_GAME);
	writeKeyConfig2(io, 1, CFGKEY_KEY_OPEN_MENU);
	writeKeyConfig2(io, 2, CFGKEY_KEY_SAVE_STATE);
	writeKeyConfig2(io, 3, CFGKEY_KEY_LOAD_STATE);
	writeKeyConfig2(io, 4, CFGKEY_KEY_FAST_FORWARD);
	writeKeyConfig2(io, 5, CFGKEY_KEY_SCREENSHOT);
#ifndef CONFIG_ENV_WEBOS
	writeKeyConfig2(io, 6, CFGKEY_KEY_EXIT);
#endif
    
	EmuSystem::writeConfig(io);
    
	io->close();

}
