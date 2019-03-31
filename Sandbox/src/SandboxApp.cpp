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
		//HELL_INFO("ExampleLayer::Update");
		
		//if (HellEngine::Input::IsKeyPressed(HELL_KEY_TAB))
		//	HELL_TRACE("Tab key is pressed (poll)!");
	}

	void OnEvent(HellEngine::Event& event) override
	{
		//HELL_TRACE("{0}", event);

		/*if (event.GetEventType() == HellEngine::EventType::KeyPressed)
		{
			HellEngine::KeyPressedEvent& e = (HellEngine::KeyPressedEvent&)event;
			HELL_TRACE("{0}", (char)e.GetKeyCode());
		}*/

		HellEngine::KeyPressedEvent& e = (HellEngine::KeyPressedEvent&)event;
		if (e.GetKeyCode() == HELL_KEY_ESCAPE)
		{

			//HellEngine::EventDispatcher dispatcher(e);
			//dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		//	HellEngine::Application::Close();
		}
	}

};

class Sandbox : public HellEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
		 
	}

};

HellEngine::Application* HellEngine::CreateApplication()
{
	return new Sandbox();
}