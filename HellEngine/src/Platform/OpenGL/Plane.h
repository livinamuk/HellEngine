#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine {

	class Plane : public RenderingPrimitive
	{
	public: // methods
		Plane();
		Plane(float x, float y, float z);
		Plane(float x, float y, float z, glm::vec3 scale);

		Plane(glm::vec3 position, float width, float depth, float height);
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

