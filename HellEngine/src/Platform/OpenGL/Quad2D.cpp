#include "hellpch.h"
#include "Quad2D.h"

namespace HellEngine
{
	unsigned int HellEngine::Quad2D::VAO = 0;

	void HellEngine::Quad2D::RenderQuad(Shader *shader)
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


	void HellEngine::Quad2D::RenderCrosshair(Shader *shader, int screenWidth, int screenHeight, int crosshairSize)
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
}
