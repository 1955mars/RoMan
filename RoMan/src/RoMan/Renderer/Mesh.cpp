#include "rmpch.h"
#include "Mesh.h"

#include <glad/glad.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "imgui.h"

#include <filesystem>

#include "RoMan/Renderer/Renderer.h"

namespace RoMan {

	glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure;    // Validation

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		virtual void write(const char* message) override
		{
			RM_CORE_ERROR("Assimp error: {0}", message);
		}
	};

	Mesh::Mesh(const std::string& filename)
		: m_FilePath(filename)
	{
		LogStream::Initialize();

		RM_CORE_INFO("Loading mesh: {0}", filename.c_str());

		m_Importer = std::make_unique<Assimp::Importer>();

		const aiScene* scene = m_Importer->ReadFile(filename, s_MeshImportFlags);
		if (!scene || !scene->HasMeshes())
			RM_CORE_ERROR("Failed to load mesh file: {0}", filename);
		
		m_MeshShader = Renderer::GetShaderLibrary()->Get("HazelPBR_Static");
		m_BaseMaterial.reset(new RoMan::Material(m_MeshShader));
		m_InverseTransform = glm::inverse(aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_SubMeshes.reserve(scene->mNumMeshes);
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];

			SubMesh submesh;
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			m_SubMeshes.push_back(submesh);

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			RM_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			RM_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");


			{
				for (size_t i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

					if (mesh->HasTangentsAndBitangents())
					{
						vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}

					if (mesh->HasTextureCoords(0))
						vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

					m_StaticVertices.push_back(vertex);
				}
			}

			// Indices
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				RM_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}

		}

		RM_CORE_TRACE("NODES:");
		RM_CORE_TRACE("-----------------------------");
		TraverseNodes(scene->mRootNode);
		RM_CORE_TRACE("-----------------------------");

		//TODO: Bones (animations)

		//Materials
		if (scene->HasMaterials())
		{
			m_Textures.resize(scene->mNumMaterials);
			m_Materials.resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				auto mi = std::make_shared<MaterialInstance>(m_BaseMaterial);
				m_Materials[i] = mi;

				RM_CORE_INFO("Material Name = {0}; Index = {1}", aiMaterialName.data, i);
				aiString aiTexPath;
				uint32_t textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
				RM_CORE_TRACE("     Texture Count = {0}", textureCount);

				aiColor3D aiColor;
				aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				RM_CORE_TRACE("COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);

				if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS)
				{
					//TODO: Should be handled by the engine's file system
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						RM_CORE_TRACE("  Texture Path = {0}", texturePath);
						mi->Set("u_AlbedoTexture", m_Textures[i]);
						mi->Set("u_AlbedoTexToggle", 1.0f);
					}
					else
					{
						RM_CORE_ERROR("Could not load texture: {0}", texturePath);
						mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
					}
				}
				else
				{
					mi->Set("u_AlbedoTexToggle", 0.0f);
					mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
				}

				for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
				{
					auto prop = aiMaterial->mProperties[i];
					RM_CORE_TRACE("Material Property:");
					RM_CORE_TRACE("  Name = {0}", prop->mKey.data);

					switch (prop->mSemantic)
					{
					case aiTextureType_NONE:
						RM_CORE_TRACE("  Semantic = aiTextureType_NONE");
						break;
					case aiTextureType_DIFFUSE:
						RM_CORE_TRACE("  Semantic = aiTextureType_DIFFUSE");
						break;
					case aiTextureType_SPECULAR:
						RM_CORE_TRACE("  Semantic = aiTextureType_SPECULAR");
						break;
					case aiTextureType_AMBIENT:
						RM_CORE_TRACE("  Semantic = aiTextureType_AMBIENT");
						break;
					case aiTextureType_EMISSIVE:
						RM_CORE_TRACE("  Semantic = aiTextureType_EMISSIVE");
						break;
					case aiTextureType_HEIGHT:
						RM_CORE_TRACE("  Semantic = aiTextureType_HEIGHT");
						break;
					case aiTextureType_NORMALS:
						RM_CORE_TRACE("  Semantic = aiTextureType_NORMALS");
						break;
					case aiTextureType_SHININESS:
						RM_CORE_TRACE("  Semantic = aiTextureType_SHININESS");
						break;
					case aiTextureType_OPACITY:
						RM_CORE_TRACE("  Semantic = aiTextureType_OPACITY");
						break;
					case aiTextureType_DISPLACEMENT:
						RM_CORE_TRACE("  Semantic = aiTextureType_DISPLACEMENT");
						break;
					case aiTextureType_LIGHTMAP:
						RM_CORE_TRACE("  Semantic = aiTextureType_LIGHTMAP");
						break;
					case aiTextureType_REFLECTION:
						RM_CORE_TRACE("  Semantic = aiTextureType_REFLECTION");
						break;
					case aiTextureType_UNKNOWN:
						RM_CORE_TRACE("  Semantic = aiTextureType_UNKNOWN");
						break;
					}

					if (prop->mType == aiPTI_String)
					{
						uint32_t strLength = *(uint32_t*)prop->mData;
						std::string str(prop->mData + 4, strLength);
						RM_CORE_TRACE("  Value = {0}", str);

						std::string key = prop->mKey.data;
						if (key == "$raw.ReflectionFactor|file")
						{
							// TODO: Temp - this should be handled by Hazel's filesystem
							std::filesystem::path path = filename;
							auto parentPath = path.parent_path();
							parentPath /= str;
							std::string texturePath = parentPath.string();

							auto texture = Texture2D::Create(texturePath);
							if (texture->Loaded())
							{
								RM_CORE_TRACE("  Metalness map path = {0}", texturePath);
								mi->Set("u_MetalnessTexture", texture);
								mi->Set("u_MetalnessTexToggle", 1.0f);
							}
							else
							{
								RM_CORE_TRACE("Could not load texture: {0}", texturePath);
								mi->Set("u_Metalness", 0.5f);
								mi->Set("u_MetalnessTexToggle", 1.0f);
							}
						}
					}
				}


				// Normal maps
				if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RM_CORE_TRACE("  Normal map path = {0}", texturePath);
						mi->Set("u_NormalTexture", texture);
						mi->Set("u_NormalTexToggle", 1.0f);
					}
					else
					{
						RM_CORE_TRACE("Could not load texture: {0}", texturePath);
						//mi->Set("u_AlbedoTexToggle", 0.0f);
						// mi->Set("u_AlbedoColor", glm::vec3{ color.r, color.g, color.b });
					}
				}

				// Roughness map
				if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RM_CORE_TRACE("  Roughness map path = {0}", texturePath);
						mi->Set("u_RoughnessTexture", texture);
						mi->Set("u_RoughnessTexToggle", 1.0f);
					}
					else
					{
						RM_CORE_TRACE("Could not load texture: {0}", texturePath);
						mi->Set("u_RoughnessTexToggle", 1.0f);
						mi->Set("u_Roughness", 0.5f);
					}
				}

				// Metalness map
				if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
				{
					// TODO: Temp - this should be handled by Hazel's filesystem
					std::filesystem::path path = filename;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();

					auto texture = Texture2D::Create(texturePath);
					if (texture->Loaded())
					{
						RM_CORE_TRACE("  Metalness map path = {0}", texturePath);
						mi->Set("u_MetalnessTexture", texture);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
					else
					{
						RM_CORE_TRACE("Could not load texture: {0}", texturePath);
						mi->Set("u_Metalness", 0.5f);
						mi->Set("u_MetalnessTexToggle", 1.0f);
					}
				}

			}
		}


		m_VertexArray = VertexArray::Create();

		{
			auto vb = VertexBuffer::Create(m_StaticVertices.data(),  m_StaticVertices.size() * sizeof(Vertex));
			vb->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float3, "a_Binormal" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				});
			m_VertexArray->AddVertexBuffer(vb);
		}

		auto ib = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
		m_VertexArray->SetIndexBuffer(ib);
		m_Scene = scene;
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::TraverseNodes(aiNode* node, int level)
	{
		std::string levelText;
		for (int i = 0; i < level; i++)
			levelText += "-";
		RM_CORE_TRACE("{0}Node name: {1}", levelText, std::string(node->mName.data));
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

	void RoMan::Mesh::OnUpdate(Timestep ts)
	{
		//TODO: Animations
	}

	void Mesh::DumpVertexBuffer()
	{
		// TODO: Convert to ImGui
		RM_CORE_TRACE("------------------------------------------------------");
		RM_CORE_TRACE("Vertex Buffer Dump");
		RM_CORE_TRACE("Mesh: {0}", m_FilePath);

		{
			for (size_t i = 0; i < m_StaticVertices.size(); i++)
			{
				auto& vertex = m_StaticVertices[i];
				RM_CORE_TRACE("Vertex: {0}", i);
				RM_CORE_TRACE("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
				RM_CORE_TRACE("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
				RM_CORE_TRACE("Binormal: {0}, {1}, {2}", vertex.Binormal.x, vertex.Binormal.y, vertex.Binormal.z);
				RM_CORE_TRACE("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
				RM_CORE_TRACE("TexCoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
				RM_CORE_TRACE("--");
			}
		}
		RM_CORE_TRACE("------------------------------------------------------");
	}

}