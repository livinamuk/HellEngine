#pragma once

#include "Glad/glad.h"
#include "GLFW/glfw3.h"
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Transform.h"

namespace HellEngine {

	class RenderingPrimitive
	{
	public:

		virtual ~RenderingPrimitive() {}		
		virtual void Draw(Shader *shader, bool bindTextures) = 0;

		Transform transform;
	};
}