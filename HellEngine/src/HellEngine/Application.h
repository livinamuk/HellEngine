#pragma once

#include "Core.h"

namespace HellEngine
{
	class HELL_ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be define in client
	Application* CreateApplication();
}
