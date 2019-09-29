#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine {

	class Plane : public RenderingPrimitive
	{
	public: // methods
		Plane();
		Plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1));
		virtual ~Plane();

	public: // fields
		glm::vec3 position;
		float width, height, depth;

	//	glm::vec2 texScale;

		void Draw(Shader *shader, bool bindTextures) override;

		static void Init();
		static unsigned int VAO;

	private:
		static bool initiated;
	};
}

