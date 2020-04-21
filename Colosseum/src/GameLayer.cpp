#include "GameLayer.h"

namespace RoMan
{
	GameLayer::GameLayer()
		: m_SceneType(SceneType::Model)
	{

	}

	GameLayer::~GameLayer()
	{

	}

	void GameLayer::OnAttach()
	{
		using namespace glm;

		//auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

		// Model Scene
		//{
		//	m_Scene = std::make_shared<Scene>("Model Scene");
		//	m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));

		//	//m_Scene->SetEnvironment(environment);

		//	m_MeshEntity = m_Scene->CreateEntity();

		//	auto mesh = std::make_shared<Mesh>("assets/models/m1911/m1911.fbx");
		//	//auto mesh = CreateRef<Mesh>("assets/meshes/cerberus/CerberusMaterials.fbx");
		//	// auto mesh = CreateRef<Mesh>("assets/models/m1911/M1911Materials.fbx");
		//	m_MeshEntity->SetMesh(mesh);

		//	m_MeshMaterial = mesh->GetMaterial();
		//}

		// Sphere Scene
		{
			m_SphereScene = std::make_shared<Scene>("PBR Sphere Scene");
			m_SphereScene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));

			//m_SphereScene->SetEnvironment(environment);

			auto sphereMesh = std::make_shared<Mesh>("assets/models/Sphere1m.fbx");
			m_SphereBaseMaterial = sphereMesh->GetMaterial();

			float x = -4.0f;
			float roughness = 0.0f;
			for (int i = 0; i < 1; i++)
			{
				auto sphereEntity = m_SphereScene->CreateEntity();

				//Ref<MaterialInstance> mi = std::make_shared<MaterialInstance>(m_SphereBaseMaterial);
				//mi->Set("u_Color", 1.0f);
				//mi->Set("u_Metalness", 1.0f);
				//mi->Set("u_Roughness", roughness);
				//x += 1.1f;
				//roughness += 0.15f;
				//m_MetalSphereMaterialInstances.push_back(mi);

				//sphereEntity->SetMesh(sphereMesh);
				//sphereEntity->SetMaterial(mi);
				//sphereEntity->Transform() = translate(mat4(1.0f), vec3(x, 0.0f, 0.0f));
			}

			//x = -4.0f;
			//roughness = 0.0f;
			//for (int i = 0; i < 8; i++)
			//{
			//	auto sphereEntity = m_SphereScene->CreateEntity();

			//	Ref<MaterialInstance> mi(new MaterialInstance(m_SphereBaseMaterial));
			//	mi->Set("u_Metalness", 0.0f);
			//	mi->Set("u_Roughness", roughness);
			//	x += 1.1f;
			//	roughness += 0.15f;
			//	m_DielectricSphereMaterialInstances.push_back(mi);

			//	sphereEntity->SetMesh(sphereMesh);
			//	sphereEntity->SetMaterial(mi);
			//	sphereEntity->Transform() = translate(mat4(1.0f), vec3(x, 1.2f, 0.0f));
			//}
		}

		m_ActiveScene = m_SphereScene;

		//m_PlaneMesh.reset(new Mesh("assets/models/Plane1m.obj"));

		// Editor
		//m_CheckerboardTex = Texture2D::Create("assets/editor/Checkerboard.tga");

		// Set lights
		//m_Light.Direction = { -0.5f, -0.5f, 1.0f };
		//m_Light.Radiance = { 1.0f, 1.0f, 1.0f };
	}

	void GameLayer::OnDetach()
	{

	}

	void GameLayer::OnUpdate(Timestep ts)
	{
		// THINGS TO LOOK AT:
		// - BRDF LUT
		// - Tonemapping and proper HDR pipeline
		using namespace RoMan;
		using namespace glm;

		//m_MeshMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		//m_MeshMaterial->Set("u_Metalness", m_MetalnessInput.Value);
		//m_MeshMaterial->Set("u_Roughness", m_RoughnessInput.Value);
		//m_MeshMaterial->Set("lights", m_Light);
		//m_MeshMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		//m_MeshMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		//m_MeshMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		//m_MeshMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		//m_MeshMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);

		//m_SphereBaseMaterial->Set("u_AlbedoColor", m_AlbedoInput.Color);
		//m_SphereBaseMaterial->Set("lights", m_Light);
		//m_SphereBaseMaterial->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
		//m_SphereBaseMaterial->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		//m_SphereBaseMaterial->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		//m_SphereBaseMaterial->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		//m_SphereBaseMaterial->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		//m_SphereBaseMaterial->Set("u_EnvMapRotation", m_EnvMapRotation);

		//if (m_AlbedoInput.TextureMap)
		//	m_MeshMaterial->Set("u_AlbedoTexture", m_AlbedoInput.TextureMap);
		//if (m_NormalInput.TextureMap)
		//	m_MeshMaterial->Set("u_NormalTexture", m_NormalInput.TextureMap);
		//if (m_MetalnessInput.TextureMap)
		//	m_MeshMaterial->Set("u_MetalnessTexture", m_MetalnessInput.TextureMap);
		//if (m_RoughnessInput.TextureMap)
		//	m_MeshMaterial->Set("u_RoughnessTexture", m_RoughnessInput.TextureMap);

		m_ActiveScene->OnUpdate(ts);
	}

	void GameLayer::OnImGuiRender()
	{
	}

	void GameLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(RM_BIND_EVENT_FN(GameLayer::OnKeyPressedEvent));
	}

	bool GameLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		return false;
	}
}