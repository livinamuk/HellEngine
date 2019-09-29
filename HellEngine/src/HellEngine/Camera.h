#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "HellEngine/Input.h"
#include "HellEngine/Application.h"
#include "HellEngine/Util.h"

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
		glm::vec3 Position;
		glm::vec3 Rotation  = glm::vec3(0);;
		glm::vec3 Scale = glm::vec3(1);
		glm::mat4 Transform;

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

		float lastX;
		float lastY;
		bool firstMouse = true;

		float headBobCounter = 0.0f;
		float headBobSpeed = 7.5f;//0.100f;
		float headBobFactor = 0.01f;// 0.005f;

		float rotationSpeed = 90.0f;


		// Constructor with vectors
		Camera(glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			//Position = position;
			WorldUp = up;
			TargetYaw = yaw;
			TargetPitch = pitch;
			CurrentYaw = yaw;
			CurrentPitch = pitch;
			//position.y = viewHeight;
		}
		// Constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			TargetYaw = yaw;
			TargetPitch = pitch;
			CurrentYaw = yaw;
			CurrentPitch = pitch;
		}

		float CalculateHeadBob()
		{
			return (float)sin(headBobCounter) * headBobFactor;
		}

		void CalculateviewPosition(glm::vec3 playerViewHeight)
		{
			glm::vec3 p = playerViewHeight;
			//p.y = CalculateHeadBob();
			this->Position = p;
		}

		// Calculates the view matrix calculated using Euler Angles and the LookAt Matrix
		void CalculateMatrices()
		{
			viewMatrix = glm::lookAt(this->Position, this->Position + Front, Up);

			Transform = glm::translate(glm::mat4(1), Position);
			Transform = glm::rotate(Transform, Rotation.z, glm::vec3(0, 0, 1));
			Transform = glm::rotate(Transform, Rotation.y, glm::vec3(0, 1, 0));
			Transform = glm::rotate(Transform, Rotation.x, glm::vec3(1, 0, 0));
			Transform = glm::scale(Transform, Scale);

			viewMatrix = glm::inverse(Transform);

			projectionViewMatrix = projectionMatrix * viewMatrix;

			Right = glm::vec3(Transform[0]);
			Up = glm::vec3(Transform[1]);
			Front = glm::vec3(Transform[2]);
		}

		void CalculateProjectionMatrix(int screenWidth, int screenHeight)
		{
			//projectionMatrix = glm::perspective(glm::radians(Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
			projectionMatrix = glm::perspective(1.00f, (float)screenWidth / (float)screenHeight, 0.02f, 100.0f);

			//HellEngine::Util::printMat4(projectionMatrix);
		}

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
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

		void updateMouse(double xpos, double ypos)
		{
			if (firstMouse)	{
				lastX = (float)xpos;
				lastY = (float)ypos;
				firstMouse = false;
			}
		xoffset = (float)xpos - lastX;
		yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top
		lastX = (float)xpos;
		lastY = (float)ypos;
		//ProcessMouseMovement(xoffset, yoffset);

		HellEngine::Application& app = HellEngine::Application::Get();
		if (app.GetWindow().IsMouseEnabled())
			return;

		Rotation += glm::vec3(yoffset, -xoffset, 0.0) / glm::vec3(200);
		}

		void Update(float deltaTime)
		{
			// Mouse
			auto[x, y] = HellEngine::Input::GetMousePosition();
			this->updateMouse(x, y);

			headBobCounter += headBobSpeed * deltaTime;

			// Update Rotation Interpolation
			UpdateRotation(deltaTime);
		}

		void UpdateRotation(float deltaTime)
		{
			CurrentYaw = Util::FInterpTo(CurrentYaw, TargetYaw, deltaTime, rotationSpeed);
			CurrentPitch = Util::FInterpTo(CurrentPitch, TargetPitch, deltaTime, rotationSpeed);
		}
	};
}