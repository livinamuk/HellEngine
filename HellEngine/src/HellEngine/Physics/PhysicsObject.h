#pragma once

#include "HellEngine/Transform.h"
#include "bullet/src/btBulletCollisionCommon.h"

namespace HellEngine
{
	class PhysicsObject
	{
	public: // fields
		Transform transform;
		std::string modelName;
		std::string materialName;

	public: // methods 
		PhysicsObject();
		PhysicsObject(glm::vec3 position, std::string modelName, std::string materialName);
		~PhysicsObject();

	public:

	};
}