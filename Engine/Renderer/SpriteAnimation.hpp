#pragma once
#include "Engine\math\AABB2.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"

enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END = 0, //play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,	//play from time=0 to end then repeat (never finish)
	SPRITE_ANIM_MODE_PINGPONG,	 //optional, play forwards, backwards, forwards, etc..
	NUM_SPRITE_ANIM_MODES
};

class SpriteAnimation
{
public:
	SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex);

	void Update( float deltaSeconds );
	AABB2 GetCurrentTexCoords() const;	// Based on the current elapsed time
	const Texture* GetTexture() const;
	void Pause();					// Starts unpaused (playing) by default
	void Resume();				// Resume after pausing
	void Reset();					// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const			{ return m_isFinished; }
	bool IsPlaying() const			{ return m_isPlaying; }
	float GetDurationSeconds() const	{ return m_durationSeconds; }
	float GetSecondsElapsed() const		{ return m_elapsedSeconds; }
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	void SetSecondsElapsed( float secondsElapsed );	    // Jump to specific time
	void SetFractionElapsed( float fractionElapsed );    // e.g. 0.33f for one-third in

public:
	const SpriteSheet& m_spriteSheet;
	AABB2 m_currentTextureCoordinates;
	SpriteAnimMode m_spriteAnimationMode;
	int m_startSpriteIndex;
	int m_endSpriteIndex;
	bool m_isFinished;
	bool m_isPlaying;
	float m_durationSeconds;
	float m_elapsedSeconds;
};
