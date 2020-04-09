#pragma once
#include "RoMan/Renderer/Shader.h"

#include "glm/glm.hpp"

//TODO: Remove
typedef unsigned int GLenum;

namespace RoMan
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const;
		void UnBind() const;

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, glm::vec4& value);

		void UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMatrix4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
	};
}