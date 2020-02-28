#pragma once

#include "Core.h"

namespace RoMan
{
	class ROMAN_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	//To be defined in client
	Application* CreateApplication();
}



