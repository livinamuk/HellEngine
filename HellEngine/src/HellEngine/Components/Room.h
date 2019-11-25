#pragma once
#include "Floor.h"
#include "Ceiling.h"
#include "Door.h"
#include "Wall.h"
#include "HellEngine/Light.h"
#include "Platform/OpenGL/TexturedMesh.h"
#include "Platform/OpenGL/LightVolume.h"
#include "Platform/OpenGL/WallMesh.h"

namespace HellEngine
{
	class Room
	{
	public: // methods
		Room(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling);
		Room(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling, Light light);
		~Room();
		void BuildAll(std::vector<Door>& doors);
		void FindDoors(std::vector<Door>& doors);
		void BuildWalls();
		float GetLowerXCoord();
		float GetLowerZCoord();
		float GetUpperXCoord();
		float GetUpperZCoord();
		void CalculateLightLimits();
		void CreateLightVolumes();
		void SetWallMaterial(unsigned int materialID);
		void RecalculateWidthAndDepth();
		void ResizeFloorAndCeiling();
		void CenterLight();
		void BuildBlackWalls();

		void ConstructWallMesh();
		void DrawWallTrim(Shader* shader);
		void AddBottomWallTrim(glm::vec3 position, float rotation, float scale);
		void AddTopWallTrim(glm::vec3 position, float rotation, float scale);

	public: // fields
		Floor floor;
		Ceiling ceiling;
		Light light;
		float width;
		float depth;
		unsigned int wallMaterialID;
		glm::vec2 texOffset;

		glm::vec3 cornerA; // bottom left
		glm::vec3 cornerB; // top right

		std::vector<Door*> doors_FrontWall;
		std::vector<Door*> doors_BackWall;
		std::vector<Door*> doors_LeftWall;
		std::vector<Door*> doors_RightWall;
		std::vector<Door*> allDoors;

		std::vector<DoorHole> doorHoles_BackWall;
		std::vector<DoorHole> doorHoles_LeftWall;
		std::vector<DoorHole> doorHoles_FrontWall;
		std::vector<DoorHole> doorHoles_RightWall;

		LightVolume lightVolume;

		std::vector<LightVolume> backWallDoorwayLightVolumes;
		std::vector<LightVolume> frontWallDoorwayLightVolumes;
		std::vector<LightVolume> leftWallDoorwayLightVolumes;
		std::vector<LightVolume> rightWallDoorwayLightVolumes;

		std::vector<Wall> walls;
		std::vector<WallHole> wallHoles;


		std::vector<Transform> bottomTrimTransforms;
		std::vector<Transform> topTrimTransforms;
		WallMesh wallMesh;

		static float Light_Volume_Bias;
	};
}