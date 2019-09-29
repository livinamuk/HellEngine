#pragma once

#include "Glad/glad.h"
#include "GLFW/glfw3.h"
#include "Platform/OpenGL/Shader.h"
#include "Platform/OpenGL/BoundingBox.h"
#include "HellEngine/Model.h"
#include "HellEngine/Transform.h"

namespace HellEngine
{
	class RenderableObject
	{
	public: // methods
		RenderableObject(Model* model, std::string texture, glm::vec3 position, glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1));
		RenderableObject(std::string name, Model* model);
		RenderableObject(Model* model, glm::vec3 position);
		RenderableObject();
		~RenderableObject();
		void Draw(Shader *shader, bool bindTextures);

	public: // fields
		std::string name;

		Transform transform;
		Transform modelTransform;

		glm::vec3 emissiveColor = glm::vec3(1);
		float angle;
		//glm::vec3 rotateAngle = glm::vec3(0.0f, 1.0f, 0.0f);
		Model* model = nullptr;
		bool hasCollision = false;
		bool hasEmissiveMap = false;
		BoundingBox boundingBox;

		unsigned int diffuseTextureID;
		unsigned int roughnessTextureID;
		unsigned int metallicTextureID;
		unsigned int normalMapID;
		unsigned int emissiveMapID;

		static std::vector<RenderableObject> RenderableObject::renderableObjects;
		static void RenderableObject::NewObject(std::string name, Model *model);
		static void SetModelByName(std::string name, Model *model);
		static void SetPositionByName(std::string name, glm::vec3 position);
		static void SetScaleByName(std::string name, glm::vec3 scale);
		//static void SetRotateAngleByName(std::string name, glm::vec3 scale);
		//static void SetAngleByName(std::string name, float angle);

		static void SetModelYRotationByName(std::string name, float angle);
		
		static void SetRotationByName(std::string name, glm::vec3 rotation);		
		static void SetDiffuseTextureByName(std::string name, int textureID);
		static void SetRoughnessTextureByName(std::string name, int textureID);
		static void SetMetallicTextureByName(std::string name, int textureID);
		static void SetEmissiveMapByName(std::string name, int textureID);
		static void SetNormalMapByName(std::string name, int textureID);
		static void SetBoundingBoxByName(std::string name, BoundingBox boundingBox);
		static RenderableObject* GetPointerToRenderableObjectByName(std::string name);

		static void RenderableObject::DrawAll(Shader *shader, bool bindTextures);


		static glm::vec3 GetPositionByName(std::string name);
	};
}