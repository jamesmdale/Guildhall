#pragma once
#include "engine\input\KeyButtonState.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "engine\input\KeyButtonState.hpp"


enum MouseMode
{
	MOUSE_ABSOLUTE_MODE, // Absolute Mode - I get mouse position - and I can potentially lock to the screen
	MOUSE_RELATIVE_MODE, // Relative mode - I care about deltas - I reset to the center (meaning, mutually exclusive modes)
	NUM_MOUSE_MODES
};

class Mouse
{
public:
	Mouse();
	void UpdateMousePosition();
	void MouseShowCursor(bool show);
	void MouseLockToScreen(bool lock);
	void SetMouseScreenPosition(IntVector2 desktopPosition);
	void SetMousePosition(Vector2 clientPosition);	
	void SetMouseMode(MouseMode mode);

	MouseMode GetMouseMode();

	Vector2 GetMouseClientPosition();
	Vector2 GetInvertedMouseClientPosition();
	Vector2 GetMouseDelta();

public:
	bool m_doubleClickLeft = false;
	bool m_doubleClickRight = false;
	//float m_frameWheelDelta = 0.f;
	Vector2 m_mousePositionLastFrame;
	Vector2 m_mousePositionThisFrame;
	bool m_isMouseLocked = false;
	MouseMode m_currentMouseMode = MOUSE_ABSOLUTE_MODE;
};


