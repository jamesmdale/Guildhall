#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
class SpriteAnim
{
public:
	SpriteAnim( const SpriteAnimDefinition* animDef );
	~SpriteAnim();

	void Update( float deltaSeconds );
	void PlayFromStart();
	bool IsFinished() const { return m_isFinished; }
	float GetElapsedSeconds() const { return m_elapsedSeconds; }
	float GetFractionElapsed() const; // Hint: Asks its SpriteAnimDefinition for total duration
	float GetSecondsRemaining() const;
	float GetFractionRemaining() const;
	void SetSecondsElapsed( float secondsElapsed );
	void SetFractionElapsed( float fractionElapsed );

	int GetTexIndexForTime();

	const Texture* GetTexture() const;
	AABB2 GetCurrentTexCoords() const;
	std::string GetName() const;

	//TODO: Might be some useful functions later
	//void Resume()
	//void Pause()

protected:
	const SpriteAnimDefinition*	m_animDef = nullptr;
	AABB2 m_currentTextureCoordinates;
	bool m_isPlaying = true;
	bool m_isFinished = false;
	float m_elapsedSeconds = 0.f;
};
