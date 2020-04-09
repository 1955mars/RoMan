#include "rmpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace RoMan
{
	Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return new OpenGLShader(filepath);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");

		return nullptr;
	}

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSrc, fragmentSrc);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}
}