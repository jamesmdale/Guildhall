#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetSession.hpp"


//  =============================================================================
NetMessage::NetMessage(const std::string& stringName)
{
	NetMessageDefinition* definition = GetRegisteredDefinitionByName(stringName);

	if (definition != nullptr)
	{
		m_header->m_messageCallbackDefinitionIndex = (uint8_t)definition->m_callbackId;
		m_definition = definition;
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