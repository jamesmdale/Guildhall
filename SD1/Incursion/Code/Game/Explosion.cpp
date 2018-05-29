#include "Explosion.hpp"
#include "GameCommon.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Core\Rgba.hpp"

Explosion::Explosion(Vector2 position, float radius, float duration)
{
	m_position = position;
	m_radius = radius;
	m_duration = duration;

	SpriteSheet* explosionSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTexture("Data/Images/Explosion_5x5.png"), 5, 5);
	m_animation = new SpriteAnimation(*explosionSheet, m_duration, SPRITE_ANIM_MODE_PLAY_TO_END, 0, 24);
}

void Explosion:: Update(float deltaTime)
{
	m_animation->Update(deltaTime);
}

void Explosion::Render()
{
	AABB2 currentAnimationCoords = m_animation->GetCurrentTexCoords();
	SpriteSheet explosionSheet = SpriteSheet( *g_theRenderer->CreateOrGetTexture("Data/Images/Explosion_5x5.png"), 5, 5);
	AABB2 spriteBounds = AABB2(Vector2(0.f, 0.f), m_radius, m_radius);

	g_theRenderer->PushMatrix();	
	g_theRenderer->Translatef(m_position);	
	g_theRenderer->Scalef(m_radius, m_radius, m_radius);
	g_theRenderer->Blend();
	g_theRenderer->DrawTexturedAABB(spriteBounds, *(explosionSheet.GetSpriteSheetTexture()), currentAnimationCoords.mins, currentAnimationCoords.maxs, Rgba(1.f, 1.f, 1.f, 1.f));
	g_theRenderer->ResetBlend();
	g_theRenderer->PopMatrix();
}