#pragma once
#include "Platform/OpenGL/RenderingPrimitive.h"
#include "HellEngine/Material.h"
#include "HellEngine/Model.h"
#include "Platform/OpenGL/BoundingPlane.h"

namespace HellEngine
{
	class Ceiling : public RenderingPrimitive
	{
	public: // methods
		Ceiling();
		~Ceiling();
		Ceiling(glm::vec3 position, float width, float depth, Material* material, bool rotateTexture);
		void Draw(Shader *shader, bool bindTextures) override;
		Model* model;
		Material* material;
		bool rotateTexture = false;

	public: // fields
		unsigned int VAO;
		float width;
		float depth;
		BoundingPlane plane;
	};
}