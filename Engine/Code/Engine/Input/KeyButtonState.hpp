#pragma once
class KeyButtonState
{
public:
	KeyButtonState();	
public:
	bool m_isDown;
	bool m_wasJustPressed;
	bool m_wasJustReleased;
};
