#pragma once
#ifdef RM_PLATFORM_WINDOWS

extern RoMan::Application* RoMan::CreateApplication();

int main(int argc, char** argv)
{

	RoMan::Log::Init();
	
	RM_CORE_WARN("Initialized Log");
	int a = 7;
	RM_INFO("Hello! Var = {0}", a);

	auto app = RoMan::CreateApplication();
	app->Run();
	delete app;

}

#else
	#error RoMan supports only Windows!
#endif