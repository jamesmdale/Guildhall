#include "Engine\Camera\Skybox.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Renderer\Renderer.hpp"


Skybox::Skybox(const char* image)
{
	m_textureCube = new TextureCube();
	bool success = m_textureCube->MakeFromImage(image);

	GUARANTEE_OR_DIE(success, "SKYBOX MAKE FROM IMAGE FAILED : CHECK IMAGE PATH");

	MeshBuilder mb;
	mb.CreateCube(Vector3::ZERO, Vector3::ONE, Rgba::WHITE);
	m_mesh = mb.CreateMesh<VertexPCU>();

	Renderer* theRenderer = Renderer::GetInstance();
	m_shader = theRenderer->CreateOrGetShader("skybox");
	
	m_model = Matrix44::IDENTITY;

	theRenderer = nullptr;
}

Skybox::~Skybox()
{
	delete(m_shader);
	m_shader = nullptr;

	delete(m_mesh);
	m_mesh = nullptr;

	delete(m_textureCube);
	m_textureCube = nullptr;
	
}
