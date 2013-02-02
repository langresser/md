#pragma once

namespace EmuControls
{

static const uint categories = 2;
static const uint gamepadKeys = 22;
static const uint systemTotalKeys = gameActionKeys + gamepadKeys;

static const char *gamepadName[gamepadKeys] =
{
		"Up",
		"Right",
		"Down",
		"Left",
		"Left+Up",
		"Right+Up",
		"Right+Down",
		"Left+Down",
		"Mode",
		"Start",
		"A",
		"B",
		"C",
		"X",
		"Y",
		"Z",
		"Turbo A",
		"Turbo B",
		"Turbo C",
		"Turbo X",
		"Turbo Y",
		"Turbo Z",
};

static KeyCategory category[categories] =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_CATEGORY_INIT,
		KeyCategory("Gamepad Controls", gamepadName, gameActionKeys),
};

typedef uint KeyArray[systemTotalKeys];

// Key Input/Keyboard

#ifdef INPUT_SUPPORTS_KEYBOARD
static const KeyArray webOSKB =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_WEBOS_KB_PROFILE_INIT,

		EMU_CONTROLS_WEBOS_KB_8WAY_DIRECTION_PROFILE_INIT,
		input_asciiKey(' '),
		Input::Key::ENTER,
		input_asciiKey('n'),
		input_asciiKey('m'),
		input_asciiKey(','),
		input_asciiKey('j'),
		input_asciiKey('k'),
		input_asciiKey('l'),
		0, 0, 0, 0, 0, 0,
};

static const KeyArray genericKB =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_GENERIC_KB_PROFILE_INIT,

	Input::Key::UP,
	Input::Key::RIGHT,
	Input::Key::DOWN,
	Input::Key::LEFT,
	0,
	0,
	0,
	0,
	input_asciiKey(' '),
	Input::Key::ENTER,
	input_asciiKey('z'),
	input_asciiKey('x'),
	input_asciiKey('c'),
	input_asciiKey('a'),
	input_asciiKey('s'),
	input_asciiKey('d'),
	0, 0, 0, 0, 0, 0,
};

#ifdef CONFIG_BASE_ANDROID
static const KeyArray androidNav =
{
	EMU_CONTROLS_IN_GAME_ACTIONS_ANDROID_NAV_PROFILE_INIT,

	Input::Key::UP,
	Input::Key::RIGHT,
	Input::Key::DOWN,
	Input::Key::LEFT,
	0,
	0,
	0,
	0,
	input_asciiKey(' '),
	Input::Key::ENTER,
	input_asciiKey('z'),
	input_asciiKey('x'),
	input_asciiKey('c'),
	input_asciiKey('a'),
	input_asciiKey('s'),
	input_asciiKey('d'),
	0, 0, 0, 0, 0, 0,
};


static const KeyArray xperiaPlay =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_ANDROID_NAV_NO_BACK_PROFILE_INIT,

		Input::Key::UP,
		Input::Key::RIGHT,
		Input::Key::DOWN,
		Input::Key::LEFT,
		0,
		0,
		0,
		0,
		Input::Key::GAME_SELECT,
		Input::Key::GAME_START,
		Input::Key::GAME_X,
		Input::Key::CENTER,
		Input::Key::ESCAPE,
		Input::Key::GAME_Y,
		Input::Key::GAME_L1,
		Input::Key::GAME_R1,
		0, 0, 0, 0, 0, 0,
};
#endif

static const ConstKeyProfile kb[] =
{
#ifdef CONFIG_ENV_WEBOS
	{ "WebOS Keyboard", webOSKB },
#endif
#ifdef CONFIG_BASE_ANDROID
	{ "Android Nav + Keyboard", androidNav },
	{ "Xperia Play", xperiaPlay },
#endif
	{ "Standard Keyboard", genericKB },
};

#endif

// Wiimote

static const KeyArray wiimote =
{
	EMU_CONTROLS_IN_GAME_ACTIONS_WIIMOTE_PROFILE_INIT,

	Input::Wiimote::UP,
	Input::Wiimote::RIGHT,
	Input::Wiimote::DOWN,
	Input::Wiimote::LEFT,
	0,
	0,
	0,
	0,
	Input::Wiimote::MINUS,
	Input::Wiimote::PLUS,
	Input::Wiimote::A,
	Input::Wiimote::_1,
	Input::Wiimote::_2,
	0,
	0,
	0,
	0, 0, 0, 0, 0, 0,
};

static const KeyArray wiiCC =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_WII_CC_PROFILE_INIT,

	Input::Wiimote::UP,
	Input::Wiimote::RIGHT,
	Input::Wiimote::DOWN,
	Input::Wiimote::LEFT,
	0,
	0,
	0,
	0,
	Input::Wiimote::MINUS,
	Input::Wiimote::PLUS,
	Input::Wiimote::Y,
	Input::Wiimote::B,
	Input::Wiimote::A,
	0, 0, 0,
	Input::Wiimote::L,
	Input::Wiimote::R,
	Input::Wiimote::X,
};

static const KeyArray wiiCC6Btn =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_WII_CC_PROFILE_INIT,

	Input::Wiimote::UP,
	Input::Wiimote::RIGHT,
	Input::Wiimote::DOWN,
	Input::Wiimote::LEFT,
	0,
	0,
	0,
	0,
	Input::Wiimote::MINUS,
	Input::Wiimote::PLUS,
	Input::Wiimote::B,
	Input::Wiimote::A,
	Input::Wiimote::R,
	Input::Wiimote::Y,
	Input::Wiimote::X,
	Input::Wiimote::L,
};

static const ConstKeyProfile wii[] =
{
		{ "Default Wiimote", wiimote },
		{ "Default Classic Controller", wiiCC },
		{ "Classic Controller (6-button)", wiiCC6Btn }
};

// iControlPad

static const KeyArray iCPDefault =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_ICP_NUBS_PROFILE_INIT,

		Input::iControlPad::UP,
		Input::iControlPad::RIGHT,
		Input::iControlPad::DOWN,
		Input::iControlPad::LEFT,
		0,
		0,
		0,
		0,
		Input::iControlPad::SELECT,
		Input::iControlPad::START,
		Input::iControlPad::A,
		Input::iControlPad::X,
		Input::iControlPad::B,
		Input::iControlPad::Y,
		Input::iControlPad::L,
		Input::iControlPad::R,
		0, 0, 0, 0, 0, 0,
};

static const ConstKeyProfile iCP[] =
{
		{ "Default iControlPad", iCPDefault }
};

// iCade

static const KeyArray iCadeDefault =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_UNBINDED_PROFILE_INIT,

		Input::ICade::UP,
		Input::ICade::RIGHT,
		Input::ICade::DOWN,
		Input::ICade::LEFT,
		0,
		0,
		0,
		0,
		Input::ICade::A,
		Input::ICade::C,
		Input::ICade::F,
		Input::ICade::H,
		Input::ICade::G,
		Input::ICade::E,
		Input::ICade::B,
		Input::ICade::D,
		0, 0, 0, 0, 0, 0,
};

static const ConstKeyProfile iCade[] =
{
		{ "Default iCade", iCadeDefault }
};

// Zeemote

static const KeyArray zeemoteDefaults =
{
		EMU_CONTROLS_IN_GAME_ACTIONS_UNBINDED_PROFILE_INIT,

		Input::Zeemote::UP,
		Input::Zeemote::RIGHT,
		Input::Zeemote::DOWN,
		Input::Zeemote::LEFT,
		0,
		0,
		0,
		0,
		0,
		Input::Zeemote::POWER,
		Input::Zeemote::A,
		Input::Zeemote::B,
		Input::Zeemote::C,
		0,
		0,
		0,
		0, 0, 0, 0, 0, 0,
};

static const ConstKeyProfile zeemote[] =
{
		{ "Default Zeemote JS1", zeemoteDefaults }
};



}
