#include "hellpch.h"
#include "Plane.h"

namespace HellEngine {

	unsigned int Plane::VAO = -1;
	bool Plane::initiated = false;

	Plane::Plane(float x, float y, float z)
	{
		if (!initiated)
			Init();

		position = glm::vec3(x, y, z);
	}

	Plane::Plane(float x, float y, float z, glm::vec3(scale))
	{
		if (!initiated)
			Init();

		position = glm::vec3(x, y, z);
		this->scale = scale;
	}

	Plane::Plane()
	{
	}

	Plane::Plane(glm::vec3 position, float width, float depth, float height)
	{
		
	}

	Plane::~Plane()
	{
	}


	void Plane::Draw(Shader *shader, bool bindTextures)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, scale);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// SECOND PLANE (FLOOR)			
		glm::vec3 floorPosition = glm::vec3(0);
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, floorPosition);
		modelMatrix = glm::scale(modelMatrix, scale);
		modelMatrix = glm::rotate(modelMatrix, ROTATE_180, glm::vec3(0.0f, 0.0f, 1.0f));
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}

	void Plane::Init()
	{
		const float SCALE_OVERRIDE = 20;
		glm::vec2 texScale = glm::vec2(SCALE_OVERRIDE, SCALE_OVERRIDE);

		float vertices[] = {
			// positions          // normals           // texture coords	
			-0.5f,  0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  1.0f * texScale.y,
			 0.5f,  0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,  1.0f * texScale.x,  1.0f * texScale.y,
			 0.5f,  0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f * texScale.x,  0.0f * texScale.y,
			 0.5f,  0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f * texScale.x,  0.0f * texScale.y,
			-0.5f,  0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  0.0f * texScale.y,
			-0.5f,  0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,  0.0f * texScale.x,  1.0f * texScale.y
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

		//glDeleteBuffers(1, &VBO);	 causes access violation crash	
	}
}