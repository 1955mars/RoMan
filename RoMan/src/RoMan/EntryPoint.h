#pragma once
#ifdef RM_PLATFORM_WINDOWS

extern RoMan::Application* RoMan::CreateApplication();

int main(int argc, char** argv)
{
	printf("RoMan Engine");
	auto app = RoMan::CreateApplication();
	app->Run();
	delete app;

}

#else
	#error RoMan supports only Windows!
#endif