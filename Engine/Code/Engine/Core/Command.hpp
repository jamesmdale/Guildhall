#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"

// A command is a single submitted commmand
// NOT the definition (which I hide internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" );

class Command;
typedef void (*command_cb)( Command &cmd ); 

struct CommandRegistration
{
public:
	CommandRegistration(command_cb cb, std::string helpText = "", std::string successMessage = "")
	{
		//add member variables that you want to save
		m_callBack = cb;
		m_helpText = helpText;
		m_successMessage = successMessage;
	}

public:
	std::string m_helpText = "";
	std::string m_successMessage = "";
	command_cb m_callBack = nullptr;
};


class Command
{

private:
	int m_tokenIndex = 0;

public:
	std::string m_commandString = "";
	std::vector<std::string> m_commandTokens;
	CommandRegistration* m_commandInfo = nullptr;

public:
	Command( char const *str );
	const char* GetName(); // would return "echo_with_color"

						   // Gets the next string in the argument list.
						   // Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();   // would return after each call...	
								   // first:  "(255,255,0)""
								   // second: "Hello \"world\""
								   // third+: ""
	std::string GetRemainingContentAsString();

	//add more helpers as necessary
	int GetNextInt();
	Rgba GetNextColor();
	float GetNextFloat();
	bool GetNextBool();

	std::string PeekNextString();

	void ResetTokenIndex() {m_tokenIndex = 0;}
	void IncrementTokenIndex() { m_tokenIndex++;}

	bool IsCorrectNumberOfParameters(int expectedParamCount);
	std::string ParseCommandStringForValidFormatting();								   
};

void RegisterCommand( char const *name, CommandRegistration definition); 
std::vector<std::string> GetRegisteredCommandList();
bool CommandRun( char const *command ); //if command was found registered return true