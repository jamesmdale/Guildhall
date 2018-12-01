#pragma once
#include <vector>

class NetMessage;
class NetConnection;

class NetMessageChannel
{
public:
	NetMessageChannel();
	~NetMessageChannel();

	uint16_t GetAndIncrementSequenceId();
	NetMessage* GetNextMessageToProcess();
	bool ShouldProcessMessage(NetMessage* message);

public:
	uint16_t m_nextSequenceId = 0;			//used for sending
	uint16_t m_nextExpectedSequenceId = 0;			//used for receiving
	std::vector<NetMessage*> m_outOfOrderMessages;	//used for receiving
};

