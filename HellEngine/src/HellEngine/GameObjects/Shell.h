#pragma once

#include "HellEngine/Transform.h"
#include "HellEngine/Camera.h"
#include "Platform/OpenGL/RenderableObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace HellEngine
{
	class Shell : public RenderableObject
	{
	public: // fields
		bool activePhysics = true;
		glm::vec3 shellPos;
		glm::vec3 finalShellPos;

		static float shellForwardFactor;
		static float shellUpFactor;
		static float shellRightFactor;
		static glm::vec3 shellVelocity;
		static float shellSpeedRight;
		static float shellSpeedUp;
		static float shellGravity;

	public: // methods
		Shell(Model* model, glm::vec3 position, Camera* camera, float deltaTime);
		void Update(float deltaTime);

		static void Init();
	};
}