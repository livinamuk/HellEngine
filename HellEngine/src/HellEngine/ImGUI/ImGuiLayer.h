
#pragma once

#include "HellEngine/Layer.h"
#include "HellEngine/Events/MouseEvent.h"
#include "HellEngine/Events/ApplicationEvent.h"
#include "HellEngine/Events/KeyEvent.h"

namespace HellEngine {

	class HELL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);


	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent&e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent&e);
		bool OnMouseMovedEvent(MouseMovedEvent&e);
		bool OnMouseScrolledEvent(MouseScrolledEvent&e);
		bool OnKeyPressedEvent(KeyPressedEvent&e);
		bool OnKeyReleasedEvent(KeyReleasedEvent&e);
		bool OnKeyTypedEvent(KeyTypedEvent&e);
		bool OnWindowResizeEvent(WindowResizeEvent&e);

	private:
		float m_Time = 0.0f;
	};

}