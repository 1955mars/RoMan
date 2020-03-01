#include "rmpch.h"
#include "Application.h"

#include "RoMan/Events/ApplicationEvent.h"
#include "RoMan/Log.h"

namespace RoMan
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{

			RM_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			RM_TRACE(e);
		}

		while (1);
	}
}


