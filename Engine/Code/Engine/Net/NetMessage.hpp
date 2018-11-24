#pragma once
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Core\BitHelper.hpp"

//forward dec netmessage for callback
class NetMessage;
class NetConnection;

enum eNetMessageFlag
{
	CONNECTIONLESS_NET_MESSAGE_FLAG = BIT_FLAG(0),
	RELIABLE_NET_MESSAGE_FLAG = BIT_FLAG(1),
	INORDER_NET_MESSAGE_FLAG = BIT_FLAG(2),
	HEARTBEAT_NET_MESSAGE_FLAG = BIT_FLAG(3),

	RELIABLE_INORDER_NET_MESSAGE_FLAG = RELIABLE_NET_MESSAGE_FLAG | INORDER_NET_MESSAGE_FLAG,

	NUM_NET_MESSAGE_FLAGS
};

typedef bool (*NetMessageCallback)(NetMessage& message, NetConnection* fromConnection);

struct NetMessageHeader
{
	uint8_t m_messageCallbackDefinitionIndex = UINT8_MAX;

	//won't be written unless message is reliale
	uint16_t m_reliableId = 0;

	//won't be written unless message is in-order
	uint16_t m_sequenceId = 0;
};

class NetMessageDefinition
{
public:
	int m_callbackId = -1;
	std::string m_callbackName;
	NetMessageCallback m_callback = nullptr;
	eNetMessageFlag m_messageFlag;
	uint8_t m_messageChannelIndex = UINT8_MAX;

public:
	bool DoesRequireConnection()
	{
		 return !AreBitsSet<uint>((uint)m_messageFlag, (uint)CONNECTIONLESS_NET_MESSAGE_FLAG) ? true : false;
	}

	bool IsReliable()
	{
		return AreBitsSet<uint>((uint)m_messageFlag, (uint)RELIABLE_NET_MESSAGE_FLAG) ? true : false;
	}

	bool IsInOrder()
	{
		return AreBitsSet<uint>((uint)m_messageFlag, (uint)RELIABLE_INORDER_NET_MESSAGE_FLAG) ? true : false;
	}
};

class NetMessage : public BytePacker
{
public:
	NetMessage(const std::string& messageName);
	~NetMessage();

	bool WriteFinalSizeToHeader();
	bool IsReadyToResend(const uint64_t& resendThreshold);
	uint16_t GetReliableId();

	//helper for accessing definition
public:
	NetMessageHeader* m_header = nullptr;
	NetMessageDefinition* m_definition = nullptr;
	uint64_t m_sendTime = 0;
};