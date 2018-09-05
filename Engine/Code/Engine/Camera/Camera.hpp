#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\math\Matrix44.hpp"
#include "Engine\Renderer\FrameBuffer.hpp"
#include "Engine\Core\Transform.hpp"
#include "Engine\Camera\Skybox.hpp"

class Camera
{
public:
	Camera();
	~Camera();

	//will add later
	void SetColorTarget(Texture* colorTarget);
	void SetDepthStencilTarget(Texture* depthTarget);
	
	//sets view
	void LookAt(Vector3 position, Vector3 target, Vector3 up = Vector3::UP);	

	//projection settings
	void SetProjection(Matrix44 projection);
	void SetProjectionOrtho(float size, float aspect, float orthoNear, float orthoFar);
	void SetOrtho(const float leftVal, const float rightVal, const float bottomVal, const float topVal, const float nearVal, const float farVal);
	void SetPerspective(float fovDegrees, float aspect, float nearVal, float farVal);

	void Translate(Vector3 position);
	void SetPosition(Vector3 position);
	void RotateByEuler(Vector3 rotation);

	//view settings
	void SetView(Matrix44 view);

	Matrix44 GetView();
	Matrix44 GetViewProjection();

public:
	//default all to identity
	Matrix44 m_viewMatrix;	  //inverse of camera (used for shader)
	Matrix44 m_projMatrix;	 //projection
	
	//camera transform
	Transform* m_transform = nullptr;

	//skybox
	Skybox* m_skybox = nullptr;

	Vector2 m_viewPortDimensions = Vector2::ZERO;

	FrameBuffer m_frameBufferOutput;
	
};

