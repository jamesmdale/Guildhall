#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\Sampler.hpp"
#include "Engine\Renderer\Shader.hpp"
#include <vector>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Core\EngineCommon.hpp"

class Material
{
public:
	Material();
	~Material();
	Material(const Material* copyMaterial);
	Material(Shader* shader);

	void SetTexture(int index, Texture* texture);
	void SetSampler(int index, Sampler* sampler);
	void SetShader(Shader* shader);
	
	Texture* GetTexture(const int index);
	Sampler* GetSampler(const int index);
	Shader* GetShader();
	int GetNumTextures();
	int GetNumSamplers();
	int GetNumProperties();
	int GetShaderProgramHandle();

	int GetSortLayerOrder();
	RenderQueueType GetRenderQueueType();
	bool GetDoesUseLighting();

	//property setters
	void SetProperty(const std::string& name, float value);
	void SetProperty(const std::string& name, const Vector4& value);
	void SetProperty(const std::string& name, const Vector3& value);
	void SetProperty(const std::string& name, const Vector2& value);
	void SetProperty(const std::string& name, const Rgba& value);	
	void SetProperty(const std::string& name, const Matrix44& value);

	//m_properties helpers
	TODO("Could optimize this by doing a create or get property");
	MaterialProperty* GetExistingPropertyOfSameTypeOrClear(const std::string& name, const MaterialPropertyDataType& ); //long af name. Maybe find something better
	void RemoveProperty(const std::string& name);
	void LoadBoundShaderPropertyDefaults();

	TODO("Requires property blocks");
	//void SetPropertyBlock(int index UniformbBuffer* block); //requires uniform buffers

	//convenience methods that are common
	//void SetTint(const Rgba& tint);
	//void SetSpecular(float amount, float power);

	static Material* Clone(Material* materialToClone);

private:
	//m_property helpers
	void RemoveProperty(const int index);
	void AddShaderBindingToPropertyList(const Binding & binding);

public:
	Shader* m_shader = nullptr;
	std::vector<Texture*> m_textures;
	std::vector<Sampler*> m_samplers;
	std::vector<MaterialProperty*> m_properties;
	bool m_isInstance = false;

	//if you want to do instance data
	//bool m_isResource; //false if this is an isntance
};



