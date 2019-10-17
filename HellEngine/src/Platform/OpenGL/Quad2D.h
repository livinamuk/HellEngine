#pragma once
#include "Platform/OpenGL/Shader.h"

namespace HellEngine
{
	class Quad2D
	{
	public:
		static void RenderQuad(Shader *shader);
		static void RenderCrosshair(Shader* shader, int screenWidth, int screenHeight, int crosshairSize);
		
		static unsigned int VAO;


		// TEXT BLIT SHIT
		static void DrawTextBlit(Shader* shader);
		static void UpdateBlitter(float deltaTime);
		static void TypeText(std::string text, bool centered);
		static void BlitText(std::string text, bool centered);

		static unsigned int BLIT_VAO, BLIT_VBO;
		static unsigned int currentCharIndex;
		static std::string textToBlit;
		static float blitTimer;
		static float blitSpeed;
		static float waitTimer;
		static float timeToWait;
		static std::string CharSheet;
		static bool centerText;


	private:
	};
}