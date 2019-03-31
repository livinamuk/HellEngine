#pragma once

#include "Core.h"

#include "Window.h"
#include "HellEngine/LayerStack.h"
#include "HellEngine/Events/Event.h"
#include "HellEngine/Events/ApplicationEvent.h"
#include "HellEngine/ImGui/ImGuiLayer.h"

namespace HellEngine
{
	class HELL_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		
		void OnEvent(Event& e);
		
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() {	return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		static Application* s_Instance;

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	// To be define in client
	Application* CreateApplication();
}
