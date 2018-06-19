#include "Game\Turret.hpp"



Turret::Turret()
{
	// create transforms
	m_gunTransform = new Transform();
	m_muzzleTransform = new Transform();

	//add children to base transform
	m_transform->AddChildTransform(m_gunTransform);
	m_gunTransform->AddChildTransform(m_muzzleTransform);
}


Turret::~Turret()
{
	delete(m_gunTransform);
	m_gunTransform = nullptr;

	delete(m_muzzleTransform);
	m_muzzleTransform = nullptr;
}
