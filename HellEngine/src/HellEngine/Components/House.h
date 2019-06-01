#pragma once
#include "HellEngine/Components/Wall.h"
#include "HellEngine/Components/Door.h"
#include "HellEngine/Components/Room.h"

namespace HellEngine {

	class House
	{
	public: // members
		House();
		~House();
		void Init();
		void DrawAll(Shader* shader, bool bindTextures);
		//void DrawWalls(Shader* shader, bool bindTextures);
		void DrawDoors(Shader* shader, bool bindTextures);
		void AddDoor(float x, float z, Axis axis);
		void AddRoom(glm::vec3 cornerA, glm::vec3 cornerB);
		void BufferWallMatrices();



	public: // fields
		//std::vector<Wall> walls;
		std::vector<Door> doors;

		glm::vec3 position;
		std::vector<Room> rooms;

		std::vector<glm::mat4> wallModelMatrices;
	};
}