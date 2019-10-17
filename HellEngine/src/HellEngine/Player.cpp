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

		// Rotation
		btTransform.setOrigin(btVector3(Util::glmVec3_to_btVec3(INITIAL_POSITION)));

		// Create it
		rigidBody = Physics::createRigidBody(1.0, btTransform, collisionShape, 0.1);
	
		// Kinematic flags
		//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		//rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}

	void Player::UpdateRigidBodyWorldTransform()
	{
	/*
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		rigidBody->setWorldTransform(transform);

		CheckBulletCollisions();*/
	}

	struct BulletCollisionData {
		bool occured;
		float distance;
		glm::vec3 point;
		glm::vec3 normal;

		BulletCollisionData() {reset();}
		void reset() {
			occured = false;
			distance = 0.0;
			point = glm::vec3(0);
			normal = glm::vec3(0);
		}
	};


	struct collisionTestResult : public btCollisionWorld::ContactResultCallback
	{
		collisionTestResult(btRigidBody& playerRigidBody, BulletCollisionData& collisionData) : btCollisionWorld::ContactResultCallback(), body(playerRigidBody), data(collisionData){}
		btRigidBody& body;
		BulletCollisionData& data;

		virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
		{
			data.occured = true;
			data.distance = cp.getDistance();
			data.normal = Util::btVec3_to_glmVec3(cp.m_normalWorldOnB);
			data.point = Util::btVec3_to_glmVec3(cp.m_localPointA);
			//std::cout << "P: " << Util::Vec3ToString(data.point) << " N: " << Util::Vec3ToString(data.normal) << "\n";
			return 0;
		}
	};


	void Player::CheckBulletCollisions()
	{
		btRigidBody plyr(*rigidBody);									// reference to player
		btCollisionObject triMesh(*Physics::m_triangleCollisionObject);	// reference to triangle mesh
		BulletCollisionData collisionData;
		collisionTestResult renderCallback(plyr, collisionData);

		// Check against rigid bodies
	/*	for (int i = 0; i < Physics::m_rigidBodies.size(); i++)
		{
			btRigidBody body(*Physics::m_rigidBodies[i]);				// reference to object in world
			Physics::m_dynamicsWorld->contactPairTest(&plyr, &body, renderCallback);

			if (collisionData.occured)
			{

			}



		}	*/

		// Check against the giant triangle mesh
		Physics::m_dynamicsWorld->contactPairTest(&plyr, &triMesh, renderCallback);

		if (collisionData.occured)
		{
			/*glm::vec2 contactPoint = this->position - collisionData.normal * collisionData.distance;
			float penetrationDepth = this->radius - collisionData.distance;


			glm::vec2 planeNormal = glm::vec2(collisionData.normal.x, collisionData.normal.z);
			glm::vec2 spherePosition = glm::vec2(position.x, position.z);



			glm::vec2 penetrationNormal = glm::vec2(collisionData.normal.x, collisionData.normal.z);
			spherePosition += penetrationNormal * penetrationDepth;*/


		//	float penetrationDistance = glm::length(collisionData.point);
		//	position.x += collisionData.normal.x * penetrationDistance;

		//	std::cout << "CON POINT: " +Util::Vec3ToString(collisionData.point) << "\n";
			//std::cout << "DIST: " + std::to_string(penetrationDistance) << "\n";

			//position.x += penetrationDistance * -1;

			//position.x = spherePosition.x;
			//position.z = spherePosition.y;

			//position.x = 1;
		}
	}

	void Player::Update(Camera* camera, float deltaTime, vector<BoundingBox*> boundingBoxes, vector<BoundingPlane*> boundingPlanePtrs)
	{
		CalculateIsRunning();
		CalculateCurrentSpeed(deltaTime);
		UpdatePhysicsMovement(deltaTime);
		UpdateMovement(camera, deltaTime, boundingBoxes, boundingPlanePtrs);
		UpdateCrouching(deltaTime);
		//UpdateGravity(deltaTime);
		UpdateAudio(deltaTime);
		//UpdateRigidBodyWorldTransform();
	}

	void Player::UpdateAudio(float deltaTime)
	{
		if (!walking)
			footstepAudioTimer = 0;
		else {
			if (walking && footstepAudioTimer == 0) {
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

		walking = false;
		isRunning = false;
		crouching = false;


		//position = rigidBody->getWorldTransform.trans();
	}

	void Player::UpdateMovement(Camera* camera, float deltaTime, vector<BoundingBox*> boundingBoxPtrs, vector<BoundingPlane*> boundingPlanePtrs)
	{
	
		// Movement
		targetVelocity.x = 0;
		targetVelocity.z = 0;

		rigidBody->activate();
		walking = false;
		glm::vec3 Forward = glm::normalize(glm::vec3(camera->Front.x, 0, camera->Front.z));

		if (Input::IsKeyPressed(HELL_KEY_W)) {
			targetVelocity -= Forward;
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_S)) {
			targetVelocity += Forward;
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_A)) {
			targetVelocity -= camera->Right;
			walking = true;
		}
		if (Input::IsKeyPressed(HELL_KEY_D)) {
			targetVelocity += camera->Right;
			walking = true;
		}

		targetVelocity *= deltaTime *= currentSpeed * 60;

	

		// Jump
		if ((IsGrounded()) && Input::IsKeyPressed(HELL_KEY_SPACE))
			targetVelocity.y += jumpStrength;
		


		// Lerp
		currentVelocity.x = Util::FInterpTo(currentVelocity.x, targetVelocity.x, deltaTime, velocityApproachSpeed);
		currentVelocity.y = Util::FInterpTo(currentVelocity.y, targetVelocity.y, deltaTime, velocityApproachSpeed);
		//currentVelocity.y = targetVelocity.y;
		currentVelocity.z = Util::FInterpTo(currentVelocity.z, targetVelocity.z, deltaTime, velocityApproachSpeed);

		rigidBody->setLinearVelocity(Util::glmVec3_to_btVec3(currentVelocity));

	}
		
	bool Player::IsGrounded()
	{
		if (position.y <= currentGroundHeight + KINDA_SMALL_NUMBER)
			return true;
		else 
			return false;
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