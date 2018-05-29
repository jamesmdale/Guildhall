#include "Engine\Renderer\SpriteAnim.hpp"
#include "Engine\Core\EngineCommon.hpp"

SpriteAnim::SpriteAnim( const SpriteAnimDefinition* animDef )
{
	m_animDef = animDef;
	m_elapsedSeconds = 0.0f;
	m_isPlaying = true;
	m_isFinished = false;
}

void SpriteAnim::Update( float deltaSeconds )
{
	m_elapsedSeconds += deltaSeconds;

	if(GetFractionElapsed() >= 1.f)
	{
		if(m_animDef->m_isLooping)
		{
			m_elapsedSeconds = 0.0f;
			PlayFromStart();
		}
		else
		{
			m_isFinished = true;
			m_isPlaying = false;
		}				
	}
	else
	{
		m_currentTextureCoordinates = GetCurrentTexCoords();
	}
}

AABB2 SpriteAnim::GetCurrentTexCoords() const	
{
	int currentSpriteIndex = (int)(GetFractionElapsed() * (m_animDef->m_spriteIndexes.size() - 1));
	AABB2 currentSprite = m_animDef->m_spriteSheet->GetTexCoordsForSpriteIndex( m_animDef->m_spriteIndexes.at(currentSpriteIndex));

	TODO("need to flip the sprite. Could come back and re-adjust how all drawing works.");
	AABB2 adjustedSprite;
	adjustedSprite.mins = Vector2(currentSprite.mins.x, currentSprite.maxs.y);
	adjustedSprite.maxs = Vector2(currentSprite.maxs.x, currentSprite.mins.y);
	return adjustedSprite;
}

void SpriteAnim::PlayFromStart()
{
	m_elapsedSeconds = 0.0f;
	m_isFinished = false;
	m_isPlaying = true;	
}

float SpriteAnim::GetSecondsRemaining() const
{
	return m_animDef->GetDuration() - m_elapsedSeconds;
}
float SpriteAnim::GetFractionElapsed() const
{
	return m_elapsedSeconds/m_animDef->GetDuration();
}
float SpriteAnim::GetFractionRemaining() const
{
	return m_elapsedSeconds/m_animDef->GetDuration();
}
void SpriteAnim::SetSecondsElapsed( float secondsElapsed )   
{
	m_elapsedSeconds = secondsElapsed;
}
void SpriteAnim::SetFractionElapsed( float fractionElapsed )
{
	m_elapsedSeconds = fractionElapsed * m_animDef->GetDuration();
}

const Texture* SpriteAnim::GetTexture() const
{
	const Texture* spriteSheetTexture = m_animDef->m_spriteSheet->GetSpriteSheetTexture();
	return spriteSheetTexture;
}

std::string SpriteAnim::GetName() const
{
	return m_animDef->m_name;
}

int SpriteAnim::GetTexIndexForTime()
{
	int totalSpritesToAnimate = (int)m_animDef->m_spriteIndexes.size();	
	float currentSpriteInSeconds = m_elapsedSeconds/m_animDef->GetDuration();

	int currentSpriteIndex = (int)(currentSpriteInSeconds * totalSpritesToAnimate);
	return currentSpriteIndex;
}