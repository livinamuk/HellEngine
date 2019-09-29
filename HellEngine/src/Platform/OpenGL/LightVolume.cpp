#include "hellpch.h"
#include "LightVolume.h"

namespace HellEngine
{
	LightVolume::LightVolume()
	{
	}

	LightVolume::LightVolume(float lowerX, float lowerZ, float upperX, float upperZ)
	{
		float bias = 0;// 0.0125f;
		
		glm::vec3 A1 = glm::vec3(lowerX - bias, -bias, lowerZ - bias);
		glm::vec3 B1 = glm::vec3(upperX + bias, -bias, lowerZ - bias);
		glm::vec3 C1 = glm::vec3(lowerX - bias, -bias, upperZ + bias);
		glm::vec3 D1 = glm::vec3(upperX + bias, -bias, upperZ + bias);

		glm::vec3 A2 = glm::vec3(lowerX - bias, 2.4f + bias, lowerZ - bias);
		glm::vec3 B2 = glm::vec3(upperX + bias, 2.4f + bias, lowerZ - bias);
		glm::vec3 C2 = glm::vec3(lowerX - bias, 2.4f + bias, upperZ + bias);
		glm::vec3 D2 = glm::vec3(upperX + bias, 2.4f + bias, upperZ + bias);

		// Floor
		vertices.push_back(A1);
		vertices.push_back(B1);
		vertices.push_back(C1);

		vertices.push_back(D1);
		vertices.push_back(C1);
		vertices.push_back(B1);

		// Ceiling
		vertices.push_back(C2);
		vertices.push_back(B2);
		vertices.push_back(A2);

		vertices.push_back(B2);
		vertices.push_back(C2);
		vertices.push_back(D2);

		// Side wall
		vertices.push_back(C1);
		vertices.push_back(A2);
		vertices.push_back(A1);

		vertices.push_back(C2);
		vertices.push_back(A2);
		vertices.push_back(C1);

		// Side wall the other one
		vertices.push_back(B1);
		vertices.push_back(B2);
		vertices.push_back(D1);

		vertices.push_back(D1);
		vertices.push_back(B2);
		vertices.push_back(D2);

		// Front wall
		vertices.push_back(A1);
		vertices.push_back(B2);
		vertices.push_back(B1);

		vertices.push_back(A1);
		vertices.push_back(A2);
		vertices.push_back(B2);

		// Back wall
		vertices.push_back(D1);
		vertices.push_back(D2);
		vertices.push_back(C1);

		vertices.push_back(D2);
		vertices.push_back(C2);
		vertices.push_back(C1);


		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	LightVolume::LightVolume(DoorHole* doorHole, Light light)
	{
		glm::vec3 origin = light.position;

		glm::vec3 direction = normalize(doorHole->bottomLeft - origin);
		glm::vec3 A = origin + (direction * light.radius);
		direction = normalize(doorHole->bottomRight - origin);
		glm::vec3 B = origin + (direction * light.radius);
		direction = normalize(doorHole->topLeft - origin);
		glm::vec3 D = origin + (direction * light.radius);
		direction = normalize(doorHole->topRight - origin);
		glm::vec3 C = origin + (direction * light.radius);

		vertices.push_back(origin);
		vertices.push_back(A);
		vertices.push_back(B);

		vertices.push_back(origin);
		vertices.push_back(D);
		vertices.push_back(A);

		vertices.push_back(origin);
		vertices.push_back(B);
		vertices.push_back(C);

		vertices.push_back(origin);
		vertices.push_back(C);
		vertices.push_back(D);

		vertices.push_back(C);
		vertices.push_back(B);
		vertices.push_back(A);

		vertices.push_back(A);
		vertices.push_back(D);
		vertices.push_back(C);

		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	void LightVolume::Draw(Shader *shader, GLenum mode)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::translate(modelMatrix, position);
		//modelMatrix = glm::scale(modelMatrix, scale);
		shader->setMat4("model", modelMatrix);
		glBindVertexArray(VAO);
		//glDrawArrays(mode, 0, vertices.size());
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDrawArrays(GL_POINTS, 0, vertices.size());
	}

	void LightVolume::Setup()
	{
		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	LightVolume LightVolume::BuildLightVolumeFromDoorHole(DoorHole doorHole, Light light, float floorHeight, float bias)
	{
		LightVolume lightVolume;
		glm::vec3 origin = light.position;

		glm::vec3 direction = normalize(doorHole.bottomLeft - origin);
		glm::vec3 A = origin + (direction * light.radius);
		direction = normalize(doorHole.bottomRight - origin);
		glm::vec3 B = origin + (direction * light.radius);
		direction = normalize(doorHole.topLeft - origin);
		glm::vec3 D = origin + (direction * light.radius);
		direction = normalize(doorHole.topRight - origin);
		glm::vec3 C = origin + (direction * light.radius);

		// fix limits
	//	A.y = std::max(A.y, floorHeight - bias);
	//	B.y = std::max(B.y, floorHeight - bias);

		// Extend bottom to match the reach of the top. Hopefully the sphere still includes this??
		//A.x = C.x;
		//B.z = D.z;
		// Nope it isn't that simple. But have a think here, it's doable.

		// Bottom
		lightVolume.vertices.push_back(B);
		lightVolume.vertices.push_back(A);
		lightVolume.vertices.push_back(doorHole.bottomLeft);

		lightVolume.vertices.push_back(B);
		lightVolume.vertices.push_back(doorHole.bottomLeft);
		lightVolume.vertices.push_back(doorHole.bottomRight);

		// Left side
		lightVolume.vertices.push_back(doorHole.topLeft);
		lightVolume.vertices.push_back(doorHole.bottomLeft);
		lightVolume.vertices.push_back(A);

		lightVolume.vertices.push_back(doorHole.topLeft);
		lightVolume.vertices.push_back(A);
		lightVolume.vertices.push_back(D);

		// Right side
		lightVolume.vertices.push_back(B);
		lightVolume.vertices.push_back(doorHole.bottomRight);
		lightVolume.vertices.push_back(doorHole.topRight);

		lightVolume.vertices.push_back(C);
		lightVolume.vertices.push_back(B);
		lightVolume.vertices.push_back(doorHole.topRight);

		// Endcap
		lightVolume.vertices.push_back(A);
		lightVolume.vertices.push_back(B);
		lightVolume.vertices.push_back(C);

		lightVolume.vertices.push_back(C);
		lightVolume.vertices.push_back(D);
		lightVolume.vertices.push_back(A);

		// Front cap (towards room)
		lightVolume.vertices.push_back(doorHole.bottomRight);
		lightVolume.vertices.push_back(doorHole.bottomLeft);
		lightVolume.vertices.push_back(doorHole.topRight);

		lightVolume.vertices.push_back(doorHole.bottomLeft);
		lightVolume.vertices.push_back(doorHole.topLeft);
		lightVolume.vertices.push_back(doorHole.topRight);

		// Top
		lightVolume.vertices.push_back(doorHole.topRight);
		lightVolume.vertices.push_back(doorHole.topLeft);
		lightVolume.vertices.push_back(C);

		lightVolume.vertices.push_back(doorHole.topLeft);
		lightVolume.vertices.push_back(D);
		lightVolume.vertices.push_back(C);

		// RETURN!
		lightVolume.Setup();
		return lightVolume;
	}

	LightVolume::~LightVolume()
	{
	}
}