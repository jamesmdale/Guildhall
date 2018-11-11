#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\EngineCommon.hpp"


//  =============================================================================
NetMessage::NetMessage(const std::string& stringName)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionByName(stringName);

	if (definition != nullptr)
	{
		m_header = new NetMessageHeader();
		m_header->m_messageCallbackDefinitionIndex = (uint8_t)definition->m_callbackId;

		m_definition = definition;
	}

	//cleanup
	definition = nullptr;
}

//  =============================================================================
bool NetMessage::WriteFinalSizeToHeader()
{
	//write empty bytes that we will refill later with total size
	ResetWrite();

	uint16_t messageSize = (uint16_t)GetWrittenByteCount();

	if (messageSize > PACKET_MTU)
	{
		return false;
	}

	WriteBytes(sizeof(uint16_t), (void*)&messageSize, false);

	SetWriteHeadToMaxWritten();
	return true;
}

//  =============================================================================
bool NetMessage::IsReadyToResend(const uint64_t& resendThreshold)
{
	uint64_t timeSinceLastResend = GetMasterClock()->GetLastHPC() - m_sendTime;

	return timeSinceLastResend > resendThreshold ? true : false;
}

//  =============================================================================
uint16_t NetMessage::GetReliableId()
{
	return m_header->m_reliableId;
}

//  =============================================================================
NetMessage::~NetMessage()
{
	delete(m_header);
	m_header = nullptr;

	m_definition = nullptr;
}