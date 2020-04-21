#pragma once
#include "OpenGLShaderUniform.h"
#include "RoMan/Renderer/Renderer.h"
#include "RoMan/Renderer/Shader.h"
#include "glm/glm.hpp"
#include "glm/glm.hpp"

//TODO: Remove
typedef unsigned int GLenum;

namespace RoMan
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath);
		static Ref<OpenGLShader> CreateFromString(const std::string& source);
		virtual ~OpenGLShader();

		virtual void Reload() override;
		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

		void Bind() const override;
		virtual RendererID GetRendererID() const override { return m_RendererID; }
		void UnBind() const;

		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) override;
		virtual void SetFSMaterialUniformBuffer(Buffer buffer) override;

		inline const ShaderUniformBufferList& GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
		inline const ShaderUniformBufferList& GetFSRendererUniforms() const override { return m_FSRendererUniformBuffers; }
		virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
		virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_FSMaterialUniformBuffer; }
		inline const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
		inline const ShaderUniformBufferDeclaration& GetFSMaterialUniformBuffer() const override { return *m_FSMaterialUniformBuffer; }
		inline const ShaderResourceList& GetResources() const override { return m_Resources; }

	private:
		void Load(const std::string& source);
		std::string ReadShaderFromFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Parse();
		void ParseUniform(const std::string& statement, ShaderDomain domain);
		void ParseUniformStruct(const std::string& block, ShaderDomain domain);
		ShaderStruct* FindStruct(const std::string& name);

		int32_t GetUniformLocation(const std::string& name) const;

		void ResolveUniforms();
		void ValidateUniforms();
		void CompileAndUploadShader();

		static GLenum ShaderTypeFromString(const std::string& type);

		void ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer);
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
		void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset);

		void UploadUniformInt(uint32_t location, int32_t value);
		void UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count);
		void UploadUniformFloat(uint32_t location, float value);
		void UploadUniformFloat2(uint32_t location, glm::vec2& value);
		void UploadUniformFloat3(uint32_t location, glm::vec3& value);
		void UploadUniformFloat4(uint32_t location, glm::vec4& value);
		void UploadUniformMatrix3(uint32_t location, const glm::mat3& matrix);
		void UploadUniformMatrix4(uint32_t location, const glm::mat4& matrix);
		void UploadUniformMatrix4Array(uint32_t location, const glm::mat4& matrix, uint32_t count);

		void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset);

		void UploadUniformInt(const std::string& name, int32_t value);
		void UploadUniformIntArray(const std::string& name, int32_t* value, int32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMatrix4(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t m_RendererID = 0;
		bool m_Loaded = false;
		bool m_IsCompute = false;

		std::string m_Name, m_AssetPath;
		std::unordered_map<GLenum, std::string> m_ShaderSource;

		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;

		ShaderUniformBufferList m_VSRendererUniformBuffers;
		ShaderUniformBufferList m_FSRendererUniformBuffers;
		Scope<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
		Scope<OpenGLShaderUniformBufferDeclaration> m_FSMaterialUniformBuffer;
		ShaderResourceList m_Resources;
		ShaderStructList m_Structs;
	};
}