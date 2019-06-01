
#pragma once

#include "HellEngine/Layer.h"
#include "HellEngine/Events/MouseEvent.h"
#include "HellEngine/Events/ApplicationEvent.h"
#include "HellEngine/Events/KeyEvent.h"

namespace HellEngine {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};

}