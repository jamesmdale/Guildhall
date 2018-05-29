#pragma once

class AnalogJoyStick
{
public:
	AnalogJoyStick(){};

public:
	float m_positionX;
	float m_positionY;
	float m_innerDeadZone;
	float m_outerDeadZone;
	float m_magnitude;
	float m_angleDegrees;
	float m_correctedPositionX;
	float m_correctedPositionY;
};