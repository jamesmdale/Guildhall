#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\BitHelper.hpp"

//forward dec netmessage for callback
class NetMessage;
class NetConnection;
enum eNetMessageFlag;

typedef bool (*NetMessageCallback)(NetMessage& message, NetConnection* fromConnection);

struct NetMessageHeader
{
	uint8_t m_messageCallbackDefinitionIndex = UCHAR_MAX;
};

struct NetMessageDefinition
{
	int m_callbackId = -1;
	std::string m_callbackName;
	NetMessageCallback m_callback = nullptr;
	eNetMessageFlag m_messageFlag;

	bool DoesRequireConnection()
	{
		if (!AreBitsSet(m_messageFlag, CONNECTIONLESS_NET_MESSAGE_FLAG))
		{
			return true;
		}

		return false;
	}
};

class NetMessage : public BytePacker
{
public:
	NetMessage(const std::string& messageName);
	~NetMessage();

	void InitializeHeaderData();
	bool WriteFinalSizeToHeader();

public:
	NetMessageHeader* m_header = nullptr;
	NetMessageDefinition* m_definition = nullptr;
};