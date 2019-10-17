#pragma once

#include "Platform/OpenGL/RenderingPrimitive.h"
#include "Platform/OpenGL/BoundingPlane.h"

namespace HellEngine {

	class BoundingBox
	{
	public: // methods
		BoundingBox(glm::vec3 position, glm::vec3 size, ObjectOrigin objectOrigin, std::string name, bool testCollisions);
		BoundingBox();
		BoundingBox(glm::vec3 lowestPositionValues, glm::vec3 highestPositionValues);
		
		Transform baseTransform;
		
		~BoundingBox();
		void Draw(Shader *shader);
		//void SetAngle(float angle); 
		void SetupPlanes();
		//void CalculateCorners();
		//void Update();

	public: // fields
		glm::vec3 position;
		glm::vec3 leftTopFront, rightTopFront, leftBottomFront, rightBottomFront;
		glm::vec3 leftTopBack, rightTopBack, leftBottomBack, rightBottomBack;
		BoundingPlane frontPlane, backPlane, leftPlane, rightPlane, topPlane; 
		ObjectOrigin objectOrigin;
		bool TestCollisions = true;
		float width, height, depth;
		std::string name;

	private: // fields
		//float angle;
	};
}

