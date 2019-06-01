#include "hellpch.h"
#include "Room.h"
#include "HellEngine/Util.h"

namespace HellEngine
{
	Room::Room(glm::vec3 cornerA, glm::vec3 cornerB)
	{
		this->cornerA = cornerA;
		this->cornerB = cornerB;

		float width = cornerB.x - cornerA.x;
		float depth = cornerB.z - cornerA.z;

		this->light = Light(cornerA.x + width / 2, 2.0f, cornerA.z + depth / 2);
	}

	Room::~Room()
	{
		// Destructor
	}

	void Room::Build(std::vector<Door>& doors)
	{
		FindDoors(doors);
		BuildWalls();
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
	}

	void Room::BuildWalls()
	{
		float totalWidth = cornerB.x - cornerA.x;
		float totalDepth = cornerB.z - cornerA.z;
		this->floor = Floor(cornerA, totalWidth, totalDepth);
		this->ceiling = Ceiling(cornerA, totalWidth, totalDepth);

		// WALLS
		walls.clear();
		wallHoles.clear();


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
				Wall wall = Wall(cursorX, cursorZ, X, wallWidth);
				walls.push_back(wall);
				cursorX += wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX - doorWidth,  0,  cursorZ), X });

			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cornerB.x - cursorX;
			Wall wall = Wall(cursorX, cursorZ, X, wallWidth);
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
				Wall wall = Wall(cursorX, cursorZ, X_NEGATIVE, wallWidth);
				walls.push_back(wall);
				cursorX -= wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX + doorWidth,  0,  cursorZ), X_NEGATIVE });
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cursorX - cornerA.x;
			Wall wall = Wall(cursorX, cursorZ, X_NEGATIVE, wallWidth);
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
				Wall wall = Wall(cursorX, cursorZ, Z, wallWidth);
				walls.push_back(wall);
				cursorZ -= wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX,  0,  cursorZ + doorWidth), Z });
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cursorZ - cornerA.z;
			Wall wall = Wall(cursorX, cursorZ, Z, wallWidth);
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
				Wall wall = Wall(cursorX, cursorZ, Z_NEGATIVE, wallWidth);
				walls.push_back(wall);
				cursorZ += wallWidth + doorWidth;
				wallHoles.push_back({ glm::vec3(cursorX,  0,  cursorZ - doorWidth), Z_NEGATIVE });
			}
			// Fill the final gap (if there's no doors this will be the entire wall)
			float wallWidth = cornerB.z - cursorZ;
			Wall wall = Wall(cursorX, cursorZ, Z_NEGATIVE, wallWidth);
			walls.push_back(wall);
		}
		for (Wall & wall : walls)
			wall.CalculateModelMatrix();
	}

	void Room::DrawWalls(Shader* shader, bool bindTextures)
	{

		shader->setInt("worldBasedTexCoords", true);

		// X aligned
		shader->setInt("X_Aligned", true);
		for (Wall & wall : walls)
			if (wall.axis == X || wall.axis == X_NEGATIVE)
				wall.Draw(shader, bindTextures);
		
		for (WallHole & wallHole : wallHoles) {
			if (wallHole.axis == X || wallHole.axis == X_NEGATIVE) {
				Model* wallHoleModel = Model::GetByName("Wall_DoorHole.obj");
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, wallHole.position);
				modelMatrix = glm::rotate(modelMatrix, Util::AxisToAngle(wallHole.axis), glm::vec3(0.0f, 1.0f, 0.0f));
				shader->setMat4("model", modelMatrix);
				wallHoleModel->Draw(shader);
			}
		}

		// Z aligned
		shader->setInt("X_Aligned", false);
		for (Wall & wall : walls)
			if (wall.axis == Z || wall.axis == Z_NEGATIVE)
				wall.Draw(shader, true);
				
		for (WallHole & wallHole : wallHoles) {
			if (wallHole.axis == Z || wallHole.axis == Z_NEGATIVE) {
				Model* wallHoleModel = Model::GetByName("Wall_DoorHole.obj");
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, wallHole.position);
				modelMatrix = glm::rotate(modelMatrix, Util::AxisToAngle(wallHole.axis), glm::vec3(0.0f, 1.0f, 0.0f));
				shader->setMat4("model", modelMatrix);
				wallHoleModel->Draw(shader);
			}
		}
		shader->setInt("worldBasedTexCoords", false);
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
		float bias = 0.09f;
		light.lowerXlimit -= bias;
		light.upperXlimit += bias;
		light.lowerZlimit -= bias;
		light.upperZlimit += bias;
	}
}
