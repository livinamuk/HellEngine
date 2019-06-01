#include "hellpch.h"
#include "Skybox.h"

namespace HellEngine {

	unsigned int Skybox::VAO = -1;
	bool Skybox::initiated = false;

	Skybox::Skybox(float x, float y, float z)
	{
		if (!initiated)
			Init();

		position = glm::vec3(x, y, z);
		this->scale = glm::vec3(1);
		this->angle = 0;
	}

	Skybox::Skybox(float x, float y, float z, glm::vec3(scale))
	{
		if (!initiated)
			Init();

		position = glm::vec3(x, y, z);
		this->scale = scale;
		this->angle = 0;
	}

	Skybox::Skybox()
	{
	}

	Skybox::~Skybox()
	{
	}


	void Skybox::Draw(Shader *shader, bool bindTextures)
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}	

	//void Skybox::Draw(Shader shader, glm::vec3 cameraPosition)
	void Skybox::Draw(Shader *shader, Camera* camera)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		modelMatrix = glm::translate(modelMatrix, camera->Position);
		modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, scale);

		glm::mat4 viewMatrix = camera->viewMatrix * modelMatrix;
		viewMatrix = glm::mat4(glm::mat3(camera->viewMatrix));
		shader->setMat4("view", viewMatrix);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Skybox::Init()
	{
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		unsigned int skyboxVBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
}