#include "hellpch.h"
#include "Door.h"
#include "HellEngine/Audio/Audio.h"


namespace HellEngine {

	Model* Door::modelDoor = nullptr;
	Model* Door::modelDoorShadowCaster = nullptr;
	Model* Door::modelDoorJam = nullptr;
	Model* Door::modelWallHole = nullptr;

	Door::Door(float x, float z, Axis axis, std::string floorMaterialName, bool rotateFloor)
	{
		this->position = glm::vec3(x, 0, z);
		this->axis = axis;

		glm::vec3 doorBoundingBoxSize = glm::vec3(0.79f, 1.97, 0.04f);
		boundingBox = BoundingBox(GetDoorModelPosition(), doorBoundingBoxSize, ObjectOrigin::BOTTOM_LEFT_FRONT_CORNER);
		boundingBox.SetAngle(this->GetAngle());

		// Floor
		if (axis == X)
			this->floor = Floor(glm::vec3(x - 0.5f, 0, z - 0.1f), 1, 0.1f, Material::GetMaterialByName(floorMaterialName), rotateFloor);
		if (axis == X_NEGATIVE)
			this->floor = Floor(glm::vec3(x - 0.5f, 0, z - 0.1f), 1, 0.1f, Material::GetMaterialByName(floorMaterialName), rotateFloor);
		if (axis == Z)
			this->floor = Floor(glm::vec3(x - 0.1f, 0, z - 0.5f), 0.1f, 1, Material::GetMaterialByName(floorMaterialName), rotateFloor);
		if (axis == Z_NEGATIVE)
			this->floor = Floor(glm::vec3(x - 0.1f, 0, z - 0.5f), 0.1f, 1, Material::GetMaterialByName(floorMaterialName), rotateFloor);
	}

	Door::~Door()
	{
	}

	void Door::Update(float deltaTime)
	{
		float openSpeed = 5;

		if (doorStatus == DOOR_OPENING)
			openAngle += openSpeed * deltaTime;
		if (doorStatus == DOOR_CLOSING)
			openAngle -= openSpeed * deltaTime;

		if (openAngle > maxOpenAngle)
			doorStatus = DOOR_OPEN;
		if (openAngle < 0)
			doorStatus = DOOR_CLOSED;

		openAngle = std::max(0.0f, openAngle);
		openAngle = std::min(openAngle, maxOpenAngle);

		// Recreate the bounding box if it has changed shape (aka the door angle changed)
		if (doorStatus == DOOR_OPENING || doorStatus == DOOR_CLOSING)
			boundingBox.SetAngle(this->GetAngle());
	}

	void Door::Interact()
	{
		Audio::PlayAudio("Door1.wav");

		if (doorStatus == DOOR_CLOSED)
			doorStatus = DOOR_OPENING;
		if (doorStatus == DOOR_OPEN)
			doorStatus = DOOR_CLOSING;
	}

	float Door::GetAngle()
	{
		return GetBaseAngle() - openAngle;
	}

	float Door::GetBaseAngle()
	{
		if (axis == X)
			return 0;
		else if (axis == X_NEGATIVE)
			return ROTATE_180;
		else if (axis == Z)
			return ROTATE_90;
		else if (axis == Z_NEGATIVE)
			return ROTATE_270;
	}

	float Door::GetJamAngle()
	{
		if (axis == X)
			return 0;
		else if (axis == X_NEGATIVE)
			return 0;
		else if (axis == Z)
			return ROTATE_90;
		else if (axis == Z_NEGATIVE)
			return ROTATE_90;
	}

	glm::vec3 Door::GetDoorModelPosition()
	{
		if (axis == X)
		{
			float x = position.x - 0.4f;
			float y = position.y;
			float z = position.z - 0.005f;
			return glm::vec3(x, y, z);
		}
		else if (axis == X_NEGATIVE)
		{
			float x = position.x + 0.400f - 0.01f;
			float y = position.y;
			float z = position.z + 0.005f - 0.1f;
			return glm::vec3(x, y, z);
		}
		else if (axis == Z)
		{
			float x = position.x - 0.005f;
			float y = position.y;
			float z = position.z + 0.4f;
			return glm::vec3(x, y, z);
		}
		else if (axis == Z_NEGATIVE)
		{
			float x = position.x + 0.005f - 0.1f;
			float y = position.y;
			float z = position.z - 0.4f;
			return glm::vec3(x, y, z);
		}
		else
			return glm::vec3(0, 0, 0);
	}
	glm::mat4 Door::GetModelMatrix()
	{
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, GetDoorModelPosition());
		modelMatrix = glm::rotate(modelMatrix,GetAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
		return modelMatrix;
	}
}