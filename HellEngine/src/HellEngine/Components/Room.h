#pragma once
#include "Floor.h"
#include "Ceiling.h"
#include "Door.h"
#include "Wall.h"
#include "HellEngine/Light.h"
#include "Platform/OpenGL/TexturedMesh.h"

namespace HellEngine
{
	class Room
	{
	public: // methods
		Room(glm::vec3 cornerA, glm::vec3 cornerB);
		~Room(); 
		void Build(std::vector<Door>& doors);
		void FindDoors(std::vector<Door>& doors);
		void BuildWalls();
		void DrawWalls(Shader* shader, bool bindTextures);
		float GetLowerXCoord();
		float GetLowerZCoord();
		float GetUpperXCoord();
		float GetUpperZCoord();
		void CalculateLightLimits();


	public: // fields
		Floor floor;
		Ceiling ceiling;
		Light light;
			
		glm::vec3 cornerA; // bottom left
		glm::vec3 cornerB; // top right

		std::vector<Door*> doors_FrontWall;
		std::vector<Door*> doors_BackWall;
		std::vector<Door*> doors_LeftWall;
		std::vector<Door*> doors_RightWall;

		//TexturedMesh wallMesh;

		std::vector<Wall> walls;
		std::vector<WallHole> wallHoles;
		// one side must be locked
	};
}