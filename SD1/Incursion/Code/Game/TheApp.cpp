#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\core\Time.hpp"
#include "Engine\Audio\AudioSystem.hpp"


TheApp* g_TheApp = nullptr;
double previousTime = 0;

TheApp::TheApp()
{
	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
	g_theAudio = new AudioSystem();
	g_Game = new Game();

	CreateSounds();
}

TheApp::~TheApp()
{
	delete g_Game;
	g_Game = nullptr;
	delete g_theAudio;
	g_theAudio = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
}

void TheApp::RunFrame()
{
	g_theRenderer->BeginFrame();	
	g_theInput->BeginFrame();
	g_theAudio->BeginFrame();

	Update();
	Render();

	g_theAudio->EndFrame();
	g_theInput->EndFrame();
	g_theRenderer->EndFrame();	
	
}

void TheApp::Initialize()
{
	
}

//theapp.beginFrame - begin frame for engine components
void TheApp::Update()
{
	double currentTime = GetCurrentTimeSeconds();
	float timeDelta = (float)(currentTime - previousTime);
	timeDelta = ClampFloat(timeDelta, 0.0f, 0.1f); //clamp time delta to prevent crazy scenarios happening.
	previousTime = currentTime;

	g_Game->Update(timeDelta); 
}

void TheApp::Render()
{	
	g_Game->Render();
}

bool TheApp::IsQuitting()
{	
	return g_isQuitting;
}

void TheApp::CreateSounds()
{
	g_theAudio->CreateOrGetSound("Data/Audio/EnemyDied.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/GameOver.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/GamePlayMusic.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/Pause.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/PlayerDied.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/PlayerHit.wav");	
	g_theAudio->CreateOrGetSound("Data/Audio/Unpause.mp3");	
	g_theAudio->CreateOrGetSound("Data/Audio/QuitGame.mp3");	
	g_theAudio->CreateOrGetSound("Data/Audio/Victory.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/StartGame.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/TestSound.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
}

