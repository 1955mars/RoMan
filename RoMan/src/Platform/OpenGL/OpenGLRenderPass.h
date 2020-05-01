#pragma once

#include "RoMan/Renderer/RenderPass.h"

namespace RoMan
{
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec);
		virtual ~OpenGLRenderPass();

		virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

	private:
		RenderPassSpecification m_Specification;
	};
}