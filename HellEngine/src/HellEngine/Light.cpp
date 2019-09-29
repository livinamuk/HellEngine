#include "hellpch.h"
#include "Light.h"

namespace HellEngine
{
	std::vector<Light*> Light::lights;

	Light::Light()
	{
	}

	Light::Light(float x, float y, float z)
	{
		position = glm::vec3(x, y, z);
		color = glm::vec3(1.0f, 0.85f, 0.69f);
		attConstant = 1.0f;
		attLinear = 0.7f;
		attExp = 1.8f;
		strength = 8.0f;

		//attConstant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		//attLinear = 0.7;
		//attExp = 1.8;
		CalculateShadowProjectionMatricies();
	}

	Light::Light(glm::vec3 position, glm::vec3 color, float attConstant, float attLinear, float attExp, float strength)
	{
		this->position = position;
		this->color = color;
		this->attConstant = attConstant;
		this->attLinear = attLinear;
		this->attExp = attExp;
		this->strength = strength;
		CalculateShadowProjectionMatricies();
	}

	Light::~Light()
	{
	}

	void Light::CalculateShadowProjectionMatricies()
	{
		shadowTransforms.clear();
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)ShadowCubeMapArray::SHADOW_SIZE / (float)ShadowCubeMapArray::SHADOW_SIZE, ShadowCubeMapArray::SHADOW_NEAR_PLANE, ShadowCubeMapArray::SHADOW_FAR_PLANE);
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	}

	void Light::CalculateRadius()
	{
		const float maxBrightness = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
		radius = (-attLinear + std::sqrt(attLinear * attLinear - 4 * attExp * (attConstant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * attExp);
	}


	void Light::AddLight(float x, float y, float z)
	{
		lights.push_back(&Light(x, y, z));
	}
}