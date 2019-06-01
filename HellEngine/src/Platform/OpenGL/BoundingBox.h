#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"
#include "Platform/OpenGL/BoundingPlane.h"

namespace HellEngine {

	class BoundingBox
	{
	public: // methods
		BoundingBox(glm::vec3 position, glm::vec3 size, ObjectOrigin objectOrigin);
		BoundingBox();
		~BoundingBox();
		void Draw(Shader *shader);
		void SetAngle(float angle); 
		void SetupPlanes();
		void CalculateCorners();
		void Update();

	public: // fields
		glm::vec3 position;
		glm::vec3 leftTopFront, rightTopFront, leftBottomFront, rightBottomFront;
		glm::vec3 leftTopBack, rightTopBack, leftBottomBack, rightBottomBack;
		BoundingPlane frontPlane, backPlane, leftPlane, rightPlane; 
		ObjectOrigin objectOrigin;
		float width, height, depth;

	private: // fields
		float angle;
	};
}

