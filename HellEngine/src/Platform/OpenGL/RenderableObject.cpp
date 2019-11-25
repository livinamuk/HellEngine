#include "hellpch.h"
#include "HellEngine/Util.h"
#include "RenderableObject.h"

namespace HellEngine
{
	std::vector<RenderableObject> RenderableObject::renderableObjects;
	Physics* RenderableObject::physics;

	HellEngine::RenderableObject::RenderableObject()
	{
	}

	HellEngine::RenderableObject::RenderableObject(std::string name, Model* model, Physics* physics)
	{
		this->name = name;
		this->model = model;
		this->transform.scale = glm::vec3(1);
		this->transform.rotation = glm::vec3(0);
		this->diffuseTextureID = -1;
		this->metallicTextureID = -1;
		this->roughnessTextureID = -1;
		this->normalMapID = -1;


		if (physics != NULL)
			CreateRigidBodyFromModel(physics);
	}

	RenderableObject::~RenderableObject()
	{
	}

	void RenderableObject::NewObject(std::string name, Model* model, Physics* physics)
	{
		renderableObjects.push_back(RenderableObject(name, model, physics));
	}

/*	void RenderableObject::SetModelByName(std::string name, Model* model, Physics* phyics)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.model = model;

				
	}*/

	void RenderableObject::SetPositionByName(std::string name, glm::vec3 position)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.transform.position = position;
	}

	void RenderableObject::SetScaleByName(std::string name, glm::vec3 scale)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.transform.scale = scale;
	}
	/*
	void RenderableObject::SetRotateAngleByName(std::string name, glm::vec3 rotateAngle)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.rotateAngle = rotateAngle;
	}

	*/void RenderableObject::SetModelYRotationByName(std::string name, float angle)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.modelTransform.rotation.y = angle;
	}



	/*void RenderableObject::SetAngleByName(std::string name, float angle)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.angle = angle;
	}*/

	void RenderableObject::SetRotationByName(std::string name, glm::vec3 rotation)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.transform.rotation = rotation;
	}

	void RenderableObject::SetDiffuseTextureByName(std::string name, int textureID)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.diffuseTextureID = textureID;
	}

	void RenderableObject::SetRoughnessTextureByName(std::string name, int textureID)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.roughnessTextureID = textureID;
	}

	void RenderableObject::SetMetallicTextureByName(std::string name, int textureID)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.metallicTextureID = textureID;
	}

	void RenderableObject::SetEmissiveMapByName(std::string name, int textureID)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name) {
				r.emissiveMapID = textureID;
				r.hasEmissiveMap = true;
			}
	}

	void RenderableObject::SetNormalMapByName(std::string name, int textureID)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				r.normalMapID = textureID;
	}

	void RenderableObject::SetBoundingBoxByName(std::string name, BoundingBox boundingBox)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name) {
				r.hasCollision = true;
				r.boundingBox = boundingBox;
			}
	}

	HellEngine::RenderableObject* RenderableObject::GetPointerToRenderableObjectByName(std::string name)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				return &r;

		return NULL;
	}

	void RenderableObject::DrawAll(Shader* shader, bool bindTextures)
	{
		for (RenderableObject& r : renderableObjects)
			r.Draw(shader, bindTextures);
	}

	glm::vec3 RenderableObject::GetPositionByName(std::string name)
	{
		for (RenderableObject& r : renderableObjects)
			if (r.name == name)
				return r.transform.position;

		return glm::vec3();
	}

	void RenderableObject::DrawBoundingBox(Shader* shader)
	{
		return;

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
		BoundingBox* boundingBox = &model->meshes[0].boundingBox;
		boundingBox->baseTransform.position = glm::vec3(0);

		shader->setMat4("model", modelTransform.to_mat4() * boundingBox->baseTransform.to_mat4());
		boundingBox->Draw(shader);


		//BoundingBox* boundingBox = &model->meshes[0].boundingBox;
		//shader->setMat4("model", transform.to_mat4() * boundingBox->baseTransform.to_mat4());
		//boundingBox->Draw(shader);
	}

	void RenderableObject::CreateRigidBodyFromModel(Physics* physics)
	{
		/*BoundingBox boundingBox = model->meshes[0].boundingBox;
		btVector3 collisionScale = Util::glmVec3_to_btVec3(boundingBox.baseTransform.scale);

		btBoxShape* collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
		collisionShape->setLocalScaling(collisionScale);

		// Position
		btTransform btTransform;
		btTransform.setIdentity();

		float x = transform.position.x;
		float y = transform.position.y + 1;
		float z = transform.position.z;

		float rotX = transform.rotation.x;
		float rotY = transform.rotation.y;
		float rotZ = transform.rotation.z;


		btTransform.setOrigin(btVector3(x, y, z));
		btTransform.setRotation(btQuaternion(rotX, rotY, rotZ));
		rigidBody = physics->createRigidBody(0.0, btTransform, collisionShape, 1.0);
		rigidBody->setUserIndex(RigidBodyType::NOTE);
		//rigidBody->setUserIndex2(i);
		physics->m_rigidBodies.push_back(rigidBody);*/
	}

	void RenderableObject::UpdateRigidBodyWorldTransform()
	{
		return;
		if (rigidBody == NULL)
			return;

		BoundingBox* boundingBox = &model->meshes[0].boundingBox;

		// Position
		btTransform worldTransform;
		worldTransform.setIdentity();
		worldTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
		worldTransform.setRotation(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z));


		btTransform newMat = worldTransform;// *translateXMat* rotateYMat* translateXMat2;
		rigidBody->setWorldTransform(newMat);
	}

	void RenderableObject::Draw(Shader* shader, bool bindTextures)
	{
		//UpdateRigidBodyWorldTransform();

		// Has this model been loaded even?
		if (model == nullptr) {
			HELL_ASSERT(false, "Renderable object '{s}' has a nullptr model", name);
			return;
		}

		if (bindTextures) {

			shader->setVec3("emissiveColor", emissiveColor);
		}


		shader->setMat4("model", transform.to_mat4() * modelTransform.to_mat4());

		model->Draw(shader);
		shader->setInt("PBR", 0.0f);
		shader->setInt("hasNormalMap", 0.0f);
		shader->setInt("hasEmissiveMap", 0.0f);
	}
}