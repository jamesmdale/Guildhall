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
#include "Engine\Renderer\ForwardRenderingPath2D.hpp"

class Game
{
public:  

	//camera members
	Camera* m_gameCamera = nullptr;
	Camera* m_uiCamera = nullptr;

	//rendering members
	ForwardRenderingPath* m_forwardRenderingPath = nullptr;
	ForwardRenderingPath2D* m_forwardRenderingPath2D = nullptr;

	//clock
	Clock* m_gameClock = nullptr;

	//game members
	TODO("Add player tank class");
	//Tank* m_playerTank = nullptr;

public:
	Game();
	~Game();
	static Game* GetInstance();
	static Game* CreateInstance();

	void Update(); //use internal clock for delta seconds;
	void PreRender();
	void Render();
	void PostRender();
	void Initialize();
	float UpdateInput(float deltaSeconds);
};






