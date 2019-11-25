#include "hellpch.h"
#include "Ceiling.h"
#include "HellEngine/AssetManager.h"

namespace HellEngine
{
	Ceiling::Ceiling()
	{
	}

	Ceiling::Ceiling(glm::vec3 position, float width, float depth, unsigned int materialID, bool rotateTexture)
	{
		this->materialID = materialID;
		transform.position = position;
		transform.position.y = 2.4f;
		transform.position.z += depth;
		this->width = width;
		this->depth = depth;
		this->rotateTexture = rotateTexture;
		transform.scale = glm::vec3(width, -1, -depth);
		this->model = AssetManager::GetModelByName("UnitPlane.obj");

		glm::vec3 A = transform.position;
		glm::vec3 B = glm::vec3(transform.position.x + width, transform.position.y, transform.position.z);
		glm::vec3 C = glm::vec3(transform.position.x + width, transform.position.y, transform.position.z - depth);
		glm::vec3 D = glm::vec3(transform.position.x, transform.position.y, transform.position.z - depth);
		this->plane = BoundingPlane(A, B, C, D, false, "Ceiling");
	}

	void Ceiling::Draw(Shader *shader, bool bindTextures)
	{
		if (bindTextures)
		{
			if (!rotateTexture)
				shader->setInt("TEXTURE_FLAG", 3);
			if (rotateTexture)
				shader->setInt("TEXTURE_FLAG", 4);
			AssetManager::BindMaterial(materialID);
			AssetManager::BindMaterial(AssetManager::GetMaterialIDByName("FloorBoards"));
		}
		shader->setMat4("model", transform.to_mat4());

		model->Draw(shader);

		if (bindTextures)
			shader->setInt("TEXTURE_FLAG", 0);
	}

	Ceiling::~Ceiling()
	{
	}
}