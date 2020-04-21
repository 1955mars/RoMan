#include "rmpch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace RoMan
{
	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;
		
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(data, size, usage);
		}

		RM_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size, usage);
		}

		RM_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			RM_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(data, size);
		}

		RM_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}