#pragma once

#include <engine-globals.h>
#include <util/number.h>

#ifdef CONFIG_BASE_X11
	#include <base/x11/inputDefs.hh>
#elif defined(CONFIG_BASE_ANDROID)
	#include <input/android/inputDefs.hh>
#elif defined(CONFIG_BASE_SDL)
	#include <base/sdl/inputDefs.hh>
#elif defined(CONFIG_BASE_IOS)
	#include <base/iphone/inputDefs.hh>
#elif defined(CONFIG_BASE_MACOSX)
	#include <base/osx/inputDefs.hh>
#elif defined(CONFIG_BASE_PS3)
	#include <input/ps3/inputDefs.hh>
#endif

typedef uint InputButton;

namespace Input
{
// mouse/pointer/touch support
#ifndef CONFIG_INPUT_PS3
	static const bool supportsPointer = 1;
	#define INPUT_SUPPORTS_POINTER
#else
	static const bool supportsPointer = 0;
#endif

static const uchar maxCursors =
#if defined(CONFIG_BASE_X11) || defined(CONFIG_BASE_IOS)
	4 // unknown max
#elif defined(CONFIG_BASE_ANDROID)
	#if CONFIG_ENV_ANDROID_MINSDK == 4
		1 // no multi-touch
	#else
		3 // unknown max
	#endif
#elif defined(CONFIG_ENV_WEBOS)
	4 // max 5
#else
	1
#endif
;

// relative pointer/trackball support
#ifdef CONFIG_BASE_ANDROID
	static const bool supportsRelativePointer = 1;
	#define INPUT_SUPPORTS_RELATIVE_POINTER
#else
	static const bool supportsRelativePointer = 0;
#endif

// keyboard/key-based input support
#if !defined(CONFIG_BASE_IOS) && !defined(CONFIG_BASE_PS3)
	static const bool supportsKeyboard = 1;
	#define INPUT_SUPPORTS_KEYBOARD
#else
	static const bool supportsKeyboard = 0;
#endif

    CallResult init();// ATTRS(cold);

void setKeyRepeat(bool on);

// Control if volume keys are used by the app or passed on to the OS
#ifdef CONFIG_BASE_ANDROID
	void setHandleVolumeKeys(bool on);
#else
	static void setHandleVolumeKeys(bool on) { }
#endif

#ifdef INPUT_SUPPORTS_POINTER
	extern uint numCursors;
	int cursorX(int p = 0) ATTRS(pure);
	int cursorY(int p = 0) ATTRS(pure);
	int cursorIsInView(int p = 0) ATTRS(pure);
	void hideCursor();
	void showCursor();
#else
	static uint numCursors = 0;
	static int cursorX(int p = 0) { return -1; };
	static int cursorY(int p = 0) { return -1; };
	static int cursorIsInView(int p = 0) { return 0; };
	static void hideCursor() { };
	static void showCursor() { };
#endif

static void cursorXY(int *x, int *y, int p = 0)
{
	*x = cursorX(p);
	*y = cursorY(p);
}

static const uint numAsciiKeys = (('~' - '!') + 1);

#ifdef CONFIG_BASE_ANDROID

// Android key-codes don't directly map to ASCII
#define input_asciiKey(c) (((c) >= 'a' && (c) <= 'z') ? (c)-68 : \
			((c) >= '0' && c <= '9') ? (c)-41 : \
			((c) == ' ') ? 62 : \
			((c) == '*') ? 17 : \
			((c) == '#') ? 18 : \
			((c) == ',') ? 55 : \
			((c) == '.') ? 56 : \
			((c) == '`') ? 68 : \
			((c) == '-') ? 69 : \
			((c) == '=') ? 70 : \
			((c) == '[') ? 71 : \
			((c) == ']') ? 72 : \
			((c) == '\\') ? 73 : \
			((c) == ';') ? 74 : \
			((c) == '\'') ? 75 : \
			((c) == '/') ? 76 : \
			((c) == '@') ? 77 : \
			((c) == '+') ? 81 : \
			0)

static uint decodeAscii(InputButton k)
{
	bug_exit("TODO");
	return 0;
}

#elif defined(CONFIG_BASE_IOS)

// TODO
#define input_asciiKey(key) (key)

static uint decodeAscii(InputButton k)
{
	bug_exit("TODO");
	return 0;
}

#else

#define input_asciiKey(key) (key)

static bool isAsciiKey(InputButton k)
{
	return k >= ' '  && k <= '~';
}

static uint decodeAscii(InputButton k)
{
	switch(k)
	{
		#ifdef INPUT_SUPPORTS_KEYBOARD
		case Key::ENTER: return 13;
		case Key::BACK_SPACE: return 8;
		#endif
		case ' ' ... '~': return k;
	}
	return 0;
}

#endif

namespace Wiimote
{
	static const uint _1 = 1,
	_2 = 2,
	A = 3,
	B = 4,
	PLUS = 5,
	MINUS = 6,
	HOME = 7,
	// Classic Controller-specific
	L = 8, R = 9,
	ZL = 10, ZR = 11,
	X = 12, Y = 13,
	CC_LSTICK_LEFT = 14, CC_LSTICK_RIGHT = 15, CC_LSTICK_UP = 16, CC_LSTICK_DOWN = 17,
	CC_RSTICK_LEFT = 18, CC_RSTICK_RIGHT = 19, CC_RSTICK_UP = 20, CC_RSTICK_DOWN = 21,
	// Directions
	LEFT = 22, RIGHT = 23, UP = 24, DOWN = 25
	;

	static const uint COUNT = 26;
}

namespace iControlPad
{
	static const uint A = 1,
	B = 2,
	X = 3,
	Y = 4,
	L = 5,
	R = 6,
	START = 7,
	SELECT = 8,
	LNUB_LEFT = 9, LNUB_RIGHT = 10, LNUB_UP = 11, LNUB_DOWN = 12,
	RNUB_LEFT = 13, RNUB_RIGHT = 14, RNUB_UP = 15, RNUB_DOWN = 16,
	LEFT = 17, RIGHT = 18, UP = 19, DOWN = 20
	;

	static const uint COUNT = 21;
}

namespace Zeemote
{
	static const uint A = 1,
	B = 2,
	C = 3,
	POWER = 4,
	// Directions (from analog stick)
	LEFT = 5, RIGHT = 6, UP = 7, DOWN = 8
	;

	static const uint COUNT = 9;
}

namespace Ps3
{
	static const uint CROSS = 1,
	CIRCLE = 2,
	SQUARE = 3,
	TRIANGLE = 4,
	L1 = 5,
	L2 = 6,
	L3 = 7,
	R1 = 8,
	R2 = 9,
	R3 = 10,
	SELECT = 11,
	START = 12,
	UP = 13, RIGHT = 14, DOWN = 15, LEFT = 16
	;

	static const uint COUNT = 17;
}

namespace ICade
{
	static const uint UP = 1,
	RIGHT = 2,
	DOWN = 3,
	LEFT = 4,
	A = 5,
	B = 6,
	C = 7,
	D = 8,
	E = 9,
	F = 10,
	G = 11,
	H = 12
	;

	static const uint COUNT = 13;
}

static bool isVolumeKey(InputButton event)
{
	#if defined(CONFIG_BASE_SDL) || !defined(INPUT_SUPPORTS_KEYBOARD)
		return 0;
	#else
		return event == Key::VOL_UP || event == Key::VOL_DOWN;
	#endif
}

const char *buttonName(uint dev, InputButton b) ATTRS(const);

}

enum { INPUT_UNUSED, INPUT_RELEASED, INPUT_PUSHED, INPUT_MOVED, INPUT_MOVED_RELATIVE, INPUT_EXIT_VIEW, INPUT_ENTER_VIEW };

enum { INPUT_POINTER_NORMAL, INPUT_POINTER_INVERT };
void input_xPointerTransform(uint mode);
void input_yPointerTransform(uint mode);
void input_pointerAxis(uint mode);

typedef void (*InputTextCallback)(void *user, const char *str);

#if defined(CONFIG_BASE_IOS) && defined(IPHONE_VKEYBOARD)
static bool input_needsVKeyboard = 1;
uint input_getVKeyboardString(InputTextCallback callback, void *user, const char *initialText);
#else
static bool input_needsVKeyboard = 0;
static uint input_getVKeyboardString(InputTextCallback callback, void *user, const char *initialText) { return 0; }
#endif

struct PackedInputAccess
{
	uint byteOffset;
	uint mask;
	uint keyEvent;

	int updateState(const uchar *prev, const uchar *curr) const
	{
		bool oldState = prev[byteOffset] & mask,
			newState = curr[byteOffset] & mask;
		if(oldState != newState)
		{
			return newState;
		}
		return -1; // no state change
	}

};

extern bool input_swappedGamepadConfirm;

class InputEvent
{
public:
	enum { DEV_NULL = 0, DEV_KEYBOARD, DEV_POINTER, DEV_REL_POINTER, DEV_WIIMOTE, DEV_ICONTROLPAD,
		DEV_ZEEMOTE, DEV_ICADE, DEV_PS3PAD };

	static const char *devTypeName(uint devType)
	{
		switch(devType)
		{
			case DEV_NULL: return "Null";
			#ifdef CONFIG_BASE_ANDROID
			case DEV_KEYBOARD: return "Key Input";
			#else
			case DEV_KEYBOARD: return "Keyboard";
			#endif
			case DEV_POINTER: return "Pointer";
			case DEV_REL_POINTER: return "Relative Pointer";
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return "Wiimote";
			case DEV_ICONTROLPAD: return "iControlPad";
			case DEV_ZEEMOTE: return "Zeemote JS1";
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return "PS3 Gamepad";
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return "iCade";
			#endif
			default: return "Unknown";
		}
	}

	static uint devTypeNumKeys(uint devType)
	{
		switch(devType)
		{
			case DEV_NULL: return 0;
			#ifdef INPUT_SUPPORTS_KEYBOARD
			case DEV_KEYBOARD: return Input::Key::COUNT;
			#endif
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return Input::Wiimote::COUNT;
			case DEV_ICONTROLPAD: return Input::iControlPad::COUNT;
			case DEV_ZEEMOTE: return Input::Zeemote::COUNT;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return Input::Ps3::COUNT;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return Input::ICade::COUNT;
			#endif
			default: bug_branch("%d", devType); return 0;
		}
	}

	constexpr InputEvent()
		: devId(0), devType(DEV_NULL), button(0), state(0), x(0), y(0) { }

	constexpr InputEvent(uint devId, uint devType, InputButton button, uint state, int x, int y)
		: devId(devId), devType(devType), button(button), state(state), x(x), y(y) { }

	constexpr InputEvent(uint devId, uint devType, InputButton button, uint state)
		: devId(devId), devType(devType), button(button), state(state), x(0), y(0) { }

	uint devId, devType;
	InputButton button;
	uint state;
	int x, y;

	bool stateIsPointer() const
	{
		return state == INPUT_MOVED || state == INPUT_EXIT_VIEW || state == INPUT_ENTER_VIEW;
	}

	bool isPointer() const
	{
		return Input::supportsPointer && (devType == DEV_POINTER/*input_eventIsFromPointer(button)*/ || stateIsPointer());
	}

	bool isRelativePointer() const
	{
		return Input::supportsRelativePointer && state == INPUT_MOVED_RELATIVE;
	}

	bool isGamepad() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE:
			case DEV_ICONTROLPAD:
			case DEV_ZEEMOTE: return 1;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return 1;
			#endif
			default : return 0;
		}
	}

	bool isKey() const
	{
		return Input::supportsKeyboard && !isPointer() && !isRelativePointer();
	}

	bool isDefaultActionButton(uint swapped = input_swappedGamepadConfirm) const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return swapped ? isDefaultCancelButton(0) : (button == Input::Wiimote::_1 || button == Input::Wiimote::B);
			case DEV_ICONTROLPAD: return swapped ? isDefaultCancelButton(0) : (button == Input::iControlPad::X);
			case DEV_ZEEMOTE: return swapped ? isDefaultCancelButton(0) : (button == Input::Zeemote::A);
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return swapped ? isDefaultCancelButton(0) : (button == Input::ICade::H || button == Input::ICade::A);
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return swapped ? isDefaultCancelButton(0) : (button == Input::Ps3::CROSS);
			#endif
			default:
				return button == input_asciiKey('z')
				#ifdef CONFIG_BASE_ANDROID
				|| button == Input::Key::CENTER || button == Input::Key::GAME_A || button == Input::Key::GAME_1
				#endif
				;
		}
	}

	bool isDefaultCancelButton(uint swapped = input_swappedGamepadConfirm) const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return swapped ? isDefaultActionButton(0) : (button == Input::Wiimote::_2 || button == Input::Wiimote::A);
			case DEV_ICONTROLPAD: return swapped ? isDefaultActionButton(0) : (button == Input::iControlPad::B);
			case DEV_ZEEMOTE: return swapped ? isDefaultActionButton(0) : (button == Input::Zeemote::B);
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return swapped ? isDefaultActionButton(0) : (button == Input::ICade::G || button == Input::ICade::C);
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return swapped ? isDefaultActionButton(0) : (button == Input::Ps3::CIRCLE);
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default:
				return button == Input::Key::ESCAPE || button == input_asciiKey('x')
					#ifdef CONFIG_INPUT_ANDROID
					|| button == Input::Key::GAME_B || button == Input::Key::GAME_2
					#endif
					;
			#endif
		}
		return 0;
	}

	bool isDefaultLeftButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::LEFT || button == Input::Wiimote::CC_LSTICK_LEFT;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::LEFT || button == Input::iControlPad::LNUB_LEFT;
			case DEV_ZEEMOTE: return button == Input::Zeemote::LEFT;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::LEFT;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::LEFT;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default:
				return button == Input::Key::LEFT
				#ifdef CONFIG_ENV_WEBOS
				|| button == input_asciiKey('d')
				#endif
				;
			#endif
		}
		return 0;
	}

	bool isDefaultRightButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::RIGHT || button == Input::Wiimote::CC_LSTICK_RIGHT;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::RIGHT || button == Input::iControlPad::LNUB_RIGHT;
			case DEV_ZEEMOTE: return button == Input::Zeemote::RIGHT;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::RIGHT;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::RIGHT;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default:
				return button == Input::Key::RIGHT
				#ifdef CONFIG_ENV_WEBOS
				|| button == input_asciiKey('g')
				#endif
				;
			#endif
		}
		return 0;
	}

	bool isDefaultUpButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::UP || button == Input::Wiimote::CC_LSTICK_UP;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::UP || button == Input::iControlPad::LNUB_UP;
			case DEV_ZEEMOTE: return button == Input::Zeemote::UP;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::UP;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::UP;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default:
				return button == Input::Key::UP
				#ifdef CONFIG_ENV_WEBOS
				|| button == input_asciiKey('r')
				#endif
				;
			#endif
		}
		return 0;
	}

	bool isDefaultDownButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::DOWN || button == Input::Wiimote::CC_LSTICK_DOWN;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::DOWN || button == Input::iControlPad::LNUB_DOWN;
			case DEV_ZEEMOTE: return button == Input::Zeemote::DOWN;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::DOWN;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::DOWN;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default:
				return button == Input::Key::DOWN
				#ifdef CONFIG_ENV_WEBOS
				|| button == input_asciiKey('c')
				#endif
				;
			#endif
		}
		return 0;
	}

	bool isDefaultPageUpButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::PLUS || button == Input::Wiimote::L;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::L;
			case DEV_ZEEMOTE: return 0;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::B;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::L1;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default: return button == Input::Key::PGUP
			#ifdef CONFIG_BASE_ANDROID
					|| button == Input::Key::GAME_L1
			#endif
					;
			#endif
		}
		return 0;
	}

	bool isDefaultPageDownButton() const
	{
		switch(devType)
		{
			#ifdef CONFIG_BLUETOOTH
			case DEV_WIIMOTE: return button == Input::Wiimote::MINUS || button == Input::Wiimote::R;
			case DEV_ICONTROLPAD: return button == Input::iControlPad::R;
			case DEV_ZEEMOTE: return 0;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case DEV_ICADE: return button == Input::ICade::D;
			#endif
			#ifdef CONFIG_BASE_PS3
			case DEV_PS3PAD: return button == Input::Ps3::R1;
			#endif
			#ifdef INPUT_SUPPORTS_KEYBOARD
			default: return button == Input::Key::PGDOWN
			#ifdef CONFIG_BASE_ANDROID
					|| button == Input::Key::GAME_R1
			#endif
					;
			#endif
		}
		return 0;
	}

	bool pushed() const
	{
		return state == INPUT_PUSHED;
	}

	bool pushed(InputButton button) const
	{
		return pushed() && this->button == button;
	}

	bool released() const
	{
		return state == INPUT_RELEASED;
	}

	bool moved() const
	{
		return state == INPUT_MOVED;
	}

	uint decodeAscii() const
	{
		return Input::decodeAscii(button);
	}
};

namespace Input
{
	typedef void(*InputEventFunc)(void* userPtr, const InputEvent &event);
	void eventHandler(InputEventFunc handler, void *userPtr = 0);

	static uint devButtonCount(uint devType)
	{
		switch(devType)
		{
			#ifdef INPUT_SUPPORTS_KEYBOARD
			case InputEvent::DEV_KEYBOARD: return Key::COUNT;
			case InputEvent::DEV_POINTER: return Key::COUNT;
			case InputEvent::DEV_REL_POINTER: return Key::COUNT;
			#endif
			#ifdef CONFIG_BLUETOOTH
			case InputEvent::DEV_WIIMOTE: return Wiimote::COUNT;
			case InputEvent::DEV_ICONTROLPAD: return iControlPad::COUNT;
			case InputEvent::DEV_ZEEMOTE: return Zeemote::COUNT;
			#endif
			#ifdef CONFIG_BASE_PS3
			case InputEvent::DEV_PS3PAD: return Ps3::COUNT;
			#endif
			#ifdef CONFIG_BASE_IOS_ICADE
			case InputEvent::DEV_ICADE: return ICade::COUNT;
			#endif
			default: return 0;
		}
	}
}
