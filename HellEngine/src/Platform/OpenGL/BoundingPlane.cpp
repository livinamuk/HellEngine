#include "hellpch.h"
#include "BoundingPlane.h"

namespace HellEngine 
{
	BoundingPlane::BoundingPlane() {}
	BoundingPlane::~BoundingPlane() {}

	BoundingPlane::BoundingPlane(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, bool testCollisions, std::string name)
	{
		this->A = A;
		this->B = B;
		this->C = C;
		this->D = D;
		this->testCollisions = testCollisions;
		this->name = name;
	}


	void BoundingPlane::Draw(Shader* shader)
	{
		if (VAO == 0)
			BufferData();

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}

	void BoundingPlane::DrawSolid(Shader* shader)
	{
		if (VAO == 0)
			BufferData();

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void BoundingPlane::BufferData()
	{
		//std::cout << "hi\n";

		glGenVertexArrays(1, &VAO);
		
		//std::cout << "VAO: " << VAO << "\n";

		unsigned int VBO;
		glGenBuffers(1, &VBO);

		glm::vec3 vertices[] = { A, B, C, D };

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
}