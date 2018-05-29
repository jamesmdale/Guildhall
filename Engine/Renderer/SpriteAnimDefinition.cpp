#include "SpriteAnimDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

SpriteAnimDefinition::SpriteAnimDefinition( const tinyxml2::XMLElement& animElement,
	const SpriteSheet& defaultSpriteSheet, 
	float defaultFps)
{
	m_spriteSheet = (SpriteSheet*) &defaultSpriteSheet;
	m_framesPerSecond = defaultFps;	

	m_name = ParseXmlAttribute(animElement, "name", m_name);
	m_spriteIndexes = ParseXmlAttribute(animElement, "spriteIndexes", m_spriteIndexes);
	m_framesPerSecond = ParseXmlAttribute(animElement, "fps", m_framesPerSecond);
	m_isLooping = ParseXmlAttribute(animElement, "isLooping", m_isLooping);
}

