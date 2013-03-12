#pragma once
#include "platform_util.h"

bool isPad();
void getScreenSize(int* width, int* height);

void showJoystick();
void hideJoystick();

void showSetting();


#define kRemoveAdsFlag @"bsrmads"