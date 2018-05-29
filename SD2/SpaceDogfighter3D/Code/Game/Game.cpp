#include "Game.hpp"
#include <stdlib.h>
#include "Engine\Math\MathUtils.hpp"
#include "engine\renderer\renderer.hpp"
#include "game\GameCommon.hpp"
#include "engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\GameObject.hpp"
#include <vector>
#include <string>
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\LightObject.hpp"
#include "Engine\Core\EngineCommon.hpp"

Game* g_theGame = nullptr;
Clock* g_gameClock = nullptr;

bool isLightSourceAttachedToCamera = true;
int shaderMode = 0;
int numStartingAsteroids = 3;
std::string currentRenderMode = "default";
bool isDebugText = false;

Game::Game()
{
	m_renderScene = new RenderScene();
	m_forwardRenderingPath = new ForwardRenderingPath();
}

Game::~Game()
{
	delete(m_forwardRenderingPath);
	m_forwardRenderingPath = nullptr;

	delete(m_renderScene);
	m_renderScene = nullptr;	

	delete(m_playerShip);
	m_playerShip = nullptr;

	delete(m_gameCamera);
	m_gameCamera = nullptr;	
}

void Game::Initialize()
{
	//initialize game camera
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	g_gameClock = new Clock(GetMasterClock());

	//create meshbuilder for initialization
	MeshBuilder meshBuilder;

/************************************************************************/
/* Create Asteroids In Scene                                            */
/************************************************************************/

	for(int asteroidIndex = 0; asteroidIndex < numStartingAsteroids; asteroidIndex++)
	{
		Asteroid* asteroid = new Asteroid(ASTEROID_LEVEL_1);
		asteroid->m_renderScene = m_renderScene;
		meshBuilder.CreateUVSphere( Vector3::ZERO, 1.f, 15, 15, Rgba::WHITE);
		asteroid->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexLit>());	

		asteroid->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("asteroid"));
	
		asteroid->m_transform->SetLocalPosition(GetRandomVector3InSphere(-75.f, 75.f));
		asteroid->m_renderScene = m_renderScene;

		//add light to lists
		m_renderScene->AddRenderable(asteroid->m_renderable);

		m_asteroids.push_back(asteroid);
	}

/************************************************************************/
/*  Create Camera                                                       */
/************************************************************************/

	m_gameCamera = new Camera();
	m_gameCamera->SetDepthStencilTarget(theRenderer->GetDefaultDepthStencilTarget());
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());	
	m_gameCamera->SetPerspective(60.f, CLIENT_ASPECT, 0.1f, 10000.f);
	m_gameCamera->Translate(Vector3(0.f, 5.f, -20.f));

	m_renderScene->AddCamera(m_gameCamera);	

/************************************************************************/
/* Create Directional Light                                             */
/************************************************************************/

	Rgba lightColor = Rgba::WHITE;
	LightObject* directionalLight = new LightObject("directionalLight", LIGHT_TYPE_DIRECTIONAL_LIGHT, lightColor, 15.f, Vector3(1.f, 0.f, 0.f), 1.f, 360.f, 360.f);
	meshBuilder.CreateUVSphere( Vector3::ZERO, 10.f, 25, 25, lightColor);

	directionalLight->m_transform->SetLocalPosition(Vector3(200.f, 0.f, 0.f));
	directionalLight->m_renderScene = m_renderScene;
	directionalLight->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexPCU>());
	directionalLight->m_renderable->SetMaterial(new Material());
	directionalLight->m_renderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	directionalLight->m_renderable->GetMaterial()->SetTexture(directionalLight->m_renderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	directionalLight->m_renderable->GetMaterial()->SetSampler(directionalLight->m_renderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);

	directionalLight->m_transform->SetLocalRotation(Vector3(0.f, -90.f, 0.f));
	directionalLight->UpdateRenderableFromTransform();
	directionalLight->UpdateLightDataFromWorldTransform();

	directionalLight->PreRender();

	//add light to lists
	m_renderScene->AddLight(directionalLight->m_light);
	m_renderScene->AddRenderable(directionalLight->m_renderable);


/************************************************************************/
/* Create Ship                                                          */
/************************************************************************/

	m_playerShip = new Ship();
	m_playerShip->SetCamera(m_gameCamera);

	//create ship game object
	meshBuilder.LoadObjectFromFile("Data/Model/scifi_fighter_mk6/scifi_fighter_mk6.obj");
	m_playerShip->m_renderScene = m_renderScene;
	m_playerShip->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexLit>());

	m_playerShip->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("ship"));

	m_playerShip->m_transform->SetLocalPosition(Vector3(0.f, 0.f, 0.f));

	//add ship to list
	m_renderScene->AddRenderable(m_playerShip->m_renderable);


	//create ship spot light
	lightColor = Rgba::WHITE;
	m_playerShip->SetSpotLight(new LightObject("lightSource", LIGHT_TYPE_SPOT_LIGHT, lightColor, 50.f, Vector3(0, 1.f, 0.75f), 1.f, 10.f, 30.f));
	meshBuilder.CreateUVSphere( Vector3::ZERO, .5f, 25, 25, lightColor);

	m_playerShip->m_spotLight->m_renderScene = m_renderScene;
	m_playerShip->m_spotLight->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexPCU>());
	m_playerShip->m_spotLight->m_renderable->SetMaterial(new Material());
	m_playerShip->m_spotLight->m_renderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
	m_playerShip->m_spotLight->m_renderable->GetMaterial()->SetTexture(m_playerShip->m_spotLight->m_renderable->GetMaterial()->GetNumTextures(), theRenderer->m_defaultTexture);
	m_playerShip->m_spotLight->m_renderable->GetMaterial()->SetSampler(m_playerShip->m_spotLight->m_renderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);
	
	//shipSpotLight->m_transform = Transform(m_playerShip->m_ship->m_transform);
	m_playerShip->m_spotLight->m_transform->TranslatePosition(Vector3(0.f, 0.f, 5.5f));

	//add object to renderable and light list
	m_renderScene->AddLight(m_playerShip->m_spotLight->m_light);
	m_renderScene->AddRenderable(m_playerShip->m_spotLight->m_renderable);	

	//test particle emitter
	m_playerShip->SetEmitter(new ParticleEmitter());
	m_playerShip->m_thrusterEmitter->m_transform->TranslatePosition(Vector3(0.f, 0.f, -5.f));
	m_playerShip->m_thrusterEmitter->m_particleTimeToLive = FloatRange(2.f, 3.f);
	m_playerShip->m_thrusterEmitter->m_isTemporary = false;
	m_playerShip->m_thrusterEmitter->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("smoke"));
	m_playerShip->m_thrusterEmitter->SetCamera(m_gameCamera);
	m_playerShip->m_thrusterEmitter->SetSpawnRatePerSecond(1.f);
	m_playerShip->m_thrusterEmitter->m_renderScene = m_renderScene;
	
	m_playerShip->m_spotLight->m_renderable->m_watch = m_playerShip->m_spotLight->m_transform;
	m_playerShip->m_renderable->m_watch = m_playerShip->m_transform;
	m_playerShip->m_thrusterEmitter->m_renderable->m_watch = m_playerShip->m_thrusterEmitter->m_transform;

	m_renderScene->AddRenderable(m_playerShip->m_thrusterEmitter->m_renderable);

	//cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

void Game::Update(float timeDelta)
{		
	UpdateInput(timeDelta);

	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();

	//if(isDebugText)
	//{
	//	std::string ambienceText = Stringf("[O][P] Ambience: %f", theRenderer->GetAmbientLightIntensity());
	//	std::string specAmountText = Stringf("[K][L] Specular Amount: %f", theRenderer->GetSpecularAmount());
	//	std::string specPowerText = Stringf("[N][M] Specular Power: %f", theRenderer->GetSpecularPower());
	//	std::string renderMode = Stringf("Debug Mode: %s", currentRenderMode.c_str());


	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 300, theWindow->m_clientHeight - 20), 20.f, 1.f, ambienceText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 300, theWindow->m_clientHeight - 40), 20.f, 1.f, specAmountText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 300, theWindow->m_clientHeight - 60), 20.f, 1.f, specPowerText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 300, theWindow->m_clientHeight - 80), 20.f, 1.f, renderMode, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);

	//	//camera info
	//	std::string cameraPositionText = Stringf("Camera Position (%f, %f, %f)", m_gameCamera->m_transform->GetLocalPositionX(), m_gameCamera->m_transform->GetLocalPositionY(), m_gameCamera->m_transform->GetLocalPositionZ());
	//	std::string cameraRotationText = Stringf("Camera Rotation (%f, %f, %f)", m_gameCamera->m_transform->GetLocalRotationAroundX(), m_gameCamera->m_transform->GetLocalRotationAroundY(), m_gameCamera->m_transform->GetLocalRotationAroundZ());
	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 500, theWindow->m_clientHeight - 100), 20.f, 1.f, cameraPositionText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	//	DebugRender::GetInstance()->CreateDebugText2D(Vector2(theWindow->m_clientWidth - 500, theWindow->m_clientHeight - 120), 20.f, 1.f, cameraRotationText, Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	//}	


	//update objects in scene
	m_playerShip->Update(timeDelta);
	
	//bullets
	for (Bullet* bullet : m_bullets)
	{
		bullet->Update(timeDelta);
	}

	//asteroids
	for (Asteroid* asteroid : m_asteroids)
	{
		asteroid->Update(timeDelta);
	}

	for (ParticleEmitter* emitter : m_collisionEmitters)
	{
		emitter->Update(timeDelta);
	}

	//handle for collisions
	for(int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); bulletIndex++)
	{
		for(int asteroidIndex = 0; asteroidIndex < (int)m_asteroids.size(); asteroidIndex++)
		{ 
			Vector3 bulletPosition = m_bullets[bulletIndex]->m_transform->GetWorldPosition();
			Vector3 asteroidPosition = m_asteroids[asteroidIndex]->m_transform->GetWorldPosition();

			float distance = GetDistance(m_bullets[bulletIndex]->m_transform->GetWorldPosition(), m_asteroids[asteroidIndex]->m_transform->GetWorldPosition());
			if(distance < m_asteroids[asteroidIndex]->GetAsteroidRadius())
			{
				//they have collided
				m_asteroids[asteroidIndex]->m_health--;
				m_bullets[bulletIndex]->MarkAsDead();

				SpawnCollisionEmitter(m_bullets[bulletIndex]->m_transform->GetLocalPosition(), (bulletPosition - asteroidPosition).GetNormalized());
			}
		}
	}


	theWindow = nullptr;
	theRenderer = nullptr;
}

void Game::PreRender()
{
	m_playerShip->PreRender();

	//bullets
	for (Bullet* bullet : m_bullets)
	{
		bullet->PreRender();
	}
	
	//asteroids
	for (Asteroid* asteroid : m_asteroids)
	{
		asteroid->PreRender();
	}	

	for (ParticleEmitter* emitter : m_collisionEmitters)
	{
		emitter->PreRender();
	}	
}

void Game::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	theRenderer->SetCamera(m_gameCamera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);
	
	//render from forward rendering path
	m_forwardRenderingPath->Render(m_renderScene);

	theRenderer = nullptr;
}

void Game::PostRender()
{
	MeshBuilder meshBuilder;
	Renderer* theRenderer = Renderer::GetInstance();

	for (int asteroidIndex = 0; asteroidIndex < (int)m_asteroids.size(); asteroidIndex++)
	{		
		if(m_asteroids[asteroidIndex]->m_health < 0)
		{
			switch(m_asteroids[asteroidIndex]->m_asteroidLevel)
			{
				case ASTEROID_LEVEL_1:
				{
					Vector3 asteroidWorldPosition = m_asteroids[asteroidIndex]->m_transform->GetWorldPosition(); 
					SpawnAsteroid(asteroidWorldPosition + GetRandomVector3InSphere(-20.f, 20.f), ASTEROID_LEVEL_2);
					SpawnAsteroid(asteroidWorldPosition + GetRandomVector3InSphere(-20.f, 20.f), ASTEROID_LEVEL_2);
					
					delete(m_asteroids[asteroidIndex]);
					m_asteroids[asteroidIndex] = nullptr;
					m_asteroids.erase(m_asteroids.begin() + asteroidIndex);
					asteroidIndex--;

					break;
				}
				case ASTEROID_LEVEL_2:
				{
					Vector3 asteroidWorldPosition = m_asteroids[asteroidIndex]->m_transform->GetWorldPosition(); 
					SpawnAsteroid(asteroidWorldPosition + GetRandomVector3InSphere(-20.f, 20.f), ASTEROID_LEVEL_3);
					SpawnAsteroid(asteroidWorldPosition + GetRandomVector3InSphere(-20.f, 20.f), ASTEROID_LEVEL_3);

					delete(m_asteroids[asteroidIndex]);
					m_asteroids[asteroidIndex] = nullptr;
					m_asteroids.erase(m_asteroids.begin() + asteroidIndex);
					asteroidIndex--;

					break;
				}
				case ASTEROID_LEVEL_3:
				{
					delete(m_asteroids[asteroidIndex]);
					m_asteroids[asteroidIndex] = nullptr;
					m_asteroids.erase(m_asteroids.begin() + asteroidIndex);
					asteroidIndex--;

					break;
				}
			}
		}
	}

	for (int bulletIndex = 0; bulletIndex < (int)m_bullets.size(); bulletIndex++)
	{
		if(!m_bullets[bulletIndex]->IsAlive())
		{
			delete(m_bullets[bulletIndex]);
			m_bullets[bulletIndex] = nullptr;
			
			m_bullets.erase(m_bullets.begin() + bulletIndex);
			bulletIndex--;
		}
	}

	for (int emitterIndex = 0; emitterIndex < (int)m_collisionEmitters.size(); emitterIndex++)
	{
		if(m_collisionEmitters[emitterIndex]->IsReadyForDelete())
		{
			delete(m_collisionEmitters[emitterIndex]);
			m_collisionEmitters[emitterIndex] = nullptr;

			m_collisionEmitters.erase(m_collisionEmitters.begin() + emitterIndex);
		}
	}
}

float Game::UpdateInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();

	m_playerShip->UpdateFromInput(timeDelta);

	if(!DevConsole::GetInstance()->IsOpen())
	{
		//update debug input
		/*if(theInput->WasKeyJustPressed((theInput->KEYBOARD_F9)))
		{
			theRenderer->ReloadShaders();
		}

		if(theInput->WasKeyJustPressed((theInput->KEYBOARD_V)))
		{
			isDebugText = !isDebugText;
		}*/

		////debug hotkeys
		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_1))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugPoint(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), .25f, Rgba::BLUE, Rgba::RED, 5.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_2))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugCube(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), Vector3(2.f, 2.f, 2.f), Rgba::RED, Rgba::BLUE, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_3))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugWireCube(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), Vector3(1.5f, 1.5f, 1.5f), Rgba::RED, Rgba::BLUE, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_4))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugQuad(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), Vector2(.25f, .25f), Rgba::RED, Rgba::BLUE, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_5))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugSphere(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), 1.f, Rgba::RED, Rgba::BLUE, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_6))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugWireSphere(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), 1.f, Rgba::RED, Rgba::BLUE, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//if(theInput->WasKeyJustPressed(theInput->KEYBOARD_7))
		//{
		//	DebugRender::GetInstance()->CreateDebugBasis(m_gameCamera->m_transform->GetWorldPosition() + m_gameCamera->m_transform->GetWorldForward(), 1.f, 10.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		//2d debug render examples
		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_8))) 
		//{
		//	Window* theWindow = Window::GetInstance();
		//	DebugRender::GetInstance()->CreateDebugCrosshair2D(theWindow->GetCenterOfClientWindow(), Rgba::WHITE, Rgba::WHITE, -1.f, ALWAYS_DEPTH_TYPE);
		//	theWindow = nullptr;
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_9))) 
		//{
		//	Window* theWindow = Window::GetInstance();
		//	DebugRender::GetInstance()->CreateDebugQuad2D(theWindow->GetCenterOfClientWindow(), Vector2(100.f, 100.f), Rgba::RED, Rgba::RED, 5.f, ALWAYS_DEPTH_TYPE);
		//	theWindow = nullptr;
		//}

		//if(theInput->WasKeyJustPressed((theInput->KEYBOARD_0)))
		//{
		//	Window* theWindow = Window::GetInstance();
		//	DebugRender::GetInstance()->CreateDebugLine2D(Vector2(theWindow->m_clientWidth, theWindow->m_clientWidth), Vector2(theWindow->m_clientWidth * 0.5f, theWindow->m_clientWidth * 0.5f), Rgba::RED, Rgba::BLUE, 10.f, ALWAYS_DEPTH_TYPE);
		//	theWindow = nullptr;
		//}

		//if(theInput->IsKeyPressed((theInput->KEYBOARD_C))) 
		//{
		//	DebugRender::GetInstance()->CreateDebugLine(m_gameCamera->m_transform->GetLocalPosition(), m_gameCamera->m_transform->GetLocalPosition() - m_gameCamera->m_transform->GetWorldForward() * 0.75f, Rgba::RED, Rgba::BLUE, 5.f, LESS_DEPTH_TYPE, m_gameCamera);
		//}

		////lighting hotkeys
		//if(theInput->IsKeyPressed((theInput->KEYBOARD_O))) 
		//{
		//	float ambientIntensity = theRenderer->GetAmbientLightIntensity();
		//	ambientIntensity -= 0.01f;
		//	ambientIntensity = ClampFloat(ambientIntensity, 0.f, 1.f);

		//	theRenderer->SetAmbientLightIntensity(ambientIntensity);
		//}
		//if(theInput->IsKeyPressed((theInput->KEYBOARD_P))) 
		//{
		//	float ambientIntensity = theRenderer->GetAmbientLightIntensity();

		//	ambientIntensity += 0.01f;
		//	ambientIntensity = ClampFloat(ambientIntensity, 0.f, 1.f);

		//	theRenderer->SetAmbientLightIntensity(ambientIntensity);
		//}

		//if(theInput->IsKeyPressed((theInput->KEYBOARD_K))) 
		//{
		//	float specularAmount = theRenderer->GetSpecularAmount();

		//	specularAmount -= 0.01f;
		//	specularAmount = ClampFloat(specularAmount, 0.f, 1.f);

		//	theRenderer->SetSpecularAmount(specularAmount);
		//}
		//if(theInput->IsKeyPressed((theInput->KEYBOARD_L))) 
		//{
		//	float specularAmount = theRenderer->GetSpecularAmount();

		//	specularAmount += 0.01f;
		//	specularAmount = ClampFloat(specularAmount, 0.f, 1.f);

		//	theRenderer->SetSpecularAmount(specularAmount);
		//}

		//if(theInput->IsKeyPressed((theInput->KEYBOARD_N))) 
		//{
		//	float specularPower = theRenderer->GetSpecularPower();

		//	specularPower -= 1.f;
		//	specularPower = ClampFloat(specularPower, 0.f, 9999999999.f);

		//	theRenderer->SetSpecularPower(specularPower);
		//}
		//if(theInput->IsKeyPressed((theInput->KEYBOARD_M))) 
		//{
		//	float specularPower = theRenderer->GetSpecularPower();

		//	specularPower += 1.f;
		//	specularPower = ClampFloat(specularPower, 0.f, 9999999999.f);

		//	theRenderer->SetSpecularPower(specularPower);
		//}		

		//add light with random properties in scene	at ship forward
		if(theInput->WasKeyJustPressed(theInput->KEYBOARD_U))
		{				
			MeshBuilder meshBuilder;

			Rgba newLightColor;
			bool isLightVisible = false;

			while(!isLightVisible)
			{
				newLightColor = Rgba(GetRandomFloatZeroOrOne(), GetRandomFloatZeroOrOne(), GetRandomFloatZeroOrOne(), 1.f);

				if(newLightColor != Rgba(0.f, 0.f, 0.f, 1.f))
				{
					isLightVisible = true;
				}
			}

			LightObject* newLight = new LightObject("lightSource", LIGHT_TYPE_POINT_LIGHT, newLightColor, 10.f, Vector3(0, 1.f, 0.75f), 0.f, 360.f, 360.f);
			newLight->m_renderScene = m_renderScene;
			meshBuilder.CreateUVSphere( Vector3::ZERO, .25f, 25, 25, newLightColor);

			newLight->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexLit>());
			newLight->m_renderable->SetMaterial(new Material());
			newLight->m_renderable->GetMaterial()->SetShader(theRenderer->m_defaultShader);
			newLight->m_renderable->GetMaterial()->SetTexture(newLight->m_renderable->GetMaterial()->GetNumTextures(), theRenderer->CreateOrGetTexture("default"));
			newLight->m_renderable->GetMaterial()->SetSampler(newLight->m_renderable->GetMaterial()->GetNumSamplers(), theRenderer->m_defaultSampler);
			
			Vector3 lightPosWorld = m_playerShip->m_spotLight->m_transform->GetWorldPosition();

			newLight->m_transform->SetLocalPosition(lightPosWorld);
			newLight->UpdateRenderableFromTransform();
			newLight->UpdateLightDataFromWorldTransform();

			//add light to scene
			m_renderScene->AddLight(newLight->m_light);
			m_renderScene->AddRenderable(newLight->m_renderable);			
					
		}
	}	

	theInput = nullptr;
	theRenderer = nullptr;
	return 0.f;
}

void Game::SpawnBullet(const Vector3& startingPosition, const Vector3& startingRotation)
{
	Renderer* theRenderer = Renderer::GetInstance();

	MeshBuilder meshBuilder;
	meshBuilder.CreateStarQuads3D( Vector3::ZERO, Vector3(1.f, 1.f, 3.f), Rgba::WHITE);

	//meshBuilder.CreateUVSphere( Vector3::ZERO, 1.f, 25, 25, Rgba::LIGHT_RED_TRANSPARENT);

	Bullet* bullet = new Bullet(startingPosition, startingRotation);
	bullet->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexPCU>());
	bullet->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("bullet"));
	bullet->m_renderScene = m_renderScene;

	Rgba bulletTint = Rgba::LIGHT_RED_TRANSPARENT;
	bullet->m_renderable->GetMaterial()->SetProperty("TINT", Rgba::ConvertToVector4(bulletTint));

	//add light to lists
	m_renderScene->AddRenderable(bullet->m_renderable);
	m_renderScene->AddLight(bullet->m_light);

	m_bullets.push_back(bullet);

	bullet = nullptr;
	theRenderer = nullptr;
}

void Game::SpawnAsteroid(const Vector3& startingPosition, AsteroidLevel level)
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder meshBuilder;

	Asteroid* asteroid = new Asteroid(level);
	asteroid->m_renderScene = m_renderScene;
	meshBuilder.CreateUVSphere( Vector3::ZERO, 1.f, 15, 15, Rgba::WHITE);
	asteroid->m_renderable->SetMesh(meshBuilder.CreateMesh<VertexLit>());	

	asteroid->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("asteroid"));

	asteroid->m_transform->SetLocalPosition(startingPosition);

	//add light to lists
	m_renderScene->AddRenderable(asteroid->m_renderable);

	m_asteroids.push_back(asteroid);

	asteroid = nullptr;

	theRenderer = nullptr;
}

void Game::SpawnCollisionEmitter(const Vector3& startingPosition, const Vector3& emissionDirection)
{
	Renderer* theRenderer = Renderer::GetInstance();

	//test particle emitter
	ParticleEmitter* emitter = new ParticleEmitter();
	emitter->m_transform->TranslatePosition(startingPosition);
	emitter->m_particleForce = emissionDirection * 10.f;
	emitter->m_particleTimeToLive = FloatRange(0.5f, 1.f);
	emitter->m_burst = IntRange(10, 20);
	emitter->m_isTemporary = true;
	emitter->m_renderable->SetMaterial(theRenderer->CreateOrGetMaterial("smoke"));
	
	Vector4 tint = Rgba::ConvertToVector4(Rgba::ORANGE);

	emitter->m_renderable->GetMaterial()->SetProperty("TINT", tint);
	emitter->SetCamera(m_gameCamera);
	//emitter->SetSpawnRatePerSecond(1.f);
	emitter->m_renderScene = m_renderScene;

	emitter->Burst();

	m_renderScene->AddRenderable(emitter->m_renderable);

	m_collisionEmitters.push_back(emitter);

	emitter = nullptr;
	theRenderer = nullptr;
}


