#include "hellpch.h"
#include "House.h"
#include "Platform/OpenGL/RenderableObject.h"
#include "HellEngine/Util.h"
#include "HellEngine/AssetManager.h"
#include <algorithm>

namespace HellEngine {

	std::vector<Door> doors;
	std::vector<Wall> walls;
	std::vector<Room> rooms;
	
	House::House()
	{
	}

	House::~House()
	{
	}

	void House::RebuildRooms()
	{
		for (Room & room : rooms)
			room.BuildAll(doors);

		for (Room& room : rooms)
			room.ConstructWallMesh();

		// Staris
		stairMesh = StairMesh(10);
		
	}

	void House::RemoveAllContents()
	{
		doors.clear();
		rooms.clear();
	}

	void House::DrawAll (Shader* shader, bool bindTextures)
	{
		// Draw Walls
		if (bindTextures) 
			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("WallPaper"));
		for (Room room : rooms)
			room.wallMesh.Draw(shader);

		// Draw Trim
		if (bindTextures) 
			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("Wood"));
		for (Room room : rooms)
			room.DrawWallTrim(shader);
		shader->setVec2("TEXTURE_SCALE", glm::vec2(1.0));

		// Draw Doors
		if (bindTextures) 
			AssetManager::BindMaterial(Door::materialID_Door);
		for (int i = 0; i < doors.size(); i++) {
			shader->setMat4("model", doors[i].GetModelMatrix());
			AssetManager::DrawModel(shader, Door::modelID_Door); 
		}

		// Draw Door Frame 
		if (bindTextures)
			AssetManager::BindMaterial(Door::materialID_Door);
		for (Door& door : doors)
		{
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, door.position);
			modelMatrix = glm::rotate(modelMatrix, door.GetJamAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
			shader->setMat4("model", modelMatrix);
			AssetManager::DrawModel(shader, Door::modelID_DoorFrame);
		}

		// Draw door floor
		for (Door & door : doors)
			door.floor.Draw(shader, bindTextures);

		//Draw Floor and Ceiling
		for (Room & room : rooms)
		{
			room.floor.Draw(shader, bindTextures);
			room.ceiling.Draw(shader, bindTextures);

			// Draw Light Bulbs
			if (bindTextures) {
				unsigned int lightMaterialID = AssetManager::GetMaterialIDByName("Light");

				RenderableObject lightObject = RenderableObject("Light", AssetManager::GetModelByName("Light.obj"), NULL);
				lightObject.transform.position = glm::vec3(room.light.position.x, 2.4125f, room.light.position.z);
				lightObject.emissiveColor = room.light.color;

				AssetManager::BindMaterial(lightMaterialID);
				lightObject.Draw(shader, true);
			}
			
		}

		// Stairs
		unsigned int materialID = AssetManager::GetMaterialIDByName("Wood");
		AssetManager::BindMaterial(materialID);
		stairMesh.Draw(shader);
	}

	void House::AddDoor(float x, float z, Axis axis, unsigned int floorMaterialID, bool initiallyOpen, bool initiallyLocked, float maxOpenAngle, bool rotateFloorTexture)
	{
		Door door(x, z, axis, floorMaterialID, initiallyOpen, initiallyLocked, maxOpenAngle, rotateFloorTexture);
		doors.push_back(door);
	}

	void House::AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling)
	{
		// No light specified, so it just centers one with default settings.
		rooms.push_back(Room(cornerA, cornerB, wallMaterialID, floorMaterialID, ceilingMaterialID, rotateFloor, rotateCeiling));
	}
	
	void House::AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, unsigned int  wallMaterialID, unsigned int floorMaterialID, unsigned int ceilingMaterialID, bool rotateFloor, bool rotateCeiling, Light light)
	{
		// Specify light
		rooms.push_back(Room(cornerA, cornerB, wallMaterialID, floorMaterialID, ceilingMaterialID, rotateFloor, rotateCeiling, light));
	}
}