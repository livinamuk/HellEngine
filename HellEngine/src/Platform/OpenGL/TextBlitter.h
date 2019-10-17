#pragma once
#include "Platform/OpenGL/Shader.h"

namespace HellEngine
{
	class TextBlitter
	{
	public:
		static void RenderQuad(Shader* shader);
		static void RenderCrosshair(Shader* shader, int screenWidth, int screenHeight, int crosshairSize);
		static unsigned int VAO;
	};
}

/*
#include "HellEngine/Transform.h"
#include "Platform/OpenGL/RenderableObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "HellEngine/Core.h"

namespace HellEngine
{
	class TextBlitter
	{
	public:

		//const std::string CharSheet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+[]{}\\|;:'\".,<>/?`~ ";
		//static unsigned int VAO;

		static void Blit(Shader* shader, std::string text);
	};
}*/