#pragma once

#include "hellpch.h"

#include "HellEngine/Core.h"
#include "HellEngine/Events/Event.h"

namespace HellEngine {

	enum WindowMode
	{
		WINDOWED,
		FULL_SCREEN,
		BORDERLESS
	};

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowMode Mode;

		WindowProps(const std::string& title = "Hell Engine",
			unsigned int width = 1280,
			unsigned int height = 720,
			WindowMode mode = WindowMode::WINDOWED)
			: Title(title), Width(width), Height(height), Mode(mode)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetWindowMode(const WindowMode& mode, unsigned int width = 0, unsigned int height = 0) = 0;
		virtual void ToggleFullscreen() = 0;
		virtual void SetFullscreen() = 0;
		virtual void ToggleMouseEnabled() = 0;
		virtual bool IsMouseEnabled() = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}