#pragma once
#include "Engine\Core\WindowsCommon.hpp"
#include "engine\input\KeyButtonState.hpp"
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0")
#include "engine\input\AnalogJoyStick.hpp"
class XboxController
{
public:
	XboxController();	
	void UpdateKeyStateButtons();	
	void UpdateButton(int xboxInputName, unsigned short xInputValue);
	void UpdateTriggers();
	void UpdateThumbSticks();
	void UpdateController();
	void UpdateControllerConnection();
	bool WasKeyJustPressed(unsigned char keyCode) const;
	bool WasKeyJustReleased(unsigned char keyCode) const;
	bool IsKeyPressed(unsigned char keyCode) const;
public:
	KeyButtonState m_buttonStates[14];
	AnalogJoyStick m_joyStickLeft;
	AnalogJoyStick m_joyStickRight;
	int m_controllerId;
	bool m_isControllerConnected;
	float m_leftTriggerValue;
	float m_rightTriggerValue;
	XINPUT_STATE controllerState;
};

enum XboxButtons
{
	XBOX_DPAD_UP,
	XBOX_DPAD_LEFT,
	XBOX_DPAD_RIGHT,
	XBOX_DPAD_DOWN ,
	XBOX_START,
	XBOX_BACK,
	XBOX_LEFT_THUMB ,
	XBOX_RIGHT_THUMB,
	XBOX_LEFT_SHOULDER,
	XBOX_RIGHT_SHOULDER,
	XBOX_A,
	XBOX_B, 
	XBOX_X ,
	XBOX_Y,
	NUM_XBOX_BUTTONS
};