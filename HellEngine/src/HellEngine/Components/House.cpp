#include "hellpch.h"
#include "House.h"
#include "Platform/OpenGL/RenderableObject.h"

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
		
		BufferWallMatrices();
	}

	void House::RemoveAllContents()
	{
		doors.clear();
		rooms.clear();
	}

	void House::DrawAll (Shader* shader, bool bindTextures)
	{
		/*
		shader->setInt("instanced", true);
		Mesh* mesh = &Wall::model->meshes[0];
		glBindVertexArray(mesh->VAO);
		glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, wallModelMatrices.size());
		glBindVertexArray(0);		
		
		shader->setInt("instanced", false);
		*/ 
		// Draw non instanced wall holes (plus the walls cause iv uncommented it coz instancing is too less of a gain to warrent adding texture coords
		shader->setInt("hasNormalMap", true); 
			for (Room & room : rooms)
			room.DrawWalls(shader, bindTextures);
		shader->setInt("hasNormalMap", false);

		// Draw doors
		if (bindTextures) {
			shader->setInt("PBR", true);
			
			/*glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("DoorFinal_DefaultMaterial_BaseColor.png"));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("DoorFinal_DefaultMaterial_Metallic.png"));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("DoorFinal_DefaultMaterial_Roughness.png"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("DoorFinal_DefaultMaterial_Normal.png"));
			*/

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_Base_Color.png"));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_Material_Metallic.png"));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_Material_Roughness.png"));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_NormalMap.png"));
		}

		// Non instaned doorr rendering (with colour picker)
		for (int i = 0; i < doors.size(); i++)
		{
			
			shader->setFloat("MousePickColor", (i + 1) / 256.0f);					// IMPORTANT !!!!!!!!!!!!!!!!11
			shader->setMat4("model", doors[i].GetModelMatrix());
			

			if (bindTextures)
				doors[i].modelDoor->Draw(shader);
			else
				doors[i].modelDoorShadowCaster->Draw(shader);
		}
		shader->setFloat("MousePickColor", 0); // no more mouse picking

		// Draw doors instanced instanced
		/*shader->setInt("instanced", true);
		std::vector<glm::mat4> doorMatrices;
		for (Door & door : doors)
			doorMatrices.push_back(door.GetModelMatrix());

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, doorMatrices.size() * sizeof(glm::mat4), &doorMatrices[0], GL_STATIC_DRAW); // GL_STREAM_DRAW

		mesh = &Door::modelDoor->meshes[0];
		glBindVertexArray(mesh->VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glVertexAttribDivisor(9, 1);
		glVertexAttribDivisor(10, 1);
		//glBindVertexArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, doorMatrices.size());
		glBindVertexArray(0);

		shader->setInt("instanced", false);*/




		// Draw non instanced DOORJAMS
		DrawDoors(shader, bindTextures);

		// Render door floor
		for (Door & door : doors)
			door.floor.Draw(shader, bindTextures);



		for (Room & room : rooms)
		{
			// Floor and ceiling (using the door setting above)
			room.floor.Draw(shader, bindTextures);
			room.ceiling.Draw(shader, bindTextures);

			// Lights
			if (bindTextures) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Light_BaseColor.png"));
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Light_Metallic.png"));
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Light_Roughness.png"));
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Light_NormalMap.png"));

				RenderableObject lightObject = RenderableObject("Light", Model::GetByName("Light.obj"));
				lightObject.diffuseTextureID = Texture::GetIDByName("Light_BaseColor.png");
				lightObject.normalMapID = Texture::GetIDByName("Light_NormalMap.png");
				lightObject.roughnessTextureID = Texture::GetIDByName("Light_Roughness.png");
				lightObject.metallicTextureID = Texture::GetIDByName("Light_Metallic.png");
				lightObject.emissiveMapID = Texture::GetIDByName("Light_EmissiveMap.png");
				lightObject.transform.position = glm::vec3(room.light.position.x, 2.4f, room.light.position.z);
				lightObject.emissiveColor = room.light.color;
				lightObject.hasEmissiveMap = true;

				//std::cout << lightObject.hasEmissiveMap << "\n";
					
				//Model* lightModel = Model::GetByName("Light.obj");
				//glm::mat4 modelMatrix = glm::mat4(1.0f);
				//modelMatrix = glm::translate(modelMatrix, glm::vec3(room.light.position.x, 2.4f, room.light.position.z));
				//shader->setMat4("model", modelMatrix);
				lightObject.Draw(shader, true);
			}
			
		}
	}

	/*void House::DrawWalls(Shader* shader, bool bindTextures)
	{
		// WALLS
		if (bindTextures) 
		{
			shader->setInt("PBR", true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("wall.png"));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Wall_Material_Metallic.png"));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Wall_Material_Roughness.png"));
		}
		for (Wall & wall : walls)
		{
			float x = wall.position.x;
			float y = 0.0f;
			float z = wall.position.z;
			float angle = wall.angle;

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
			modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			shader->setMat4("model", modelMatrix);
			wall.model->Draw(shader);
		}
		shader->setInt("PBR", false);
	}*/

	void House::DrawDoors(Shader* shader, bool bindTextures)
	{
		// Doors
	/*	for (int i = 0; i < doors.size(); i++)
		{
			shader->setFloat("MousePickColor", (i + 1) / 256.0f);

			if (bindTextures) {
				shader->setInt("PBR", true);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("door.png"));
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_Material_Metallic.png"));
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Door_Material_Roughness.png"));
			}
			shader->setMat4("model", doors[i].GetModelMatrix());
			doors[i].modelDoor->Draw(shader);
		}
		shader->setFloat("MousePickColor", 0);
	*/

		// Frame and jam
		for (Door & door : doors)
		{
			float x = door.position.x;
			float y = door.position.y;
			float z = door.position.z;

			// Translation
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
			modelMatrix = glm::rotate(modelMatrix, door.GetJamAngle(), glm::vec3(0.0f, 1.0f, 0.0f));

			shader->setMat4("model", modelMatrix);


			if (bindTextures) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Wood_Material_Base_Color.png"));
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Wood_Material_Metallic.png"));
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("Wood_Material_Roughness.png"));
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName("EmptyNormalMap.png"));
			}
			door.modelDoorJam->Draw(shader);
		}
	}


	void House::AddDoor(float x, float z, Axis axis, std::string floorMaterialName, bool rotateFloorTexture)
	{
		Door door(x, z, axis, floorMaterialName, rotateFloorTexture);
		doors.push_back(door);
	}

	void House::AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, std::string wallMaterialName, std::string floorMaterialName, std::string ceilingMaterialName, bool rotateFloor, bool rotateCeiling)
	{
		// No light specified, so it just centers one with default settings.
		rooms.push_back(Room(cornerA, cornerB, wallMaterialName, floorMaterialName, ceilingMaterialName, rotateFloor, rotateCeiling));
	}
	
	void House::AddRoom(glm::vec3 cornerA, glm::vec3 cornerB, std::string wallMaterialName, std::string floorMaterialName, std::string ceilingMaterialName, bool rotateFloor, bool rotateCeiling, Light light)
	{
		// Specify light
		rooms.push_back(Room(cornerA, cornerB, wallMaterialName, floorMaterialName, ceilingMaterialName, rotateFloor, rotateCeiling, light));
	}

	void House::BufferWallMatrices()
	{
		wallModelMatrices.clear();
		for (Room & room : rooms)
			for (Wall & wall : room.walls)
				wallModelMatrices.push_back(wall.modelMatrix);

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, wallModelMatrices.size() * sizeof(glm::mat4), &wallModelMatrices[0], GL_STATIC_DRAW); // GL_STREAM_DRAW

		glBindVertexArray(Wall::model->meshes[0].VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glVertexAttribDivisor(9, 1);
		glVertexAttribDivisor(10, 1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void House::LoadTestScene()
	{	
		this->position = glm::vec3(0, 0, 0);
		rooms.clear();
		doors.clear();

		AddRoom(glm::vec3(0.0f, 0, 0), glm::vec3(1.2f, 0, 6.6f), "Wall", "FloorBoards", "WoodPaintedBoards", false, false);		// Hall
		AddRoom(glm::vec3(-4.1f, 0, 0.5f), glm::vec3(-0.1f, 0, 3.5f), "Wall", "WoodPaintedBoards", "WoodPaintedBoards", false, false);	// Side room A
		AddRoom(glm::vec3(-4.1f, 0, 3.6f), glm::vec3(-0.1f, 0, 6.6f), "Wall", "Lino", "WoodPaintedBoards", false, false);	// Side room B
		AddRoom(glm::vec3(-2.0f, 0, 6.7f), glm::vec3(3.2f, 0, 9.6f), "Wall", "FloorBoards", "WoodPaintedBoards", false, false);	// Back room

		AddDoor(0.6f, 0, X, "FloorBoards", false);
		AddDoor(0.0f, 2, Z_NEGATIVE, "FloorBoards", false);
		AddDoor(0.0f, 5.1f, Z_NEGATIVE, "FloorBoards", false);
		AddDoor(-3.0f, 3.6f, X_NEGATIVE, "FloorBoards", false);
		AddDoor(0.6f, 6.7f, X, "FloorBoards", false);
		AddDoor(1.3f, 2, Z, "FloorBoards", false);

		rooms[0].light.strength = 5.0f;
		rooms[1].light.strength = 2.01f;
		rooms[2].light.strength = 5.0f;
		rooms[3].light.strength = 2.964f;
		rooms[0].light.color = glm::vec3(1.0f, 0.780, 0.529f);
		rooms[1].light.color = glm::vec3(0.819f, 0.788f, 0.784f);
		rooms[2].light.color = glm::vec3(1.0f, 0.780, 0.529f);

		rooms[0].light.strength = 8.0f;
		rooms[1].light.strength = 8.0f;
		rooms[2].light.strength = 8.0f;
		rooms[3].light.strength = 8.0;
		rooms[0].light.color = glm::vec3(1.0f, 0.780, 0.529f);
		rooms[1].light.color = glm::vec3(1.0f, 0.780, 0.529f);
		rooms[2].light.color = glm::vec3(1.0f, 0.780, 0.529f);
		rooms[3].light.color = glm::vec3(1.0f, 0.780, 0.529f);

		// Red room
		rooms[2].light.strength = 40.0f;
		rooms[2].light.attExp = 1.8f;
		rooms[2].light.attConstant = 0.0f;
		rooms[2].light.attLinear = 0.0f;
		rooms[2].light.color = glm::vec3(1.0f, 0, 0);
	}
}