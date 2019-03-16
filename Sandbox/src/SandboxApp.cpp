#include "HellEngine.h"

class Sandbox : public HellEngine::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

HellEngine::Application* HellEngine::CreateApplication()
{
	return new Sandbox();
}