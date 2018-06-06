#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RenderScene.hpp"

static enum eMenuState
{
	NONE_MENU_STATE,
	LOADING_MENU_STATE,
	MAIN_MENU_STATE,	
	READY_UP_MENU_STATE,
	PLAYING_MENU_STATE,
	NUM_MENU_STATES
};

class MenuState
{
public:
	MenuState(Camera* camera);

	virtual ~MenuState();	

	virtual void Update(float deltaSeconds);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual float UpdateFromInput(float deltaSeconds);

	//transition methods
	virtual void TransitionIn(float secondsTransitioning);
	virtual void TransitionOut(float secondsTransitioning);

	//static methods
	static void UpdateGlobalMenuState(float deltaSeconds);

	TODO("6/5 - Add logic for handling update,prerender,render, etc when transitioning");
	static void TransitionMenuStates(MenuState* toState);
	static void TransitionMenuStatesImmediate(MenuState* toState);

	static MenuState* GetCurrentMenuState();
	static MenuState* GetTransitionMenuState();

	//list managers
	TODO("6/5 - Add better way of managing these. For now, we assume they only have one max of each possibel type");
	static MenuState* GetMenuStateFromListByType(eMenuState menuStateType);
	static void AddMenuState(MenuState* menuState);

	static float GetSecondsInCurrentState();

private:
	static void FinishTransition();
	

public:
	eMenuState m_type = NONE_MENU_STATE;
	Camera* m_camera = nullptr;
	RenderScene* m_renderScene = nullptr;

	bool m_doesResetOnTransition = true;

private:
	static float s_secondsInState;
	static float s_secondsTransitioning;
	static bool s_isFinishedTransitioningOut;
	static bool s_isFinishedTransitioningIn;
	static std::vector<MenuState*> s_menuStates;
};

//static variables
extern MenuState* g_currentState;
extern MenuState* g_transitionState;


