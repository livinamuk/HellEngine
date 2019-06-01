#pragma once

#include "Platform/OpenGL/ShadowCubeMapArray.h"

namespace HellEngine
{
	class Light
	{
	public: // methods
		Light();
		Light(float x, float y, float z);
		~Light();
		void CalculateShadowProjectionMatricies();
		void CalculateRadius();

	public: // fields
		glm::vec3 position;
		glm::vec3 color; 
		std::vector<glm::mat4> shadowTransforms;
		//float strength;
		float radius;
		float strength;
		float attConstant;
		float attLinear;
		float attExp;

		float lowerXlimit;
		float upperXlimit;
		float lowerZlimit;
		float upperZlimit;

	public: // static
		static void Light::AddLight(float x, float y, float z);
		static std::vector<Light*>lights;
	};
}
