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
		RM_INFO("ExampleLayer::Update");
	}

	void OnEvent(RoMan::Event& event) override
	{
		RM_TRACE("{0}", event);
	}

};


class Colosseum : public RoMan::Application
{
public:
	Colosseum()
	{
		PushLayer(new ExampleLayer());
	}

	~Colosseum()
	{

	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum();
}