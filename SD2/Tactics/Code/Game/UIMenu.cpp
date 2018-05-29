#include "UIMenu.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\Game.hpp"

Rgba defaultBackgroundImageTint = Rgba(.75f, .75f, .75f, 1.f);
Rgba backgroundImageTintIfOptionSelected = Rgba(.15f, .15f, .15f, 1.f);

const float dPadInputCooldown = .25f;
float timeSinceNavigationLastInput = 0.f;

void UIMenu::Initialize()
{
	//initialize things
}

void UIMenu::DisableSelection(bool isDisabled)
{
	m_isSelectionDisabled = isDisabled;
}

void UIMenu::AddMenuOption(std::string text, int index, bool isAbility)
{
	MenuOption option = MenuOption();
	option.text = text;
	option.index = index;
	option.isAbility = isAbility;
	m_menuOptions.push_back(option);
}

//used for when a menu item is chosen and completed
void UIMenu::DisableOption(int index)
{
	for(int optionIndex = 0; optionIndex < (int)m_menuOptions.size(); optionIndex++)
	{
		if(m_menuOptions[optionIndex].index == index)
		{
			m_menuOptions[optionIndex].isActive = false;
		}
	}

	TODO("for now just set the currentSelectedOption to -1 when disabling an option because it's used. Might want to revist this later if you decide that disabling is what you should be doing when a menu item is clicked.");
	m_currentSelectedOption = -1;
}

void UIMenu::DisableAbilityOptions()
{
	for(int optionIndex = 0; optionIndex < (int)m_menuOptions.size(); optionIndex++)
	{
		if(m_menuOptions[optionIndex].isAbility == true)
		{
			m_menuOptions[optionIndex].isActive = false;
		}
	}
}

void UIMenu::Render()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	Camera* uiCamera = g_theGame->m_uiCamera;
	
	AABB2 uiBounds = AABB2(theWindow->m_clientWidth * .05f, theWindow->m_clientHeight * .05f, theWindow->m_clientWidth * .30f, theWindow->m_clientHeight* .30f);
	theRenderer->SetCamera(g_theGame->m_uiCamera);

	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS, true);	
	theRenderer->SetTexture(*theRenderer->m_defaultTexture);

	//draw small menu box
	//texture from "https://colinquevitch.wordpress.com/2012/10/03/on-board/menu_background_new_002/"
	Texture* backgroundTexture = theRenderer->CreateOrGetTexture("Data/Images/uiMenuBackground.png");

	Rgba backgroundTint = defaultBackgroundImageTint;
	if(m_currentSelectedOption != -1)
	{
		backgroundTint = backgroundImageTintIfOptionSelected;
	}
	theRenderer->DrawTexturedAABB(uiBounds, *backgroundTexture, Vector2(0.f, 0.f), Vector2(1.f, 1.f), backgroundTint);

	//calculate for text in box
	Vector2 uiDimensions = uiBounds.GetDimensions();
	float offSetAmountY = uiDimensions.y / ((float)m_menuOptions.size() + 1.f);
	float offsetAmountX = uiBounds.maxs.x - uiBounds.mins.x;
	Vector2 uiBoxTopCenter = Vector2(uiBounds.GetCenter().x, uiBounds.GetCenter().y + (uiDimensions.y * .5f));	

	for(int optionIndex = 0; optionIndex < (int)m_menuOptions.size(); optionIndex++)
	{
		Rgba textColor = Rgba::GRAY;

		if(m_currentSelectedOption != -1)
		{
			if(m_currentSelectedOption == optionIndex)
			{
				textColor = Rgba::RED;
			}			
		}
		else
		{
			if(m_menuOptions[optionIndex].isActive)
			{
				textColor = Rgba::WHITE;
			}
			if(m_menuOptions[optionIndex].isActive && m_currentHoverIndex == m_menuOptions[optionIndex].index)
			{
				textColor = Rgba::YELLOW;
			}
		}
		
		Vector2 drawTextAtPosition = Vector2(uiBoxTopCenter.x, uiBoxTopCenter.y - (offSetAmountY * ((float)optionIndex + 1)));

		theRenderer->DrawText2DCentered(drawTextAtPosition, m_menuOptions[optionIndex].text, offSetAmountY, textColor, 1.f, theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont"));
	}

	//clean up pointers
	uiCamera = nullptr;
	theRenderer = nullptr;
	theWindow = nullptr;	
}

void UIMenu::UpdateFromInput(float deltaSeconds)
{

	InputSystem* theInput = InputSystem::GetInstance();
	XboxController inputController = theInput->GetController(0);

	bool areNavigationKeysReadyForInput = false;
	timeSinceNavigationLastInput += deltaSeconds;
	if(timeSinceNavigationLastInput > dPadInputCooldown)
	{
		areNavigationKeysReadyForInput = true;
	}

	if(m_currentSelectedOption == -1 && areNavigationKeysReadyForInput == true) //if we aren't in the default state we don't allow scrolling or selecting
	{
		TODO("CRITICAL: Figure out why was just pressed is messed up for dpad input only on xbox controller");
		if(inputController.WasKeyJustPressed(XBOX_DPAD_UP))
		{
			FindAndSetHoverToFirstEnabledOption(1);
			timeSinceNavigationLastInput = 0.f;
		}

		if(inputController.WasKeyJustPressed(XBOX_DPAD_DOWN))
		{
			FindAndSetHoverToFirstEnabledOption(-1);
			timeSinceNavigationLastInput = 0.f;
		}

		if(inputController.m_joyStickLeft.m_correctedPositionX  != 0.f && inputController.m_joyStickLeft.m_correctedPositionY != 0.f)
		{
			timeSinceNavigationLastInput = 0.f;

			float positionY = inputController.m_joyStickLeft.m_correctedPositionY;		
			float radius = inputController.m_joyStickLeft.m_magnitude;

			if(radius > .3f)
			{
				if(positionY > 0.f)
				{
					FindAndSetHoverToFirstEnabledOption(1);
					timeSinceNavigationLastInput = 0.f;
				}
				else
				{
					FindAndSetHoverToFirstEnabledOption(-1);
					timeSinceNavigationLastInput = 0.f;					
				}					
			}
		}		
	}
		
	if(inputController.WasKeyJustPressed(XBOX_A) && m_isSelectionDisabled == false)
	{
		m_currentSelectedOption = m_currentHoverIndex;		
	}

	if(inputController.WasKeyJustPressed(XBOX_B))
	{
		if(m_currentSelectedOption != -1)
		{
			m_currentSelectedOption = -1;
		}		
	}

	theInput = nullptr;
}

void UIMenu::FindAndSetHoverToFirstEnabledOption(int direction)
{
	if(direction == -1)
	{
		//starting at the current index, increment to find the next one.
		for(int optionIndex = m_currentHoverIndex + 1; optionIndex < (int)m_menuOptions.size(); optionIndex++)
		{
			if(m_menuOptions[optionIndex].isActive)
			{
				m_currentHoverIndex = optionIndex;
				return;
			}
		}
	}
	else
	{
		for(int optionIndex = m_currentHoverIndex - 1; optionIndex > -1 ; optionIndex--)
		{
			if(m_menuOptions[optionIndex].isActive)
			{
				m_currentHoverIndex = optionIndex;
				return;
			}
		}
	}	
}

std::string UIMenu::GetOptionNameIndex(int index)
{
	std::string returnText = "";
	if(index >= 0 && index < (int)m_menuOptions.size())
	{
		returnText = m_menuOptions[index].text;
	}

	return returnText;
}

int UIMenu::GetOptionIndexByName(std::string name)
{
	for(int optionIndex = 0; optionIndex < (int)m_menuOptions.size(); optionIndex++)
	{
		if(m_menuOptions[optionIndex].text == name)
		{
			return optionIndex;
		}
	}

	return -1; //if we don't find it
}
