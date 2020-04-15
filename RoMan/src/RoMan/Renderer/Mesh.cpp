#include "rmpch.h"

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace RoMan
{
	static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	Mesh::Mesh(const std::string& filename)
		:m_FilePath(filename)
	{
		m_Importer = std::make_unique<Assimp::Importer>();

		const aiScene* scene = m_Importer->ReadFile(filename, aiProcess_Triangulate | aiProcess_GenUVCoords);

		if (!scene || !scene->HasMeshes())
			RM_CORE_ASSERT(false, "Failed to load mesh {0}", filename);

		m_InverseTransform = glm::inverse(aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_SubMeshes.reserve(scene->mNumMeshes);

		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			SubMesh subMesh;
			subMesh.BaseVertex = vertexCount;
			subMesh.BaseIndex = indexCount;

			subMesh.IndexCount = mesh->mNumFaces * 3;

			vertexCount += mesh->mNumVertices;
			indexCount += subMesh.IndexCount;

			m_SubMeshes.push_back(subMesh);

			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				//vertex.Normal = { mesh->mNormals[i].x , mesh->mNormals[i].y, mesh->mNormals[i].z };

				if (mesh->HasTextureCoords(0))
					vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				m_Vertices.push_back(vertex);
			}

			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				RM_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Each face must have three indices");
				m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}
		}

		TraverseNodes(scene->mRootNode);

		m_VertexArray.reset(VertexArray::Create());

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create((float *)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex)));
		vertexBuffer->SetLayout(
			{
				{ShaderDataType::Float3, "a_Position"},
				//{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_TexCoord"}
			}
		);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create((uint32_t *)m_Indices.data(), m_Indices.size() * sizeof(Index)));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_Scene = scene;
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::TraverseNodes(aiNode* node, int level)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t mesh = node->mMeshes[i];
			m_SubMeshes[mesh].Transform = aiMatrix4x4ToGlm(node->mTransformation);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			aiNode* child = node->mChildren[i];
			TraverseNodes(child, level + 1);
		}
	}
}