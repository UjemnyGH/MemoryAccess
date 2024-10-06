/**
*
* @author Piotr UjemnyGH Plombon
*
* App core, function definitions
*
*/


#include "memoryAccessCore.hpp"

// Global counter (quite useful)
int gGlobalCounter;

// Software version
std::uint32_t gSoftwareVersion = MA_MAKE_SOFT_VERSION(1, 0, 0, 1);

// Get software version as string
std::string SoftwareVerisonToString() {
	uint8_t release = (uint8_t)((gSoftwareVersion & 0xff000000) >> 24);
	uint8_t major = (uint8_t)((gSoftwareVersion & 0xff0000) >> 16);
	uint8_t minor = (uint8_t)((gSoftwareVersion & 0xff00) >> 8);
	uint8_t snap = (uint8_t)(gSoftwareVersion & 0xff);

	std::string result;
	result += '0' + release % 0xf;
	result += '.';
	result += '0' + major % 0xf;
	result += '.';
	result += '0' + minor % 0xf;
	result += '.';
	result += '0' + snap % 0xf;

	return result;
}

// Reset global counter
void ResetGlobalCounter() {
	gGlobalCounter = 0;
}

// Convert string into int
constexpr std::uint32_t StringToInt(const char* str, int h) {
	return !str[h] ? 5381 : (StringToInt(str, h + 1) * 33) ^ str[h];
}

// Convert keyname into virtual key value, dont touch meat of this function
const std::int32_t KeynameToVkValue(const char* keyname) {
	switch (StringToInt(keyname)) {
	case StringToInt("back"):
		return VK_BACK;

		break;
	case StringToInt("tab"):
		return VK_TAB;

		break;
	case StringToInt("clear"):
		return VK_CLEAR;

		break;
	case StringToInt("return"):
		return VK_RETURN;

		break;
	case StringToInt("shift"):
		return VK_SHIFT;

		break;
	case StringToInt("control"):
		return VK_CONTROL;

		break;
	case StringToInt("pause"):
		return VK_PAUSE;

		break;
	case StringToInt("capital"):
		return VK_CAPITAL;

		break;
	case StringToInt("escape"):
		return VK_ESCAPE;

		break;
	case StringToInt("space"):
		return VK_SPACE;

		break;
	case StringToInt("end"):
		return VK_END;

		break;
	case StringToInt("home"):
		return VK_HOME;

		break;
	case StringToInt("left"):
		return VK_LEFT;

		break;
	case StringToInt("up"):
		return VK_UP;

		break;
	case StringToInt("right"):
		return VK_RIGHT;

		break;
	case StringToInt("down"):
		return VK_DOWN;

		break;
	case StringToInt("select"):
		return VK_SELECT;

		break;
	case StringToInt("print"):
		return VK_PRINT;

		break;
	case StringToInt("insert"):
		return VK_INSERT;

		break;
	case StringToInt("delete"):
		return VK_DELETE;

		break;
	case StringToInt("lwin"):
		return VK_LWIN;

		break;
	case StringToInt("rwin"):
		return VK_RWIN;

		break;
	case StringToInt("sleep"):
		return VK_SLEEP;

		break;
	case StringToInt("num0"):
		return VK_NUMPAD0;

		break;
	case StringToInt("num1"):
		return VK_NUMPAD1;

		break;
	case StringToInt("num2"):
		return VK_NUMPAD2;

		break;
	case StringToInt("num3"):
		return VK_NUMPAD3;

		break;
	case StringToInt("num4"):
		return VK_NUMPAD4;

		break;
	case StringToInt("num5"):
		return VK_NUMPAD5;

		break;
	case StringToInt("num6"):
		return VK_NUMPAD6;

		break;
	case StringToInt("num7"):
		return VK_NUMPAD7;

		break;
	case StringToInt("num8"):
		return VK_NUMPAD8;

		break;
	case StringToInt("num9"):
		return VK_NUMPAD9;

		break;
	case StringToInt("multiply"):
		return VK_MULTIPLY;

		break;
	case StringToInt("add"):
		return VK_ADD;

		break;
	case StringToInt("separator"):
		return VK_SEPARATOR;

		break;
	case StringToInt("subtract"):
		return VK_SUBTRACT;

		break;
	case StringToInt("decimal"):
		return VK_DECIMAL;

		break;
	case StringToInt("divide"):
		return VK_DIVIDE;

		break;
	case StringToInt("f1"):
		return VK_F1;

		break;
	case StringToInt("f2"):
		return VK_F2;

		break;
	case StringToInt("f3"):
		return VK_F3;

		break;
	case StringToInt("f4"):
		return VK_F4;

		break;
	case StringToInt("f5"):
		return VK_F5;

		break;
	case StringToInt("f6"):
		return VK_F6;

		break;
	case StringToInt("f7"):
		return VK_F7;

		break;
	case StringToInt("f8"):
		return VK_F8;

		break;
	case StringToInt("f9"):
		return VK_F9;

		break;
	case StringToInt("f10"):
		return VK_F10;

		break;
	case StringToInt("f11"):
		return VK_F11;

		break;
	case StringToInt("f12"):
		return VK_F12;

		break;
	case StringToInt("f13"):
		return VK_F13;

		break;
	case StringToInt("f14"):
		return VK_F14;

		break;
	case StringToInt("f15"):
		return VK_F15;

		break;
	case StringToInt("f16"):
		return VK_F16;

		break;
	case StringToInt("f17"):
		return VK_F17;

		break;
	case StringToInt("f18"):
		return VK_F18;

		break;
	case StringToInt("f19"):
		return VK_F19;

		break;
	case StringToInt("f20"):
		return VK_F20;

		break;
	case StringToInt("f21"):
		return VK_F21;

		break;
	case StringToInt("f22"):
		return VK_F22;

		break;
	case StringToInt("f23"):
		return VK_F23;

		break;
	case StringToInt("f24"):
		return VK_F24;

		break;
	case StringToInt("numlock"):
		return VK_NUMLOCK;

		break;
	case StringToInt("scroll"):
		return VK_SCROLL;

		break;
	case StringToInt("lshift"):
		return VK_LSHIFT;

		break;
	case StringToInt("rshift"):
		return VK_RSHIFT;

		break;
	case StringToInt("lcontrol"):
		return VK_LCONTROL;

		break;
	case StringToInt("rcontrol"):
		return VK_RCONTROL;

		break;
	case StringToInt("lmenu"):
		return VK_LMENU;

		break;
	case StringToInt("rmenu"):
		return VK_RMENU;

		break;

	default:
		// If none of that bullshit above, just return normal key
		if (std::string(keyname).size() == 1) {
			return keyname[0];
		}

		break;
	}

	return 0;
}