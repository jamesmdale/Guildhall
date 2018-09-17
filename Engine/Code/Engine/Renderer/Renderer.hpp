#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Disc2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Renderer\BitmapFont.hpp"
#include <map>
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Renderer\RenderBuffer.hpp"
#include "Engine\Renderer\Sampler.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Renderer\RendererTypes.hpp"
#include "Engine\Renderer\UniformBuffer.hpp"
#include "Engine\Renderer\Material.hpp"
#include "Engine\Core\Light.hpp"
#include "Engine\Core\Image.hpp"

//class RenderBuffer;
//class Mesh;
class MeshBuilder;
struct VertexPCU;
class Mesh;
class ShaderProgram;
class Shader;
struct RenderState; 
class Material;

struct LightBuffer
{
	Vector4 m_ambientColorAndIntensity  = Vector4(1.f, 1.f, 1.f, 0.25f);  //r, g, b, strength
	Light m_lights[MAX_LIGHTS];

	void SetBufferToDefault()
	{
		m_ambientColorAndIntensity = Vector4(1.f, 1.f, 1.f, 0.25f);
		for(int lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++)
		{
			m_lights[lightIndex].SetToDefault();
			m_lights[lightIndex].Disable();
		}
	}
};

struct SpecularLightBuffer
{
	float m_specularAmount = 0.75f;
	float m_specularPower = 100.f;
	Vector2 m_padding0;

	void SetBufferToDefault()
	{
		m_specularAmount = 0.75f;
		m_specularPower = 100.f;
	}
};

class Renderer
{
public:
	std::map<std::string, Texture*> m_loadedTextures;
	std::map<std::string, BitmapFont*> m_loadedFonts;
	std::map<std::string, ShaderProgram*> m_loadedShaderPrograms;
	std::map<std::string, Shader*> m_loadedShaders;
	std::map<std::string, Mesh*> m_loadedMeshes;
	std::map<std::string, Material*> m_loadedMaterials;
	std::map<std::string, Image*> m_loadedImages;
	//std::map<std::string, Material*> m_loadedMaterials

	GLuint m_defaultVAO = 0;

	//cameras
	Camera* m_defaultCamera;
	Camera* m_currentCamera;

	Camera* m_effectCamera;

	//shader
	Shader* m_defaultShader;
	Shader* m_currentShader;

	//shader programs
	/*ShaderProgram* m_defaultShaderProgram;
	ShaderProgram* m_currentShaderProgram;*/

	//sampler
	Sampler* m_defaultSampler;

	//textures
	Texture* m_defaultTexture;

	//texture targets
	Texture* m_defaultColorTarget;
	Texture* m_defaultDepthTarget;

	//meshes
	Mesh* m_defaultMesh;

	//effects
	Texture* m_effectTarget;
	Texture* m_effectScratch;	

	//ubos
	UniformBuffer* m_lightBuffer;
	UniformBuffer* m_specularLightBuffer;

private:
	Renderer() {}
	
public:
	~Renderer() {}
	static Renderer* GetInstance();
	static Renderer* CreateInstance();

	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void PostStartup();

	//draw functions --------------------------------------------------------------------------------------------------------------
	void DrawMeshImmediate(const VertexPCU* verts, int const numVerts, int drawPrimitive, const Matrix44& modelMatrix = Matrix44::IDENTITY);
	void DrawMeshImmediateWithIndices(const VertexPCU* verts, int* indices, int const numVerts, int const numIndices, int drawPrimitive);
	void DrawMesh(Mesh* mesh, const Matrix44& modelMatrix = Matrix44::IDENTITY);

	//line ------------------------------------------------------------------------------------------------------------------------
	void DrawLine(const Vector2& startingPoint, const Vector2& endPoint);
	void DrawLine(const Vector3 & startingPoint, const Vector3 & endPoint);
	void DrawLineWithColor(const Vector3& startingPoint, const Vector3& endPoint, const Rgba& color);
	//void DrawLineWithColorFeathered(const Vector2& startingPoint, const Vector2& endPoint, const Rgba& startColor, const Rgba& endColor) const;

	//triangle --------------------------------------------------------------------------------------------------------------------
	void DrawTriangle(const Vector3& aPosition, const Vector3& bPosition, const Vector3& cPosition, const Rgba& tint);

	//AABB (quad) -----------------------------------------------------------------------------------------------------------------
	void DrawAABB(const AABB2& bounds, const Rgba& tint);
	void DrawTexturedAABB(Matrix44& transformMatrix, const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint);
	void DrawTexturedAABB(const AABB2& bounds, const Texture & texture, const Vector2 & texCoordsAtMins, const Vector2 & texCoordsAtMaxs, const Rgba & tint);
	void DrawOrientedTexturedAABB(Matrix44& transformMatrix, const AABB2& bounds, Texture* texture, const Rgba& tint = Rgba::WHITE, const AABB2& uvs = AABB2::ZERO_TO_ONE);

	//disc ------------------------------------------------------------------------------------------------------------------------
	void DrawDottedDisc2WithColor(const Disc2&, const Rgba& color, const float& numberOfSides);
	
	//text ------------------------------------------------------------------------------------------------------------------------
	void DrawText2D( const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint = Rgba::WHITE, float aspectScale = 1.f, const BitmapFont* font = nullptr );
	void DrawText2DCentered(const Vector2& drawCenterPoint, const std::string& asciiText, float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont* font);
	void DrawOrientedText2DCentered(Matrix44& transformMatrix, const std::string & asciiText, float cellHeight, const Rgba & tint, float aspectScale, const BitmapFont * font);
	//void DrawOrientedTexturedAABB(const Vector3& position, const AABB2& bounds, Texture* texture, const Rgba& tint, const AABB2& uvs);

	//cube -------------------------------------------------------------------------------------------------------------------------
	void DrawCube(const Vector3& center, const Vector3& dimensions, const Rgba& tint = Rgba::WHITE, const AABB2& uvTop = AABB2::ZERO_TO_ONE, const AABB2& uvSide = AABB2::ZERO_TO_ONE, const AABB2& uvBottom = AABB2::ZERO_TO_ONE);

	//skybox-------------------------------------------------------------------------------------------------------------------------
	void DrawSkybox(Skybox* skybox);

	//mesh -------------------------------------------------------------------------------------------------------------------------
	void BindMeshToProgram(ShaderProgram* program, Mesh* mesh);	

	//GL Functions -------------------------------------------------------------------------------------------------------------------------
	void LineWidth(float lineWidth);
	void Enable();
	void EnableDepth(DepthComparisonType compareType, bool shouldWrite);
	void ClearColor(const Rgba& color) const;
	void ClearDepth(const float& amount) const;
	void DisableDepth();
	void Clear() const;	
	//void SetColor(const Rgba& color) const;
	void Blend() const;
	void ResetBlend() const;
	void SetDrawMode(DrawModeFaceType face, DrawModeType mode);
	void SetDefaultDrawMode();	

	//render target -------------------------------------------------------------------------------------------------------------------------
	Texture* CreateRenderTarget(int width, int height, TextureFormatType format = TEXTURE_FORMAT_RGBA8);
	Texture* CreateDepthStencilTarget(int width, int height);
	Texture* GetDefaultDepthStencilTarget();
	Texture* GetDefaultRenderTarget();

	//frame buffer operations -------------------------------------------------------------------------------------------------------------------------
	bool CopyFrameBuffer( FrameBuffer* destination, FrameBuffer* source );

	//textures -------------------------------------------------------------------------------------------------------------------------
	void SetTexture(const Texture& texture);
	void SetTexture(const Texture & texture, int index);
	void BindTexture(const Texture& texture, int index, Sampler* sampler = nullptr);

	void BindTextureCube(TextureCube * textureCube, int index);

	//samplers -------------------------------------------------------------------------------------------------------------------------
	void BindSampler(Sampler* sampler, int textureIndex);

	//void SetTextureIntoSlot(const Texture& texture, int slotIndex);

	//Camera -------------------------------------------------------------------------------------------------------------------------
	void SetCamera(Camera* camera);

	//shader functions -------------------------------------------------------------------------------------------------------------------------
	void BindRenderState( const RenderState& state ); 
	void BindShaderProgram(ShaderProgram* program);
	void SetShader( Shader* shader ); 
	void UseShaderProgram(ShaderProgram* program);	
	void ReloadShaders();
	void LoadErrorShader();	
	void CopyTexture(Texture* destination, Texture* source);

	TODO("Set general method for uniform binding");
	bool SetMatrix44Uniform(int programHandle, std::string uniformName, const Matrix44& input);
	bool SetFloatUniform(int programHandle, std::string uniformName, float input);
	bool SetVector2Uniform(int programHandle, std::string uniformName, const Vector2& input);
	bool SetVector3Uniform(int programHandle, std::string uniformName, const Vector3& input);
	bool SetVector4Uniform(int programHandle, std::string uniformName, const Vector4& input);

	//material functions -----------------------------------------------------------------------------------------------------------------------
	void BindMaterial(Material* material);

	//lighting functions -----------------------------------------------------------------------------------------------------------------------
	void SetAmbientLight(const Rgba& color, float intensity);
	void SetAmbientLightIntensity(float intensity);
	void SetSpecularConstants(float specularAmount, float specularPower);
	void SetSpecularAmount(float specularAmount);
	void SetSpecularPower(float specularPower);	

	Vector4 GetAmbientLight();
	Rgba GetAmbientLightAsRGBA();
	float GetAmbientLightIntensity();
	float GetSpecularAmount();
	float GetSpecularPower();

	void DisableAllLights();
	void ResetSpecularBuffer();
	void EnablePointLight(int index, const Vector3& position, const Rgba& color, float intensity = 1.0f, const Vector3& attenuationConstants = Vector3(0.f, 0.f, 1.f));
	void EnableDirectionalLight(int index, const Vector3& position, const Rgba& color, float intensity = 1.0f, const Vector3& attenuationConstants = Vector3(0.f, 0.f, 1.f), const Vector3& forward = Vector3::FORWARD, float directionFactor = 1.f, float innerAngle = 360.f, float outerAngle = 360.f, float isShadowCasting = 0.f, const Matrix44& viewProjection = Matrix44::IDENTITY);
	void EnableConeLight(int index, const Vector3& position, Rgba& color, float intensity = 1.0f, const Vector3& attenuationConstants = Vector3(0.f, 0.f, 1.f), const Vector3& forward = Vector3::FORWARD, float directionFactor = 1.f, float innerAngle = 360.f, float outerAngle = 360.f);
	
	//enable light regardless of type -----------------------------------------------------------------------------------------------------------------------
	void EnableLight(int index, const Vector3& position, const Rgba& color, float intensity = 1.0f, const Vector3& attenuationConstants = Vector3(0.f, 0.f, 1.f), const Vector3& forward = Vector3::FORWARD, float directionFactor = 1.f, float innerAngle = 360.f, float outerAngle = 360.f, float isShadowCasting = 0.f, const Matrix44& viewProjection = Matrix44::IDENTITY);
	void EnableLight(int index, const Light& light);
	void SetLightBufferFromArray(std::vector<Light*> lights);

	//effects -----------------------------------------------------------------------------------------------------------------------------------
	void ApplyEffect(ShaderProgram * program);
	void FinishEffects();

	//Resource Getters ----------------------------------------------------------------------------------------------------------------------------
	ShaderProgram* CreateOrGetShaderProgramFromPath(const std::string& shaderFilePath);
	Shader* CreateOrGetShader(const std::string& shaderName);
	Material* CreateOrGetMaterial(const std::string& materialName);
	ShaderProgram* Renderer::CreateOrGetShaderProgramFromSeparatePaths(const std::string& id, const std::string& shaderFilePathVS = "", const std::string& shaderFilePathFS = "");

	Texture* CreateOrGetTexture(const std::string& imageFilePath );
	Texture* CreateOrGetTexture(Image& imageFilePath);
	BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontName);	
	Image* CreateOrGetImage(const std::string imageFilePath);

};

extern void* gGLLibrary; 
extern void* gGLwnd;   // window our context is attached to; 
extern void* gHDC;   // our device context
extern void* gGLContext;    // our rendering context; 

bool RenderStartup();
void GLShutdown();


