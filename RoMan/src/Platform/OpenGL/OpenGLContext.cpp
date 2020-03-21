#include "rmpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace RoMan
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		RM_CORE_ASSERT(m_WindowHandle, "Window Handle is null!")
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RM_CORE_ASSERT(status, "Failed to initialize Glad");

		RM_CORE_INFO("OpenGL Info:");
		RM_CORE_INFO(" Vendor: {0}", glGetString(GL_VENDOR));
		RM_CORE_INFO(" Renderer: {0}", glGetString(GL_RENDERER));
		RM_CORE_INFO(" Version: {0}", glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}