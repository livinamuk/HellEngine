#include "hellpch.h"
#include "WindowsInput.h"

#include "HellEngine/Application.h"
#include <GLFW/glfw3.h>

namespace HellEngine {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedOnceImpl(int keycode)
	{/*
		// First check if key was pressed last frame
		if (WasKeyPressedLastFrame[keycode])
			return false;

		// Now check if it's being pressed now
		if (IsKeyPressedImpl(keycode))
		{ 
			WasKeyPressedLastFrame[keycode] = true;
			return true;
		}*/
		return false;
	}	
	
	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsKeyReleasedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), keycode);

		return state == GLFW_RELEASE;
	}
	

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return y;
	}

}