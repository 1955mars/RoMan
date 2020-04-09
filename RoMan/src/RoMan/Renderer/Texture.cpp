#include "rmpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace RoMan
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return  std::make_shared<OpenGLTexture2D>(path);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}
}