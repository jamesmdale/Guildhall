#pragma once
#include "Engine\ThirdParty\fmod\fmod.hpp"
#include <string>
#include <vector>
#include <map>
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
class AudioSystem;

struct AudioGroup
{
	std::string groupName;
	std::vector<SoundID> soundIds;

	TODO("Add sound settings to individual sound ids in a new struct for added control");
	//sound settings
	bool isLooped = false;
	float volume = 1.0f;
	float balance = 0.0f;
	float speed = 1.0f;
	bool isPaused = false;
};


//-----------------------------------------------------------------------------------------------
class AudioSystem
{
public:
	AudioSystem();
	static AudioSystem* CreateInstance();
	static AudioSystem* GetInstance();
	virtual ~AudioSystem();

	void Startup();
	void Shutdown();	

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual SoundPlaybackID     PlaySoundFromGroup(const std::string& soundGroupName);
	void						CreateOrGetAudioGroupFromXML(const std::string & xmlFilePath);
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );
	virtual void				SetMasterVolume(float volume);
	virtual float				GetMasterVolume();

	static void					ToggleMasterMute();

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
	std::vector<AudioGroup*>			m_registeredAudioGroups;
	std::vector<FMOD::Channel*>			m_channelAssignments;
};


void SetOverallVolume(Command& cmd);
void ToggleMute(Command& cmd);


