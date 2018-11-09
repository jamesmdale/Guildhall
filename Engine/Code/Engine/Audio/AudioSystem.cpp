#include "Engine\Audio\AudioSystem.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Engine\Core\DevConsole.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
#include "Game/EngineBuildPreferences.hpp"
//#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod_vc.lib" )
#endif

static AudioSystem* g_theAudioSystem = nullptr;

bool isMuted = false;
float tempPreviousMasterVolume = 1.0f;


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );
}

AudioSystem* AudioSystem::CreateInstance()
{
	if (g_theAudioSystem == nullptr)
	{
		g_theAudioSystem = new AudioSystem();
	}

	return g_theAudioSystem;
}

AudioSystem * AudioSystem::GetInstance()
{
	return g_theAudioSystem;
}

void AudioSystem::Startup()
{
	//register devconsole commands
	RegisterCommand("set_volume", CommandRegistration(SetOverallVolume, ": Type set_volume with 0.0f to 1.0f value to set overall volume"));
	RegisterCommand("toggle_mute", CommandRegistration(ToggleMute, ": Toggle sound mute"));
}

void AudioSystem::Shutdown()
{
	delete(g_theAudioSystem);
	g_theAudioSystem = nullptr;
}

//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );

		if(isMuted)
			channelAssignedToSound->setVolume(0.f);
		else
			channelAssignedToSound->setVolume(volume);


		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	m_channelAssignments.push_back(channelAssignedToSound);

	return (SoundPlaybackID) channelAssignedToSound;
}

SoundPlaybackID AudioSystem::PlaySoundFromGroup(const std::string& soundGroupName)
{
	AudioGroup* audioGroup = nullptr;
	for (int audioGroupIndex = 0; audioGroupIndex < (int)m_registeredAudioGroups.size(); ++audioGroupIndex)
	{
		if (soundGroupName == m_registeredAudioGroups[audioGroupIndex]->groupName)
		{
			audioGroup = m_registeredAudioGroups[audioGroupIndex];
			break;
		}
	}

	if (audioGroup == nullptr)	
		return MISSING_SOUND_ID;
	
	int randomSound = GetRandomIntInRange(0, (int)audioGroup->soundIds.size() - 1);

	SoundPlaybackID id = PlaySound(audioGroup->soundIds[randomSound], audioGroup->isLooped, audioGroup->volume, audioGroup->balance, audioGroup->speed, audioGroup->isPaused);

	m_channelAssignments.push_back((FMOD::Channel*)id);

	return id;
}

void AudioSystem::CreateOrGetAudioGroupFromXML(const std::string& xmlFilePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(xmlFilePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	AudioGroup* audioGroup = new AudioGroup();
	std::string filePath = "Data/Audio/";

	audioGroup->groupName = ParseXmlAttribute(*pRoot, "name", audioGroup->groupName);
	filePath = ParseXmlAttribute(*pRoot, "path", filePath);

	for (const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{	
		std::string soundName = "invalid";

		soundName = ParseXmlAttribute(*definitionNode, "name", soundName);
		std::string audioPath = Stringf("%s%s", filePath.c_str(), soundName.c_str());
		SoundID id = CreateOrGetSound(audioPath);

		if (id != MISSING_SOUND_ID)
		{
			audioGroup->soundIds.push_back(id);
		}		
	}

	m_registeredAudioGroups.push_back(audioGroup);
	audioGroup = nullptr;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


// static methods =========================================================================================
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}

void AudioSystem::SetMasterVolume(float volume)
{
	FMOD::ChannelGroup* master;
	m_fmodSystem->getMasterChannelGroup(&master);

	master->setVolume(volume);

	master = nullptr;
}

float AudioSystem::GetMasterVolume()
{
	FMOD::ChannelGroup* master;
	m_fmodSystem->getMasterChannelGroup(&master);

	float volume = 1.f;

	master->getVolume(&volume);

	master = nullptr;
	return volume;
}


void AudioSystem::ToggleMasterMute()
{
	AudioSystem* theAudio = AudioSystem::GetInstance();

	isMuted = !isMuted;

	if (isMuted == true)
	{
		tempPreviousMasterVolume = theAudio->GetMasterVolume();
		theAudio->SetMasterVolume(0.0f);
	}
	else
	{
		theAudio->SetMasterVolume(tempPreviousMasterVolume);
	}

	theAudio = nullptr;
}


// commands =========================================================================================

void SetOverallVolume(Command &cmd)
{
	AudioSystem* theAudio = AudioSystem::GetInstance();

	float overallVolume = ClampFloatZeroToOne(cmd.GetNextFloat());
	theAudio->SetMasterVolume(overallVolume);

	cmd.m_commandInfo->m_successMessage = Stringf("%s %f", "Volume set to ", overallVolume);
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());

	theAudio = nullptr;
}

void ToggleMute(Command & cmd)
{
	AudioSystem* theAudio = AudioSystem::GetInstance();

	theAudio->ToggleMasterMute();

	std::string muteString;

	if(isMuted)
		muteString = "Sound muted!";
	else
		muteString = "Sound unmuted!";

	cmd.m_commandInfo->m_successMessage = Stringf("%s", muteString);
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());

	theAudio = nullptr;
}


//#endif  !defined( ENGINE_DISABLE_AUDIO )
