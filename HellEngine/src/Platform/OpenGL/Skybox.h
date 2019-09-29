#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"
#include "HellEngine/Camera.h"

namespace HellEngine {

	class Skybox : public RenderingPrimitive
	{
	public:
		Skybox();
		Skybox(glm::vec3 position, glm::vec3 scale = glm::vec3(1));
		virtual ~Skybox();

		void Draw(Shader *shader, bool bindTextures);
		void Draw(Shader *shader, Camera* camera);

		static void Init();
		static unsigned int VAO;

	private:
		static bool initiated;
	};
}

