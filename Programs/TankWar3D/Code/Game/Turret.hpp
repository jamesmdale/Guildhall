#pragma once
#include "Engine\Core\GameObject.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Menu\PlayingState.hpp"

class Turret : public GameObject
{
public:
	Turret();

	Turret(const std::string& name) : GameObject(name)
	{
		// create transforms
		m_gunTransform = new Transform();
		m_muzzleTransform = new Transform();

		//add children to base transform
		m_transform->AddChildTransform(m_gunTransform);
		m_gunTransform->AddChildTransform(m_muzzleTransform);
	}

	Turret(const std::string& name, Camera* camera) : GameObject(name)
	{
		// create transforms
		m_gunTransform = new Transform();
		m_muzzleTransform = new Transform();

		//add children to base transform
		m_transform->AddChildTransform(m_gunTransform);
		m_transform->AddChildTransform(m_muzzleTransform);
	}

	//overrides
	virtual ~Turret() override;

public:
	// transforms
	Transform* m_gunTransform = nullptr;
	Transform* m_muzzleTransform = nullptr;
};

