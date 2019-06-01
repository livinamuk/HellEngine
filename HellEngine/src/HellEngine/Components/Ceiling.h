#pragma once
#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine
{
	class Ceiling : public RenderingPrimitive
	{
	public: // methods
		Ceiling();
		~Ceiling();
		Ceiling(glm::vec3 position, float width, float depth);
		void Draw(Shader *shader, bool bindTextures) override;

	public: // fields
		unsigned int VAO;
		float width;
		float depth;
	};
}