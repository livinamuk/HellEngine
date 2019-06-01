#include "hellpch.h"
#include "Application.h"

#include "HellEngine/Log.h"
#include "Platform/Windows/WindowsInput.h"

#include "HellEngine/Camera.h"
#include "HellEngine/Light.h"
#include "HellEngine/Model.h"
#include "Platform/OpenGL/Shader.h"
#include "Platform/OpenGL/Texture.h"

#include "Glad/glad.h"
#include "Input.h"
#include "GLFW/glfw3.h"

#include "Platform/OpenGL/Cube.h"
#include "Platform/OpenGL/Plane.h"

#include "HellEngine/Components/Door.h"
#include "HellEngine/Components/Wall.h"


namespace HellEngine

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		//m_ImGuiLayer = new ImGuiLayer("ImGui");
		PushOverlay(m_ImGuiLayer);

		//Renderer::Init();
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}
	

	void Application::Run()
	{
		while (m_Running)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			if (Input::IsKeyPressed(HELL_KEY_ESCAPE))
				m_Running = false;

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}