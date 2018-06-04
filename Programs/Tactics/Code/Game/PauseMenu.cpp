#include "Game\PauseMenu.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Game\Game.hpp"


Vector2 menuLeftStickCorrectXYPause = Vector2(0.f, 0.f);

void PauseMenu::Initialize()
{
	//initialize stuff
}

void PauseMenu::Update(float deltaSeconds)
{
	UpdateFromInput(deltaSeconds);
}

void PauseMenu::Render()
{
	Rgba playColor = Rgba::GRAY;
	Rgba quitColor = Rgba::GRAY;

	switch(m_selectedMenuOption)
	{
	case RESUME:
		playColor = Rgba::WHITE;
		break;
	case EXIT:
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

	theRenderer->DrawAABB(screenBounds, Rgba(0.f, 0.f, 0.f, .25f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), "Game Paused", theWindow->m_clientHeight * .1f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), "Resume", theWindow->m_clientHeight * .075f, playColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .25f), "EXIT", theWindow->m_clientHeight * .075f, quitColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	theWindow = nullptr;
	delete(theWindow);
	theRenderer = nullptr;
	delete(theRenderer);
}

void PauseMenu::ResetMenuState()
{
	m_selectedMenuOption = RESUME;
}

float PauseMenu::UpdateFromInput(float deltaSeconds)
{
	if(DevConsole::GetInstance()->IsOpen())
	{
		return deltaSeconds;
	}

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	if(inputController.m_joyStickLeft.m_correctedPositionX  != 0.f && inputController.m_joyStickLeft.m_correctedPositionY != 0.f)
	{
		menuLeftStickCorrectXYPause.x = inputController.m_joyStickLeft.m_correctedPositionX;
		menuLeftStickCorrectXYPause.y = inputController.m_joyStickLeft.m_correctedPositionY;		
		float leftStickRadius = inputController.m_joyStickLeft.m_magnitude;

		if(leftStickRadius > .3f)
		{			
			if(menuLeftStickCorrectXYPause.y < 0.f && m_selectedMenuOption == RESUME)
			{
				m_selectedMenuOption = EXIT;
			}
			else if(menuLeftStickCorrectXYPause.y > 0.f && m_selectedMenuOption == EXIT)
			{
				m_selectedMenuOption = RESUME;
			}	
		}
	}
	else
	{
		menuLeftStickCorrectXYPause = Vector2(0.f, 0.f);
	}

	if(inputController.WasKeyJustPressed(XBOX_DPAD_DOWN))
	{
		if(m_selectedMenuOption == RESUME)
		{
			m_selectedMenuOption = EXIT;
		}
	}

	if(inputController.WasKeyJustPressed(XBOX_DPAD_UP))
	{
		if(m_selectedMenuOption == EXIT)
		{
			m_selectedMenuOption = RESUME;
		}
	}

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		switch(m_selectedMenuOption)
		{
		case(RESUME):
			ResetMenuState();
			g_gameClock->SetPaused(false);
			g_theGame->TransitionEncounterStates(2);//return to playing because this is the only place that matters.
			break;
		case(EXIT):
			g_isQuitting = true;
			break;
		}
	}

	if(theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE) || inputController.WasKeyJustPressed(XBOX_B))
	{
		g_gameClock->SetPaused(false);
		g_theGame->TransitionEncounterStates(2); //return to playing because this is the only place that matters.
	}

	theInput = nullptr;
	delete(theInput);
	return deltaSeconds; //new deltaSeconds
}
