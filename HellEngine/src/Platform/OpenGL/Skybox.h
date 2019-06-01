#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"
#include "HellEngine/Camera.h"

namespace HellEngine {

	class Skybox : public RenderingPrimitive
	{
	public:
		Skybox();
		Skybox(float x, float y, float z);
		Skybox(float x, float y, float z, glm::vec3 scale);
		virtual ~Skybox();

		void Draw(Shader *shader, bool bindTextures);
		//void Draw(Shader shader, glm::vec3 cameraPosition);
		void Draw(Shader *shader, Camera* camera);

		static void Init();
		static unsigned int VAO;

	private:
		static bool initiated;
	};
}

