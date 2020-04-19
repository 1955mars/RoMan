#pragma once

#include "RoMan/Core/Timestep.h"

#include "RoMan/Renderer/VertexArray.h"
#include "RoMan/Renderer/Buffer.h"
#include "RoMan/Renderer/Shader.h"
#include "RoMan/Renderer/Material.h"

#include <vector>
#include <string>

#include "glm/glm.hpp"

struct aiNode;
struct aiScene;

namespace Assimp
{
	class Importer;
}

namespace RoMan
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;
	};

	class SubMesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;

		glm::mat4 Transform;
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	class Mesh
	{
	public:
		Mesh(const std::string& filename);
		~Mesh();

		void Render(Timestep ts, Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>());
		void Render(Timestep ts, const glm::mat4& transform = glm::mat4(1.0f), Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>());
		void OnImGuiRender();
		void DumpVertexBuffer();

		inline Ref<Shader> GetMeshShader() { return m_MeshShader; }
		inline Ref<Material> GetMaterial() { return m_Material; }
		inline const std::string& GetFilePath() const { return m_FilePath; }

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
	private:
		void TraverseNodes(aiNode* node, int level = 0);

	private:
		std::vector<SubMesh> m_SubMeshes;

		std::unique_ptr<Assimp::Importer> m_Importer;

		glm::mat4 m_InverseTransform;


		Ref<VertexArray> m_VertexArray;

		std::vector<Vertex> m_StaticVertices;
		std::vector<Index> m_Indices;

		const aiScene* m_Scene;

		// Materials
		Ref<Shader> m_MeshShader;
		Ref<Material> m_Material;


		std::string m_FilePath;
	};
}