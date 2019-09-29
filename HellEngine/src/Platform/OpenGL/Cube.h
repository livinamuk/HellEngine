#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine {

	class Cube : public RenderingPrimitive
	{
	public:
		Cube();
		Cube(glm::vec3 position, glm::vec3 scale = glm::vec3(1));
		virtual ~Cube();

		void Draw(Shader *shader, bool bindTextures) override;
		void DecalDraw(Shader* shader, glm::vec3 normal);

		static void Init();
		static unsigned int VAO;

	private:
		static bool initiated;
	};
}

