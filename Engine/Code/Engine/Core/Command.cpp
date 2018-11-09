#include "Engine\Core\Command.hpp"
#include <utility>
#include "Game\TheApp.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\Rgba.hpp"

std::map<std::string, CommandRegistration> s_registeredDefinitions;

//  =========================================================================================
Command::Command( char const *str )
{
	m_commandString = str;
}

//  =========================================================================================
bool CommandRun( char const *command )
{
	bool isValidCommand = false;

	Command cmd = Command(command);

	std::string errorMessage = cmd.ParseCommandStringForValidFormatting();

	if(errorMessage != "")
	{
		DevConsolePrintf(Rgba::RED, errorMessage.c_str());
		return isValidCommand;
	}

	std::string commandName = cmd.GetName();
	ToLower(commandName);

	std::map<std::string, CommandRegistration>::iterator cmdIterator = s_registeredDefinitions.begin();
	for(cmdIterator = s_registeredDefinitions.begin(); cmdIterator != s_registeredDefinitions.end(); ++cmdIterator)
	{
		if(commandName == cmdIterator->first)
		{
			break;
		}
	}
	
	if (cmdIterator != s_registeredDefinitions.end())
	{
		isValidCommand = true;
		cmd.m_commandInfo = &cmdIterator->second;
		((command_cb)cmd.m_commandInfo->m_callBack)(cmd);
	}
	else
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Unknown command");
	}

	return isValidCommand;
}

//  =========================================================================================
//currently supported characters for special parsing is \" and parenthesis (,).
//These cases must be handled specifically for parsing.  Will parse actual input again in the function itself.
std::string Command::ParseCommandStringForValidFormatting()
{
	std::string currentStringToken;
	bool isCurrentStringQuoted = false;
	bool isCurrentStringParenthesis = false;

	if ((int)m_commandString.size() > 100)
	{
		return std::string("INVALID: Command input too long");
	}

	for (int characterIndex = 0; characterIndex < (int)m_commandString.size(); characterIndex++)
	{
		char currentChar = m_commandString[characterIndex];

		if (currentChar == ' ' && !isCurrentStringQuoted && !isCurrentStringParenthesis)
		{
			if (!currentStringToken.empty())
			{
				m_commandTokens.push_back(currentStringToken);
				currentStringToken.clear();
			}
		}
		else if (currentChar == '\"')
		{
			if (isCurrentStringParenthesis)
			{
				return std::string("INVALID: Cannot have string in parenthesis");
			}
			if (isCurrentStringQuoted)
			{
				if (currentStringToken.empty())
				{
					return std::string("INVALID: Cannot have empty string in quotes");
				}
				isCurrentStringQuoted = false;
				m_commandTokens.push_back(currentStringToken);
				currentStringToken.clear();
			}
			else
			{
				if (!currentStringToken.empty())
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
				isCurrentStringQuoted = true;
			}
		}
		else if (currentChar == '(')
		{
			if (isCurrentStringQuoted)
			{
				currentStringToken.push_back(currentChar);
			}
			else if (isCurrentStringParenthesis)
			{
				return std::string("INVALID: Cannot have nested '('");
			}
			else
			{
				if (!currentStringToken.empty())
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
				isCurrentStringParenthesis = true;
			}
		}
		else if (currentChar == ')')
		{
			if (isCurrentStringQuoted)
			{
				currentStringToken.push_back(currentChar);
			}
			else if (isCurrentStringParenthesis)
			{
				if (currentStringToken.empty())
				{
					return std::string("INVALID: Cannot have empty rbga...(empty rgba)");
				}
				else
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
					isCurrentStringParenthesis = false;
				}
			}
		}
		else
		{
			currentStringToken.push_back(currentChar);
		}
	}
	if (!currentStringToken.empty())
	{
		m_commandTokens.push_back(currentStringToken);
	}
	

	return std::string(""); //no errors so we return an empty string
}

//  =========================================================================================
const char* Command::GetName()
{
	return m_commandTokens[0].c_str(); //the fist should always be the name of the command.
}

//  =========================================================================================
std::string Command::GetNextString()
{
	m_tokenIndex++;

	if(m_tokenIndex >= (int)m_commandTokens.size())
	{
		return "";
	}
	else
	{
		return m_commandTokens[m_tokenIndex];
	}
}

//  =========================================================================================
std::string Command::GetRemainingContentAsString()
{
	std::string outString = "";

	while (m_tokenIndex + 1 < (int)m_commandTokens.size())
	{
		m_tokenIndex++;
		outString.append(Stringf(" %s", m_commandTokens[m_tokenIndex].c_str()));
	}

	return outString;
}

//  =========================================================================================
int Command::GetNextInt()
{
	return ConvertStringToInt(GetNextString());
}

//  =========================================================================================
Rgba Command::GetNextColor()
{
	return ConvertStringToRGBA(GetNextString());
}

//  =========================================================================================
float Command::GetNextFloat()
{
	return ConvertStringToFloat(GetNextString());
}

//  =========================================================================================
bool Command::GetNextBool()
{
	return ConvertStringToBool(GetNextString());
}

//  =========================================================================================
std::string Command::PeekNextString()
{
	if (m_tokenIndex >= (int)m_commandTokens.size())
	{
		return "";
	}
	else
	{
		return m_commandTokens[m_tokenIndex + 1];
	}
}

//  =========================================================================================
void RegisterCommand(char const* name, CommandRegistration definition)
{
	std::string cmdName(name);
	ToLower(cmdName);
	s_registeredDefinitions.insert(std::make_pair(cmdName, definition));
}

//  =========================================================================================
std::vector<std::string> GetRegisteredCommandList()
{
	std::vector<std::string> commandsAsText;
	for( std::map<std::string, CommandRegistration>::iterator command = s_registeredDefinitions.begin(); command != s_registeredDefinitions.end() ; ++command )
	{
		commandsAsText.push_back((Stringf("%s %s", command->first.c_str(), command->second.m_helpText.c_str())));
	}

	return commandsAsText;
}

//  =========================================================================================
bool Command::IsCorrectNumberOfParameters(int expectedParamCount)
{
	//subtract one to account for the name
	if((int)m_commandTokens.size() - 1 != expectedParamCount)
	{
		return false;
	}

	return true;
}
