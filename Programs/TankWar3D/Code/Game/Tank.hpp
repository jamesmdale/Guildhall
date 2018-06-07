#pragma once
#include "Engine\Core\LightObject.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Engine\Camera\Camera.hpp"

class Tank : public GameObject
{
public:
	Tank();

	Tank(const std::string& name) : GameObject(name)
	{
		//tank init
	}

	Tank(const std::string& name, Camera* camera) : GameObject(name)
	{
		SetCamera(camera);
	}

	//overrides
	virtual ~Tank() override;

	virtual void Update(float deltaTime) override;
	virtual void PreRender() override;

	void SetCamera(Camera* camera);
	void UpdateFromInput(float timeDelta);

public:
	Camera* m_camera = nullptr;

};

