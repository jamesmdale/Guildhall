#pragma once
#include "Engine\Core\LightObject.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Game\Menu\PlayingState.hpp"
#include "Game\Turret.hpp"
#include "Engine\Renderer\Renderable.hpp"

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

	void Initialize();

	//update methods
	virtual void Update(float deltaTime) override;
	void UpdateFromInput(float deltaSeconds);
	Vector3 UpdateTarget(float deltaSeconds);
	void UpdateTrajectoryRenderable(const Vector3& target);

	//misc methods
	RayCastHit3 RaycastFromCamera(float deltaSeconds);

	void SetCamera(Camera* camera);

	//convenience methods
	inline bool IsAlive(){return m_heatlth > 0 ? true : false;}

public:
	// transforms
	Transform* m_cameraPivotTransform  = nullptr;
	Transform* m_tankBodyTransform  = nullptr;

	Turret* m_turret = nullptr;

	Camera* m_camera = nullptr;
	Stopwatch* m_breadCrumbTimer = nullptr;
	PlayingState* m_gameState = nullptr;
	
	Vector3 m_baseDimensions;
	Vector3 m_currentTarget;

	Renderable* m_trajectory = nullptr;

	int m_heatlth = 100;
};

