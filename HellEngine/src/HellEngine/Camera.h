#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

#include <vector>

#include "HellEngine/Input.h"
#include "HellEngine/Application.h"
#include "HellEngine/Util.h"
#include "HellEngine/Transform.h"

namespace HellEngine {

	// Default camera values
	const float YAW = 90;// 270.0f;
	const float PITCH = -6.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.2f;
	const float ZOOM = 45.0f;



	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	public:

		float xoffset, yoffset;

		// Camera Attributes
	//	glm::vec3 Position;
		//glm::vec3 Rotation  = glm::vec3(-0.13f, ROTATE_180, 0);
		//glm::vec3 Scale = glm::vec3(1);
		Transform transform;

		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		glm::vec3 VelocityGoal;
		glm::vec3 Movement;

		glm::mat4 projectionViewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		// Euler Angles
		float TargetYaw;
		float CurrentYaw;
		float TargetPitch;
		float CurrentPitch;

		// Camera options
		float MouseSensitivity;
		float Zoom;

		float oldX;
		float oldY;
		bool firstMouse = true;

		float headBobCounter = 0.0f;
		float headBobSpeed = 7.5f;//0.100f;
		float headBobFactor = 0.02f;// 0.005f;

		float rotationSpeed = 90.0f;

		Camera()
		{
			transform.position = glm::vec3(0);
			transform.rotation = glm::vec3(-0.13f, ROTATE_180, 0);
			transform.scale = glm::vec3(1);

			auto [xpos, ypos] = HellEngine::Input::GetMousePosition();
			oldX = (float)xpos;
			oldY = (float)ypos;
		}

		float CalculateHeadBob()
		{
			return (float)sin(headBobCounter) * headBobFactor;
		}

		void CalculateviewPosition(glm::vec3 playerViewHeight)
		{
			glm::vec3 p = playerViewHeight;
			//p.y = CalculateHeadBob();
			transform.position = p;
		}

		void CalculateMatrices()
		{
			glm::mat4 transform = this->transform.to_mat4();

			viewMatrix = glm::inverse(transform);
			projectionViewMatrix = projectionMatrix * viewMatrix;

			Right = glm::vec3(transform[0]);
			Up = glm::vec3(transform[1]);
			Front = glm::vec3(transform[2]);
		}

		/*void CalculateVectors()
		{
			glm::mat4 transform = this->transform.to_mat4();
			Right = glm::vec3(transform[0]);
			Up = glm::vec3(transform[1]);
			Front = glm::vec3(transform[2]);
			viewMatrix = glm::inverse(transform);
		}*/

		void CalculateProjectionMatrix(int screenWidth, int screenHeight)
		{
			projectionMatrix = glm::perspective(1.00f, (float)screenWidth / (float)screenHeight, 0.02f, 100.0f);
		}

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			HellEngine::Application& app = HellEngine::Application::Get();
			if (app.GetWindow().IsMouseEnabled())
				return;

			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			TargetYaw += xoffset;
			TargetPitch += yoffset;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (TargetPitch > 89.0f)
					TargetPitch = 89.0f;
				if (TargetPitch < -89.0f)
					TargetPitch = -89.0f;
			}
		}

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset)
		{
			if (Zoom >= 1.0f && Zoom <= 45.0f)
				Zoom -= yoffset;
			if (Zoom <= 1.0f)
				Zoom = 1.0f;
			if (Zoom >= 45.0f)
				Zoom = 45.0f;
		}

		void Update(float deltaTime)
		{
			// Mouse
			auto[xpos, ypos] = HellEngine::Input::GetMousePosition();
			xoffset = (float)xpos - oldX;
			yoffset = oldY - (float)ypos;
			oldX = (float)xpos;
			oldY = (float)ypos;

			HellEngine::Application& app = HellEngine::Application::Get();
			if (app.GetWindow().IsMouseEnabled())
				return;

			float yLimit = 1.5f;
			transform.rotation += glm::vec3(yoffset, -xoffset, 0.0) / glm::vec3(200);
			transform.rotation.x = min(transform.rotation.x, yLimit);
			transform.rotation.x = max(transform.rotation.x, -yLimit);

			// Headbob
			headBobCounter += headBobSpeed * deltaTime;
		}

		float SimplexHeadBob()
		{
			return (float)glm::simplex(glm::vec3(glm::gaussRand(0.051f, 0.85f), glm::gaussRand(0.073f, 0.99f), glm::gaussRand(0.051f, 0.63f)));
		}

		float SmoothHeadBob()
		{
			return (float)(cos(headBobCounter) * headBobFactor);
		}
	};
}