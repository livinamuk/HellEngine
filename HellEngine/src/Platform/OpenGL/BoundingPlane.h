#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"

namespace HellEngine {

	class BoundingPlane
	{
	public: // methods
		BoundingPlane();
		BoundingPlane(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, bool testCollisions, std::string name);	// A & B are on top.  
		~BoundingPlane();
		void Draw(Shader* shader);
		void DrawSolid(Shader* shader);
		void BufferData();

	public: // fields
		glm::vec3 position;
		glm::vec3 A, B, C, D; // corners
		glm::vec3 scale;
		unsigned int VAO = 0;
		bool testCollisions = true;
		std::string name = "None";

	};
}

