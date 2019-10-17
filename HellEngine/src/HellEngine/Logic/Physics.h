#pragma once

//#include "react/reactphysics3d.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
//#include "../CommonInterfaces/CommonRigidBodyBase.h"

#include "Platform/OpenGL/BoundingPlane.h"

#include "HellEngine/Components/House.h"

//using namespace reactphysics3d;
 
/*#include "btBulletDynamicsCommon.h"
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"

//#include "../CommonInterfaces/CommonRigidBodyBase.h"
*/

namespace HellEngine
{
	class Physics
	{
	public: // fields

		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btDbvtBroadphase* m_broadphase;
		btSequentialImpulseConstraintSolver* sol;
		btConstraintSolver* m_solver;
		
		btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
		
		static btDiscreteDynamicsWorld* m_dynamicsWorld;
		static btAlignedObjectArray<btRigidBody*> m_rigidBodies;
		static btCollisionObject* m_triangleCollisionObject;

		btRigidBody* body0;
		btRigidBody* body1;
		

		//btRigidBody* doorRigidBody;
		//btHingeConstraint* doorHinge;

		btBoxShape* COLLISION_SHAPE_BOX;

	public: // methods 
		Physics();
		~Physics();
		void InitCollisionShapes();
		void CreateWorld();
		void AddHouse(House* house);
		void Update(float deltaTime);

		static btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction);
		void AddWallsToPhysicsWorld(std::vector<BoundingPlane*> planes);

	//public: // static fields
		btBoxShape* collisionShapeBox;

	//public: // static methods

	};
}