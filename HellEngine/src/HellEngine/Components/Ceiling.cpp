#include "hellpch.h"
#include "Ceiling.h"

namespace HellEngine
{
	Ceiling::Ceiling()
	{
	}

	Ceiling::Ceiling(glm::vec3 position, float width, float depth)
	{
		this->position = position;
		this->width = width;
		this->depth = depth;

		float x1 = position.x;
		float x2 = position.x + width;
		float y = position.y + 2.4f;
		float z1 = position.z;
		float z2 = position.z + depth;

		glm::vec2 texScale = glm::vec2(1, 1);

		float vertices[] = {
			// positions          // normals           // texture coords	
			 x1,  y,  z1,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  depth * texScale.y,
			 x2,  y,  z1,  0.0f,  -1.0f,  0.0f,  width * texScale.x,  depth * texScale.y,
			 x2,  y,  z2,  0.0f,  -1.0f,  0.0f,  width * texScale.x,  0.0f * texScale.y,
			 x2,  y,  z2,  0.0f,  -1.0f,  0.0f,  width * texScale.x,  0.0f * texScale.y,
			 x1,  y,  z2,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  0.0f * texScale.y,
			 x1,  y,  z1,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  depth * texScale.y
		};

		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	void Ceiling::Draw(Shader *shader, bool bindTextures)
	{

		//scale = glm::vec3(10, 1, 10);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::translate(modelMatrix, position);
		//modelMatrix = glm::scale(modelMatrix, scale);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	Ceiling::~Ceiling()
	{
	}
}