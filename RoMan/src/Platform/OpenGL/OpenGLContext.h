#pragma once

#include "RoMan/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace RoMan
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}


