#pragma once
#include <RoMan.h>
#include <RoMan/EntryPoint.h>

#include "GameLayer.h"

class Colosseum : public RoMan::Application
{
public:
	Colosseum(const RoMan::ApplicationProps& props)
		: Application(props)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new RoMan::GameLayer());
	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum({ "Colosseum", 1600, 900 });
}