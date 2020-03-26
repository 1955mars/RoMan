#include "rmpch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace RoMan
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:
			RM_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;
		
		case RendererAPI::OpenGL:
			return new OpenGLVertexBuffer(vertices, size);
		}

		RM_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:
			RM_CORE_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::OpenGL:
			return new OpenGLIndexBuffer(indices, count);
		}

		RM_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}