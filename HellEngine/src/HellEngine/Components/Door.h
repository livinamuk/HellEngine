#pragma once

#include "Platform/OpenGL/Shader.h"
#include "HellEngine/Model.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "Floor.h"

namespace HellEngine {

	enum DoorStatus {
		DOOR_OPEN,
		DOOR_OPENING,
		DOOR_CLOSED,
		DOOR_CLOSING
	};

	class Door
	{
	public: // methods
		Door(float x, float z, Axis axis, std::string floorMaterialName, bool rotateFloor);
		~Door();

		void Update(float deltaTime);
		void Interact();
		float GetAngle();
		float GetBaseAngle();
		float GetJamAngle();
		glm::vec3 Door::GetDoorModelPosition();
		glm::mat4 GetModelMatrix();

	public: // fields
		glm::vec3 position;
		glm::vec3 rotation; 
		BoundingBox boundingBox;
		Axis axis;
		float doorStatus = DOOR_CLOSED;
		Floor floor;

	private: // fields
		float openAngle = 0;
		//float baseAngle;
		float maxOpenAngle = 2;

	public: // static
		static Model* modelDoor;
		static Model* modelDoorShadowCaster;
		static Model* modelDoorJam;
		static Model* modelWallHole;
	};
}