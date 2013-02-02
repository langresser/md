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

#define thisModuleName "input:ps3"
#include <input/interface.h>
#include <input/common/common.h>
#include <cell/pad.h>
#include <cell/sysmodule.h>

namespace Input
{

static const PackedInputAccess padDataAccess[] =
{
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_SELECT, Ps3::SELECT },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_L3, Ps3::L3 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_R3, Ps3::R3 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_START, Ps3::START },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_UP, Ps3::UP },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_RIGHT, Ps3::RIGHT },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_DOWN, Ps3::DOWN },
	{ CELL_PAD_BTN_OFFSET_DIGITAL1, CELL_PAD_CTRL_LEFT, Ps3::LEFT },

	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_L2, Ps3::L2 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_R2, Ps3::R2 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_L1, Ps3::L1 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_R1, Ps3::R1 },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_TRIANGLE, Ps3::TRIANGLE },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_CIRCLE, Ps3::CIRCLE },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_CROSS, Ps3::CROSS },
	{ CELL_PAD_BTN_OFFSET_DIGITAL2, CELL_PAD_CTRL_SQUARE, Ps3::SQUARE },
};

static const uint numPads = 5;
static uint padStatus[numPads] = { 0 };
static CellPadInfo2 padInfo;
static CellPadData padData[numPads] = { { 0, { 0 } } };

void update()
{
	cellPadGetInfo2(&padInfo);

	iterateTimes(sizeofArray(padStatus), i)
	{
		if(padInfo.port_status[i] != 0 && padStatus[i] == 0)
		{
			//logMsg("pad connection %d vendor:%d product:%d", i, padInfo.vendor_id[i], padInfo.product_id[i]);
			logMsg("pad connection %d type:%d capability:%d", i, padInfo.device_type[i], padInfo.device_capability[i]);
			mem_zero(padData[i]);
		}
		padStatus[i] = padInfo.port_status[i];

		CellPadData data = { 0, { 0 } };
		cellPadGetData(i, &data);
		if(data.len != 0)
		{
			//logMsg("%d bytes from port", data.len, i);
			forEachInArray(padDataAccess, e)
			{
				bool oldState = padData[i].button[e->byteOffset] & e->mask,
					newState = data.button[e->byteOffset] & e->mask;
				if(oldState != newState)
				{
					//logMsg("%d %s %s", i, buttonName(InputEvent::DEV_PS3PAD, e->keyEvent), newState ? "pushed" : "released");
					if(likely(onInputEventHandler != 0))
						onInputEventHandler(onInputEventHandlerCtx, InputEvent(i, InputEvent::DEV_PS3PAD, e->keyEvent, newState ? INPUT_PUSHED : INPUT_RELEASED));
				}
			}
			memcpy(&padData[i], &data, sizeof(CellPadData));
		}
	}
}

void setKeyRepeat(bool on)
{
	// TODO
}

CallResult init()
{
	cellSysmoduleLoadModule(CELL_SYSMODULE_IO);

	int ret = cellPadInit(numPads);
	if(ret != CELL_OK && ret != CELL_PAD_ERROR_ALREADY_INITIALIZED)
	{
		logErr("error in cellPadInit");
		Base::exit();
	}

	return OK;
}

}

#undef thisModuleName
