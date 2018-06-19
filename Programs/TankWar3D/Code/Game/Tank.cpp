#include "Game\Tank.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Game\Game.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Debug\DebugRender.cpp"
#include "Engine\Window\Window.hpp"
Tank::Tank()
{
	// create transforms =============================================================================
	m_cameraPivotTransform  = new Transform();
	m_tankBodyTransform  = new Transform();

	//add children to base transform
	m_transform->AddChildTransform(m_cameraPivotTransform);
	m_transform->AddChildTransform(m_tankBodyTransform);
}

Tank::~Tank()
{
	delete(m_cameraPivotTransform);
	m_cameraPivotTransform = nullptr;

	delete(m_tankBodyTransform);
	m_tankBodyTransform = nullptr;	

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
	
	Vector3 basePosition = m_transform->GetWorldPosition();

	//get height and normal from terrain
	float heightFromTerrain = m_playingState->m_terrain->GetHeightAtPositionXZ(Vector2(basePosition.x, basePosition.z)) + m_baseDimensions.y;	
	Vector3 terrainNormal = m_playingState->m_terrain->GetNormalAtPositionXZ(Vector2(basePosition.x, basePosition.z));

	//calculate new basis from normal (new up)
	Vector3 newRight = CrossProduct(terrainNormal, m_tankBodyTransform->GetWorldForward());
	Vector3 newForward = CrossProduct(newRight, terrainNormal);

	//create a matrix using the new basis that we can extract the rotation from
	Matrix44 newMatrix;
	newMatrix.SetIBasis(Vector4(newRight.GetNormalized(), 0.f));
	newMatrix.SetJBasis(Vector4(terrainNormal.GetNormalized(), 0.f));
	newMatrix.SetKBasis(Vector4(newForward.GetNormalized(), 0.f));
	Vector3 rotation = newMatrix.GetRotation();

	//update the tank transform's position
	m_transform->SetLocalPosition(Vector3(basePosition.x, heightFromTerrain, basePosition.z));
	m_transform->SetLocalRotation(Vector3(0.f, rotation.y, 0.f));

	//update the tank body's rotation
	m_tankBodyTransform->SetLocalRotation(Vector3(rotation.x, 0.f, rotation.z));

	//debug
	DebugRender::GetInstance()->CreateDebugBasis(m_transform->GetWorldMatrix(), Vector3(basePosition.x, basePosition.y + 0.5f, basePosition.z), 1.f, 0.f, 1.f, m_playingState->m_camera);

	DebugRender::GetInstance()->CreateDebugCrosshair2D(Window::GetInstance()->GetCenterOfClientWindow(), Rgba::GREEN, Rgba::GREEN, 0.0f, 1);
}

void Tank::SetCamera(Camera* camera)
{
	m_camera = camera;

	m_cameraPivotTransform->AddChildTransform(m_camera->m_transform);
}

void Tank::UpdateFromInput(float timeDelta)
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();

	// update mouse input =============================================================================
	Vector2 mouseDelta = Vector2::ZERO;		

	mouseDelta = InputSystem::GetInstance()->GetMouse()->GetMouseDelta();				

	// calculate rotation for camera and use same rotation for tank ============================================================================

	m_cameraPivotTransform->AddRotation(Vector3(mouseDelta.y, mouseDelta.x, 0.f) * (timeDelta * 10.f));
	//m_cameraPivotTransform->AddRotation(Vector3(, 0.f, 0.f) * (timeDelta * 10.f));	

	TODO("Restrict rotation stuff");
	/*float clampedRotationX = ClampFloat(m_cameraPivotTransform->GetLocalRotationAroundX(), -90.f, 90.f);
	float clampedRotationY = Modulus(m_transform->GetLocalRotationAroundY(), 360.f);

	m_transform->SetLocalRotation(Vector3(m_transform->GetLocalRotationAroundX(), clampedRotationY, m_transform->GetLocalRotationAroundZ()));
	m_cameraPivotTransform->SetLocalRotation(Vector3(clampedRotationX, m_transform->GetLocalRotationAroundY(), m_transform->GetLocalRotationAroundZ()));*/

	// update movement =============================================================================
	Vector3 positionToAdd = Vector3::ZERO;
	Vector3 positionAtStartOfFrame = positionToAdd;

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
		if(theInput->IsKeyPressed(theInput->KEYBOARD_W) || theInput->IsKeyPressed(theInput->KEYBOARD_S))
		{	
			float multiplyValue = 10.f;

			if (theInput->IsKeyPressed(theInput->KEYBOARD_SHIFT))
			{
				multiplyValue *= 3.f;
			}

			Vector3 rotation = Vector3(0.f, -10.f, 0.f) * timeDelta * multiplyValue;
			m_transform->AddRotation(rotation);
		}
	}

	if(theInput->IsKeyPressed(theInput->KEYBOARD_D))
	{
		if(theInput->IsKeyPressed(theInput->KEYBOARD_W) || theInput->IsKeyPressed(theInput->KEYBOARD_S))
		{	
			float multiplyValue = 10.f;

			if (theInput->IsKeyPressed(theInput->KEYBOARD_SHIFT))
			{
				multiplyValue *= 3.f;
			}

			Vector3 rotation = Vector3(0.f, 10.f, 0.f) * timeDelta * multiplyValue;
			m_transform->AddRotation(rotation);
		}

		
	}

	theRenderer = nullptr;
	theInput = nullptr;
}
