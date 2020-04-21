#pragma once

#include "RoMan/Core.h"
#include "RoMan/Renderer/Material.h"
#include "RoMan/Renderer/Mesh.h"

#include <glm/glm.hpp>

namespace RoMan
{
	class Entity
	{
	public:
		Entity();
		~Entity();

		void SetMesh(const Ref<Mesh>& mesh) { m_Mesh = mesh; }
		Ref<Mesh> GetMesh() { return m_Mesh; }

		void SetMaterial(const Ref<MaterialInstance>& material) { m_Material = material; }
		Ref<MaterialInstance> GetMaterial() { return m_Material; }

		const glm::mat4& GetTransform() { return m_Transform; }
		glm::mat4& Transform() { return m_Transform; }

	private:
		glm::mat4 m_Transform;

		Ref<Mesh> m_Mesh;
		Ref<MaterialInstance> m_Material;
	};
}