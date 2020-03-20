#include "rmpch.h"
#include <RoMan.h>

class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//RM_INFO("ExampleLayer::Update");

		if (RoMan::Input::IsKeyPressed(RM_KEY_TAB))
		{
			RM_TRACE("Tab key is pressed (polling!)");
		}
	}

	void OnEvent(RoMan::Event& event) override
	{
		//RM_TRACE("{0}", event);

		if (event.GetEventType() == RoMan::EventType::KeyPressed)
		{
			RoMan::KeyPressedEvent& e = (RoMan::KeyPressedEvent&) event;
			if (e.GetKeyCode() == RM_KEY_TAB)
			{
				RM_TRACE("Tab key is pressed (event!)");
			}
			RM_TRACE("{0}", (char)e.GetKeyCode());
		}

	}

};


class Colosseum : public RoMan::Application
{
public:
	Colosseum()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new RoMan::ImGuiLayer());
	}

	~Colosseum()
	{

	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum();
}