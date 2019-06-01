#pragma once
#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine
{
	class Floor : public RenderingPrimitive
	{
	public: // methods
		Floor();
		~Floor();
		Floor(glm::vec3 position, float width, float depth);
		void Draw(Shader *shader, bool bindTextures) override;

	public: // fields
		unsigned int VAO;
		float width;
		float depth;
	};
}