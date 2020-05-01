#pragma once

#include "RoMan/Core.h"
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

		void OnUpdate(Timestep ts);
		void DumpVertexBuffer();

		Ref<Shader> GetMeshShader() { return m_MeshShader; }
		Ref<Material> GetMaterial() { return m_BaseMaterial; }
		std::vector<Ref<MaterialInstance>> GetMaterials() { return m_Materials; }
		const std::vector<Ref<Texture2D>> GetTextures() { return m_Textures; }
		inline const std::string& GetFilePath() const { return m_FilePath; }

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
		Ref<Material> m_BaseMaterial;
		std::vector<Ref<Texture2D>> m_Textures;
		std::vector<Ref<Texture2D>> m_NormalMaps;
		std::vector<Ref<MaterialInstance>> m_Materials;


		std::string m_FilePath;

		friend class Renderer;
		friend class SceneHierarchyPanel;
	};
}