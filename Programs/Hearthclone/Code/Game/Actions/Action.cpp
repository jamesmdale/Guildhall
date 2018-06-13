#include "Game\Actions\Action.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include <stdlib.h>

std::map<std::string, ActionFunction> s_registeredActions;

// genereal functios =============================================================================

void RegisterAllActions()
{
	RegisterAction("draw", Draw);
}

void RegisterAction(std::string name, ActionFunction action)
{
	s_registeredActions.emplace(name, action);
}

std::vector<std::string> GetRegisteredActionList()
{
	std::vector<std::string> registeredActionNames;

	for (std::map<std::string, ActionFunction>::iterator iterator = s_registeredActions.begin(); iterator != s_registeredActions.end(); ++iterator)
	{
		registeredActionNames.push_back(iterator->first);
	}

	return registeredActionNames;
}

ActionFunction GetActionDataFromRegisteredListByName(const std::string & actionName)
{
	std::map<std::string, ActionFunction>::iterator iterator = s_registeredActions.find(actionName);

	return iterator->second;
}

// actions =============================================================================

//template
/*	TEMPLATE
	void DoStuff(const std::map<std::string, std::string>& parameters)
	{
		// get parameters =============================================================================
		std::string thing1 = parameters.find("thing")->second;
		int thing2 = atoi(parameters.find("thing2")->second.c_str());

		// Do Stuff =============================================================================		
	}
*/

void Draw(const std::map<std::string, std::string>& parameters)
{
	// get parameters =============================================================================
	//std::map<std::string, std::string>::iterator player = parameters.find("player");

	std::string player = parameters.find("target")->second;
	int drawAmount = atoi(parameters.find("amount")->second.c_str());

	// process function =============================================================================
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Player* targetPlayer = nullptr;

	if (player == "enemy")
	{
		targetPlayer = gameState->m_enemyPlayer;				
	}
	else
	{
		targetPlayer = gameState->m_player;
	}

	for (int drawIndex = 0; drawIndex < drawAmount; ++drawIndex)
	{
		targetPlayer->m_hand.push_back(targetPlayer->m_deck[targetPlayer->m_deck.size() - 1]);

		targetPlayer->m_deck[targetPlayer->m_deck.size() - 1] = nullptr;
		targetPlayer->m_deck.pop_back();
	}

	targetPlayer = nullptr;
}