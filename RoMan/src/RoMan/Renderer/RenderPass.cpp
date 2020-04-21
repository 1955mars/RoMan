#include "rmpch.h"
#include "RenderPass.h"

#include "Platform/OpenGL/OpenGLRenderPass.h"

namespace RoMan
{
	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLRenderPass>(spec);
		}

		RM_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}