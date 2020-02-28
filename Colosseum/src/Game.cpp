#include <RoMan.h>

class Colosseum : public RoMan::Application
{
public:
	Colosseum()
	{

	}

	~Colosseum()
	{

	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum();
}