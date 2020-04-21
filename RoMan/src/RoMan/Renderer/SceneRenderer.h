#pragma once

#include "RoMan/Renderer/Mesh.h"
#include "RoMan/Renderer/Renderer.h"
#include "RoMan/Renderer/RenderPass.h"
#include "RoMan/Scene/Entity.h"
#include "RoMan/Scene/Scene.h"


namespace RoMan
{
	class SceneRenderer
	{
	public:
		static void Init();
		
		static void SetViewPortSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene);
		static void EndScene();

		static void SubmitEntity(Entity* entity);

		static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

		static Ref<Texture2D> GetFinalColorBuffer();

		static uint32_t GetFinalColorBufferRendererID();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
	};
}