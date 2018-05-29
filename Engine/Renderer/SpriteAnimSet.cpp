#include "Engine/Renderer/SpriteAnimSet.hpp"

SpriteAnimSet::SpriteAnimSet( SpriteAnimSetDefinition* animSetDef )
{
	m_animSetDef = animSetDef;

	std::map<std::string, SpriteAnimDefinition*>::iterator defIterator = m_animSetDef->m_namedAnimDefs.begin();

	// Iterate over the map using Iterator till end.
	while (defIterator != m_animSetDef->m_namedAnimDefs.end())
	{	
		SpriteAnim* newAnim = new SpriteAnim(defIterator->second);
		m_namedAnims[defIterator->first] = newAnim;
		defIterator++;
	}

	SetCurrentAnim("Idle"); //idle is our default.  If we don't find and idle, it doesn't set one.
}
void SpriteAnimSet::Update( float deltaSeconds )
{
	//TODO: add logic determining which anim to run from here.  This is the animation related pointer held in the entity itself.
	m_currentAnim->Update(deltaSeconds);
}
void SpriteAnimSet::StartAnim( const std::string& animName )
{
	SetCurrentAnim(animName);
	m_currentAnim->PlayFromStart();
}
void SpriteAnimSet::SetCurrentAnim( const std::string& animName )
{
	auto animIndex = m_namedAnims.find(animName);
	if(animIndex != m_namedAnims.end())
	{
		m_currentAnim = m_namedAnims.at(animName);		
	}
	else
	{
		m_currentAnim = m_namedAnims.at("Idle");
	}

	m_currentAnim->PlayFromStart();
}
const Texture& 	SpriteAnimSet::GetCurrentTexture() const
{
	 return *(m_currentAnim->GetTexture());
}
AABB2 SpriteAnimSet::GetCurrentUVs() const
{
	return m_currentAnim->GetCurrentTexCoords();
}
