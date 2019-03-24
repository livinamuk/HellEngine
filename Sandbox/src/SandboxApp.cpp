#include <HellEngine.h>

class ExampleLayer : public HellEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		HELL_INFO("ExampleLayer::Update");
	}

	void OnEvent(HellEngine::Event& event) override
	{
		HELL_TRACE("{0}", event);
	}

};

class Sandbox : public HellEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new HellEngine::ImGuiLayer());
	}

	~Sandbox()
	{
		 
	}

};

HellEngine::Application* HellEngine::CreateApplication()
{
	return new Sandbox();
}