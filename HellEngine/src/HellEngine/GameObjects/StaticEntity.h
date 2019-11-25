#pragma once

#include "HellEngine/Util.h"
#include "HellEngine/Material.h"
#include "HellEngine/Logic/Physics.h"
#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Model.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/matrix_decompose.hpp"

namespace HellEngine
{
	class StaticEntity
	{
	public: // fields
		btRigidBody* rigidBody;
		unsigned int modelID = -1;
		unsigned int materialID = -1;
		std::string name = "UNNAMED_STATIC_ENTITY";

	private: // fields
		Transform transform;

	public: // methods
		StaticEntity(unsigned int modelID, glm::vec3 position, std::string name);
		~StaticEntity();
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::vec3 rotation);
		void SetScale(glm::vec3 rotation);
		void DrawBoundingBox(Shader* shader);
		void Draw(Shader* shader, bool bindTextures);
		void Delete();

	private: // methods
		void CreateRigidBody();
		void UpdateRigidBodyTransform();
		//void UpdateWorldTransform();
	};
}