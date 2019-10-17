#include "hellpch.h"
#include "Decal.h"
#include "Platform/OpenGL/Cube.h"
#include "HellEngine/Util.h"

namespace HellEngine
{
	Decal::Decal(DecalType decalType, glm::vec3 position, glm::vec3 normal)
	{
		this->decalType = decalType;
		this->transform.position = position;
		this->normal = normal * glm::vec3(-1);
		this->randomRotation = Util::RandomFloat(0, HELL_PI * 2);

		if (decalType == DecalType::BULLET_HOLE)
			transform.scale = glm::vec3(0.025f);
	}

	void Decal::Draw(Shader* shader, bool blackOnly)
	{
		// Calculate rotation
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform.position);
		glm::vec3 squareNormal = glm::vec3(0, 0, 1);
		float angle = glm::acos(glm::dot(normal, squareNormal));
		if (angle > 0.001f)
		{
			if (angle == HELL_PI)
				modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
			else {
				glm::vec3 axis = glm::normalize(glm::cross(squareNormal, normal));
				modelMatrix = glm::rotate(modelMatrix, angle, axis);
		 	}
		}
		// Draw the cunt
		modelMatrix = glm::rotate(modelMatrix, randomRotation, glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, transform.scale);
		shader->setMat4("model", modelMatrix);

		glActiveTexture(GL_TEXTURE1);
		if (!blackOnly)
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("BulletHole1.png"));
		else
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("BulletHole1_Black.png"));

		glBindVertexArray(Cube::VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

