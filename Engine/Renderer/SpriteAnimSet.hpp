#pragma once
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include <map>
#include "Engine/Renderer/Texture.hpp"
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"

class SpriteAnimSet
{
public:
	SpriteAnimSet( SpriteAnimSetDefinition* animSetDef );
	~SpriteAnimSet();

	void Update( float deltaSeconds );
	void StartAnim( const std::string& animName );
	void SetCurrentAnim( const std::string& animName );
	const Texture& 	GetCurrentTexture() const;
	AABB2 GetCurrentUVs() const;
	SpriteAnim* GetCurrentAnim() const {return m_currentAnim;};

protected:
	SpriteAnimSetDefinition* m_animSetDef = nullptr;
	std::map< std::string, SpriteAnim* > m_namedAnims;
	SpriteAnim*	m_currentAnim = nullptr;
};
