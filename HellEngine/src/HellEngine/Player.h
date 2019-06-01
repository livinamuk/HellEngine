#pragma once
#include "HellEngine/Components/House.h"
#include "Platform/OpenGL/BoundingPlane.h"
#include "Platform/OpenGL/BoundingBox.h"

namespace HellEngine 
{
	class Player
	{
	public: // methods
		Player();
		~Player();
		void ProcessCollisions(House* house);
		void HandleBoundingBoxCollisions(vector<BoundingBox*> boundingBoxes);
		void HandleBoundingPlaneCollisions(vector<BoundingPlane*> boundingPlanePtrs);
		void HandleBoundingBoxCollision(BoundingBox* box);

	public: // fields
		glm::vec3 position;
		float radius = 0.1f;
	};
}