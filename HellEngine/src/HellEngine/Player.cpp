#include "hellpch.h"
#include "Player.h"
#include "HellEngine/Input.h"
#include "HellEngine/Util.h"

namespace HellEngine
{
	/*float MovementSpeed = 2.25f;
	float viewHeight = 1.6f;
	float headBobCounter = 0.0f;
	float headBobSpeed = 0.3f;
	float headBobFactor = 0.0075f;*/
	
	const float COLLISION_RESPONSE_AMOUNT = 0.001f;

	Player::Player()
	{
	}


	Player::~Player()
	{
	}

	//https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection

	void Player::ProcessCollisions(House* house)
	{
		
		float w = 0.5f; // Half the walls width
		float collisionResponseAmount = 0.0001f;

		return;

/*		for (Wall & wall : house->walls)
		{
			// Z facing
			if (wall.axis == Axis::Z)
			{
				// Too far left or right, or behind the wall
				if ((position.x + radius < wall.position.x - w) || (position.x - radius > wall.position.x + w) || (position.z + radius < wall.position.z))
					continue;

				// Collision
				while (position.z - radius < wall.position.z)
					position += wall.normal * collisionResponseAmount;
			}
			// -Z facing
			if (wall.axis == Axis::Z_NEGATIVE)
			{
				// Too far left or right, or behind the wall
				if ((position.x + radius < wall.position.x - w) || (position.x - radius > wall.position.x + w) || (position.z - radius > wall.position.z))
					continue;
				// Collision
				while (position.z + radius > wall.position.z)
					position += wall.normal * collisionResponseAmount;
			}
			// X facing
			if (wall.axis == Axis::X)
			{
				// Too far left or right, or behind the wall
				if ((position.z + radius < wall.position.z - w) || (position.z - radius > wall.position.z + w) || (position.x + radius < wall.position.x))
					continue;
				// Collision
				while (position.x - radius < wall.position.x)
					position += wall.normal * collisionResponseAmount;
			}
			// -X facing
			if (wall.axis == Axis::X_NEGATIVE)
			{
				//Too far left or right, or behind the wall
				if ((position.z + radius < wall.position.z - w) || (position.z - radius > wall.position.z + w) || (position.x - radius > wall.position.x))
					continue;
				// Collision
				while (position.x + radius > wall.position.x)
					position += wall.normal * collisionResponseAmount;;
			}
		}*/
	}

	void Player::HandleBoundingBoxCollisions(vector<BoundingBox*> boundingBoxPtrs)
	{
		for (BoundingBox* & b : boundingBoxPtrs)
			HandleBoundingBoxCollision(b);
	}

	void Player::HandleBoundingPlaneCollisions(vector<BoundingPlane*> boundingPlanePtrs)
	{
		for (BoundingPlane* & plane : boundingPlanePtrs)
			while (Util::CircleIntersectsLine(position, plane->A, plane->C, radius))
				position += plane->normal * COLLISION_RESPONSE_AMOUNT;
	}
	
	void Player::HandleBoundingBoxCollision(BoundingBox* box)
	{
		bool checkFront = true;
		bool checkBack = true;
		bool checkLeft = true;
		bool checkRight = true;

		while (checkFront || checkBack || checkLeft || checkRight)
		{
			checkFront = Util::CircleIntersectsLine(position, box->frontPlane.A, box->frontPlane.C, radius);
			if (checkFront)
				position += box->frontPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkBack = Util::CircleIntersectsLine(position, box->backPlane.A, box->backPlane.C, radius);
			if (checkBack)
				position += box->backPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkLeft = Util::CircleIntersectsLine(position, box->leftPlane.A, box->leftPlane.C, radius);
			if (checkLeft)
					position += box->leftPlane.normal * COLLISION_RESPONSE_AMOUNT;

			checkRight = Util::CircleIntersectsLine(position, box->rightPlane.A, box->rightPlane.C, radius);
			if (checkRight)
				position += box->rightPlane.normal * COLLISION_RESPONSE_AMOUNT;
		}
	}

	/*void Player::ProcessCollisions(BoundingPlane* plane)
	{
		float collisionResponseAmount = 0.001f;

			while (Util::CircleIntersectsLine(position, plane->A, plane->C, radius))
				position += plane->normal * collisionResponseAmount;
	}*/
}