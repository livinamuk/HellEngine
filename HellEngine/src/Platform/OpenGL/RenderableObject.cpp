#include "hellpch.h"
#include "RenderableObject.h"

namespace HellEngine
{
	std::vector<RenderableObject> RenderableObject::renderableObjects;


	HellEngine::RenderableObject::RenderableObject(Model* model, glm::vec3 position)
	{
		this->model = model;
		this->transform.position = position;
	}

	HellEngine::RenderableObject::RenderableObject()
	{
	}

	HellEngine::RenderableObject::RenderableObject(Model* model, std::string texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		this->model = model;
		this->transform.position = position;
		this->transform.rotation = rotation;
		this->transform.scale = scale;

		this->diffuseTextureID = Texture::GetIDByName(texture);
		this->metallicTextureID = -1;
		this->roughnessTextureID = -1;
		this->normalMapID = -1;
	}

	HellEngine::RenderableObject::RenderableObject(std::string name, Model *model)
	{
		this->name = name;
		this->model = model;
		this->transform.scale = glm::vec3(1);
		this->transform.rotation = glm::vec3(0);
		this->diffuseTextureID = -1;
		this->metallicTextureID = -1;
		this->roughnessTextureID = -1;
		this->normalMapID = -1;
	}

	RenderableObject::~RenderableObject()
	{
	}

	void RenderableObject::NewObject(std::string name, Model *model)
	{
		renderableObjects.push_back(RenderableObject(name, model));
	}

	void RenderableObject::SetModelByName(std::string name, Model *model)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name) 
				r.model = model;

		
	}

	void RenderableObject::SetPositionByName(std::string name, glm::vec3 position)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.transform.position = position;
	}

	void RenderableObject::SetScaleByName(std::string name, glm::vec3 scale)
	{
		for (RenderableObject & r : renderableObjects)
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
		for (RenderableObject & r : renderableObjects)
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
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.diffuseTextureID = textureID;
	}

	void RenderableObject::SetRoughnessTextureByName(std::string name, int textureID)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.roughnessTextureID = textureID;
	}

	void RenderableObject::SetMetallicTextureByName(std::string name, int textureID)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.metallicTextureID = textureID;
	}

	void RenderableObject::SetEmissiveMapByName(std::string name, int textureID)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name) {
				r.emissiveMapID = textureID;
				r.hasEmissiveMap = true;
			}
	}

	void RenderableObject::SetNormalMapByName(std::string name, int textureID)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.normalMapID = textureID;
	}

	void RenderableObject::SetBoundingBoxByName(std::string name, BoundingBox boundingBox)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name) {
				r.hasCollision = true;
				r.boundingBox = boundingBox;
			}
	}

	HellEngine::RenderableObject* RenderableObject::GetPointerToRenderableObjectByName(std::string name)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				return &r;

		return NULL;
	}

	void RenderableObject::DrawAll(Shader *shader, bool bindTextures)
	{
		for (RenderableObject & r : renderableObjects)
			r.Draw(shader, bindTextures);
	}

	glm::vec3 RenderableObject::GetPositionByName(std::string name)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				return r.transform.position;
		
		return glm::vec3();
	}

	void RenderableObject::Draw(Shader *shader, bool bindTextures)
	{
		// Has this model been loaded even?
		if (model == nullptr) {
			HELL_ASSERT(false, "Renderable object '{s}' has a nullptr model", name);
			return;
		}

		if (bindTextures) {

			shader->setInt("PBR", 1.0f);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTextureID);

			if (roughnessTextureID == -1)
				shader->setInt("PBR", 0.0f);
			else {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, roughnessTextureID);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, metallicTextureID);
			}

			// Normal mapping
			if (normalMapID != -1) {
				shader->setInt("hasNormalMap", 1.0f);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, normalMapID);
			}

			// Emissive map
			if (hasEmissiveMap)
			{
				shader->setInt("hasEmissiveMap", 1.0f);
				shader->setVec3("emissiveColor", emissiveColor);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, emissiveMapID);
			}
		}

		shader->setMat4("model", transform.to_mat4() * modelTransform.to_mat4());
		
		model->Draw(shader);
		shader->setInt("PBR", 0.0f);
		shader->setInt("hasNormalMap", 0.0f);
		shader->setInt("hasEmissiveMap", 0.0f);
	}
}