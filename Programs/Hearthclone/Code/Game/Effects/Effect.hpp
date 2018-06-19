#pragma once
#include <queue>
#include "Engine\Core\EngineCommon.hpp"
#include <map>
#include <string>
#include <vector>
#include "Engine\Core\Widget.hpp"
#include "Engine\Time\Stopwatch.hpp"


typedef bool (*EffectCallback)(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float deltaSeconds);

/*	EXAMPLE : Effect layout

bool Func(const std::map<std::string, std::string>& pararmeters);
*/

// convenience functions =============================================================================
void RegisterAllEffects();
void RegisterEffect(std::string name, EffectCallback effect);
std::vector<std::string> GetRegisteredEffectList();
EffectCallback GetEffectCallbackFromRegisteredListByName(const std::string& effectName);

struct EffectData
{
	EffectData() {};
	EffectData(const EffectCallback callbackFunction, Widget* widget, const std::map<std::string, std::string> functionParameters)
	{
		callback = callbackFunction;
		parameters = functionParameters;
		targetWidget = widget;		
	}

	EffectData( const std::string& functionName, Widget* widget, const std::map<std::string, std::string> functionParameters)
	{		
		targetWidget = widget;
		parameters = functionParameters;
		callback = GetEffectCallbackFromRegisteredListByName(functionName);
	}

	Widget* targetWidget;
	std::map<std::string, std::string> parameters;
	EffectCallback callback;
};

// EffectQueue functions =========================================================================================
void ProcessEffectQueue();
int GetEffectQueueCount();
void AddEffectToEffectQueue(const EffectData& effect);
void AddEffectToEffectQueue(const std::string& callbackName, Widget* targetWidget, const std::map<std::string, std::string> parameters);
void AddEffectToEffectQueue(EffectCallback function, Widget* targetWidget, const std::map<std::string, std::string>& params);

Stopwatch* SetNewStopwatch(float timer);
void ClearStopwatch();
Stopwatch* GetStopwatch();

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

bool DrawEffect(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float deltaSeconds);

bool AttackEffect(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float deltaSeconds);

bool TargetFromInputEffect(Widget* targetWidget, const std::map<std::string, std::string>& parameters, float DeltaSeconds);







