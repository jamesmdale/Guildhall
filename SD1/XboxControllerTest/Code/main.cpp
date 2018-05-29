//---------------------------------------------------------------------------
// XboxControllerTest02
//
// A simple test program to demonstrate reading connectivity and state from
//	USB Xbox controllers (works for Xbox 360 or Xbox One controllers).
//
#include <stdio.h>
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility


//---------------------------------------------------------------------------
// "controllerNumber" can be 0,1,2,3; API supports up to 4 Xbox controllers at once
void PrintXboxControllerState( int controllerNumber )
{
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( controllerNumber, &xboxControllerState );
	if( errorStatus == ERROR_SUCCESS )
	{
		printf( "XBox controller #%d reports:                                        \n", controllerNumber );
		printf( "  wButtons=%5d (0x%08x), bLeftTrigger=%3d, bRightTrigger=%3d        \n",
			xboxControllerState.Gamepad.wButtons, xboxControllerState.Gamepad.wButtons,
			xboxControllerState.Gamepad.bLeftTrigger, xboxControllerState.Gamepad.bRightTrigger );
		printf( "  sThumbLX=%6d    sThumbLY=%6d                                \n", xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY );
		printf( "  sThumbRX=%6d    sThumbRY=%6d                                \n", xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY );

		bool isButtonADown = (xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A;
		printf( "Button A is %s                                                      \n", isButtonADown ? "DOWN" : "UP" );
	}
	else if( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		printf( "Xbox controller #%d is not connected.                                \n", controllerNumber );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
	}
	else
	{
		printf( "Xbox controller #%d reports error code %u (0x%08x).                  \n", controllerNumber, errorStatus, errorStatus );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
		printf( "                                                                    \n" );
	}
	printf( "\n" );
}


//---------------------------------------------------------------------------
int main( int, char** )
{
	system( "cls" );
	HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD cursorPosition = { 0, 0 };
	for( ;; )
	{
		SetConsoleCursorPosition( consoleHandle, cursorPosition );

		PrintXboxControllerState( 0 );
		PrintXboxControllerState( 1 );
		PrintXboxControllerState( 2 );
		PrintXboxControllerState( 3 );

		Sleep( 10 ); // reduce cursor jump flickering
	}
}

