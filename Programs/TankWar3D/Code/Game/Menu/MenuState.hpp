#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RenderScene.hpp"

enum eGameState
{
	NONE_MENU_STATE,
	LOADING_MENU_STATE,
	MAIN_MENU_STATE,	
	READY_UP_MENU_STATE,
	PLAYING_MENU_STATE,
	NUM_MENU_STATES
};

class GameState
{
public:
	GameState(Camera* camera);

	virtual ~GameState();	

	virtual void Update(float deltaSeconds);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual float UpdateFromInput(float deltaSeconds);

	//transition methods
	virtual void TransitionIn(float secondsTransitioning);
	virtual void TransitionOut(float secondsTransitioning);

	virtual void ResetState();
	virtual void Initialize();

	bool IsInitialized(){return m_isInitialized;};

	//static methods
	static void UpdateGlobalMenuState(float deltaSeconds);

	TODO("6/5 - Add logic for handling update,prerender,render, etc when transitioning");
	static void TransitionMenuStates(GameState* toState);
	static void TransitionMenuStatesImmediate(GameState* toState);

	static GameState* GetCurrentMenuState();
	static GameState* GetTransitionMenuState();

	//list managers
	TODO("6/5 - Add better way of managing these. For now, we assume they only have one max of each possibel type");
	static GameState* GetMenuStateFromGlobalListByType(eGameState menuStateType);
	static void AddMenuState(GameState* menuState);

	static float GetSecondsInCurrentState();

	

private:
	static void FinishTransition();	

public:
	eGameState m_type = NONE_MENU_STATE;
	Camera* m_camera = nullptr;
	RenderScene* m_renderScene = nullptr;

	bool m_doesResetOnTransition = true;
	
	static bool s_isFinishedTransitioningOut;
	static bool s_isFinishedTransitioningIn;
	bool m_isInitialized = false;

private:
	static float s_secondsInState;
	static float s_secondsTransitioning;	
	static std::vector<GameState*> s_menuStates;
};

//static variables
extern GameState* g_currentState;
extern GameState* g_transitionState;


