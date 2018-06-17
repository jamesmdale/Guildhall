#include "Game\Effects\Effect.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include <stdlib.h>
#include <queue>
#include "Engine\Time\Clock.hpp"

std::map<std::string, EffectCallback> s_registeredEffects;
std::queue<EffectData> EffectQueue;
Stopwatch* currentEffectStopwatch = nullptr;

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

EffectCallback GetEffectCallbackFromRegisteredListByName(const std::string & effectName)
{
	std::map<std::string, EffectCallback>::iterator iterator = s_registeredEffects.find(effectName);

	return iterator->second;
}

// RefereeQueue methods =========================================================================================

void ProcessEffectQueue()
{
	//process everything on the RefereeQueue before allowing new user actions
	if(GetEffectQueueCount() != 0)
	{
		EffectData effect = EffectQueue.front();
		bool success = effect.callback(effect.targetWidget, effect.parameters, GetMasterDeltaSeconds());
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

void AddEffectToEffectQueue(const std::string & callbackName, Widget * targetWidget, const std::map<std::string, std::string> parameters)
{
	EffectData data = EffectData(callbackName, targetWidget, parameters);
	EffectQueue.push(data);
}

void AddEffectToEffectQueue(EffectCallback callback, Widget* targetWidget, const std::map<std::string, std::string>& parameters)
{
	EffectData data;
	
	data.callback = callback;
	data.targetWidget = targetWidget;
	data.parameters = parameters;

	EffectQueue.push(data);
}

Stopwatch* SetNewStopwatch(float timer)
{
	currentEffectStopwatch = new Stopwatch(GetMasterClock());
	currentEffectStopwatch->SetTimer(timer);

	return currentEffectStopwatch;
}

void ClearStopwatch()
{
	currentEffectStopwatch = nullptr;
}

Stopwatch* GetStopwatch()
{
	return currentEffectStopwatch;
}
