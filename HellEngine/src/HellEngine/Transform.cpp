#include "hellpch.h"
#include "Transform.h"

namespace HellEngine
{
	glm::mat4 Transform::to_mat4()
	{
		glm::mat4 m = glm::translate(glm::mat4(1), position);
		m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
		m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
		m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
		m = glm::scale(m, scale);
		return m;
	}
}