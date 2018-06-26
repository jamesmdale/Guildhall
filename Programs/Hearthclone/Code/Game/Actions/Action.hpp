#pragma once
#include <map>
#include <string>
#include <vector>

typedef void (*ActionCallback)(const std::map<std::string, std::string>& parameters);

/*	EXAMPLE : action function layout
	
	void Func(const std::map<std::string, std::string>& pararmeters);
*/

// convenience functions =============================================================================
void RegisterAllActions();
void RegisterAction(std::string name, ActionCallback action);
std::vector<std::string> GetRegisteredActionList();
ActionCallback GetActionDataFromRegisteredListByName(const std::string& actionName);

struct ActionData
{
	ActionData() {};
	ActionData(const ActionCallback callbackFunction, const std::map<std::string, std::string> functionParameters)
	{
		parameters = functionParameters;
		callback = callbackFunction;
	}

	ActionData(const std::string& callbackFunction, const std::map<std::string, std::string> functionParameters)
	{
		parameters = functionParameters;
		callback = GetActionDataFromRegisteredListByName(callbackFunction);
	}

	std::map<std::string, std::string> parameters;
	ActionCallback callback;
};

// RefereeQueue functions =========================================================================================
void ProcessRefereeQueue();
int GetRefereeQueueCount();
void AddActionToRefereeQueue(ActionData action);
void AddActionToRefereeQueue(const std::string& callbackName, const std::map<std::string, std::string> parameters);
void AddActionToRefereeQueue(ActionCallback callback, std::map<std::string, std::string> parameters);

// action list =============================================================================

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

void DrawAction(const std::map<std::string, std::string>& parameters);

void AttackAction(const std::map<std::string, std::string>& parameters);

void CastFromHandAction(const std::map<std::string, std::string>& parameters);

void EndTurnAction(const std::map<std::string, std::string>& parameters);

void StartTurnAction(const std::map<std::string, std::string>& parameters);







