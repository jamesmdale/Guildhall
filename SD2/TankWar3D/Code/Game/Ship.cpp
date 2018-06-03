#include "Game\Ship.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Core\StringUtils.hpp"

const float timeToFireCooldown = 0.5f;

Ship::Ship()
{
	
}

Ship::~Ship()
{
	delete(m_spotLight);
	m_spotLight = nullptr;

	m_camera = nullptr;
}

void Ship::Update(float timeDelta)
{
	if(m_fireCooldown > 0.f)
	{
		m_fireCooldown -= timeDelta;
		m_fireCooldown = ClampFloat(m_fireCooldown, 0.f, timeToFireCooldown);
	}

	m_thrusterEmitter->Update(timeDelta);

	return;
}

void Ship::PreRender()
{
	UpdateRenderableFromTransform();
	m_spotLight->PreRender();
	m_thrusterEmitter->PreRender();
}


void Ship::SetCamera(Camera* camera)
{
	if(camera != nullptr)
	{
		if(m_camera != nullptr)
		{
			m_transform->RemoveChildTransform(m_camera->m_transform);	
			m_camera = nullptr;
		}

		m_camera = camera;
		m_transform->AddChildTransform(m_camera->m_transform);
	}
}

void Ship::SetSpotLight(LightObject* light)
{
	if(light != nullptr)
	{
		if(m_spotLight != nullptr)
		{
			m_transform->RemoveChildTransform(m_spotLight->m_transform);	
			m_spotLight = nullptr;
		}

		m_spotLight = light;
		m_transform->AddChildTransform(m_spotLight->m_transform);
	}
}

void Ship::SetEmitter(ParticleEmitter* emitter)
{
	if(emitter != nullptr)
	{
		if(m_thrusterEmitter != nullptr)
		{
			m_transform->RemoveChildTransform(m_thrusterEmitter->m_transform);	
			m_camera = nullptr;
		}

		m_thrusterEmitter = emitter;
		m_transform->AddChildTransform(m_thrusterEmitter->m_transform);
	}
}

void Ship::UpdateFromInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();

	/************************************************************************/
	/* Update Mouse Input - Camera, Ship, and Spotlight Rotation and Position */
	/************************************************************************/
	Vector2 mouseDelta = Vector2::ZERO;		

	mouseDelta = InputSystem::GetInstance()->GetMouse()->GetMouseDelta();				

	//calculate rotation for camera and use same rotation for ship and light
	m_transform->AddRotation(Vector3(mouseDelta.y, mouseDelta.x, 0.f) * timeDelta * 10.f);

	float clampedX = ClampFloat(m_transform->GetLocalRotationAroundX(), -90.f, 90.f);
	float clampedY = Modulus(m_transform->GetLocalRotationAroundY(), 360.f);

	Vector3 rotation = Vector3(clampedX, clampedY, 0.f);

	m_transform->SetLocalRotation(Vector3(rotation));

	Vector3 positionToAdd = Vector3::ZERO;
	Vector3 positionAtStartOfFrame = positionToAdd;
	//Vector3 positionAtStartOfFrame = m_transform->GetWorldPosition();

	//update movement
	if(theInput->IsKeyPressed(theInput->KEYBOARD_W))
	{
		//calculate movement for camera and use same movement for ship and light
		positionToAdd = m_transform->GetWorldForward() * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_S))
	{	
		positionToAdd = (-1.f * m_transform->GetWorldForward()) * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_A))
	{
		positionToAdd = (-1.f * m_transform->GetWorldRight()) * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_D))
	{
		positionToAdd = m_transform->GetWorldRight() * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_SHIFT))
	{
		positionToAdd = m_transform->GetWorldUp() * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_CONTROL))
	{
		positionToAdd = (-1.f * m_transform->GetWorldUp()) * timeDelta * 10.f;
		m_transform->TranslatePosition(positionToAdd);
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_SPACE))
	{
		if(m_fireCooldown <= 0.0f)
		{
			Matrix44 worldFrontOfShip = m_spotLight->m_transform->GetWorldMatrix();

			g_theGame->SpawnBullet(worldFrontOfShip.GetPosition(), m_transform->GetLocalRotation());
			m_fireCooldown = timeToFireCooldown;
		}
	}

	if(positionToAdd != Vector3::ZERO)
	{
		//m_thrusterEmitter->SpawnParticles(1);
		m_thrusterEmitter->m_particleForce = (positionToAdd - positionAtStartOfFrame).GetNormalized();
	}
	else
	{
		m_thrusterEmitter->m_spawnRatePerSecond = 0;
	}

	theRenderer = nullptr;
	theInput = nullptr;
}
