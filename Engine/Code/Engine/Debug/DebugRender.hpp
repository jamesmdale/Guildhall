#pragma once
#include <vector>
#include "Engine\Debug\DebugRenderObject.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include <string>
#include "Engine\Camera\Camera.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Math\Matrix44.hpp"

class DebugRender
{
public:
	DebugRender();

	static DebugRender* GetInstance();
	static DebugRender* CreateInstance();

public:
	void BeginFrame(){};
	void EndFrame();	

	void Startup();
	void Shutdown();

	void Update(float deltaSeconds);
	void Render();

	//utility functions
	void ToggleDebugRenderer();
	bool IsEnabled(){return m_isEnabled;};
	void ClearDebugRenderer(){m_debugObjects.clear();};
	Shader* GetDebugShader(){return m_debugShader;};
	Camera* GetDebugCamera(){return m_debugCamera;};

	//create functions
	void CreateDebugPoint(const Vector3& position = Vector3::ZERO,
		float scale = 1.f,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,
		Camera* camera = nullptr);

	void CreateDebugLine(const Vector3& positionStart,
		const Vector3& positionEnd,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,
		
		Camera* camera = nullptr);

	void CreateDebugBasis(const Vector3& positionStart,
		float scale = 1.f,
		float timeToLive = 0.f,		
		int depthType = 1,
		Camera* camera = nullptr);

	void CreateDebugBasis(const Matrix44& basis,
		const Vector3& position,
		float scale = 1.f,
		float timeToLive = 0.f,
		int depthType = 1,
		Camera* camera = nullptr);

	void CreateDebugSphere(const Vector3& center,
		float radius = 1.f,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,		
		Camera* camera = nullptr);

	void CreateDebugWireSphere(const Vector3& center,
		float radius = 1.f,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,		
		Camera* camera = nullptr);

	void CreateDebugWireCube(const Vector3& center,
		const Vector3& dimensions,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,		
		Camera* camera = nullptr);

	void CreateDebugQuad(const Vector3& center,
		const Vector2& dimensions,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,		
		Camera* camera = nullptr);

	void CreateDebugBillboardQuad(const Vector3 & center, const Vector3 & up, const Vector3 & right, const Vector2 & dimensions, const Rgba & startColor, const Rgba & endColor, float timeToLive, int depthType, Camera * camera);

	void CreateDebugCube(const Vector3& center,
		const Vector3& dimensions,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1,		
		Camera* camera = nullptr);


	//2d functions use debug camera and are in pixel space.

	void CreateDebugQuad2D(const Vector2& center,
		const Vector2& dimensions,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1);

	void CreateDebugLine2D(const Vector2& positionStart,
		const Vector2& positionEnd,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1
		);

	void CreateDebugCrosshair2D(const Vector2& center,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1
	);

	void CreateDebugText2D(const Vector2& center,
		float cellHeight,
		float aspectScale,
		std::string text,
		const Rgba& startColor = Rgba::WHITE,
		const Rgba& endColor = Rgba::WHITE,
		float timeToLive = 0.f,
		int depthType = 1
		);

private:
	std::vector<DebugRenderObject*> m_debugObjects;
	bool m_isEnabled = true;
	Camera* m_debugCamera = nullptr;
	Shader* m_debugShader = nullptr;
};

void ToggleDebug(Command &cmd);
void ClearDebug(Command &cmd);
