#pragma once

#include "Entity.h"
#include "RoMan/Renderer/Camera.h"

#include <vector>

namespace RoMan
{
	struct Environment
	{
		Ref<TextureCube> RadianceMap;
		Ref<TextureCube> IrradianceMap;

		static Environment Load(const std::string& filename);
	};

	class Scene
	{
	public:
		Scene(const std::string& debugName = "Scene");
		~Scene();

		void Init();

		void OnUpdate(Timestep ts);

		void SetCamera(const Camera& camera);
		Camera& GetCamera() { return m_Camera; }

		void SetEnvironment(const Environment& environment);
		void SetSkybox(const Ref<TextureCube>& skybox);

		float& GetSkyboxLoD() { return m_SkyboxLoD; }

		void AddEntity(Entity* entity);
		Entity* CreateEntity();
	private:
		std::string m_DebugSceneName;
		std::vector<Entity*> m_Entities;
		Camera m_Camera;

		Environment m_Environment;
		Ref<TextureCube> m_SkyboxTexture;
		Ref<MaterialInstance> m_SkyboxMaterial;

		float m_SkyboxLoD = 1.0f;

		friend class SceneRenderer;
		friend class SceneHierarchyPanel;
	};
}