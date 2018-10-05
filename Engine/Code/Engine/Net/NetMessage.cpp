#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"


//  =============================================================================
NetMessage::NetMessage(const std::string& stringName)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionByName(stringName);

	if (definition != nullptr)
	{
		m_header = new NetMessageHeader();
		m_header->m_messageCallbackDefinitionIndex = (uint8_t)definition->m_callbackId;
		m_definition = definition;

		//write empty bytes that we will refill later with total size
		WriteBytes(sizeof(uint16_t), nullptr, false);

		//write index of defintion
		WriteBytes(sizeof(uint8_t), (void*)m_header->m_messageCallbackDefinitionIndex, false);
	}

	//cleanup
	definition = nullptr;
}

//  =============================================================================
NetMessage::~NetMessage()
{
	delete(m_header);
	m_header = nullptr;

	m_definition = nullptr;
}