#include "Game\Effects\Effect.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include <stdlib.h>
#include <queue>

std::map<std::string, EffectCallback> s_registeredEffects;
std::queue<EffectData> EffectQueue;

// genereal functions =============================================================================

void RegisterAllEffects()
{
	RegisterEffect("draw_effect", DrawEffect);
	RegisterEffect("attack_effect", AttackEffect);
}

void RegisterEffect(std::string name, EffectCallback effect)
{
	s_registeredEffects.emplace(name, effect);
}

std::vector<std::string> GetRegisteredEffectList()
{
	std::vector<std::string> registeredEffectNames;

	for (std::map<std::string, EffectCallback>::iterator iterator = s_registeredEffects.begin(); iterator != s_registeredEffects.end(); ++iterator)
	{
		registeredEffectNames.push_back(iterator->first);
	}

	return registeredEffectNames;
}

EffectCallback GetEffectCallbackFromRegisteredListByName(const std::string & actionName)
{
	std::map<std::string, EffectCallback>::iterator iterator = s_registeredEffects.find(actionName);

	return iterator->second;
}

// referee methods =========================================================================================

void ProcessEffectQueue()
{
	//process everything on the referee before allowing new user actions
	if(GetEffectQueueCount() != 0)
	{
		EffectData effect = EffectQueue.front();
		bool success = effect.callback(effect.parameters, GetMasterDeltaSeconds());
		if (success == true)
		{
			EffectQueue.pop(); //fifo
		}
	}
}

int GetEffectQueueCount()
{
	return (int)EffectQueue.size();
}

void AddEffectToEffectQueue(const EffectData& action)
{
	EffectQueue.push(action);
}

void AddEffectToEffectQueue(EffectCallback callback, const std::map<std::string, std::string>& parameters)
{
	EffectData data;
	data.callback = callback;
	data.parameters = parameters;

	EffectQueue.push(data);
}

// effects =============================================================================

//template
/*	TEMPLATE
void DoStuff(const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
// Get Parameters =============================================================================
std::string thing1 = parameters.find("thing")->second;
int thing2 = atoi(parameters.find("thing2")->second.c_str());

// Process Function =============================================================================		
}
*/

bool DrawEffect(const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
	static bool isComplete = false;
	
	//do something over time

	isComplete = true;

	return isComplete;
}

bool AttackEffect(const std::map<std::string, std::string>& parameters, float deltaSeconds)
{
	// Get Parameters =============================================================================
	std::string attackTarget = parameters.find("target")->second;

	// Process Function =============================================================================		

	return true;
}