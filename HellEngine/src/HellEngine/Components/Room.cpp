#include "hellpch.h"
#include "Room.h"
#include "HellEngine/Util.h"
#include "HellEngine/Material.h"
#include <algorithm>

#include "Platform/OpenGL/RenderableObject.h"
namespace HellEngine
{
	float Room::Light_Volume_Bias = 0.0925f;

	Room::Room(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling)
	{
		float lowestX = std::min(cornerA.x, cornerB.x);
		float lowestZ = std::min(cornerA.z, cornerB.z);
		float highestX = std::max(cornerA.x, cornerB.x);
		float highestZ = std::max(cornerA.z, cornerB.z);

		this->cornerA = glm::vec3(lowestX, 0, lowestZ);
		this->cornerB = glm::vec3(highestX, 0, highestZ);
		RecalculateWidthAndDepth();

		this->wallMaterialID = wallMaterialID;
		this->floor = Floor(cornerA, width, depth, floorMaterialID, rotateFloor);
		this->ceiling = Ceiling(cornerA, width, depth, ceilingMaterialID, rotateCeiling);
		this->light = Light(cornerA.x + width / 2, 2.0f, cornerA.z + depth / 2);	// centered light
	}	
	
	Room::Room(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling, Light light)
	{
		float lowestX = std::min(cornerA.x, cornerB.x);
		float lowestZ = std::min(cornerA.z, cornerB.z);
		float highestX = std::max(cornerA.x, cornerB.x);
		float highestZ = std::max(cornerA.z, cornerB.z);

		this->cornerA = glm::vec3(lowestX, 0, lowestZ);
		this->cornerB = glm::vec3(highestX, 0, highestZ);
		RecalculateWidthAndDepth();

		this->wallMaterialID = wallMaterialID;
		this->floor = Floor(cornerA, width, depth, floorMaterialID, rotateFloor);
		this->ceiling = Ceiling(cornerA, width, depth, ceilingMaterialID, rotateCeiling);
		this->light = light;	// constructor specified light
	}

	Room::~Room()
	{
		// Destructor
	}

	void Room::RecalculateWidthAndDepth()
	{
		this->width = cornerB.x - cornerA.x;
		this->depth = cornerB.z - cornerA.z;
	}

	void Room::BuildAll(std::vector<Door>& doors)
	{
		FindDoors(doors);
		BuildWalls();
		CreateLightVolumes();
		RecalculateWidthAndDepth();
		ResizeFloorAndCeiling();
	}

	bool sameCoordinate(float a, float b)
	{
		float epsilon = 0.1f;
		return abs(a - b) < epsilon;
	}

	bool withinRange(float x, float lower, float upper)
	{
		if (x < lower)
			return false;
		if (x > upper)
			return false;
		else
			return true;
	}

	void Room::FindDoors(std::vector<Door>& doors)
	{
		// Start from scratch yo
		doors_BackWall.clear();
		doors_FrontWall.clear();
		doors_LeftWall.clear();
		doors_RightWall.clear();
		doorHoles_BackWall.clear();
		doorHoles_FrontWall.clear();
		doorHoles_LeftWall.clear();
		doorHoles_RightWall.clear();
		allDoors.clear();

		// Iterate over each door and add it to the appropriate door vector.
		for (Door & door : doors)
		{
			// Falls within X wall range...
			if (withinRange(door.position.x, GetLowerXCoord(), GetUpperXCoord()))
			{
				if (sameCoordinate(door.position.z, GetLowerZCoord()))
					doors_FrontWall.push_back(&door);
				else if (sameCoordinate(door.position.z, GetUpperZCoord() + 0.1f))
					doors_BackWall.push_back(&door);
			}
			// Falls within Z wall range...
			if (withinRange(door.position.z, GetLowerZCoord(), GetUpperZCoord()))
			{
				if (sameCoordinate(door.position.x, GetLowerXCoord()))
					doors_LeftWall.push_back(&door);
				else if (sameCoordinate(door.position.x, GetUpperXCoord() + 0.1f))
					doors_RightWall.push_back(&door);
			}
		}

		// Make a list of all the doors, for ease later on.
		for (Door* door : doors_BackWall)
			allDoors.push_back(door);
		for (Door* door : doors_FrontWall)
			allDoors.push_back(door);
		for (Door* door : doors_LeftWall)
			allDoors.push_back(door);
		for (Door* door : doors_RightWall)
			allDoors.push_back(door);
	}

	void Room::AddBottomWallTrim(glm::vec3 position, float rotation, float scale)
	{
		Transform trim;
		trim.position = position;
		trim.rotation.y = rotation;
		trim.scale.x = scale;
		bottomTrimTransforms.push_back(trim);
	}	
	
	void Room::AddTopWallTrim(glm::vec3 position, float rotation, float scale)
	{
		Transform trim;
		trim.position = position;
		trim.rotation.y = rotation;
		trim.scale.x = scale;
		topTrimTransforms.push_back(trim);
	}

	void Room::ConstructWallMesh()
	{
		float doorWidth = 1;
		float doorHeight = 2;
		float roomHeight = 2.4f;

		float cursorX;
		float cursorZ;
		float wallWidth;

		wallMesh.ClearMesh();
		bottomTrimTransforms.clear();
		topTrimTransforms.clear();

		// Top trim transforms
		AddTopWallTrim(glm::vec3(cornerA.x, 0, cornerA.z), 0, cornerB.x - cornerA.x);
		AddTopWallTrim(glm::vec3(cornerB.x, 0, cornerB.z), ROTATE_180, cornerB.x - cornerA.x);
		AddTopWallTrim(glm::vec3(cornerB.x, 0, cornerA.z), ROTATE_270, cornerB.z - cornerA.z);
		AddTopWallTrim(glm::vec3(cornerA.x, 0, cornerB.z), ROTATE_90, cornerB.z - cornerA.z);

		// Sort doors
		std::sort(doors_FrontWall.begin(), doors_FrontWall.end(), [](const Door* a, const Door* b) { return a->position.x < b->position.x; });
		std::sort(doors_BackWall.begin(), doors_BackWall.end(), [](const Door* a, const Door* b) { return a->position.x > b->position.x; });
		std::sort(doors_LeftWall.begin(), doors_LeftWall.end(), [](const Door* a, const Door* b) { return a->position.z > b->position.z; });
		std::sort(doors_RightWall.begin(), doors_RightWall.end(), [](const Door* a, const Door* b) { return a->position.z < b->position.z; });

		// Front walls
		cursorX = cornerA.x;
		cursorZ = cornerA.z;
		for (int i = 0; i < doors_FrontWall.size(); i++)
		{
			float wallWidth = doors_FrontWall[i]->position.x - cursorX - (doorWidth / 2);
			wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX + wallWidth, roomHeight, cursorZ), X); // wall
			AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), 0, wallWidth);			
			cursorX += wallWidth + doorWidth;
			wallMesh.AddQuad(glm::vec3(cursorX - doorWidth, doorHeight, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ), X); // above door
		}
		wallWidth = cornerB.x - cursorX;
		wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX + wallWidth, roomHeight, cursorZ), X); // wall
		AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), 0, wallWidth);


		
		// Back walls
		cursorX = cornerB.x;
		cursorZ = cornerB.z;
		for (int i = 0; i < doors_BackWall.size(); i++)
		{
			float wallWidth = cursorX - doors_BackWall[i]->position.x - (doorWidth / 2);
			wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX - wallWidth, roomHeight, cursorZ), X_NEGATIVE); // wall
			AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_180, wallWidth);			
			cursorX -= wallWidth + doorWidth;
			wallMesh.AddQuad(glm::vec3(cursorX + doorWidth, doorHeight, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ), X_NEGATIVE); // above door
		}
		wallWidth = cursorX - cornerA.x;
		wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX - wallWidth, roomHeight, cursorZ), X_NEGATIVE); // wall
		AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_180, wallWidth);
			
		// Left walls
		cursorZ = cornerB.z;
		cursorX = cornerA.x;
		for (int i = 0; i < doors_LeftWall.size(); i++)
		{
			float wallWidth = cursorZ - doors_LeftWall[i]->position.z - (doorWidth / 2);
			wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ - wallWidth), Z); // wall
			AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_90, wallWidth);
			cursorZ -= wallWidth + doorWidth;
			wallMesh.AddQuad(glm::vec3(cursorX, doorHeight, cursorZ + doorWidth), glm::vec3(cursorX, roomHeight, cursorZ), Z); // above door
		}
		wallWidth = cursorZ - cornerA.z;
		wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ - wallWidth), Z); // wall
		AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_90, wallWidth);
		
		// Right walls
		cursorZ = cornerA.z;
		cursorX = cornerB.x;
		for (int i = 0; i < doors_RightWall.size(); i++)
		{
			float wallWidth = doors_RightWall[i]->position.z - cursorZ - (doorWidth / 2);
			wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ + wallWidth), Z_NEGATIVE); // wall
			AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_270, wallWidth);
			cursorZ += wallWidth + doorWidth;
			wallMesh.AddQuad(glm::vec3(cursorX, doorHeight, cursorZ - doorWidth), glm::vec3(cursorX, roomHeight, cursorZ), Z_NEGATIVE); // above door
		}
		wallWidth = cornerB.z - cursorZ;
		wallMesh.AddQuad(glm::vec3(cursorX, 0, cursorZ), glm::vec3(cursorX, roomHeight, cursorZ + wallWidth), Z_NEGATIVE); // wall
		AddBottomWallTrim(glm::vec3(cursorX, 0, cursorZ), ROTATE_270, wallWidth);
		
	}


	void Room::DrawWallTrim(Shader* shader)
	{
		RenderableObject bottomTrim = RenderableObject("WallTrimFloor", AssetManager::GetModelByName("WallTrim.obj"), NULL);
		RenderableObject topTrim = RenderableObject("WallTrimFloor", AssetManager::GetModelByName("WallTrimTop.obj"), NULL);

		for (Transform transform : bottomTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			bottomTrim.transform = transform;
			bottomTrim.Draw(shader, true);
		}	
		for (Transform transform : topTrimTransforms) {
			shader->setVec2("TEXTURE_SCALE", glm::vec2(transform.scale.x, 1.0));
			topTrim.transform = transform;
			topTrim.Draw(shader, true);
		}
	}

	void Room::BuildWalls()
	{
		// WALLS
		walls.clear();
		wallHoles.clear();

		// Misc. Organise this somewhere.
		//unsigned int wallMaterialID = 
		//Material* material = Material::GetMaterialByName(wallMaterialName);
		float doorWidth = 1;

		// Build x walls
		{
			// Setup
			std::sort(doors_FrontWall.begin(), doors_FrontWall.end(),
				[](const Door* a, const Door* b) { return a->position.x < b->position.x; });
			float cursorX = cornerA.x;
			float cursorZ = cornerA.z;

			// Find and fill any door gaps
			for (int i = 0; i < doors_FrontWall.size(); i++) {
				float wallWidth = doors_FrontWall[i]->position.x - cursorX - (doorWidth / 2);
				Wall wall = Wall(cursorX, cursorZ, X, wallWidth, wallMaterialID);
				walls.push_back(wall);
				cursorX += wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX - doorWidth,  0,  cursorZ), X });

				////////////////////////////////////////////////////////////////////////////////////////
				// door hole
				DoorHole doorHole;
				doorHole.bottomLeft = glm::vec3(cursorX - 0.1f, 0, cursorZ);
				doorHole.bottomRight = glm::vec3(cursorX - 0.9f, 0, cursorZ);
				doorHole.topLeft = glm::vec3(cursorX - 0.1f, 2, cursorZ);
				doorHole.topRight = glm::vec3(cursorX - 0.9f, 2, cursorZ);
				doorHoles_FrontWall.push_back(doorHole);
				////////////////////////////////////////////////////////////////////////////////////////
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cornerB.x - cursorX;
			Wall wall = Wall(cursorX, cursorZ, X, wallWidth, wallMaterialID);
			walls.push_back(wall);
		}
		// Build X Negative Walls
		{
			// Setup
			std::sort(doors_BackWall.begin(), doors_BackWall.end(),
				[](const Door* a, const Door* b) { return a->position.x > b->position.x; });
			float cursorX = cornerB.x;
			float cursorZ = cornerB.z;

			// Find and fill any door gaps
			for (int i = 0; i < doors_BackWall.size(); i++) {
				float wallWidth = cursorX - doors_BackWall[i]->position.x - (doorWidth / 2);
				Wall wall = Wall(cursorX, cursorZ, X_NEGATIVE, wallWidth, wallMaterialID);
				walls.push_back(wall);
				cursorX -= wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX + doorWidth,  0,  cursorZ), X_NEGATIVE });


				////////////////////////////////////////////////////////////////////////////////////////
				// door hole
				DoorHole doorHole;
				doorHole.bottomLeft = glm::vec3(cursorX + 0.1f, 0, cursorZ);
				doorHole.bottomRight = glm::vec3(cursorX + 0.9f, 0, cursorZ);
				doorHole.topLeft = glm::vec3(cursorX + 0.1f, 2, cursorZ);
				doorHole.topRight = glm::vec3(cursorX + 0.9f, 2, cursorZ);
				doorHoles_BackWall.push_back(doorHole);
				////////////////////////////////////////////////////////////////////////////////////////
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cursorX - cornerA.x;
			Wall wall = Wall(cursorX, cursorZ, X_NEGATIVE, wallWidth, wallMaterialID);
			walls.push_back(wall);
		}

		// Build Z walls
		{
			// Setup
			std::sort(doors_LeftWall.begin(), doors_LeftWall.end(),
				[](const Door* a, const Door* b) { return a->position.z > b->position.z; });
			float cursorZ = cornerB.z;
			float cursorX = cornerA.x;

			// Find and fill any door gaps
			for (int i = 0; i < doors_LeftWall.size(); i++) {
				float wallWidth = cursorZ - doors_LeftWall[i]->position.z - (doorWidth / 2);
				Wall wall = Wall(cursorX, cursorZ, Z, wallWidth, wallMaterialID);
				walls.push_back(wall);
				cursorZ -= wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX,  0,  cursorZ + doorWidth), Z });

				////////////////////////////////////////////////////////////////////////////////////////
				// door hole
				DoorHole doorHole;
				doorHole.bottomLeft = glm::vec3(cursorX, 0, cursorZ + 0.1f);
				doorHole.bottomRight = glm::vec3(cursorX, 0, cursorZ + 0.9f);
				doorHole.topLeft = glm::vec3(cursorX, 2, cursorZ + 0.1f);
				doorHole.topRight = glm::vec3(cursorX, 2, cursorZ + 0.9f);
				doorHoles_LeftWall.push_back(doorHole);
				////////////////////////////////////////////////////////////////////////////////////////
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cursorZ - cornerA.z;
			Wall wall = Wall(cursorX, cursorZ, Z, wallWidth, wallMaterialID);
			walls.push_back(wall);
		}

		// Build Z_Negative walls
		{
			// Setup
			std::sort(doors_RightWall.begin(), doors_RightWall.end(),
				[](const Door* a, const Door* b) { return a->position.z < b->position.z; });
			float cursorZ = cornerA.z;
			float cursorX = cornerB.x;

			// Find and fill any door gaps
			for (int i = 0; i < doors_RightWall.size(); i++) {
				float wallWidth = doors_RightWall[i]->position.z - cursorZ - (doorWidth / 2);
				Wall wall = Wall(cursorX, cursorZ, Z_NEGATIVE, wallWidth, wallMaterialID);
				walls.push_back(wall);
				cursorZ += wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX,  0,  cursorZ - doorWidth), Z_NEGATIVE });

				////////////////////////////////////////////////////////////////////////////////////////
				// door hole
				DoorHole doorHole;
				doorHole.bottomLeft = glm::vec3(cursorX, 0, cursorZ - 0.1f);
				doorHole.bottomRight = glm::vec3(cursorX, 0, cursorZ - 0.9f);
				doorHole.topLeft = glm::vec3(cursorX, 2, cursorZ - 0.1f);
				doorHole.topRight = glm::vec3(cursorX, 2, cursorZ - 0.9f);
				doorHoles_RightWall.push_back(doorHole);
				////////////////////////////////////////////////////////////////////////////////////////
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cornerB.z - cursorZ;
			Wall wall = Wall(cursorX, cursorZ, Z_NEGATIVE, wallWidth, wallMaterialID);
			walls.push_back(wall);
		}
		for (Wall & wall : walls)
			wall.CalculateModelMatrix();

	}

	float Room::GetLowerXCoord()
	{
		return std::min(cornerA.x, cornerB.x);
	}

	float Room::GetLowerZCoord()
	{
		return std::min(cornerA.z, cornerB.z);
	}

	float Room::GetUpperXCoord()
	{
		return std::max(cornerA.x, cornerB.x);
	}

	float Room::GetUpperZCoord()
	{
		return std::max(cornerA.z, cornerB.z);
	}

	void Room::CalculateLightLimits()
	{
		// This function sets 4 values which limit the fragment shader from shading fragments outside this room.
		// If there is an open door on a wall, the light radius is the limit, otherwise it's the wall.

		light.CalculateRadius(); // TODO: don't calculate this every frame. only when lights change

		// LEFT WALL
		light.lowerXlimit = GetLowerXCoord();
		for (Door * door : doors_LeftWall)
			if (door->doorStatus != DoorStatus::DOOR_CLOSED)
				light.lowerXlimit = light.position.x - light.radius;

		// RIGHT WALL
		light.upperXlimit = GetUpperXCoord();
		for (Door * door : doors_RightWall)
			if (door->doorStatus != DoorStatus::DOOR_CLOSED)
				light.upperXlimit = light.position.x + light.radius;

		// BACK WALL
		light.upperZlimit = GetUpperZCoord();
		for (Door * door : doors_BackWall)
			if (door->doorStatus != DoorStatus::DOOR_CLOSED)
				light.upperZlimit = light.position.z + light.radius;

		// FRONT WALL
		light.lowerZlimit = GetLowerZCoord();
		for (Door * door : doors_FrontWall)
			if (door->doorStatus != DoorStatus::DOOR_CLOSED)
				light.lowerZlimit = light.position.z - light.radius;

		// Fix acne
		float bias = 0.08f;
		light.lowerXlimit -= bias;
		light.upperXlimit += bias;
		light.lowerZlimit -= bias;
		light.upperZlimit += bias;
	}

	void Room::CreateLightVolumes()
	{
		lightVolume.vertices.clear();
		backWallDoorwayLightVolumes.clear();
		frontWallDoorwayLightVolumes.clear();
		leftWallDoorwayLightVolumes.clear();
		rightWallDoorwayLightVolumes.clear();

		float bias = Light_Volume_Bias;
		float lowerX = GetLowerXCoord();
		float lowerZ = GetLowerZCoord();
		float upperX = GetUpperXCoord();
		float upperZ = GetUpperZCoord();

		glm::vec3 A1 = glm::vec3(lowerX - bias, -bias, lowerZ - bias);
		glm::vec3 B1 = glm::vec3(upperX + bias, -bias, lowerZ - bias);
		glm::vec3 C1 = glm::vec3(lowerX - bias, -bias, upperZ + bias);
		glm::vec3 D1 = glm::vec3(upperX + bias, -bias, upperZ + bias);

		glm::vec3 A2 = glm::vec3(lowerX - bias, 2.4f + bias, lowerZ - bias);
		glm::vec3 B2 = glm::vec3(upperX + bias, 2.4f + bias, lowerZ - bias);
		glm::vec3 C2 = glm::vec3(lowerX - bias, 2.4f + bias, upperZ + bias);
		glm::vec3 D2 = glm::vec3(upperX + bias, 2.4f + bias, upperZ + bias);
		
		// Floor
		lightVolume.vertices.push_back(A1);
		lightVolume.vertices.push_back(B1);
		lightVolume.vertices.push_back(C1);
		lightVolume.vertices.push_back(D1);
		lightVolume.vertices.push_back(C1);
		lightVolume.vertices.push_back(B1);

		// Ceiling
		lightVolume.vertices.push_back(C2);
		lightVolume.vertices.push_back(B2);
		lightVolume.vertices.push_back(A2);
		lightVolume.vertices.push_back(B2);
		lightVolume.vertices.push_back(C2);
		lightVolume.vertices.push_back(D2);

		// Side wall
		lightVolume.vertices.push_back(C1);
		lightVolume.vertices.push_back(A2);
		lightVolume.vertices.push_back(A1);
		lightVolume.vertices.push_back(C2);
		lightVolume.vertices.push_back(A2);
		lightVolume.vertices.push_back(C1);

		// Side wall the other one
		lightVolume.vertices.push_back(B1);
		lightVolume.vertices.push_back(B2);
		lightVolume.vertices.push_back(D1);
		lightVolume.vertices.push_back(D1);
		lightVolume.vertices.push_back(B2);
		lightVolume.vertices.push_back(D2);

		// Front wall
		lightVolume.vertices.push_back(A1);
		lightVolume.vertices.push_back(B2);
		lightVolume.vertices.push_back(B1);
		lightVolume.vertices.push_back(A1);
		lightVolume.vertices.push_back(A2);
		lightVolume.vertices.push_back(B2);

		// Back wall
		lightVolume.vertices.push_back(D1);
		lightVolume.vertices.push_back(D2);
		lightVolume.vertices.push_back(C1);
		lightVolume.vertices.push_back(D2);
		lightVolume.vertices.push_back(C2);
		lightVolume.vertices.push_back(C1);
		
		lightVolume.Setup();

		light.CalculateRadius();

		for (DoorHole & doorHole : doorHoles_BackWall)
			backWallDoorwayLightVolumes.push_back(LightVolume::BuildLightVolumeFromDoorHole(doorHole, light, 0, bias));
		
		for (DoorHole & doorHole : doorHoles_FrontWall)
			frontWallDoorwayLightVolumes.push_back(LightVolume::BuildLightVolumeFromDoorHole(doorHole, light, 0, bias));
		
		for (DoorHole & doorHole : doorHoles_LeftWall)
			leftWallDoorwayLightVolumes.push_back(LightVolume::BuildLightVolumeFromDoorHole(doorHole, light, 0, bias));
		
		for (DoorHole & doorHole : doorHoles_RightWall)
			rightWallDoorwayLightVolumes.push_back(LightVolume::BuildLightVolumeFromDoorHole(doorHole, light, 0, bias));
	}
	void Room::SetWallMaterial(unsigned int materialID)
	{
		for (Wall & wall : walls)
			wall.materialID = materialID;

		this->wallMaterialID = materialID;
	}

	void Room::ResizeFloorAndCeiling()
	{
		this->floor = Floor(cornerA, width, depth, floor.materialID, floor.rotateTexture);
		this->ceiling = Ceiling(cornerA, width, depth, ceiling.materialID, ceiling.rotateTexture);
	}

	void Room::CenterLight()
	{
		this->light.position.x = cornerA.x + width / 2;
		this->light.position.z = cornerA.z + depth / 2;
		light.CalculateShadowProjectionMatricies();
	}
}
