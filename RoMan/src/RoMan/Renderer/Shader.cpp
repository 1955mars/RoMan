#include "rmpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace RoMan
{
	std::vector<Ref<Shader>> Shader::s_AllShaders;

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		Ref<Shader> result = nullptr;

		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
				RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
				return nullptr;

			case RendererAPI::API::OpenGL:
				result =  std::make_shared<OpenGLShader>(filepath);
		}

		RM_CORE_ASSERT(result, "Renderer API is not supported by RoMan Engine");

		s_AllShaders.push_back(result);
		return result;
	}

	Ref<Shader> Shader::CreateFromString(const std::string& source)
	{
		Ref<Shader> result = nullptr;
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:
				RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
				return nullptr;

			case RendererAPI::API::OpenGL:
				result = OpenGLShader::CreateFromString(source);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");

		s_AllShaders.push_back(result);
		return result;
	}

	ShaderLibrary::ShaderLibrary()
	{
	}

	ShaderLibrary::~ShaderLibrary()
	{
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		RM_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		RM_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}