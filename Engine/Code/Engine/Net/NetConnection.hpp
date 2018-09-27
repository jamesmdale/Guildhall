#pragma once
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetAddress.hpp"

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void QueueMessage(NetMessage* message);
	void FlushOutgoingMessages();

public:
	uint8_t m_index = UCHAR_MAX; //max of 255
	NetAddress m_address;

	std::vector<NetMessage*> m_outgoingMessages;
};