#include "Game/AdventureMapPortalGenerationStep.hpp"
#include "Game/PortalDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

AdventureMapPortalGenerationStep::AdventureMapPortalGenerationStep( const tinyxml2::XMLElement& element)
{	
	std::string portalDefinitionName = ParseXmlAttribute(element, "type", std::string("default"));	
	std::map<std::string, PortalDefinition*>::iterator portalDefIterator = PortalDefinition::s_portalDefinitions.find(portalDefinitionName);
	GUARANTEE_OR_DIE(portalDefIterator != PortalDefinition::s_portalDefinitions.end(), "Missing portal definition declared in AdventureDefinitions.xml for portal");
	if(portalDefIterator != PortalDefinition::s_portalDefinitions.end())
	{
		m_portalDefinition = portalDefIterator->second;
	}
	
	m_onTileType = ParseXmlAttribute(element, "onTileType", m_onTileType);
	m_toMap = ParseXmlAttribute(element, "toMap", m_toMap);
	m_toTileType = ParseXmlAttribute(element, "toTileType", m_toTileType);

	std::string reciprocalTypeName = ParseXmlAttribute(element, "reciprocalType", std::string("default"));
	portalDefIterator = PortalDefinition::s_portalDefinitions.find(reciprocalTypeName);
	GUARANTEE_OR_DIE(portalDefIterator != PortalDefinition::s_portalDefinitions.end(), "Missing portal reciprocal definition declared in AdventureDefinitions.xml for portal" );
	if(portalDefIterator != PortalDefinition::s_portalDefinitions.end())
	{
		m_reciprocalType = portalDefIterator->second;
	}
}