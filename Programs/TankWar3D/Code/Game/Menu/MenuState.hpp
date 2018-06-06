#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RenderScene.hpp"

enum eMenuState
{
	NONE_MENU_STATE,
	LOADING_MENU_STATE,
	MAIN_MENU_STATE,	
	DECK_SELECTION_MENU_STATE,
	PLAYING_MENU_STATE,
	NUM_GAME_STATES
};

class MenuState
{
public:
	MenuState(Camera* camera);

	virtual ~MenuState();	

	virtual void Update(float deltaTime);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual void PreRender();
	virtual float UpdateFromInput(float deltaTime);
	virtual void ResetMenuState();

	//transition methods
	virtual void TransitionIn(float secondsTransitioning);
	virtual void TransitionOut(float secondsTransitioning);

	//static methods
	static void UpdateGlobalMenuState(float timeDelta);
	static void TransitionMenuStates(MenuState* toState);
	static void TransitionMenuStatesImmediate(MenuState* toState);

	static MenuState* GetCurrentMenuState();
	static MenuState* GetTransitionMenuState();

public:
	eMenuState m_state = NONE_MENU_STATE;
	Camera* m_camera = nullptr;
	RenderScene* m_renderScene = nullptr;

	bool m_doesResetOnTransition = true;

	static float s_secondsInState;
	static float s_secondsTransitioning;
	static bool s_isFinishedTransitioningOut;
	static bool s_isFinishedTransitioningIn;
};

//static variables
extern MenuState* g_currentState;
extern MenuState* g_transitionState;


