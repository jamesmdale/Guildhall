#pragma once
#include <queue>
#include "Engine\Core\EngineCommon.hpp"
#include <map>
#include <string>
#include <vector>


typedef bool (*EffectCallback)(const std::map<std::string, std::string>& parameters, float deltaSeconds);

/*	EXAMPLE : Effect layout

bool Func(const std::map<std::string, std::string>& pararmeters);
*/

// convenience functions =============================================================================
void RegisterAllEffects();
void RegisterEffect(std::string name, EffectCallback action);
std::vector<std::string> GetRegisteredEffectList();
EffectCallback GetEffectCallbackFromRegisteredListByName(const std::string& actionName);

struct EffectData
{
	EffectData() {};
	EffectData(const std::string& functionName, const std::map<std::string, std::string> functionParameters)
	{
		name = functionName;
		parameters = functionParameters;
		callback = GetEffectCallbackFromRegisteredListByName(functionName);
	}

	std::string name;
	std::map<std::string, std::string> parameters;
	EffectCallback callback;
};

// referee functions =========================================================================================
void ProcessEffectQueue();
int GetEffectQueueCount();
void AddEffectToEffectQueue(const EffectData& action);
void AddEffectToEffectQueue(EffectCallback function, const std::map<std::string, std::string>&);

// action list =============================================================================

//template
/*	TEMPLATE
void DoStuff(const std::map<std::string, std::string>& parameters)
{
// get parameters =============================================================================
std::string thing1 = parameters.find("thing")->second;
int thing2 = atoi(parameters.find("thing2")->second.c_str());

// Do Stuff =============================================================================	

return complete
}
*/

bool DrawEffect(const std::map<std::string, std::string>& parameters, float deltaSeconds);

bool AttackEffect(const std::map<std::string, std::string>& parameters, float deltaSeconds);







