#include "rmpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace RoMan
{
	Ref<Texture2D> Texture2D::Create(TextureFormat format, unsigned int width, unsigned int height, TextureWrap wrap)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return  std::make_shared<OpenGLTexture2D>(format, width, height, wrap);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, bool srgb)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return  std::make_shared<OpenGLTexture2D>(path, srgb);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return  std::make_shared<OpenGLTextureCube>(path);

		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}

	uint32_t Texture::GetBPP(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:    return 3;
		case TextureFormat::RGBA:   return 4;
		}
		return 0;
	}
}