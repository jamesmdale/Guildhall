#include "Engine\Net\NetMessageChannel.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetMessage.hpp"

//  =============================================================================
NetMessageChannel::NetMessageChannel()
{
}

//  =============================================================================
NetMessageChannel::~NetMessageChannel()
{
	m_outOfOrderMessages.clear();
}

//  =============================================================================
uint16_t NetMessageChannel::GetAndIncrementSequenceId()
{
	uint16_t sequenceId = m_nextSequenceId;
	++m_nextSequenceId;
	
	return sequenceId;
}

// ==============================================================================
NetMessage* NetMessageChannel::GetNextMessageToProcess()
{
	//find the next out of order message to process
	for (int messageIndex = 0; messageIndex < (int)m_outOfOrderMessages.size(); ++messageIndex)
	{
		if (m_outOfOrderMessages[messageIndex]->m_header->m_sequenceId == m_nextExpectedSequenceId)
		{
			++m_nextExpectedSequenceId;

			NetMessage* messageToProcess = m_outOfOrderMessages[messageIndex];
			m_outOfOrderMessages[messageIndex] = nullptr;
			m_outOfOrderMessages.erase(m_outOfOrderMessages.begin() + messageIndex);

			return messageToProcess;
		}
	}

	//we didn't find one that matched our next expected sequence id, so return nullptr
	return nullptr;
}

//  =============================================================================
bool NetMessageChannel::ShouldProcessMessage(NetMessage* message)
{
	//if the message is not equal to the next expected sequence id, stick it in the list and don't process
	if (message->m_header->m_sequenceId != m_nextExpectedSequenceId)
	{
		m_outOfOrderMessages.push_back(message);
		return false;
	}	
	
	//this message sequence id matches our expected id.
	++m_nextExpectedSequenceId;
	return true;
}
