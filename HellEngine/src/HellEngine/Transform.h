#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace HellEngine
{
	class Transform
	{
	public: // fields
		glm::vec3 position = glm::vec3(0);
		glm::vec3 rotation = glm::vec3(0);
		glm::vec3 scale = glm::vec3(1);

	public: // methods
		glm::mat4 to_mat4();
	};
}