#include "hellpch.h"
#include "StaticEntity.h"


namespace HellEngine 
{
	StaticEntity::StaticEntity(Model* model, glm::vec3 position, std::string name)
	{
		this->model = model;
		this->name = name;
		this->transform.position = position;

		HELL_ERROR(name + " Constructor");
		CreateRigidBody();
	}

	StaticEntity::~StaticEntity()
	{
		for (int i = 0; i < Physics::m_rigidBodies.size(); i++)
		{
			if (Physics::m_rigidBodies[i] == this->rigidBody)
				Physics::m_rigidBodies.removeAtIndex(i);
		}

		Physics::m_dynamicsWorld->removeRigidBody(rigidBody);
		//HELL_ERROR(name + " Destructor");
	}

	void StaticEntity::SetPosition(glm::vec3 position)
	{
		transform.position = position;
		UpdateRigidBodyTransform();
	}

	void StaticEntity::SetRotation(glm::vec3 rotation)
	{
		transform.rotation = rotation;
		UpdateRigidBodyTransform();
	}

	void StaticEntity::SetScale(glm::vec3 scale)
	{
		transform.scale = scale;
		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		glm::vec3 newScale = transform.scale * model->meshes[0].boundingBox.baseTransform.scale;
		collisionShape->setLocalScaling(Util::glmVec3_to_btVec3(newScale));	
		rigidBody->setCollisionShape(collisionShape);
	}

	void StaticEntity::CreateRigidBody()
	{
		// Setup
		BoundingBox boundingBox = model->meshes[0].boundingBox;
		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(Util::glmVec3_to_btVec3(boundingBox.baseTransform.scale));

		// Position
		btTransform btTransform;
		btTransform.setIdentity();

		// Rotation
		btTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
		btTransform.setRotation(btQuaternion(transform.rotation.y, transform.rotation.x, transform.rotation.z));
		
		// Create it
		rigidBody = Physics::createRigidBody(0.0, btTransform, collisionShape, 1.0);
		Physics::m_rigidBodies.push_back(rigidBody);

		// Add entity Data
		EntityData* entityData = new EntityData();
		entityData->name = name;
		rigidBody->setUserPointer(entityData);

		// Kinematic flags
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}

void StaticEntity::UpdateRigidBodyTransform()
{
	// Get final transformation matrix
	BoundingBox* boundingBox = &model->meshes[0].boundingBox;
	glm::mat4 matrix = transform.to_mat4()* boundingBox->baseTransform.to_mat4();

	// Strip the rotation
	matrix[0][0] = 0;
	matrix[0][1] = 0;
	matrix[0][2] = 0;
	matrix[1][0] = 0;
	matrix[1][1] = 0;
	matrix[1][2] = 0;
	matrix[2][0] = 0;
	matrix[2][1] = 0;
	matrix[2][2] = 0;

	// NOW reapply it (so after the previous transform multiplication, and not before, get it? u better)
	btTransform test;
	test.setFromOpenGLMatrix(glm::value_ptr(matrix));
	glm::quat quat(glm::vec3(transform.rotation.x, transform.rotation.y, transform.rotation.z)); //where y,x,z are euler angles ,
	btQuaternion btQuat = btQuaternion(quat.x, quat.y, quat.z, quat.w);
	test.setRotation(btQuat);

	// Set the fucking thing
	rigidBody->setWorldTransform(test);
}


	/*void StaticEntity::UpdateWorldTransform()
	{
		// Think about what you are doing here:
		// this is updating the 'Transform transform' based off the Bullet Physics position, but this never changes...
		// it might be smarter to do it the other way around. cause you are only ever gonna wanna change the Transform transform
		// and update the physics engine when you do most likely. But leave for now cause u did it backwards first and it won't affect nothign.

		// Position
		float xPos = (float)rigidBody->getCenterOfMassPosition().x();
		float yPos = (float)rigidBody->getCenterOfMassPosition().y();
		float zPos = (float)rigidBody->getCenterOfMassPosition().z();
		transform.position = glm::vec3(xPos, yPos, zPos);

		// Rotation
		btScalar x, y, z;
		rigidBody->getWorldTransform().getRotation().getEulerZYX(x, y, z);
		transform.rotation = glm::vec3(z, y, x);
	}*/

	void StaticEntity::DrawBoundingBox(Shader* shader)
	{
		BoundingBox* boundingBox = &model->meshes[0].boundingBox;
		shader->setMat4("model", transform.to_mat4()  * boundingBox->baseTransform.to_mat4());
		boundingBox->Draw(shader);
	}

	void StaticEntity::Draw(Shader* shader, bool bindTextures)
	{
		UpdateRigidBodyTransform();

		if (bindTextures) 
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(material->baseColor));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(material->roughness));
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(material->metallic));
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(material->normalMap));
		
			// Emissive map
			//if (material.hasEmissiveMap)
			//{
				//shader->setInt("hasEmissiveMap", 1.0f);
				//shader->setVec3("emissiveColor", emissiveColor);
				//glActiveTexture(GL_TEXTURE4);
				//glBindTexture(GL_TEXTURE_2D, Texture::GetIDByName(material->emi));
			//}
		}
		shader->setMat4("model", transform.to_mat4());
		model->Draw(shader);
	}
}
