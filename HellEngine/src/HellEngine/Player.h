#pragma once
#include "HellEngine/Components/House.h"
#include "Platform/OpenGL/BoundingPlane.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "HellEngine/Camera.h"


namespace HellEngine 
{
	class Player
	{
	public: // methods
		Player();
		~Player();
		void Update(Camera* camera, float deltaTime);

		void ProcessCollisions(House* house);

		void HandleBoundingBoxCollisions(vector<BoundingBox*> boundingBoxes);
		void HandleBoundingPlaneCollisions(vector<BoundingPlane*> boundingPlanePtrs);
		void HandleBoundingBoxCollision(BoundingBox* box);
		float GetViewHeight();
		glm::vec3 GetViewPosition();
		bool IsGrounded();

	private: // methods
		void UpdateRunning();
		void UpdateCurrentSpeed(float deltaTime);
		void UpdateCrouching(float deltaTime);
		void UpdateMovement(Camera* camera, float deltaTime);
		void ProcessKeyboard(Movement_Direction direction, Camera* camera, float deltaTime);
		void UpdateGravity(float deltaTime);
		void UpdateAudio(float deltaTime);

	public: // fields
		glm::vec3 position;
		float radius = 0.2f;// 0125f;

		// Crouching
		bool crouching = false;
		float currentHeight;
		float crouchDownSpeed = 12;
		float couchingViewHeight = 1.1f;
		float standingViewHeight = 1.6f;

		// Movement
		float gravity = -0.3f;
		glm::vec3 currentVelocity = glm::vec3(0);
		glm::vec3 targetVelocity = glm::vec3(0);
		float velocityApproachSpeed = 20;
		bool running = false;
		float currentSpeed;
		float walkingSpeed = 2.25;
		float runningSpeed = 3.25f;
		float crouchingSpeed = 1.75f;
		float jumpStrength = 0.05f;
		float currentGroundHeight = 0;

		bool walking;
		float footstepAudioTimer;
		float footstepAudioLoopLength = 0.35f;
	};
}