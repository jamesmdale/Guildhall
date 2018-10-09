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
void NetMessage::InitializeHeaderData()
{
	//write empty bytes that we will refill later with total size
	uint16_t wordValue = UNSIGNED_WORD_MAX;
	WriteBytes(sizeof(uint16_t), (void*)&wordValue, false);

	//write index of defintion
	WriteBytes(sizeof(uint8_t), (void*)&m_header->m_messageCallbackDefinitionIndex, false);
}

//  =============================================================================
bool NetMessage::WriteFinalSizeToHeader()
{
	//write empty bytes that we will refill later with total size
	ResetWrite();

	uint16_t messageSize = (uint16_t)GetBufferSize();

	if (messageSize > PACKET_MTU)
	{
		return false;
	}

	WriteBytes(sizeof(uint16_t), (void*)&messageSize, false);
	return true;
}

//  =============================================================================
NetMessage::~NetMessage()
{
	delete(m_header);
	m_header = nullptr;

	m_definition = nullptr;
}