#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine {

	class Cube : public RenderingPrimitive
	{
	public:
		Cube();
		Cube(float x, float y, float z);
		Cube(float x, float y, float z, glm::vec3 scale);
		virtual ~Cube();

		void Draw(Shader *shader, bool bindTextures) override;

		static void Init();
		static unsigned int VAO;

	private:
		static bool initiated;
	};
}

