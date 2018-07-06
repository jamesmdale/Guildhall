#include "Engine\Renderer\Material.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\ShaderProgram.hpp"
#include "Engine\Core\StringUtils.hpp"

Material::Material()
{

}

Material::~Material()
{
	TODO("Delete instanced shaders later");
	//delete(m_shader);
	//m_shader = nullptr;
}

Material::Material(const Material* copyMaterial)
{
	m_isInstance = copyMaterial->m_isInstance;
	m_shader = copyMaterial->m_shader;
	
	for (int samplerIndex = 0; samplerIndex < (int)copyMaterial->m_samplers.size(); ++samplerIndex)
	{
		m_samplers.push_back(copyMaterial->m_samplers[samplerIndex]); //copy sampler exaclty as is (all it contains is handle)
	}

	for (int textureIndex = 0; textureIndex < (int)copyMaterial->m_textures.size(); ++textureIndex)
	{
		m_textures.push_back(copyMaterial->m_textures[textureIndex]); //copy textures exactly as is. (for now)
	}

	for (int propIndex = 0; propIndex < (int)copyMaterial->m_properties.size(); ++propIndex)
	{
		switch (copyMaterial->m_properties[propIndex]->GetDataType())
		{
		case FLOAT_MATERIAL_PROPERTY:
			m_properties.push_back(new MaterialPropertyFloat(copyMaterial->m_properties[propIndex]->m_name, *(float*)copyMaterial->m_properties[propIndex]->GetData()));
			break;
		case MATRIX44_MATERIAL_PROPERTY:
			m_properties.push_back(new MaterialPropertyMatrix44(copyMaterial->m_properties[propIndex]->m_name, *(Matrix44*)copyMaterial->m_properties[propIndex]->GetData()));
			break;
		case VECTOR2_MATERIAL_PROPERTY:
			m_properties.push_back(new MaterialPropertyVector2(copyMaterial->m_properties[propIndex]->m_name, *(Vector2*)copyMaterial->m_properties[propIndex]->GetData()));
			break;
		case VECTOR3_MATERIAL_PROPERTY:
			m_properties.push_back(new MaterialPropertyVector3(copyMaterial->m_properties[propIndex]->m_name, *(Vector3*)copyMaterial->m_properties[propIndex]->GetData()));
			break;
		case VECTOR4_MATERIAL_PROPERTY:
			m_properties.push_back(new MaterialPropertyVector4(copyMaterial->m_properties[propIndex]->m_name, *(Vector4*)copyMaterial->m_properties[propIndex]->GetData()));
			break;
		}
	}
}

Material::Material(Shader* shader)
{
	m_shader = shader;

	for (Binding binding : m_shader->m_bindings)
	{
		AddShaderBindingToPropertyList(binding);
	}
}

void Material::LoadBoundShaderPropertyDefaults()
{
	for (Binding binding : m_shader->m_bindings)
	{
		AddShaderBindingToPropertyList(binding);
	}
}

void Material::SetTexture(int index, Texture* texture)
{
	TODO("Index might be bad so we have to handle that condition. Sure this is good?");
	if(index < (int)m_textures.size())
	{
		m_textures[index] = texture;
		return;
	}	

	m_textures.push_back(texture);
}

void Material::SetSampler(int index, Sampler* sampler)
{
	TODO("Index might be bad so we have to handle that condition. Sure this is good?");
	if(index < (int)m_samplers.size())
	{
		m_samplers[index] = sampler;
		return;
	}

	m_samplers.push_back(sampler); 
}

void Material::SetShader(Shader* shader)
{
	m_shader = shader;
}


//-----------------------------------------------------------------------------------------------------------------------------
//Getters

Texture * Material::GetTexture(const int index)
{
	return m_textures[index];
}

Sampler * Material::GetSampler(const int index)
{
	return m_samplers[index];
}

Shader* Material::GetShader()
{
	return m_shader;
}

int Material::GetNumTextures()
{
	return (int)m_textures.size();
}

int Material::GetNumSamplers()
{
	return (int)m_samplers.size();
}

int Material::GetNumProperties()
{
	return (int)m_properties.size();
}

int Material::GetShaderProgramHandle()
{
	int handleId = -1;
	if(m_shader != nullptr)
	{
		handleId = m_shader->GetProgram()->GetHandle();
	}	

	return handleId;
}

int Material::GetSortLayerOrder()
{
	return m_shader->GetSortingLayer();
}

RenderQueueType Material::GetRenderQueueType()
{
	return m_shader->GetRenderQueueType();
}

bool Material::GetDoesUseLighting()
{
	return m_shader->m_doesUseLighting;
}

//-----------------------------------------------------------------------------------------------------------------------------
//Property setters

void Material::SetProperty(const std::string& name, float value)
{
	MaterialPropertyFloat* prop = (MaterialPropertyFloat*)GetExistingPropertyOfSameTypeOrClear(name, FLOAT_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyFloat(name, value);
		m_properties.push_back(prop);
		return;
	}
	
	prop->SetData((void*)&value);	
}


void Material::SetProperty(const std::string & name, const Vector4& value)
{
	MaterialPropertyVector4* prop = (MaterialPropertyVector4*)GetExistingPropertyOfSameTypeOrClear(name, VECTOR4_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyVector4(name, value);
		m_properties.push_back(prop);
		return;
	}
	
	prop->SetData((void*)&value);	
}

void Material::SetProperty(const std::string & name, const Vector3& value)
{
	MaterialPropertyVector3* prop = (MaterialPropertyVector3*)GetExistingPropertyOfSameTypeOrClear(name, VECTOR3_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyVector3(name, value);
		m_properties.push_back(prop);
		return;
	}

	prop->SetData((void*)&value);	
}

void Material::SetProperty(const std::string & name, const Vector2& value)
{
	MaterialPropertyVector2* prop = (MaterialPropertyVector2*)GetExistingPropertyOfSameTypeOrClear(name, VECTOR2_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyVector2(name, value);
		m_properties.push_back(prop);
		return;
	}

	prop->SetData((void*)&value);		
}

void Material::SetProperty(const std::string & name, const Rgba& value)
{
	MaterialPropertyRGBA* prop = (MaterialPropertyRGBA*)GetExistingPropertyOfSameTypeOrClear(name, RGBA_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyRGBA(name, value);
		m_properties.push_back(prop);
		return;
	}

	prop->SetData((void*)&value);	
}

void Material::SetProperty(const std::string& name, const Matrix44& value)
{
	MaterialPropertyMatrix44* prop = (MaterialPropertyMatrix44*)GetExistingPropertyOfSameTypeOrClear(name, MATRIX44_MATERIAL_PROPERTY);

	if(prop == nullptr)
	{
		prop = new MaterialPropertyMatrix44(name, value);
		m_properties.push_back(prop);
		return;
	}

	prop->SetData((void*)&value);	
}

//-----------------------------------------------------------------------------------------------------------------------------

//m_properties helpers
MaterialProperty* Material::GetExistingPropertyOfSameTypeOrClear(const std::string& name, const MaterialPropertyDataType& type)
{
	MaterialProperty* prop = nullptr;
	for(int propIndex = 0; propIndex < (int)m_properties.size(); propIndex++)
	{
		if(m_properties[propIndex]->m_name == name)
		{
			if(m_properties[propIndex]->GetDataType() == type)
			{
				return m_properties[propIndex];
			}
			else
			{
				//if we find the property but it's a different type, return nullptr so we know to make a new one
				RemoveProperty(propIndex);
				return prop;
			}
		}
	}

	//property not found. Return nullptr so we know to make a new one.
	return prop;
}

void Material::RemoveProperty(const std::string & name)
{
	for(int propIndex = 0; propIndex < (int)m_properties.size(); propIndex++)
	{
		if(m_properties[propIndex]->m_name == name)
		{
			m_properties.erase(m_properties.begin() + propIndex);
			return;
		}
	}
}

void Material::RemoveProperty(const int index)
{
	m_properties.erase(m_properties.begin() + index);
}

void Material::AddShaderBindingToPropertyList(const Binding& binding)
{
	TODO("Add more bindings as necessary");

	if(binding.m_dataType == "int")
	{
		int val = ConvertStringToInt(binding.m_value);
		SetProperty(binding.m_bindingName, val);
		return;
	}

	if(binding.m_dataType == "float")
	{
		float val = ConvertStringToFloat(binding.m_value);
		SetProperty(binding.m_bindingName, val);
		return;
	}

	if(binding.m_dataType == "vector2")
	{
		Vector2 val;
		val.SetFromText(binding.m_value.c_str());
		SetProperty(binding.m_bindingName, val);
		return;
	}

	if(binding.m_dataType == "vector3")
	{
		Vector3 val;
		val.SetFromText(binding.m_value.c_str());
		SetProperty(binding.m_bindingName, val);
		return;
	}

	if(binding.m_dataType == "vector4")
	{
		Vector4 val;
		val.SetFromText(binding.m_value.c_str());
		SetProperty(binding.m_bindingName, val);
		return;
	}

	if(binding.m_dataType == "rgba")
	{
		Rgba val;
		val.SetFromText(binding.m_value.c_str());
		SetProperty(binding.m_bindingName, val);
		return;
	}

	TODO("Add materix binding as necessary");
	/*if(binding.m_dataType == "matrix44")
	{
		Matrix44 val;
		val.SetFromText(binding.m_value.c_str());
		MaterialPropertyMatrix44* prop = new MaterialPropertyMatrix44(binding.m_bindingName, val);
		m_properties.push_back(prop);
	}*/
}

Material* Material::Clone(Material* materialToClone)
{
	if(materialToClone == nullptr)
	{
		return nullptr;
	}

	Material* clonedMaterial = new Material(materialToClone);

	clonedMaterial->m_isInstance = true;
	return clonedMaterial;
}
