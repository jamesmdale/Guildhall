#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine\Input\Mouse.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\AABB2.hpp"
#include <windef.h>

Mouse::Mouse()
{
}

TODO("Move lock region with window"); 

void Mouse::ProcessInput(size_t wParam)
{
	TODO("Finish mouse input for buttons");
	unsigned char asKey = (unsigned char) wParam;
	switch(asKey)
	{
		case MK_CONTROL:
		{
			break;
		}
	}
}
//
//void Mouse::UpdateKeyStateButtons()
//{
//	for(int i = 0; i < NUM_MOUSE_BUTTONS; i++)
//	{
//		m_buttonStates[i].m_wasJustPressed = false;
//		m_buttonStates[i].m_wasJustReleased = false;
//	}
//
//	UpdateButton(MOUSE_LEFT_BUTTON, VK_LBUTTON);
//	UpdateButton(MOUSE_RIGHT_BUTTON, VK_RBUTTON);
//	UpdateButton(MOUSE_MIDDLE_BUTTON, VK_MBUTTON);
//	UpdateButton(MOUSE_EXTRA1_BUTTON, VK_XBUTTON1);
//	UpdateButton(MOUSE_EXTRA2_BUTTON, VK_XBUTTON2);
//
//}	


//void Mouse::UpdateButton(int mouseInputName, unsigned char moustInputValue)
//{
//	if((controllerState.Gamepad.wButtons) & (xInputValue))
//	{
//		if(!m_buttonStates[xboxInputName].m_isDown)
//		{
//			m_buttonStates[xboxInputName].m_wasJustPressed = true;
//		}
//		m_buttonStates[xboxInputName].m_isDown = true;		
//	}
//	else
//	{
//		if(m_buttonStates[xboxInputName].m_isDown)
//		{
//			m_buttonStates[xboxInputName].m_wasJustReleased = true;
//		}
//		m_buttonStates[xboxInputName].m_isDown = false;
//	}
//}


Vector2 Mouse::GetMouseClientPosition()
{
	POINT desktop_pos;
	::GetCursorPos( &desktop_pos ); 

	HWND hwnd = (HWND)Window::GetInstance()->GetHandle(); 

	::ScreenToClient( hwnd, &desktop_pos ); 
	POINT client_pos = desktop_pos; 

	return Vector2( (float)client_pos.x, (float)client_pos.y ); 
};

Vector2 Mouse::GetMouseDelta()
{
	// m_position_last_frame; this is known
	// m_position_this_frame; // this is also known
	return m_mousePositionThisFrame - m_mousePositionLastFrame;
}

void Mouse::UpdateMouse()
{
	Window* theWindow = Window::GetInstance();

	m_mousePositionLastFrame = m_mousePositionThisFrame; 
	m_mousePositionThisFrame = GetMouseClientPosition();

	switch(m_currentMouseMode)
	{
	case MOUSE_ABSOLUTE_MODE:
		//might want to do additional stuff later for the 'default' mode
		break;

	case MOUSE_RELATIVE_MODE:
		m_mousePositionLastFrame = theWindow->GetCenterOfClientWindow();
		SetMousePosition( m_mousePositionLastFrame ); 
		break;
	}	
}

void Mouse::MouseShowCursor( bool show ) 
{
	::ShowCursor( show ); 
}


void Mouse::MouseLockToScreen( bool lock ) 
{
	if (lock == false) 
	{
		::ClipCursor( nullptr ); // this unlock the mouse
		m_currentMouseMode = MOUSE_ABSOLUTE_MODE;
	} 
	else 
	{
		HWND hwnd = (HWND)Window::GetInstance()->GetHandle();  // Get your windows HWND
		m_currentMouseMode = MOUSE_RELATIVE_MODE;

		RECT clientRect; // window class RECDT
		::GetClientRect( hwnd, &clientRect ); 

		POINT offset; 
		offset.x = 0; 
		offset.y = 0; 
		::ClientToScreen( hwnd, &offset ); 

		clientRect.left += offset.x; 
		clientRect.right += offset.x; 
		clientRect.top += offset.y; 
		clientRect.bottom += offset.y; 

		::ClipCursor( &clientRect ); 
	}
}

void Mouse::SetMouseScreenPosition( IntVector2 desktopPosition )
{
	::SetCursorPos( desktopPosition.x, desktopPosition.y ); 
}

void Mouse::SetMousePosition( Vector2 clientPosition ) 
{
	// Exercise to the reader
	// Convert client to screen position, call above method

	HWND hwnd = (HWND)Window::GetInstance()->GetHandle();  // Get your windows HWND
	POINT position;
	position.x = (long)clientPosition.x;
	position.y = (long)clientPosition.y;
	::ClientToScreen(hwnd, &position);

	::SetCursorPos(position.x, position.y);
}

void Mouse::SetMouseMode(MouseMode mode)
{
	m_currentMouseMode = mode;
}

MouseMode Mouse::GetMouseMode()
{
	return m_currentMouseMode;
}
;
