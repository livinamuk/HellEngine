#include "hellpch.h"
#include "Door.h"
#include "HellEngine/Audio/Audio.h"
#include "Platform/OpenGL/Quad2D.h"
#include "HellEngine/AssetManager.h"


namespace HellEngine {

	unsigned int Door::materialID_Door;
	unsigned int Door::materialID_DoorFrame;
	unsigned int Door::modelID_Door;
	unsigned int Door::modelID_DoorFrame;

	bool Door::PlayerHasKey = false;

	Door::Door(float x, float z, Axis axis, unsigned int floorMaterialID, bool initiallyOpen, bool initiallyLocked, float maxOpenAngle, bool rotateFloor)
	{
		// May as well do this here. Over and over lol.
		materialID_Door = AssetManager::GetMaterialIDByName("Door");
		materialID_DoorFrame = AssetManager::GetMaterialIDByName("Wood");
		modelID_Door = AssetManager::GetModelIDByName("door.obj");
		modelID_DoorFrame = AssetManager::GetModelIDByName("Door_jam.obj");

		this->position = glm::vec3(x, 0, z);
		this->axis = axis;
		this->initiallyLocked = initiallyLocked;
		this->locked = initiallyLocked;
		this->maxOpenAngle = maxOpenAngle;

		if (initiallyOpen)
			doorStatus = DoorStatus::DOOR_OPEN;
		else
			doorStatus = DoorStatus::DOOR_CLOSED;

		// Floor
		if (axis == X)
			this->floor = Floor(glm::vec3(x - 0.5f, 0, z - 0.1f), 1, 0.1f, floorMaterialID, rotateFloor);
		if (axis == X_NEGATIVE)
			this->floor = Floor(glm::vec3(x - 0.5f, 0, z - 0.1f), 1, 0.1f, floorMaterialID, rotateFloor);
		if (axis == Z)
			this->floor = Floor(glm::vec3(x - 0.1f, 0, z - 0.5f), 0.1f, 1, floorMaterialID, rotateFloor);
		if (axis == Z_NEGATIVE)
			this->floor = Floor(glm::vec3(x - 0.1f, 0, z - 0.5f), 0.1f, 1, floorMaterialID, rotateFloor);

		// Base Transform
		BoundingBox* boundingBox = &AssetManager::GetModelByName("door.obj")->meshes[0].boundingBox;
		baseTransform.setIdentity();		
		if (axis == X) {
			rotationWhenClosed = 0;
			baseTransform.setOrigin(btVector3(position.x, position.y + 1, position.z - (boundingBox->depth / 2)));
			baseTransform.setRotation(btQuaternion(rotationWhenClosed, 0, 0));
		}
		else if (axis == X_NEGATIVE) {
			rotationWhenClosed = ROTATE_180;
			baseTransform.setOrigin(btVector3(position.x, position.y + 1, position.z + 0.015f - 0.1f + boundingBox->depth));
			baseTransform.setRotation(btQuaternion(rotationWhenClosed, 0, 0));
		}
		else if (axis == Z) {
			rotationWhenClosed = ROTATE_90;
			baseTransform.setOrigin(btVector3(position.x + 0.015f - boundingBox->depth, position.y + 1,  position.z));
			baseTransform.setRotation(btQuaternion(rotationWhenClosed, 0, 0));
		}
		else if (axis == Z_NEGATIVE) {
			rotationWhenClosed = ROTATE_270;
			baseTransform.setOrigin(btVector3(position.x + 0.015f - 0.1f + boundingBox->depth, position.y + 1, position.z));
			baseTransform.setRotation(btQuaternion(rotationWhenClosed, 0, 0));
		}
	}

	Door::~Door()
	{
	}

	void Door::DrawBoundingBox(Shader* shader)
	{
		// Position
		float xPos = (float)rigidBody->getCenterOfMassPosition().x();
		float yPos = (float)rigidBody->getCenterOfMassPosition().y();
		float zPos = (float)rigidBody->getCenterOfMassPosition().z();
		modelTransform.position = glm::vec3(xPos, yPos, zPos);

		// Rotation
		btScalar x, y, z;
		rigidBody->getWorldTransform().getRotation().getEulerZYX(x, y, z);
		modelTransform.rotation = glm::vec3(z, y, x);

		// Bounding box
		BoundingBox* boundingBox = &AssetManager::GetModelByName("door.obj")->meshes[0].boundingBox;
		boundingBox->baseTransform.position = glm::vec3(0);

		shader->setMat4("model", modelTransform.to_mat4() * boundingBox->baseTransform.to_mat4());
		boundingBox->Draw(shader);
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


		// posibily reduntant
		openAngle = std::max(0.0f, openAngle);
		openAngle = std::min(openAngle, maxOpenAngle);

		if (doorStatus == DOOR_OPEN)
			openAngle = maxOpenAngle;
		if (doorStatus == DOOR_CLOSED)
			openAngle = 0;
		if (doorStatus == LOCKED_FROM_THE_OTHER_SIDE)
			openAngle = 0;

		UpdateRigidBodyTransfrom();
	}

	void Door::UpdateRigidBodyTransfrom()
	{
		BoundingBox* boundingBox = &AssetManager::GetModelByName("door.obj")->meshes[0].boundingBox;

		btTransform translateXMat;
		translateXMat.setIdentity();
		translateXMat.setOrigin(btVector3(-boundingBox->width / 2, -boundingBox->height / 2, boundingBox->depth / 2));

		btTransform rotateYMat;
		rotateYMat.setIdentity();
		rotateYMat.setRotation(btQuaternion(-openAngle, 0, 0));

		btTransform translateXMat2;
		translateXMat2.setIdentity();
		translateXMat2.setOrigin(btVector3(boundingBox->width / 2, boundingBox->height / 2, -boundingBox->depth / 2));

		btTransform newMat = baseTransform * translateXMat * rotateYMat * translateXMat2;
		rigidBody->setWorldTransform(newMat);
	}
	void Door::Interact(glm::vec3 playerPosition)
	{
		// locked from the otherr side
		if (playerPosition.x < 1.1f && doorStatus == LOCKED_FROM_THE_OTHER_SIDE) {
			Audio::PlayAudio("Door_Locked.wav");
			Quad2D::TypeText("IT'S LOCKED FROM THE OTHER SIDE.", true);
			locked = false;
			return;
		}

		// you unlocked it
		if (playerPosition.x > 1.1f && doorStatus == LOCKED_FROM_THE_OTHER_SIDE) {
			Audio::PlayAudio("Door_Unlock.wav");
			Quad2D::TypeText("YOU UNLOCKED IT.", true);
			locked = false;
			doorStatus = DOOR_CLOSED;
			return;
		}

		// open
		if (!locked)
		{
			if (doorStatus == DOOR_CLOSED) {
				doorStatus = DOOR_OPENING;
				Audio::PlayAudio("Door_Open.wav");
			}
			if (doorStatus == DOOR_OPEN) {
				doorStatus = DOOR_CLOSING;
				Audio::PlayAudio("Door_Open.wav");
			}
			return;
		}

		// locked
		if (locked && !PlayerHasKey) {

			Audio::PlayAudio("Door_Locked.wav");
			Quad2D::TypeText("IT'S LOCKED WITH A KEY.", true);
			return;
		}

		// unlock it
		if (locked && PlayerHasKey) {
			Audio::PlayAudio("Door_Unlock.wav");
			Quad2D::TypeText("YOU UNLOCKED IT.", true);
			locked = false;
			return;
		}
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
		// Position
		float xPos = (float)rigidBody->getCenterOfMassPosition().x();
		float yPos = (float)rigidBody->getCenterOfMassPosition().y();
		float zPos = (float)rigidBody->getCenterOfMassPosition().z();
		modelTransform.position = glm::vec3(xPos, yPos, zPos);

		// Rotation
		btScalar x, y, z;
		rigidBody->getWorldTransform().getRotation().getEulerZYX(x, y, z);
		modelTransform.rotation = glm::vec3(z, y, x);

		// Bounding box
		BoundingBox* boundingBox = &AssetManager::GetModelByName("door.obj")->meshes[0].boundingBox;
		//boundingBox->baseTransform.position = glm::vec3(0);

		// Modifier
		glm::mat4 translation = glm::mat4(1);
		glm::translate(translation, glm::vec3(10.0f, boundingBox->height * 0.5f, boundingBox->depth * 0.5f));

		return modelTransform.to_mat4();
	}
}