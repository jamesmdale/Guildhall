#include "SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex) : m_spriteSheet(spriteSheet)
{
	m_durationSeconds = durationSeconds;
	m_elapsedSeconds = 0.0f;
	m_spriteAnimationMode = playbackMode;
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;
	m_isPlaying = true;
	m_isFinished = false;
}

void SpriteAnimation::Update( float deltaSeconds )
{
	m_elapsedSeconds += deltaSeconds;
	if(m_elapsedSeconds >= m_durationSeconds)
	{
		m_isFinished = true;
		m_isPlaying = false;		
	}
	else
	{
		m_currentTextureCoordinates = GetCurrentTexCoords();
	}
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const	
{
	int totalSpritesToAnimate = m_endSpriteIndex - m_startSpriteIndex;	
	float currentSpriteInSeconds = m_elapsedSeconds/(float)m_durationSeconds;

	int currentSpriteIndex = (int)(currentSpriteInSeconds * totalSpritesToAnimate);

	AABB2 currentSprite = m_spriteSheet.GetTexCoordsForSpriteIndex(currentSpriteIndex);
	return currentSprite;
}
const Texture* SpriteAnimation::GetTexture() const
{
	const Texture* spriteSheetTexture = m_spriteSheet.GetSpriteSheetTexture();
	return spriteSheetTexture;
}
void SpriteAnimation::Pause()
{

}
void SpriteAnimation::Resume()
{

}
void SpriteAnimation::Reset()
{
	m_elapsedSeconds = 0.0f;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	return m_durationSeconds - m_elapsedSeconds;
}
float SpriteAnimation::GetFractionElapsed() const
{
	return m_elapsedSeconds/m_durationSeconds;
}
float SpriteAnimation::GetFractionRemaining() const
{
	return m_elapsedSeconds/m_durationSeconds;
}
void SpriteAnimation::SetSecondsElapsed( float secondsElapsed )   
{
	m_elapsedSeconds = secondsElapsed;
}
void SpriteAnimation::SetFractionElapsed( float fractionElapsed )
{
	m_elapsedSeconds = fractionElapsed * m_durationSeconds;
}