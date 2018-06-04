#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\GameObject.hpp"
#include <vector>
#include "Engine\Renderer\RenderScene.hpp"
#include "Engine\Renderer\ForwardRenderingPath.hpp"
#include "Engine\ParticleSystem\ParticleEmitter.hpp"

class Game
{
public:  
	Camera* m_gameCamera = nullptr;

	ForwardRenderingPath* m_forwardRenderingPath = nullptr;
	RenderScene* m_renderScene = nullptr;

public:
	Game();
	~Game();
	void Update(float timeDelta);
	void TransitionGameStates(GameState toState);
	void PreRender();
	void Render();
	void PostRender();
	void Initialize();
	float UpdateInput(float timeDelta);
};

extern Game* g_theGame;
extern Clock* g_gameClock;






