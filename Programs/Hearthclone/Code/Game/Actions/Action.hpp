#pragma once
#include <map>
#include <string>
#include <vector>

typedef void (*ActionFunction)(const std::map<std::string, std::string>& parameters);

/*	EXAMPLE : action function layout
	
	void Func(const std::map<std::string, std::string>& pararmeters);
*/


struct ActionData
{
	ActionData(const std::string& functionName, const std::map<std::string, std::string> functionParameters)
	{
		name = functionName;
		parameters = functionParameters;
	}

	std::string name;
	std::map<std::string, std::string> parameters;
	ActionFunction functionPointer;
};

// convenience funcitons =============================================================================
void RegisterAllActions();
void RegisterAction(std::string name, ActionFunction action);
std::vector<std::string> GetRegisteredActionList();
ActionFunction GetActionDataFromRegisteredListByName(const std::string& actionName);

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

void Draw(const std::map<std::string, std::string>& parameters);

void Attack(const std::map<std::string, std::string>& parameters);







