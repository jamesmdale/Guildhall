#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <vector>
#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include <minwindef.h>


bool AppMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam ) 
{
	UNUSED(lParam); //set this for now to prevent annoying warnings.
	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			g_isQuitting = true;
			return false; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;
			InputSystem::GetInstance()->ProcessKeyDown(asKey);

			// If ESC, Quit the app
			/*if(asKey == VK_ESCAPE && !(DevConsole::GetInstance()->IsOpen())) 
			{
			g_isQuitting = true;
			return false;
			}*/

			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			InputSystem::GetInstance()->ProcessKeyUp(asKey);
			return false;
			break;
		}
		
		//mouse info
		case WM_ACTIVATE:
		{
			bool is_active = (WA_INACTIVE != LOWORD( wParam ));
			if (InputSystem::GetInstance()->GetMouse()->GetMouseMode() == MOUSE_RELATIVE_MODE)
			{
				if (Window::GetInstance()->GetHandle() == HWND(lParam))
				{
					InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(!is_active);
				}
				else
				{
					InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(is_active);
				}
			}			
		}

		case WM_LBUTTONDBLCLK:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}

		case WM_LBUTTONDOWN:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}

		case WM_LBUTTONUP:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}

		case WM_RBUTTONUP:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}

		case WM_RBUTTONDBLCLK:
		{
			InputSystem::GetInstance()->ProcessMouseInput(wParam);
			break;
		}
	}

	return true; 
}

//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(float clientAspect )
{
	Window* window = Window::CreateInstance( APP_NAME, clientAspect ); 
	InputSystem::CreateInstance(); //post startup to follow if necessary later

	window->AddHandler( AppMessageHandler ); 
}

void Initialize()
{
	g_theApp = new TheApp();
	CreateOpenGLWindow( CLIENT_ASPECT );

	RenderStartup();

	Renderer::CreateInstance();
	Renderer::GetInstance()->PostStartup();

	DevConsole::CreateInstance();
	DevConsole::GetInstance()->Startup();

	DebugRender::CreateInstance();
	DebugRender::GetInstance()->Initialize();

	g_theApp->Initialize();
}

//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	g_theApp->RunFrame();
}


void Shutdown()
{
	GLShutdown();

	// Destroy the global App instance	
	delete g_theApp;			
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	UNUSED(applicationInstanceHandle);
	Initialize();

	// Program main loop; keep running frames until it's time to quit
	while( !g_isQuitting)
	{
		RunFrame();
		Sleep(1);
	}

	Shutdown();
	return 0;
}


