#pragma once
#include "Game\LightObject.hpp"
#include "Engine\Core\GameObject.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\ParticleSystem\ParticleEmitter.hpp"

class Ship : public GameObject
{
public:

/************************************************************************/
/*	Constructors                                                        */
/************************************************************************/
	Ship();

	Ship(const std::string& name) : GameObject(name)
	{
			
	}

	Ship(const std::string& name, LightObject* spotLight, Camera* camera, ParticleEmitter* emitter) : GameObject(name)
	{
		SetSpotLight(spotLight);
		SetCamera(camera);
		SetEmitter(emitter);
	}

/************************************************************************/
/*	Destructors		                                                    */
/************************************************************************/

	virtual ~Ship() override;

/************************************************************************/
/*	Methods		                                                        */
/************************************************************************/	

	void SetCamera(Camera* camera);
	void SetSpotLight(LightObject* light);
	void SetEmitter(ParticleEmitter* emitter);
	void UpdateFromInput(float timeDelta);

	virtual void Update(float deltaTime) override;
	virtual void PreRender() override;

public:
	LightObject* m_spotLight = nullptr;
	Camera* m_camera = nullptr;
	ParticleEmitter* m_thrusterEmitter = nullptr;

private:
	float m_fireCooldown = 0.0f;
	bool m_isFiring = false;
};

