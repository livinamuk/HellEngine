#pragma once
#include "Platform/OpenGL/Shader.h"

namespace HellEngine
{
	class Quad2D
	{
	public:
		static void RenderQuad(Shader *shader);
		static void RenderCrosshair(Shader *shader, int screenWidth, int screenHeight, int crosshairSize);
		static unsigned int VAO;
	};


}