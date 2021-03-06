#pragma once

#include <gfx/defs.hh>
extern int g_rotation;

namespace Gfx
{

GC mmToPixelXScaler = 0, mmToPixelYScaler = 0;
uint viewPixelWidth_ = 0, viewPixelHeight_ = 0, viewMMWidth_ = 0, viewMMHeight_ = 0;
Projector proj = { 0 };

void setupScreenSize()
{
	assert(viewMMWidth_ != 0 && viewMMHeight_ != 0);
	mmToPixelXScaler = (GC)viewPixelWidth() / (GC)viewMMWidth();
	mmToPixelYScaler = (GC)viewPixelHeight() / (GC)viewMMHeight();
}

static GC orientationToGC(uint o)
{
	switch(o)
	{
		case VIEW_ROTATE_0: return 0.;
		case VIEW_ROTATE_90: return 90.;
		case VIEW_ROTATE_180: return 180.;
		case VIEW_ROTATE_270: return -90.;
		default: bug_branch("%d", o); return 0.;
	}
}

#ifdef CONFIG_GFX_SOFT_ORIENTATION
uint rotateView = VIEW_ROTATE_0;
static uint validOrientations = VIEW_ROTATE_0 | VIEW_ROTATE_90 | VIEW_ROTATE_270;
uint preferedOrientation = VIEW_ROTATE_0;

uint setValidOrientations(uint oMask, bool manageAutoOrientation)
{
	if(oMask == VIEW_ROTATE_AUTO)
	{
        if (g_rotation == 0) {
            oMask = VIEW_ROTATE_90 | VIEW_ROTATE_270;
        } else {
            oMask = VIEW_ROTATE_0;
        }
	}
	else
	{
		assert(oMask >= BIT(0) && oMask < BIT(4));
	}

	if(manageAutoOrientation)
	{
		if(bit_numSet(oMask) > 1)
			Base::setAutoOrientation(1);
		else
			Base::setAutoOrientation(0);
	}

	validOrientations = oMask;
	if(validOrientations & preferedOrientation)
		return setOrientation(preferedOrientation);
	if(!(validOrientations & rotateView))
	{
		if(validOrientations & VIEW_ROTATE_0)
			return setOrientation(VIEW_ROTATE_0);
		else if(validOrientations & VIEW_ROTATE_90)
			return setOrientation(VIEW_ROTATE_90);
		else if(validOrientations & VIEW_ROTATE_180)
			return setOrientation(VIEW_ROTATE_180);
		else if(validOrientations & VIEW_ROTATE_270)
			return setOrientation(VIEW_ROTATE_270);
		else
		{
			logWarn("warning: valid orientation mask contain no valid values");
			return 0;
		}
	}
	return 0;
}
#endif

}
