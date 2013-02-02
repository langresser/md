#pragma once
#include <input/common/common.h>
#include <input/DragPointer.hh>

namespace Input
{

static PointerState m[Input::maxCursors] = { { 0, 0, 0 } };
uint numCursors = Input::maxCursors;
static DragPointer dragStateArr[Input::maxCursors];

DragPointer *dragState(int p)
{
	return &dragStateArr[p];
}

static void mouseEvent(uint button, int p, uint action, int x, int y)
{
	if(unlikely(p >= Input::maxCursors))
	{
		logMsg("touch index out of range");
		return;
	}
	pointerPos(x, y, &m[p].x, &m[p].y);
	#ifdef CONFIG_BASE_SDL_PDL
		button = Input::Pointer::LBUTTON;
		m[p].inWin = action != INPUT_RELEASED; // handle touch end on WebOS
	#else
		m[p].inWin = 1;
	#endif
	dragStateArr[p].pointerEvent(button, action, m[p].x, m[p].y);
	//logMsg("p %d @ %d,%d %d", p, m[p].x, m[p].y, action);
	if(likely(Input::onInputEventHandler != 0))
		Input::onInputEventHandler(Input::onInputEventHandlerCtx, InputEvent(p, InputEvent::DEV_POINTER, button, action, m[p].x, m[p].y));
}

static void keyEvent(SDLKey k, uint action)
{
	assert(k < Key::COUNT);
	//logMsg("key %s %d", Input::buttonName(InputEvent::DEV_KEYBOARD, k), action);
	if(likely(Input::onInputEventHandler != 0))
		Input::onInputEventHandler(Input::onInputEventHandlerCtx, InputEvent(0, InputEvent::DEV_KEYBOARD, k & 0xFFF, action));
}

void setKeyRepeat(bool on)
{
	if(on)
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	else
		SDL_EnableKeyRepeat(0, 0);
}

int cursorX(int p) { return m[p].x; }
int cursorY(int p) { return m[p].y; }
int cursorIsInView(int p) { return m[p].inWin; }

CallResult init()
{
	iterateTimes((uint)Input::maxCursors, i)
	{
		dragStateArr[i].init(i);
	}

	return OK;
}

}
