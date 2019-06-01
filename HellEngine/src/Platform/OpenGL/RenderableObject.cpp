#include "hellpch.h"
#include "RenderableObject.h"

namespace HellEngine
{
	std::vector<RenderableObject> RenderableObject::renderableObjects;

	HellEngine::RenderableObject::RenderableObject(Model* model, std::string texture, glm::vec3 position, float angle, glm::vec3 scale)
	{
		this->model = model;
		this->position = position;
		this->angle = angle;
		this->scale = scale;

		this->diffuseTextureID = Texture::GetIDByName(texture);
		this->metallicTextureID = -1;
		this->roughnessTextureID = -1;
		this->normalMapID = -1;
	}

	HellEngine::RenderableObject::RenderableObject(std::string name, Model *model)
	{
		this->name = name;
		this->model = model;
		this->scale = glm::vec3(1);
		this->angle = 0;
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
				r.position = position;
	}

	void RenderableObject::SetScaleByName(std::string name, glm::vec3 scale)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.scale = scale;
	}

	void RenderableObject::SetRotateAngleByName(std::string name, glm::vec3 rotateAngle)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.rotateAngle = rotateAngle;
	}
	
	void RenderableObject::SetAngleByName(std::string name, float angle)
	{
		for (RenderableObject & r : renderableObjects)
			if (r.name == name)
				r.angle = angle;
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
				return r.position;
		
		return glm::vec3();
	}

	void RenderableObject::Draw(Shader *shader, bool bindTextures)
	{
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
			else
				shader->setInt("hasNormalMap", 0.0f);
		}

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::rotate(modelMatrix, angle, rotateAngle);
		modelMatrix = glm::scale(modelMatrix, scale);
		shader->setMat4("model", modelMatrix);
		
		model->Draw(shader);
		shader->setInt("PBR", 0.0f);
		shader->setInt("hasNormalMap", 0.0f);
	}
}