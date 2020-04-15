#pragma once
#include <vector>
#include <string>

#include "glm/glm.hpp"

#include "RoMan/Renderer/VertexArray.h"
#include "RoMan/Renderer/Buffer.h"
#include "RoMan/Renderer/Shader.h"

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
		//glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	struct SubMesh
	{
		uint32_t BaseVertex;
		uint32_t BaseIndex;
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

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
	private:
		void TraverseNodes(aiNode* node, int level = 0);

	private:
		std::vector<SubMesh> m_SubMeshes;

		glm::mat4 m_InverseTransform;

		std::unique_ptr<Assimp::Importer> m_Importer;

		Ref<VertexArray> m_VertexArray;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		const aiScene* m_Scene;

		//Materials
		Ref<Shader> m_Shader;

		std::string m_FilePath;
	};
}