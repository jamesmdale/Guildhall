#include "Game\Tank.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Core\StringUtils.hpp"

Tank::Tank()
{

}

Tank::~Tank()
{
	m_camera = nullptr;
	m_playingState = nullptr;
}

void Tank::Update(float timeDelta)
{
	bool doesAddNewBreadCrumb = m_breadCrumbTimer->Decrement();
	if (doesAddNewBreadCrumb)
	{
		m_breadCrumbTimer->Reset();
		DebugRender::GetInstance()->CreateDebugPoint(m_transform->GetWorldPosition() + m_transform->GetWorldForward(), .25f, Rgba::GREEN, Rgba::RED, 4.f, LESS_DEPTH_TYPE, m_camera);
	}
	
	Vector3 currentPosition = m_transform->GetWorldPosition();
	float heightFromTerrain = m_playingState->m_terrain->GetHeightAtPositionXZ(Vector2(currentPosition.x, currentPosition.z));

	/*m_transform->TranslatePosition(Vector3(currentPosition.x, heightFromTerrain, currentPosition.z));*/
	m_transform->SetLocalPosition(Vector3(currentPosition.x, heightFromTerrain, currentPosition.z));
}

void Tank::PreRender()
{
	UpdateRenderableFromTransform();
}


void Tank::SetCamera(Camera* camera)
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

void Tank::UpdateFromInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();

	//upate from mouse
	Vector2 mouseDelta = Vector2::ZERO;		

	mouseDelta = InputSystem::GetInstance()->GetMouse()->GetMouseDelta();				

	//calculate rotation for camera and use same rotation for tank
	m_transform->AddRotation(Vector3(0.f, mouseDelta.x, 0.f) * timeDelta * 10.f);

	float clampedX = ClampFloat(m_transform->GetLocalRotationAroundX(), -90.f, 90.f);
	float clampedY = Modulus(m_transform->GetLocalRotationAroundY(), 360.f);

	Vector3 rotation = Vector3(clampedX, clampedY, 0.f);

	m_transform->SetLocalRotation(Vector3(rotation));

	Vector3 positionToAdd = Vector3::ZERO;
	Vector3 positionAtStartOfFrame = positionToAdd;

	//update movement
	if(theInput->IsKeyPressed(theInput->KEYBOARD_W))
	{
		//calculate movement for camera and use same movement for Tank and light
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

	theRenderer = nullptr;
	theInput = nullptr;
}
