#include "Game\Swarmer.hpp"
#include "Game\Spawner.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\GameCommon.hpp"


Swarmer::Swarmer()
{
	m_health = g_swarmerStartingHealth;
}


Swarmer::~Swarmer()
{
	m_parentSpawner->RemoveDeadSwarmer(this);
	m_parentSpawner = nullptr;

	delete(m_eyeTransform);
	m_eyeTransform = nullptr;
}

void Swarmer::Update(float deltaSeconds)
{
}

void Swarmer::Initialize()
{
	//update transforms
	m_eyeTransform = new Transform();
	
	MeshBuilder meshBuilder;

	//add main body renderable
	Renderable* bodyRenderable = new Renderable();
	meshBuilder.CreateUVSphere(Vector3::ZERO, 1.f, 20, 20, Rgba::WHITE);
	bodyRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	bodyRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("lit")));
	bodyRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienSkin.jpg"));
	AddRenderable(bodyRenderable);

	m_transform->AddChildTransform(bodyRenderable->m_transform);

	//add eye renderable
	Renderable* eyeRenderable = new Renderable();
	meshBuilder.CreateUVSphere(Vector3::ZERO, 0.25f, 20, 20, Rgba::WHITE);
	eyeRenderable->AddMesh(meshBuilder.CreateMesh<VertexLit>());
	eyeRenderable->SetMaterial(Material::Clone(Renderer::GetInstance()->CreateOrGetMaterial("lit")));
	eyeRenderable->m_material->SetTexture(0, Renderer::GetInstance()->CreateOrGetTexture("Data/Images/alienEye.jpg"));

	m_eyeTransform->SetLocalPosition(Vector3(0.f, 0.5f, 0.75f));
	m_eyeTransform->AddRotation(Vector3(0.f, 90.f, 0.f));

	m_eyeTransform->AddChildTransform(eyeRenderable->m_transform);

	AddRenderable(eyeRenderable);

	m_transform->AddChildTransform(m_eyeTransform);

	//add renderables to scene
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	//cleanup
	eyeRenderable = nullptr;
	bodyRenderable = nullptr;
}
