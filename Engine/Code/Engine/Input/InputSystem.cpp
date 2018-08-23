#include "Engine\Core\WindowsCommon.hpp"
#include "InputSystem.hpp"

static InputSystem* g_theInput = nullptr;

//function keys
const unsigned char InputSystem::KEYBOARD_F1 = VK_F1;
const unsigned char InputSystem::KEYBOARD_F2 = VK_F2;
const unsigned char InputSystem::KEYBOARD_F3 = VK_F3;
const unsigned char InputSystem::KEYBOARD_F4 = VK_F4;
const unsigned char InputSystem::KEYBOARD_F5 = VK_F5;
const unsigned char InputSystem::KEYBOARD_F6 = VK_F6;
const unsigned char InputSystem::KEYBOARD_F7 = VK_F7;
const unsigned char InputSystem::KEYBOARD_F8 = VK_F8;
const unsigned char InputSystem::KEYBOARD_F9 = VK_F9;
const unsigned char InputSystem::KEYBOARD_F10 = VK_F10;
const unsigned char InputSystem::KEYBOARD_F11 = VK_F11;
const unsigned char InputSystem::KEYBOARD_F12 = VK_F12;



//main letters & numbers
const unsigned char InputSystem::KEYBOARD_0 = 0x30;
const unsigned char InputSystem::KEYBOARD_1 = 0x31;
const unsigned char InputSystem::KEYBOARD_2 = 0x32;
const unsigned char InputSystem::KEYBOARD_3 = 0x33;
const unsigned char InputSystem::KEYBOARD_4 = 0x34;
const unsigned char InputSystem::KEYBOARD_5 = 0x35;
const unsigned char InputSystem::KEYBOARD_6 = 0x36;
const unsigned char InputSystem::KEYBOARD_7 = 0x37;
const unsigned char InputSystem::KEYBOARD_8 = 0x38;
const unsigned char InputSystem::KEYBOARD_9 = 0x39;
const unsigned char InputSystem::KEYBOARD_A = 0x41;
const unsigned char InputSystem::KEYBOARD_B = 0x42;
const unsigned char InputSystem::KEYBOARD_C = 0x43;
const unsigned char InputSystem::KEYBOARD_D = 0x44;
const unsigned char InputSystem::KEYBOARD_E = 0x45;
const unsigned char InputSystem::KEYBOARD_F = 0x46;
const unsigned char InputSystem::KEYBOARD_G = 0x47;
const unsigned char InputSystem::KEYBOARD_H = 0x48;
const unsigned char InputSystem::KEYBOARD_I = 0x49;
const unsigned char InputSystem::KEYBOARD_J = 0x4A;
const unsigned char InputSystem::KEYBOARD_K = 0x4B;
const unsigned char InputSystem::KEYBOARD_L = 0x4C;
const unsigned char InputSystem::KEYBOARD_M = 0x4D;
const unsigned char InputSystem::KEYBOARD_N = 0x4E;
const unsigned char InputSystem::KEYBOARD_O = 0x4F;
const unsigned char InputSystem::KEYBOARD_P = 0x50;
const unsigned char InputSystem::KEYBOARD_Q = 0x51;
const unsigned char InputSystem::KEYBOARD_R = 0x52;
const unsigned char InputSystem::KEYBOARD_S = 0x53;
const unsigned char InputSystem::KEYBOARD_T = 0x54;
const unsigned char InputSystem::KEYBOARD_U = 0x55;
const unsigned char InputSystem::KEYBOARD_V = 0x56;
const unsigned char InputSystem::KEYBOARD_W = 0x57;
const unsigned char InputSystem::KEYBOARD_X = 0x58;
const unsigned char InputSystem::KEYBOARD_Y = 0x59;
const unsigned char InputSystem::KEYBOARD_Z = 0x5A;

//NUMPAD
const unsigned char InputSystem::KEYBOARD_NUMPAD_0 = VK_NUMPAD0;
const unsigned char InputSystem::KEYBOARD_NUMPAD_1 = VK_NUMPAD1;
const unsigned char InputSystem::KEYBOARD_NUMPAD_2 = VK_NUMPAD2;
const unsigned char InputSystem::KEYBOARD_NUMPAD_3 = VK_NUMPAD3;
const unsigned char InputSystem::KEYBOARD_NUMPAD_4 = VK_NUMPAD4;
const unsigned char InputSystem::KEYBOARD_NUMPAD_5 = VK_NUMPAD5;
const unsigned char InputSystem::KEYBOARD_NUMPAD_6 = VK_NUMPAD6;
const unsigned char InputSystem::KEYBOARD_NUMPAD_7 = VK_NUMPAD7;
const unsigned char InputSystem::KEYBOARD_NUMPAD_8 = VK_NUMPAD8;
const unsigned char InputSystem::KEYBOARD_NUMPAD_9 = VK_NUMPAD9;
const unsigned char InputSystem::KEYBOARD_MULTIPLY = VK_MULTIPLY;
const unsigned char InputSystem::KEYBOARD_DIVIDE = VK_DIVIDE;
const unsigned char InputSystem::KEYBOARD_ADD = VK_ADD;
const unsigned char InputSystem::KEYBOARD_SUBTRACT = VK_SUBTRACT;
const unsigned char InputSystem::KEYBOARD_DECIMAL = VK_DECIMAL;
const unsigned char InputSystem::KEYBOARD_SEPARATOR = VK_SEPARATOR;
const unsigned char InputSystem::KEYBOARD_NUMLOCK = VK_NUMLOCK;

//other keys
const unsigned char InputSystem::KEYBOARD_TILDE = 0xC0;
const unsigned char InputSystem::KEYBOARD_LEFT_WINDOWS = VK_LWIN;
const unsigned char InputSystem::KEYBOARD_RIGHT_WINDOWS = VK_RWIN;
const unsigned char InputSystem::KEYBOARD_SLEEP = VK_SLEEP;
const unsigned char InputSystem::KEYBOARD_SCROLLLOCK = VK_SCROLL;
const unsigned char InputSystem::KEYBOARD_LEFT_SHIFT = VK_LSHIFT;
const unsigned char InputSystem::KEYBOARD_RIGHT_SHIFT = VK_RSHIFT;
const unsigned char InputSystem::KEYBOARD_LEFT_CONTROL = VK_LCONTROL;
const unsigned char InputSystem::KEYBOARD_RIGHT_CONTROL = VK_RCONTROL;
const unsigned char InputSystem::KEYBOARD_LEFT_MENU = VK_LMENU;
const unsigned char InputSystem::KEYBOARD_RIGHT_MENU = VK_RMENU;
const unsigned char InputSystem::KEYBOARD_UP_ARROW = VK_UP;
const unsigned char InputSystem::KEYBOARD_LEFT_ARROW = VK_LEFT;
const unsigned char InputSystem::KEYBOARD_RIGHT_ARROW = VK_RIGHT;
const unsigned char InputSystem::KEYBOARD_DOWN_ARROW = VK_DOWN;
const unsigned char InputSystem::KEYBOARD_ESCAPE = VK_ESCAPE;
const unsigned char InputSystem::KEYBOARD_EXECUTE = VK_EXECUTE;
const unsigned char InputSystem::KEYBOARD_CAPSLOCK= VK_CAPITAL;
const unsigned char InputSystem::KEYBOARD_CLEAR = VK_CLEAR;
const unsigned char InputSystem::KEYBOARD_SHIFT = VK_SHIFT;
const unsigned char InputSystem::KEYBOARD_RETURN = VK_RETURN;
const unsigned char InputSystem::KEYBOARD_BACK = VK_BACK;
const unsigned char InputSystem::KEYBOARD_CONTROL = VK_CONTROL;
const unsigned char InputSystem::KEYBOARD_TAB = VK_TAB;
const unsigned char InputSystem::KEYBOARD_MENU = VK_MENU;
const unsigned char InputSystem::KEYBOARD_PAUSE = VK_PAUSE;
const unsigned char InputSystem::KEYBOARD_SPACE = VK_SPACE;
const unsigned char InputSystem::KEYBOARD_PAGEUP = VK_PRIOR;
const unsigned char InputSystem::KEYBOARD_PAGEDOWN = VK_NEXT;
const unsigned char InputSystem::KEYBOARD_END = VK_END;
const unsigned char InputSystem::KEYBOARD_HOME = VK_HOME;
const unsigned char InputSystem::KEYBOARD_SELECT = VK_SELECT;
const unsigned char InputSystem::KEYBOARD_PRINT = VK_PRINT;
const unsigned char InputSystem::KEYBOARD_PRINT_SCREEN = VK_SNAPSHOT;
const unsigned char InputSystem::KEYBOARD_INSERT = VK_INSERT;
const unsigned char InputSystem::KEYBOARD_DELETE = VK_DELETE;
const unsigned char InputSystem::KEYBOARD_HELP = VK_HELP;

//mouse keycodes
const unsigned char InputSystem::MOUSE_LEFT_CLICK = VK_LBUTTON;
const unsigned char InputSystem::MOUSE_RIGHT_CLICK = VK_RBUTTON;
const unsigned char InputSystem::MOUSE_MIDDLE_BUTTON = VK_MBUTTON;
const unsigned char InputSystem::MOUSE_4_BUTTON = VK_XBUTTON1;
const unsigned char InputSystem::MOUSE_5_BUTTON = VK_XBUTTON2;

//InputSystem::InputSystem()
//{
//	//set controller ids
//	for(int i = 0; i < NUM_CONTROLLERS; i++)
//	{
//		m_controllers[i].m_controllerId = i;
//	}
//}

InputSystem::InputSystem()
{
	m_mouse = new Mouse();
}

InputSystem::~InputSystem()
{
	delete(m_mouse);
	m_mouse = nullptr;
}

InputSystem* InputSystem::CreateInstance()
{
	if(g_theInput == nullptr)
	{
		g_theInput = new InputSystem();
	}

	return g_theInput;
}

InputSystem* InputSystem::GetInstance()
{
	return g_theInput;
}

Mouse* InputSystem::InputSystem::GetMouse()
{
	return m_mouse;
}

void InputSystem::BeginFrame()	
{
	if (m_isActiveWindow)
	{
		UpdateKeyboard();
		UpdateControllers();
		UpdateMouse();		
	}
	RunMessagePump();
}							

void InputSystem::EndFrame()
{
	//cleanup
}

//insert process key function
void InputSystem::UpdateKeyboard()
{
	//clear all flags for all keys
	for(int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++ keyCode)
	{
		InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustPressed = false;
		InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustReleased = false;
	}
}

//clear all flags for all controller buttons
void InputSystem::UpdateControllers()
{
	for(int i = 0; i < NUM_CONTROLLERS; i++)
	{
		if(i == 0)
		{
			InputSystem::GetInstance()->m_controllers[i].UpdateController();
		}		
	}
}

//mouse handling
void InputSystem::UpdateMouse()
{
	for (int keyCode = 0; keyCode < InputSystem::NUM_MOUSE_BUTTONS; ++keyCode)
	{
		InputSystem::GetInstance()->m_buttonStates[keyCode].m_wasJustPressed = false;
		InputSystem::GetInstance()->m_buttonStates[keyCode].m_wasJustReleased = false;
	}

	InputSystem::GetInstance()->GetMouse()->m_doubleClickLeft = false;
	InputSystem::GetInstance()->GetMouse()->m_doubleClickRight = false;

	InputSystem::GetInstance()->GetMouse()->UpdateMousePosition();
}


//keyboard handling
bool InputSystem::WasKeyJustPressed(unsigned char keyCode) const
{
	bool wasKeyJustPressed = InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustPressed;

	return wasKeyJustPressed;
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	bool wasKeyJustReleased = InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustReleased;

	return wasKeyJustReleased;
}


bool InputSystem::IsKeyPressed(unsigned char keyCode) const
{
	bool isKeyPressedDown = InputSystem::GetInstance()->m_keyStates[keyCode].m_isDown;

	return isKeyPressedDown;
}

void InputSystem::ProcessKeyDown(unsigned char keyCode)
{
	if(InputSystem::GetInstance()->m_keyStates[keyCode].m_isDown == false)
	{
		InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustPressed = true;
	}
	
	InputSystem::GetInstance()->m_keyStates[keyCode].m_isDown = true;
}

void InputSystem::ProcessKeyUp(unsigned char keyCode)
{
	InputSystem::GetInstance()->m_keyStates[keyCode].m_wasJustReleased = true;
	InputSystem::GetInstance()->m_keyStates[keyCode].m_isDown = false;
}

void InputSystem::ProcessMouseButtons(size_t wParam)
{
	//process left mouse
	if (wParam & MK_LBUTTON)	
		ProcessKeyDown(VK_LBUTTON);
	else if(IsKeyPressed(VK_LBUTTON))
		ProcessKeyUp(VK_LBUTTON);

	//process right mouse
	if (wParam & MK_RBUTTON)
		ProcessKeyDown(VK_RBUTTON);
	else if(IsKeyPressed(VK_RBUTTON))
		ProcessKeyUp(VK_RBUTTON);

	//process middle mouse
	if (wParam & MK_MBUTTON)
		ProcessKeyDown(VK_MBUTTON);
	else if(IsKeyPressed(VK_MBUTTON))
		ProcessKeyUp(VK_MBUTTON);

	//process mouse 4
	if (wParam & MK_XBUTTON1)
		ProcessKeyDown(VK_XBUTTON1);
	else if(IsKeyPressed(VK_XBUTTON1))
		ProcessKeyUp(VK_XBUTTON1);

	//process mouse 5
	if (wParam & MK_XBUTTON2)
		ProcessKeyDown(VK_XBUTTON2);
	else if(IsKeyPressed(VK_XBUTTON2))
		ProcessKeyUp(VK_XBUTTON2);
}

bool InputSystem::GetMouseDoubleClickLeft()
{
	return GetMouse()->m_doubleClickLeft;
}

bool InputSystem::GetMouseDoubleClickRight()
{
	return GetMouse()->m_doubleClickRight;
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" function
	}
}