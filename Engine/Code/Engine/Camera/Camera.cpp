#include "Engine\Camera\Camera.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"

//https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710(v=vs.85).aspx  - View Matrix Reference

Camera::Camera()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	m_viewPortDimensions = theWindow->GetClientDimensions(); //default dimensions to window size

// 	m_frameBufferOutput.m_depthStencilTarget = theRenderer->CreateDepthStencilTarget(m_viewPortDimensions.x, m_viewPortDimensions.y);
// 	m_frameBufferOutput.m_colorTarget = theRenderer->CreateRenderTarget(m_viewPortDimensions.x, m_viewPortDimensions.y);

	m_transform = new Transform();

	theRenderer = nullptr;
	theWindow = nullptr;
}

Camera::~Camera()
{
	delete(m_transform);
	m_transform = nullptr;

	delete(m_skybox);
	m_skybox = nullptr;
}

//will add later
void Camera::SetColorTarget(Texture* colorTarget)
{
	m_frameBufferOutput.m_colorTarget = colorTarget;
}

void Camera::SetDepthStencilTarget(Texture* depthTarget)
{
	m_frameBufferOutput.m_depthStencilTarget = depthTarget;
}

//model setters
void Camera::LookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Vector3 zAxis = target - position;
	zAxis.Normalize();

	Vector3 xAxis = CrossProduct(up, zAxis);
	xAxis.Normalize();

	Vector3 yAxis = CrossProduct(zAxis, xAxis);
	yAxis.Normalize(); //should already be normalized.

	//used for perspective camera in the future
	float xAxisDot = (DotProduct(xAxis, position));
	float yAxisDot = (DotProduct(yAxis, position));
	float zAxisDot = (DotProduct(zAxis, position));
	
	Vector4 positionDisplacement = Vector4(xAxisDot, yAxisDot, zAxisDot, 1.f);

	m_transform->m_transformMatrix = Matrix44(Vector4(xAxis, 0.f), Vector4(yAxis, 0.f), Vector4(zAxis, 0.f), positionDisplacement);
	
	m_viewMatrix = m_transform->m_transformMatrix.InvertFastToNew();
	//inverse of m_cameraMatrix = view
}

//this adds to current rotation
void Camera::RotateByEuler(Vector3 rotation) 
{
	m_transform->AddRotation(rotation);
	if(m_transform->IsTransformDirty())
	{
		m_viewMatrix = m_transform->GetWorldMatrix().InvertFastToNew();
	}
}

//this adds to current position
void Camera::Translate(Vector3 position) 
{
	m_transform->TranslatePosition(position);
	if(m_transform->IsTransformDirty())
	{
		m_viewMatrix = m_transform->GetWorldMatrix().InvertFastToNew();
	}	
}

//sets current position
void Camera::SetPosition(Vector3 position)
{
	m_transform->SetLocalPosition(position);
	if(m_transform->IsTransformDirty())
	{
		m_viewMatrix = m_transform->GetWorldMatrix().InvertFastToNew();
	}	
}

void Camera::SetView(Matrix44 view)
{
	m_viewMatrix = view;
}

//projection settings
void Camera::SetProjection(Matrix44 projection)
{
	m_projMatrix = projection;
}

void Camera::SetProjectionOrtho(float size, float aspectRatio, float orthoNear, float orthoFar)
{
	float orthoHeight = size;
	float orthoWidth = size * aspectRatio;

	SetOrtho(-orthoWidth/2.f, orthoWidth/2, -orthoHeight/2.f, orthoHeight/2.f, orthoNear, orthoFar);
}

void Camera::SetOrtho(const float leftVal, const float rightVal, const float bottomVal, const float topVal, const float nearVal, const float farVal)
{
	// Establish a 3D (orthographic) drawing coordinate system: (0,0) bottom-left to (1000,1000) top-right - UPDATED: 8/24 JMD

	Matrix44 orthoProjection = Matrix44();

	orthoProjection.Ix = 2.0f/(rightVal - leftVal);
	orthoProjection.Jy = 2.0f/(topVal - bottomVal);
	orthoProjection.Kz = 2.0f/(farVal - nearVal);

	orthoProjection.Tx = -1.f * (rightVal + leftVal)/(rightVal - leftVal);
	orthoProjection.Ty = -1.f * (topVal + bottomVal)/(topVal - bottomVal);
	orthoProjection.Tz = -1.f * (nearVal + farVal)/(nearVal - farVal);

	SetProjection(orthoProjection);
}

void Camera::SetPerspective(float fovDegrees, float aspect, float nearVal, float farVal)
{
	Matrix44 perspectiveProjection = Matrix44();

	perspectiveProjection.PerspectiveProjection(fovDegrees, aspect, nearVal, farVal);
	
	SetProjection(perspectiveProjection);
}

Matrix44 Camera::GetView()
{
	m_viewMatrix = m_transform->GetWorldMatrix();
	m_viewMatrix.Invert();
	return m_viewMatrix;
}

Matrix44 Camera::GetViewProjection()
{
	Matrix44 viewProjection = m_projMatrix;
	viewProjection.Append(m_viewMatrix);

	return viewProjection;
}
