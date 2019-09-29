#include "hellpch.h"
#include "Player.h"
#include "HellEngine/Input.h"
#include "HellEngine/Util.h"
#include "HellEngine/Audio/Audio.h"

namespace HellEngine
{
	/*float MovementSpeed = 2.25f;
	float viewHeight = 1.6f;
	float headBobCounter = 0.0f;
	float headBobSpeed = 0.3f;
	float headBobFactor = 0.0075f;*/
	
	const float COLLISION_RESPONSE_AMOUNT = 0.001f;

	Player::Player()
	{
		//position = glm::vec3(0.6f, 0, 0.1f);
		position = glm::vec3(3.5f, 0, 4.5f);
		srand(time(NULL));
	}


	Player::~Player()
	{
	}

	void Player::Update(Camera* camera, float deltaTime)
	{
		UpdateRunning();
		UpdateCurrentSpeed(deltaTime);
		UpdateMovement(camera, deltaTime);
		UpdateCrouching(deltaTime);
		UpdateGravity(deltaTime);
		UpdateAudio(deltaTime);
	}

	void Player::UpdateAudio(float deltaTime)
	{
		if (!walking)
			footstepAudioTimer = 0;
		else {
			if (walking && footstepAudioTimer == 0)	{
				int random_number = std::rand() % 4 + 1;
				std::string file = "player_step_" + std::to_string(random_number) + ".wav";
				Audio::PlayAudio(file);
			}			
			footstepAudioTimer += deltaTime;

			if (!running)
				footstepAudioLoopLength = 0.35f;
			else
				footstepAudioLoopLength = 0.25f;

			if (footstepAudioTimer > footstepAudioLoopLength)
				footstepAudioTimer = 0;
		}
	}

	void Player::UpdateGravity(float deltaTime)
	{
		targetVelocity.y += (gravity * deltaTime);
		//targetVelocity.y = std::max(targetVelocity.y, currentGroundHeight);

		if (IsGrounded())
		{
			position.y = std::max(position.y, currentGroundHeight);
			targetVelocity.y = 0;
		}
	}

	void Player::UpdateCurrentSpeed(float deltaTime)
	{
		if (!running)
			currentSpeed = walkingSpeed;
		else
			currentSpeed = runningSpeed;
		if (crouching)
			currentSpeed = crouchingSpeed;
	}

	void Player::UpdateRunning()
	{
		if (Input::IsKeyPressed(HELL_KEY_LEFT_SHIFT))
			running = true;
		if (Input::IsKeyReleased(HELL_KEY_LEFT_SHIFT))
			running = false;
		if (crouching)
			running = false;
	}

	void Player::UpdateMovement(Camera* camera, float deltaTime)
	{
		// Movement
		targetVelocity.x = 0;
		targetVelocity.z = 0;
		
		walking = false;
		
		if (Input::IsKeyPressed(HELL_KEY_W)) {
			ProcessKeyboard(Movement_Direction::FORWARD, camera, deltaTime);
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_S)) {
			ProcessKeyboard(Movement_Direction::BACKWARD, camera, deltaTime);
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_A)) {
			ProcessKeyboard(Movement_Direction::LEFT, camera, deltaTime);
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_D)) {
			ProcessKeyboard(Movement_Direction::RIGHT, camera, deltaTime);
			walking = true;
		}


		// Jump
		if ((IsGrounded()) && Input::IsKeyPressed(HELL_KEY_SPACE))
		//if (Input::IsKeyPressed(HELL_KEY_SPACE))
			targetVelocity.y += jumpStrength;

		// Lerp
		currentVelocity.x = Util::FInterpTo(currentVelocity.x, targetVelocity.x, deltaTime, velocityApproachSpeed);
		currentVelocity.y = Util::FInterpTo(currentVelocity.y, targetVelocity.y, deltaTime, velocityApproachSpeed);
		//currentVelocity.y = targetVelocity.y;
		currentVelocity.z = Util::FInterpTo(currentVelocity.z, targetVelocity.z, deltaTime, velocityApproachSpeed);
		position += currentVelocity;
	}

	bool Player::IsGrounded()
	{
		if (position.y <= currentGroundHeight + KINDA_SMALL_NUMBER)
			return true;
		else 
			return false;
	}

	void Player::ProcessKeyboard(Movement_Direction direction, Camera* camera, float deltaTime)
	{
		// BUG SOMEHOW??????????? u reverssed the movement diretions to fix it.
		glm::vec3 Forward = glm::normalize(glm::vec3(camera->Front.x, 0, camera->Front.z));
		if (direction == BACKWARD)
			targetVelocity += Forward * currentSpeed * deltaTime;;
		if (direction == FORWARD)
			targetVelocity -= Forward * currentSpeed * deltaTime;;
		if (direction == LEFT)
			targetVelocity -= camera->Right * currentSpeed * deltaTime;;
		if (direction == RIGHT)
			targetVelocity += camera->Right * currentSpeed * deltaTime;;
	}

	void Player::UpdateCrouching(float deltaTime)
	{
		if (HellEngine::Input::IsKeyPressed(HELL_KEY_LEFT_CONTROL))
			crouching = true;
		if (HellEngine::Input::IsKeyReleased(HELL_KEY_LEFT_CONTROL))
			crouching = false;

		float target = crouching ? couchingViewHeight : standingViewHeight;
		currentHeight = Util::FInterpTo(currentHeight, target, deltaTime, crouchDownSpeed);
	}

	float Player::GetViewHeight()
	{
		return currentHeight;
	}

	glm::vec3 Player::GetViewPosition()
	{
		return glm::vec3(position.x, position.y + GetViewHeight(), position.z);
	}


	//https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection

	void Player::ProcessCollisions(House* house)
	{
		
		float w = 0.5f; // Half the walls width
		float collisionResponseAmount = 0.0001f;

		return;
	}

	void Player::HandleBoundingBoxCollisions(vector<BoundingBox*> boundingBoxPtrs)
	{
		for (BoundingBox* & b : boundingBoxPtrs)
			HandleBoundingBoxCollision(b);
	}

	void Player::HandleBoundingPlaneCollisions(vector<BoundingPlane*> boundingPlanePtrs)
	{
		for (BoundingPlane* & plane : boundingPlanePtrs)
		{
			if (plane->testCollisions)
				while (Util::CircleIntersectsLine(position, plane->A, plane->C, radius))
					position += plane->normal * COLLISION_RESPONSE_AMOUNT;
		}
	}
	
	void Player::HandleBoundingBoxCollision(BoundingBox* box)
	{
		bool checkFront = true;
		bool checkBack = true;
		bool checkLeft = true;
		bool checkRight = true;

		while (checkFront || checkBack || checkLeft || checkRight)
		{
			checkFront = Util::CircleIntersectsLine(position, box->frontPlane.A, box->frontPlane.C, radius);
			if (checkFront)
				position += box->frontPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkBack = Util::CircleIntersectsLine(position, box->backPlane.A, box->backPlane.C, radius);
			if (checkBack)
				position += box->backPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkLeft = Util::CircleIntersectsLine(position, box->leftPlane.A, box->leftPlane.C, radius);
			if (checkLeft)
					position += box->leftPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkRight = Util::CircleIntersectsLine(position, box->rightPlane.A, box->rightPlane.C, radius);
			if (checkRight)
				position += box->rightPlane.normal * COLLISION_RESPONSE_AMOUNT;
		}
	}

	/*void Player::ProcessCollisions(BoundingPlane* plane)
	{
		float collisionResponseAmount = 0.001f;

			while (Util::CircleIntersectsLine(position, plane->A, plane->C, radius))
				position += plane->normal * collisionResponseAmount;
	}*/
}