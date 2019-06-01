#include "hellpch.h"
#include "BoundingBox.h"

namespace HellEngine {

	BoundingBox::BoundingBox(glm::vec3 position, glm::vec3 size, ObjectOrigin objectOrigin)
	{
		this->position = position;
		this->objectOrigin = objectOrigin;
		this->angle = 0;
		this->width = size.x;
		this->height = size.y;
		this->depth = size.z;
		CalculateCorners();
		SetupPlanes();
	}

	BoundingBox::BoundingBox()
	{
	}

	BoundingBox::~BoundingBox()
	{
	}

	void BoundingBox::CalculateCorners()
	{
		if (objectOrigin == BOTTOM_CENTERED)
		{
			leftBottomFront = glm::vec3(-width / 2, 0, depth / 2);
			rightBottomFront = glm::vec3(width / 2, 0, depth / 2);
			leftTopFront = glm::vec3(-width / 2, height, depth / 2);
			rightTopFront = glm::vec3(width / 2, height, depth / 2);
			leftBottomBack = glm::vec3(-width / 2, 0, -depth / 2);
			rightBottomBack = glm::vec3(width / 2, 0, -depth / 2);
			leftTopBack = glm::vec3(-width / 2, height, -depth / 2);
			rightTopBack = glm::vec3(width / 2, height, -depth / 2);
		}
		else if (objectOrigin == BOTTOM_LEFT_CENTERED)
		{
			leftBottomFront = glm::vec3(0, 0, depth / 2);
			rightBottomFront = glm::vec3(width, 0, depth / 2);
			leftTopFront = glm::vec3(0, height, depth / 2);
			rightTopFront = glm::vec3(width, height, depth / 2);
			leftBottomBack = glm::vec3(0, 0, -depth / 2);
			rightBottomBack = glm::vec3(width, 0, -depth / 2);
			leftTopBack = glm::vec3(0, height, -depth / 2);
			rightTopBack = glm::vec3(width, height, -depth / 2);
		}
		else if (objectOrigin == BOTTOM_LEFT_FRONT_CORNER)
		{
			leftBottomFront = glm::vec3(0, 0, 0);
			rightBottomFront = glm::vec3(width, 0, 0);
			leftTopFront = glm::vec3(0, height, 0);
			rightTopFront = glm::vec3(width, height, 0);
			leftBottomBack = glm::vec3(0, 0, -depth);
			rightBottomBack = glm::vec3(width, 0, -depth);
			leftTopBack = glm::vec3(0, height, -depth);
			rightTopBack = glm::vec3(width, height, -depth);
		}
		else if (objectOrigin == BOTTOM_LEFT_BACK_CORNER)
		{
			leftBottomFront = glm::vec3(0, 0, depth);
			rightBottomFront = glm::vec3(width, 0, depth);
			leftTopFront = glm::vec3(0, height, depth);
			rightTopFront = glm::vec3(width, height, depth);
			leftBottomBack = glm::vec3(0, 0, 0);
			rightBottomBack = glm::vec3(width, 0, 0);
			leftTopBack = glm::vec3(0, height, 0);
			rightTopBack = glm::vec3(width, height, 0);
		}

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, position);
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		leftBottomFront = glm::vec3(model * glm::vec4(leftBottomFront, 1));
		rightBottomFront = glm::vec3(model * glm::vec4(rightBottomFront, 1));
		leftTopFront = glm::vec3(model * glm::vec4(leftTopFront, 1));
		rightTopFront = glm::vec3(model * glm::vec4(rightTopFront, 1));
		leftBottomBack = glm::vec3(model * glm::vec4(leftBottomBack, 1));
		rightBottomBack = glm::vec3(model * glm::vec4(rightBottomBack, 1));
		leftTopBack = glm::vec3(model * glm::vec4(leftTopBack, 1));
		rightTopBack = glm::vec3(model * glm::vec4(rightTopBack, 1));
	}

	void BoundingBox::Draw(Shader *shader)
	{
		frontPlane.Draw(shader);
		backPlane.Draw(shader);
		leftPlane.Draw(shader);
		rightPlane.Draw(shader);
	}

	void BoundingBox::SetupPlanes()
	{
		frontPlane = BoundingPlane(leftTopFront, rightTopFront, rightBottomFront, leftBottomFront);
		backPlane = BoundingPlane(rightTopBack, leftTopBack, leftBottomBack, rightBottomBack);
		leftPlane = BoundingPlane(leftTopBack, leftTopFront, leftBottomFront, leftBottomBack);
		rightPlane = BoundingPlane(rightTopFront, rightTopBack, rightBottomBack, rightBottomFront);
	}

	void BoundingBox::SetAngle(float angle)
	{
		this->angle = angle; 
		CalculateCorners();
		SetupPlanes();
	}
}