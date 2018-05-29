#include "Game\Menu.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Game\Game.hpp"

Vector2 menuLeftStickCorrectXY = Vector2(0.f, 0.f);

void Menu::Initialize()
{
	//initialize stuff
}

void Menu::Update(float deltaSeconds)
{
	UpdateFromInput(deltaSeconds);
}

void Menu::Render()
{
	Rgba playColor = Rgba::GRAY;
	Rgba quitColor = Rgba::GRAY;

	switch(m_selectedMenuOption)
	{
		case PLAY:
			playColor = Rgba::WHITE;
			break;
		case QUIT:
			quitColor = Rgba::WHITE;
			break;
	}

	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	AABB2 screenBounds = AABB2(0.f, 0.f, theWindow->m_clientWidth, theWindow->m_clientHeight);
	theRenderer->SetCamera(g_theGame->m_uiCamera);

	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS, true);	
	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	theRenderer->DrawAABB(screenBounds, Rgba(0.f, 0.f, 0.f, 1.f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), "Tactics", theWindow->m_clientHeight * .1f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), "Play", theWindow->m_clientHeight * .075f, playColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .25f), "Quit", theWindow->m_clientHeight * .075f, quitColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	theWindow = nullptr;
	delete(theWindow);
	theRenderer = nullptr;
	delete(theRenderer);
}

void Menu::ResetMenuState()
{
	m_selectedMenuOption = PLAY;
}

float Menu::UpdateFromInput(float deltaSeconds)
{
	if(DevConsole::GetInstance()->IsOpen())
	{
		return deltaSeconds;
	}

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(inputController.m_joyStickLeft.m_correctedPositionX  != 0.f && inputController.m_joyStickLeft.m_correctedPositionY != 0.f)
	{
		menuLeftStickCorrectXY.x = inputController.m_joyStickLeft.m_correctedPositionX;
		menuLeftStickCorrectXY.y = inputController.m_joyStickLeft.m_correctedPositionY;		
		float leftStickRadius = inputController.m_joyStickLeft.m_magnitude;

		if(leftStickRadius > .3f)
		{			
			if(menuLeftStickCorrectXY.y < 0.f && m_selectedMenuOption == PLAY)
			{
				m_selectedMenuOption = QUIT;
			}
			else if(menuLeftStickCorrectXY.y > 0.f && m_selectedMenuOption == QUIT)
			{
				m_selectedMenuOption = PLAY;
			}	
		}
	}
	else
	{
		menuLeftStickCorrectXY = Vector2(0.f, 0.f);
	}

	if(inputController.WasKeyJustPressed(XBOX_DPAD_DOWN))
	{
		if(m_selectedMenuOption == PLAY)
		{
			m_selectedMenuOption = QUIT;
		}
	}

	if(inputController.WasKeyJustPressed(XBOX_DPAD_UP))
	{
		if(m_selectedMenuOption == QUIT)
		{
			m_selectedMenuOption = PLAY;
		}
	}

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		switch(m_selectedMenuOption)
		{
			case(PLAY):
				ResetMenuState();
				g_theGame->TransitionGameStates(PLAYING_STATE);
				break;
			case(QUIT):
				g_isQuitting = true;
				break;
		}
	}

	if(theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE) || inputController.WasKeyJustPressed(XBOX_BACK))
	{
		g_isQuitting = true;
	}

	theInput = nullptr;
	delete(theInput);
	return deltaSeconds; //new deltaSeconds
}
