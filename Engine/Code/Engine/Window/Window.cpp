#include "Engine\Window\Window.hpp"
#include "Engine\Core\WindowsCommon.hpp"

#define GAME_WINDOW_CLASS (TEXT( "Simple Window Class" ))

static Window* g_theWindow = nullptr; 

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	bool run_default = true; 
	Window *window = Window::GetInstance(); 
	if (nullptr != window) {
		for (windows_message_handler_cb iter : window->m_messageHandlers) {
			run_default = iter( wmMessageCode, wParam, lParam ) && run_default;  
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	if (run_default) {
		return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
	} else {
		return 0; 
	}
}

static void RegisterWindowClass() 
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = GAME_WINDOW_CLASS;
	RegisterClassEx( &windowClassDescription );
}

Window::Window( char const *appName, float clientAspect )
{
	RegisterWindowClass(); 

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	//windows has (0,0) at top left
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	//store off height and width for accessing later (prevents having to calculate)
	m_clientHeight = clientHeight;
	m_clientWidth = clientWidth;
	m_aspectRatio = m_clientWidth/m_clientHeight; //calculate aspect ratio after height and width are calculated

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, appName, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		GAME_WINDOW_CLASS,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		GetModuleHandle( NULL ),
		NULL );

	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	m_hwnd = (void*)hwnd; 
}

Window::~Window()
{
	::DestroyWindow( (HWND)m_hwnd ); 

	::UnregisterClass( GAME_WINDOW_CLASS, GetModuleHandle(NULL) ); 
	m_hwnd = nullptr; 
	g_theWindow = nullptr; 
}

void Window::AddHandler( windows_message_handler_cb cb )
{
	m_messageHandlers.push_back( cb ); 
}


Window* Window::CreateInstance( char const *title, float aspect ) 
{
	if (g_theWindow == nullptr) {
		g_theWindow = new Window( title, aspect ); 
	}
	return g_theWindow; 
}

Window* Window::GetInstance()
{
	return g_theWindow; 
}

AABB2 Window::GetClientWindow()
{
	HWND hwnd = (HWND)Window::GetInstance()->GetHandle();  // Get your windows HWND
	RECT windowRect;
	//LPRECT windowRect = new LPRECT();

	::GetClientRect(hwnd, &windowRect);

	//windows has (0,0) top left
	AABB2 windowAABB2 = AABB2(Vector2((float)windowRect.left, (float)windowRect.top), Vector2((float)windowRect.right, (float)windowRect.bottom));

	return windowAABB2;
}

Vector2 Window::GetCenterOfClientWindow()
{
	AABB2 clientWindow = GetClientWindow();
	return clientWindow.GetCenter();
}

