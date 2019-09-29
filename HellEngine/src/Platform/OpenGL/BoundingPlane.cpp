#include "hellpch.h"
#include "BoundingPlane.h"

namespace HellEngine {



	BoundingPlane::BoundingPlane()
	{
	}

	BoundingPlane::BoundingPlane(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, bool testCollisions)
	{
		if (VAO = -1)
			glGenVertexArrays(1, &VAO);

		this->A = A;
		this->B = B;
		this->C = C;
		this->D = D;
		//this->D.z += 1;
		//this->C.z += 1;
		this->testCollisions = testCollisions;

		BufferData();

		this->angle = angle;

		Recalculate_ABC_and_Normal();
	}

	BoundingPlane::~BoundingPlane()
	{
	}

	void BoundingPlane::Draw(Shader *shader)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::translate(modelMatrix, position);
		//modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		shader->setMat4("model", modelMatrix);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}

	void BoundingPlane::SetAngle(float angle)
	{
		this->angle = angle;
		Recalculate_ABC_and_Normal();
	}

	float BoundingPlane::GetAngle()
	{
		return angle;
	}

	void BoundingPlane::BufferData()
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);

		glm::vec3 vertices[] = { A, B, C, D };

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//glDeleteBuffers(1, &VBO);	 causes access violation crash	
	}

	void BoundingPlane::Recalculate_ABC_and_Normal()
	{
		/*glm::mat4 model = glm::mat4(1.0f);;
		model = glm::translate(model, position);
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		A = glm::vec3(model * glm::vec4(vertices[0], 1));
		B = glm::vec3(model * glm::vec4(vertices[1], 1));
		C = glm::vec3(model * glm::vec4(vertices[2], 1));
		*/
		// Get plane normal
		glm::vec3 v1 = A - B;
		glm::vec3 v2 = C - B;
		normal = glm::normalize(glm::cross(v1, v2));
	}
}