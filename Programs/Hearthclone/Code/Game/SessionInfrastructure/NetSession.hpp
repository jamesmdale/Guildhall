#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\COre\DevConsole.hpp"
#include <string>

#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

typedef void (*NetMessageProcessorCallback)(const NetMessage& message, const NetConnection& from);

/*
Gives us storage for named messages and their corresponding callbacks.
When we recieve a message we will loop through the registered messages after getting the initial string name.
*/

struct NetSendInfo
{
public:
	NetSendInfo(){};

public:
	NetMessage message;
	NetConnection toAddress;
};

class NetMessage
{
public:
	NetMessage()
	{

	}

	NetMessage(const std::string& messageContent)
	{
		m_message = messageContent;
		m_messageSize = messageContent.size();
	}

	NetMessage(const char* messageContent, const size_t& messageSize)
	{
		m_message = messageContent;
		m_messageSize = messageSize;
	}

	std::string GetNextString();
	int GetNextInt();
	float GetNextFloat();
	bool GetNextBool();

	std::string PeekNextString();

	void ResetTokenIndex() { m_tokenIndex = 0; }
	void IncrementTokenIndex() { m_tokenIndex++; }

	//put string into easily accessible tokens
	bool ParseStringIntoTokens()
	{
		bool isParseSuccess = true;

		std::string currentStringToken;
		bool isCurrentStringQuoted = false;
		bool isCurrentStringParenthesis = false;

		for (int characterIndex = 0; characterIndex < (int)m_message.size(); characterIndex++)
		{
			char currentChar = m_message[characterIndex];

			if (currentChar == ' ' && !isCurrentStringQuoted && !isCurrentStringParenthesis)
			{
				if (!currentStringToken.empty())
				{
					m_messageTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
			}
			else if (currentChar == '\"')
			{
				if (isCurrentStringParenthesis)
				{
					DevConsolePrintf("INVALID: Cannot have string in parenthesis");
					return !isParseSuccess;
				}
				if (isCurrentStringQuoted)
				{
					if (currentStringToken.empty())
					{
						DevConsolePrintf("INVALID: Cannot have empty string in quotes");
						return !isParseSuccess;
					}
					isCurrentStringQuoted = false;
					m_messageTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
				else
				{
					if (!currentStringToken.empty())
					{
						m_messageTokens.push_back(currentStringToken);
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
					DevConsolePrintf("INVALID: Cannot have nested '('");
					return !isParseSuccess;
				}
				else
				{
					if (!currentStringToken.empty())
					{
						m_messageTokens.push_back(currentStringToken);
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
						DevConsolePrintf("INVALID: Cannot have empty rbga...(empty rgba)");
						return !isParseSuccess;
					}
					else
					{
						m_messageTokens.push_back(currentStringToken);
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
			m_messageTokens.push_back(currentStringToken);
		}

		return isParseSuccess;
	}

private:
	int m_tokenIndex = 0;
public:
	std::string m_message = "";
	size_t m_messageSize = 0;

	std::vector<std::string> m_messageTokens;
};

class NetMessageProcessorRegistration
{
public:
	NetMessageProcessorRegistration(const char* messageId, NetMessageProcessorCallback callBack)
	{
		m_messageId = messageId;
		m_callBack = callBack;
	}

public:
	std::string m_messageId = "";
	NetMessageProcessorCallback m_callBack = nullptr;
};

//houses everything we need to know about a connecion made to the session.
struct NetConnection
{
public:
	int m_connectionIndex = 0;
	BytePacker* m_bytePacker = nullptr;
	NetAddress* m_address = nullptr;
};

//session that actually manages communications between clients we are connected to.
class NetSession
{
public:
	NetSession();
	~NetSession();


	//creates a new registrationMessage and stores it in our list
	void RegisterMessage(const char* messageId, NetMessageProcessorCallback callback);

	//Attempts to make 'port' one that we can communicate on
	bool AddBinding(const int port);

	//always works becasue UDP
	void AddConnection(NetAddress& connectionAddress);

	//processes all messages on all ports we have bound
	void ProcessIncomingMessages();

	//creates a queue
	void ProcessOutgoingMessages();

	void QueueOutGoingMessage(const NetSendInfo& outMessage){m_outMessageQueue.enqueue(outMessage);}

public:
	std::vector<UDPSocket*> m_openPorts;
	std::vector<NetMessageProcessorRegistration*> m_registeredMessages;
	std::vector<NetConnection*> m_connections;

	ThreadSafeQueue<NetSendInfo> m_outMessageQueue;
};

