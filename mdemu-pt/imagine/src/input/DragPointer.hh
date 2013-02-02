#pragma once

#include <input/interface.h>
#include <gfx/interface.h>
#include <util/rectangle2.h>

class DragPointer
{
public:
	void init(int pid)
	{
		this->pid = pid;
		prevX = this->x = Input::cursorX(pid);
		prevY = this->y = Input::cursorY(pid);
		pushed = 0;
	}

	void pointerEvent(uint button, uint state, int x, int y)
	{
		if(state == INPUT_PUSHED)
		{
			pushX = x;
			pushY = y;
			//time = gfx_frameTime;
			pushed = 1;
			//logMsg("pushX %d pushY %d", pushX, pushY);
		}
		else if(state == INPUT_RELEASED)
		{
			pushed = 0;
		}
		prevX = this->x;
		prevY = this->y;
		this->x = x;
		this->y = y;
		//logMsg("new drag state prevX %d prevY %d", prevX, prevY);
	}

	int relX() const
	{
		return x - prevX;
	}

	int relY() const
	{
		return y - prevY;
	}

	bool dragged() const
	{
		return dragX() != 0 || dragY() != 0;
	}

	/*bool draggedFromRect(const Rect<int> &r) const
	{
		return dragged() && r.overlaps(pushX, pushY);
	}*/

	bool draggedFromRect(const Rect2<int> &r) const
	{
		return dragged() && r.overlaps(pushX, pushY);
	}

	int dragX() const
	{
		if(pushed)
			return x - pushX;
		else
			return 0;
	}

	int dragY() const
	{
		if(pushed)
			return y - pushY;
		else
			return 0;
	}

	/*uint timeSincePush()
	{
		return gfx_frameTime - time;
	}*/

	int prevX, prevY; // location during previous update
	int pushX, pushY; // location of most recent push
	int x, y; // current location
	int pid;
	//uint time;
	uint pushed;
};

namespace Input
{
#ifdef INPUT_SUPPORTS_POINTER
	DragPointer *dragState(int p);
#else
	//static DragPointer *dragState(int p) { bugExit("input_dragState called with no platform support"); return 0; }
#endif
}
