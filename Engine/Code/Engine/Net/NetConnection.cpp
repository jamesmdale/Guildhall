#include "Engine\Net\NetConnection.hpp"

//  =============================================================================
NetConnection::NetConnection()
{
}

//  =============================================================================
NetConnection::~NetConnection()
{
}

//  =============================================================================
void NetConnection::QueueMessage(NetMessage* message)
{
	m_outgoingMessages.push_back(message);
}

//  =============================================================================
void NetConnection::FlushOutgoingMessages()
{
	for (int messageIndex = 0; messageIndex < (int)m_outgoingMessages.size(); ++messageIndex)
	{
		//compact message into packets and then send
	}
}
