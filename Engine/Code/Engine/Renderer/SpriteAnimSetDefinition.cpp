#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "engine/Renderer/SpriteAnimDefinition.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition( const tinyxml2::XMLElement& animSetElement, Renderer& renderer )
{
	std::string spriteSheetName = "invalid";
	IntVector2 spriteSheetlayout = IntVector2(0,0);
	float fps = 1.0f;

	spriteSheetName = ParseXmlAttribute(animSetElement, "spriteSheet", spriteSheetName);
	spriteSheetlayout = ParseXmlAttribute(animSetElement, "spriteLayout", spriteSheetlayout);
	fps = ParseXmlAttribute(animSetElement, "fps", fps);

	GUARANTEE_OR_DIE(spriteSheetName != "invalid", "Sprite sheet for spriteanimsetdefinition was invalid");
	
	SpriteSheet* definitionSheet = new SpriteSheet(*renderer.CreateOrGetTexture(std::string("Data/Images/" + spriteSheetName)), spriteSheetlayout.x, spriteSheetlayout.y);

	for(const tinyxml2::XMLElement* definitionNode = animSetElement.FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{	
		SpriteAnimDefinition* newDefinition = new SpriteAnimDefinition(*definitionNode, *definitionSheet, fps);

		m_namedAnimDefs.insert(std::pair<std::string, SpriteAnimDefinition*>(std::string(newDefinition->m_name), newDefinition));
	}
}
