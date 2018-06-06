#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RenderScene.hpp"
#include "Engine\Renderer\ForwardRenderingPath.hpp"

//
//enum eMenuState
//{
//	NONE_MENU_STATE,
//	LOADING_MENU_STATE,
//	MAIN_MENU_STATE,	
//	DECK_SELECTION_MENU_STATE,
//	PLAYING_MENU_STATE,
//	NUM_GAME_STATES
//};

class MenuState
{
public:
	MenuState(ForwardRenderingPath* renderingPath, Camera* camera);

	virtual ~MenuState();	

	virtual void Update(float deltaTime);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual float UpdateFromInput(float deltaTime);
	virtual void ResetMenuState();

	//static methods
	static void UpdateGlobalMenuState(float timeDelta);
	static void TransitionMenuStates(const MenuState& toState);

public:
	//eMenuState m_state = NONE_MENU_STATE;
	Camera* m_camera = nullptr;
	ForwardRenderingPath* m_renderingPath = nullptr;
	RenderScene* m_renderScene = nullptr;

	bool m_doesResetOnTransition = true;

	//static variables
	static MenuState* s_currentState;
	static  MenuState* s_transitionState;
	static float s_secondsInState;
	static float s_secondsTransistioning;
	static bool s_isFinishedTransistioning;	
};

