#pragma once
#include "HellEngine/Components/Wall.h"
#include "HellEngine/Components/Door.h"
#include "HellEngine/Components/Room.h"
#include "Platform/OpenGL/StairMesh.h"

namespace HellEngine {

	class House
	{
	public: // members
		House();
		~House();
		void RebuildRooms();
		void DrawAll(Shader* shader, bool bindTextures);
		void DrawDoors(Shader* shader, bool bindTextures);
		void AddDoor(float x, float z, Axis axis, unsigned int floorMaterialID, bool initiallyOpen, bool initiallyLocked, float maxOpenAngle, bool rotateFloorTexture);
		void AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling);
		void AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling, Light light);
		void RemoveAllContents();

	//	void ConstructWallMesh();

	public: // fields
		//std::vector<Wall> walls;
		std::vector<Door> doors;
		glm::vec3 position;
		std::vector<Room> rooms;

		StairMesh stairMesh;

		//WallMesh WallMesh;

		std::vector<glm::mat4> wallModelMatrices;

	public: // level editor related
		glm::vec2 newDoorPosition = glm::vec2(0, 0);
		glm::vec2 newRoomCornerAPosition = glm::vec2(0, 0);
		glm::vec2 newRoomCornerBPosition = glm::vec2(1, 1);
	};
}