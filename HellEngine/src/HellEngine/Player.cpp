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
		//btCylinderShape* collisionShape = new btCylinderShape(btVector3(radius, height, radius));
		collisionShape->setLocalScaling(btVector3(1, 1, 1));

		// Position
		btTransform btTransform;
		btTransform.setIdentity();
		btTransform.setOrigin(btVector3(Util::glmVec3_to_btVec3(INITIAL_POSITION)));

		// Create it
		rigidBody = Physics::createRigidBody(1.0, btTransform, collisionShape, 0.1);
		
		// Set rigid body name
		EntityData* entityData = new EntityData();
		entityData->name = "PLAYER";
		rigidBody->setUserPointer(entityData);

		// Set collision filter group (for raycasting and not hitting the player capsule)
		rigidBody->getBroadphaseProxy()->m_collisionFilterGroup = btBroadphaseProxy::CharacterFilter;
	}

	void Player::Update(Camera* camera, Physics* physics, float deltaTime)
	{
		CalculateIsRunning();
		CastFloorRay(physics);
		CalculateIsGrounded();
		CalculateCurrentSpeed(deltaTime);
		UpdatePhysicsMovement(deltaTime);
		UpdateMovement(camera, deltaTime);
		UpdateCrouching(deltaTime);
		UpdateAudio(deltaTime);
	}

	void Player::CalculateIsGrounded()
	{
		if (floorRay.name == "NO RAY HIT")
			isGrounded = false;
		else 
			if (floorRay.distance < 0.3f) // 0.2 is the distance on flat ground, and 0.28 is the distance on stairs
				isGrounded = true;
		else
			isGrounded = false;
	}

	void Player::CastFloorRay(Physics* physics)
	{
		// Begining and end of ray
		glm::vec3 origin = position + glm::vec3(0, 0.0f, 0);
		glm::vec3 end = origin + glm::vec3(0, -1.0f, 0);

		// Create a call back and ingore player capsule
		btCollisionWorld::ClosestRayResultCallback rayCallback(Util::glmVec3_to_btVec3(origin), Util::glmVec3_to_btVec3(end));
		rayCallback.m_collisionFilterMask &= ~btBroadphaseProxy::CharacterFilter;

		physics->m_dynamicsWorld->rayTest(Util::glmVec3_to_btVec3(origin), Util::glmVec3_to_btVec3(end), rayCallback);

		floorRay.index = -1;
		floorRay.hitPoint = glm::vec3(0);
		floorRay.surfaceNormal = glm::vec3(0);
		floorRay.distance = -1;
		floorRay.name = "NO RAY HIT";

		if (rayCallback.hasHit())
		{
			// Collision object
			btVector3 objectCOM = rayCallback.m_collisionObject->getWorldTransform().getOrigin();
			btVector3 RayCastOffsetFromCOM = objectCOM - rayCallback.m_hitPointWorld;

			btRigidBody* rigidBody = (btRigidBody*)rayCallback.m_collisionObject;

			floorRay.hitPoint = Util::btVec3_to_glmVec3(rayCallback.m_hitPointWorld);
			floorRay.distance = (rayCallback.m_hitPointWorld - Util::glmVec3_to_btVec3(origin)).length();
			floorRay.surfaceNormal = Util::btVec3_to_glmVec3(rayCallback.m_hitNormalWorld);

			EntityData* entityData = (EntityData*)rigidBody->getUserPointer();
			if (entityData) {
				floorRay.name = entityData->name;
			}
		}

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

		/*
		//	currentVelocity.y = 0;
		//	rigidBody->setAngularFactor(btVector3(0,0,0));

		// Prevent silding down slopes
		if (floorRay.name == "STAIRS" && isGrounded && !isMoving) {
			rigidBody->setGravity(btVector3(0, 0, 0));
			currentVelocity.y = 0;
		}
		else
			rigidBody->setGravity(btVector3(0, -10, 0));

		float MAX_Y_VEL_UP_STAIRS = 21;

		if (floorRay.name == "STAIRS")
			currentVelocity.y = std::min(currentVelocity.y, MAX_Y_VEL_UP_STAIRS);

		// Perhaps think about shooting 4 rays, all on different edges of the player capsule rigid body, and taking any hit.
		*/

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