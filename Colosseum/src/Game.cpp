#include "rmpch.h"
#include <RoMan.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{
		using namespace glm;

		m_Mesh.reset(new RoMan::Mesh("assets/models/m1911/m1911.fbx"));
		m_MeshMaterial.reset(new RoMan::MaterialInstance(m_Mesh->GetMaterial()));

		// Environment
		m_EnvironmentCubeMap = RoMan::TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Radiance.tga");
		//m_EnvironmentCubeMap.reset(RoMan::TextureCube::Create("assets/textures/environments/DebugCubeMap.tga"));
		m_EnvironmentIrradiance = RoMan::TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Irradiance.tga");
		m_BRDFLUT = RoMan::Texture2D::Create("assets/textures/BRDF_LUT.tga");

		m_Framebuffer.reset(RoMan::Framebuffer::Create(1280, 720, RoMan::FramebufferFormat::RGBA16F));
		m_FinalPresentBuffer.reset(RoMan::Framebuffer::Create(1280, 720, RoMan::FramebufferFormat::RGBA8));

		m_Light.Direction = { -0.5f, -0.5f, 1.0f };
		m_Light.Radiance = { 1.0f, 1.0f, 1.0f };

	}

	void OnUpdate(RoMan::Timestep ts) override
	{
		using namespace RoMan;
		using namespace glm;

		m_Camera.Update(ts);
		auto viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

		m_Framebuffer->Bind();
		Renderer::Clear();

		m_MeshMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		m_MeshMaterial->Set("u_Metalness", m_MetalnessInput.Value);
		m_MeshMaterial->Set("u_Roughness", m_RoughnessInput.Value);
		m_MeshMaterial->Set("u_ViewProjectionMatrix", viewProjection);
		m_MeshMaterial->Set("u_ModelMatrix", scale(mat4(1.0f), vec3(m_MeshScale)));
		m_MeshMaterial->Set("lights", m_Light);
		m_MeshMaterial->Set("u_CameraPosition", m_Camera.GetPosition());
		m_MeshMaterial->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		m_MeshMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);

		m_MeshMaterial->Set("u_EnvRadianceTex", m_EnvironmentCubeMap);
		m_MeshMaterial->Set("u_EnvIrradianceTex", m_EnvironmentIrradiance);
		m_MeshMaterial->Set("u_BRDFLUTTexture", m_BRDFLUT);

		if (m_AlbedoInput.TextureMap)
			m_MeshMaterial->Set("u_AlbedoTexture", m_AlbedoInput.TextureMap);
		if (m_NormalInput.TextureMap)
			m_MeshMaterial->Set("u_NormalTexture", m_NormalInput.TextureMap);
		if (m_MetalnessInput.TextureMap)
			m_MeshMaterial->Set("u_MetalnessTexture", m_MetalnessInput.TextureMap);
		if (m_RoughnessInput.TextureMap)
			m_MeshMaterial->Set("u_RoughnessTexture", m_RoughnessInput.TextureMap);

		if (m_Mesh)
			m_Mesh->Render(ts, scale(mat4(1.0f), vec3(m_MeshScale)), m_MeshMaterial);

		m_Framebuffer->Unbind();

	}

	virtual void OnImGuiRender() override
	{
	}

	void OnEvent(RoMan::Event& event) override
	{

	}

private:
	RoMan::ShaderLibrary m_ShaderLibrary;

	RoMan::Ref<RoMan::Mesh> m_Mesh;
	RoMan::Ref<RoMan::MaterialInstance> m_MeshMaterial;

	RoMan::Ref<RoMan::Texture2D> m_BRDFLUT;
	float m_MeshScale = 1.0f;

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		RoMan::Ref<RoMan::Texture2D> TextureMap;
		bool SRGB = true;
		bool UseTexture = false;
	};
	AlbedoInput m_AlbedoInput;

	struct NormalInput
	{
		RoMan::Ref<RoMan::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	NormalInput m_NormalInput;

	struct MetalnessInput
	{
		float Value = 1.0f;
		RoMan::Ref<RoMan::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	MetalnessInput m_MetalnessInput;

	struct RoughnessInput
	{
		float Value = 0.5f;
		RoMan::Ref<RoMan::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	RoughnessInput m_RoughnessInput;

	std::unique_ptr<RoMan::Framebuffer> m_Framebuffer, m_FinalPresentBuffer;

	RoMan::Ref<RoMan::VertexBuffer> m_VertexBuffer;
	RoMan::Ref<RoMan::IndexBuffer> m_IndexBuffer;
	RoMan::Ref<RoMan::TextureCube> m_EnvironmentCubeMap, m_EnvironmentIrradiance;

	RoMan::Camera m_Camera;

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;
	};
	Light m_Light;
	float m_LightMultiplier = 0.3f;

	// PBR params
	float m_Exposure = 1.0f;

	bool m_RadiancePrefilter = false;

	float m_EnvMapRotation = 0.0f;

	enum class Scene : uint32_t
	{
		Spheres = 0, Model = 1
	};
	Scene m_Scene;

	// Editor resources
	RoMan::Ref<RoMan::Texture2D> m_CheckerboardTex;

};


class Colosseum : public RoMan::Application
{
public:
	Colosseum()
	{
		PushLayer(new ExampleLayer());
	}

	~Colosseum()
	{

	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum();
}