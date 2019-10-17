#include "hellpch.h"
#include "HellEngine/Util.h"
#include "PhysicsObject.h"

namespace HellEngine
{
	PhysicsObject::PhysicsObject(glm::vec3 position, std::string modelName, std::string materialName)
	{
		transform.position = position;
		this->modelName = modelName;
		this->materialName = materialName;
	}

	PhysicsObject::PhysicsObject()
	{
	}

	PhysicsObject::~PhysicsObject()
	{
		
	}



}

