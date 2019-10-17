#pragma once

#include "HellEngine/Window.h"
#include "HellEngine/Renderer/GraphicsContext.h"
 

namespace HellEngine {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		void SetWindowMode(const WindowMode& mode, unsigned int width, unsigned int height) override;
		void ToggleFullscreen() override;
		void ToggleMouseEnabled() override;
		void SetFullscreen() override;
		bool IsMouseEnabled() override;

		inline virtual void* GetNativeWindow() const { return m_Window; }



	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		bool m_fullscreen = true;
		bool m_mouseEnabled = false;
		GLFWwindow * m_Window;
		GLFWmonitor* m_PrimaryMonitor; // Stores a reference to the primary monitor
		GLFWvidmode m_BaseVideoMode;   // Stores the underlying video mode being used by the OS

		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			unsigned int WindowedWidth, WindowedHeight;
			bool VSync;
			WindowMode Mode;


			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		struct WindowedModeParams {
			unsigned int Width, Height;
			int XPos, YPos;
		};
		WindowedModeParams m_OldWindowedParams;
	};

}