#pragma once
#include "engine\input\KeyButtonState.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\IntVector2.hpp"


enum MouseMode
{
	MOUSE_ABSOLUTE_MODE, // Absolute Mode - I get mouse position - and I can potentially lock to the screen
	MOUSE_RELATIVE_MODE, // Relative mode -> I care about deltas - I reset to the center (meaning, mutually exclusive modes)
	NUM_MOUSE_MODES
};

enum MouseButtons
{
	MOUSE_LEFT_BUTTON,
	MOUSE_RIGHT_BUTTON,
	MOUSE_MIDDLE_BUTTON,
	MOUSE_EXTRA1_BUTTON,
	MOUSE_EXTRA2_BUTTON,
	NUM_MOUSE_BUTTONS
};

//enum MouseMode
//{
//	RELATIVE_MOUSE_MODE
//};

class Mouse
{
public:
	Mouse();
	void ProcessInput(size_t wParam);
	void UpdateMouse();
	void MouseShowCursor(bool show);
	void MouseLockToScreen(bool lock);
	void SetMouseScreenPosition(IntVector2 desktopPosition);
	void SetMousePosition(Vector2 clientPosition);	
	void SetMouseMode(MouseMode mode);

	MouseMode GetMouseMode();

	Vector2 GetMouseClientPosition();
	Vector2 GetMouseDelta();
	//void UpdateButton(int mouseInputName, unsigned char moustInputValue);
	//void UpdateKeyStateButtons();

public:
	KeyButtonState m_buttonStates[5];
	//float m_frameWheelDelta = 0.f;
	Vector2 m_mousePositionLastFrame;
	Vector2 m_mousePositionThisFrame;
	MouseMode m_currentMouseMode = MOUSE_ABSOLUTE_MODE;
};


