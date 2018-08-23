#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Input\Mouse.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\AABB2.hpp"
#include <windef.h>

Mouse::Mouse()
{
}


Vector2 Mouse::GetMouseClientPosition()
{
	POINT desktop_pos;
	::GetCursorPos( &desktop_pos ); 

	HWND hwnd = (HWND)Window::GetInstance()->GetHandle(); 

	::ScreenToClient( hwnd, &desktop_pos ); 
	POINT client_pos = desktop_pos; 

	Vector2 dimensions = Window::GetInstance()->GetClientDimensions();

	return Vector2( (float)client_pos.x, (float)client_pos.y ); 
}

Vector2 Mouse::GetInvertedMouseClientPosition()
{
	Vector2 clientPosition = GetMouseClientPosition();

	Vector2 dimensions = Window::GetInstance()->GetClientDimensions();

	//invert client y
	return Vector2(clientPosition.x, dimensions.y - clientPosition.y);
}
;

Vector2 Mouse::GetMouseDelta()
{
	// m_position_last_frame; this is known
	// m_position_this_frame; // this is also known
	return m_mousePositionThisFrame - m_mousePositionLastFrame;
}

void Mouse::UpdateMousePosition()
{
	Window* theWindow = Window::GetInstance();

	HWND hwnd = GetActiveWindow();
	if (theWindow->GetHandle() == hwnd)
	{
		m_mousePositionLastFrame = m_mousePositionThisFrame; 
		m_mousePositionThisFrame = GetMouseClientPosition();

		if (m_currentMouseMode == MOUSE_RELATIVE_MODE)
		{
			m_mousePositionLastFrame = theWindow->GetCenterOfClientWindow();		
			SetMousePosition(m_mousePositionLastFrame);				
		}
	}
}

void Mouse::MouseShowCursor( bool show ) 
{
	::ShowCursor( show ); 
	::ShowCursor( show );
	::ShowCursor( show );
	::ShowCursor( show );
	::ShowCursor( show );
}


void Mouse::MouseLockToScreen( bool lock ) 
{
	if (lock == false) 
	{
		::ClipCursor( nullptr ); // this unlock the mouse
		m_isMouseLocked = false;
	} 
	else 
	{
		HWND hwnd = (HWND)Window::GetInstance()->GetHandle();  // Get your windows HWND
		m_isMouseLocked = true;

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

TODO("Move lock region with window"); 

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
	SetMousePosition(Window::GetInstance()->GetCenterOfClientWindow());
	m_currentMouseMode = mode;
}

MouseMode Mouse::GetMouseMode()
{
	return m_currentMouseMode;
}
;
