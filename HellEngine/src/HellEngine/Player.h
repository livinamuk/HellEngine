#pragma once
#include "HellEngine/Components/House.h"
#include "Platform/OpenGL/BoundingPlane.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "HellEngine/Camera.h"
#include "HellEngine/Logic/Physics.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"

namespace HellEngine 
{
	class Player
	{
	public: // methods
		Player();
		~Player();
		void Update(Camera* camera, float deltaTime, vector<BoundingBox*> boundingBoxes, vector<BoundingPlane*> boundingPlanePtrs);


		float GetViewHeight();
		glm::vec3 GetViewPosition();
		bool IsGrounded();
		btRigidBody* rigidBody;

	private: // methods
		void CalculateIsRunning();
		void CalculateCurrentSpeed(float deltaTime);
		void UpdateCrouching(float deltaTime);
		void UpdateMovement(Camera* camera, float deltaTime, vector<BoundingBox*> boundingBoxes, vector<BoundingPlane*> boundingPlanePtrs);
		void UpdateGravity(float deltaTime);
		void UpdateAudio(float deltaTime);
		void CreateRigidBody();
		void UpdateRigidBodyWorldTransform();
		void CheckBulletCollisions();

		void UpdatePhysicsMovement(float deltaTime);

	public: // fields
		glm::vec3 position;
		float radius = 0.2f;// 0125f;
		float height = 0.01f;

		bool collision = false;

		bool NoClip = false;

		// Crouching
		bool crouching = false;
		float currentHeight;
		float crouchDownSpeed = 12;
		float couchingViewHeight = 1.1f;
		float standingViewHeight = 1.4f;

		// Movement
		float gravity = -0.3f;
		glm::vec3 currentVelocity = glm::vec3(0);
		glm::vec3 targetVelocity = glm::vec3(0);
		float velocityApproachSpeed = 1;
		bool isRunning = false;
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