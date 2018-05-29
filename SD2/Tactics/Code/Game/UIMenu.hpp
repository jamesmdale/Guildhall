#pragma once
#include <vector>
#include <string>
#include "Engine\Core\EngineCommon.hpp"


TODO("Might switch to enums later");
//enum ActionOptionType
//{
//	MOVE_ACTION_OPTION,
//	ATTACK_ACTION_OPTION,
//	WAIT_ACTION_OPTION,
//	NUM_ACTION_OPTIONS
//};

struct MenuOption
{
	std::string text;
	int index;
	bool isActive = true;
	bool isAbility = false;
};

class UIMenu
{
public:
public:
	UIMenu(){};

	void AddMenuOption(std::string text, int value, bool isAbility);
	void DisableOption(int index);
	void DisableAbilityOptions();
	void Render();	
	void Initialize();
	void DisableSelection(bool isDisabled);

	void UpdateFromInput(float deltaSeconds);
	void FindAndSetHoverToFirstEnabledOption(int direction);

	std::string GetOptionNameIndex(int index);
	int GetOptionIndexByName(std::string name);

public:
	int m_currentHoverIndex;
	int m_currentSelectedOption = -1;
	std::vector<MenuOption> m_menuOptions;
	bool m_isSelectionDisabled = false;
};