#pragma once
#include "Engine\Renderer\GlFunctions.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"

enum MaterialPropertyDataType
{
	UNDEFINED_MATERIAL_PROPERTY,
	FLOAT_MATERIAL_PROPERTY,
	VECTOR2_MATERIAL_PROPERTY,
	VECTOR3_MATERIAL_PROPERTY,
	VECTOR4_MATERIAL_PROPERTY,
	RGBA_MATERIAL_PROPERTY,
	MATRIX44_MATERIAL_PROPERTY
};

class MaterialProperty
{
public:
	MaterialProperty(std::string propertyName)
	{
		m_name = propertyName;
	}
	virtual ~MaterialProperty(){};

	virtual void SetData(const void* data){UNUSED(data);};
	virtual void* GetData(){return nullptr;};	
	virtual void Bind(int bindpoint) = 0;
	virtual MaterialPropertyDataType GetDataType(){return UNDEFINED_MATERIAL_PROPERTY;}
	
public:
	std::string m_name;
	//data
};

class MaterialPropertyFloat : public MaterialProperty
{
public:
	MaterialPropertyFloat(const std::string& name, float data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyFloat() override{};

	virtual void SetData(void* data)
	{
		m_data = *(float*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	//bind point is uniformlocation
	virtual void Bind(int bindPoint) override
	{
		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniform1fv(idx, 1, (const GLfloat*) &m_data);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return FLOAT_MATERIAL_PROPERTY;
	}

public:
	float m_data;
};

class MaterialPropertyVector2 : public MaterialProperty
{
public:
	MaterialPropertyVector2(const std::string& name, Vector2 data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyVector2() override{};

	virtual void SetData(void* data)
	{
		m_data = *(Vector2*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	virtual void Bind(int bindPoint) override
	{
		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniform2fv(idx, 1, (const GLfloat*) &m_data);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return VECTOR2_MATERIAL_PROPERTY;
	}

public:
	Vector2 m_data;
};

class MaterialPropertyVector3 : public MaterialProperty
{
public:
	MaterialPropertyVector3(const std::string& name, Vector3 data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyVector3() override{};

	virtual void SetData(void* data)
	{
		m_data = *(Vector3*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	virtual void Bind(int bindPoint) override
	{
		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniform3fv(idx, 1, (const GLfloat*) &m_data);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return VECTOR3_MATERIAL_PROPERTY;
	}

public:
	Vector3 m_data;
};

class MaterialPropertyVector4 : public MaterialProperty
{
public:
	MaterialPropertyVector4(const std::string& name, Vector4 data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyVector4() override{};

	virtual void SetData(void* data)
	{
		m_data = *(Vector4*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	virtual void Bind(int bindPoint) override
	{
		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniform4fv(idx, 1, (const GLfloat*) &m_data);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return VECTOR4_MATERIAL_PROPERTY;
	}


public:
	Vector4 m_data;
};

class MaterialPropertyRGBA : public MaterialProperty
{
public:
	MaterialPropertyRGBA(const std::string& name, Rgba data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyRGBA() override{};

	virtual void SetData(void* data)
	{
		m_data = *(Rgba*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	virtual void Bind(int bindPoint) override
	{
		//convert rgba to vector4
		Vector4 rgba = Vector4(m_data);

		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniform4fv(idx, 1, (const GLfloat*) &rgba);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return RGBA_MATERIAL_PROPERTY;
	}


public:
	Rgba m_data;
};

class MaterialPropertyMatrix44 : public MaterialProperty
{
public:
	MaterialPropertyMatrix44(const std::string& name, Matrix44 data) : MaterialProperty(name)
	{
		m_data = data;	
	}

	~MaterialPropertyMatrix44() override{};

	virtual void SetData(void* data)
	{
		m_data = *(Matrix44*)data;
	}

	virtual void* GetData()
	{
		return (void*)&m_data;
	}

	virtual void Bind(int bindPoint) override
	{
		GLint idx = glGetUniformLocation( bindPoint, m_name.c_str() ); 
		GL_CHECK_ERROR();

		if (idx >= 0) {
			glUniformMatrix4fv(idx, 1, GL_FALSE, (const GLfloat*) &m_data);
			GL_CHECK_ERROR();
		}
	}

	virtual MaterialPropertyDataType GetDataType() override 
	{
		return MATRIX44_MATERIAL_PROPERTY;
	}


public:
	Matrix44 m_data;
};