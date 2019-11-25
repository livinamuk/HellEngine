#include "hellpch.h"
#include "Player.h"
#include "HellEngine/Input.h"
#include "HellEngine/Util.h"
#include "HellEngine/Audio/Audio.h"

namespace HellEngine
{
	const glm::vec3 INITIAL_POSITION = glm::vec3(0.6f, 0.3f, -0.6f);

	Player::Player()
	{
		position = INITIAL_POSITION;
		srand(time(NULL));
		CreateRigidBody();
	}

	Player::~Player()
	{
	}

	void Player::CreateRigidBody()
	{
		// Setup
		btCapsuleShape* collisionShape = new btCapsuleShape(radius, height);
		collisionShape->setLocalScaling(btVector3(1, 1, 1));

		// Position
		btTransform btTransform;
		btTransform.setIdentity();
		btTransform.setOrigin(btVector3(Util::glmVec3_to_btVec3(INITIAL_POSITION)));

		// Create it
		rigidBody = Physics::createRigidBody(1.0, btTransform, collisionShape, 0.1);
	}

	void Player::Update(Camera* camera, Physics* physics, float deltaTime)
	{
		CalculateIsRunning();
		CalculateCurrentSpeed(deltaTime);
		UpdatePhysicsMovement(deltaTime);
		UpdateMovement(camera, deltaTime);
		UpdateCrouching(deltaTime);
		UpdateAudio(deltaTime);
	}

	void Player::UpdateAudio(float deltaTime)
	{
		if (!isMoving)
			footstepAudioTimer = 0;
		else {
			if (isMoving && footstepAudioTimer == 0) {
				int random_number = std::rand() % 4 + 1;
				std::string file = "player_step_" + std::to_string(random_number) + ".wav";
				Audio::PlayAudio(file);
			}
			footstepAudioTimer += deltaTime;

			if (!isRunning)
				footstepAudioLoopLength = 0.35f;
			else
				footstepAudioLoopLength = 0.25f;

			if (footstepAudioTimer > footstepAudioLoopLength)
				footstepAudioTimer = 0;
		}
	}

	void Player::CalculateCurrentSpeed(float deltaTime)
	{
		if (!isRunning)
			currentSpeed = walkingSpeed;
		else
			currentSpeed = runningSpeed;
		if (crouching)
			currentSpeed = crouchingSpeed;
	}

	void Player::CalculateIsRunning()
	{
		if (Input::IsKeyPressed(HELL_KEY_LEFT_SHIFT))
			isRunning = true;
		if (Input::IsKeyReleased(HELL_KEY_LEFT_SHIFT))
			isRunning = false;
		if (crouching)
			isRunning = false;
	}

	void Player::UpdatePhysicsMovement(float deltaTime)
	{
		float xPos = (float)rigidBody->getCenterOfMassPosition().x();
		float yPos = (float)rigidBody->getCenterOfMassPosition().y();
		float zPos = (float)rigidBody->getCenterOfMassPosition().z();
		position = glm::vec3(xPos, yPos, zPos);

		isMoving = false;
		isRunning = false;
		crouching = false;
	}



	void Player::UpdateMovement(Camera* camera, float deltaTime)
	{
		// Movement
		targetVelocity.x = 0;
		targetVelocity.z = 0;

		rigidBody->activate();
		isMoving = false;
		glm::vec3 Forward = glm::normalize(glm::vec3(camera->Front.x, 0, camera->Front.z));

		if (Input::IsKeyPressed(HELL_KEY_W)) {
			targetVelocity -= Forward;
			isMoving = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_S)) {
			targetVelocity += Forward;
			isMoving = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_A)) {
			targetVelocity -= camera->Right;
			isMoving = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_D)) {
			targetVelocity += camera->Right;
			isMoving = true;
		}

		targetVelocity *= deltaTime *= currentSpeed * 60;

		// Lerp
		currentVelocity.x = Util::FInterpTo(currentVelocity.x, targetVelocity.x, deltaTime, velocityApproachSpeed);
		currentVelocity.y = rigidBody->getLinearVelocity().getY();
		currentVelocity.z = Util::FInterpTo(currentVelocity.z, targetVelocity.z, deltaTime, velocityApproachSpeed);

		// Prevent sliding about
		if (!isMoving) {
			currentVelocity.x = 0;
			currentVelocity.z = 0;
		}

		rigidBody->setLinearVelocity(Util::glmVec3_to_btVec3(currentVelocity));
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
}