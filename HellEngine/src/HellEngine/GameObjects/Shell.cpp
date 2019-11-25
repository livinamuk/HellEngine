#include "hellpch.h"
#include "Shell.h"
#include "HellEngine/Audio/Audio.h"

namespace HellEngine
{
	//Model* model = nullptr;

	float Shell::shellForwardFactor = 0.4f;
	float Shell::shellUpFactor = 0.15;
	float Shell::shellRightFactor = 0.225f;
	glm::vec3 Shell::shellVelocity = glm::vec3(0);
	float Shell::shellSpeedRight = 0.0175f; // * 0.17
	float Shell::shellSpeedUp = 0.0175f;
	float Shell::shellGravity = 0.19f;


	Shell::Shell(Model* model, glm::vec3 position, Camera* camera, float deltaTime)
	{
		this->model = model;
		transform.position = position;
		transform.rotation = camera->transform.rotation;

		modelTransform.rotation.y = ROTATE_180;
		modelTransform.scale = glm::vec3(0.01f);
		
		shellVelocity = camera->Up * shellSpeedUp;
		shellVelocity += camera->Right * shellSpeedRight ;

	}	

	void Shell::Update(float deltaTime)
	{
		if (activePhysics)
		{
			shellVelocity.y -= shellGravity * deltaTime;
			transform.position += shellVelocity;
		}

		if (transform.position.y < 0) 
		{
			Audio::PlayAudio("ShellBounce.wav"); 
			activePhysics = false;
			transform.position.y = 0.01f;
			shellVelocity = glm::vec3(0);
			transform.rotation.x = 0;
		}
	//	std::max(transform.position.y, 0.0f);
	}

	void Shell::Init()
	{
		float shellForwardFactor = 0.25f;
		float shellUpFactor = 0.1;
		float shellRightFactor = 0.1f;
		glm::vec3 shellVelocity = glm::vec3(0);
		float shellSpeedRight = 1; // * 0.17
		float shellSpeedUp = 1;
		float shellGravity = 0.1f;
	}
}

