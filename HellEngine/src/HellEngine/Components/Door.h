#pragma once

#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Model.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "Floor.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"

namespace HellEngine {

	enum DoorStatus {
		DOOR_OPEN,
		DOOR_OPENING,
		DOOR_CLOSED,
		DOOR_CLOSING,
		LOCKED_FROM_THE_OTHER_SIDE
	};

	class Door
	{
	public: // methods
		Door(float x, float z, Axis axis, unsigned int floorMaterialID, bool initiallyOpen, bool initiallyLocked, float maxOpenAngle, bool rotateFloor);
		~Door();

		void Update(float deltaTime);
		void Interact(glm::vec3 playerPosition);
		float GetAngle();
		float GetBaseAngle();
		float GetJamAngle();
		glm::vec3 Door::GetDoorModelPosition();
		glm::mat4 GetModelMatrix();
		void DrawBoundingBox(Shader* shader);

		void Door::UpdateRigidBodyTransfrom();

		Transform modelTransform;
		btTransform baseTransform;
		btRigidBody* rigidBody; 

	public: // fields

		float rotationWhenClosed;
		glm::vec3 position;
		glm::vec3 rotation; 
		Axis axis;
		int doorStatus = DOOR_CLOSED;
		bool initiallyOpen = false;
		bool initiallyLocked = false;
		bool locked = false;
		//int 
		Floor floor;
		float maxOpenAngle = 2;

	public: // fields
		float openAngle = 0;
		
	public: // static
		static bool PlayerHasKey;
		static unsigned int materialID_Door;
		static unsigned int materialID_DoorFrame;
		static unsigned int modelID_Door;
		static unsigned int modelID_DoorFrame;
	};
}