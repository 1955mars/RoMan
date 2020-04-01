#include "rmpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace RoMan
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: 
			RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
			return nullptr;

		case RendererAPI::OpenGL: 
			return new OpenGLVertexArray();
				
		}

		RM_CORE_ASSERT(false, "Renderer API is not supported by RoMan Engine");
		return nullptr;
	}
}