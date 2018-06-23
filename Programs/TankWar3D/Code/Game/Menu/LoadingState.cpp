#include "Game\Menu\LoadingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Audio\AudioSystem.hpp"

static bool s_isLoading = false;

LoadingState::~LoadingState()
{
	m_backGroundTexture = nullptr;
}

void LoadingState::Update(float deltaSeconds)
{
	if (s_isLoading)
	{
		Renderer* theRenderer = Renderer::GetInstance();
		AudioSystem* theAudio = AudioSystem::GetInstance();

		// load intensive resources (big textures, audio, etc) =========================================================================================
		
		//load textures
		theRenderer->CreateOrGetImage("Data/Images/galaxy2.png");
		theRenderer->CreateOrGetImage("Data/Images/terrain.jpg");

		//load audio
		theAudio->CreateOrGetAudioGroupFromXML("Data/Audio/AudioGroups/Lasers.xml");

		//sleep to insure loading screen visibility
		Sleep(1);

		//after you are finished loading
		GameState* state = GetMenuStateFromGlobalListByType(MAIN_MENU_STATE);
		GUARANTEE_OR_DIE(state != nullptr, "LOADING STATE TRANSITION: PLAYING STATE NOT FOUND");

		TransitionMenuStatesImmediate(GetMenuStateFromGlobalListByType(MAIN_MENU_STATE));

		theAudio = nullptr;
		theRenderer = nullptr;
	}	
}

void LoadingState::PreRender()
{
}

void LoadingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();

	theRenderer->SetCamera(m_camera);

	theRenderer->SetTexture(*m_backGroundTexture);
	theRenderer->SetShader(theRenderer->m_defaultShader);

	theRenderer->m_defaultShader->EnableColorBlending(BLEND_OP_ADD, BLEND_SOURCE_ALPHA, BLEND_ONE_MINUS_SOURCE_ALPHA);

	theRenderer->DrawAABB(theWindow->GetClientWindow(), Rgba(0.f, 0.f, 0.f, 1.f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), "Loading!", theWindow->m_clientHeight * .1f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	
	theRenderer->m_defaultShader->DisableBlending();

	//render on the first pass and then update will happen on the second
	if(s_isLoading == false)
	{ 
		s_isLoading = true;
	}
	

	theRenderer = nullptr;
}

float LoadingState::UpdateFromInput(float deltaSeconds)
{
	return deltaSeconds;
}

void LoadingState::PostRender()
{
}
