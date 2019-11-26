#include "hellpch.h"
#include "Physics.h"
#include "HellEngine/Util.h"
#include "Physics.h"
#include "HellEngine/AssetManager.h"

namespace HellEngine
{
	btCollisionObject* Physics::m_triangleCollisionObject;
	btDiscreteDynamicsWorld* Physics::m_dynamicsWorld;
	btAlignedObjectArray<btRigidBody*> Physics::m_rigidBodies;
	 
	Physics::Physics()
	{
		// Build the broadphase
		btBroadphaseInterface* broadphase = new btDbvtBroadphase();

		// Set up the collision configuration and dispatcher
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		// The actual physics solver
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		// The world.
		btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, GRAVITY, 0));

		InitCollisionShapes();
		CreateWorld();
	}

	Physics::~Physics()
	{
		//delete world;
	}

	void Physics::CreateWorld()
	{
		///collision configuration contains default setup for memory, collision setup
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
		m_broadphase = new btDbvtBroadphase();
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;
		m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
		m_dynamicsWorld->setGravity(btVector3(0, -10, 0));


		// Create the ground
		btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		m_collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		btRigidBody* floor;

		{
			btScalar mass(0.);
			floor = createRigidBody(mass, groundTransform, groundShape, 1.0f);
		}

		EntityData* entityData = new EntityData();
		entityData->name = "FLOOR";
		entityData->vectorIndex = 0;
		floor->setUserPointer(entityData);

		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(btVector3( 0.15, 0.15, 0.15));
		m_collisionShapes.push_back(collisionShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);
		float friction = 0.5f;

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			collisionShape->calculateLocalInertia(mass, localInertia);


		for (int k = 0; k < 6; k++)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					startTransform.setOrigin(btVector3(
						btScalar(4.9f + 0.2 * i),
						btScalar(0.1f + .2 * k),
						btScalar(2.0f + 0.2 * j)));

					//colShape->
					m_rigidBodies.push_back(createRigidBody(mass, startTransform, collisionShape, friction));
				}
			}
		}

	
		/*btDefaultMotionState* motionState;

		float doorWidth = 0.8f;
		float doorHeight = 2.0f;
		float doorDepth = 0.036f;

		btBoxShape* CollisionShape = new btBoxShape(btVector3(1, 1, 1));
		CollisionShape->setLocalScaling(btVector3(doorWidth * 0.5f, doorHeight * 0.5f, doorDepth * 0.5f));
		btTransform btTransform;
		btTransform.setIdentity();
		btTransform.setOrigin(btVector3(0, 1, 0));
		doorRigidBody = createRigidBody(mass, btTransform, CollisionShape, friction);
		doorRigidBody->setActivationState(DISABLE_DEACTIVATION);
		

		glm::vec3 collisionShape = glm::vec3(doorWidth * 0.5f, doorHeight * 0.5f, doorDepth * 0.5f);

		//BoxShape boxShape = new BoxShape(Door.CollisionShape);
		//pivotA = new Vector3(, Door.CollisionShape.Y, Door.CollisionShape.Z);


		const btVector3 pivot = btVector3(-collisionShape.x, collisionShape.y, -collisionShape.z);
		btVector3 axis(0.0f, 1.0f, 0.0f);
		doorHinge = new btHingeConstraint(*doorRigidBody, pivot, axis);
		 
		float low = 2;
		float high = 0;
		float softness = 0;
		float biasFactor = 0.01f;
		float relaxationFactor = 0;
		doorHinge->setLimit(low, high, softness, biasFactor, relaxationFactor);

		float targetVelocity = 1.0f;
		float maxMotorImpulse = 1.0f;
		doorHinge->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
		
		m_dynamicsWorld->
dBody(doorRigidBody);
		m_dynamicsWorld->addConstraint(doorHinge);

		m_rigidBodies.push_back(doorRigidBody);*/

	}

	void Physics::AddStairMeshToPhysicsWorld(StairMesh stairMesh)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (BoundingPlane& plane : stairMesh.boundingPlanes)
		{
			if (plane.testCollisions)
			{
				btVector3 vertA = Util::glmVec3_to_btVec3(plane.A);
				btVector3 vertB = Util::glmVec3_to_btVec3(plane.B);
				btVector3 vertC = Util::glmVec3_to_btVec3(plane.C);
				btVector3 vertD = Util::glmVec3_to_btVec3(plane.D);

				triangleMesh->addTriangle(vertA, vertB, vertC);
				triangleMesh->addTriangle(vertC, vertD, vertA);
			}
		}
		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		m_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		m_triangleCollisionObject = new btCollisionObject();
		m_triangleCollisionObject->setCollisionShape(triangleMeshShape);
		m_triangleCollisionObject->setWorldTransform(meshTransform);

		EntityData* entityData = new EntityData();
		entityData->name = "STAIRS";
		entityData->vectorIndex = 0;
		m_triangleCollisionObject->setUserPointer(entityData);

		m_dynamicsWorld->addCollisionObject(m_triangleCollisionObject);
		m_triangleCollisionObject->setCollisionFlags(m_triangleCollisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	void Physics::AddWallsToPhysicsWorld(std::vector<BoundingPlane*> planes)
	{
		//return;
		btTriangleMesh* triangleMesh = new btTriangleMesh();

		for (BoundingPlane* plane : planes)
		{
			if (plane->testCollisions)
			{
				btVector3 vertA = Util::glmVec3_to_btVec3(plane->A);
				btVector3 vertB = Util::glmVec3_to_btVec3(plane->B);
				btVector3 vertC = Util::glmVec3_to_btVec3(plane->C);
				btVector3 vertD = Util::glmVec3_to_btVec3(plane->D);

				triangleMesh->addTriangle(vertA, vertB, vertC);
				triangleMesh->addTriangle(vertC, vertD, vertA);
			}
		}
		btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

		m_collisionShapes.push_back(triangleMeshShape);

		btTransform meshTransform;
		meshTransform.setIdentity();
		meshTransform.setOrigin(btVector3(0, 0, 0));

		m_triangleCollisionObject = new btCollisionObject();
		m_triangleCollisionObject->setCollisionShape(triangleMeshShape);
		m_triangleCollisionObject->setWorldTransform(meshTransform);
		EntityData* entityData = new EntityData();
		entityData->name = "WALL";
		entityData->vectorIndex = 0;
		m_triangleCollisionObject->setUserPointer(entityData);
		m_dynamicsWorld->addCollisionObject(m_triangleCollisionObject);
		m_triangleCollisionObject->setCollisionFlags(m_triangleCollisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	btRigidBody* Physics::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction)
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
		btDefaultMotionState * myMotionState = new btDefaultMotionState(startTransform);

		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
		cInfo.m_friction = friction;

		btRigidBody* body = new btRigidBody(cInfo);
		//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
		btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
		body->setWorldTransform(startTransform);
#endif  //

		body->setUserIndex(-1);
		m_dynamicsWorld->addRigidBody(body);

		return body;
		
	}


	void Physics::Update(float deltaTime)
	{
		m_dynamicsWorld->stepSimulation(deltaTime);
	}

	void Physics::InitCollisionShapes()
	{
		COLLISION_SHAPE_BOX = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		btCylinderShape* Collision_Shape_Cylinder = new btCylinderShape(btVector3(1, 1, 1));
	}

	void Physics::AddHouse(House* house)
	{

		BoundingBox boundingBox = AssetManager::GetModelByName("door.obj")->meshes[0].boundingBox;
		btVector3 collisionScale = Util::glmVec3_to_btVec3(boundingBox.baseTransform.scale);

		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);

		//for (Door & door : house->doors)
		for (int i = 0; i < house->doors.size(); i++)
		{
			Door* door = &house->doors[i];

			// Position
			btTransform btTransform;
			btTransform.setIdentity();

			float x = door->position.x;
			float y = door->position.y + 1;
			float z = door->position.z;
					   
			btTransform.setOrigin(btVector3(x, y, z));
			btTransform.setRotation(btQuaternion(door->rotationWhenClosed, 0, 0));
			door->rigidBody = createRigidBody(0.0, btTransform, collisionShape, 1.0);

			// Entity Data
			EntityData* entityData = new EntityData();
			entityData->name = "DOOR";
			entityData->vectorIndex = i;
			door->rigidBody->setUserPointer(entityData);

			door->rigidBody->setCollisionFlags(door->rigidBody->getCollisionFlags() |
				btCollisionObject::CF_KINEMATIC_OBJECT |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	

			//m_rigidBodies.push_back(door->rigidBody);
		}

		/*BoundingBox boundingBox = AssetManager::GetByName("door.obj")->meshes[0].boundingBox;
		btVector3 collisionScale = Util::glmVec3_to_btVec3(boundingBox.baseTransform.scale);

		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);

		float mass = 1.0f;
		float friction = 0.5f;
		float softness = 0;
		float biasFactor = 0.01f;
		float relaxationFactor = 0;
		float targetVelocity = 1.0f;
		float maxMotorImpulse = 1.0f;

		//for (Door & door : house->doors)
		for (int i = 0; i < house->doors.size(); i++)
		{
			Door* door = &house->doors[i];

			btTransform btTransform;
			btTransform.setIdentity();

			float x = door->position.x;
			float y = door->position.y + 1;
			float z = door->position.z;



			// Rotation
			float low = 0;
			float high = 0;
			float rotation = 0;

			if (door->axis == X) {
				low = -1.7f;
				high = 0;
				rotation = 0;
			}

			if (door->axis == X_NEGATIVE) {
				low = -1.7f - HELL_PI;// -ROTATE_180;
				high = 0 - HELL_PI;// -ROTATE_180;
				rotation = 0;
			}
			if (door->axis == Z)
			{
				rotation = ROTATE_90;

			}
			if (door->axis == Z_NEGATIVE)
			{
				rotation = ROTATE_270;
			}


			btTransform.setOrigin(btVector3(x, y, z));
			btTransform.setRotation(btQuaternion(rotation, 0, 0));
			door->rigidBody = createRigidBody(mass, btTransform, collisionShape, friction);
			door->rigidBody->setActivationState(DISABLE_DEACTIVATION);

			door->rigidBody->setUserIndex(RigidBodyType::DOOR);
			door->rigidBody->setUserIndex2(i);

			glm::vec3 pivotPosition = boundingBox.baseTransform.scale * 0.5f;
			btVector3 axis(0.0f, 1.0f, 0.0f);

			const btVector3 pivot = btVector3(-pivotPosition.x, pivotPosition.y, pivotPosition.z);
			door->hinge = new btHingeConstraint(*door->rigidBody, pivot, axis);

			door->hinge->setLimit(low, high, softness, biasFactor, relaxationFactor);
			door->hinge->enableAngularMotor(true, targetVelocity, maxMotorImpulse);

			m_dynamicsWorld->addRigidBody(door->rigidBody);
			m_dynamicsWorld->addConstraint(door->hinge);

			m_rigidBodies.push_back(door->rigidBody);
			
		}*/
	}
}

