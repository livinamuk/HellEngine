#pragma once

#include "Glad/glad.h"
#include "GLFW/glfw3.h"
#include "Platform/OpenGL/Shader.h"

namespace HellEngine {

	class RenderingPrimitive
	{
	public:

		virtual ~RenderingPrimitive() {}		
		virtual void Draw(Shader *shader, bool bindTextures) = 0;

		glm::vec3 position;
		glm::vec3 scale;
		float angle;
	};
}