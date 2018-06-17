#pragma once
#include "Engine\Input\KeyButtonState.hpp"
#include "Engine\Input\XboxController.hpp"
#include "Engine\Input\Mouse.hpp"
class InputSystem
{
private:
	InputSystem();

public:
	~InputSystem();
	static InputSystem* CreateInstance();
	static InputSystem* GetInstance();
	Mouse* GetMouse();
	void BeginFrame();
	void EndFrame();
	void ProcessKeyDown(unsigned char keyCode);
	void ProcessKeyUp(unsigned char keyCode);

	void ProcessMouseButtons(size_t wParam);

	bool GetMouseDoubleClickLeft();
	bool GetMouseDoubleClickRight();
	bool IsKeyPressed(unsigned char keyCode) const;	
	bool WasKeyJustPressed(unsigned char keycode) const;
	bool WasKeyJustReleased(unsigned char keyCode) const;

	XboxController& GetController(int controllerId){ return m_controllers[controllerId];}

public:
	bool m_isActiveWindow = true;

public:
	static const int NUM_KEYS = 256;
	static const int NUM_CONTROLLERS = 4;
	static const int NUM_MOUSE_BUTTONS = 5;

	static const unsigned char KEYBOARD_ESCAPE;
	static const unsigned char KEYBOARD_UP_ARROW;
	static const unsigned char KEYBOARD_RIGHT_ARROW;
	static const unsigned char KEYBOARD_LEFT_ARROW;
	static const unsigned char KEYBOARD_DOWN_ARROW;

	static const unsigned char KEYBOARD_SHIFT;
	static const unsigned char KEYBOARD_F1;
	static const unsigned char KEYBOARD_F2;
	static const unsigned char KEYBOARD_F3;
	static const unsigned char KEYBOARD_F4;
	static const unsigned char KEYBOARD_F5;
	static const unsigned char KEYBOARD_F6;
	static const unsigned char KEYBOARD_F7;
	static const unsigned char KEYBOARD_F8;
	static const unsigned char KEYBOARD_F9;
	static const unsigned char KEYBOARD_F10;
	static const unsigned char KEYBOARD_F11;
	static const unsigned char KEYBOARD_F12;
	static const unsigned char KEYBOARD_BACK;
	static const unsigned char KEYBOARD_TAB;
	static const unsigned char KEYBOARD_CLEAR;
	static const unsigned char KEYBOARD_RETURN;
	static const unsigned char KEYBOARD_CONTROL;
	static const unsigned char KEYBOARD_MENU;
	static const unsigned char KEYBOARD_PAUSE;
	static const unsigned char KEYBOARD_CAPSLOCK;
	static const unsigned char KEYBOARD_SPACE;
	static const unsigned char KEYBOARD_PAGEUP;
	static const unsigned char KEYBOARD_PAGEDOWN;
	static const unsigned char KEYBOARD_END;
	static const unsigned char KEYBOARD_HOME;
	static const unsigned char KEYBOARD_SELECT;
	static const unsigned char KEYBOARD_PRINT;
	static const unsigned char KEYBOARD_EXECUTE;
	static const unsigned char KEYBOARD_PRINT_SCREEN;
	static const unsigned char KEYBOARD_INSERT;
	static const unsigned char KEYBOARD_DELETE;
	static const unsigned char KEYBOARD_HELP;
	static const unsigned char KEYBOARD_0;
	static const unsigned char KEYBOARD_1;
	static const unsigned char KEYBOARD_2;
	static const unsigned char KEYBOARD_3;
	static const unsigned char KEYBOARD_4;
	static const unsigned char KEYBOARD_5;
	static const unsigned char KEYBOARD_6;
	static const unsigned char KEYBOARD_7;
	static const unsigned char KEYBOARD_8;
	static const unsigned char KEYBOARD_9;
	static const unsigned char KEYBOARD_A;
	static const unsigned char KEYBOARD_B;
	static const unsigned char KEYBOARD_C;
	static const unsigned char KEYBOARD_D;
	static const unsigned char KEYBOARD_E;
	static const unsigned char KEYBOARD_F;
	static const unsigned char KEYBOARD_G;
	static const unsigned char KEYBOARD_H;
	static const unsigned char KEYBOARD_I;
	static const unsigned char KEYBOARD_J;
	static const unsigned char KEYBOARD_K;
	static const unsigned char KEYBOARD_L;
	static const unsigned char KEYBOARD_M;
	static const unsigned char KEYBOARD_N;
	static const unsigned char KEYBOARD_O;
	static const unsigned char KEYBOARD_P;
	static const unsigned char KEYBOARD_Q;
	static const unsigned char KEYBOARD_R;
	static const unsigned char KEYBOARD_S;
	static const unsigned char KEYBOARD_T;
	static const unsigned char KEYBOARD_U;
	static const unsigned char KEYBOARD_V;
	static const unsigned char KEYBOARD_W;
	static const unsigned char KEYBOARD_X;
	static const unsigned char KEYBOARD_Y;
	static const unsigned char KEYBOARD_Z;
	static const unsigned char KEYBOARD_TILDE;
	static const unsigned char KEYBOARD_LEFT_WINDOWS;
	static const unsigned char KEYBOARD_RIGHT_WINDOWS;
	static const unsigned char KEYBOARD_SLEEP;
	static const unsigned char KEYBOARD_NUMPAD_0;
	static const unsigned char KEYBOARD_NUMPAD_1;
	static const unsigned char KEYBOARD_NUMPAD_2;
	static const unsigned char KEYBOARD_NUMPAD_3;
	static const unsigned char KEYBOARD_NUMPAD_4;
	static const unsigned char KEYBOARD_NUMPAD_5;
	static const unsigned char KEYBOARD_NUMPAD_6;
	static const unsigned char KEYBOARD_NUMPAD_7;
	static const unsigned char KEYBOARD_NUMPAD_8;
	static const unsigned char KEYBOARD_NUMPAD_9;

	static const unsigned char KEYBOARD_MULTIPLY;
	static const unsigned char KEYBOARD_DIVIDE;
	static const unsigned char KEYBOARD_ADD;
	static const unsigned char KEYBOARD_SUBTRACT;
	static const unsigned char KEYBOARD_DECIMAL;
	static const unsigned char KEYBOARD_SEPARATOR;
	static const unsigned char KEYBOARD_NUMLOCK;
	static const unsigned char KEYBOARD_SCROLLLOCK;
	static const unsigned char KEYBOARD_LEFT_SHIFT;
	static const unsigned char KEYBOARD_LEFT_CONTROL;
	static const unsigned char KEYBOARD_LEFT_MENU;
	static const unsigned char KEYBOARD_RIGHT_SHIFT;
	static const unsigned char KEYBOARD_RIGHT_CONTROL;
	static const unsigned char KEYBOARD_RIGHT_MENU;

	static const unsigned short XBOX_DPAD_UP;
	static const unsigned short XBOX_DPAD_LEFT;
	static const unsigned short XBOX_DPAD_RIGHT;
	static const unsigned short XBOX_DPAD_DOWN ;
	static const unsigned short XBOX_START;
	static const unsigned short XBOX_LEFT;
	static const unsigned short XBOX_LEFT_THUMB;
	static const unsigned short XBOX_RIGHT_THUMB;
	static const unsigned short XBOX_LEFT_SHOULDER;
	static const unsigned short XBOX_RIGHT_SHOULDER;
	static const unsigned short XBOX_A;
	static const unsigned short XBOX_B;
	static const unsigned short XBOX_X;
	static const unsigned short XBOX_Y;

	//mouse
	static const unsigned char MOUSE_LEFT_CLICK;
	static const unsigned char MOUSE_RIGHT_CLICK;
	static const unsigned char MOUSE_MIDDLE_BUTTON;
	static const unsigned char MOUSE_4_BUTTON;
	static const unsigned char MOUSE_5_BUTTON;

	//more keycodes can go here as necessary (browser buttons, media, etc)
	//xbox controller logic here
	//xbox controller options here as well

	protected:
		void UpdateKeyboard();
		void UpdateControllers();
		void UpdateMouse();

	protected:
		KeyButtonState m_keyStates[NUM_KEYS];
		KeyButtonState m_buttonStates[NUM_MOUSE_BUTTONS];
		XboxController m_controllers[NUM_CONTROLLERS];
		Mouse* m_mouse = nullptr;
};

void RunMessagePump();