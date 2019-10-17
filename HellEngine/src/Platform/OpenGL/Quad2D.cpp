#include "hellpch.h"
#include "Quad2D.h"
#include <algorithm>

namespace HellEngine
{
	unsigned int HellEngine::Quad2D::VAO = 0;
	unsigned int HellEngine::Quad2D::BLIT_VAO = 0;
	unsigned int HellEngine::Quad2D::BLIT_VBO = 0;
	unsigned int HellEngine::Quad2D::currentCharIndex = 0;
	float HellEngine::Quad2D::blitTimer = 0;
	float HellEngine::Quad2D::blitSpeed = 50;
	float HellEngine::Quad2D::waitTimer = 0;
	float HellEngine::Quad2D::timeToWait = 2;
	bool HellEngine::Quad2D::centerText = true;
	std::string HellEngine::Quad2D::CharSheet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+[]{}\\|;:'\".,<>/?`~ ";


	std::string HellEngine::Quad2D::textToBlit = "";

	void HellEngine::Quad2D::RenderQuad(Shader* shader)
	{
		if (VAO == 0)
		{
			float quadVertices[] = {
				// positions         texcoords
				-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
					1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
					1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			};
			unsigned int VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}

		shader->setMat4("model", glm::mat4(1));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}


	void HellEngine::Quad2D::RenderCrosshair(Shader* shader, int screenWidth, int screenHeight, int crosshairSize)
	{
		float width = (1.0 / screenWidth) * crosshairSize;
		float height = (1.0 / screenHeight) * crosshairSize;
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1));
		shader->setMat4("model", modelMatrix);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void HellEngine::Quad2D::UpdateBlitter(float deltaTime)
	{
		// Main timer
		blitTimer += deltaTime * blitSpeed;
		currentCharIndex = blitTimer;
		currentCharIndex = std::min(currentCharIndex, (unsigned int)textToBlit.size());

		if (blitSpeed == -1) {
			currentCharIndex = (unsigned int)textToBlit.size();
			return;
		}

		if (timeToWait == -1)
			return;

		// Wait time
		if (currentCharIndex == (unsigned int)textToBlit.size())
			waitTimer += deltaTime;
		if (waitTimer > timeToWait)
			textToBlit = "";
	}

	void HellEngine::Quad2D::DrawTextBlit(Shader* shader)
	{
		// Have to hard code this coz otherwise the text is the same size in pixels regardless of resolution.
		float screenWidth = 1920;
		float screenHeight = 1080;

		// If you are actually blitting (a non -1 speed), and have already waited long enough, then don't draw shit
		if (blitSpeed != -1 && waitTimer > timeToWait)
			return;

		float charWidth = 16;
		float charHeight = 32;
		float textureWidth = 766 * 2;
		static float textScale = 0.75f;
		static float lineHeight = 40 / (screenHeight / 2);;
		static float cursor_X = 0;
		static float cursor_Y = 0;
		static float margin_X = -0.975f / textScale;
		static float margin_Y = 0.9f / textScale;

		std::vector<Vertex2D> vertices;

		float h = charHeight / (screenHeight / 2);
		float w = charWidth / (screenWidth / 2);

		size_t lastLineBreakIndex = 0;
		//bool centered;

		vertices.clear();

		// Find the length of the first line
		int lengthOfLine = textToBlit.find("_", 0);

		
		//If there are no lline breaks then it's the whole string
		if (lengthOfLine == -1)
			lengthOfLine = textToBlit.size();

		// Center the cursor
		if (centerText)
		{
			float centerPosition = 0 - ((lengthOfLine * w) * 0.5f);
			cursor_X = centerPosition;
			cursor_Y = -0.5f;
		}
		else
		{

			cursor_X = margin_X;
			cursor_Y = margin_Y;
		}


		// else use a margin

		for (int i = 0; i < currentCharIndex; i++)
		{
			char character = textToBlit[i];
			float totalWidth = textToBlit.find("_", lastLineBreakIndex) * w;

			// If there wasn't one, then it's simply the beginning of string
			if (lastLineBreakIndex == -1)
				lastLineBreakIndex = 0;

			// Check for line brak
			if (character == '_')
				{
					lastLineBreakIndex = i;
					std::string nextLine = textToBlit.substr(lastLineBreakIndex + 1, textToBlit.find('_', lastLineBreakIndex + 1) - i);
					lengthOfLine = nextLine.length() - 1;

					if (lengthOfLine == -1)
						lengthOfLine = textToBlit.size() - i;

					if (centerText)
						cursor_X = 0 - ((lengthOfLine * w) * 0.5f);
					else
						cursor_X = margin_X;
					
					cursor_Y -= lineHeight;
					continue;
				}



			int charPos = CharSheet.find(character);

			float tex_coord_L = (charWidth / textureWidth) * charPos;
			float tex_coord_R = (charWidth / textureWidth) * (charPos + 1);

			Vertex2D v1 = { glm::vec3(cursor_X, cursor_Y + h, 0), glm::vec2(tex_coord_L, 0) };
			Vertex2D v2 = { glm::vec3(cursor_X, cursor_Y, 0), glm::vec2(tex_coord_L, 1) };
			Vertex2D v3 = { glm::vec3(cursor_X + w, cursor_Y + h, 0), glm::vec2(tex_coord_R, 0) };
			Vertex2D v4 = { glm::vec3(cursor_X + w, cursor_Y, 0), glm::vec2(tex_coord_R, 1) };

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices.push_back(v4);

			cursor_X += w;
		}

		if (BLIT_VAO == 0)
		{
			glGenVertexArrays(1, &BLIT_VAO);
			glGenBuffers(1, &BLIT_VBO);
			std::cout << "Initilized Blitter.\n";

		}
		glBindVertexArray(BLIT_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, BLIT_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), &vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(textScale, textScale, 1));
		shader->setMat4("model", modelMatrix);

		glBindVertexArray(BLIT_VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
		glBindVertexArray(0);
	}

	void HellEngine::Quad2D::TypeText(std::string text, bool centered)
	{
		HellEngine::Quad2D::blitTimer = 0;
		HellEngine::Quad2D::waitTimer = 0;
		HellEngine::Quad2D::blitSpeed = 50;
		HellEngine::Quad2D::textToBlit = text;
		HellEngine::Quad2D::centerText = centered;
		timeToWait = 2;
	}

	void HellEngine::Quad2D::BlitText(std::string text, bool centered)
	{
		HellEngine::Quad2D::blitTimer = 0;
		HellEngine::Quad2D::waitTimer = 0; 
		HellEngine::Quad2D::blitSpeed = -1;
		HellEngine::Quad2D::textToBlit = text;
		HellEngine::Quad2D::centerText = centered;
		timeToWait = -1;
	}
}
