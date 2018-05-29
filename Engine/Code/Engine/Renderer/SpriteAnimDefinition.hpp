#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

class SpriteAnimDefinition
{
	friend class SpriteAnim;
	friend class SpriteAnimSetDefinition;

public:
	SpriteAnimDefinition( const tinyxml2::XMLElement& animElement,  const SpriteSheet& defaultSpriteSheet,
		float defaultFps);

	~SpriteAnimDefinition();

	float GetDuration() const { return (float) m_spriteIndexes.size() / m_framesPerSecond; }

protected:
	std::string	m_name;
	SpriteSheet* m_spriteSheet = nullptr;
	float m_framesPerSecond = 10.f;
	std::vector<int> m_spriteIndexes;
	bool m_isLooping = true;
};
