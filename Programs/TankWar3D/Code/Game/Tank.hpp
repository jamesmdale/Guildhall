#pragma once
#include "Engine\Core\LightObject.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Game\Menu\PlayingState.hpp"

class Tank : public GameObject
{
public:
	Tank();

	Tank(const std::string& name) : GameObject(name)
	{
		// create transforms
		m_cameraPivotTransform  = new Transform();
		m_tankBodyTransform  = new Transform();

		//add children to base transform
		m_transform->AddChildTransform(m_cameraPivotTransform);
		m_transform->AddChildTransform(m_tankBodyTransform);
	}

	Tank(const std::string& name, Camera* camera) : GameObject(name)
	{
		// create transforms
		m_cameraPivotTransform  = new Transform();
		m_tankBodyTransform  = new Transform();

		//add children to base transform
		m_transform->AddChildTransform(m_cameraPivotTransform);
		m_transform->AddChildTransform(m_tankBodyTransform);

		SetCamera(camera);
	}

	//overrides
	virtual ~Tank() override;

	virtual void Update(float deltaTime) override;

	void SetCamera(Camera* camera);
	void UpdateFromInput(float timeDelta);

public:
	// transforms
	Transform* m_cameraPivotTransform  = nullptr;
	Transform* m_tankBodyTransform  = nullptr;

	Camera* m_camera = nullptr;
	Stopwatch* m_breadCrumbTimer = nullptr;
	PlayingState* m_playingState = nullptr;
	
	Vector3 m_baseDimensions;
	Vector3 m_currentTarget;
};

