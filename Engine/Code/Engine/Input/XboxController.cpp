
#include "engine\input\XboxController.hpp"
#include "engine\math\MathUtils.hpp"
#include "engine\math\Vector2.hpp"

XboxController::XboxController()
{
	memset(&controllerState, 0, sizeof(controllerState));
	m_controllerId = 0;
}

void XboxController::UpdateKeyStateButtons()
{
	for(int i = 0; i < NUM_XBOX_BUTTONS; i++)
	{
		m_buttonStates[i].m_wasJustPressed = false;
		m_buttonStates[i].m_wasJustReleased = false;
	}

	UpdateButton(XBOX_DPAD_UP, XINPUT_GAMEPAD_DPAD_UP);
	UpdateButton(XBOX_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_LEFT);
	UpdateButton(XBOX_DPAD_RIGHT, XINPUT_GAMEPAD_DPAD_RIGHT);
	UpdateButton(XBOX_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_DOWN);
	UpdateButton(XBOX_START, XINPUT_GAMEPAD_START);
	UpdateButton(XBOX_BACK, XINPUT_GAMEPAD_BACK);
	UpdateButton(XBOX_LEFT_THUMB, XINPUT_GAMEPAD_LEFT_THUMB);
	UpdateButton(XBOX_RIGHT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB);
	UpdateButton(XBOX_LEFT_SHOULDER, XINPUT_GAMEPAD_LEFT_SHOULDER);
	UpdateButton(XBOX_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	UpdateButton(XBOX_A, XINPUT_GAMEPAD_A);
	UpdateButton(XBOX_B, XINPUT_GAMEPAD_B);
	UpdateButton(XBOX_X, XINPUT_GAMEPAD_X);
	UpdateButton(XBOX_Y, XINPUT_GAMEPAD_Y);	
}

void XboxController::UpdateButton(int xboxInputName, unsigned short xInputValue)
{
	if((controllerState.Gamepad.wButtons) & (xInputValue))
	{
		if(!m_buttonStates[xboxInputName].m_isDown)
		{
			m_buttonStates[xboxInputName].m_wasJustPressed = true;
		}
		m_buttonStates[xboxInputName].m_isDown = true;		
	}
	else
	{
		if(m_buttonStates[xboxInputName].m_isDown)
		{
			m_buttonStates[xboxInputName].m_wasJustReleased = true;
		}
		m_buttonStates[xboxInputName].m_isDown = false;
	}
}

void XboxController::UpdateTriggers()
{
	m_leftTriggerValue = controllerState.Gamepad.bLeftTrigger / 255.f;
	m_rightTriggerValue = controllerState.Gamepad.bRightTrigger / 255.f;
}

void XboxController::UpdateThumbSticks()
{
	m_joyStickLeft.m_positionX = controllerState.Gamepad.sThumbLX;
	m_joyStickLeft.m_positionY = controllerState.Gamepad.sThumbLY;
	m_joyStickRight.m_positionX = controllerState.Gamepad.sThumbRX;
	m_joyStickRight.m_positionY = controllerState.Gamepad.sThumbRY;

	//adjust left stick for deadzones and then set position
	float stickLeftXFloat = RangeMapFloat(m_joyStickLeft.m_positionX, -32768, 32767, -1, 1 );
	float stickLeftYFloat = RangeMapFloat(m_joyStickLeft.m_positionY, -32768, 32767, -1, 1 );
	float radiusLeftStick = GetRadius(Vector2(stickLeftXFloat, stickLeftYFloat));
	float leftStickTheta = AtanfAsDegrees(stickLeftYFloat, stickLeftXFloat);	
	float correctedRadiusLeftStick = RangeMapFloat(radiusLeftStick, .3f, .9f, 0.0f, 1.0f);
	float clampedRadiusLeftStick = ClampFloatZeroToOne(correctedRadiusLeftStick);	


	m_joyStickLeft.m_innerDeadZone = .3f;
	m_joyStickLeft.m_outerDeadZone = .9f;
	m_joyStickLeft.m_magnitude = radiusLeftStick;
	m_joyStickLeft.m_angleDegrees = leftStickTheta;
	m_joyStickLeft.m_correctedPositionX = clampedRadiusLeftStick * CosDegrees(leftStickTheta);
	m_joyStickLeft.m_correctedPositionY = clampedRadiusLeftStick * SinDegrees(leftStickTheta);
	

	//adjust right stick for deadzones and then set position
	float stickRightXFloat = RangeMapFloat(m_joyStickRight.m_positionX, -32768, 32767, -1, 1 );
	float stickRightYFloat = RangeMapFloat(m_joyStickRight.m_positionY, -32768, 32767, -1, 1 );
	float radiusRightStick = GetRadius(Vector2(stickRightXFloat, stickRightYFloat));

	float rightStickTheta = AtanfAsDegrees(stickRightYFloat, stickRightXFloat);
	float correctedRadiusRightStick = RangeMapFloat(radiusRightStick, .3f, .9f, 0.0f, 1.0f);
	float clampedRadiusRightStick = ClampFloatZeroToOne(correctedRadiusRightStick);

	m_joyStickRight.m_innerDeadZone = .3f;
	m_joyStickRight.m_outerDeadZone = .9f;
	m_joyStickRight.m_magnitude = radiusRightStick;
	m_joyStickRight.m_angleDegrees = rightStickTheta;
	m_joyStickRight.m_correctedPositionX = clampedRadiusRightStick * CosDegrees(rightStickTheta);
	m_joyStickRight.m_correctedPositionY = clampedRadiusRightStick * SinDegrees(rightStickTheta);	
}

void XboxController::UpdateController()
{
	UpdateControllerConnection();

	if(m_isControllerConnected)
	{
		UpdateKeyStateButtons();
		UpdateThumbSticks();
		UpdateTriggers();
	}	
}

void XboxController::UpdateControllerConnection()
{
	DWORD Result = XInputGetState(m_controllerId, &controllerState);

	if(Result == ERROR_SUCCESS)
	{
		m_isControllerConnected = true;
	}
	else
	{
		m_isControllerConnected = false;
	}
}

bool XboxController::WasKeyJustPressed(unsigned char keyCode) const
{
	bool wasKeyJustPressed = m_buttonStates[keyCode].m_wasJustPressed;

	return wasKeyJustPressed;
}

bool XboxController::WasKeyJustReleased(unsigned char keyCode) const
{
	bool wasKeyJustReleased = m_buttonStates[keyCode].m_wasJustReleased;

	return wasKeyJustReleased;
}
bool XboxController::IsKeyPressed(unsigned char keyCode) const
{
	bool isKeyPressedDown = m_buttonStates[keyCode].m_isDown;

	return isKeyPressedDown;
}
